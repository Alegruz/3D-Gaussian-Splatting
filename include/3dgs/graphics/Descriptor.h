#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class Device;

    class Descriptor final
    {
    public:
        struct CreateInfo
        {
            Device& Device;
            VkDescriptorSet DescriptorSet;
        };

    public:
        Descriptor() = delete;

        Descriptor(const Descriptor&) = delete;
        Descriptor& operator=(const Descriptor&) = delete;

        ~Descriptor() noexcept = default;

        IIIXRLAB_INLINE constexpr Descriptor(Descriptor&& other) noexcept = default;
        Descriptor& operator=(Descriptor&&) = delete;
    
    protected:
        IIIXRLAB_INLINE constexpr Descriptor(const CreateInfo& createInfo) noexcept
            : mDevice(createInfo.Device)
            , mDescriptorSet(createInfo.DescriptorSet)
        {
        }
    
    protected:
        Device& mDevice;
        VkDescriptorSet mDescriptorSet;
    };
} // namespace iiixrlab::graphics
