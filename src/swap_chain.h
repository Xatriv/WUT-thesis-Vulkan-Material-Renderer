#pragma once

#include <array>
#include <vector>

#include "device.h"
#include "app_config.h"


namespace vmr{
class SwapChain {

private:
    AppConfig* _appConfig;
    Device* _device;
    GLFWwindow* _window;
    VkSwapchainKHR _swapChain;
    std::vector<VkImage> _swapChainImages;
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    std::vector<VkImageView> _swapChainImageViews;
    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkRenderPass _renderPass;
    VkImage _depthImage;
    VkDeviceMemory _depthImageMemory;
    VkImageView _depthImageView;
    VkImage _textureImage;
    VkImage _normalMapImage;
    VkDeviceMemory _textureImageMemory;
    VkDeviceMemory _normalMapMemory;
    VkImageView _textureImageView;
    VkImageView _normalMapImageView;
    VkSampler _textureSampler;
    
    void createSwapChain();
    void createImageViews();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void cleanupSwapChain();
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createTextureImage(VkImage& image, VkDeviceMemory& memory, std::string path);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat format);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

public:
    SwapChain(Device* device, GLFWwindow* window, AppConfig* appConfig);
    ~SwapChain();

    VkSwapchainKHR&             swapChain()             {return _swapChain; }
    VkExtent2D&                 extent()                {return _swapChainExtent; }
    VkRenderPass&               renderPass()            {return _renderPass; }
    VkFormat&                   imageFormat()           {return _swapChainImageFormat; }
    std::vector<VkFramebuffer>& framebuffers()          {return _swapChainFramebuffers; }
    VkImageView&                textureImageView()      {return _textureImageView; }
    VkImageView&                normalMapImageView()    {return _normalMapImageView; }
    VkSampler                   textureSampler()        {return _textureSampler; }

    void recreateSwapChain();
    void createFramebuffers();
    void createDepthResources();
    void createTextureImages();
    void createTextureImageViews();
    void createTextureSampler();

};

}
