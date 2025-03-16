#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class CommandBuffer;
    class Device;

    class CommandPool final
    {
    public:
        struct CreateInfo final
        {
            Device& Device;
            VkCommandPool CommandPool;
        };

    public:
        CommandPool() = delete;

        CommandPool(const CreateInfo& createInfo) noexcept;

        CommandPool(const CommandPool&) = delete;
        CommandPool& operator=(const CommandPool&) = delete;

        ~CommandPool() noexcept;

        CommandPool(CommandPool&&) = delete;
        CommandPool& operator=(CommandPool&&) = delete;

        IIIXRLAB_INLINE CommandBuffer& GetCommandBuffer(const uint32_t index) noexcept { return *mCommandBuffers[index]; }
        IIIXRLAB_INLINE const CommandBuffer& GetCommandBuffer(const uint32_t index) const noexcept { return *mCommandBuffers[index]; }
        IIIXRLAB_INLINE constexpr VkCommandPool GetCommandPool() const noexcept { return mCommandPool; }

        void AllocateCommandBuffers(const char* name, const uint32_t commandBuffersCount) noexcept;
        void FreeCommandBuffers() noexcept;
    
    private:
        Device& mDevice;
        VkCommandPool mCommandPool;

        std::vector<std::unique_ptr<CommandBuffer>> mCommandBuffers;
    };
}   // namespace iiixrlab::graphics