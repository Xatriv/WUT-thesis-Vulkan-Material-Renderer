#pragma once

#include <optional>
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <limits>

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
    bool isPressed(int key);
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

