#include "3dgs/graphics/PhysicalDevice.hpp"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/Instance.h"

namespace iiixrlab::graphics
{
    PhysicalDevice::PhysicalDevice(CreateInfo& createInfo) noexcept
        : mInstance(createInfo.Instance)
        , mPhysicalDevice(createInfo.PhysicalDevice)
        , mPhysicalDeviceMemoryProperties(createInfo.PhysicalDeviceMemoryProperties)
        , mQueueFamilyIndex(UINT32_MAX)
        , mQueueFamilyProperties()
		, mDevice()
    {
        assert(mPhysicalDevice != VK_NULL_HANDLE);

		std::vector<VkQueueFamilyProperties2> queueFamilyPropertiesList;
		selectMainQueueFamilyIndex(queueFamilyPropertiesList, mQueueFamilyIndex, mPhysicalDevice, mInstance.GetApiVersion());
		mQueueFamilyProperties = queueFamilyPropertiesList[mQueueFamilyIndex];

		Device::CreateInfo deviceCreateInfo =
		{
			.PhysicalDevice = *this,
		};
		deviceCreateInfo.Device = createDevice(mPhysicalDevice, mInstance.GetApiVersion(), queueFamilyPropertiesList);
		mDevice = std::make_unique<Device>(deviceCreateInfo);
    }

	PhysicalDevice::~PhysicalDevice() noexcept
	{
		mDevice.reset();
		mPhysicalDevice = VK_NULL_HANDLE;
	}
	
	void PhysicalDevice::DestroyDevice(VkDevice& device) noexcept
	{
		if (device != VK_NULL_HANDLE)
		{
			vkDestroyDevice(device, nullptr);
			device = VK_NULL_HANDLE;
		}
	}

	uint32_t PhysicalDevice::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkMemoryPropertyFlags MemoryPropertyFlag, const VkPhysicalDeviceMemoryProperties& physicalDeviceMemoryProperties) noexcept
	{
		// Search memory types to find first index with those properties
		for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < physicalDeviceMemoryProperties.memoryTypeCount; ++memoryTypeIndex)
		{
			if ((memoryTypeBits & 1) == 1)
			{
				// Type is available, does it match user properties?
				if ((physicalDeviceMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & MemoryPropertyFlag) == MemoryPropertyFlag)
				{
					return memoryTypeIndex;
				}
			}
			memoryTypeBits >>= 1;
		}
		assert(false);
		return physicalDeviceMemoryProperties.memoryTypeCount;
	}

	void PhysicalDevice::GetSurfacePresentModes(std::vector<VkPresentModeKHR>& outSurfacePresentModes, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) noexcept
	{
		VkResult vr = VK_SUCCESS;
		uint32_t presentModesCount;
		vr = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
		assert(vr == VK_SUCCESS && presentModesCount > 0);

		outSurfacePresentModes.resize(presentModesCount);
		vr = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, outSurfacePresentModes.data());
		assert(vr == VK_SUCCESS);
	}

	void PhysicalDevice::LogProperties(const VkPhysicalDeviceProperties2& properties2, const bool bIsSelected /*= false*/) noexcept
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

	VkDevice PhysicalDevice::createDevice(const VkPhysicalDevice physicalDevice, const uint32_t apiVersion, const std::vector<VkQueueFamilyProperties2>& queueFamilyPropertiesList) noexcept
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

	void PhysicalDevice::logQueueFamilyProperties(const VkQueueFamilyProperties2& queueFamilyProperties2, const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyIndex, const bool bIsSelected /*= false*/) noexcept
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

	void PhysicalDevice::selectMainQueueFamilyIndex(std::vector<VkQueueFamilyProperties2>& outQueueFamilyProperties, uint32_t& outMainQueueFamilyPropertyIndex, const VkPhysicalDevice physicalDevice, const uint32_t apiVersion) noexcept
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

} // namespace iiixrlab
