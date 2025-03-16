#include "3dgs/graphics/CommandBuffer.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    CommandBuffer::CommandBuffer(const CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mCommandBuffer(createInfo.CommandBuffer)
    {
        assert(mCommandBuffer != VK_NULL_HANDLE);
    }

    CommandBuffer::~CommandBuffer() noexcept
    {
        mDevice.DestroyCommandBuffer(mCommandBuffer);
    }

    void CommandBuffer::Begin() noexcept
    {
        VkCommandBufferBeginInfo commandBufferBeginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
        };
        VkResult vr = vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo);
        assert(vr == VK_SUCCESS);
    }

    void CommandBuffer::End() noexcept
    {
        VkResult vr = vkEndCommandBuffer(mCommandBuffer);
        assert(vr == VK_SUCCESS);
    }

    void CommandBuffer::Reset() noexcept
    {
        VkResult vr = vkResetCommandBuffer(mCommandBuffer, 0);
        assert(vr == VK_SUCCESS);
    }
}   // namespace iiixrlab::graphics