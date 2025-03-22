#pragma once

#include "pch.h"

#include "3dgs/graphics/IRenderScene.h"

#include "3dgs/scene/Gaussian.h"

namespace iiixrlab::graphics
{
	class GaussianRenderScene final : public TRenderScene<iiixrlab::scene::Gaussian>
	{
	public:
        GaussianRenderScene() = delete;
		GaussianRenderScene(IRenderScene::CreateInfo& createInfo) noexcept;
		
        GaussianRenderScene(const GaussianRenderScene&) = delete;
		GaussianRenderScene& operator=(const GaussianRenderScene&) = delete;

		GaussianRenderScene(GaussianRenderScene&&) = delete;
		GaussianRenderScene& operator=(GaussianRenderScene&&) = delete;

		~GaussianRenderScene() noexcept;
        
		void Render(CommandBuffer& commandBuffer) noexcept override;
		void Update(CommandBuffer& commandBuffer) noexcept override;
	};
} // namespace iiixrlab::graphics
