#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class ConstantBuffer;
	class DescriptorSet;
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
			std::vector<std::unique_ptr<DescriptorSet>> DescriptorSets;
		};
		
	public:
		Pipeline() = delete;
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		explicit Pipeline(Pipeline&& other) noexcept;

		Pipeline& operator=(Pipeline&&) = delete;

		~Pipeline() noexcept;
		
		IIIXRLAB_INLINE DescriptorSet& GetDescriptorSet(const uint32_t index) noexcept { return *mDescriptorSets[index]; }
		IIIXRLAB_INLINE const DescriptorSet& GetDescriptorSet(const uint32_t index) const noexcept { return *mDescriptorSets[index]; }
		IIIXRLAB_INLINE constexpr uint32_t GetDescriptorSetCount() const noexcept { return static_cast<uint32_t>(mDescriptorSets.size()); }

		IIIXRLAB_INLINE const std::string& GetName() const noexcept { return mName; }

	protected:
		explicit Pipeline(CreateInfo& createInfo) noexcept;

	private:
		Device& mDevice;
		std::string mName;
		VkPipelineLayout mPipelineLayout;
		VkPipeline mPipeline;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		std::vector<std::unique_ptr<DescriptorSet>> mDescriptorSets;
	};
} // namespace iiixrlab::graphics
