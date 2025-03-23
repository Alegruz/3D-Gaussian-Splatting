#pragma once

#include "pch.h"

#include "3dgs/graphics/Buffer.h"

namespace iiixrlab::graphics
{
	class DescriptorSet;

	class ConstantBuffer final : public Buffer
	{
	public:
		friend class DescriptorSet;
		friend class Device;

	public:
		ConstantBuffer() = delete;

		ConstantBuffer(const ConstantBuffer&) = delete;
		ConstantBuffer& operator=(const ConstantBuffer&) = delete;

		~ConstantBuffer() noexcept = default;

		IIIXRLAB_INLINE constexpr ConstantBuffer(ConstantBuffer&& other) noexcept = default;
		ConstantBuffer& operator=(ConstantBuffer&&) = delete;

		bool SetData(const void* data, const size_t size) noexcept;

	protected:
		ConstantBuffer(const CreateInfo& createInfo) noexcept;

	protected:
		uint8_t* mMappedData;
	};
} // namespace iiixrlab::graphics
