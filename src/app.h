#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <optional>
#include <array>
#include <vector>

#include "device.h"
#include "swap_chain.h"
#include "pipeline.h"

#define MOVEMENT_CAMERA 0
#define MOVEMENT_LIGHT 1


namespace vmr {
class App{
public:
    void run();
private:
    GLFWwindow* window;
    Device* device;
    SwapChain* swapChain;
    Pipeline* modelPipeline;
    Pipeline* lightPipeline;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;
    std::vector<Vertex> sphereVertices;
    std::vector<uint32_t> sphereIndices;

    int movementMode = MOVEMENT_CAMERA;
    glm::vec3 lightPosition = glm::vec3(20.0f, 0.0f, 10.0f);
    glm::vec3 observerPosition = glm::vec3(0.0f, 5.0f, 2.0f);

    void initWindow();
    void initVulkan();
    void handleKeystrokes();
    void mainLoop();
    void cleanup();
    void createRenderPass();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObjects();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

}

