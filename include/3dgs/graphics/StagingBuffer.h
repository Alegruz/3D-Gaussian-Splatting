#pragma once

#include "pch.h"

#include "3dgs/graphics/Buffer.h"

namespace iiixrlab::graphics
{
    class StagingBuffer final : public Buffer
    {
    public:
		friend class Device;

	public:
        StagingBuffer() = delete;

        StagingBuffer(const StagingBuffer&) = delete;
        StagingBuffer& operator=(const StagingBuffer&) = delete;

        ~StagingBuffer() noexcept = default;

        IIIXRLAB_INLINE constexpr StagingBuffer(StagingBuffer&& other) noexcept = default;
        StagingBuffer& operator=(StagingBuffer&&) = delete;

    protected:
        IIIXRLAB_INLINE constexpr StagingBuffer(const CreateInfo& createInfo) noexcept
			: Buffer(createInfo)
		{
		}
    };
} // namespace iiixrlab::graphics
