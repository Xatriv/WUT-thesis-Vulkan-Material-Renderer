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
#include "model_pipeline.h"
#include "light_pipeline.h"
#include "window.h"


namespace vmr {
class App{
private:
    AppConfig* _appConfig;
    Window* _window;
    Device* _device;
    SwapChain* _swapChain;
    ModelPipeline* _modelPipeline;
    LightPipeline* _lightPipeline;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    uint32_t _currentFrame = 0;

    void initVulkan();
    void mainLoop();
    void cleanup();
    void createRenderPass();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObjects();
    
public:
    App(AppConfig* config);
    void run();
};

}

