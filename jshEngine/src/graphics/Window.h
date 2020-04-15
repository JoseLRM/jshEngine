#pragma once

#include "..//common.h"

namespace jshWindow {

#ifdef JSH_ENGINE
	bool Initialize();
	bool Close();
	bool UpdateInput();
#endif
	void SetTitle(const wchar* const title);

	void* GetWindowHandle();
	void* GetConsoleHandle();

	void SetBounds(int x, int y, int width, int height);

	int GetX();
	int GetY();
	int GetWidth();
	int GetHeight();

	void ShowMouse();
	void HideMouse();
}