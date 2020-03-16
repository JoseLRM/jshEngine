#include "Window.h"

#include "WinLib.h"

namespace jshWindow {

	HWND windowHandle;
	HWND consoleHandle;

	int screenX = 0;
	int screenY = 0;
	int screenW = 1080;
	int screenH = 720;

	LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message) {
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

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