#include "3dgs/graphics/GaussianRenderScene.h"

#include "3dgs/graphics/CommandBuffer.h"
#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/IRenderScene.hpp"
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
		commandBuffer.Draw(3, 1, 0, 0);
	}

	void GaussianRenderScene::Update(CommandBuffer& commandBuffer) noexcept
	{
		if (mVertexBuffer == nullptr)
		{
			uint32_t vertexBufferSize = 0;
			for (const auto& renderable : GetRenderables())
			{
				const iiixrlab::scene::GaussianInfo& gaussianInfo = renderable->GetGaussianInfo();
				vertexBufferSize += gaussianInfo.NumPoints * 3 * sizeof(float) * 3;
			}
			mVertexBuffer = mDevice.CreateVertexBuffer("GaussianVertexBuffer", vertexBufferSize);
		}

		for (const auto& renderable : GetRenderables())
		{
			[[maybe_unused]] const iiixrlab::scene::GaussianInfo& gaussianInfo = renderable->GetGaussianInfo();
			const StagingBuffer& stagingBuffer = renderable->GetStagingBuffer();

			commandBuffer.CopyBuffer(stagingBuffer, *mVertexBuffer, { .srcOffset = 0, .dstOffset = 0, .size = stagingBuffer.GetSize() });
		}
	}
} // namespace iiixrlab::graphics