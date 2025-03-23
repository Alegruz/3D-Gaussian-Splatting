#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class ConstantBuffer;
    
    class DescriptorSet final
    {
    public:
        friend class CommandBuffer;
        friend class Device;

    public:
        struct CreateInfo
        {
            Device& Device;
            VkDescriptorSet DescriptorSet;
        };

    public:
        DescriptorSet() = delete;

        DescriptorSet(const DescriptorSet&) = delete;
        DescriptorSet& operator=(const DescriptorSet&) = delete;

        ~DescriptorSet() noexcept = default;

        IIIXRLAB_INLINE constexpr DescriptorSet(DescriptorSet&& other) noexcept
            : mDevice(other.mDevice)
            , mDescriptorSet(other.mDescriptorSet)
        {
            other.mDescriptorSet = VK_NULL_HANDLE;
        }
        DescriptorSet& operator=(DescriptorSet&&) = delete;

        void Bind(const ConstantBuffer& descriptorBufferInfos) noexcept;
    
    protected:
        IIIXRLAB_INLINE constexpr DescriptorSet(const CreateInfo& createInfo) noexcept
            : mDevice(createInfo.Device)
            , mDescriptorSet(createInfo.DescriptorSet)
        {
        }
    
    protected:
        Device& mDevice;
        VkDescriptorSet mDescriptorSet;
    };
} // namespace iiixrlab::graphics
