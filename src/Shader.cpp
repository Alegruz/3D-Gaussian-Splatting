#include "3dgs/graphics/Shader.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::graphics
{
    Shader::Shader(CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mEntryPoint(createInfo.EntryPoint)
        , mShaderModule(mDevice.CreateShaderModule(createInfo.Path.string().c_str(), createInfo.Path))
        , mType(createInfo.Type)
    {
    }

    Shader::~Shader() noexcept
    {
        mDevice.DestroyShaderModule(mShaderModule);
    }
} // namespace iiixrlab::graphics
