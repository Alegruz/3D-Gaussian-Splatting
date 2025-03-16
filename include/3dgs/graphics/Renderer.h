#pragma once

#include "3dgs/Common.h"

namespace iiixrlab
{
	class Texture;
	class Window;

	namespace graphics
	{
		class FrameResource;
		class Instance;

		struct RendererCreateInfo;
	
		class Renderer final
		{
		public:
			Renderer() = delete;
			Renderer(const Renderer&) = delete;
			Renderer(Renderer&&) = delete;
	
			Renderer(const RendererCreateInfo& createInfo) noexcept;
	
			~Renderer() noexcept;
			
			Renderer& operator=(const Renderer&) = delete;
			Renderer& operator=(Renderer&&) = delete;
	
			void Render() noexcept;
	
		private:
			std::unique_ptr<Instance>	mInstance;
	
			std::vector<std::unique_ptr<FrameResource>> mFrameResources;
			uint32_t mCurrentFrameIndex;
	
			VkPipelineLayout mPipelineLayout;
			VkPipeline mPipeline;
			VkDescriptorSetLayout mDescriptorSetLayout;
			VkDescriptorPool mDescriptorPool;
		};
	
		
		struct RendererCreateInfo final
		{
			ProjectInfo ApplicationInfo;
			ProjectInfo EngineInfo;
			uint32_t    FramesCount = DEFAULT_FRAMES_COUNT;
			Window&     Window;
		};
	}
}