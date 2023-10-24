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


namespace vmr {
class App{
private:
    AppConfig* _appConfig;
    GLFWwindow* _window;
    Device* _device;
    SwapChain* _swapChain;
    Pipeline* _modelPipeline;
    Pipeline* _lightPipeline;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    uint32_t _currentFrame = 0;
    bool _framebufferResized = false;

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
    static void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos);
    
public:
    App(AppConfig* config);
    void run();
};

}

