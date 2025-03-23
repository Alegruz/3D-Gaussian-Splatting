#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class DescriptorSet;

    class DescriptorPool final
    {
    public:
        friend class Device;

    public:
        struct CreateInfo
        {
            Device& Device;
            VkDescriptorPool DescriptorPool;
        };

    public:
        DescriptorPool() = delete;

        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        DescriptorPool(DescriptorPool&& other) noexcept;
        DescriptorPool& operator=(DescriptorPool&&) = delete;

        ~DescriptorPool() noexcept;

        void AllocateDescriptorSets(std::vector<std::unique_ptr<DescriptorSet>>& inoutDescriptorSets, const VkDescriptorSetLayout descriptorSetLayout, const std::vector<std::string>& names) noexcept;
        void DeallocateDescriptorSets(std::vector<std::unique_ptr<DescriptorSet>>& inoutDescriptorSets) noexcept;

    protected:
        IIIXRLAB_INLINE constexpr DescriptorPool(const CreateInfo& createInfo) noexcept
            : mDevice(createInfo.Device)
            , mDescriptorPool(createInfo.DescriptorPool)
        {
        }

    protected:
        Device& mDevice;
        VkDescriptorPool mDescriptorPool;
    };
} // namespace iiixrlab::graphics
