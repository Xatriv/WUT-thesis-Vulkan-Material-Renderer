#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

/// #define USE_TEXTURES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <limits>

#include "app.h"


const uint32_t WIDTH = 1280;
const uint32_t HEIGHT = 720;

const std::string SPHERE_MODEL_PATH = "../models/sphere.obj";
const std::string MODEL_PATH = "../models/skull.obj";

const std::string VERT_SHADER_PATH = "../shaders/phong_shader.vert.spv";
const std::string FRAG_SHADER_PATH = "../shaders/phong_shader.frag.spv";
const std::string LIGHT_VERT_SHADER_PATH = "../shaders/light_shader.vert.spv";
const std::string LIGHT_FRAG_SHADER_PATH = "../shaders/light_shader.frag.spv";



#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


namespace vmr{

void App::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void App::initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //don't create opengl context
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //handling this requires special care

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Material Renderer", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}
void App::initVulkan() {

    device = new Device(window);

    swapChain = new SwapChain(device, window);
    createRenderPass();
    modelPipeline = new Pipeline(device, swapChain, VERT_SHADER_PATH, FRAG_SHADER_PATH, MODEL_PATH, true, &lightPosition, &observerPosition);
    lightPipeline = new Pipeline(device, swapChain, LIGHT_VERT_SHADER_PATH, LIGHT_FRAG_SHADER_PATH, SPHERE_MODEL_PATH, false, &lightPosition, &observerPosition);
    createCommandPool();
    swapChain->createDepthResources();
    swapChain->createFramebuffers();
    #ifdef USE_TEXTURES
        swapChain->createTextureImage();
        swapChain->createTextureImageView();
        swapChain->createTextureSampler();
    #endif
    modelPipeline->prepareModel();
    lightPipeline->prepareModel();
    createCommandBuffers();
    createSyncObjects();
}

void App::handleKeystrokes(){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) movementMode = MOVEMENT_CAMERA;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) movementMode = MOVEMENT_LIGHT;
    if (movementMode == MOVEMENT_LIGHT){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) lightPosition.x += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) lightPosition.x += -0.01f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) lightPosition.y += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) lightPosition.y += -0.01f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) lightPosition.z += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) lightPosition.z += -0.01f;
    } else if (movementMode == MOVEMENT_CAMERA) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) observerPosition.y += -0.01f;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) observerPosition.y += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) observerPosition.x += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) observerPosition.x += -0.01f;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) observerPosition.z += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) observerPosition.z += -0.01f;
    }
}

void App::mainLoop() { //render frames
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        handleKeystrokes();
        drawFrame();
    }

    vkDeviceWaitIdle(device->logical());
}

void App::cleanup() {
    delete swapChain;

    vkDestroyRenderPass(device->logical(), swapChain->renderPass(), nullptr);

    delete modelPipeline;
    delete lightPipeline;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device->logical(), renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device->logical(), imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device->logical(), inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(device->logical(), device->commandPool(), nullptr);

    delete device;


    glfwDestroyWindow(window);

    glfwTerminate();
}

void App::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChain->imageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear the values to a constant at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = device->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    //render pass may have multiple subpasses (like postprocesses that ought to be grouped together)
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // VK_SUBPASS_EXTERNAL refers to the implicit subpass before or after the render pass depending on whether it is specified in srcSubpass or dstSubpass
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device->logical(), &renderPassInfo, nullptr, &swapChain->renderPass()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void App::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = device->findQueueFamilies(device->physical());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device->logical(), &poolInfo, nullptr, &(device->commandPool())) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void App::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device->commandPool();
    //Can be submitted to a queue for execution, but cannot be called from other command buffers.
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device->logical(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->renderPass();
    renderPassInfo.framebuffer = swapChain->framebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->extent();

    //color of pixels within render are with undefined values
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, modelPipeline->pipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->extent().width);
    viewport.height = static_cast<float>(swapChain->extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain->extent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    modelPipeline->bind(commandBuffer, currentFrame);

    //-----light-------
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, lightPipeline->pipeline());

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    lightPipeline->bind(commandBuffer, currentFrame);
  
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::drawFrame() {
    vkWaitForFences(device->logical(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device->logical(), swapChain->swapChain(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain->recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    modelPipeline->updateUniformBuffer(currentFrame);
    lightPipeline->updateUniformBuffer(currentFrame);

    // Only reset the fence if we are submitting work
    vkResetFences(device->logical(), 1, &inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame],  0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device->graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain->swapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(device->presentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        framebufferResized = false;
        swapChain->recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    if (movementMode == MOVEMENT_CAMERA){
        std::cout<<"CAM "<<observerPosition.x<<";"<<observerPosition.y<<";"<<observerPosition.z<<"\n";
    } else if (movementMode == MOVEMENT_LIGHT){
        std::cout<<"SRC "<<lightPosition.x<<";"<<lightPosition.y<<";"<<lightPosition.z<<"\n";
    }
}

void App::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device->logical(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device->logical(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device->logical(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}


void App::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

}
