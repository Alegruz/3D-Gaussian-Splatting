#include "3dgs/graphics/DescriptorPool.h"

#include "3dgs/graphics/DescriptorSet.h"
#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
        : mDevice(other.mDevice)
        , mDescriptorPool(other.mDescriptorPool)
    {
        other.mDescriptorPool = VK_NULL_HANDLE;
    }

    DescriptorPool::~DescriptorPool() noexcept
    {
        mDevice.DestroyDescriptorPool(mDescriptorPool);
    }

    void DescriptorPool::AllocateDescriptorSets(std::vector<std::unique_ptr<DescriptorSet>>& inoutDescriptorSets, const VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::string>& names) noexcept
    {
        mDevice.AllocateDescriptorSets(*this, inoutDescriptorSets, descriptorSetLayout, names);
    }

    void DescriptorPool::DeallocateDescriptorSets(std::vector<std::unique_ptr<DescriptorSet>>& inoutDescriptorSets) noexcept
    {
        mDevice.DeallocateDescriptorSets(*this, inoutDescriptorSets);
    }
} // namespace iiixrlab::graphics
