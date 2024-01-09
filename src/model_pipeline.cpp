#pragma once

#include "model_pipeline.h"

namespace vmr{

ModelPipeline::ModelPipeline(Device* device, SwapChain* swapChain, AppConfig* appConfig, std::string vertPath, std::string fragPath, std::string modelPath) 
            : Pipeline(device, swapChain, appConfig, vertPath, fragPath, modelPath){
    createDescriptorSetLayout();
    createGraphicsPipeline(vertPath, fragPath);
};

void ModelPipeline::prepareModel() {
    loadModel();
    prepareTangentSpace();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

void ModelPipeline::createUniformBuffers() {
    VkDeviceSize bufferSize =  sizeof( ModelUniformBufferObject);

    _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);

        vkMapMemory(_device->logical(), _uniformBuffersMemory[i], 0, bufferSize, 0, &_uniformBuffersMapped[i]);
    }
}

void ModelPipeline::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(_device->logical(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void ModelPipeline::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(_device->logical(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = _uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof( ModelUniformBufferObject);

        VkDescriptorImageInfo textureImageInfo{};
        textureImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        textureImageInfo.imageView = _swapChain->textureImageView();
        textureImageInfo.sampler = _swapChain->textureSampler();

        VkDescriptorImageInfo normalMapImageInfo{};
        normalMapImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        normalMapImageInfo.imageView = _swapChain->normalMapImageView();
        normalMapImageInfo.sampler = _swapChain->textureSampler();

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;


        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &textureImageInfo;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = _descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pImageInfo = &normalMapImageInfo;
        

        vkUpdateDescriptorSets(_device->logical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void ModelPipeline::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding normalMapLayoutBinding{};
    normalMapLayoutBinding.binding = 2;
    normalMapLayoutBinding.descriptorCount = 1;
    normalMapLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    normalMapLayoutBinding.pImmutableSamplers = nullptr;
    normalMapLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {uboLayoutBinding, samplerLayoutBinding, normalMapLayoutBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(_device->logical(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void ModelPipeline::updateUniformBuffer(uint32_t currentImage) {
    glm::vec3 cameraPos = _appConfig->observerPosition();
    auto cameraFront = glm::normalize(_appConfig->cameraFront());
    auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, _appConfig->cameraUp());
    auto proj = glm::perspective(glm::radians(70.0f), _swapChain->extent().width / (float) _swapChain->extent().height, 0.05f, 100.0f);
    proj[1][1] *= -1; // coordinate flip due to opposite Y in Vulkan vs OpenGL

    ModelUniformBufferObject ubo{};
    auto rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate = glm::rotate(rotate, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto scale = glm::scale(rotate, glm::vec3(0.5f, 0.5f, 0.5f));
    ubo.model = scale;
    ubo.view = view;
    ubo.proj = proj;
    ubo.shininess = 8;
    ubo.position = cameraPos;
    ubo.lightPosition = _appConfig->lightPosition();
    memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

}


void ModelPipeline::prepareTangentSpace(){
    for (int i = 0; i < _indices.size(); i += 3) {
        glm::vec3 v1 = _vertices.at(_indices.at(i)).pos;
        glm::vec3 v2 = _vertices.at(_indices.at(i+1)).pos;
        glm::vec3 v3 = _vertices.at(_indices.at(i+2)).pos;

        glm::vec2 uv1 = _vertices.at(_indices.at(i)).texCoord;
        glm::vec2 uv2 = _vertices.at(_indices.at(i+1)).texCoord;
        glm::vec2 uv3 = _vertices.at(_indices.at(i+2)).texCoord;

        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        _vertices.at(_indices.at(i)).tangent.x   = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        _vertices.at(_indices.at(i+1)).tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        _vertices.at(_indices.at(i+2)).tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        _vertices.at(_indices.at(i)).bitangent.x   = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        _vertices.at(_indices.at(i+1)).bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        _vertices.at(_indices.at(i+2)).bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    }
}

}