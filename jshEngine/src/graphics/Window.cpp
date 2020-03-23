#include "Window.h"

#include "WinLib.h"
#include "Input.h"
#include "EventSystem.h"
#include "Debug.h"

#include "..//ImGui/imgui.h"
#include "..//ImGui/imgui_impl_win32.h"

using namespace jsh;

namespace jshWindow {

	HWND windowHandle;
	HWND consoleHandle;

	int screenX = 0;
	int screenY = 0;
	int screenW = 1080;
	int screenH = 720;

	LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifdef JSH_IMGUI
		ImGui_ImplWin32_WndProcHandler(windowHandle, message, wParam, lParam);
#endif

		switch (message) {
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			// input
			uint8 keyCode = (uint8)wParam;
			if (keyCode > 255) jshLogW("Unknown keycode: %u", keyCode);
			else if (~lParam & (1 << 30)) jshInput::KeyDown(keyCode);

			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			// input
			uint8 keyCode = (uint8)wParam;
			if (keyCode > 255) jshLogW("Unknown keycode: %u", keyCode);
			else jshInput::KeyUp(keyCode);

			break;
		}
		case WM_LBUTTONDOWN:
			jshInput::MouseDown(0);
			break;
		case WM_RBUTTONDOWN:
			jshInput::MouseDown(1);
			break;
		case WM_MBUTTONDOWN:
			jshInput::MouseDown(2);
			break;
		case WM_LBUTTONUP:
			jshInput::MouseUp(0);
			break;
		case WM_RBUTTONUP:
			jshInput::MouseUp(1);
			break;
		case WM_MBUTTONUP:
			jshInput::MouseUp(2);
			break;
		case WM_MOUSEMOVE:
			uint16 x = LOWORD(lParam);
			uint16 y = HIWORD(lParam);
			jshInput::MousePos(x, y);
			break;
		}

		return DefWindowProcW(windowHandle, message, wParam, lParam);
	}

	bool Initialize()
	{
		// register class
		WNDCLASSW windowClass;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hbrBackground = NULL;
		windowClass.hCursor = NULL;
		windowClass.hIcon = NULL;
		windowClass.hInstance = NULL;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.lpszClassName = L"jshWindow";
		windowClass.lpszMenuName = L"jshWindow";
		windowClass.style = NULL;

		RegisterClassW(&windowClass);

		// create class
	 	windowHandle = CreateWindowExW(
			NULL, L"jshWindow", L"jshWindow", 
			WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
			screenX, screenY, screenW, screenH, NULL, NULL, NULL, NULL
		);
		if (windowHandle == NULL) return false;
		// show window
		ShowWindow(windowHandle, SW_SHOWDEFAULT);

		consoleHandle = GetConsoleWindow();

		return true;
	}

	bool Close()
	{
		return CloseWindow(windowHandle);
	}

	bool UpdateInput()
	{
		jshInput::Update();

		MSG message;

		while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&message);
			DispatchMessageW(&message);

			if (message.message == WM_QUIT) 
				return false;
		}
		return true;
	}

	void SetTitle(const wchar* const title) {
		SetWindowTextW(windowHandle, title);
	}

	void* GetWindowHandle()
	{
		return windowHandle;
	}
	void* GetConsoleHandle()
	{
		return consoleHandle;
	}

	void SetBounds(int x, int y, int width, int height)
	{
		screenX = x;
		screenY = y;
		screenW = width;
		screenH = height;
	}

	int GetX() { return screenX; }
	int GetY() { return screenY; }
	int GetWidth() { return screenW; }
	int GetHeight() { return screenH; }
}