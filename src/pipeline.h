#pragma once

#include <tiny_obj_loader.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <variant>

#include "app_config.h"
#include "device.h"
#include "swap_chain.h"
#include "vertex.h"
#include "basic_vertex.h"

const int MAX_FRAMES_IN_FLIGHT = 2;


namespace std {
    template<> struct hash<vmr::Vertex> {
        size_t operator()(vmr::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };

    template<> struct hash<vmr::BasicVertex> {
        size_t operator()(vmr::BasicVertex const& vertex) const {
            return (hash<glm::vec3>()(vertex.pos));
        }
    };
};

struct ModelUniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 lightPosition;
};

struct LightUniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};


namespace vmr {
class Pipeline {
private:
    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;

    virtual void createDescriptorPool() = 0;
    virtual void createDescriptorSets() = 0;
    virtual void createUniformBuffers() = 0;

protected:
    AppConfig *_appConfig;
    Device *_device;
    SwapChain *_swapChain;
    std::string _modelPath;
    VkPipelineLayout _pipelineLayout;
    VkPipeline _graphicsPipeline;
    std::vector<std::variant<Vertex, BasicVertex>> _vertices;
    std::vector<uint32_t> _indices;
    std::vector<VkDescriptorSet> _descriptorSets;
    VkDescriptorSetLayout _descriptorSetLayout;
    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    std::vector<VkBuffer> _uniformBuffers;
    VkDescriptorPool _descriptorPool;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;
    std::vector<void *> _uniformBuffersMapped;

    virtual void createDescriptorSetLayout() = 0;
    virtual void loadModel() = 0;
    virtual void createVertexBuffer() = 0;
    virtual void createGraphicsPipeline(std::string vertPath, std::string fragPath) = 0;
    void createIndexBuffer();
    std::vector<char> readFile(const std::string &filename);
    VkShaderModule createShaderModule(const std::vector<char> &code);

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