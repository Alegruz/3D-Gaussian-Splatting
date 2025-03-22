#include "3dgs/graphics/IRenderable.h"

#include "3dgs/graphics/Buffer.h"
#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/FrameResource.h"
#include "3dgs/graphics/StagingBuffer.h"

namespace iiixrlab::graphics
{
    IRenderable::IRenderable(IRenderable&& other) noexcept
        : mDevice(other.mDevice)
        , mStagingBuffer(std::move(other.mStagingBuffer))
        , mUploadingFrameIndex(other.mUploadingFrameIndex)
    {
        other.mStagingBuffer.reset();
    }

    IRenderable::~IRenderable()
    {
        if (mStagingBuffer != nullptr)
        {
            mStagingBuffer.reset();
        }
    }

    void IRenderable::Update(CommandBuffer& commandBuffer) noexcept
    {
        if (mUploadingFrameIndex == commandBuffer.GetFrameResource().GetFrameIndex())
        {
            mStagingBuffer.reset();
        }
    }

    void IRenderable::Upload(CommandBuffer& commandBuffer, Buffer& dstBuffer) noexcept
    {
        commandBuffer.CopyBuffer(*mStagingBuffer, dstBuffer, { .srcOffset = 0, .dstOffset = 0, .size = mStagingBuffer->GetSize() });
        mUploadingFrameIndex = commandBuffer.GetFrameResource().GetFrameIndex();
    }
} // namespace iiixrlab::graphics
