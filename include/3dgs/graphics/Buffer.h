#pragma once

#include "pch.h"

#include "3dgs/graphics/GpuResource.h"

namespace iiixrlab::graphics
{
	class PhysicalDevice;
	
	class Buffer : public GpuResource
	{
	public:
		friend class CommandBuffer;
		friend class Device;

	public:
		struct CreateInfo final
		{
			GpuResource::CreateInfo GpuResourceCreateInfo;
			VkBuffer Buffer;
			VkDeviceMemory BufferMemory;
		};

	public:
		Buffer() = delete;

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		~Buffer() noexcept;

		IIIXRLAB_INLINE constexpr Buffer(Buffer&& other) noexcept
			: GpuResource(std::move(other))
			, mBuffer(other.mBuffer)
			, mBufferMemory(other.mBufferMemory)
			, mDescriptorBufferInfo(other.mDescriptorBufferInfo)
		{
			other.mBuffer = VK_NULL_HANDLE;
			other.mBufferMemory = VK_NULL_HANDLE;
		}
		Buffer& operator=(Buffer&&) = delete;

		IIIXRLAB_INLINE constexpr const VkDescriptorBufferInfo& GetDescriptorBufferInfo() const noexcept { return mDescriptorBufferInfo; }

	protected:
		static void create(VkDevice device, CreateInfo& inoutCreateInfo, const PhysicalDevice& physicalDevice, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags& memoryPropertyFlag) noexcept;

	protected:
		IIIXRLAB_INLINE constexpr Buffer(const CreateInfo& createInfo) noexcept
			: GpuResource(createInfo.GpuResourceCreateInfo)
			, mBuffer(createInfo.Buffer)
			, mBufferMemory(createInfo.BufferMemory)
			, mDescriptorBufferInfo({ .buffer = mBuffer, .offset = 0, .range = GetTotalSize() })
		{
		}

	protected:
		VkBuffer mBuffer;
		VkDeviceMemory mBufferMemory;
		VkDescriptorBufferInfo mDescriptorBufferInfo;
	};
}