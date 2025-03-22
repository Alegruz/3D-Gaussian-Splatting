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

		renderableCreateInfo.StagingBuffer = createInfo.Device.CreateStagingBuffer("Gaussian Vertex Buffer", createInfo.GaussianInfo.NumPoints * 3 * sizeof(float) * 3);

		Gaussian gaussian = Gaussian(renderableCreateInfo, createInfo.GaussianInfo);
		return std::make_unique<Gaussian>(std::move(gaussian));
	}
} // namespace iiixrlab::scene
