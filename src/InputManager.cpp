#include "3dgs/InputManager.h"

namespace iiixrlab
{
	void InputManager::OnKeyUp(const uint8_t key) noexcept
	{
		mKeyStates[key] = eKeyState::RELEASING;
		mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASING)].insert(key);
		mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSING)].erase(key);
		mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSED)].erase(key);
	}

	void InputManager::OnKeyDown(const uint8_t key) noexcept
	{
		mKeyStates[key] = eKeyState::PRESSING;
		mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSING)].insert(key);
		mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASING)].erase(key);
		mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASED)].erase(key);
	}

	void InputManager::OnMouseMove(const math::Vector2f& deltaPosition) noexcept
	{
		mMouseDeltaPosition = deltaPosition;
	}

	void InputManager::Update() noexcept
	{
		for (uint8_t key : mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASING)])
		{
			assert(mKeyStates[key] == eKeyState::RELEASING);
			mKeyStates[key] = eKeyState::RELEASED;
			mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASED)].insert(key);
		}
		mKeysPerState[static_cast<uint8_t>(eKeyState::RELEASING)].clear();

		for (uint8_t key : mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSING)])
		{
			assert(mKeyStates[key] == eKeyState::PRESSING);
			mKeyStates[key] = eKeyState::PRESSED;
			mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSED)].insert(key);
		}
		mKeysPerState[static_cast<uint8_t>(eKeyState::PRESSING)].clear();
	}

	void InputManager::PostUpdate() noexcept
	{
		mMouseDeltaPosition = math::Vector2f{0.0f, 0.0f};
	}
} // namespace iiixrlab
