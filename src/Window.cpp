#include "3dgs/Window.h"

#include "3dgs/InputManager.h"

namespace iiixrlab
{
#if defined(_WIN32)
	LRESULT CALLBACK Window::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		Window* pWindow;
		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWindow);
		}
		else
		{
			pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (pWindow)
		{
			return pWindow->HandleMessage(uMsg, wParam, lParam);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
	{
		static bool bIsRightMouseButtonDown = false;

		switch (uMsg)
		{
		case WM_CLOSE:
		{
			HMENU hMenu;
			hMenu = GetMenu(mhWnd);
			if (mhMenu != NULL)
			{
				DestroyMenu(mhMenu);
			}
			DestroyWindow(mhWnd);
			UnregisterClass(
				mProjectNameW.c_str(),
				mhInstance
			);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
		{
			InputManager& inputManager = InputManager::GetInstance();
			inputManager.OnKeyDown(static_cast<uint8_t>(wParam));
		}
		break;
		case WM_KEYUP:
		{
			InputManager& inputManager = InputManager::GetInstance();
			inputManager.OnKeyUp(static_cast<uint8_t>(wParam));
		}
		break;
		case WM_KILLFOCUS:
		{
			mbIsInFocus = false;
		}
		break;
		case WM_INPUT:
		{
			uint32_t uDataSize = 0u;

			GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&uDataSize,
				sizeof(RAWINPUTHEADER)
			);
			if (uDataSize > 0 && mbIsInFocus)
			{
				std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(uDataSize);
				if (GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lParam),
					RID_INPUT,
					rawData.get(),
					&uDataSize,
					sizeof(RAWINPUTHEADER)
				))
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						RAWMOUSE& mouse = raw->data.mouse;
						if (bIsRightMouseButtonDown == true)
						{
							if ((mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == 0 && (mouse.lLastX != 0 && mouse.lLastY != 0))
							{
								float X = static_cast<float>(mouse.lLastX);
								float Y = static_cast<float>(mouse.lLastY);
								const iiixrlab::math::Vector2f ssDeltaPosition{X, Y};
								InputManager& inputManager = InputManager::GetInstance();
								inputManager.OnMouseMove(ssDeltaPosition);
							}
						}
					}
				}
			}
		}
		break;
		case WM_MOUSELEAVE:
		{
			bIsRightMouseButtonDown = false;
		}
		break;
		case WM_RBUTTONDOWN:
		{
			bIsRightMouseButtonDown = true;
		}
		break;
		case WM_RBUTTONUP:
		{
			bIsRightMouseButtonDown = false;
		}
		break;
		case WM_SETFOCUS:
		{
			mbIsInFocus = true;
		}
		break;
		default:
			return DefWindowProc(mhWnd, uMsg, wParam, lParam);
		}

		return 0;
	}
#endif  // defined(_WIN32)

	Window::Window(const ApplicationInfo& applicationInfo) noexcept
		: mApplicationInfo(applicationInfo)
		, mProjectNameW(applicationInfo.Info.Name.begin(), applicationInfo.Info.Name.end())
		, mbIsInFocus(true)
#if defined(_WIN32)
		, mhInstance(static_cast<HINSTANCE>(GetModuleHandle(nullptr)))
		, mhWnd(nullptr)
		, mhMenu(nullptr)
		, mRect()
#endif  // defined(_WIN32)
	{
#if defined(_WIN32)
		RAWINPUTDEVICE rid =
		{
			.usUsagePage = 0x01,	// MOUSE	
			.usUsage = 0X02,
			.dwFlags = 0u,			// Default flags
			.hwndTarget = NULL
		};

		BOOL bResult = RegisterRawInputDevices(&rid, 1u, sizeof(rid));
		assert(FAILED(bResult) == FALSE);

		HICON hIcon = NULL;
		TCHAR szExePath[MAX_PATH];
		GetModuleFileName(NULL, szExePath, MAX_PATH);

		// If the icon is NULL, then use the first one found in the exe
		if (hIcon == NULL)
		{
			hIcon = ExtractIcon(mhInstance, szExePath, 0);
		}

		// Register the windows class
		WNDCLASS wndClass;
		wndClass.style = CS_DBLCLKS;
		wndClass.lpfnWndProc = StaticWindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = mhInstance;
		wndClass.hIcon = hIcon;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = mProjectNameW.c_str();

		if (!RegisterClass(&wndClass))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			{
				const HRESULT hr = HRESULT_FROM_WIN32(dwError);
				std::cout << std::hex << "HRESULT: " << hr << '\n';
			}
			else
			{
				std::cout << std::hex << "error: " << dwError << '\n';
			}
		}

		const int x = CW_USEDEFAULT;
		const int y = CW_USEDEFAULT;

		// No menu in this example.
		mhMenu = NULL;

		const int nDefaultWidth = mApplicationInfo.Width;
		const int nDefaultHeight = mApplicationInfo.Height;
		SetRect(&mRect, 0, 0, nDefaultWidth, nDefaultHeight);
		AdjustWindowRect(
			&mRect,
			WS_OVERLAPPEDWINDOW,
			(mhMenu != NULL) ? true : false
		);

		// Create the window for our viewport.
		mhWnd = CreateWindowEx(
			0,
			mProjectNameW.c_str(),
			mProjectNameW.c_str(),
			WS_OVERLAPPEDWINDOW,
			x, y,
			(mRect.right - mRect.left), (mRect.bottom - mRect.top),
			0,
			mhMenu,
			mhInstance,
			this
		);

		if (mhWnd == NULL)
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			{
				const HRESULT hr = HRESULT_FROM_WIN32(dwError);
				std::cout << std::hex << "HRESULT: " << hr << '\n';
			}
			else
			{
				std::cout << std::hex << "error: " << dwError << '\n';
			}
			IIIXRLAB_DEBUG_BREAK();
		}

		bResult = ShowWindow(mhWnd, SW_SHOWDEFAULT);
		if (FAILED(bResult))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_CLASS_ALREADY_EXISTS)
			{
				const HRESULT hr = HRESULT_FROM_WIN32(dwError);
				std::cout << std::hex << "HRESULT: " << hr << '\n';
			}
			else
			{
				std::cout << std::hex << "error: " << dwError << '\n';
			}
			IIIXRLAB_DEBUG_BREAK();
		}
#endif  // defined(_WIN32)
	}

	bool Window::HandleEvents(bool& outQuitApplication) noexcept
	{
#if defined(_WIN32)
		MSG msg;
		const BOOL bHasMessage = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		if (bHasMessage == TRUE)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		outQuitApplication = msg.message == WM_QUIT;
		return bHasMessage == TRUE;
#else
#error 0;
#endif  // defined(_WIN32)
	}
} // namespace iiixrlab
