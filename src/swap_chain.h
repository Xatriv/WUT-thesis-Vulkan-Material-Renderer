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
#include <array>

#include <vector>

#include "device.h"


namespace vmr{
class SwapChain {

private:
//TODO underscores
    Device* _device;
    GLFWwindow* _window;
    VkSwapchainKHR _swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass _renderPass;
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    
    void createSwapChain();
    void createImageViews();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void cleanupSwapChain();
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat format);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
public:
    SwapChain(Device* device, GLFWwindow* window);
    ~SwapChain();
    void recreateSwapChain();
    VkSwapchainKHR&             swapChain()     {return _swapChain; }
    VkExtent2D&                 extent()        {return _swapChainExtent; }
    VkRenderPass&               renderPass()    {return _renderPass; }
    VkFormat&                   imageFormat()   {return _swapChainImageFormat; }
    std::vector<VkFramebuffer>& framebuffers()  {return swapChainFramebuffers; }

    void createFramebuffers();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();

};

}
