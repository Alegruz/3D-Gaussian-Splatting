#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class Device;
    class FrameResource;

    class CommandBuffer final
    {
    public:
        friend class Queue;

    public:
        struct CreateInfo final
        {
            Device& Device;
            VkCommandBuffer CommandBuffer;
        };
    
    public:
        CommandBuffer() = delete;

        CommandBuffer(const CreateInfo& createInfo) noexcept;

        CommandBuffer(const CommandBuffer&) = delete;
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        ~CommandBuffer() noexcept;

        CommandBuffer(CommandBuffer&&) = delete;
        CommandBuffer& operator=(CommandBuffer&&) = delete;

        IIIXRLAB_INLINE constexpr VkCommandBuffer GetCommandBuffer() const noexcept { return mCommandBuffer; }

        void Barrier(const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask, const VkImageMemoryBarrier& imageMemoryBarriers) noexcept;
        void Begin(FrameResource& frameResource) noexcept;
        void BindDescriptorSets(const VkPipelineLayout pipelineLayout, const VkDescriptorSet& descriptorSet) noexcept;
        void BindPipeline(const VkPipeline pipeline) noexcept;
        void End() noexcept;
        void Reset() noexcept;
    
    private:
        Device& mDevice;
        VkCommandBuffer mCommandBuffer;

        FrameResource* mFrameResourceOrNull;
    };
} // namespace iiixrlab::graphics
