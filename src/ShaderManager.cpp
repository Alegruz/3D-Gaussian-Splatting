#include "3dgs/graphics/ShaderManager.h"

namespace iiixrlab::graphics
{
	ShaderManager& ShaderManager::GetInstance() noexcept
	{
		static ShaderManager instance;
		return instance;
	}

	ShaderManager::ShaderManager() noexcept
		: mGlobalSession()
		, mShaders()
	{
		SlangGlobalSessionDesc globalSessionDesc = {};
		slang::createGlobalSession(&globalSessionDesc, mGlobalSession.writeRef());
	}

	ShaderManager::~ShaderManager() noexcept
	{
		for (auto& [name, shader] : mShaders)
		{
			shader.reset();
		}
		mShaders.clear();
	}

	void ShaderManager::AddShaders(std::vector<Shader::CreateInfo>& createInfos) noexcept
	{
		if (createInfos.empty() == true)
		{
			return;
		}

		const Shader::CreateInfo& mainCreateInfo = createInfos[0];

		const std::filesystem::path& shaderPath = mainCreateInfo.Path;
		const std::string shaderFilename = shaderPath.stem().string();
		std::filesystem::path shaderAbsPath = std::filesystem::current_path() / shaderPath;
		if (!std::filesystem::exists(shaderAbsPath))
		{
			std::cerr << "Shader file not found: " << shaderAbsPath.string() << std::endl;
			IIIXRLAB_DEBUG_BREAK();
		}
		const std::filesystem::path shaderCachePath = shaderAbsPath.parent_path() / "caches";
		if (!std::filesystem::exists(shaderCachePath))
		{
			std::filesystem::create_directories(shaderCachePath);
		}

		uint32_t shadersToCompileCount = 0;
		for (const Shader::CreateInfo& createInfo : createInfos)
		{
			if (shaderPath != createInfo.Path)
			{
				std::cerr << "Shader filename mismatch: " << shaderPath << " != " << createInfo.Path << std::endl;
				IIIXRLAB_DEBUG_BREAK();
				continue;
			}

			const std::string shaderName = shaderFilename + "_" + createInfo.EntryPoint;
			auto ppShader = GetShaderOrNull(shaderName);
			if (ppShader != nullptr)
			{
				continue;
			}
			++shadersToCompileCount;
		}

		if (shadersToCompileCount <= 0)
		{
			return;
		}

		std::vector<slang::TargetDesc> targetDescs = 
		{
			slang::TargetDesc
			{
				.format = SlangCompileTarget::SLANG_SPIRV,
				.profile = mGlobalSession->findProfile("spirv_1_5"),
				.flags = 0,
			},
			slang::TargetDesc
			{
				.format = SlangCompileTarget::SLANG_SPIRV_ASM,
				.profile = mGlobalSession->findProfile("SM_6_6"),
				.flags = 0,
			},
		};

		std::vector<slang::CompilerOptionEntry> compilerOptions =
		{
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::Include,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::String,
					.stringValue0 = "assets/shaders",
				},
			},
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::EmitSpirvDirectly,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::Int,
					.intValue0 = 1,
				},
			},
#if defined(_DEBUG)
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::DebugInformation,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::Int,
					.intValue0 = SlangDebugInfoLevel::SLANG_DEBUG_INFO_LEVEL_MAXIMAL,
				},
			},
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::DebugInformationFormat,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::Int,
					.intValue0 = SlangDebugInfoFormat::SLANG_DEBUG_INFO_FORMAT_PDB,
				},
			},
#endif	// NOT defined(_DEBUG)
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::Optimization,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::Int,
#if defined(_DEBUG)
					.intValue0 = SlangOptimizationLevel::SLANG_OPTIMIZATION_LEVEL_NONE,
#else	// NOT defined(_DEBUG)
					.intValue0 = SlangOptimizationLevel::SLANG_OPTIMIZATION_LEVEL_HIGH,
#endif	// NOT defined(_DEBUG)
				},
			},
			slang::CompilerOptionEntry
			{
				.name = slang::CompilerOptionName::WarningsAsErrors,
				.value = slang::CompilerOptionValue
				{
					.kind = slang::CompilerOptionValueKind::String,
					.stringValue0 = "all",
				},
			},
		};

		slang::SessionDesc sessionDesc = 
		{
			.targets = targetDescs.data(),
			.targetCount = static_cast<uint32_t>(targetDescs.size()),
			.compilerOptionEntries = compilerOptions.data(),
			.compilerOptionEntryCount = static_cast<uint32_t>(compilerOptions.size()),
		};


		Slang::ComPtr<slang::ISession> session;
		mGlobalSession->createSession(sessionDesc, session.writeRef());
		slang::IModule* module = nullptr;
		{
			Slang::ComPtr<slang::IBlob> diagnosticBlob;
			module = session->loadModule(shaderAbsPath.string().c_str(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::cerr << reinterpret_cast<const char*>(diagnosticBlob->getBufferPointer()) << std::endl;
				IIIXRLAB_DEBUG_BREAK();
			}
			assert(module != nullptr);
		}
		
		std::vector<slang::IComponentType*> componentTypes =
		{
			module,
		};
		std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
		entryPoints.reserve(static_cast<uint32_t>(Shader::eType::COUNT));

		for (const Shader::CreateInfo& createInfo : createInfos)
		{
			const std::string shaderName = shaderFilename + "_" + createInfo.EntryPoint;
			std::filesystem::path shaderCacheFilePath = shaderCachePath / (shaderName + ".spv");
			entryPoints.push_back(Slang::ComPtr<slang::IEntryPoint>());
			Slang::ComPtr<slang::IEntryPoint>& entryPoint = entryPoints.back();
			module->findEntryPointByName(createInfo.EntryPoint.c_str(), entryPoint.writeRef());
			componentTypes.push_back(entryPoint);
		}
		
		Slang::ComPtr<slang::IComponentType> program;
		{
			Slang::ComPtr<slang::IBlob> diagnosticBlob;
			session->createCompositeComponentType(componentTypes.data(), componentTypes.size(), program.writeRef(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::cerr << reinterpret_cast<const char*>(diagnosticBlob->getBufferPointer()) << std::endl;
				IIIXRLAB_DEBUG_BREAK();
			}
			assert(program != nullptr);
		}

        for (uint32_t entryPointIndex = 0; entryPointIndex < entryPoints.size(); ++entryPointIndex)
        {
			Shader::CreateInfo& createInfo = createInfos[entryPointIndex];
            const std::string shaderName = shaderFilename + "_" + createInfo.EntryPoint;
            std::filesystem::path shaderCacheFilePath = shaderCachePath / (shaderName + ".spv");
	
			Slang::ComPtr<slang::IBlob> spirvBlob;
			{
				Slang::ComPtr<slang::IBlob> diagnosticBlob;
				SlangResult result = program->getEntryPointCode(
					entryPointIndex,
					0,
					spirvBlob.writeRef(),
					diagnosticBlob.writeRef()
				);
				if (diagnosticBlob)
				{
					std::cerr << reinterpret_cast<const char*>(diagnosticBlob->getBufferPointer()) << std::endl;
					IIIXRLAB_DEBUG_BREAK();
				}
				assert(result == SLANG_OK);
				assert(spirvBlob != nullptr);
			}
	
			Slang::ComPtr<slang::IBlob> spirvAsmBlob;
			{
				Slang::ComPtr<slang::IBlob> diagnosticBlob;
				SlangResult result = program->getEntryPointCode(
					entryPointIndex,
					1,
					spirvAsmBlob.writeRef(),
					diagnosticBlob.writeRef()
				);
				if (diagnosticBlob)
				{
					std::cerr << reinterpret_cast<const char*>(diagnosticBlob->getBufferPointer()) << std::endl;
					IIIXRLAB_DEBUG_BREAK();
				}
				assert(result == SLANG_OK);
				assert(spirvAsmBlob != nullptr);
			}
	
			std::ofstream ofs(shaderCacheFilePath.string(), std::ios::binary);
			assert(ofs.is_open());
			ofs.write(reinterpret_cast<const char*>(spirvBlob->getBufferPointer()), spirvBlob->getBufferSize());
			ofs.close();
			
			const std::filesystem::path shaderCacheAsmPath = shaderAbsPath.parent_path() / "asms";
			if (!std::filesystem::exists(shaderCacheAsmPath))
			{
				std::filesystem::create_directories(shaderCacheAsmPath);
			}
			std::filesystem::path shaderCacheAsmFilePath = shaderCacheAsmPath / (shaderName + ".asm");
			ofs.open(shaderCacheAsmFilePath.string(), std::ios::binary);
			assert(ofs.is_open());
			ofs.write(reinterpret_cast<const char*>(spirvAsmBlob->getBufferPointer()), spirvAsmBlob->getBufferSize());
			ofs.close();

			createInfo.Path = shaderCacheFilePath;
	
			std::unique_ptr<Shader> shader = std::make_unique<Shader>(createInfo);
			mShaders.insert({shaderName, std::move(shader)});
		}
	}

	void ShaderManager::DestroyShaders() noexcept
	{
		for (auto& [name, shader] : mShaders)
		{
			shader.reset();
		}
		mShaders.clear();
	}
} // namespace iiixrlab::graphics
