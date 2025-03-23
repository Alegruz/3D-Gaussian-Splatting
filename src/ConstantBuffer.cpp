#include "3dgs/graphics/ConstantBuffer.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    ConstantBuffer::ConstantBuffer(const CreateInfo& createInfo) noexcept
        : Buffer(createInfo)
        , mMappedData(nullptr)
    {
        mDevice.MapMemory(*this, reinterpret_cast<void**>(&mMappedData));
    }

    bool ConstantBuffer::SetData(const void* data, const size_t size) noexcept
    {
        if (mMappedData == nullptr)
        {
            std::cerr << "The data is not mapped.\n";
            IIIXRLAB_DEBUG_BREAK();
            return false;
        }

        if (GetTotalSize() < size)
        {
            std::cerr << "The size of the data is too large.\n";
            IIIXRLAB_DEBUG_BREAK();
            return false;
        }

        std::memcpy(mMappedData, data, size);

        return true;
    }
} // namespace iiixrlab::graphics
