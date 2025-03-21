#include "3dgs/graphics/Device.hpp"

#include "3dgs/graphics/CommandPool.h"
#include "3dgs/graphics/Instance.h"
#include "3dgs/graphics/PhysicalDevice.h"
#include "3dgs/graphics/Queue.h"
#include "3dgs/graphics/Shader.h"
#include "3dgs/graphics/ShaderManager.h"
#include "3dgs/graphics/SwapChain.h"
#include "3dgs/graphics/Texture.h"

namespace iiixrlab::graphics
{
	Device::Device(CreateInfo& createInfo) noexcept
		: mPhysicalDevice(createInfo.PhysicalDevice)
		, mDevice(createInfo.Device)
		, mQueues()
		, mCommandPool()
		, mDescriptorPool(VK_NULL_HANDLE)
	{
		assert(mDevice != VK_NULL_HANDLE);

		std::vector<VkQueue> queues;
		getQueues(queues, mDevice, mPhysicalDevice.GetInstance().GetApiVersion(), mPhysicalDevice.GetQueueFamilyIndex(), mPhysicalDevice.GetQueueFamilyProperties());
		const uint32_t queuesCount = static_cast<uint32_t>(queues.size());
		mQueues.reserve(queuesCount);
		for (uint32_t queueIndex = 0; queueIndex < queuesCount; ++queueIndex)
		{
			Queue::CreateInfo queueCreateInfo =
			{
				.Device = *this,
				.Queue = queues[queueIndex],
				.QueueFamilyIndex = mPhysicalDevice.GetQueueFamilyIndex(),
				.QueueIndex = queueIndex,
			};
			mQueues.push_back(std::make_unique<Queue>(queueCreateInfo));
		}
	}

	Device::~Device() noexcept
	{
		vkDeviceWaitIdle(mDevice);
		for (std::unique_ptr<Queue>& queue : mQueues)
		{
			queue->Wait();
		}

		if (mDescriptorPool != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
			mDescriptorPool = VK_NULL_HANDLE;
		}

		mCommandPool->FreeCommandBuffers();
		mCommandPool.reset();
		mQueues.clear();

		ShaderManager& shaderManager = ShaderManager::GetInstance();
		shaderManager.DestroyShaders();

		PhysicalDevice::DestroyDevice(mDevice);
	}

	VkFence Device::CreateFence(const char* name) noexcept
	{
		VkResult vr = VK_SUCCESS;

		assert(name != nullptr);
		VkFence fence = VK_NULL_HANDLE;
		VkFenceCreateInfo fenceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};
		vr = vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &fence);
		assert(vr == VK_SUCCESS && fence != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_FENCE, fence);
#endif	// defined(_DEBUG)

		return fence;
	}

	VkPipeline Device::CreatePipeline(const char* name, const std::vector<std::string>& shaderNames, VkPipelineLayout pipelineLayout, const Texture& colorAttachment, const Texture& depthAttachment) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkPipeline pipeline = VK_NULL_HANDLE;

		ShaderManager& shaderManager = ShaderManager::GetInstance();
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		for (const std::string& shaderName : shaderNames)
		{
			std::unique_ptr<Shader>* ppShader = shaderManager.GetShaderOrNull(shaderName);
			if (ppShader == nullptr)
			{
				continue;
			}
			Shader& shader = **ppShader;

			VkPipelineShaderStageCreateInfo shaderStageCreateInfo =
			{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.module = shader.GetShaderModule(),
				.pName = "main",	// Slang always uses "main"
			};

			switch (shader.GetType())
			{
			case Shader::eType::VERTEX:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				break;
			case Shader::eType::TESSELLATION_CONTROL:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				break;
			case Shader::eType::TESSELLATION_EVALUATION:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				break;
			case Shader::eType::GEOMETRY:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;
			case Shader::eType::FRAGMENT:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			case Shader::eType::COMPUTE:
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				break;
			default:
				assert(false);
				break;
			}
			shaderStageCreateInfos.push_back(shaderStageCreateInfo);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr,
		};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1,
		};

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.lineWidth = 1.0f,
		};

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		};

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.front = VkStencilOpState
			{
				.failOp = VK_STENCIL_OP_KEEP,
				.passOp = VK_STENCIL_OP_KEEP,
				.compareOp = VK_COMPARE_OP_ALWAYS,
			},
			.back = VkStencilOpState
			{
				.failOp = VK_STENCIL_OP_KEEP,
				.passOp = VK_STENCIL_OP_KEEP,
				.compareOp = VK_COMPARE_OP_ALWAYS,
			},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
		};

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState =
		{
			.blendEnable = VK_FALSE,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		};

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachmentState,
		};

		std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size()),
			.pDynamicStates = dynamicStateEnables.data(),
		};

		VkFormat colorAttachmentFormat = colorAttachment.GetFormat();
		VkFormat depthAttachmentFormat = depthAttachment.GetFormat();
		VkPipelineRenderingCreateInfo renderingCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &colorAttachmentFormat,
			.depthAttachmentFormat = depthAttachmentFormat,
		};

		VkGraphicsPipelineCreateInfo pipelineCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &renderingCreateInfo,
			.flags = 0,
			.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size()),
			.pStages = shaderStageCreateInfos.data(),
			.pVertexInputState = &vertexInputStateCreateInfo,
			.pInputAssemblyState = &inputAssemblyStateCreateInfo,
			.pTessellationState = nullptr,
			.pViewportState = &viewportStateCreateInfo,
			.pRasterizationState = &rasterizationStateCreateInfo,
			.pMultisampleState = &multisampleStateCreateInfo,
			.pDepthStencilState = &depthStencilStateCreateInfo,
			.pColorBlendState = &colorBlendStateCreateInfo,
			.pDynamicState = &dynamicStateCreateInfo,
			.layout = pipelineLayout,
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = 0,
		};

		vr = vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
		assert(vr == VK_SUCCESS && pipeline != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_PIPELINE, pipeline);
#endif	// defined(_DEBUG)

		return pipeline;
	}

	VkPipelineLayout Device::CreatePipelineLayout(const char* name, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
			.pSetLayouts = descriptorSetLayouts.data(),
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr,
		};
		vr = vkCreatePipelineLayout(mDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		assert(vr == VK_SUCCESS && pipelineLayout != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_PIPELINE_LAYOUT, pipelineLayout);
#endif	// defined(_DEBUG)

		return pipelineLayout;
	}

	VkShaderModule Device::CreateShaderModule(const char* name, const std::filesystem::path& path) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkShaderModule shaderModule = VK_NULL_HANDLE;

		std::ifstream file(path, std::ios::ate | std::ios::binary);
		assert(file.is_open());

		const size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		VkShaderModuleCreateInfo shaderModuleCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = buffer.size(),
			.pCode = reinterpret_cast<const uint32_t*>(buffer.data()),
		};
		vr = vkCreateShaderModule(mDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
		assert(vr == VK_SUCCESS && shaderModule != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_SHADER_MODULE, shaderModule);
#endif	// defined(_DEBUG)

		return shaderModule;
	}

	uint32_t Device::AcquireNextImage(const SwapChain& swapChain, const VkSemaphore semaphore, const VkFence fence) noexcept
	{
		VkResult vr = VK_SUCCESS;
		uint32_t imageIndex = swapChain.GetFramesCount();
		while ((vr = vkAcquireNextImageKHR(mDevice, swapChain.mSwapChain, UINT64_MAX, semaphore, fence, &imageIndex)) == VK_NOT_READY)
		{
			YieldProcessor();
		}

		assert(vr == VK_SUCCESS && imageIndex < swapChain.GetFramesCount());
		return imageIndex;
	}

	VkCommandBuffer Device::AllocateCommandBuffer(const char* name) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo commandBufferAllocateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = mCommandPool->GetCommandPool(),
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		vr = vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &commandBuffer);
		assert(vr == VK_SUCCESS && commandBuffer != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_COMMAND_BUFFER, commandBuffer);
#endif	// defined(_DEBUG)

		return commandBuffer;
	}

	VkDescriptorSetLayout Device::CreateDescriptorSetLayout(const char* name, const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size()),
			.pBindings = descriptorSetLayoutBindings.data(),
		};
		vr = vkCreateDescriptorSetLayout(mDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
		assert(vr == VK_SUCCESS && descriptorSetLayout != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, descriptorSetLayout);
#endif	// defined(_DEBUG)

		return descriptorSetLayout;
	}

	VkImageView Device::CreateImageView(const char* name, const VkImage image, const VkFormat format, const uint8_t usage) noexcept
	{
		VkResult vr = VK_SUCCESS;
		VkImageView imageView = VK_NULL_HANDLE;
		VkImageAspectFlags aspectFlags = 0x0;
		if (usage & static_cast<uint8_t>(Texture::eUsageType::COLOR_ATTACHMENT))
		{
			aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
		}
		if (usage & static_cast<uint8_t>(Texture::eUsageType::DEPTH_STENCIL))
		{
			aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		VkImageViewCreateInfo imageViewCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = format,
			.components =
			{
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange =
			{
				.aspectMask = aspectFlags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};
		vr = vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &imageView);
		assert(vr == VK_SUCCESS && imageView != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_IMAGE_VIEW, imageView);
#endif	// defined(_DEBUG)

		return imageView;
	}

	VkSemaphore Device::CreateSemaphore(const char* name) noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(name != nullptr);
		VkSemaphore semaphore = VK_NULL_HANDLE;
		VkSemaphoreCreateInfo semaphoreCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
		};
		vr = vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &semaphore);
		assert(vr == VK_SUCCESS && semaphore != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName(name, VK_OBJECT_TYPE_SEMAPHORE, semaphore);
#endif	// defined(_DEBUG)

		return semaphore;
	}

	std::unique_ptr<Texture> Device::CreateTexture(const TextureCreateInfo& textureCreateInfo) noexcept
	{
		VkResult vr = VK_SUCCESS;
		Texture::CreateInfo createInfo =
		{
			.Device = *this,
			.Image = textureCreateInfo.ImageOrNull,
			.Format = textureCreateInfo.Format,
			.bIsBackBuffer = textureCreateInfo.ImageOrNull != VK_NULL_HANDLE,
		};

		std::vector<char> debugName(64);
		if (createInfo.Image == VK_NULL_HANDLE)
		{
			VkImageUsageFlags usage = 0x0;
			if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::SAMPLED))
			{
				usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			}
			if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::STORAGE))
			{
				usage |= VK_IMAGE_USAGE_STORAGE_BIT;
			}
			if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::COLOR_ATTACHMENT))
			{
				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
			if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::DEPTH_STENCIL))
			{
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}

			VkImageCreateInfo imageCreateInfo =
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.imageType = VK_IMAGE_TYPE_2D,
				.format = textureCreateInfo.Format,
				.extent = textureCreateInfo.Extent,
				.mipLevels = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling = VK_IMAGE_TILING_OPTIMAL,
				.usage = usage,
				.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices = nullptr,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			};

			vr = vkCreateImage(mDevice, &imageCreateInfo, nullptr, &createInfo.Image);
			assert(vr == VK_SUCCESS && createInfo.Image != VK_NULL_HANDLE);
			
			VkMemoryRequirements memoryRequirements = {};
			vkGetImageMemoryRequirements(mDevice, createInfo.Image, &memoryRequirements);

			VkMemoryAllocateInfo memoryAllocateInfo =
			{
				.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
				.pNext = nullptr,
				.allocationSize = memoryRequirements.size,
				.memoryTypeIndex = PhysicalDevice::GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mPhysicalDevice.GetPhysicalDeviceMemoryProperties()),
			};

			vr = vkAllocateMemory(mDevice, &memoryAllocateInfo, nullptr, &createInfo.DeviceMemory);
			assert(vr == VK_SUCCESS && createInfo.DeviceMemory != VK_NULL_HANDLE);

			vr = vkBindImageMemory(mDevice, createInfo.Image, createInfo.DeviceMemory, 0);
			assert(vr == VK_SUCCESS);

#if defined(_DEBUG)
			SetDebugName(textureCreateInfo.Name, VK_OBJECT_TYPE_IMAGE, createInfo.Image);

			sprintf_s(debugName.data(), debugName.size(), "DeviceMemory[%s]", textureCreateInfo.Name);
			SetDebugName(debugName.data(), VK_OBJECT_TYPE_DEVICE_MEMORY, createInfo.DeviceMemory);
#endif	// defined(_DEBUG)
		}

		if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::SAMPLED))
		{
			sprintf_s(debugName.data(), debugName.size(), "Sampled[%s]", textureCreateInfo.Name);
			createInfo.SampledViewOrNull = CreateImageView(debugName.data(), createInfo.Image, createInfo.Format, textureCreateInfo.Usage);
		}
		if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::STORAGE))
		{
			sprintf_s(debugName.data(), debugName.size(), "Storage[%s]", textureCreateInfo.Name);
			createInfo.StorageViewOrNull = CreateImageView(debugName.data(), createInfo.Image, createInfo.Format, textureCreateInfo.Usage);
		}
		if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::COLOR_ATTACHMENT))
		{
			sprintf_s(debugName.data(), debugName.size(), "ColorAttachment[%s]", textureCreateInfo.Name);
			createInfo.ColorAttachmentViewOrNull = CreateImageView(debugName.data(), createInfo.Image, createInfo.Format, textureCreateInfo.Usage);
		}
		if (textureCreateInfo.Usage & static_cast<uint8_t>(Texture::eUsageType::DEPTH_STENCIL))
		{
			sprintf_s(debugName.data(), debugName.size(), "DepthAttachment[%s]", textureCreateInfo.Name);
			createInfo.DepthAttachmentViewOrNull = CreateImageView(debugName.data(), createInfo.Image, createInfo.Format, textureCreateInfo.Usage);
		}

		return std::make_unique<Texture>(createInfo);
	}

	void Device::DestroyCommandBuffer(VkCommandBuffer& commandBuffer) noexcept
	{
		if (commandBuffer != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(mDevice, mCommandPool->GetCommandPool(), 1, &commandBuffer);
			commandBuffer = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyCommandPool(VkCommandPool& commandPool) noexcept
	{
		if (commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(mDevice, commandPool, nullptr);
			commandPool = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout) noexcept
	{
		if (descriptorSetLayout != VK_NULL_HANDLE)
		{
			vkDestroyDescriptorSetLayout(mDevice, descriptorSetLayout, nullptr);
			descriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyFence(VkFence& fence) noexcept
	{
		if (fence != VK_NULL_HANDLE)
		{
			vkDestroyFence(mDevice, fence, nullptr);
			fence = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyImage(VkImage& image) noexcept
	{
		if (image != VK_NULL_HANDLE)
		{
			vkDestroyImage(mDevice, image, nullptr);
			image = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyImageView(VkImageView& imageView) noexcept
	{
		if (imageView != VK_NULL_HANDLE)
		{
			vkDestroyImageView(mDevice, imageView, nullptr);
			imageView = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyPipeline(VkPipeline& pipeline) noexcept
	{
		if (pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(mDevice, pipeline, nullptr);
			pipeline = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyPipelineLayout(VkPipelineLayout& pipelineLayout) noexcept
	{
		if (pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(mDevice, pipelineLayout, nullptr);
			pipelineLayout = VK_NULL_HANDLE;
		}
	}

	void Device::DestroySemaphore(VkSemaphore& semaphore) noexcept
	{
		if (semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(mDevice, semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
	}

	void Device::DestroyShaderModule(VkShaderModule& shaderModule) noexcept
	{
		if (shaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(mDevice, shaderModule, nullptr);
			shaderModule = VK_NULL_HANDLE;
		}
	}

	void Device::DestroySwapChain(VkSwapchainKHR& swapChain) noexcept
	{
		if (swapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(mDevice, swapChain, nullptr);
			swapChain = VK_NULL_HANDLE;
		}
	}

	void Device::FreeMemory(VkDeviceMemory& deviceMemory) noexcept
	{
		if (deviceMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(mDevice, deviceMemory, nullptr);
			deviceMemory = VK_NULL_HANDLE;
		}
	}

	CommandPool& Device::InitializeCommandPool() noexcept
	{
		VkResult vr = VK_SUCCESS;
		assert(mCommandPool == nullptr);

		CommandPool::CreateInfo commandPoolCreateInfo =
		{
			.Device = *this,
			.CommandPool = VK_NULL_HANDLE,
		};

		VkCommandPoolCreateInfo vkCommandPoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = mPhysicalDevice.GetQueueFamilyIndex(),
		};
		vr = vkCreateCommandPool(mDevice, &vkCommandPoolCreateInfo, nullptr, &commandPoolCreateInfo.CommandPool);
		assert(vr == VK_SUCCESS && commandPoolCreateInfo.CommandPool != VK_NULL_HANDLE);
#if defined(_DEBUG)
		SetDebugName("Command Pool", VK_OBJECT_TYPE_COMMAND_POOL, commandPoolCreateInfo.CommandPool);
#endif	// defined(_DEBUG)
		mCommandPool = std::make_unique<CommandPool>(commandPoolCreateInfo);
		return *mCommandPool;
	}

	void Device::InitializeDescriptors() noexcept
	{
		VkResult vr = VK_SUCCESS;
		const uint32_t framesCount = mPhysicalDevice.GetInstance().GetSwapChain().GetFramesCount();

		VkDescriptorPoolSize descriptorPoolSize =
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = framesCount,
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
			.maxSets = framesCount,
			.poolSizeCount = 1,
			.pPoolSizes = &descriptorPoolSize,
		};

		vr = vkCreateDescriptorPool(mDevice, &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);
		assert(vr == VK_SUCCESS && mDescriptorPool != VK_NULL_HANDLE);
	}

	void Device::ResetFence(VkFence& fence) noexcept
	{
		assert(fence != VK_NULL_HANDLE);
		VkResult vr = vkResetFences(mDevice, 1, &fence);
		assert(vr == VK_SUCCESS);
	}

	void Device::WaitForFence(VkFence& fence) noexcept
	{
		assert(fence != VK_NULL_HANDLE);
		VkResult vr = vkWaitForFences(mDevice, 1, &fence, VK_TRUE, UINT64_MAX);
		assert(vr == VK_SUCCESS);
	}

	void Device::getQueues(std::vector<VkQueue>& outQueues, const VkDevice device, const uint32_t apiVersion, const uint32_t mainQueueFamilyPropertyIndex, const VkQueueFamilyProperties2& queueFamilyProperties) noexcept
	{
		for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperties.queueFamilyProperties.queueCount; ++queueIndex)
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
} // namespace iiixrlab::graphics
