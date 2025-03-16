#pragma once

#include "3dgs/graphics/PhysicalDevice.h"

namespace iiixrlab::graphics
{
	constexpr uint32_t PhysicalDevice::Score(const uint32_t apiVersion, const VkPhysicalDeviceProperties2& properties2) noexcept
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
	
}   // namespace iiixrlab::graphics