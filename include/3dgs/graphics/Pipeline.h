#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;

	class Pipeline final
	{
	public:
		friend class Device;
		friend class CommandBuffer;

	public:
		struct CreateInfo final
		{
			Device& Device;
			std::string Name;
			VkPipelineLayout PipelineLayout;
			VkPipeline Pipeline;
			std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
			VkDescriptorPool DescriptorPool;
		};
		
	public:
		Pipeline() = delete;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		IIIXRLAB_INLINE constexpr Pipeline(Pipeline&& other) noexcept
			: mDevice(other.mDevice)
			, mName(std::move(other.mName))
			, mPipelineLayout(other.mPipelineLayout)
			, mPipeline(other.mPipeline)
			, mDescriptorSetLayouts(std::move(other.mDescriptorSetLayouts))
			, mDescriptorPool(other.mDescriptorPool)
		{
			other.mPipelineLayout = VK_NULL_HANDLE;
			other.mPipeline = VK_NULL_HANDLE;
			other.mDescriptorSetLayouts.clear();
			other.mDescriptorPool = VK_NULL_HANDLE;
		}

		Pipeline& operator=(Pipeline&&) = delete;

		~Pipeline() noexcept;

		IIIXRLAB_INLINE const std::string& GetName() const noexcept { return mName; }

	protected:
		IIIXRLAB_INLINE constexpr Pipeline(const CreateInfo& createInfo) noexcept
			: mDevice(createInfo.Device)
			, mName(createInfo.Name)
			, mPipelineLayout(createInfo.PipelineLayout)
			, mPipeline(createInfo.Pipeline)
			, mDescriptorSetLayouts(std::move(createInfo.DescriptorSetLayouts))
			, mDescriptorPool(createInfo.DescriptorPool)
		{
		}

	private:
		Device& mDevice;
		std::string mName;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mPipeline;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		VkDescriptorPool mDescriptorPool;
	};
} // namespace iiixrlab::graphics
