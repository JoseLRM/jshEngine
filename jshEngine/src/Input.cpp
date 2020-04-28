#include "Input.h"

#include <vector>
#include "EventSystem.h"
#include "Debug.h"
#include "Window.h"

using namespace jsh;

namespace jshInput {

	bool g_Keys[255];
	bool g_KeysPressed[255];
	bool g_KeysReleased[255];

	bool g_Mouse[3];
	bool g_MousePressed[3];
	bool g_MouseReleased[3];

	vec2 g_Pos;
	vec2 g_RPos;
	vec2 g_Dragged;

	bool IsKey(uint8 id) {
		return g_Keys[id];
	}
	bool IsKeyPressed(uint8 id) {
		return g_KeysPressed[id];
	}
	bool IsKeyReleased(uint8 id) {
		return g_KeysReleased[id];
	}

	bool IsMouse(uint8 id) {
		return g_Mouse[id];
	}
	bool IsMousePressed(uint8 id) {
		return g_MousePressed[id];
	}
	bool IsMouseReleased(uint8 id) {
		return g_MouseReleased[id];
	}

	jsh::vec2 MousePos()
	{
		return g_Pos;
	}
	jsh::vec2 MouseRPos()
	{
		return g_RPos;
	}
	jsh::vec2 MouseDragged()
	{
		return g_Dragged;
	}

	void Update()
	{
		// KEYS
		// events
		for (uint8 i = 0; i < 255; ++i) {
			if (g_Keys[i]) {
				KeyEvent e(i);
				if (g_KeysPressed[i]) e = KeyEvent(i, JSH_EVENT_PRESSED);
				else if (g_KeysReleased[i]) e = KeyEvent(i, JSH_EVENT_RELEASED);
			
				jshEvent::Dispatch(e);
			}
		}
		// reset pressed and released
		for (uint8 i = 0; i < 255; ++i) {
			g_KeysPressed[i] = false;
			g_KeysReleased[i] = false;
		}

		// MOUSE BUTTONS
		// events
		for (uint8 i = 0; i < 3; ++i) {
			if (g_Mouse[i]) {
				MouseEvent e(JSH_EVENT_BUTTON);
				jshEvent::Dispatch(e);

				MouseButtonEvent be(i);
				if (g_MousePressed[i]) be = MouseButtonEvent(i, JSH_EVENT_PRESSED);
				else if (g_MouseReleased[i]) be = MouseButtonEvent(i, JSH_EVENT_RELEASED);
				jshEvent::Dispatch(be);
			}
		}

		// reset pressed and released
		for (uint8 i = 0; i < 3; ++i) {
			g_MousePressed[i] = false;
			g_MouseReleased[i] = false;
		}

		g_RPos = g_Pos;
		g_Dragged.x = 0.f;
		g_Dragged.y = 0.f;
	}

	void KeyDown(uint8 id) {
		g_Keys[id] = true;
		g_KeysPressed[id] = true;

		KeyPressedEvent keyPressedEvent(id);
		jshEvent::Dispatch(keyPressedEvent);
	}
	void KeyUp(uint8 id) {
		g_Keys[id] = false;
		g_KeysReleased[id] = true;

		KeyReleasedEvent keyReleasedEvent(id);
		jshEvent::Dispatch(keyReleasedEvent);
	}
	void MouseDown(uint8 id) {
		g_Mouse[id] = true;
		g_MousePressed[id] = true;

		MouseButtonPressedEvent mousePressedEvent(id);
		jshEvent::Dispatch(mousePressedEvent);
	}
	void MouseUp(uint8 id) {
		g_Mouse[id] = false;
		g_MouseReleased[id] = true;

		MouseButtonReleasedEvent mouseReleasedEvent(id);
		jshEvent::Dispatch(mouseReleasedEvent);
	}
	void MousePos(uint16 x, uint16 y)
	{
		g_Pos.x = ((float)x / jshWindow::GetWidth()) - 0.5f;
		g_Pos.y = -(((float)y / jshWindow::GetHeight()) - 0.5f);
	}

	void MouseDragged(int dx, int dy)
	{
		g_Dragged.x = dx;
		g_Dragged.y = dy;

		MouseDraggedEvent e(g_Pos.x, g_Pos.y, g_Dragged.x, g_Dragged.y);
		jshEvent::Dispatch(e);

		MouseEvent e2(JSH_EVENT_DRAGGED);
		jshEvent::Dispatch(e2);
	}

}