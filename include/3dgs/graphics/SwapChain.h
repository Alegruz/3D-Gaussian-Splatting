#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Instance;
	class Texture;

	struct BackBuffer final
	{
		std::unique_ptr<Texture>	Color;
		std::unique_ptr<Texture>	Depth;
	};
	
	class SwapChain final
	{
	public:
		friend class Device;
		friend class Queue;

	public:
		struct CreateInfo final
		{
			Instance&		Instance;
			Device&			Device;
			VkSurfaceKHR	Surface;
			VkExtent3D		FrameExtent;
			uint32_t		FramesCount;
			VkSwapchainKHR	SwapChain;
			std::vector<BackBuffer>	BackBuffers;
		};

	public:
		SwapChain() = delete;

		SwapChain(CreateInfo& createInfo) noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		~SwapChain() noexcept;

		SwapChain(SwapChain&&) = delete;
		SwapChain& operator=(SwapChain&&) = delete;

		IIIXRLAB_INLINE constexpr uint32_t GetFramesCount() const noexcept { return mFramesCount; }
		IIIXRLAB_INLINE constexpr const BackBuffer& GetBackBuffer(const uint32_t index) const noexcept { return mBackBuffers[index]; }

	private:
		Instance&		mInstance;
		Device&			mDevice;
		VkSurfaceKHR	mSurface;
		uint32_t		mFramesCount;
		VkSwapchainKHR	mSwapChain;
		VkExtent2D		mFrameExtent;

		std::vector<BackBuffer>	mBackBuffers;
	};
} // namespace iiixrlab::graphics
