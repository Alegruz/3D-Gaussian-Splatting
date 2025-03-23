#include "3dgs/graphics/Pipeline.h"

#include "3dgs/graphics/DescriptorPool.h"
#include "3dgs/graphics/DescriptorSet.h"
#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    Pipeline::Pipeline(CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mName(createInfo.Name)
        , mPipelineLayout(createInfo.PipelineLayout)
        , mPipeline(createInfo.Pipeline)
        , mDescriptorSetLayouts(std::move(createInfo.DescriptorSetLayouts))
        , mDescriptorSets()
    {
        for (std::unique_ptr<DescriptorSet>& descriptorSet : createInfo.DescriptorSets)
        {
            mDescriptorSets.push_back(std::move(descriptorSet));
        }
        createInfo.DescriptorSets.clear();
    }

    Pipeline::Pipeline(Pipeline&& other) noexcept
        : mDevice(other.mDevice)
        , mName(std::move(other.mName))
        , mPipelineLayout(other.mPipelineLayout)
        , mPipeline(other.mPipeline)
        , mDescriptorSetLayouts(std::move(other.mDescriptorSetLayouts))
        , mDescriptorSets(std::move(other.mDescriptorSets))
    {
        other.mPipelineLayout = VK_NULL_HANDLE;
        other.mPipeline = VK_NULL_HANDLE;
        other.mDescriptorSetLayouts.clear();
        other.mDescriptorSets.clear();
    }

    Pipeline::~Pipeline() noexcept
    {
        mDevice.GetDescriptorPool().DeallocateDescriptorSets(mDescriptorSets);
		mDevice.DestroyPipeline(mPipeline);
		mDevice.DestroyPipelineLayout(mPipelineLayout);

        for (VkDescriptorSetLayout& descriptorSetLayout : mDescriptorSetLayouts)
        {
            mDevice.DestroyDescriptorSetLayout(descriptorSetLayout);
        }
    }
} // namespace iiixrlab::graphics
