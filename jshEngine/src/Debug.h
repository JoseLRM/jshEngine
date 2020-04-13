#pragma once

#include "common.h"

namespace jsh {
	class Mesh;
}

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
		#define jshLogE(x, ...) printf("\n[JSH][ERROR] "); printf(x, __VA_ARGS__)
		#define jshLogI(x, ...) printf("\n[JSH][INFO] "); printf(x, __VA_ARGS__)	
		#define jshLogW(x, ...) printf("\n[JSH][WARNING] "); printf(x, __VA_ARGS__)
	#else
		#define jshLogE(x, ...) printf("\n[ERROR] "); printf(x, __VA_ARGS__)
		#define jshLogI(x, ...) printf("\n[INFO] "); printf(x, __VA_ARGS__)
		#define jshLogW(x, ...) printf("\n[WARNING] "); printf(x, __VA_ARGS__)
	#endif
	#define jshLog(x, ...)	 printf(x, __VA_ARGS__)
	#define jshLogln(x, ...) printf("\n"); printf(x, __VA_ARGS__)
#else
	#define jshLog(x, ...) 
	#define jshLogln(x, ...)
	#define jshLogE(x, ...)
	#define jshLogI(x, ...)
	#define jshLogW(x, ...)
#endif 

	void ShowOkWindow(const wchar* const description, uint8 level = 0);
	void ShowOkWindow(const wchar* const title, const wchar* const description, uint8 level = 0);

#ifdef JSH_IMGUI
	void ShowImGuiWindow();

	void ShowMeshImGuiWindow(jsh::Mesh* mesh);
#endif

}