#include "3dgs/graphics/Texture.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/PhysicalDevice.h"

namespace iiixrlab::graphics
{
	constexpr Texture::Texture(Texture&& other) noexcept
		: mDevice(other.mDevice)
		, mImage(other.mImage)
		, mFormat(other.mFormat)
		, mDeviceMemory(other.mDeviceMemory)
		, mSampledViewOrNull(other.mSampledViewOrNull)
		, mStorageViewOrNull(other.mStorageViewOrNull)
		, mColorAttachmentViewOrNull(other.mColorAttachmentViewOrNull)
		, mDepthAttachmentViewOrNull(other.mDepthAttachmentViewOrNull)
		, mbIsBackBuffer(other.mbIsBackBuffer)
	{
		other.mImage = VK_NULL_HANDLE;
		other.mFormat = VK_FORMAT_UNDEFINED;
		other.mDeviceMemory = VK_NULL_HANDLE;
		other.mSampledViewOrNull = VK_NULL_HANDLE;
		other.mStorageViewOrNull = VK_NULL_HANDLE;
		other.mColorAttachmentViewOrNull = VK_NULL_HANDLE;
		other.mDepthAttachmentViewOrNull = VK_NULL_HANDLE;
		other.mbIsBackBuffer = false;
	}

	Texture::Texture(CreateInfo& createInfo) noexcept
		: mDevice(createInfo.Device)
		, mImage(createInfo.Image)
		, mFormat(createInfo.Format)
		, mDeviceMemory(createInfo.DeviceMemory)
		, mSampledViewOrNull(createInfo.SampledViewOrNull)
		, mStorageViewOrNull(createInfo.StorageViewOrNull)
		, mColorAttachmentViewOrNull(createInfo.ColorAttachmentViewOrNull)
		, mDepthAttachmentViewOrNull(createInfo.DepthAttachmentViewOrNull)
		, mbIsBackBuffer(createInfo.bIsBackBuffer)
	{
	}

	Texture::~Texture() noexcept
	{
		mDevice.DestroyImageView(mSampledViewOrNull);
		mDevice.DestroyImageView(mStorageViewOrNull);
		mDevice.DestroyImageView(mColorAttachmentViewOrNull);
		mDevice.DestroyImageView(mDepthAttachmentViewOrNull);
		mDevice.FreeMemory(mDeviceMemory);
		if (mbIsBackBuffer == false)
		{
			mDevice.DestroyImage(mImage);
		}
	}
}