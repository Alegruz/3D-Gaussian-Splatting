#include "3dgs/scene/Gaussian.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::scene
{
	std::unique_ptr<Gaussian> Gaussian::Create(CreateInfo& createInfo) noexcept
	{
		iiixrlab::graphics::IRenderable::CreateInfo renderableCreateInfo =
		{
			.Device = createInfo.Device,
		};

		renderableCreateInfo.StagingBuffer = createInfo.Device.CreateStagingBuffer("Gaussian Vertex Buffer", createInfo.GaussianInfo.NumPoints * 3 * sizeof(float));

		Gaussian gaussian = Gaussian(renderableCreateInfo, createInfo.GaussianInfo);
		return std::make_unique<Gaussian>(std::move(gaussian));
	}
	
	Gaussian::Gaussian(iiixrlab::graphics::IRenderable::CreateInfo& createInfo, const GaussianInfo& gaussianInfo) noexcept
		: iiixrlab::graphics::IRenderable(createInfo)
		, mGaussianInfo(gaussianInfo)
	{
		uint8_t* data = nullptr;
		mDevice.MapMemory(*mStagingBuffer, reinterpret_cast<void**>(&data));
		memcpy(data, mGaussianInfo.Positions.data(), mGaussianInfo.NumPoints * 3 * sizeof(float));
	}
} // namespace iiixrlab::scene
