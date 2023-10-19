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
private:
    const uint32_t WIDTH = 1280;
    const uint32_t HEIGHT = 720;
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

    int _movementMode = MOVEMENT_CAMERA;
    glm::vec3 _lightPosition = glm::vec3(20.0f, 0.0f, 10.0f);
    glm::vec3 _observerPosition = glm::vec3(1.0f, 4.5f, 2.0f);
    glm::vec3 _cameraDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 _cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);
    const float cameraSpeed = 0.005f;
    bool firstMouse = true;
    double lastX = WIDTH / 2;
    double lastY = HEIGHT / 2;
    float pitch;
    float yaw;

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
    void run();
};

}

