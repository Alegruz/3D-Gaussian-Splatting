#include "3dgs/graphics/GaussianRenderScene.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/DescriptorSet.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/IRenderScene.hpp"
#include "3dgs/graphics/Pipeline.h"
#include "3dgs/graphics/Shader.h"
#include "3dgs/graphics/ShaderManager.h"
#include "3dgs/graphics/StagingBuffer.h"
#include "3dgs/graphics/VertexBuffer.h"

namespace iiixrlab::graphics
{
	GaussianRenderScene::GaussianRenderScene(IRenderScene::CreateInfo& createInfo) noexcept
		: TRenderScene<iiixrlab::scene::Gaussian>(createInfo)
	{
	}

	GaussianRenderScene::~GaussianRenderScene() noexcept
	{
	}
	
	void GaussianRenderScene::Render(CommandBuffer& commandBuffer) noexcept
	{	
		auto pipelineFindResult = mPipelines.find("GaussianPipeline");
		if (pipelineFindResult == mPipelines.end())
		{
			std::cerr << "Pipeline: GaussianPipeline is not found.\n";
			IIIXRLAB_DEBUG_BREAK();
			return;
		}
		Pipeline& pipeline = *pipelineFindResult->second;
		commandBuffer.Bind(pipeline);
		
		for (const auto& renderable : GetRenderables())
		{
			const std::vector<iiixrlab::math::Vector3f>& sphereVertices = renderable->GetSphereVertices();
			const uint32_t sphereVerticesCount = static_cast<uint32_t>(sphereVertices.size());
			const iiixrlab::scene::GaussianInfo& gaussianInfo = renderable->GetGaussianInfo();
			std::vector<CommandBuffer::VertexBindingInfo> vertexBindingInfos;
			vertexBindingInfos.push_back({ .BindingIndex = 0, .Stride = sphereVerticesCount * sizeof(iiixrlab::math::Vector3f) });
			vertexBindingInfos.push_back({ .BindingIndex = 1, .Stride = gaussianInfo.NumPoints * 3 * sizeof(float) });
			commandBuffer.Bind(*mVertexBuffer, vertexBindingInfos);

			// commandBuffer.Draw(sphereVerticesCount, 1, 0, 0);
			commandBuffer.Draw(sphereVerticesCount, gaussianInfo.NumPoints, 0, 0);
		}
	}

	void GaussianRenderScene::updateInner(CommandBuffer& commandBuffer, [[maybe_unused]] const float deltaTime) noexcept
	{
		if (mVertexBuffer == nullptr)
		{
			uint32_t vertexBufferSize = 0;
			for (const auto& renderable : GetRenderables())
			{
				const std::vector<iiixrlab::math::Vector3f>& sphereVertices = renderable->GetSphereVertices();
				vertexBufferSize += static_cast<uint32_t>(sphereVertices.size() * sizeof(iiixrlab::math::Vector3f));

				const iiixrlab::scene::GaussianInfo& gaussianInfo = renderable->GetGaussianInfo();
				vertexBufferSize += gaussianInfo.NumPoints * 3 * sizeof(float);
			}
			mVertexBuffer = mDevice.CreateVertexBuffer("GaussianVertexBuffer", vertexBufferSize);

			auto pipelineFindResult = mPipelines.find("GaussianPipeline");
			if (pipelineFindResult == mPipelines.end())
			{
				std::cerr << "Pipeline: GaussianPipeline is not found.\n";
				IIIXRLAB_DEBUG_BREAK();
				return;
			}
			Pipeline& pipeline = *pipelineFindResult->second;
			DescriptorSet& descriptorSet = pipeline.GetDescriptorSet(0);
			const ConstantBuffer& cameraBuffer = mCamera->GetConstantBuffer();
			descriptorSet.Bind(cameraBuffer);
		}

		for (const auto& renderable : GetRenderables())
		{
			[[maybe_unused]] const iiixrlab::scene::GaussianInfo& gaussianInfo = renderable->GetGaussianInfo();
			StagingBuffer& stagingBuffer = renderable->GetStagingBuffer();
			commandBuffer.CopyBuffer(stagingBuffer, *mVertexBuffer, { .srcOffset = 0, .dstOffset = 0, .size = stagingBuffer.GetTotalSize() });
		}
	}
} // namespace iiixrlab::graphics