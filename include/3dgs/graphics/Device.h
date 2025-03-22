#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
#undef CreateSemaphore

	class Buffer;
	class CommandPool;
	class Pipeline;
	class PhysicalDevice;
	class Queue;
	class StagingBuffer;
	class SwapChain;
	class Texture;
	class VertexBuffer;

	struct PipelineCreateInfo final
	{
		const char* Name;
		std::vector<VkVertexInputBindingDescription> VertexInputBindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescriptions;
		std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
		std::vector<std::string> ShaderNames;
		VkPipelineLayout PipelineLayout;
		Texture& ColorAttachment;
		Texture& DepthAttachment;
	};

	struct TextureCreateInfo final
	{
		const char* Name;
		VkFormat Format;
		uint8_t Usage;
		VkExtent3D Extent;
		VkImage ImageOrNull = VK_NULL_HANDLE;	// If nullptr, the image is a back buffer
	};

	class Device final
	{
	public:
		friend class Instance;
		friend class PhysicalDevice;

	public:
		struct CreateInfo final
		{
			PhysicalDevice& PhysicalDevice;
			VkDevice        Device;
		};

	public:
		Device() = default;

		Device(CreateInfo& createInfo) noexcept;

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		
		~Device() noexcept;

		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		IIIXRLAB_INLINE Queue& GetQueue(const uint32_t index = 0) noexcept { return *mQueues[index]; }
		IIIXRLAB_INLINE const Queue& GetQueue(const uint32_t index = 0) const noexcept { return *mQueues[index]; }
		IIIXRLAB_INLINE const PhysicalDevice& GetPhysicalDevice() const noexcept { return mPhysicalDevice; }

		uint32_t AcquireNextImage(const SwapChain& swapChain, const VkSemaphore semaphore, const VkFence fence) noexcept;
		VkCommandBuffer AllocateCommandBuffer(const char* name) noexcept;
		VkImageView CreateImageView(const char* name, const VkImage image, const VkFormat format, const uint8_t usage) noexcept;
		VkFence CreateFence(const char* name) noexcept;
		std::unique_ptr<Pipeline> CreatePipeline(const PipelineCreateInfo& pipelineCreateInfo) noexcept;
		VkShaderModule CreateShaderModule(const char* name, const std::filesystem::path& path) noexcept;
		VkSemaphore CreateSemaphore(const char* name) noexcept;
		std::unique_ptr<StagingBuffer> CreateStagingBuffer(const char* name, const uint32_t stagingBufferSize) noexcept;
		std::unique_ptr<Texture> CreateTexture(const TextureCreateInfo& textureCreateInfo) noexcept;
		std::unique_ptr<VertexBuffer> CreateVertexBuffer(const char* name, const uint32_t vertexBufferSize) noexcept;
		void DestroyCommandBuffer(VkCommandBuffer& commandBuffer) noexcept;
		void DestroyCommandPool(VkCommandPool& commandPool) noexcept;
		void DestroyDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) noexcept;
		void DestroyFence(VkFence& fence) noexcept;
		void DestroyImage(VkImage& image) noexcept;
		void DestroyImageView(VkImageView& imageView) noexcept;
		void DestroyPipeline(VkPipeline& pipeline) noexcept;
		void DestroyPipelineLayout(VkPipelineLayout& pipelineLayout) noexcept;
		void DestroySemaphore(VkSemaphore& semaphore) noexcept;
		void DestroyShaderModule(VkShaderModule& shaderModule) noexcept;
		void DestroySwapChain(VkSwapchainKHR& swapChain) noexcept;
		void DestroyBuffer(VkBuffer& vertexBuffer) noexcept;
		void FreeMemory(VkDeviceMemory& deviceMemory) noexcept;
		CommandPool& InitializeCommandPool() noexcept;
		void InitializeDescriptors() noexcept;
		void MapMemory(Buffer& buffer, void** data) noexcept;
		void ResetFence(VkFence& fence) noexcept;
		void WaitForFence(VkFence& fence) noexcept;

#if defined(_DEBUG)
		void SetDebugName(const char* name, const VkObjectType objectType, const void* object) noexcept;
#endif	// defined(_DEBUG)
		
	private:
		static void getQueues(std::vector<VkQueue>& outQueues, const VkDevice device, const uint32_t apiVersion, const uint32_t mainQueueFamilyPropertyIndex, const VkQueueFamilyProperties2& queueFamilyProperties) noexcept;

#if defined(_DEBUG)
		static void setDebugName(const char* name, const VkDevice device, const VkObjectType objectType, const void* object) noexcept;
#endif	// defined(_DEBUG)

	private:
		PhysicalDevice& mPhysicalDevice;
		VkDevice mDevice;

		std::vector<std::unique_ptr<Queue>> mQueues;
		std::unique_ptr<CommandPool> mCommandPool;
		VkDescriptorPool mDescriptorPool;
	};
} // namespace iiixrlab::graphics
