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

#include "vertex.h"
#include "device.h"
#include "swap_chain.h"

#define MOVEMENT_CAMERA 0
#define MOVEMENT_LIGHT 1


struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 lightModel;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 rgb;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 lightPosition;
    alignas(16) int shininess;
};

namespace vmr {
class App{
public:
    void run();
private:
    GLFWwindow* window;
    Device* device;
    SwapChain* swapChain;
    VkPipelineLayout pipelineLayout;
    VkPipelineLayout lightPipelineLayout;
    VkPipeline graphicsPipeline;
    VkPipeline lightGraphicsPipeline;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;
    VkBuffer vertexBuffer;
    VkBuffer sphereVertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory sphereVertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer sphereIndexBuffer;
    VkDeviceMemory sphereIndexBufferMemory;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;


    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int modelIndicesCount;
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
    void createGraphicsPipeline(VkPipeline* pipeline, VkPipelineLayout* layout,  std::string vertPath, std::string fragPath);
    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void createSyncObjects();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void createVertexBuffer();
    void createSphereVertexBuffer();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createIndexBuffer();
    void createSphereIndexBuffer();
    void createDescriptorSetLayout();
    void createUniformBuffers();
    void updateUniformBuffer(uint32_t currentImage);
    void createDescriptorPool();
    void createDescriptorSets();
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);   
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createTextureImageView();
    void createTextureSampler();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    void loadModel();
};
}

