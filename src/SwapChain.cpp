#include "3dgs/graphics/SwapChain.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/Instance.h"
#include "3dgs/graphics/Texture.h"

namespace iiixrlab::graphics
{
    SwapChain::SwapChain(CreateInfo& createInfo) noexcept
        : mInstance(createInfo.Instance)
        , mDevice(createInfo.Device)
        , mSurface(createInfo.Surface)
        , mFramesCount(createInfo.FramesCount)
        , mSwapChain(createInfo.SwapChain)
        , mFrameExtent{.width = createInfo.FrameExtent.width, .height = createInfo.FrameExtent.height}
        , mBackBuffers(std::move(createInfo.BackBuffers))
    {
        assert(mSurface != VK_NULL_HANDLE);
        assert(mFramesCount > 0);
        assert(mSwapChain != VK_NULL_HANDLE);
    }

    SwapChain::~SwapChain() noexcept
    {
        mBackBuffers.clear();
        mDevice.DestroySwapChain(mSwapChain);
        mInstance.DestroySurface(mSurface);
    }
} // namespace iiixrlab::graphics
