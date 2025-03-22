#include "3dgs/graphics/Pipeline.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    Pipeline::~Pipeline() noexcept
    {
		mDevice.DestroyPipeline(mPipeline);
		mDevice.DestroyPipelineLayout(mPipelineLayout);

        for (VkDescriptorSetLayout& descriptorSetLayout : mDescriptorSetLayouts)
        {
            mDevice.DestroyDescriptorSetLayout(descriptorSetLayout);
        }
    }
} // namespace iiixrlab::graphics
