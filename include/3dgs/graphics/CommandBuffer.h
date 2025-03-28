#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Buffer;
	class Device;
	class FrameResource;
	class Pipeline;
	class VertexBuffer;

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

		struct VertexBindingInfo final
		{
			uint32_t 		BindingIndex;
			VkDeviceSize	Stride;
		};
	
	public:
		CommandBuffer() = delete;

		CommandBuffer(const CreateInfo& createInfo) noexcept;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		~CommandBuffer() noexcept;

		CommandBuffer(CommandBuffer&&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;

		IIIXRLAB_INLINE constexpr FrameResource& GetFrameResource() noexcept { return *mFrameResourceOrNull; }
		IIIXRLAB_INLINE constexpr const FrameResource& GetFrameResource() const noexcept { return *mFrameResourceOrNull; }

		IIIXRLAB_INLINE constexpr const Pipeline& GetPipeline() const noexcept { return *mPipelineOrNull; }

		void Barrier(const VkPipelineStageFlags srcStageMask, const VkPipelineStageFlags dstStageMask, const VkImageMemoryBarrier& imageMemoryBarriers) noexcept;
		void Begin(FrameResource& frameResource) noexcept;
		void BeginRender() noexcept;
		void BindDescriptorSets(const VkPipelineLayout pipelineLayout, const VkDescriptorSet& descriptorSet) noexcept;
		void Bind(const Pipeline& pipeline) noexcept;
		void Bind(const VertexBuffer& vertexBuffer, const std::vector<VertexBindingInfo>& vertexBindingInfos) noexcept;
		void CopyBuffer(const Buffer& srcBuffer, Buffer& dstBuffer, const VkBufferCopy& bufferCopy) noexcept;
		void Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance) noexcept;
		void DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) noexcept;
		void End() noexcept;
		void Reset() noexcept;
	
	private:
		Device& mDevice;
		VkCommandBuffer mCommandBuffer;

		FrameResource* mFrameResourceOrNull;
		const Pipeline* mPipelineOrNull;
	};
} // namespace iiixrlab::graphics
