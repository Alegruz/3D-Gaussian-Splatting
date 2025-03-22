#pragma once

#include "3dgs/graphics/IRenderable.h"

#include "3dgs/scene/DataTypes.h"

namespace iiixrlab::scene
{
    class iiixrlab::graphics::Device;
    class iiixrlab::graphics::CommandBuffer;
    
    class Gaussian final : public iiixrlab::graphics::IRenderable
    {
    public:
        struct CreateInfo final
        {
            iiixrlab::graphics::Device& Device;
            const GaussianInfo& GaussianInfo;
        };

    public:
        static std::unique_ptr<Gaussian> Create(CreateInfo& createInfo) noexcept;

    public:
        Gaussian() = delete;

        Gaussian(const Gaussian&) = delete;
        Gaussian& operator=(const Gaussian&) = delete;

        ~Gaussian() noexcept = default;

        IIIXRLAB_INLINE constexpr Gaussian(Gaussian&&) noexcept = default;
        Gaussian& operator=(Gaussian&&) = delete;

        IIIXRLAB_INLINE const GaussianInfo& GetGaussianInfo() const noexcept { return mGaussianInfo; }

    protected:
        Gaussian(iiixrlab::graphics::IRenderable::CreateInfo& createInfo, const GaussianInfo& gaussianInfo) noexcept;

    private:
        const GaussianInfo& mGaussianInfo;
    };
} // namespace iiixrlab::scene
