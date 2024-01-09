#pragma once

#include "pipeline.h"

namespace vmr
{
    class ModelPipeline : public Pipeline
    {
    private:
        void createDescriptorPool() override;
        void createDescriptorSets() override;
        void createDescriptorSetLayout() override;
        void createUniformBuffers() override;
        void prepareTangentSpace();

    public:
        ModelPipeline(Device *device, SwapChain *swapChain, AppConfig *appConfig, std::string vertPath, std::string fragPath, std::string modelPath);
        void updateUniformBuffer(uint32_t currentImage) override;
        void prepareModel() override;
    };
}