#include "3dgs/graphics/Renderer.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/CommandPool.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/FrameResource.h"
#include "3dgs/graphics/Instance.h"
#include "3dgs/graphics/IRenderScene.hpp"
#include "3dgs/graphics/PhysicalDevice.h"
#include "3dgs/graphics/Queue.h"
#include "3dgs/graphics/Shader.h"
#include "3dgs/graphics/ShaderManager.h"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"
#include "3dgs/Window.h"

namespace iiixrlab::graphics
{
	Renderer::Renderer(const RendererCreateInfo& createInfo) noexcept
		: mRenderScene()
		, mInstance(std::make_unique<Instance>(Instance::CreateInfo{.ApplicationInfo = createInfo.ApplicationInfo, .EngineInfo = createInfo.EngineInfo}))
		, mFrameResources()
		, mCurrentFrameIndex(0)
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
		Device& device = mInstance->GetPhysicalDevice().GetDevice();
		device.GetQueue().Wait();
		
		if (mRenderScene != nullptr)
		{
			mRenderScene.reset();
		}

		for (std::unique_ptr<FrameResource>& frameResource : mFrameResources)
		{
			frameResource.reset();
		}
		mFrameResources.clear();
		mInstance.reset();
	}

	void Renderer::Render() noexcept
	{
		FrameResource& currentFrameResource = *mFrameResources[mCurrentFrameIndex];
		Device& device = mInstance->GetPhysicalDevice().GetDevice();
		SwapChain& swapChain = mInstance->GetSwapChain();
		
		CommandBuffer& commandBuffer = currentFrameResource.GetCommandBuffer();

		commandBuffer.BeginRender();
		
		mRenderScene->Render(commandBuffer);

		currentFrameResource.End();

		Queue& queue = device.GetQueue();
		queue.Submit(currentFrameResource);
		queue.Present(swapChain, currentFrameResource);
		mCurrentFrameIndex = (mCurrentFrameIndex + 1) % swapChain.GetFramesCount();
	}

	void Renderer::Update() noexcept
	{
		FrameResource& currentFrameResource = *mFrameResources[mCurrentFrameIndex];
		currentFrameResource.Wait();

		Device& device = mInstance->GetPhysicalDevice().GetDevice();
		SwapChain& swapChain = mInstance->GetSwapChain();
		uint32_t imageIndex = device.AcquireNextImage(swapChain, currentFrameResource.GetPresentCompleteSemaphore(), VK_NULL_HANDLE);
		assert(mCurrentFrameIndex == imageIndex);
		mCurrentFrameIndex = imageIndex;

		currentFrameResource.Begin();

		CommandBuffer& commandBuffer = currentFrameResource.GetCommandBuffer();

		mRenderScene->Update(commandBuffer);
	}
}