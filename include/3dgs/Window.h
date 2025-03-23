#pragma once

#include "3dgs/Common.h"

namespace iiixrlab
{
	class Window final
	{
	public:
#if defined(_WIN32)
		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
#endif  // defined(_WIN32)
	
	public:
		Window() = delete;
		Window(const Window&) = delete;
		Window(Window&&) = delete;

		Window(const ApplicationInfo& applicationInfo) noexcept;
		IIIXRLAB_INLINE ~Window() noexcept = default;

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

		bool HandleEvents(bool& outQuitApplication) noexcept;

		IIIXRLAB_INLINE constexpr uint32_t GetWidth() const noexcept { return mApplicationInfo.Width; }
		IIIXRLAB_INLINE constexpr uint32_t GetHeight() const noexcept { return mApplicationInfo.Height; }

#if defined(_WIN32)
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

		IIIXRLAB_INLINE constexpr HINSTANCE GetInstance() const noexcept { return mhInstance; }
		IIIXRLAB_INLINE constexpr HWND GetWindow() const noexcept { return mhWnd; }
#endif  // defined(_WIN32)

	private:
		ApplicationInfo mApplicationInfo;
		std::wstring    mProjectNameW;
		bool			mbIsInFocus;
		
#if defined(_WIN32)
		HINSTANCE       mhInstance;
		HWND            mhWnd;
		HMENU           mhMenu;
		RECT            mRect;
#endif  // defined(_WIN32)
	};
} // namespace iiixrlab
