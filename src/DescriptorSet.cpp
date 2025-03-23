#include "3dgs/graphics/DescriptorSet.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/DescriptorPool.h"

namespace iiixrlab::graphics
{
    void DescriptorSet::Bind(const ConstantBuffer& constantBuffer) noexcept
    {
		mDevice.BindDescriptorSet(*this, constantBuffer);
    }
}   // namespace iiixrlab::graphics