#include "3dgs/Renderer.h"

#include "3dgs/Window.h"

namespace iiixrlab
{
#if defined(_DEBUG)
	static void setDebugName(const char* name, const VkDevice device, const VkObjectType objectType, const void* object) noexcept;
#endif	// defined(_DEBUG)

	VkBool32 Renderer::DebugReportCallback(VkDebugReportFlagsEXT flags, [[maybe_unused]] VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, [[maybe_unused]] void* pUserData) noexcept
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

	VkBool32 Renderer::DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, [[maybe_unused]] void* pUserData) noexcept
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

	Renderer::Renderer(const RendererCreateInfo& createInfo) noexcept
		: mApiVersion(0)
		, mInstance(VK_NULL_HANDLE)
#if defined(_DEBUG)
		, mDebugReportCallback(VK_NULL_HANDLE)
		, mDebugUtilsMessenger(VK_NULL_HANDLE)
#endif	// defined(_DEBUG)
		, mSurface(VK_NULL_HANDLE)
		, mPhysicalDevice(VK_NULL_HANDLE)
		, mQueueFamilyProperties()
		, mMainQueueFamilyPropertyIndex(UINT32_MAX)
		, mDevice(VK_NULL_HANDLE)
		, mQueues()
		, mSwapChain(VK_NULL_HANDLE)
		, mCommandPool(VK_NULL_HANDLE)
		, mFramesCount()
	{
		VkResult vr = volkInitialize();
		assert(vr == VK_SUCCESS);

		initializeInstance(mInstance, mApiVersion, createInfo);
#if defined(_DEBUG)
		initializeDebugCallback(mDebugReportCallback, mDebugUtilsMessenger, mInstance);
#endif	// defined(_DEBUG)

		mPhysicalDevice = selectPhysicalDevice(mApiVersion, mInstance);
		selectMainQueueFamilyIndex(mQueueFamilyProperties, mMainQueueFamilyPropertyIndex, mPhysicalDevice, mApiVersion);
		mDevice = createDevice(mPhysicalDevice, mApiVersion, mQueueFamilyProperties);
		getQueues(mQueues, mDevice, mApiVersion, mMainQueueFamilyPropertyIndex, mQueueFamilyProperties);
		SwapChainCreateInfo swapChainCreateInfo =
		{
			.OutSwapChain = mSwapChain,
			.OutSurface = mSurface,
			.OutFramesCount = mFramesCount,
			.Window = createInfo.Window,
			.Instance = mInstance,
			.PhysicalDevice = mPhysicalDevice,
			.Device = mDevice,
			.ApiVersion = mApiVersion,
			.MainQueueFamilyPropertyIndex = mMainQueueFamilyPropertyIndex,
			.QueueFamilyProperties = mQueueFamilyProperties,
			.FramesCount = createInfo.FramesCount,
		};
		initializeSwapChain(swapChainCreateInfo);

		VkCommandPoolCreateInfo commandPoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = mMainQueueFamilyPropertyIndex,
		};
		vr = vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool);
		assert(vr == VK_SUCCESS && mCommandPool != VK_NULL_HANDLE);
#if defined(_DEBUG)
		setDebugName("Command Pool", mDevice, VK_OBJECT_TYPE_COMMAND_POOL, mCommandPool);
#endif	// defined(_DEBUG)

		mCommandBuffers.resize(mFramesCount);
		VkCommandBufferAllocateInfo commandBufferAllocateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = mFramesCount,
		};
		vr = vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, mCommandBuffers.data());
		assert(vr == VK_SUCCESS);

		for (uint32_t commandBufferIndex = 0; commandBufferIndex < mFramesCount; ++commandBufferIndex)
		{
			VkCommandBuffer commandBuffer = mCommandBuffers[commandBufferIndex];
			assert(commandBuffer != VK_NULL_HANDLE);
#if defined(_DEBUG)
			std::vector<char> commandBufferName(32);
			sprintf_s(commandBufferName.data(), commandBufferName.size(), "Command Buffer[%u]", commandBufferIndex);
			setDebugName(commandBufferName.data(), mDevice, VK_OBJECT_TYPE_COMMAND_BUFFER, commandBuffer);
#endif	// defined(_DEBUG)
		}
	}

	Renderer::~Renderer() noexcept
	{
		if (mCommandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
			mCommandPool = VK_NULL_HANDLE;
		}

		if (mSwapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
			mSwapChain = VK_NULL_HANDLE;
		}

		destroyDevice(mDevice, mQueues);

		if (mSurface != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
			mSurface = VK_NULL_HANDLE;
		}

#if defined(_DEBUG)
		destroyDebugCallback(mDebugReportCallback, mDebugUtilsMessenger, mInstance);
#endif	// defined(_DEBUG)

		if (mInstance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(mInstance, nullptr);
			mInstance = VK_NULL_HANDLE;
		}
	}

	void Renderer::Render() noexcept
	{
		
	}

	void Renderer::getSurfacePresentModes(std::vector<VkPresentModeKHR>& outSurfacePresentModes, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) noexcept
	{
		VkResult vr = VK_SUCCESS;
		uint32_t presentModesCount;
		vr = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
		assert(vr == VK_SUCCESS && presentModesCount > 0);

		outSurfacePresentModes.resize(presentModesCount);
		vr = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, outSurfacePresentModes.data());
		assert(vr == VK_SUCCESS);
	}

	void Renderer::logPhysicalDeviceProperties(const VkPhysicalDeviceProperties2& properties2, const bool bIsSelected /*= false*/) noexcept
	{
		std::cout << "Physical Device: ";
		if (bIsSelected == true)
		{
			std::cout << "\tSelected";
		}
		std::cout << "\n\tname: " << properties2.properties.deviceName << '\n'
			<< "\tapiVersion: "
			<< VK_VERSION_MAJOR(properties2.properties.apiVersion) << '.'
			<< VK_VERSION_MINOR(properties2.properties.apiVersion) << '.'
			<< VK_VERSION_PATCH(properties2.properties.apiVersion) << '\n'
			<< "\tdriverVersion: " << properties2.properties.driverVersion << '\n'
			<< "\tvendorID: ";

		switch (properties2.properties.vendorID)
		{
		case 0x1002:
			std::cout << "AMD" << '\n';
			break;
		case 0x1010:
			std::cout << "ImgTec" << '\n';
			break;
		case 0x10DE:
			std::cout << "NVIDIA" << '\n';
			break;
		case 0x13B5:
			std::cout << "ARM" << '\n';
			break;
		case 0x5143:
			std::cout << "Qualcomm" << '\n';
			break;
		case 0x8086:
			std::cout << "Intel" << '\n';
			break;
		case 0x8087:
			std::cout << "Imagination" << '\n';
			break;
		case 0x1AE0:
			std::cout << "Google" << '\n';
			break;
		case 0x1C5C:
			std::cout << "Microsoft" << '\n';
			break;
		default:
			assert(false);
			break;
		}

		std::cout << "\tdeviceID: " << properties2.properties.deviceID << '\n'
			<< "\tdeviceType: ";
		
		switch (properties2.properties.deviceType)
		{ 
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				std::cout << "Integrated GPU" << '\n';
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: 
				std::cout << "Discrete GPU" << '\n';
				break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				std::cout << "Virtual GPU" << '\n';
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				std::cout << "CPU" << '\n';
				break;
			default:
				assert(false);
				break;  
		};
	}

	void Renderer::logQueueFamilyProperties(const VkQueueFamilyProperties2& queueFamilyProperties2, const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyIndex, const bool bIsSelected /*= false*/) noexcept
	{
		const VkQueueFamilyProperties& queueFamilyProperties = queueFamilyProperties2.queueFamilyProperties;
		std::cout << "Queue Family[" << queueFamilyIndex << "]: ";
		if (bIsSelected == true)
		{
			std::cout << "\tSelected";
		}
		std::cout << '\n'
			<< "\tqueueCount: " << queueFamilyProperties.queueCount << '\n'
			<< "\tqueueFlags: " << std::hex << static_cast<uint32_t>(queueFamilyProperties.queueFlags) << '\t';
		
		const bool bIsGraphicsQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
		const bool bIsComputeQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
		const bool bIsTransferQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0;
		if (bIsGraphicsQueue == true)
		{
			std::cout << "[g]";
		}
		if (bIsComputeQueue == true)
		{
			std::cout << "[c]";
		}
		if (bIsTransferQueue == true)
		{
			std::cout << "[t]";
		}

		std::cout << '\n';

		bool bIsPresentationSupported = false;
#if defined(_WIN32)
		bIsPresentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
#elif defined(__linux__)
		bIsPresentationSupported = vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyIndex, nullptr);
#else
		assert(false);
#endif
		if (bIsPresentationSupported == true)
		{
			std::cout << "\tPresentation supported" << '\n';
		}

		std::cout << "\ttimestampValidBits: " << queueFamilyProperties.timestampValidBits << '\n'
		<< "\tminImageTransferGranularity: " << queueFamilyProperties.minImageTransferGranularity.width << ", " << queueFamilyProperties.minImageTransferGranularity.height << ", " << queueFamilyProperties.minImageTransferGranularity.depth << '\n';
	}

	constexpr uint32_t Renderer::scorePhysicalDevice(const uint32_t apiVersion, const VkPhysicalDeviceProperties2& properties2) noexcept
	{
		uint32_t score = 0;

		if (apiVersion >= properties2.properties.apiVersion && MINIMUM_VK_API_VERSION <= properties2.properties.apiVersion)
		{
			// Prefer discrete GPUs
			if (properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				score += 1000;
			}

			// Add score based on the amount of memory (in MB)
			score += properties2.properties.limits.maxMemoryAllocationCount / (1024 * 1024);

			// Add other criteria as needed
			// ...
		}

		return score;
	}

	void Renderer::initializeInstance(VkInstance& outInstance, uint32_t& outApiVersion, const RendererCreateInfo& createInfo) noexcept
	{
		outApiVersion = 0;
		VkResult vr = vkEnumerateInstanceVersion(&outApiVersion);
		assert(vr == VK_SUCCESS);
		assert(outApiVersion >= MINIMUM_VK_API_VERSION);
		std::cout << "Vulkan Instance Version: " << VK_API_VERSION_VARIANT(outApiVersion) << '.' << VK_API_VERSION_MAJOR(outApiVersion) << '.' << VK_API_VERSION_MINOR(outApiVersion) << '.' << VK_API_VERSION_PATCH(outApiVersion) << '\n';

		VkApplicationInfo applicationInfo =
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = createInfo.ApplicationInfo.Name.c_str(),
			.applicationVersion = createInfo.ApplicationInfo.Version,
			.pEngineName = createInfo.EngineInfo.Name.c_str(),
			.engineVersion = createInfo.EngineInfo.Version,
			.apiVersion = outApiVersion,
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

		std::vector<const char*> extensionNamesToEnable =
		{
#if defined(_DEBUG)
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif	// defined(_DEBUG)
			//VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME,
			//VK_EXT_VALIDATION_FLAGS_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
#error 0
#endif
			VK_KHR_SURFACE_EXTENSION_NAME,
		};

		if (outApiVersion < VK_API_VERSION_1_1)
		{
			extensionNamesToEnable.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		VkInstanceCreateInfo instanceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = pNext,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(extensionNamesToEnable.size()),
			.ppEnabledExtensionNames = extensionNamesToEnable.data(),
		};

		outInstance = VK_NULL_HANDLE;
		vr = vkCreateInstance(&instanceCreateInfo, nullptr, &outInstance);
		assert(vr == VK_SUCCESS);

		volkLoadInstance(outInstance);
	}

	VkPhysicalDevice Renderer::selectPhysicalDevice(const uint32_t apiVersion, VkInstance& instance) noexcept
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
			const uint32_t score = scorePhysicalDevice(apiVersion, physicalDeviceProperties2);
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
			logPhysicalDeviceProperties(physicalDeviceProperties2, physicalDevice == bestPhysicalDevice);
		}

		return bestPhysicalDevice;
	}

	void Renderer::selectMainQueueFamilyIndex(std::vector<VkQueueFamilyProperties2>& outQueueFamilyProperties, uint32_t& outMainQueueFamilyPropertyIndex, const VkPhysicalDevice physicalDevice, const uint32_t apiVersion) noexcept
	{
		uint32_t queueFamilyPropertyCount = 0;
		PFN_vkGetPhysicalDeviceQueueFamilyProperties2 pfnVkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
		pfnVkGetPhysicalDeviceQueueFamilyProperties2 = apiVersion >= VK_API_VERSION_1_1 ? vkGetPhysicalDeviceQueueFamilyProperties2 : vkGetPhysicalDeviceQueueFamilyProperties2KHR;
		assert(pfnVkGetPhysicalDeviceQueueFamilyProperties2 != nullptr);

		pfnVkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyPropertyCount, nullptr);
		assert(queueFamilyPropertyCount > 0);
		outMainQueueFamilyPropertyIndex = queueFamilyPropertyCount;
		outQueueFamilyProperties.resize(queueFamilyPropertyCount);
		pfnVkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyPropertyCount, outQueueFamilyProperties.data());

		for (uint32_t queueFamilyPropertiesIndex = 0; queueFamilyPropertiesIndex < queueFamilyPropertyCount; ++queueFamilyPropertiesIndex)
		{
			const VkQueueFamilyProperties2& queueFamilyProperties2 = outQueueFamilyProperties[queueFamilyPropertiesIndex];
			const VkQueueFamilyProperties& queueFamilyProperties = queueFamilyProperties2.queueFamilyProperties;
			
			const bool bIsGraphicsQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
			const bool bIsComputeQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
			const bool bIsTransferQueue = (queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0;

			bool bIsPresentationSupported = false;
#if defined(_WIN32)
			bIsPresentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyPropertiesIndex);
#elif defined(__linux__)
			bIsPresentationSupported = vkGetPhysicalDeviceXlibPresentationSupportKHR(physicalDevice, queueFamilyPropertiesIndex, nullptr);
#else
			assert(false);
#endif
			if (outMainQueueFamilyPropertyIndex >= queueFamilyPropertyCount && bIsGraphicsQueue && bIsComputeQueue && bIsTransferQueue && bIsPresentationSupported)
			{
				outMainQueueFamilyPropertyIndex = queueFamilyPropertiesIndex;
			}

			logQueueFamilyProperties(queueFamilyProperties2, physicalDevice, queueFamilyPropertiesIndex, queueFamilyPropertiesIndex == outMainQueueFamilyPropertyIndex);
		}
		assert(outMainQueueFamilyPropertyIndex < queueFamilyPropertyCount);
	}

	VkDevice Renderer::createDevice(const VkPhysicalDevice physicalDevice, const uint32_t apiVersion, const std::vector<VkQueueFamilyProperties2>& queueFamilyPropertiesList) noexcept
	{
		VkDevice device = VK_NULL_HANDLE;

		// pNext chain
		void* pNext = nullptr;
		VkPhysicalDeviceFeatures2 physicalDeviceFeatures2 =
		{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.pNext = pNext,
		};
		vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
		pNext = &physicalDeviceFeatures2;
		
		if (apiVersion >= VK_API_VERSION_1_4)
		{
			VkPhysicalDeviceVulkan14Features physicalDeviceVulkan14Features =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES,
				.pNext = pNext,
			};
			pNext = &physicalDeviceVulkan14Features;   
		}

		if (apiVersion >= VK_API_VERSION_1_3)
		{
			VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
				.pNext = pNext,
			};
			pNext = &physicalDeviceVulkan13Features;
		}
		
		if (apiVersion >= VK_API_VERSION_1_2)
		{
			VkPhysicalDeviceVulkan12Features physicalDeviceVulkan12Features =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
				.pNext = pNext,
			};
			pNext = &physicalDeviceVulkan12Features;
		}
		
		if (apiVersion >= VK_API_VERSION_1_1)
		{
			VkPhysicalDeviceVulkan11Features physicalDeviceVulkan11Features =
			{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES,
				.pNext = pNext,
			};
			pNext = &physicalDeviceVulkan11Features;
		}

		// Queue creation
		uint32_t queueCount = 0;
		const uint32_t queueFamilyPropertiesCount = static_cast<uint32_t>(queueFamilyPropertiesList.size());
		std::vector<std::vector<float>> normalizedPrioritiesPerFamilyProperty;
		normalizedPrioritiesPerFamilyProperty.reserve(queueFamilyPropertiesCount);
		std::vector<std::vector<uint32_t>> unnormalizedPrioritiesPerFamilyProperty;
		unnormalizedPrioritiesPerFamilyProperty.reserve(queueFamilyPropertiesCount);
		for (const VkQueueFamilyProperties2& queueFamilyProperties2 : queueFamilyPropertiesList)
		{
			const VkQueueFamilyProperties& queueFamilyProperties = queueFamilyProperties2.queueFamilyProperties;

			unnormalizedPrioritiesPerFamilyProperty.push_back(std::vector<uint32_t>());
			normalizedPrioritiesPerFamilyProperty.push_back(std::vector<float>());
			for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperties.queueCount; ++queueIndex)
			{
				unnormalizedPrioritiesPerFamilyProperty.back().push_back(queueFamilyProperties.queueCount - queueIndex);
			}
			unnormalizedPrioritiesPerFamilyProperty.back().reserve(unnormalizedPrioritiesPerFamilyProperty.back().size());
			queueCount += queueFamilyProperties.queueCount;
		}

		for (uint32_t queueFamilyPropertiesIndex = 0; queueFamilyPropertiesIndex < queueFamilyPropertiesCount; ++queueFamilyPropertiesIndex)
		{
			uint32_t prioritySum = 0;
			for (const uint32_t unnormalizedPriority : std::as_const(unnormalizedPrioritiesPerFamilyProperty[queueFamilyPropertiesIndex]))
			{
				prioritySum += unnormalizedPriority;
			}

			const uint32_t prioritiesCount = static_cast<uint32_t>(unnormalizedPrioritiesPerFamilyProperty[queueFamilyPropertiesIndex].size());
			for (uint32_t priorityIndex = 0; priorityIndex < prioritiesCount; ++priorityIndex)
			{
				normalizedPrioritiesPerFamilyProperty[queueFamilyPropertiesIndex].push_back(static_cast<float>(unnormalizedPrioritiesPerFamilyProperty[queueFamilyPropertiesIndex][priorityIndex]) / static_cast<float>(prioritySum));
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(queueFamilyPropertiesCount);
		for (uint32_t queueFamilyPropertyIndex = 0; queueFamilyPropertyIndex < queueFamilyPropertiesCount; ++queueFamilyPropertyIndex)
		{
			const VkQueueFamilyProperties2& queueFamilyProperties2 = queueFamilyPropertiesList[queueFamilyPropertyIndex];
			const VkQueueFamilyProperties& queueFamilyProperties = queueFamilyProperties2.queueFamilyProperties;
			
			VkDeviceQueueCreateInfo queueCreateInfo =
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.queueFamilyIndex = queueFamilyPropertyIndex,
				.queueCount = queueFamilyProperties.queueCount,
				.pQueuePriorities = normalizedPrioritiesPerFamilyProperty[queueFamilyPropertyIndex].data(),
			};
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Extensions
		std::vector<const char*> extensionNamesToEnable =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		VkDeviceCreateInfo deviceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = pNext,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(extensionNamesToEnable.size()),
			.ppEnabledExtensionNames = extensionNamesToEnable.data(),
			.pEnabledFeatures = nullptr,
		};
		VkResult vr = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
		assert(vr == VK_SUCCESS);

		volkLoadDevice(device);

		return device;
	}

	void Renderer::getQueues(std::vector<VkQueue>& outQueues, const VkDevice device, const uint32_t apiVersion, const uint32_t mainQueueFamilyPropertyIndex, const std::vector<VkQueueFamilyProperties2>& queueFamilyProperties) noexcept
	{
		for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperties[mainQueueFamilyPropertyIndex].queueFamilyProperties.queueCount; ++queueIndex)
		{
			VkQueue queue = VK_NULL_HANDLE;
			if (apiVersion >= VK_API_VERSION_1_1)
			{
				VkDeviceQueueInfo2 queueInfo2 =
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2,
					.pNext = nullptr,
					.flags = 0,
					.queueFamilyIndex = mainQueueFamilyPropertyIndex,
					.queueIndex = queueIndex
				};

				vkGetDeviceQueue2(device, &queueInfo2, &queue);
			}
			else
			{
				vkGetDeviceQueue(device, mainQueueFamilyPropertyIndex, queueIndex, &queue);
			}
			assert(queue != VK_NULL_HANDLE);
#if defined(_DEBUG)
			std::vector<char> queueName(32);
			sprintf_s(queueName.data(), 32, "Queue[%u]", queueIndex);
			setDebugName(queueName.data(), device, VK_OBJECT_TYPE_QUEUE, queue);
#endif	// defined(_DEBUG)

			outQueues.push_back(queue);
		}
	}

	void Renderer::initializeSwapChain(SwapChainCreateInfo& createInfo) noexcept
	{
		VkResult vr = VK_SUCCESS;
#if defined(_WIN32)
		VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.hinstance = createInfo.Window.GetInstance(),
			.hwnd = createInfo.Window.GetWindow(),
		};
		vr = vkCreateWin32SurfaceKHR(createInfo.Instance, &surfaceCreateInfo, nullptr, &createInfo.OutSurface);
#else	// NOT defined(_WIN32)
#error 0
#endif	// NOT defined(_WIN32)
		assert(vr == VK_SUCCESS && createInfo.OutSurface != VK_NULL_HANDLE);
#if defined(_DEBUG)
		setDebugName("Surface", createInfo.Device, VK_OBJECT_TYPE_SURFACE_KHR, createInfo.OutSurface);
#endif	// defined(_DEBUG)

		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		vr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(createInfo.PhysicalDevice, createInfo.OutSurface, &surfaceCapabilities);
		assert(vr == VK_SUCCESS);

		std::vector<VkPresentModeKHR> presentModes;
		getSurfacePresentModes(presentModes, createInfo.PhysicalDevice, createInfo.OutSurface);

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

		VkSwapchainCreateInfoKHR swapChainCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = nullptr,
			.flags = 0,
			.surface = createInfo.OutSurface,
			.minImageCount = std::clamp(createInfo.FramesCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount),
			.imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = VkExtent2D{ 
				.width = std::clamp(createInfo.Window.GetWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width), 
				.height = std::clamp(createInfo.Window.GetHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height) 
			},
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
		vr = vkCreateSwapchainKHR(createInfo.Device, &swapChainCreateInfo, nullptr, &createInfo.OutSwapChain);
		assert(vr == VK_SUCCESS && createInfo.OutSwapChain != VK_NULL_HANDLE);
#if defined(_DEBUG)
		setDebugName("Swap Chain", createInfo.Device, VK_OBJECT_TYPE_SWAPCHAIN_KHR, createInfo.OutSwapChain);					
#endif	// defined(_DEBUG)

		vr = vkGetSwapchainImagesKHR(createInfo.Device, createInfo.OutSwapChain, &createInfo.OutFramesCount, nullptr);
		assert(vr == VK_SUCCESS && createInfo.OutFramesCount > 0);
	}

	void Renderer::destroyDevice(VkDevice& device, std::vector<VkQueue>& queues) noexcept
	{
		if (device != VK_NULL_HANDLE)
		{
			queues.clear();

			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}
	}

#if defined(_DEBUG)
	void Renderer::initializeDebugCallback(VkDebugReportCallbackEXT& outCallback, VkDebugUtilsMessengerEXT& outMessenger, const VkInstance& instance) noexcept
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

	void Renderer::destroyDebugCallback(VkDebugReportCallbackEXT& callback, VkDebugUtilsMessengerEXT& messenger, VkInstance& instance) noexcept
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

	IIIXRLAB_INLINE void setDebugName(const char* name, const VkDevice device, const VkObjectType objectType, const void* object) noexcept
	{
		VkDebugUtilsObjectNameInfoEXT objectNameInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
			.pNext = nullptr,
			.objectType = objectType,
			.objectHandle = reinterpret_cast<uint64_t>(object),
			.pObjectName = name,
		};
		vkSetDebugUtilsObjectNameEXT(device, &objectNameInfo);
	}
#endif	// defined(_DEBUG)
}