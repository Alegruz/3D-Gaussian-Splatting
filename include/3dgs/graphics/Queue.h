#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;
	class FrameResource;
	class SwapChain;

	class Queue final
	{
	public:
		struct CreateInfo final
		{
			Device&	Device;
			VkQueue Queue;
			uint32_t QueueFamilyIndex;
			uint32_t QueueIndex;
		};

	public:
		Queue() = default;

		Queue(CreateInfo& createInfo) noexcept;

		Queue(const Queue&) = delete;
		Queue& operator=(const Queue&) = delete;

		Queue(Queue&&) = delete;
		Queue& operator=(Queue&&) = delete;

		~Queue() noexcept;

		void Present(SwapChain& swapChain, FrameResource& frameResource) noexcept;
		void Submit(FrameResource& frameResource) noexcept;
		void Wait() noexcept;

	private:
		Device&	mDevice;
		VkQueue	mQueue;
		uint32_t mQueueFamilyIndex;
		uint32_t mQueueIndex;
	};
} // namespace iiixrlab::graphics
