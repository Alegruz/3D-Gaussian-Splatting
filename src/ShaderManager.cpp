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

    void ShaderManager::AddShader(Shader::CreateInfo& createInfo) noexcept
    {
        const std::string shaderFilename = createInfo.Path.stem().string();
        const std::string shaderName = shaderFilename + "_" + createInfo.EntryPoint;
        auto ppShader = GetShaderOrNull(shaderName);
        if (ppShader != nullptr)
        {
            return;
        }

        std::filesystem::path shaderAbsPath = std::filesystem::current_path() / createInfo.Path;
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
        std::filesystem::path shaderCacheFilePath = shaderCachePath / (shaderName + ".spv");
        if (std::filesystem::exists(shaderCacheFilePath) == false)
        {
            std::vector<slang::TargetDesc> targetDescs;
            targetDescs.push_back( 
                slang::TargetDesc
                {
                    .format = SlangCompileTarget::SLANG_SPIRV,
                    .profile = mGlobalSession->findProfile("spirv_1_5"),
                    .flags = 0,
                }
            );
            targetDescs.push_back( 
                slang::TargetDesc
                {
                    .format = SlangCompileTarget::SLANG_SPIRV_ASM,
                    .profile = mGlobalSession->findProfile("SM_6_6"),
                    .flags = 0,
                }
            );
    
            std::vector<slang::CompilerOptionEntry> compilerOptions;
            compilerOptions.push_back(
                slang::CompilerOptionEntry
                {
                    .name = slang::CompilerOptionName::Include,
                    .value = slang::CompilerOptionValue
                    {
                        .kind = slang::CompilerOptionValueKind::String,
                        .stringValue0 = "assets/shaders",
                    },
                }
            );
            compilerOptions.push_back(
                slang::CompilerOptionEntry
                {
                    .name = slang::CompilerOptionName::EmitSpirvDirectly,
                    .value = slang::CompilerOptionValue
                    {
                        .kind = slang::CompilerOptionValueKind::Int,
                        .intValue0 = 1,
                    },
                }
            );
            slang::CompilerOptionEntry stageOption =
            {
                .name = slang::CompilerOptionName::Stage,
                .value = slang::CompilerOptionValue
                {
                    .kind = slang::CompilerOptionValueKind::Int,
                },
            };
    
            switch (createInfo.Type)
            {
            case Shader::eType::VERTEX:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_VERTEX;
                break;
            case Shader::eType::TESSELLATION_CONTROL:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_HULL;
                break;
            case Shader::eType::TESSELLATION_EVALUATION:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_DOMAIN;
                break;
            case Shader::eType::GEOMETRY:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_GEOMETRY;
                break;
            case Shader::eType::FRAGMENT:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_FRAGMENT;
                break;
            case Shader::eType::COMPUTE:
                stageOption.value.intValue0 = SlangStage::SLANG_STAGE_COMPUTE;
                break;
            default:
                assert(false);
                break;
            }
            compilerOptions.push_back(stageOption);
            compilerOptions.push_back(
                slang::CompilerOptionEntry
                {
                    .name = slang::CompilerOptionName::WarningsAsErrors,
                    .value = slang::CompilerOptionValue
                    {
                        .kind = slang::CompilerOptionValueKind::String,
                        .stringValue0 = "all",
                    },
                }
            );
    
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
    
            Slang::ComPtr<slang::IEntryPoint> entryPoint;
            module->findEntryPointByName(createInfo.EntryPoint.c_str(), entryPoint.writeRef());
    
            std::vector<slang::IComponentType*> componentTypes =
            {
                module,
                entryPoint,
            };
    
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
    
            Slang::ComPtr<slang::IBlob> spirvBlob;
            {
                Slang::ComPtr<slang::IBlob> diagnosticBlob;
                SlangResult result = program->getEntryPointCode(
                    0,
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
                    0,
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
        }
        createInfo.Path = shaderCacheFilePath;

        std::unique_ptr<Shader> shader = std::make_unique<Shader>(createInfo);
        mShaders.insert({shaderName, std::move(shader)});
    }
} // namespace iiixrlab::graphics
