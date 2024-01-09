#pragma once

#include "light_pipeline.h"

namespace vmr
{

    LightPipeline::LightPipeline(Device *device, SwapChain *swapChain, AppConfig *appConfig, std::string vertPath, std::string fragPath, std::string modelPath)
        : Pipeline(device, swapChain, appConfig, vertPath, fragPath, modelPath)
    {
        createDescriptorSetLayout();
        createGraphicsPipeline(vertPath, fragPath);
    };

    void LightPipeline::prepareModel()
    {
        loadModel();
        createVertexBuffer();
        createIndexBuffer();
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
    }

    void LightPipeline::createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(LightUniformBufferObject);

        _uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        _uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _device->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBuffersMemory[i]);

            vkMapMemory(_device->logical(), _uniformBuffersMemory[i], 0, bufferSize, 0, &_uniformBuffersMapped[i]);
        }
    }

    void LightPipeline::createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(_device->logical(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void LightPipeline::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, _descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        _descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(_device->logical(), &allocInfo, _descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(LightUniformBufferObject);

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = _descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(_device->logical(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void LightPipeline::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 1> bindings = {uboLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(_device->logical(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void LightPipeline::updateUniformBuffer(uint32_t currentImage)
    {
        glm::vec3 cameraPos = _appConfig->observerPosition();
        auto cameraFront = glm::normalize(_appConfig->cameraFront());
        auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, _appConfig->cameraUp());
        auto proj = glm::perspective(glm::radians(70.0f), _swapChain->extent().width / (float)_swapChain->extent().height, 0.05f, 100.0f);
        proj[1][1] *= -1; // coordinate flip due to opposite Y in Vulkan vs OpenGL

        const float scaleLight = 0.025f;

        LightUniformBufferObject ubo{};
        auto translate = glm::translate(glm::mat4(1.0f), _appConfig->lightPosition());
        auto scale = glm::scale(translate, glm::vec3(scaleLight, scaleLight, scaleLight));
        ubo.model = scale;
        ubo.view = view;
        ubo.proj = proj;
        memcpy(_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

}