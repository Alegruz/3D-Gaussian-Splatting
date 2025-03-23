#pragma once

#include "pch.h"

namespace iiixrlab
{
    class InputManager final
    {
    public:
        enum class eKeyState : uint8_t
        {
            IDLE = 0,
            RELEASING = 1,
            RELEASED = 2,
            PRESSING = 3,
            PRESSED = 4,
            COUNT,
        };

    public:
        static IIIXRLAB_INLINE InputManager& GetInstance() noexcept
        {
            static InputManager instance;
            return instance;
        }

    public:
        InputManager(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;

        ~InputManager() noexcept = default;

        InputManager& operator=(const InputManager&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        IIIXRLAB_INLINE constexpr eKeyState GetKeyState(const uint8_t key) const noexcept { return mKeyStates[key]; }
        IIIXRLAB_INLINE constexpr const math::Vector2f& GetMouseDeltaPosition() const noexcept { return mMouseDeltaPosition; }

        void OnKeyUp(const uint8_t key) noexcept;
        void OnKeyDown(const uint8_t key) noexcept;
        void OnMouseMove(const math::Vector2f& deltaPosition) noexcept;

        void Update() noexcept;

    private:
        IIIXRLAB_INLINE constexpr InputManager() noexcept = default;
        
    private:
        eKeyState mKeyStates[256];
        std::unordered_set<uint8_t> mKeysPerState[static_cast<uint8_t>(eKeyState::COUNT)];
        math::Vector2f mMouseDeltaPosition;
    };
} // namespace iiixrlab
