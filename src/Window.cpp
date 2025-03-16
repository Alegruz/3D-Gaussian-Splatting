#include "3dgs/Window.h"

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

        switch (uMsg)
        {
        case WM_CLOSE:
        {
            HMENU hMenu;
            hMenu = GetMenu(hWnd);
            if (hMenu != NULL)
            {
                DestroyMenu(hMenu);
            }
            DestroyWindow(hWnd);
            UnregisterClass(
                pWindow->mProjectNameW.c_str(),
                pWindow->mhInstance
            );
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }
#endif  // defined(_WIN32)

    Window::Window(const ApplicationInfo& applicationInfo) noexcept
        : mApplicationInfo(applicationInfo)
#if defined(_WIN32)
        , mhInstance(static_cast<HINSTANCE>(GetModuleHandle(nullptr)))
#endif  // defined(_WIN32)
    {
        mProjectNameW.assign(mApplicationInfo.Info.Name.begin(), mApplicationInfo.Info.Name.end());

#if defined(_WIN32)
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

        const BOOL bResult = ShowWindow(mhWnd, SW_SHOWDEFAULT);
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
