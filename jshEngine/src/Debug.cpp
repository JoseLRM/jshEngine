#include "Debug.h"

#include "graphics/WinLib.h"

namespace jshDebug {

	bool Initialize()
	{
#ifdef JSH_CONSOLE
		ShowConsole();
#else
		HideConsole();
#endif 
		return true;
	}

#ifdef JSH_ENGINE
#ifdef JSH_DEBUG
	bool InDebugMode() { return true; }
#else
	bool InDebugMode() { return false; }
#endif
#endif

	void ShowConsole()
	{
		HWND console = GetConsoleWindow();
		ShowWindow(console, SW_SHOWDEFAULT);
	}

	void HideConsole()
	{
		HWND console = GetConsoleWindow();
		ShowWindow(console, SW_HIDE);
	}

	void ShowOkWindow(const wchar* const description, uint8 level) {
		if(level == 0) 
			MessageBox(NULL, description, L"Just information", MB_ICONINFORMATION | MB_OK);
		else if(level == 1)
			MessageBox(NULL, description, L"Warning!", MB_ICONWARNING | MB_OK);
		else
			MessageBox(NULL, description, L"Error!!", MB_ICONERROR | MB_OK);
	}

}