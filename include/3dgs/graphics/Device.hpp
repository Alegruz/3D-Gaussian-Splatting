#pragma once

#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
#if defined(_DEBUG)
    IIIXRLAB_INLINE void Device::SetDebugName(const char* name, const VkObjectType objectType, const void* object) noexcept
    {
        Device::setDebugName(name, mDevice, objectType, object);
    }

    IIIXRLAB_INLINE void Device::setDebugName(const char* name, const VkDevice device, const VkObjectType objectType, const void* object) noexcept
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
#endif  // defined(_DEBUG)
}   // namespace iiixrlab::graphics