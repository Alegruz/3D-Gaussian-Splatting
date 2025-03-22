#pragma once

#include "pch.h"

#include "3dgs/scene/DataTypes.h"

namespace iiixrlab::scene
{
    class Scene final
    {
    public:
        Scene() = delete;
        Scene(const std::filesystem::path& modelPath) noexcept;
        IIIXRLAB_INLINE constexpr ~Scene() noexcept = default;

        IIIXRLAB_INLINE constexpr const GaussianInfo& GetGaussianInfo() const noexcept { return mGaussianInfo; }

    private:
        GaussianInfo mGaussianInfo;
    };
}