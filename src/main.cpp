#include "pch.h"

#include "3dgs/graphics/Renderer.h"
#include "3dgs/Scene.h"
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
	
	iiixrlab::Scene scene(applicationInfo.ModelPath);

	bool bQuitApplication = false;
	while (bQuitApplication == false)
	{
		const bool bHasEvents = window.HandleEvents(bQuitApplication);
		if (bHasEvents == false)
		{
			renderer.Render();
		}
	}

	return 0;
}
