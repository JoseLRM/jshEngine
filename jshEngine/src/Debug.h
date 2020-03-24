#pragma once

#include "common.h"

namespace jshDebug {

#ifdef JSH_ENGINE
	bool Initialize();
#endif

	bool InDebugMode();

	void ShowConsole();
	void HideConsole();

#ifdef JSH_CONSOLE
	#include <stdio.h>
	#ifdef JSH_ENGINE
		#define jshLogE(x, ...) printf("[JSH][ERROR] "x"\n", __VA_ARGS__)
		#define jshLogI(x, ...) printf("[JSH][INFO] "x"\n", __VA_ARGS__) 
		#define jshLogW(x, ...) printf("[JSH][WARNING] "x"\n", __VA_ARGS__)
	#else
		#define jshLogE(x, ...) printf("[ERROR] "x"\n", __VA_ARGS__)
		#define jshLogI(x, ...) printf("[INFO] "x"\n", __VA_ARGS__) 
		#define jshLogW(x, ...) printf("[WARNING] "x"\n", __VA_ARGS__)
	#endif
	#define jshLog(x, ...) printf(x, __VA_ARGS__)
	#define jshLogln(x, ...) printf(x"\n", __VA_ARGS__)
#else
	#define jshLog(x, ...) 
	#define jshLogln(x, ...)
	#define jshLogE(x, ...)
	#define jshLogI(x, ...)
	#define jshLogW(x, ...)
#endif 

	void ShowOkWindow(const wchar* const description, uint8 level = 0);

#ifdef JSH_IMGUI
	void ShowImGuiWindow();
#endif

}