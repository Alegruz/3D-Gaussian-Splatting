#include "3dgs/graphics/Renderer.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/CommandPool.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/FrameResource.h"
#include "3dgs/graphics/Instance.h"
#include "3dgs/graphics/PhysicalDevice.h"
#include "3dgs/graphics/Queue.h"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"
#include "3dgs/Window.h"

namespace iiixrlab::graphics
{
	Renderer::Renderer(const RendererCreateInfo& createInfo) noexcept
		: mInstance(std::make_unique<Instance>(Instance::CreateInfo{.ApplicationInfo = createInfo.ApplicationInfo, .EngineInfo = createInfo.EngineInfo}))
		, mFrameResources()
		, mCurrentFrameIndex(0)
		, mPipelineLayout(VK_NULL_HANDLE)
		, mPipeline(VK_NULL_HANDLE)
		, mDescriptorSetLayout(VK_NULL_HANDLE)
		, mDescriptorPool(VK_NULL_HANDLE)
	{
		Device& device = mInstance->GetPhysicalDevice().GetDevice();
		SwapChain& swapChain = mInstance->InitializeSwapChain(createInfo.FramesCount, createInfo.Window);
		CommandPool& commandPool = mInstance->GetPhysicalDevice().GetDevice().InitializeCommandPool();
		const uint32_t framesCount = swapChain.GetFramesCount();
		commandPool.AllocateCommandBuffers("CommandBuffer", framesCount);

		for (uint32_t frameBufferIndex = 0; frameBufferIndex < framesCount; ++frameBufferIndex)
		{			
			FrameResource::CreateInfo frameResourceCreateInfo =
			{
				.Device = device,
				.SwapChain = swapChain,
				.CommandBuffer = commandPool.GetCommandBuffer(frameBufferIndex),
				.BackBuffer = *swapChain.GetBackBuffer(frameBufferIndex).Color,
				.DepthBuffer = *swapChain.GetBackBuffer(frameBufferIndex).Depth,
				.FrameIndex = frameBufferIndex,
				.FramesCount = framesCount,
			};
			mFrameResources.push_back(std::make_unique<FrameResource>(frameResourceCreateInfo));
		}
	}

	Renderer::~Renderer() noexcept
	{
		mFrameResources.clear();
		mInstance.reset();
	}

	void Renderer::Render() noexcept
	{
		FrameResource& currentFrameResource = *mFrameResources[mCurrentFrameIndex];
		currentFrameResource.Wait();

		Device& device = mInstance->GetPhysicalDevice().GetDevice();
		SwapChain& swapChain = mInstance->GetSwapChain();
		uint32_t imageIndex = device.AcquireNextImage(swapChain, currentFrameResource.GetPresentCompleteSemaphore(), VK_NULL_HANDLE);
		assert(mCurrentFrameIndex == imageIndex);
		mCurrentFrameIndex = imageIndex;

		currentFrameResource.Begin();
		currentFrameResource.End();

		Queue& queue = device.GetQueue();
		queue.Submit(currentFrameResource);
		queue.Present(swapChain, currentFrameResource);
		mCurrentFrameIndex = (mCurrentFrameIndex + 1) % swapChain.GetFramesCount();
	}
}