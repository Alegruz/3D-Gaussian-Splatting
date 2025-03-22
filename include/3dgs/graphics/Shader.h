#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class Device;
	
	class Shader final
	{
	public:
		enum class eType : uint8_t
		{
			UNKNOWN = 0,
			VERTEX = 1,
			TESSELLATION_CONTROL = 2,
			TESSELLATION_EVALUATION = 3,
			GEOMETRY = 4,
			FRAGMENT = 5,
			COMPUTE = 6,
			COUNT,
		};

		struct CreateInfo final
		{
			Device&					Device;
			std::filesystem::path   Path;
			std::string             EntryPoint;
			eType                   Type;
		};
	
	public:
		Shader() = delete;

		Shader(const CreateInfo& createInfo) noexcept;

		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		~Shader() noexcept;

		Shader(Shader&&) = delete;
		Shader& operator=(Shader&&) = delete;

		IIIXRLAB_INLINE constexpr const std::string& GetEntryPoint() const noexcept { return mEntryPoint; }
		IIIXRLAB_INLINE constexpr VkShaderModule GetShaderModule() const noexcept { return mShaderModule; }
		IIIXRLAB_INLINE constexpr eType GetType() const noexcept { return mType; }
		
	private:
		Device& mDevice;
		std::string mEntryPoint;
		VkShaderModule mShaderModule;
		eType mType;
	};
} // namespace iiixrlab::graphics
