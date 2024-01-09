#pragma once

#include <tiny_obj_loader.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>

#include "app_config.h"
#include "device.h"
#include "swap_chain.h"
#include "vertex.h"

const int MAX_FRAMES_IN_FLIGHT = 2;


namespace std {
    template<> struct hash<vmr::Vertex> {
        size_t operator()(vmr::Vertex const& vertex) const {
            //TODO include all fields
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
};

struct ModelUniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 lightPosition;
    alignas(16) int shininess;
};

struct LightUniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;

};


namespace vmr {
class Pipeline {
private:
    VkPipelineLayout _pipelineLayout;
    VkPipeline _graphicsPipeline;
    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;

    std::vector<char> readFile(const std::string &filename);
    VkShaderModule createShaderModule(const std::vector<char> &code);

    virtual void createDescriptorPool() = 0;
    virtual void createDescriptorSets() = 0;
    virtual void createUniformBuffers() = 0;

protected:
    AppConfig *_appConfig;
    Device *_device;
    SwapChain *_swapChain;
    std::string _modelPath;
    std::vector<Vertex> _vertices;
    std::vector<uint32_t> _indices;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorSetLayout _descriptorSetLayout;
    std::vector<VkBuffer> _uniformBuffers;
    VkDescriptorPool _descriptorPool;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;
    std::vector<void *> _uniformBuffersMapped;

    virtual void createDescriptorSetLayout() = 0;
    void createGraphicsPipeline(std::string vertPath, std::string fragPath);
    void createVertexBuffer();
    void createIndexBuffer();
    void loadModel();

public:
    Pipeline(Device *device, SwapChain *swapChain, AppConfig *appConfig, std::string vertPath, std::string fragPath, std::string modelPath);
    ~Pipeline();
    VkPipeline &pipeline() { return _graphicsPipeline; }
    VkPipelineLayout &layout() { return _pipelineLayout; }
    std::vector<VkDescriptorSet> descriptorSets() { return _descriptorSets; }

    void bind(VkCommandBuffer &commandBuffer, int currentFrame);
    virtual void updateUniformBuffer(uint32_t currentImage) = 0;
    virtual void prepareModel() = 0;
};
}