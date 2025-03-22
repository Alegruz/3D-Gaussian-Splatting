#include "3dgs/graphics/IRenderable.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/StagingBuffer.h"

namespace iiixrlab::graphics
{
    IRenderable::IRenderable(IRenderable&& other) noexcept
        : mDevice(other.mDevice)
        , mStagingBuffer(std::move(other.mStagingBuffer))
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
} // namespace iiixrlab::graphics
