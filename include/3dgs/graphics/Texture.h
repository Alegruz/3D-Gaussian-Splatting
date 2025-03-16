#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;

	class Texture final
	{
	public:
		enum class eUsageType : uint8_t
		{
			UNKNOWN         	= 0x0,
			SAMPLED         	= 0x1,
			STORAGE         	= 0x2,
			COLOR_ATTACHMENT	= 0x4,
			DEPTH_STENCIL		= 0x8,
		};

		struct CreateInfo final
		{
			Device& Device;
			VkImage Image;
			VkFormat Format;
			VkDeviceMemory DeviceMemory;

			VkImageView SampledViewOrNull;
			VkImageView StorageViewOrNull;
			VkImageView ColorAttachmentViewOrNull;
			VkImageView DepthAttachmentViewOrNull;

			bool bIsBackBuffer = false;
		};

	public:
		Texture() = delete;
		Texture(const Texture&) = delete;
		constexpr Texture(Texture&& other) noexcept;

		Texture(CreateInfo& createInfo) noexcept;

		~Texture() noexcept;

		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&& other) noexcept;

		IIIXRLAB_INLINE constexpr VkImage GetImage() const noexcept { return mImage; }
		IIIXRLAB_INLINE constexpr VkFormat GetFormat() const noexcept { return mFormat; }
		IIIXRLAB_INLINE constexpr VkDeviceMemory GetDeviceMemory() const noexcept { return mDeviceMemory; }

		IIIXRLAB_INLINE constexpr VkImageView GetSampledViewOrNull() const noexcept { return mSampledViewOrNull; }
		IIIXRLAB_INLINE constexpr VkImageView GetStorageViewOrNull() const noexcept { return mStorageViewOrNull; }
		IIIXRLAB_INLINE constexpr VkImageView GetColorAttachmentViewOrNull() const noexcept { return mColorAttachmentViewOrNull; }
		IIIXRLAB_INLINE constexpr VkImageView GetDepthStencilViewOrNull() const noexcept { return mDepthAttachmentViewOrNull; }
	
	private:
		Device& mDevice;
		VkImage mImage;
		VkFormat mFormat;
		VkDeviceMemory mDeviceMemory;

		VkImageView mSampledViewOrNull;
		VkImageView mStorageViewOrNull;
		VkImageView mColorAttachmentViewOrNull;
		VkImageView mDepthAttachmentViewOrNull;

		bool mbIsBackBuffer;
	};
} // namespace iiixrlab
