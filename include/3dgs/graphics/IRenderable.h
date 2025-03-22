#pragma once

#include "pch.h"

#include "3dgs/graphics/StagingBuffer.h"

namespace iiixrlab::graphics
{
    class Device;
    class StagingBuffer;

    class IRenderable
    {
    public:
        struct CreateInfo final
        {
            Device& Device;
            std::unique_ptr<StagingBuffer> StagingBuffer;
        };

    public:
        IRenderable() = delete;

        IRenderable(const IRenderable&) = delete;
        IRenderable& operator=(const IRenderable&) = delete;

        virtual ~IRenderable();

        IRenderable(IRenderable&&) noexcept;
        IRenderable& operator=(IRenderable&&) = delete;
        
        IIIXRLAB_INLINE StagingBuffer& GetStagingBuffer() noexcept { return *mStagingBuffer; }
        IIIXRLAB_INLINE const StagingBuffer& GetStagingBuffer() const noexcept { return *mStagingBuffer; }

    protected:
        IIIXRLAB_INLINE IRenderable(CreateInfo& createInfo) noexcept
            : mDevice(createInfo.Device)
            , mStagingBuffer(std::move(createInfo.StagingBuffer))
        {
        }

        Device& mDevice;
        std::unique_ptr<StagingBuffer> mStagingBuffer;
    };

	template <typename RenderableType>
    concept Renderable = std::is_base_of<IRenderable, RenderableType>::value;
} // namespace iiixrlab::graphics
