#pragma once

#include "3dgs/Common.h"

namespace iiixrlab
{
    class Window;

    struct RendererCreateInfo;

    class Renderer final
    {
    public:
        static VkBool32 DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) noexcept;
        static VkBool32 DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) noexcept;

    public:
        Renderer() = delete;
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;

        Renderer(const RendererCreateInfo& createInfo) noexcept;

        ~Renderer() noexcept;
        
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void Render() noexcept;

    public:
        static constexpr const uint32_t MINIMUM_VK_API_VERSION = VK_API_VERSION_1_3;
        static constexpr const uint32_t DEFAULT_FRAMES_COUNT = 3;

    private:
        struct SwapChainCreateInfo final
        {
            VkSwapchainKHR& OutSwapChain;
            VkSurfaceKHR& OutSurface;
            uint32_t& OutFramesCount;
            const Window& Window;
            const VkInstance Instance;
            const VkPhysicalDevice PhysicalDevice;
            const VkDevice Device;
            const uint32_t ApiVersion;
            const uint32_t MainQueueFamilyPropertyIndex;
            const std::vector<VkQueueFamilyProperties2>& QueueFamilyProperties;
            const uint32_t FramesCount;
        };

    private:
        static void getSurfacePresentModes(std::vector<VkPresentModeKHR>& outSurfacePresentModes, const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) noexcept;
        static void logPhysicalDeviceProperties(const VkPhysicalDeviceProperties2& properties2, const bool bIsSelected = false) noexcept;
        static void logQueueFamilyProperties(const VkQueueFamilyProperties2& queueFamilyProperties2, const VkPhysicalDevice physicalDevice, const uint32_t queueFamilyIndex, const bool bIsSelected = false) noexcept;
        static constexpr uint32_t scorePhysicalDevice(const uint32_t apiVersion, const VkPhysicalDeviceProperties2& properties2) noexcept;
        
        static void initializeInstance(VkInstance& outInstance, uint32_t& outApiVersion, const RendererCreateInfo& createInfo) noexcept;
        static VkPhysicalDevice selectPhysicalDevice(const uint32_t apiVersion, VkInstance& instance) noexcept;
        static void selectMainQueueFamilyIndex(std::vector<VkQueueFamilyProperties2>& outQueueFamilyProperties, uint32_t& outMainQueueFamilyPropertyIndex, const VkPhysicalDevice physicalDevice, const uint32_t apiVersion) noexcept;
        static VkDevice createDevice(const VkPhysicalDevice physicalDevice, const uint32_t apiVersion, const std::vector<VkQueueFamilyProperties2>& queueFamilyPropertiesList) noexcept;
        static void getQueues(std::vector<VkQueue>& outQueues, const VkDevice device, const uint32_t apiVersion, const uint32_t mainQueueFamilyPropertyIndex, const std::vector<VkQueueFamilyProperties2>& queueFamilyProperties) noexcept;
        static void initializeSwapChain(SwapChainCreateInfo& createInfo) noexcept;

        static void destroyDevice(VkDevice& device, std::vector<VkQueue>& queues) noexcept;
        
#if defined(_DEBUG)
        static void initializeDebugCallback(VkDebugReportCallbackEXT& outCallback, VkDebugUtilsMessengerEXT& outMessenger, const VkInstance& instance) noexcept;
        static void destroyDebugCallback(VkDebugReportCallbackEXT& callback, VkDebugUtilsMessengerEXT& messenger, VkInstance& instance) noexcept;
#endif  // defined(_DEBUG)

    private:
        uint32_t    mApiVersion;
        VkInstance  mInstance;
#if defined(_DEBUG)
        VkDebugReportCallbackEXT mDebugReportCallback;
        VkDebugUtilsMessengerEXT mDebugUtilsMessenger;
#endif  // defined(_DEBUG)
        VkSurfaceKHR        mSurface;
        VkPhysicalDevice    mPhysicalDevice;
        uint32_t    mFramesCount;
        std::vector<VkQueueFamilyProperties2> mQueueFamilyProperties;
        uint32_t    mMainQueueFamilyPropertyIndex;
        VkDevice    mDevice;
        std::vector<VkQueue>   mQueues;
        VkSwapchainKHR  mSwapChain;
        VkCommandPool   mCommandPool;
        std::vector<VkCommandBuffer> mCommandBuffers;
    };

    
    struct RendererCreateInfo final
    {
        ProjectInfo ApplicationInfo;
        ProjectInfo EngineInfo;
        uint32_t    FramesCount = Renderer::DEFAULT_FRAMES_COUNT;
        Window&     Window;
    };
}