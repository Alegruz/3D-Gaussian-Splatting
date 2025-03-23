#include "pch.h"

#include "3dgs/graphics/Device.h"
#include "3dgs/graphics/GaussianRenderScene.h"
#include "3dgs/graphics/Instance.h"
#include "3dgs/graphics/IRenderScene.hpp"
#include "3dgs/graphics/Pipeline.h"
#include "3dgs/graphics/PhysicalDevice.h"
#include "3dgs/graphics/Renderer.h"
#include "3dgs/graphics/Shader.h"
#include "3dgs/graphics/ShaderManager.h"
#include "3dgs/graphics/SwapChain.h"

#include "3dgs/scene/Gaussian.h"
#include "3dgs/scene/Scene.h"

#include "3dgs/InputManager.h"
#include "3dgs/Window.h"

namespace iiixrlab
{
	void ParseCommandlineArguments(ApplicationInfo& outApplicationInfo, const uint32_t numArguments, char** arguments)
	{
		for (uint32_t argumentIndex = 1; argumentIndex < numArguments; ++argumentIndex)
		{
			const char* argument = arguments[argumentIndex];
			if (strcmp(argument, "-m") == 0)
			{
				outApplicationInfo.ModelPath = (std::filesystem::current_path() / arguments[++argumentIndex]).generic_string();
			}
			else if (strcmp(argument, "-w") == 0)
			{
				outApplicationInfo.Width = std::atoi(arguments[++argumentIndex]);
			}
			else if (strcmp(argument, "-h") == 0)
			{
				outApplicationInfo.Height = std::atoi(arguments[++argumentIndex]);
			}
		}
	}
}

int main(int argc, char** argv)
{
	iiixrlab::ApplicationInfo applicationInfo =
	{
		.Info =
		{
			.Name = "3D Gaussian Splatting Renderer",
			.Version = IIIXRLAB_MAKE_API_VERSION(0, 0, 1, 0),
		}
	};

	iiixrlab::ParseCommandlineArguments(applicationInfo, argc, argv);
	if (applicationInfo.ModelPath.empty() == true)
	{
		std::cout << "Model path is empty!! Please provide a model path with -m <file-path>!!" << std::endl;
		assert(false);
		return -1;
	}

	iiixrlab::Window window(applicationInfo);

	iiixrlab::ProjectInfo engineInfo =
	{
		.Name = "3DGS Renderer Engine",
		.Version = IIIXRLAB_MAKE_API_VERSION(0, 0, 1, 0),
	};

	iiixrlab::graphics::RendererCreateInfo createInfo =
	{
		.ApplicationInfo = applicationInfo.Info,
		.EngineInfo = engineInfo,
		.FramesCount = 3,
		.Window = window,
	};

	iiixrlab::graphics::Renderer renderer(createInfo);
	iiixrlab::graphics::Instance& instance = renderer.GetInstance();
	iiixrlab::graphics::SwapChain& swapChain = instance.GetSwapChain();
	iiixrlab::graphics::PhysicalDevice& physicalDevice = instance.GetPhysicalDevice();
	iiixrlab::graphics::Device& device = physicalDevice.GetDevice();

	iiixrlab::scene::Scene scene(applicationInfo.ModelPath);

	iiixrlab::graphics::ShaderManager& shaderManager = iiixrlab::graphics::ShaderManager::GetInstance();

	std::vector<iiixrlab::graphics::Shader::CreateInfo> shaderCreateInfos =
	{
		iiixrlab::graphics::Shader::CreateInfo
		{
			.Device = device,
			.Path = "assets/shaders/Gaussian.slang",
			.EntryPoint = "VSMain",
			.Type = iiixrlab::graphics::Shader::eType::VERTEX,
		},
		iiixrlab::graphics::Shader::CreateInfo
		{
			.Device = device,
			.Path = "assets/shaders/Gaussian.slang",
			.EntryPoint = "PSMain",
			.Type = iiixrlab::graphics::Shader::eType::FRAGMENT,
		},
	};
	shaderManager.AddShaders(shaderCreateInfos);

	std::unique_ptr<iiixrlab::graphics::Pipeline> pipeline = nullptr;
	{
		iiixrlab::graphics::PipelineCreateInfo pipelineCreateInfo =
		{
			.Name = "GaussianPipeline",
			.VertexInputBindingDescriptions =
			{
				{
					.binding = 0,
					.stride = 3 * sizeof(float),
					.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE,
				},
			},
			.VertexInputAttributeDescriptions =
			{
				{
					.location = 0,
					.binding = 0,
					.format = VK_FORMAT_R32G32B32_SFLOAT,
					.offset = 0,
				},
			},
			.DescriptorSetLayoutBindings =
			{
				{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
					.pImmutableSamplers = nullptr,
				},
			},
			.ShaderNames = {"Gaussian_VSMain", "Gaussian_PSMain"},
			.PipelineLayout = VK_NULL_HANDLE,
			.ColorAttachment = *swapChain.GetBackBuffer(0).Color,
			.DepthAttachment = *swapChain.GetBackBuffer(0).Depth,
		};
		
		pipeline = device.CreatePipeline(pipelineCreateInfo);
	}

	std::unordered_map<std::string, std::unique_ptr<iiixrlab::graphics::Pipeline>> pipelines;
	pipelines.reserve(1);
	pipelines.insert(std::make_pair(pipeline->GetName(), std::move(pipeline)));
	
	iiixrlab::graphics::IRenderScene::CreateInfo renderSceneCreateInfo =
	{
		.Device = device,
		.Pipelines = std::move(pipelines),
		.Width = static_cast<float>(swapChain.GetExtent().width),
		.Height = static_cast<float>(swapChain.GetExtent().height),
	};
	std::unique_ptr<iiixrlab::graphics::GaussianRenderScene> gaussianRenderScene = std::make_unique<iiixrlab::graphics::GaussianRenderScene>(renderSceneCreateInfo);

	iiixrlab::scene::Gaussian::CreateInfo gaussianCreateInfo =
	{
		.Device = renderer.GetInstance().GetPhysicalDevice().GetDevice(),
		.GaussianInfo = scene.GetGaussianInfo(),
	};
	std::unique_ptr<iiixrlab::scene::Gaussian> gaussian = iiixrlab::scene::Gaussian::Create(gaussianCreateInfo);
	gaussianRenderScene->AddRenderable(std::move(gaussian));

	renderer.SetRenderScene(std::move(gaussianRenderScene));

	LARGE_INTEGER startingTime;
	LARGE_INTEGER endingTime;
	LARGE_INTEGER elapsedMicroseconds;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingTime);

	bool bQuitApplication = false;
	while (bQuitApplication == false)
	{
		const bool bHasEvents = window.HandleEvents(bQuitApplication);
		if (bHasEvents == false)
		{
			// Update our time
			QueryPerformanceCounter(&endingTime);
			elapsedMicroseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;
			elapsedMicroseconds.QuadPart *= 1000000;
			elapsedMicroseconds.QuadPart /= frequency.QuadPart;
			QueryPerformanceFrequency(&frequency);
			QueryPerformanceCounter(&startingTime);

			const float deltaTime = static_cast<float>(elapsedMicroseconds.QuadPart) / 1000000.0f;
			
			iiixrlab::InputManager& inputManager = iiixrlab::InputManager::GetInstance();
			inputManager.Update();
			renderer.Update(deltaTime);
			renderer.Render();
		}
	}

	return 0;
}
