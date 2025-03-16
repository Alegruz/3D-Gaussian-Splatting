#include "3dgs/graphics/Queue.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/FrameResource.h"
#include "3dgs/graphics/SwapChain.h"

namespace iiixrlab::graphics
{
    Queue::Queue(CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mQueue(createInfo.Queue)
        , mQueueFamilyIndex(createInfo.QueueFamilyIndex)
        , mQueueIndex(createInfo.QueueIndex)
    {
        assert(mQueue != VK_NULL_HANDLE);
    }

    Queue::~Queue() noexcept
    {
        mQueue = VK_NULL_HANDLE;
    }

    void Queue::Present(SwapChain& swapChain, FrameResource& frameResource) noexcept
    {
        VkSemaphore semaphore = frameResource.GetRenderFinishedSemaphore();
        uint32_t frameIndex = frameResource.GetFrameIndex();
        VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapChain.mSwapChain,
            .pImageIndices = &frameIndex,
            .pResults = nullptr,
        };
        VkResult vr = vkQueuePresentKHR(mQueue, &presentInfo);
        if (vr == VK_SUBOPTIMAL_KHR || vr == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // Recreate swap chain
            assert(false);
        }
        else
        {
            assert(vr == VK_SUCCESS);
        }
    }

    void Queue::Submit(FrameResource& frameResource) noexcept
    {
        VkSemaphore waitSemaphore = frameResource.GetPresentCompleteSemaphore();
        VkSemaphore signalSemaphore = frameResource.GetRenderFinishedSemaphore();
        VkFence fence = frameResource.GetWaitFence();
        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        CommandBuffer& commandBuffer = frameResource.GetCommandBuffer();

        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &waitSemaphore,
            .pWaitDstStageMask = &waitDstStageMask,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer.mCommandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &signalSemaphore,
        };
        VkResult vr = vkQueueSubmit(mQueue, 1, &submitInfo, fence);
        assert(vr == VK_SUCCESS);
    }

    void Queue::Wait() noexcept
    {
        vkQueueWaitIdle(mQueue);
    }
} // namespace iiixrlab::graphics
