#pragma once

#include "pch.h"

#include "3dgs/graphics/Buffer.h"

namespace iiixrlab::graphics
{
    class VertexBuffer final : public Buffer
    {
    public:
		friend class CommandBuffer;
		friend class Device;

	public:
        VertexBuffer() = delete;

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        ~VertexBuffer() noexcept = default;

        IIIXRLAB_INLINE constexpr VertexBuffer(VertexBuffer&& other) noexcept = default;
        VertexBuffer& operator=(VertexBuffer&&) = delete;

    protected:
        IIIXRLAB_INLINE constexpr VertexBuffer(const CreateInfo& createInfo) noexcept
			: Buffer(createInfo)
		{
		}
    };
} // namespace iiixrlab::graphics
