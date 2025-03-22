#include "3dgs/scene/Scene.h"

namespace iiixrlab::scene
{
    Scene::Scene(const std::filesystem::path& modelPath) noexcept
    {
        std::ifstream modelFile(modelPath);
        if (modelFile.is_open() == false)
        {
            std::cout << "Unable to open file " << modelPath << "!! Check if the path is valid!!" << std::endl;
            assert(false);
            return;
        }

        // Load the model
		const std::filesystem::path extension = modelPath.extension();
		if (extension == ".ply")
		{
			std::cout << "Loading ply files is not yet implemented!!" << std::endl;
			assert(false);
			return;
		}
		else if (extension == ".spz")
		{
			std::cout << "Loading spz file " << modelPath << "!!" << '\n';
			spz::GaussianCloud gaussianCloud = spz::loadSpz(modelPath.string());
	
			mGaussianInfo.NumPoints = gaussianCloud.numPoints;
			mGaussianInfo.ShDegree = gaussianCloud.shDegree;
			mGaussianInfo.isAntialiased = gaussianCloud.antialiased;
			mGaussianInfo.Positions = std::move(gaussianCloud.positions);
			mGaussianInfo.Scales = std::move(gaussianCloud.scales);
			mGaussianInfo.Rotations = std::move(gaussianCloud.rotations);
			mGaussianInfo.Alphas = std::move(gaussianCloud.alphas);
			mGaussianInfo.Colors = std::move(gaussianCloud.colors);
			mGaussianInfo.SphericalHarmonics = std::move(gaussianCloud.sh);
	
			return;
		}
	
		std::cout << "Invalid file extension " << extension << "!!" << std::endl;
		assert(false);
    }
}