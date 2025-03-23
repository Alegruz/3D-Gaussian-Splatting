#pragma once

#include "3dgs/Common.h"

#include "3dgs/graphics/IRenderScene.h"

namespace iiixrlab
{
	class Texture;
	class Window;

	namespace graphics
	{
		class FrameResource;
		class Instance;
		class IRenderScene;

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
	
			IIIXRLAB_INLINE IRenderScene& GetRenderScene() noexcept { return *mRenderScene; }
			IIIXRLAB_INLINE const IRenderScene& GetRenderScene() const noexcept { return *mRenderScene; }
			IIIXRLAB_INLINE void SetRenderScene(std::unique_ptr<IRenderScene>&& renderScene) noexcept { mRenderScene = std::move(renderScene); }

			IIIXRLAB_INLINE Instance& GetInstance() noexcept { return *mInstance; }
			IIIXRLAB_INLINE const Instance& GetInstance() const noexcept { return *mInstance; }

			void Render() noexcept;
			void Update(const float deltaTime) noexcept;
	
		private:
			std::unique_ptr<IRenderScene>	mRenderScene;
			std::unique_ptr<Instance>		mInstance;
	
			std::vector<std::unique_ptr<FrameResource>> mFrameResources;
			uint32_t mCurrentFrameIndex;
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