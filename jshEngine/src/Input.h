#pragma once

#include "common.h"

#define JSH_MOUSE_LEFT			0
#define JSH_MOUSE_RIGHT			1
#define JSH_MOUSE_CENTER        2

#define JSH_KEY_BACK           0x08
#define JSH_KEY_TAB            0x09
#define JSH_KEY_CLEAR          0x0C
#define JSH_KEY_RETURN         0x0D
#define JSH_KEY_SHIFT          0x10
#define JSH_KEY_CONTROL        0x11
#define JSH_KEY_MENU           0x12
#define JSH_KEY_PAUSE          0x13
#define JSH_KEY_CAPITAL        0x14
#define JSH_KEY_KANA           0x15
#define JSH_KEY_HANGEUL        0x15  
#define JSH_KEY_HANGUL         0x15
#define JSH_KEY_JUNJA          0x17
#define JSH_KEY_FINAL          0x18
#define JSH_KEY_HANJA          0x19
#define JSH_KEY_KANJI          0x19
#define JSH_KEY_ESCAPE         0x1B
#define JSH_KEY_CONVERT        0x1C
#define JSH_KEY_NONCONVERT     0x1D
#define JSH_KEY_ACCEPT         0x1E
#define JSH_KEY_MODECHANGE     0x1F
#define JSH_KEY_SPACE          0x20
#define JSH_KEY_PRIOR          0x21
#define JSH_KEY_NEXT           0x22
#define JSH_KEY_END            0x23
#define JSH_KEY_HOME           0x24
#define JSH_KEY_LEFT           0x25
#define JSH_KEY_UP             0x26
#define JSH_KEY_RIGHT          0x27
#define JSH_KEY_DOWN           0x28
#define JSH_KEY_SELECT         0x29
#define JSH_KEY_PRINT          0x2A
#define JSH_KEY_EXECUTE        0x2B
#define JSH_KEY_SNAPSHOT       0x2C
#define JSH_KEY_INSERT         0x2D
#define JSH_KEY_DELETE         0x2E
#define JSH_KEY_HELP           0x2F
#define JSH_KEY_LWIN           0x5B
#define JSH_KEY_RWIN           0x5C
#define JSH_KEY_APPS           0x5D
#define JSH_KEY_SLEEP          0x5F
#define JSH_KEY_NUMPAD0        0x60
#define JSH_KEY_NUMPAD1        0x61
#define JSH_KEY_NUMPAD2        0x62
#define JSH_KEY_NUMPAD3        0x63
#define JSH_KEY_NUMPAD4        0x64
#define JSH_KEY_NUMPAD5        0x65
#define JSH_KEY_NUMPAD6        0x66
#define JSH_KEY_NUMPAD7        0x67
#define JSH_KEY_NUMPAD8        0x68
#define JSH_KEY_NUMPAD9        0x69
#define JSH_KEY_MULTIPLY       0x6A
#define JSH_KEY_ADD            0x6B
#define JSH_KEY_SEPARATOR      0x6C
#define JSH_KEY_SUBTRACT       0x6D
#define JSH_KEY_DECIMAL        0x6E
#define JSH_KEY_DIVIDE         0x6F
#define JSH_KEY_F1             0x70
#define JSH_KEY_F2             0x71
#define JSH_KEY_F3             0x72
#define JSH_KEY_F4             0x73
#define JSH_KEY_F5             0x74
#define JSH_KEY_F6             0x75
#define JSH_KEY_F7             0x76
#define JSH_KEY_F8             0x77
#define JSH_KEY_F9             0x78
#define JSH_KEY_F10            0x79
#define JSH_KEY_F11            0x7A
#define JSH_KEY_F12            0x7B
#define JSH_KEY_F13            0x7C
#define JSH_KEY_F14            0x7D
#define JSH_KEY_F15            0x7E
#define JSH_KEY_F16            0x7F
#define JSH_KEY_F17            0x80
#define JSH_KEY_F18            0x81
#define JSH_KEY_F19            0x82
#define JSH_KEY_F20            0x83
#define JSH_KEY_F21            0x84
#define JSH_KEY_F22            0x85
#define JSH_KEY_F23            0x86
#define JSH_KEY_F24            0x87
#define JSH_KEY_NUMLOCK        0x90
#define JSH_KEY_SCROLL         0x91
#define JSH_KEY_OEM_NEC_EQUAL  0x92
#define JSH_KEY_OEM_FJ_JISHO   0x92
#define JSH_KEY_OEM_FJ_MASSHOU 0x93
#define JSH_KEY_OEM_FJ_TOUROKU 0x94
#define JSH_KEY_OEM_FJ_LOYA    0x95
#define JSH_KEY_OEM_FJ_ROYA    0x96
#define JSH_KEY_LSHIFT         0xA0
#define JSH_KEY_RSHIFT         0xA1
#define JSH_KEY_LCONTROL       0xA2
#define JSH_KEY_RCONTROL       0xA3
#define JSH_KEY_LMENU          0xA4
#define JSH_KEY_RMENU          0xA5


namespace jshInput {

	bool IsKey(uint8 id);
	bool IsKeyPressed(uint8 id);
	bool IsKeyReleased(uint8 id);

	bool IsMouse(uint8 id);
	bool IsMousePressed(uint8 id);
	bool IsMouseReleased(uint8 id);

	jsh::vec2 MousePos();
	jsh::vec2 MouseRPos();
	jsh::vec2 MouseDragged();

#ifdef JSH_ENGINE

	void Update();

	void KeyDown(uint8 id);
	void KeyUp(uint8 id);

	void MouseDown(uint8 id);
	void MouseUp(uint8 id);

	void MousePos(uint16 mouseX, uint16 mouseY);
	void MouseDragged(int dx, int dy);

#endif 

}