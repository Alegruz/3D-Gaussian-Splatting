#pragma once

#if defined(_MSC_VER)
    #define IIIXRLAB_INLINE __forceinline
    #define IIIXRLAB_DEBUG_BREAK __debugbreak
#elif defined(__GNUC__) || defined(__clang__)
    #define IIIXRLAB_INLINE inline __attribute__((always_inline))
    #define IIIXRLAB_DEBUG_BREAK __builtin_trap
#else
    #define IIIXRLAB_INLINE inline
    #define IIIXRLAB_DEBUG_BREAK
#endif

#define IIIXRLAB_MAKE_API_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29U) | (((uint32_t)(major)) << 22U) | (((uint32_t)(minor)) << 12U) | ((uint32_t)(patch)))