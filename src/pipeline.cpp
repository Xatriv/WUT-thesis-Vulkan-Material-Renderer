#define TINYOBJLOADER_IMPLEMENTATION

#include "app_config.h"
#include "pipeline.h"

namespace vmr{

Pipeline::Pipeline(Device* device, SwapChain* swapChain, AppConfig* appConfig, std::string vertPath, std::string fragPath, std::string modelPath) 
            : _device(device), _swapChain(swapChain), _appConfig(appConfig), _modelPath(modelPath){};

Pipeline::~Pipeline(){
    vkDestroyPipeline(_device->logical(), _graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(_device->logical(), _pipelineLayout, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(_device->logical(), _uniformBuffers[i], nullptr);
        vkFreeMemory(_device->logical(), _uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(_device->logical(), _descriptorPool, nullptr);

    vkDestroyDescriptorSetLayout(_device->logical(), _descriptorSetLayout, nullptr);

    vkDestroyBuffer(_device->logical(), _indexBuffer, nullptr);
    vkFreeMemory(_device->logical(), _indexBufferMemory, nullptr);

    vkDestroyBuffer(_device->logical(), _vertexBuffer, nullptr);
    vkFreeMemory(_device->logical(), _vertexBufferMemory, nullptr);
}


//helper function to read the spir-v files
std::vector<char> Pipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}


VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(_device->logical(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

void Pipeline::createIndexBuffer() {
    VkDeviceSize bufferSize = sizeof(_indices[0]) * _indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    _device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(_device->logical(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, _indices.data(), (size_t) bufferSize);
    vkUnmapMemory(_device->logical(), stagingBufferMemory);

    _device->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

    _device->copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    vkDestroyBuffer(_device->logical(), stagingBuffer, nullptr);
    vkFreeMemory(_device->logical(), stagingBufferMemory, nullptr);
}

void Pipeline::bind(VkCommandBuffer& commandBuffer, int currentFrame) {
    VkBuffer vertexBuffers[] = {_vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets[currentFrame], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
}

void Pipeline::printModelInfo() {
    std::cout<<"Loaded model: \""<<_modelPath<<"\" ("<<_vertices.size()<<" vertices; "<<_indices.size()<<" indices )\n";
}

}