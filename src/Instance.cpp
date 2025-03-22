#include "3dgs/graphics/Instance.h"

#include "3dgs/Window.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/PhysicalDevice.hpp"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"

namespace iiixrlab::graphics
{
	class InstanceExtensionBuilder final : public IExtensionBuilder
	{
	public:
		InstanceExtensionBuilder() noexcept;
		~InstanceExtensionBuilder() noexcept = default;

		InstanceExtensionBuilder& operator=(const InstanceExtensionBuilder&) = delete;
		InstanceExtensionBuilder& operator=(InstanceExtensionBuilder&&) = delete;
	};

	bool IExtensionBuilder::AddExtension(const std::string& extension) noexcept
	{
		if (mAvailableExtensions.find(extension) == mAvailableExtensions.end())
		{
			std::cerr << "Extension: " << extension << " is not available.\n";
			IIIXRLAB_DEBUG_BREAK();
			return false;
		}

		if (mExtensionsToEnableMap.find(extension) != mExtensionsToEnableMap.end())
		{
			return false;
		}

		auto insertResult = mExtensionsToEnableMap.insert(extension);
		mExtensionsToEnable.push_back(insertResult.first->c_str());
		return true;
	}

	InstanceExtensionBuilder::InstanceExtensionBuilder() noexcept
	{
		uint32_t extensionCount = 0;
		VkResult vr = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		assert(vr == VK_SUCCESS && extensionCount > 0);

		std::vector<VkExtensionProperties> extensionProperties(extensionCount);
		vr = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());
		assert(vr == VK_SUCCESS);

		for (const VkExtensionProperties& extensionProperty : extensionProperties)
		{	
			mAvailableExtensions.insert(extensionProperty.extensionName);
		}

		mExtensionsToEnable.reserve(extensionCount);
		mExtensionsToEnableMap.reserve(extensionCount);
	}

	static constexpr void FilterMessages(bool& bInoutBreak, bool& bInoutPrintMessage, const char* message) noexcept
	{
		if (bInoutBreak == true || bInoutPrintMessage == true)
		{
			if (strstr(message, "#LLP_LAYER_3") != nullptr)
			{
				if (strstr(message, "GalaxyOverlayVkLayer") != nullptr)
				{
					bInoutPrintMessage = false;
					bInoutBreak = false;
				}
			}

			if (strstr(message, "uses API version") != nullptr 
			&& strstr(message, "which is older than the application specified API version of ") != nullptr 
			&& strstr(message, "May cause issues.") != nullptr)
			{
				bInoutPrintMessage = false;
				bInoutBreak = false;
			}
		}
	}

	VkBool32 Instance::DebugReportCallback(VkDebugReportFlagsEXT flags, [[maybe_unused]] VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, [[maybe_unused]] void* pUserData) noexcept
	{
		bool bBreak = false;
		bool bPrintMessage = false;

		switch (flags)
		{
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
			break;
		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
			bBreak = true;
			bPrintMessage = true;
			break;
		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
			bPrintMessage = true;
			break;
		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
			bBreak = true;
			bPrintMessage = true;
			break;
		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			bPrintMessage = true;
			break;
		case VK_DEBUG_REPORT_FLAG_BITS_MAX_ENUM_EXT:
			[[fallthrough]];
		default:
			assert(false);
			break;
		}
		
		FilterMessages(bBreak, bPrintMessage, pMessage);

		if (bPrintMessage == true)
		{
			std::cout << std::hex << "object: " << object << " location: " << location << " messageCode: " << messageCode << " layerPrefix: " << pLayerPrefix << " message: " << pMessage << '\n';
		}
		
		if (bBreak == true)
		{
			IIIXRLAB_DEBUG_BREAK();
		}

		return VK_TRUE;
	}

	VkBool32 Instance::DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) noexcept
	{
		bool bBreak = false;
		bool bPrintMessage = false;
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			bBreak = true;
			bPrintMessage = true;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			bBreak = true;
			bPrintMessage = true;
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			[[fallthrough]];
		default:
			assert(false);
			break;
		}

		switch (messageTypes)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
			[[fallthrough]];
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
			[[fallthrough]];
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
			[[fallthrough]];
		case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT:
			[[fallthrough]];
		default:
			assert(false);
			break;
		}
		
		if (pCallbackData != nullptr)
		{
			FilterMessages(bBreak, bPrintMessage, pCallbackData->pMessage);
		}

		if (bPrintMessage == true)
		{
			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				std::cout << "[VERBOSE]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				std::cout << "[INFO]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				std::cout << "[WARNING]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				std::cout << "[SEVERITY]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
				[[fallthrough]];
			default:
				assert(false);
				break;
			}

			if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			{
				//std::cout << "[GENERAL]";
			}
			if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			{
				//std::cout << "[VALIDATION]";
				bPrintMessage = true;
			}
			if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			{
				//std::cout << "[PERFORMANCE]";
				bPrintMessage = true;
			}
			if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
			{
				assert(false);
			}

			assert(pCallbackData != nullptr);
			std::cout << "MessageId: [" << pCallbackData->pMessageIdName << ", #: " << pCallbackData->messageIdNumber << "] Message:" << pCallbackData->pMessage << '\n';
			for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i)
			{
				const VkDebugUtilsLabelEXT& queueLabel = pCallbackData->pQueueLabels[i];
				std::cout << "\tQueueLabel: " << queueLabel.pLabelName << '\n';
			}
			for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i)
			{
				const VkDebugUtilsLabelEXT& commandBufferLabel = pCallbackData->pCmdBufLabels[i];
				std::cout << "\tCommandBufferLabel: " << commandBufferLabel.pLabelName << '\n';
			}
			for (uint32_t i = 0; i < pCallbackData->objectCount; ++i)
			{
				const VkDebugUtilsObjectNameInfoEXT& object = pCallbackData->pObjects[i];
				std::cout << "\tObjectType: " << object.objectType << std::hex << " Handle: " << object.objectHandle << " Name: " << (object.pObjectName ? object.pObjectName : "") << '\n';
			}
		}

		if (bBreak == true)
		{
			IIIXRLAB_DEBUG_BREAK();
		}

		return VK_TRUE;
	}


	Instance::Instance(const CreateInfo& createInfo) noexcept
		: mInstance(VK_NULL_HANDLE)
		, mApiVersion(0)
		, mPhysicalDevice()
#if defined(_DEBUG)
		, mDebugReportCallback(VK_NULL_HANDLE)
		, mDebugUtilsMessenger(VK_NULL_HANDLE)
#endif	// defined(_DEBUG)
	{
		VkResult vr = volkInitialize();
		assert(vr == VK_SUCCESS);

		vr = vkEnumerateInstanceVersion(&mApiVersion);
		assert(vr == VK_SUCCESS);
		assert(mApiVersion >= MINIMUM_VK_API_VERSION);
		std::cout << "Vulkan Instance Version: " << VK_API_VERSION_VARIANT(mApiVersion) << '.' << VK_API_VERSION_MAJOR(mApiVersion) << '.' << VK_API_VERSION_MINOR(mApiVersion) << '.' << VK_API_VERSION_PATCH(mApiVersion) << '\n';

		VkApplicationInfo applicationInfo =
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = createInfo.ApplicationInfo.Name.c_str(),
			.applicationVersion = createInfo.ApplicationInfo.Version,
			.pEngineName = createInfo.EngineInfo.Name.c_str(),
			.engineVersion = createInfo.EngineInfo.Version,
			.apiVersion = mApiVersion,
		};

		std::vector<VkValidationFeatureEnableEXT> validationFeaturesToEnable =
		{
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
		};

		void* pNext = nullptr;
		//VkValidationFeaturesEXT validationFeatures =
		//{
		//	.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
		//	.pNext = pNext,
		//	.enabledValidationFeatureCount = static_cast<uint32_t>(validationFeaturesToEnable.size()),
		//	.pEnabledValidationFeatures = validationFeaturesToEnable.data(),
		//	.disabledValidationFeatureCount = 0,
		//	.pDisabledValidationFeatures = nullptr,
		//};
		// pNext = &validationFeatures;

#if defined(_DEBUG)
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			//.pNext = &validationFeatures,
			.pNext = pNext,
			.flags = 0,
			.messageSeverity = (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
			.messageType = (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
			.pfnUserCallback = DebugUtilsMessengerCallback,
			.pUserData = nullptr,
		};
		pNext = &debugUtilsMessengerCreateInfo;

		VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			.pNext = pNext,
			.flags = (VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT),
			.pfnCallback = DebugReportCallback,
			.pUserData = nullptr,
		};
		pNext = &debugReportCallbackCreateInfo;
#endif	// defined(_DEBUG)

		std::vector<const char*> layerNamesToEnable =
		{
#if defined(_DEBUG)
			"VK_LAYER_KHRONOS_validation",
#endif	// defined(_DEBUG)
		};

		InstanceExtensionBuilder instanceExtensionBuilder;
#if defined(_DEBUG)
		instanceExtensionBuilder.AddExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		instanceExtensionBuilder.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif	// defined(_DEBUG)
		// instanceExtensionBuilder.AddExtension(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
		// instanceExtensionBuilder.AddExtension(VK_EXT_VALIDATION_FLAGS_EXTENSION_NAME);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		instanceExtensionBuilder.AddExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error 0
#endif
		instanceExtensionBuilder.AddExtension(VK_KHR_SURFACE_EXTENSION_NAME);

		const std::vector<const char*>& extensionNamesToEnable = instanceExtensionBuilder.GetExtensionsToEnable();

		if (mApiVersion < VK_API_VERSION_1_1)
		{
			instanceExtensionBuilder.AddExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		VkInstanceCreateInfo instanceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = pNext,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(layerNamesToEnable.size()),
			.ppEnabledLayerNames = layerNamesToEnable.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensionNamesToEnable.size()),
			.ppEnabledExtensionNames = extensionNamesToEnable.data(),
		};

		vr = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
		assert(vr == VK_SUCCESS);

		volkLoadInstance(mInstance);

#if defined(_DEBUG)
		initializeDebugCallback(mDebugReportCallback, mDebugUtilsMessenger, mInstance);
#endif	// defined(_DEBUG)

		PhysicalDevice::CreateInfo physicalDeviceCreateInfo =
		{
			.Instance = *this,
		};

		physicalDeviceCreateInfo.PhysicalDevice = selectPhysicalDevice(physicalDeviceCreateInfo.PhysicalDeviceMemoryProperties, mApiVersion, mInstance);
		mPhysicalDevice = std::make_unique<PhysicalDevice>(physicalDeviceCreateInfo);
	}

	Instance::~Instance() noexcept
	{
		mSwapChain.reset();
		mPhysicalDevice.reset();

#if defined(_DEBUG)
		destroyDebugCallback(mDebugReportCallback, mDebugUtilsMessenger, mInstance);
#endif	// defined(_DEBUG)

		if (mInstance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(mInstance, nullptr);
			mInstance = VK_NULL_HANDLE;
		}
	}

	void Instance::DestroySurface(VkSurfaceKHR& surface) noexcept
	{
		if (surface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(mInstance, surface, nullptr);
			surface = VK_NULL_HANDLE;
		}
	}

	SwapChain& Instance::InitializeSwapChain(const uint32_t framesCount, const iiixrlab::Window& window) noexcept
	{
		VkResult vr = VK_SUCCESS;

		VkPhysicalDevice& vkPhysicalDevice = mPhysicalDevice->mPhysicalDevice;
		Device& device = *mPhysicalDevice->mDevice;
		VkDevice& vkDevice = device.mDevice;
		
		SwapChain::CreateInfo createInfo = 
		{
			.Instance = *this,
			.Device = *mPhysicalDevice->mDevice,
			.FramesCount = framesCount,	
		};

#if defined(_WIN32)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.hinstance = window.GetInstance(),
			.hwnd = window.GetWindow(),
		};
		vr = vkCreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, nullptr, &createInfo.Surface);
#else	// NOT defined(_WIN32)
#error 0
#endif	// NOT defined(_WIN32)
		assert(vr == VK_SUCCESS && createInfo.Surface != VK_NULL_HANDLE);
#if defined(_DEBUG)
		device.SetDebugName("Surface", VK_OBJECT_TYPE_SURFACE_KHR, createInfo.Surface);
#endif	// defined(_DEBUG)

		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		vr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, createInfo.Surface, &surfaceCapabilities);
		assert(vr == VK_SUCCESS);

		std::vector<VkPresentModeKHR> presentModes;
		PhysicalDevice::GetSurfacePresentModes(presentModes, vkPhysicalDevice, createInfo.Surface);

		uint32_t mainPresentModeIndex = UINT32_MAX;
		const uint32_t presentModesCount = static_cast<uint32_t>(presentModes.size());
		for (uint32_t presentModeIndex = 0; presentModeIndex < presentModesCount; ++presentModeIndex)
		{
			const VkPresentModeKHR& presentMode = presentModes[presentModeIndex];

			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				mainPresentModeIndex = presentModeIndex;
			}
		}
		assert(mainPresentModeIndex < presentModesCount);

		createInfo.FrameExtent = VkExtent3D{ 
			.width = std::clamp(window.GetWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width), 
			.height = std::clamp(window.GetHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height), 
			.depth = 1,
		};

		VkSwapchainCreateInfoKHR swapChainCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = createInfo.Surface,
			.minImageCount = std::clamp(createInfo.FramesCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount),
			.imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = VkExtent2D{ .width = createInfo.FrameExtent.width, .height = createInfo.FrameExtent.height },
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = presentModes[mainPresentModeIndex],
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE,
		};
		vr = vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &createInfo.SwapChain);
		assert(vr == VK_SUCCESS && createInfo.SwapChain != VK_NULL_HANDLE);
#if defined(_DEBUG)
		device.SetDebugName("Swap Chain", VK_OBJECT_TYPE_SWAPCHAIN_KHR, createInfo.SwapChain);					
#endif	// defined(_DEBUG)

		vr = vkGetSwapchainImagesKHR(vkDevice, createInfo.SwapChain, &createInfo.FramesCount, nullptr);
		assert(vr == VK_SUCCESS && createInfo.FramesCount > 0);
		
		std::vector<VkImage> backBuffers(createInfo.FramesCount);
		vr = vkGetSwapchainImagesKHR(vkDevice, createInfo.SwapChain, &createInfo.FramesCount, backBuffers.data());

		std::vector<char> backBufferName(64);
		std::vector<char> depthBufferName(64);

		for (uint32_t frameIndex = 0; frameIndex < createInfo.FramesCount; ++frameIndex)
		{
#if defined(_DEBUG)
			sprintf_s(backBufferName.data(), backBufferName.size(), "Back Buffer[%u]", frameIndex);
#endif	// defined(_DEBUG)

			TextureCreateInfo backBufferCreateInfo =
			{
#if defined(_DEBUG)
				.Name = backBufferName.data(),
#endif	// defined(_DEBUG)
				.Format = VK_FORMAT_B8G8R8A8_SRGB,
				.Usage = static_cast<uint8_t>(Texture::eUsageType::COLOR_ATTACHMENT),
				.Extent = createInfo.FrameExtent,
				.ImageOrNull = backBuffers[frameIndex],
			};

#if defined(_DEBUG)
			sprintf_s(depthBufferName.data(), depthBufferName.size(), "Depth Buffer[%u]", frameIndex);
#endif	// defined(_DEBUG)

			TextureCreateInfo depthBufferCreateInfo =
			{
#if defined(_DEBUG)
				.Name = depthBufferName.data(),
#endif	// defined(_DEBUG)
				.Format = VK_FORMAT_D32_SFLOAT,
				.Usage = static_cast<uint8_t>(Texture::eUsageType::DEPTH_STENCIL),
				.Extent = createInfo.FrameExtent,
			};

			createInfo.BackBuffers.push_back(
				BackBuffer
				{
					.Color = device.CreateTexture(backBufferCreateInfo), 
					.Depth = device.CreateTexture(depthBufferCreateInfo),
				}
			);
		}

		mSwapChain = std::make_unique<SwapChain>(createInfo);
		return *mSwapChain;
	}

	VkPhysicalDevice Instance::selectPhysicalDevice(VkPhysicalDeviceMemoryProperties& outPhysicalDeviceMemoryProperties, const uint32_t apiVersion, VkInstance& instance) noexcept
	{
		uint32_t numPhysicalDevices = 0;
		vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
		vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data());

		VkPhysicalDevice bestPhysicalDevice = VK_NULL_HANDLE;
		uint32_t bestScore = 0;
		for (const VkPhysicalDevice& physicalDevice : physicalDevices)
		{
			void* pNext = nullptr;
			VkPhysicalDeviceProperties2 physicalDeviceProperties2 =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
				.pNext = pNext,
			};

			vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
			const uint32_t score = PhysicalDevice::Score(apiVersion, physicalDeviceProperties2);
			if (score > bestScore)
			{
				bestScore = score;
				bestPhysicalDevice = physicalDevice;
			}
		}

		assert(bestScore != 0);
		assert(bestPhysicalDevice != VK_NULL_HANDLE);

		void* pNext = nullptr;
		VkPhysicalDeviceProperties2 bestPhysicalDeviceProperties2 =
		{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
			.pNext = pNext,
		};
		vkGetPhysicalDeviceProperties2(bestPhysicalDevice, &bestPhysicalDeviceProperties2);

		for (const VkPhysicalDevice& physicalDevice : physicalDevices)
		{
			pNext = nullptr;
			VkPhysicalDeviceProperties2 physicalDeviceProperties2 =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
				.pNext = pNext,
			};

			vkGetPhysicalDeviceProperties2(physicalDevice, &physicalDeviceProperties2);
			PhysicalDevice::LogProperties(physicalDeviceProperties2, physicalDevice == bestPhysicalDevice);
		}

		vkGetPhysicalDeviceMemoryProperties(bestPhysicalDevice, &outPhysicalDeviceMemoryProperties);

		return bestPhysicalDevice;
	}

#if defined(_DEBUG)
	void Instance::initializeDebugCallback(VkDebugReportCallbackEXT& outCallback, VkDebugUtilsMessengerEXT& outMessenger, const VkInstance& instance) noexcept
	{
		VkResult vr = VK_SUCCESS;
		
		outCallback = VK_NULL_HANDLE;
		VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = (VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT),
			.pfnCallback = DebugReportCallback,
			.pUserData = nullptr,
		};
		vr = vkCreateDebugReportCallbackEXT(instance, &debugReportCallbackCreateInfo, nullptr, &outCallback);
		assert(vr == VK_SUCCESS && outCallback != VK_NULL_HANDLE);

		outMessenger = VK_NULL_HANDLE;
		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = 0,
			.messageSeverity = (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT),
			.messageType = (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT),
			.pfnUserCallback = DebugUtilsMessengerCallback,
			.pUserData = nullptr,
		};
		vr = vkCreateDebugUtilsMessengerEXT(instance, &debugUtilsMessengerCreateInfo, nullptr, &outMessenger);
		assert(vr == VK_SUCCESS && outMessenger != VK_NULL_HANDLE);
	}

	void Instance::destroyDebugCallback(VkDebugReportCallbackEXT& callback, VkDebugUtilsMessengerEXT& messenger, VkInstance& instance) noexcept
	{
		if (callback != VK_NULL_HANDLE)
		{
			vkDestroyDebugReportCallbackEXT(instance, callback, nullptr);
			callback = VK_NULL_HANDLE;
		}

		if (messenger != VK_NULL_HANDLE)
		{
			vkDestroyDebugUtilsMessengerEXT(instance, messenger, nullptr);
			messenger = VK_NULL_HANDLE;
		}
	}
#endif	// defined(_DEBUG)
} // namespace iiixrlab
