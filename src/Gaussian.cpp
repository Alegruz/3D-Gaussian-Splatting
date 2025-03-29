#include "3dgs/scene/Gaussian.h"

#include "3dgs/graphics/Device.h"

namespace iiixrlab::scene
{
	static std::vector<iiixrlab::math::Vector3f> GenerateSphereVertices(const float radius, const uint32_t slicesCount, const uint32_t stacksCount)
	{
		std::vector<iiixrlab::math::Vector3f> vertices;

		// add top vertex
		vertices.push_back(iiixrlab::math::Vector3f{0, radius, 0});

		// generate vertices per stack / slice
		for (uint32_t i = 0; i < stacksCount - 1; ++i)
		{
			const float phi = static_cast<float>(std::numbers::pi_v<double> * static_cast<double>(i + 1) / static_cast<double>(stacksCount));
			for (uint32_t j = 0; j < slicesCount; ++j)
			{
				const float theta = static_cast<float>(2.0 * std::numbers::pi_v<double> * static_cast<double>(j) / static_cast<double>(slicesCount));
				const float x = radius * std::sinf(phi) * std::cosf(theta);
				const float y = radius * std::cosf(phi);
				const float z = radius * std::sinf(phi) * std::sinf(theta);
				vertices.push_back(iiixrlab::math::Vector3f{x, y, z});
			}
		}

		// add bottom vertex
		vertices.push_back(iiixrlab::math::Vector3f{0, -radius, 0});

		const iiixrlab::math::Vector3f& v0 = vertices.front();
		const iiixrlab::math::Vector3f& v1 = vertices.back();

		std::vector<iiixrlab::math::Vector3f> mesh;
		// add top / bottom triangles
		for (uint32_t i = 0; i < slicesCount; ++i)
		{
			uint32_t i0 = i + 1;
			uint32_t i1 = (i + 1) % slicesCount + 1;
			mesh.push_back(v0);
			mesh.push_back(vertices[i0]);
			mesh.push_back(vertices[i1]);
			
			i0 = i + slicesCount * (stacksCount - 2) + 1;
			i1 = (i + 1) % slicesCount + slicesCount * (stacksCount - 2) + 1;
			mesh.push_back(v1);
			mesh.push_back(vertices[i1]);
			mesh.push_back(vertices[i0]);
		}

		// add quads per stack / slice
		for (uint32_t j = 0; j < stacksCount - 2; j++)
		{
			const uint32_t j0 = j * slicesCount + 1;
			const uint32_t j1 = (j + 1) * slicesCount + 1;
			for (uint32_t i = 0; i < slicesCount; i++)
			{
				const uint32_t i0 = j0 + i;
				const uint32_t i1 = j0 + (i + 1) % slicesCount;
				const uint32_t i2 = j1 + (i + 1) % slicesCount;
				const uint32_t i3 = j1 + i;

				mesh.push_back(vertices[i2]);
				mesh.push_back(vertices[i1]);
				mesh.push_back(vertices[i0]);

				mesh.push_back(vertices[i3]);
				mesh.push_back(vertices[i2]);
				mesh.push_back(vertices[i0]);
			}
		}
		return mesh;
	}

	std::unique_ptr<Gaussian> Gaussian::Create(CreateInfo& createInfo) noexcept
	{
		iiixrlab::graphics::IRenderable::CreateInfo renderableCreateInfo =
		{
			.Device = createInfo.Device,
		};

		std::vector<iiixrlab::math::Vector3f> sphereVertices = GenerateSphereVertices(1.0f, 4, 4);
		const uint32_t vertexBufferSize = static_cast<uint32_t>(sphereVertices.size() * sizeof(iiixrlab::math::Vector3f) + createInfo.GaussianInfo.NumPoints * 3 * sizeof(float));
		renderableCreateInfo.StagingBuffer = createInfo.Device.CreateStagingBuffer("Gaussian Vertex Buffer", vertexBufferSize);

		Gaussian gaussian = Gaussian(renderableCreateInfo, createInfo.GaussianInfo, std::move(sphereVertices));
		return std::make_unique<Gaussian>(std::move(gaussian));
	}
	
	Gaussian::Gaussian(iiixrlab::graphics::IRenderable::CreateInfo& createInfo, const GaussianInfo& gaussianInfo, std::vector<iiixrlab::math::Vector3f>&& sphereVertices) noexcept
		: iiixrlab::graphics::IRenderable(createInfo)
		, mGaussianInfo(gaussianInfo)
		, mSphereVertices(std::move(sphereVertices))
	{
		uint8_t* data = nullptr;
		mDevice.MapMemory(*mStagingBuffer, reinterpret_cast<void**>(&data));
		uint32_t offset = 0;
		memcpy(data + offset, mSphereVertices.data(), mSphereVertices.size() * sizeof(iiixrlab::math::Vector3f));
		offset += static_cast<uint32_t>(mSphereVertices.size() * sizeof(iiixrlab::math::Vector3f));
		memcpy(data + offset, mGaussianInfo.Positions.data(), mGaussianInfo.NumPoints * 3 * sizeof(float));
	}
} // namespace iiixrlab::scene
