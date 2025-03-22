#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
    class Buffer;
    class Device;
    class FrameResource;
    class Pipeline;

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
        void BeginRender() noexcept;
        void BindDescriptorSets(const VkPipelineLayout pipelineLayout, const VkDescriptorSet& descriptorSet) noexcept;
        void Bind(const Pipeline& pipeline) noexcept;
        void CopyBuffer(const Buffer& srcBuffer, Buffer& dstBuffer, const VkBufferCopy& bufferCopy) noexcept;
        void Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance) noexcept;
        void DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) noexcept;
        void End() noexcept;
        void Reset() noexcept;
    
    private:
        Device& mDevice;
        VkCommandBuffer mCommandBuffer;

        FrameResource* mFrameResourceOrNull;
    };
} // namespace iiixrlab::graphics
