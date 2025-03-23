#pragma once

#include "pch.h"

#include "3dgs/graphics/IRenderable.h"

#include "3dgs/graphics/Pipeline.h"
#include "3dgs/graphics/VertexBuffer.h"

namespace iiixrlab::scene
{
	class Camera;
}

namespace iiixrlab::graphics
{

	class CommandBuffer;
	class Device;
	class Pipeline;

	class IRenderScene
	{
	public:
		struct CreateInfo final
		{
			Device& Device;
			std::unordered_map<std::string, std::unique_ptr<Pipeline>>&& Pipelines;
			float Width;
			float Height;
		};

	public:
		IRenderScene() = delete;

		IRenderScene(const IRenderScene&) = delete;
		IRenderScene& operator=(const IRenderScene&) = delete;

		IRenderScene(IRenderScene&&) = delete;
		IRenderScene& operator=(IRenderScene&&) = delete;

		virtual ~IRenderScene() noexcept;

		virtual void Render(CommandBuffer& commandBuffer) noexcept = 0;
		IIIXRLAB_INLINE void Update(CommandBuffer& commandBuffer, const float deltaTime) noexcept { update(commandBuffer, deltaTime); }

	protected:
		IRenderScene(CreateInfo& createInfo) noexcept;

		virtual void update(CommandBuffer& commandBuffer, const float deltaTime) noexcept = 0;

	protected:
		Device& mDevice;
		std::unordered_map<std::string, std::unique_ptr<Pipeline>> mPipelines;
		std::unique_ptr<VertexBuffer> mVertexBuffer;
		std::unique_ptr<iiixrlab::scene::Camera>	mCamera;
	};

	template<Renderable TRenderable>
	class TRenderScene : public IRenderScene
	{
	public:
		TRenderScene() = delete;
		constexpr TRenderScene(IRenderScene::CreateInfo& createInfo) noexcept;
		
		TRenderScene(const TRenderScene&) = delete;
		TRenderScene& operator=(const TRenderScene&) = delete;

		TRenderScene(TRenderScene&&) = delete;
		TRenderScene& operator=(TRenderScene&&) = delete;

		virtual constexpr ~TRenderScene() noexcept;

		IIIXRLAB_INLINE constexpr const std::vector<std::unique_ptr<TRenderable>>& GetRenderables() const noexcept { return mRenderables; }
		
		constexpr void AddRenderable(std::unique_ptr<TRenderable>&& renderable) noexcept;

	protected:
		void update(CommandBuffer& commandBuffer, const float deltaTime) noexcept override final;
		virtual void updateInner(CommandBuffer& commandBuffer, const float deltaTime) noexcept = 0;
		
	private:
		std::vector<std::unique_ptr<TRenderable>>	mRenderables;
	};
} // namespace iiixrlab::graphics
