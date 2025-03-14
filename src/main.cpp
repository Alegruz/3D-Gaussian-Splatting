#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

struct RenderInfo
{
	std::filesystem::path   ModelPath;
};

void ParseCommandlineArguments(RenderInfo& outRenderInfo, const uint32_t numArguments, char** arguments)
{
	for (uint32_t argumentIndex = 1; argumentIndex < numArguments; ++argumentIndex)
	{
		const char* argument = arguments[argumentIndex];
		std::cout << argument << '\n';

		if (strcmp(argument, "-m") == 0)
		{
			outRenderInfo.ModelPath = arguments[++argumentIndex];
		}
	}
}

int main(int argc, char** argv)
{
	RenderInfo renderInfo;
	ParseCommandlineArguments(renderInfo, argc, argv);
	if (renderInfo.ModelPath.empty() == true)
	{
		std::cout << "Model path is empty!! Please provide a model path with -m <file-path>!!" << std::endl;
		assert(false);
		return -1;
	}

	std::ifstream modelFile(renderInfo.ModelPath);
	if (modelFile.is_open() == false)
	{
		std::cout << "Unable to open file " << renderInfo.ModelPath << "!! Check if the path is valid!!" << std::endl;
		assert(false);
		return -1;
	}

	return 0;
}
