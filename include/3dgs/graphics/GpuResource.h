#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;
	
	class GpuResource
	{
	public:
		struct CreateInfo final
		{
			Device& Device;
			const char* Name;
			uint8_t* Data;
			uint32_t Size;
			uint32_t Stride;
		};

	public:
		GpuResource() = delete;

		GpuResource(const GpuResource&) = delete;
		GpuResource& operator=(const GpuResource&) = delete;

		~GpuResource() noexcept = default;

		IIIXRLAB_INLINE constexpr GpuResource(GpuResource&& other) noexcept
			: mDevice(other.mDevice)
			, mName(other.mName)
			, mSize(other.mSize)
			, mStride(other.mStride)
		{
			other.mName = nullptr;
			other.mSize = 0;
			other.mStride = 0;
		}
		GpuResource& operator=(GpuResource&&) = delete;

		IIIXRLAB_INLINE constexpr const char* GetName() const noexcept { return mName; }
		IIIXRLAB_INLINE constexpr uint32_t GetSize() const noexcept { return mSize; }
		IIIXRLAB_INLINE constexpr uint32_t GetStride() const noexcept { return mStride; }
		IIIXRLAB_INLINE constexpr uint32_t GetTotalSize() const noexcept { return mSize * mStride; }

	protected:
		IIIXRLAB_INLINE constexpr GpuResource(const CreateInfo& createInfo) noexcept
			: mDevice(createInfo.Device)
			, mName(createInfo.Name)
			, mSize(createInfo.Size)
			, mStride(createInfo.Stride)
		{
		}

	protected:
		Device& mDevice;
		const char* mName;
		uint32_t mSize;
		uint32_t mStride;
	};
} // namespace iiixrlab::graphics
