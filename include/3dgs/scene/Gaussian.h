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
            std::vector<iiixrlab::math::Vector3f> SphereVertices;
        };

        struct InstanceInfo final
        {
            iiixrlab::math::Vector3f Position;
            iiixrlab::math::Vector3f ScaleInLogScale;
            iiixrlab::math::Vector4f Quaternion;
            iiixrlab::math::Vector4f ColorAsShDcComponentAndAlphaBeforeSigmoidActivision;
        };

        struct ConstantBufferInfo final
        {
            std::array<float, 45>    SphericalHarmonicsCoefficients;
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
        IIIXRLAB_INLINE const std::vector<iiixrlab::math::Vector3f>& GetSphereVertices() const noexcept { return mSphereVertices; }

    protected:
        Gaussian(iiixrlab::graphics::IRenderable::CreateInfo& createInfo, const GaussianInfo& gaussianInfo, std::vector<iiixrlab::math::Vector3f>&& sphereVertices) noexcept;

    private:
        const GaussianInfo& mGaussianInfo;
        std::vector<iiixrlab::math::Vector3f> mSphereVertices;
    };
} // namespace iiixrlab::scene
