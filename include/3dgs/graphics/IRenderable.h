#pragma once

#include "pch.h"

#include "3dgs/graphics/StagingBuffer.h"

namespace iiixrlab::graphics
{
    class CommandBuffer;
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

        void Update(CommandBuffer& commandBuffer, const float deltaTime) noexcept;
        void Upload(CommandBuffer& commandBuffer, Buffer& dstBuffer) noexcept;

    protected:
        IIIXRLAB_INLINE IRenderable(CreateInfo& createInfo) noexcept
            : mDevice(createInfo.Device)
            , mStagingBuffer(std::move(createInfo.StagingBuffer))
            , mUploadingFrameIndex(UINT32_MAX)
        {
        }

    protected:

        Device& mDevice;
        std::unique_ptr<StagingBuffer> mStagingBuffer;
        uint32_t mUploadingFrameIndex;
    };

	template <typename RenderableType>
    concept Renderable = std::is_base_of<IRenderable, RenderableType>::value;
} // namespace iiixrlab::graphics
