#include "3dgs/scene/Camera.h"

#include "3dgs/graphics/ConstantBuffer.h"
#include "3dgs/graphics/Device.h"

namespace iiixrlab::scene
{
	Camera::Camera(const CreateInfo& createInfo) noexcept
		: mPosition(createInfo.Position)
		, mPitchYawRoll()
		, mDistanceToOutput(0.0f)
		, mConstantBuffer()
		, mInfo()
		, mSpeed(1.0f)
	{
		updateViewMatrix(createInfo.Position, 0.0f, 0.0f);

		// Initialize the projection matrix
		constexpr const float NearPlane = 1.0f;
		constexpr const float FarPlane = 1000.0f;
		const float AspectRatio = createInfo.Width / createInfo.Height;
		constexpr const float verticalFieldOfView = static_cast<float>(std::numbers::pi_v<double> / 8.0);
		const float ProjectionPlane = ::tan(verticalFieldOfView);

		mDistanceToOutput = createInfo.Width / 2.0f * ProjectionPlane;

		mInfo.Projection = iiixrlab::math::Matrix4x4f
		({
			1.0f / (AspectRatio * ProjectionPlane),	0.0f,								0.0f,											0.0f,
			0.0f,									1.0f / ProjectionPlane,				0.0f,											0.0f,
			0.0f,									0.0f,								FarPlane / (FarPlane - NearPlane),				-NearPlane * FarPlane / (FarPlane - NearPlane),
			0.0f,									0.0f,								1.0f,											0.0f
		});
		
		mConstantBuffer = createInfo.Device.CreateConstantBuffer("CameraConstantBuffer", sizeof(mInfo));
		mConstantBuffer->SetData(&mInfo, sizeof(mInfo));
	}

	Camera::Camera(Camera&& other) noexcept
		: mPosition(other.mPosition)
		, mPitchYawRoll(other.mPitchYawRoll)
		, mDistanceToOutput(other.mDistanceToOutput)
		, mConstantBuffer(std::move(other.mConstantBuffer))
		, mInfo(other.mInfo)
		, mSpeed(other.mSpeed)
	{
	}

	Camera::~Camera() noexcept
	{
		mConstantBuffer.reset();
	}

	void Camera::Update(const float deltaTime, const iiixrlab::math::Vector3f& direction, const iiixrlab::math::Vector3f& pitchYawRoll) noexcept
	{
		bool bNeedsToUpdateCamera = false;
		if (direction.IsSizeZero() == false)
		{
			const iiixrlab::math::Vector3f unitDirection = iiixrlab::math::Vector3f::Normalize(direction);
			const iiixrlab::math::Vector3f deltaTranslate = unitDirection * deltaTime * mSpeed;
			mPosition += deltaTranslate;
			bNeedsToUpdateCamera = true;
		}

		if (pitchYawRoll.IsSizeZero() == false)
		{
			mPitchYawRoll += pitchYawRoll;
			bNeedsToUpdateCamera = true;
		}

		if (bNeedsToUpdateCamera)
		{
			updateViewMatrix(mPosition, mPitchYawRoll.GetX(), mPitchYawRoll.GetY());
			mConstantBuffer->SetData(&mInfo, sizeof(mInfo));
		}
	}

	iiixrlab::math::Vector3f Camera::GetPitchYawRollFromScreenSpaceDeltaPosition(const iiixrlab::math::Vector2f& deltaPosition) const noexcept
	{
		const float pitch = std::atanf(deltaPosition.GetY() / mDistanceToOutput);
		const float yaw = std::atanf(deltaPosition.GetX() / mDistanceToOutput);
		return iiixrlab::math::Vector3f{pitch, yaw, 0.0f};
	}

	void Camera::updateViewMatrix(const iiixrlab::math::Vector3f& position, const float pitch, const float yaw) noexcept
	{
		const float cosPitch = std::cos(pitch);
		const float sinPitch = std::sin(pitch);
		const float cosYaw = std::cos(yaw);
		const float sinYaw = std::sin(yaw);

		const iiixrlab::math::Vector3f xAxis = iiixrlab::math::Vector3f{ cosYaw, 0, -sinYaw };
		const iiixrlab::math::Vector3f yAxis = iiixrlab::math::Vector3f{ sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
		const iiixrlab::math::Vector3f zAxis = iiixrlab::math::Vector3f{ sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

		mInfo.View = iiixrlab::math::Matrix4x4f
		({
			xAxis.GetX(),   xAxis.GetY(), 	xAxis.GetZ(), 	-iiixrlab::math::Vector3f::Dot(xAxis, position),
			yAxis.GetX(),   yAxis.GetY(), 	yAxis.GetZ(), 	-iiixrlab::math::Vector3f::Dot(yAxis, position),
			zAxis.GetX(),   zAxis.GetY(), 	zAxis.GetZ(), 	-iiixrlab::math::Vector3f::Dot(zAxis, position),
			0.0f,       	0.0f, 			0.0f, 			1.0f,
		});
	}
} // namespace iiixrlab::scene