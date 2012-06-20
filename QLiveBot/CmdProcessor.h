#pragma once

#define EXTRA_BINDABLE_KEYS 58

typedef struct keyAssociation_s
{
	char * keyName;
	short ID;
} keyAssociation_t;

keyAssociation_t keyNames[EXTRA_BINDABLE_KEYS] = 
{
	{ "mouse1", 0x01 },
	{ "mouse2", 0x02 },
	{ "mouse3", 0x03 },
	{ "mouse4", 0x04 },
	{ "mouse5", 0x05 },
	{ "shift", VK_SHIFT },
	{ "lshift", VK_LSHIFT },
	{ "rshift", VK_RSHIFT },
	{ "alt", VK_MENU },
	{ "ralt", VK_RMENU },
	{ "lalt", VK_LMENU },
	{ "ctrl", VK_CONTROL },
	{ "lctrl", VK_LCONTROL },
	{ "rctrl", VK_RCONTROL },
	{ "tab", VK_TAB },
	{ "up", VK_UP },
	{ "down", VK_DOWN },
	{ "left", VK_LEFT },
	{ "right", VK_RIGHT },
	{ "insert", VK_INSERT },
	{ "delete", VK_DELETE },
	{ "home", VK_HOME },
	{ "end", VK_END },
	{ "pageup", VK_PRIOR },
	{ "pagedn", VK_NEXT },
	{ "backsp", VK_BACK },
	{ "enter", VK_RETURN },
	{ "pause", VK_PAUSE },
	{ "numlock", VK_NUMLOCK },
	{ "end", VK_END },
	{ "space", VK_SPACE },
	{ "esc", VK_ESCAPE },
	{ "uparrow", VK_UP },
	{ "downarrow", VK_DOWN },
	{ "leftarrow", VK_LEFT },
	{ "rightarrow", VK_RIGHT},
	{ "numpad0", VK_NUMPAD0 },
	{ "numpad1", VK_NUMPAD1 },
	{ "numpad2", VK_NUMPAD2 },
	{ "numpad3", VK_NUMPAD3 },
	{ "numpad4", VK_NUMPAD4 },
	{ "numpad5", VK_NUMPAD5 },
	{ "numpad6", VK_NUMPAD6 },
	{ "numpad7", VK_NUMPAD7 },
	{ "numpad8", VK_NUMPAD8 },
	{ "numpad9", VK_NUMPAD9 },
	{ "F1", VK_F1 },
	{ "F2", VK_F2 },
	{ "F3", VK_F3 },
	{ "F4", VK_F4 },
	{ "F5", VK_F5 },
	{ "F6", VK_F6 },
	{ "F7", VK_F7 },
	{ "F8", VK_F8 },
	{ "F9", VK_F9 },
	{ "F10", VK_F10 },
	{ "F11", VK_F11 },
	{ "F12", VK_F12 },
};

short KeyNameToCode(const char * key)
{
	for (int i = 0; i < EXTRA_BINDABLE_KEYS; ++i)
		if(!_stricmp(key, keyNames[i].keyName))
			return keyNames[i].ID;
	return -1;
}