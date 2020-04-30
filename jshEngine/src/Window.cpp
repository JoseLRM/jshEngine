#include "Window.h"

#include "WinLib.h"
#include "Input.h"
#include "EventSystem.h"
#include "Debug.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "vector.h"
#include "Engine.h"
#include "Graphics.h"

using namespace jsh;

namespace jshWindow {

	HWND windowHandle;
	HWND consoleHandle;

	DWORD g_Style;

	int screenX = 0;
	int screenY = 0;
	int screenW = 1080;
	int screenH = 720;

	jsh::vector<byte> rawMouseBuffer;

	LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
	{
#ifdef JSH_IMGUI
		{
			LPARAM newLParam = lParam;

			if (message == WM_MOUSEMOVE) {
				vec2 mPos = vec2(LOWORD(lParam), HIWORD(lParam));
				mPos /= vec2(jshWindow::GetWidth(), jshWindow::GetHeight());
				uvec2 aux = jshGraphics::GetResolution();
				mPos *= vec2(aux.x, aux.y);
				aux = uvec2(mPos.x, mPos.y);

				LPARAM newLParam = *((LPARAM*)& aux.x);
			}

			ImGui_ImplWin32_WndProcHandler(windowHandle, message, wParam, newLParam);
		}
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
			if (keyCode > 255) {
				jshLogW("Unknown keycode: %u", keyCode);
			}
			else if (~lParam & (1 << 30)) jshInput::KeyDown(keyCode);

			break;
		}
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			// input
			uint8 keyCode = (uint8)wParam;
			if (keyCode > 255) {
				jshLogW("Unknown keycode: %u", keyCode);
				VK_SHIFT;
			}
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
		{
			uint16 x = LOWORD(lParam);
			uint16 y = HIWORD(lParam);
			jshInput::MousePos(x, y);
			break;
		}

		// RAW MOUSE
		case WM_INPUT:
		{
			UINT size;
			if(GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1) break;

			rawMouseBuffer.resize(size);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawMouseBuffer.data(), &size, sizeof(RAWINPUTHEADER)) == -1) break;

			RAWINPUT& rawInput = reinterpret_cast<RAWINPUT&>(*rawMouseBuffer.data());
			if (rawInput.header.dwType == RIM_TYPEMOUSE) {
				if (rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0) {
					jshInput::MouseDragged(rawInput.data.mouse.lLastX, rawInput.data.mouse.lLastY);
				}
			}
		}
			break;

		case WM_SIZE:
		{
			screenW = LOWORD(lParam);
			screenH = HIWORD(lParam);
			jsh::WindowResizedEvent e(screenW, screenH);
			jshEvent::Dispatch(e);

			break;
		}
		case WM_MOVE:
		{
			screenX = LOWORD(lParam);
			screenY = HIWORD(lParam);
			jsh::WindowMovedEvent e(screenX, screenY);
			jshEvent::Dispatch(e);
			break;
		}
		case WM_SETFOCUS:
		{
			jsh::WindowGainFocusEvent e;
			jshEvent::Dispatch(e);
			break;
		}
		case WM_KILLFOCUS:
		{
			jsh::WindowLostFocusEvent e;
			jshEvent::Dispatch(e);
			break;
		}
		}

		return DefWindowProcW(windowHandle, message, wParam, lParam);
	}

	void AdjustWindow(int x, int y, int& w, int& h)
	{
		RECT rect;
		rect.left = x;
		rect.right = x + w;
		rect.top = y;
		rect.bottom = y + h;

		AdjustWindowRect(&rect, g_Style, false);
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;
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
		g_Style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX;

		int w, h;
		w = screenW;
		h = screenH;
		AdjustWindow(screenX, screenY, w, h);

	 	windowHandle = CreateWindowExW(
			NULL, L"jshWindow", jshEngine::GetNameW(), 
			g_Style,
			screenX, screenY, w, h, NULL, NULL, NULL, NULL
		);
		if (windowHandle == NULL) return false;
		// show window
		ShowWindow(windowHandle, SW_SHOWDEFAULT);

		consoleHandle = GetConsoleWindow();

		// register raw mouse input
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 1;
		rid.usUsage = 2;
		rid.hwndTarget = nullptr;
		rid.dwFlags = 0;

		if(!RegisterRawInputDevices(&rid, 1u, sizeof(RAWINPUTDEVICE))) return false;

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

		int w, h;
		w = screenW;
		h = screenH;
		AdjustWindow(screenX, screenY, w, h);

		SetWindowPos(windowHandle, 0, screenX, screenY, w, h, 0);
	}

	int GetX() { return screenX; }
	int GetY() { return screenY; }
	int GetWidth() { return screenW; }
	int GetHeight() { return screenH; }
	bool InFullscreen() { return false; }

	void ShowMouse()
	{
		while(ShowCursor(TRUE) < 0);

		ClipCursor(nullptr);

		jshImGui(ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse);
	}

	void HideMouse()
	{
		while (ShowCursor(FALSE) >= 0);

		jshImGui(ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse);

		RECT r;
		GetClientRect(windowHandle, &r);
		MapWindowPoints(windowHandle, nullptr, (POINT*)& r, 2);
		ClipCursor(&r);
	}

}