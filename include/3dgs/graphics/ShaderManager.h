#pragma once

#include "pch.h"

#include "3dgs/graphics/Shader.h"

namespace iiixrlab::graphics
{
    class ShaderManager final
    {
    public:
        static ShaderManager& GetInstance() noexcept;

    public:
        ShaderManager(const ShaderManager&) = delete;
        ShaderManager& operator=(const ShaderManager&) = delete;

        ~ShaderManager() noexcept;

        ShaderManager(ShaderManager&&) = delete;
        ShaderManager& operator=(ShaderManager&&) = delete;

        IIIXRLAB_INLINE std::unique_ptr<Shader>* GetShaderOrNull(const std::string& name) noexcept { auto result = mShaders.find(name); return result != mShaders.end() ? &result->second : nullptr; }
        IIIXRLAB_INLINE const std::unique_ptr<Shader>* GetShaderOrNull(const std::string& name) const noexcept { const auto result = mShaders.find(name); return result != mShaders.cend() ? &result->second : nullptr; }

        void AddShaders(std::vector<Shader::CreateInfo>& createInfo) noexcept;
        void DestroyShaders() noexcept;

    private:
        ShaderManager() noexcept;
    
    private:
        Slang::ComPtr<slang::IGlobalSession> mGlobalSession;
        std::unordered_map<std::string, std::unique_ptr<Shader>> mShaders;
    };
} // namespace iiixrlab::graphics
