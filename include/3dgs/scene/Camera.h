#pragma once

#include "pch.h"

namespace iiixrlab::graphics
{
	class ConstantBuffer;
	class Device;
	class IRenderScene;
}	// namespace iiixrlab::graphics

namespace iiixrlab::scene
{
	class Camera final
	{
	public:
		friend class iiixrlab::graphics::IRenderScene;

	public:
		struct CreateInfo
		{
			iiixrlab::graphics::Device& Device;
			iiixrlab::math::Vector3f Position;
			float Width;
			float Height;
		};

		struct Info final
		{
			iiixrlab::math::Matrix4x4f View;
			iiixrlab::math::Matrix4x4f Projection;
		};

	public:
		Camera() = delete;

		Camera(const Camera&) = delete;
		Camera& operator=(const Camera&) = delete;

		~Camera() noexcept;

		Camera(Camera&& other) noexcept;
		Camera& operator=(Camera&&) = delete;

		IIIXRLAB_INLINE constexpr const Info& GetInfo() const noexcept { return mInfo; }
		IIIXRLAB_INLINE constexpr const iiixrlab::math::Vector3f& GetPosition() const noexcept { return mPosition; }
		IIIXRLAB_INLINE constexpr const iiixrlab::math::Vector3f& GetPitchYawRoll() const noexcept { return mPitchYawRoll; }
		IIIXRLAB_INLINE constexpr float GetSpeed() const noexcept { return mSpeed; }
		IIIXRLAB_INLINE iiixrlab::graphics::ConstantBuffer& GetConstantBuffer() noexcept { return *mConstantBuffer; }
		IIIXRLAB_INLINE const iiixrlab::graphics::ConstantBuffer& GetConstantBuffer() const noexcept { return *mConstantBuffer; }

		void Update(const float deltaTime, const iiixrlab::math::Vector3f& direction, const iiixrlab::math::Vector3f& pitchYawRoll) noexcept;
		iiixrlab::math::Vector3f GetPitchYawRollFromScreenSpaceDeltaPosition(const iiixrlab::math::Vector2f& deltaPosition) const noexcept;

	protected:
		Camera(const CreateInfo& createInfo) noexcept;

		void updateViewMatrix(const iiixrlab::math::Vector3f& position, const float pitch, const float yaw) noexcept;

	protected:
		iiixrlab::math::Vector3f	mPosition;
		iiixrlab::math::Vector3f	mPitchYawRoll;
		float		mDistanceToOutput;

		Info		mInfo;
		float		mSpeed;

		std::unique_ptr<iiixrlab::graphics::ConstantBuffer> mConstantBuffer;
	};
} // namespace iiixrlab::scene
