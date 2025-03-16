#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;
	class Instance;

	class PhysicalDevice
	{
	public:
		friend class Instance;

	public:
		struct CreateInfo final
		{
			Instance&			Instance;
			VkPhysicalDevice	PhysicalDevice;
			VkPhysicalDeviceMemoryProperties	PhysicalDeviceMemoryProperties;
		};

	public:
		static void DestroyDevice(VkDevice& device) noexcept;
		static uint32_t GetMemoryTypeIndex(const uint32_t memoryTypeBits, VkMemoryPropertyFlags MemoryPropertyFlag, const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties) noexcept;
		static void GetSurfacePresentModes(std::vector<VkPresentModeKHR>& outSurfacePresentModes, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) noexcept;
		static void LogProperties(const VkPhysicalDeviceProperties2& properties2, const bool bIsSelected = false) noexcept;
		static constexpr uint32_t Score(const uint32_t apiVersion, const VkPhysicalDeviceProperties2& properties2) noexcept;

	public:
		PhysicalDevice() = delete;
		PhysicalDevice(const PhysicalDevice&) = delete;
		PhysicalDevice(PhysicalDevice&&) = delete;

		PhysicalDevice(CreateInfo& createInfo) noexcept;

		~PhysicalDevice() noexcept;

		PhysicalDevice& operator=(const PhysicalDevice&) = delete;
		PhysicalDevice& operator=(PhysicalDevice&&) = delete;

		IIIXRLAB_INLINE constexpr VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const noexcept { return mPhysicalDeviceMemoryProperties; }
		IIIXRLAB_INLINE constexpr uint32_t GetQueueFamilyIndex() const noexcept { return mQueueFamilyIndex; }
		IIIXRLAB_INLINE constexpr const VkQueueFamilyProperties2& GetQueueFamilyProperties() const noexcept { return mQueueFamilyProperties; }
		IIIXRLAB_INLINE Device& GetDevice() noexcept { return *mDevice; }
		IIIXRLAB_INLINE const Device& GetDevice() const noexcept { return *mDevice; }
		IIIXRLAB_INLINE const Instance& GetInstance() const noexcept { return mInstance; }

	private:
		static VkDevice createDevice(const VkPhysicalDevice physicalDevice, const uint32_t apiVersion, const std::vector<VkQueueFamilyProperties2>& queueFamilyPropertiesList) noexcept;
		static void logQueueFamilyProperties(const VkQueueFamilyProperties2& queueFamilyProperties2, const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyIndex, const bool bIsSelected = false) noexcept;
		static void selectMainQueueFamilyIndex(std::vector<VkQueueFamilyProperties2>& outQueueFamilyProperties, uint32_t& outMainQueueFamilyPropertyIndex, const VkPhysicalDevice physicalDevice, const uint32_t apiVersion) noexcept;

	private:
		Instance&           mInstance;
		VkPhysicalDevice    mPhysicalDevice;
		VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
		uint32_t                    mQueueFamilyIndex;
		VkQueueFamilyProperties2    mQueueFamilyProperties;
		std::unique_ptr<Device>	mDevice;
	};
} // namespace iiixrlab
