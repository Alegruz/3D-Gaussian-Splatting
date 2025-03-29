#pragma once

#include "3dgs/graphics/IRenderScene.h"

#include "3dgs/scene/Camera.h"

#include "3dgs/InputManager.h"

namespace iiixrlab::graphics
{
    IIIXRLAB_INLINE IRenderScene::IRenderScene(CreateInfo& createInfo) noexcept
        : mDevice(createInfo.Device)
        , mPipelines(std::move(createInfo.Pipelines))
        , mVertexBuffer()
		, mCamera()
    {
        iiixrlab::scene::Camera::CreateInfo cameraCreateInfo =
        {
            .Device = mDevice,
            .Position = iiixrlab::math::Vector3f{ 0.0f, 0.0f, 0.0f },
            .Width = createInfo.Width,
            .Height = createInfo.Height,
        };
        iiixrlab::scene::Camera camera(cameraCreateInfo);
        mCamera = std::make_unique<iiixrlab::scene::Camera>(std::move(camera));
    }

    IIIXRLAB_INLINE IRenderScene::~IRenderScene() noexcept
    {
        for (auto& pipeline : mPipelines)
        {
            pipeline.second.reset();
        }
        mPipelines.clear();

        if (mVertexBuffer != nullptr)
        {
            mVertexBuffer.reset();
        }
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr TRenderScene<TRenderable>::TRenderScene(IRenderScene::CreateInfo& createInfo) noexcept
        : IRenderScene(createInfo)
    {}

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr TRenderScene<TRenderable>::~TRenderScene() noexcept
    {
        mRenderables.clear();
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE constexpr void TRenderScene<TRenderable>::AddRenderable(std::unique_ptr<TRenderable>&& renderable) noexcept
    {
        mRenderables.push_back(std::move(renderable));
    }

	template<Renderable TRenderable>
    IIIXRLAB_INLINE void TRenderScene<TRenderable>::update(CommandBuffer& commandBuffer, const float deltaTime) noexcept
    {
        for (auto& renderable : mRenderables)
        {
            renderable->Update(commandBuffer, deltaTime);
        }

		iiixrlab::math::Vector3f direction;
        InputManager& inputManager = InputManager::GetInstance();
		const iiixrlab::InputManager::eKeyState leftKeyState = inputManager.GetKeyState('A');
		if (leftKeyState == iiixrlab::InputManager::eKeyState::PRESSED || leftKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
		{
			direction.GetX() -= 1.0f;
		}
		else
		{
			const iiixrlab::InputManager::eKeyState rightKeyState = inputManager.GetKeyState('D');
			if (rightKeyState == iiixrlab::InputManager::eKeyState::PRESSED || rightKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
			{
				direction.GetX() += 1.0f;
			}
		}

		const iiixrlab::InputManager::eKeyState frontKeyState = inputManager.GetKeyState('W');
        if (frontKeyState == iiixrlab::InputManager::eKeyState::PRESSED || frontKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
        {
            direction.GetZ() += 1.0f;
        }
        else
        {
            const iiixrlab::InputManager::eKeyState backKeyState = inputManager.GetKeyState('S');
            if (backKeyState == iiixrlab::InputManager::eKeyState::PRESSED || backKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
            {
                direction.GetZ() -= 1.0f;
            }
        }

		const iiixrlab::InputManager::eKeyState upKeyState = inputManager.GetKeyState('E');
        if (upKeyState == iiixrlab::InputManager::eKeyState::PRESSED || upKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
        {
            direction.GetY() -= 1.0f;
        }
        else
        {
            const iiixrlab::InputManager::eKeyState downKeyState = inputManager.GetKeyState('Q');
            if (downKeyState == iiixrlab::InputManager::eKeyState::PRESSED || downKeyState == iiixrlab::InputManager::eKeyState::PRESSING)
            {
                direction.GetY() += 1.0f;
            }
        }

        const iiixrlab::math::Vector2f& ssDeltaPosition = inputManager.GetMouseDeltaPosition();
        const iiixrlab::math::Vector3f pitchYawRoll = mCamera->GetPitchYawRollFromScreenSpaceDeltaPosition(ssDeltaPosition);
        
        mCamera->Update(deltaTime, direction, pitchYawRoll);

        updateInner(commandBuffer, deltaTime);
    }
} // namespace iiixrlab::graphics
