#pragma once

#include "3dgs/graphics/IRenderScene.h"

namespace iiixrlab::graphics
{
    IIIXRLAB_INLINE IRenderScene::IRenderScene(CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mPipelines(std::move(createInfo.Pipelines))
        , mVertexBuffer()
    {
    }

    IIIXRLAB_INLINE IRenderScene::~IRenderScene() noexcept
    {
        for (auto& pipeline : mPipelines)
        {
            pipeline.second.reset();
        }
        mPipelines.clear();

        if (mVertexBuffer != nullptr)
        {
            mVertexBuffer.reset();
        }
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr TRenderScene<TRenderable>::TRenderScene(IRenderScene::CreateInfo& createInfo) noexcept
        : IRenderScene(createInfo)
    {}

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr TRenderScene<TRenderable>::~TRenderScene() noexcept
    {
        mRenderables.clear();
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr void TRenderScene<TRenderable>::AddRenderable(std::unique_ptr<TRenderable>&& renderable) noexcept
    {
        mRenderables.push_back(std::move(renderable));
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE void TRenderScene<TRenderable>::update(CommandBuffer& commandBuffer) noexcept
    {
        for (auto& renderable : mRenderables)
        {
            renderable->Update(commandBuffer);
        }

        updateInner(commandBuffer);
    }
} // namespace iiixrlab::graphics
