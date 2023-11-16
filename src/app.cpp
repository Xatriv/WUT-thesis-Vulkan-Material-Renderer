#include "app.h"
#include "app_config.h"


#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif


namespace vmr{

App::App(AppConfig* config) : _appConfig(config) { }

void App::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void App::initWindow(){
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //don't create opengl context

    _window = glfwCreateWindow(_appConfig->windowWidth(), _appConfig->windowHeight(), "Vulkan Material Renderer", nullptr, nullptr);
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(_window, mouseMovementCallback);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}
void App::initVulkan() {

    _device = new Device(_window);

    _swapChain = new SwapChain(_device, _window, _appConfig);
    createRenderPass();
    _modelPipeline = new Pipeline(_device, _swapChain, _appConfig, _appConfig->modelVertexShaderPath(), _appConfig->modelFragmentShaderPath(), _appConfig->displayModelPath(),  true);
    _lightPipeline = new Pipeline(_device, _swapChain, _appConfig, _appConfig->lightVertexShaderPath(), _appConfig->lightFragmentShaderPath(), _appConfig->sphereModelPath(), false);
    createCommandPool();
    _swapChain->createDepthResources();
    _swapChain->createFramebuffers();
    _swapChain->createTextureImages();
    _swapChain->createTextureImageViews();
    _swapChain->createTextureSampler();
    _modelPipeline->prepareModel();
    _lightPipeline->prepareModel();
    createCommandBuffers();
    createSyncObjects();
}

void App::handleKeystrokes(){
    if (isPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(_window, true);
    if (isPressed(GLFW_KEY_1)) _appConfig->movementMode(MOVEMENT_CAMERA);
    if (isPressed(GLFW_KEY_2)) _appConfig->movementMode(MOVEMENT_LIGHT);
    if (_appConfig->movementMode() == MOVEMENT_LIGHT){
        if (isPressed(GLFW_KEY_W)) _appConfig->lightPosition().x += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_S)) _appConfig->lightPosition().x += -_appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_A)) _appConfig->lightPosition().y += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_D)) _appConfig->lightPosition().y += -_appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_Q)) _appConfig->lightPosition().z += _appConfig->lightSpeed();
        if (isPressed(GLFW_KEY_E)) _appConfig->lightPosition().z += -_appConfig->lightSpeed();
    } else if (_appConfig->movementMode() == MOVEMENT_CAMERA) {
        if (isPressed(GLFW_KEY_W)) _appConfig->observerPosition() += _appConfig->cameraSpeed() * _appConfig->cameraFront();
        if (isPressed(GLFW_KEY_S)) _appConfig->observerPosition() -= _appConfig->cameraSpeed() * _appConfig->cameraFront();
        if (isPressed(GLFW_KEY_A)) _appConfig->observerPosition() -= glm::normalize(glm::cross(_appConfig->cameraFront(), _appConfig->cameraUp())) * _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_D)) _appConfig->observerPosition() += glm::normalize(glm::cross(_appConfig->cameraFront(), _appConfig->cameraUp())) * _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_Q)) _appConfig->observerPosition().z += _appConfig->cameraSpeed();
        if (isPressed(GLFW_KEY_E)) _appConfig->observerPosition().z += -_appConfig->cameraSpeed();
    }
}

bool App::isPressed(int key) {
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

void App::mainLoop() { //render frames
    auto start = std::chrono::high_resolution_clock::now();
    uint64_t framesCount = 0;
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        handleKeystrokes();
        drawFrame();
        framesCount++;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto executionTime = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout<<"Avg fps: "<<framesCount / (double) executionTime.count()<<std::endl;

    vkDeviceWaitIdle(_device->logical());
}

void App::cleanup() {
    delete _swapChain;

    vkDestroyRenderPass(_device->logical(), _swapChain->renderPass(), nullptr);

    delete _modelPipeline;
    delete _lightPipeline;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_device->logical(), _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(_device->logical(), _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(_device->logical(), _inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(_device->logical(), _device->commandPool(), nullptr);

    delete _device;


    glfwDestroyWindow(_window);

    glfwTerminate();
}

void App::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChain->imageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //Clear the values to a constant at the start
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = _device->findDepthFormat();
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

    if (vkCreateRenderPass(_device->logical(), &renderPassInfo, nullptr, &_swapChain->renderPass()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void App::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = _device->findQueueFamilies(_device->physical());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //Allow command buffers to be rerecorded individually, without this flag they all have to be reset together
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(_device->logical(), &poolInfo, nullptr, &(_device->commandPool())) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void App::createCommandBuffers() {
    _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _device->commandPool();
    //Can be submitted to a queue for execution, but cannot be called from other command buffers.
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

    if (vkAllocateCommandBuffers(_device->logical(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
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
    renderPassInfo.renderPass = _swapChain->renderPass();
    renderPassInfo.framebuffer = _swapChain->framebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = _swapChain->extent();

    //color of pixels within render are with undefined values
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _modelPipeline->pipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(_swapChain->extent().width);
    viewport.height = static_cast<float>(_swapChain->extent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChain->extent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    _modelPipeline->bind(commandBuffer, _currentFrame);

    //-----light-------
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _lightPipeline->pipeline());

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    
    _lightPipeline->bind(commandBuffer, _currentFrame);
  
    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::drawFrame() {
    vkWaitForFences(_device->logical(), 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(_device->logical(), _swapChain->swapChain(), UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        _swapChain->recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    _modelPipeline->updateUniformBuffer(_currentFrame);
    _lightPipeline->updateUniformBuffer(_currentFrame);

    // Only reset the fence if we are submitting work
    vkResetFences(_device->logical(), 1, &_inFlightFences[_currentFrame]);

    vkResetCommandBuffer(_commandBuffers[_currentFrame],  0);
    recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[_currentFrame];

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_device->graphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain->swapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(_device->presentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        _framebufferResized = false;
        _swapChain->recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    if (_appConfig->movementMode() == MOVEMENT_CAMERA){
        std::cout<<"CAM "<<_appConfig->observerPosition().x<<";"
                        <<_appConfig->observerPosition().y<<";"
                        <<_appConfig->observerPosition().z<<"\n";
    } else if (_appConfig->movementMode() == MOVEMENT_LIGHT){
        std::cout<<"SRC "<<_appConfig->lightPosition().x<<";"
                        <<_appConfig->lightPosition().y<<";"
                        <<_appConfig->lightPosition().z<<"\n";
    }
}

void App::createSyncObjects() {
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(_device->logical(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(_device->logical(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(_device->logical(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}


void App::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    app->_framebufferResized = true;
}

void App::mouseMovementCallback(GLFWwindow* window, double xpos, double ypos){
    auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    if (app->_appConfig->firstMouse()) {
        app->_appConfig->lastX(xpos);
        app->_appConfig->lastY(ypos);
        app->_appConfig->firstMouse(false); 
    }

    float xoffset = xpos - app->_appConfig->lastX();
    float yoffset = ypos - app->_appConfig->lastY(); 
    app->_appConfig->lastX(xpos);
    app->_appConfig->lastY(ypos);

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    app->_appConfig->yaw(app->_appConfig->yaw() - xoffset);
    app->_appConfig->pitch(app->_appConfig->pitch() - yoffset);

    app->_appConfig->pitch(std::clamp(app->_appConfig->pitch(), -89.0f, 89.0f));
    
    app->_appConfig->cameraFront().x = cos(glm::radians(app->_appConfig->yaw())) * cos(glm::radians(app->_appConfig->pitch()));
    app->_appConfig->cameraFront().y = sin(glm::radians(app->_appConfig->yaw())) * cos(glm::radians(app->_appConfig->pitch()));
    app->_appConfig->cameraFront().z = sin(glm::radians(app->_appConfig->pitch()));
}  

}
