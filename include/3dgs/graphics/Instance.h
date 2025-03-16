#pragma once

#include "pch.h"

namespace iiixrlab
{
	class Window;

	struct ProjectInfo;
}

namespace iiixrlab::graphics
{
	class PhysicalDevice;
	class SwapChain;

	class Instance final
	{
	public:
		struct CreateInfo final
		{
			const iiixrlab::ProjectInfo& ApplicationInfo;
			const iiixrlab::ProjectInfo& EngineInfo;
		};

	public:
		static VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) noexcept;
		static VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept;

	public:
		Instance() = delete;
		Instance(const Instance&) = delete;
		Instance(Instance&&) = delete;

		Instance(const CreateInfo& createInfo) noexcept;

		~Instance() noexcept;

		Instance& operator=(const Instance&) = delete;
		Instance& operator=(Instance&&) = delete;

		IIIXRLAB_INLINE constexpr uint32_t GetApiVersion() const noexcept { return mApiVersion; }
		IIIXRLAB_INLINE PhysicalDevice& GetPhysicalDevice() noexcept { return *mPhysicalDevice; }
		IIIXRLAB_INLINE const PhysicalDevice& GetPhysicalDevice() const noexcept { return *mPhysicalDevice; }
		IIIXRLAB_INLINE SwapChain& GetSwapChain() noexcept { return *mSwapChain; }
		IIIXRLAB_INLINE const SwapChain& GetSwapChain() const noexcept { return *mSwapChain; }

		void DestroySurface(VkSurfaceKHR& surface) noexcept;
		SwapChain& InitializeSwapChain(const uint32_t framesCount, const Window& window) noexcept;
	
	private:
		static VkPhysicalDevice selectPhysicalDevice(VkPhysicalDeviceMemoryProperties& outPhysicalDeviceMemoryProperties, const uint32_t apiVersion, VkInstance& instance) noexcept;

#if defined(_DEBUG)
		static void initializeDebugCallback(VkDebugReportCallbackEXT& outCallback, VkDebugUtilsMessengerEXT& outMessenger, const VkInstance& instance) noexcept;
		static void destroyDebugCallback(VkDebugReportCallbackEXT& callback, VkDebugUtilsMessengerEXT& messenger, VkInstance& instance) noexcept;
#endif  // defined(_DEBUG)

	private:
		VkInstance mInstance;
		uint32_t mApiVersion;

		std::unique_ptr<PhysicalDevice>	mPhysicalDevice;
		std::unique_ptr<SwapChain> mSwapChain;

#if defined(_DEBUG)
		VkDebugReportCallbackEXT mDebugReportCallback;
		VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
#endif  // defined(_DEBUG)
	};
}