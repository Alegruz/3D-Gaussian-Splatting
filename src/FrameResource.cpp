#include "3dgs/graphics/FrameResource.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/Device.hpp"
#include "3dgs/graphics/Renderer.h"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"

namespace iiixrlab::graphics
{
	FrameResource::FrameResource(const CreateInfo& createInfo) noexcept
		: mDevice(createInfo.Device)
		, mSwapChain(createInfo.SwapChain)
		, mCommandBuffer(createInfo.CommandBuffer)
		, mBackBuffer(createInfo.BackBuffer)
		, mDepthBuffer(createInfo.DepthBuffer)
		, mPresentCompleteSemaphore(createInfo.Device.CreateSemaphore("Present Complete Semaphore"))
		, mRenderFinishedSemaphore(createInfo.Device.CreateSemaphore("Render Finished Semaphore"))
		, mWaitFence(createInfo.Device.CreateFence("Wait Fence"))
		, mFrameIndex(createInfo.FrameIndex)
		, mFramesCount(createInfo.FramesCount)
	{
		assert(mPresentCompleteSemaphore != VK_NULL_HANDLE);
		assert(mRenderFinishedSemaphore != VK_NULL_HANDLE);
		assert(mWaitFence != VK_NULL_HANDLE);
	}

	FrameResource::FrameResource(FrameResource&& other) noexcept
		: mDevice(other.mDevice)
		, mSwapChain(other.mSwapChain)
		, mCommandBuffer(other.mCommandBuffer)
		, mBackBuffer(other.mBackBuffer)
		, mDepthBuffer(other.mDepthBuffer)
		, mPresentCompleteSemaphore(other.mPresentCompleteSemaphore)
		, mRenderFinishedSemaphore(other.mRenderFinishedSemaphore)
		, mWaitFence(other.mWaitFence)
		, mFrameIndex(other.mFrameIndex)
		, mFramesCount(other.mFramesCount)
	{
		other.mPresentCompleteSemaphore = VK_NULL_HANDLE;
		other.mRenderFinishedSemaphore = VK_NULL_HANDLE;
		other.mWaitFence = VK_NULL_HANDLE;
		other.mFrameIndex = UINT32_MAX;
		other.mFramesCount = UINT32_MAX - 1;
	}

	FrameResource::~FrameResource() noexcept
	{
		Wait();
		mDevice.DestroyFence(mWaitFence);
		mDevice.DestroySemaphore(mRenderFinishedSemaphore);
		mDevice.DestroySemaphore(mPresentCompleteSemaphore);
	}

	void FrameResource::Begin() noexcept
	{
		mCommandBuffer.Reset();
		mCommandBuffer.Begin(*this);
	}

	void FrameResource::End() noexcept
	{
		mCommandBuffer.End();
	}

	void FrameResource::Wait() noexcept
	{
		mDevice.WaitForFence(mWaitFence);
		mDevice.ResetFence(mWaitFence);
	}
} // namespace iiixrlab
