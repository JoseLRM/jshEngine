#pragma once

#include "common.h"

namespace jshInput {

	bool IsKey(uint8 id);
	bool IsKeyPressed(uint8 id);
	bool IsKeyReleased(uint8 id);

	bool IsMouse(uint8 id);
	bool IsMousePressed(uint8 id);
	bool IsMouseReleased(uint8 id);

#ifdef JSH_ENGINE

	void Update();

	void KeyDown(uint8 id);
	void KeyUp(uint8 id);

	void MouseDown(uint8 id);
	void MouseUp(uint8 id);

	void MousePos(uint16 mouseX, uint16 mouseY);

#endif 

}