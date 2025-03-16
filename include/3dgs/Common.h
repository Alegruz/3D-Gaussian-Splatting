#pragma once

#include "pch.h"

#include "3dgs/CommonDefines.h"

namespace iiixrlab
{
    struct ProjectInfo final
    {
        std::string Name;
        uint32_t    Version;
    };
    
	struct ApplicationInfo
	{
		ProjectInfo				Info;
		uint32_t				Width;
		uint32_t				Height;
		std::filesystem::path	ModelPath;
	};
}