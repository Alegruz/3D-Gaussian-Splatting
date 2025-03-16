#include "3dgs/graphics/CommandPool.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/Device.hpp"

namespace iiixrlab::graphics
{
	CommandPool::CommandPool(const CreateInfo& createInfo) noexcept
		: mDevice(createInfo.Device)
		, mCommandPool(createInfo.CommandPool)
	{
		assert(mCommandPool != VK_NULL_HANDLE);
	}

	CommandPool::~CommandPool() noexcept
	{
		mCommandBuffers.clear();
		mDevice.DestroyCommandPool(mCommandPool);
	}

	void CommandPool::AllocateCommandBuffers(const char* name, const uint32_t commandBuffersCount) noexcept
	{
		for (uint32_t commandBufferIndex = 0; commandBufferIndex < commandBuffersCount; ++commandBufferIndex)
		{
			CommandBuffer::CreateInfo commandBufferCreateInfo =
			{
				.Device = mDevice,
				.CommandBuffer = mDevice.AllocateCommandBuffer(name),
			};
			mCommandBuffers.push_back(std::make_unique<CommandBuffer>(commandBufferCreateInfo));
		}
	}

	void CommandPool::FreeCommandBuffers() noexcept
	{
		mCommandBuffers.clear();
	}
} // namespace iiixrlab::graphics
