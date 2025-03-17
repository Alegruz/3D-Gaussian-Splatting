#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class CommandBuffer;
	class Device;
	class SwapChain;
	class Texture;

	class FrameResource final
	{
	public:
		struct CreateInfo final
		{
			Device&			Device;
			SwapChain&		SwapChain;
			CommandBuffer&	CommandBuffer;
			Texture&		BackBuffer;
			Texture&		DepthBuffer;
			uint32_t        FrameIndex;
			uint32_t 	  	FramesCount;
		};

	public:
		FrameResource() = delete;
		FrameResource(const FrameResource&) = delete;
		FrameResource(FrameResource&& other) noexcept;

		FrameResource(const CreateInfo& createInfo) noexcept;

		~FrameResource() noexcept;

		FrameResource& operator=(const FrameResource&) = delete;
		FrameResource& operator=(FrameResource&&) = delete;

		void Begin() noexcept;
		void End() noexcept;
		void Wait() noexcept;

		IIIXRLAB_INLINE constexpr const SwapChain& GetSwapChain() const noexcept { return mSwapChain; }
		IIIXRLAB_INLINE constexpr CommandBuffer& GetCommandBuffer() noexcept { return mCommandBuffer; }
		IIIXRLAB_INLINE constexpr const CommandBuffer& GetCommandBuffer() const noexcept { return mCommandBuffer; }
		IIIXRLAB_INLINE constexpr VkSemaphore GetPresentCompleteSemaphore() const noexcept { return mPresentCompleteSemaphore; }
		IIIXRLAB_INLINE constexpr VkSemaphore GetRenderFinishedSemaphore() const noexcept { return mRenderFinishedSemaphore; }
		IIIXRLAB_INLINE constexpr VkFence GetWaitFence() const noexcept { return mWaitFence; }
		IIIXRLAB_INLINE constexpr uint32_t GetFrameIndex() const noexcept { return mFrameIndex; }
		IIIXRLAB_INLINE constexpr uint32_t GetFramesCount() const noexcept { return mFramesCount; }
		IIIXRLAB_INLINE constexpr Texture& GetBackBuffer() noexcept { return mBackBuffer; }
		IIIXRLAB_INLINE constexpr const Texture& GetBackBuffer() const noexcept { return mBackBuffer; }
		IIIXRLAB_INLINE constexpr Texture& GetDepthBuffer() noexcept { return mDepthBuffer; }
		IIIXRLAB_INLINE constexpr const Texture& GetDepthBuffer() const noexcept { return mDepthBuffer; }

	private:
		Device&			mDevice;
		SwapChain& 		mSwapChain;
		CommandBuffer&	mCommandBuffer;
        Texture&		mBackBuffer;
        Texture&		mDepthBuffer;
		VkSemaphore     mPresentCompleteSemaphore;
		VkSemaphore     mRenderFinishedSemaphore;
		VkFence         mWaitFence;
		uint32_t        mFrameIndex;
		uint32_t		mFramesCount;
	};
} // namespace iiixrlab
