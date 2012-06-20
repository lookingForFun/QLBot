#pragma once
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include "hook.h"
#include "../Common/eString.h"

using namespace std;

class HookBind
{
public:
	HookBind(const string& bindText, const string& keyUpCmd, bool isNamedKey);
	void		setCmd(string bindText, bool isKeyUp = false) {
		if(!isKeyUp)
			cmd = bindText;
		else
			keyUpCmd = bindText;
	}
	const char* getCmd(bool p_bKeyDown = true);
	std::string saveBind();
	bool		IsNamedKey() const {
		return isNamedKey;
	}

private:
	bool		isNamedKey;
	bool		isKeyDown;
	string		cmd;
	string		keyUpCmd;
};

class HookConsole
{
public:
	HookConsole();
	bool					IsOpen() const { return bOpen; }
	void					Initialize();
	void					Unload();
	void					LoadSettings();
	void					Render();
	void					Open();
	void					Close();
	
	void					ProcessKeyPress(WPARAM wParam, bool p_isDown);
	void					AddBind(int vKey, HookBind * bind) {
		binds[vKey] = bind;
	}
	void					RemoveBind(int vKey);
	HookBind*				GetBind(int vKey) {
		return binds[vKey];
	}
	void 					SaveBinds();
	void					ProcessCommand(const string& szCmd);
	void					Print(const char * p_lineText) { 
		AddLine(p_lineText);
	}

	HHOOK					GetHook() const { return _hookHandle; }
	static int				KeyNameToCode(const char * key);
	static const char *		KeyCodeToName(int key);


private:
	void					RestoreBinds();
	bool					CVarCommand(eLine currentCmd);
	void					AddLine(const string& lineText, bool isCurrentLine = false);
	void					Clear() {
		lines.clear();
	}
	void					NEW_CMD(const char * x, void * y);
	bool					bInitialized;
	bool					bOpen;
	string					sCurrentLine; //line being typed
	int                     iCursorPosition;
	vector<string>			lines; //old ones
	map<int, HookBind*>		binds;
	map<string, void (*)(eLine*)>		commands;
	float*					consoleColor;
	HHOOK					_hookHandle;
};

extern HookConsole hConsole;