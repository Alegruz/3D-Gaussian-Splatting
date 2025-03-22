#include "3dgs/graphics/Buffer.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/PhysicalDevice.h"

namespace iiixrlab::graphics
{
	void Buffer::create(VkDevice device, CreateInfo& inoutCreateInfo, const PhysicalDevice& physicalDevice, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags& memoryPropertyFlag) noexcept
	{
		VkResult vr = VK_SUCCESS;
		VkBufferCreateInfo bufferCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = inoutCreateInfo.GpuResourceCreateInfo.Size,
			.usage = usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		};
		vr = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &inoutCreateInfo.Buffer);
		assert(vr == VK_SUCCESS && inoutCreateInfo.Buffer != VK_NULL_HANDLE);
#if defined(_DEBUG)
		inoutCreateInfo.GpuResourceCreateInfo.Device.SetDebugName(inoutCreateInfo.GpuResourceCreateInfo.Name, VK_OBJECT_TYPE_BUFFER, inoutCreateInfo.Buffer);
#endif	// defined(_DEBUG)

		VkMemoryRequirements memoryRequirements = {};
		vkGetBufferMemoryRequirements(device, inoutCreateInfo.Buffer, &memoryRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memoryRequirements.size,
			.memoryTypeIndex = PhysicalDevice::GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryPropertyFlag, physicalDevice.GetPhysicalDeviceMemoryProperties()),
		};
		vr = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &inoutCreateInfo.BufferMemory);
		assert(vr == VK_SUCCESS && inoutCreateInfo.BufferMemory != VK_NULL_HANDLE);
#if defined(_DEBUG)
		inoutCreateInfo.GpuResourceCreateInfo.Device.SetDebugName(inoutCreateInfo.GpuResourceCreateInfo.Name, VK_OBJECT_TYPE_DEVICE_MEMORY, inoutCreateInfo.BufferMemory);
#endif	// defined(_DEBUG)

		vr = vkBindBufferMemory(device, inoutCreateInfo.Buffer, inoutCreateInfo.BufferMemory, 0);
		assert(vr == VK_SUCCESS);
	}

	Buffer::~Buffer() noexcept
	{
		mDevice.DestroyBuffer(mBuffer);
		mDevice.FreeMemory(mBufferMemory);
	}
} // namespace iiixrlab::graphics
