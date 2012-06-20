#include <regex>
#include <sstream>
#include "../Common/ImportExport.h"
#include "hookConsole.h"
#include "Engine.h"
#include "Utils.h"
#include "Timers.h"
#include "ConsoleCommands.h"
#include "DrawUtils.h"

using namespace std::tr1;

#undef _stricmp
#define MAX_LINES 20

#define EXTRA_BINDABLE_KEYS 55
HookConsole hConsole;

typedef void (*cmdFunc_t)(eLine*);

typedef struct keyAssociation_t
{
	const char * keyName;
	const int ID;
} keyAssociation_t;

keyAssociation_t keyNames[EXTRA_BINDABLE_KEYS] = 
{
	{ "mouse1", VK_LBUTTON },
	{ "mouse2", VK_RBUTTON },
	{ "mouse3", VK_MBUTTON },
	{ "mouse4", VK_XBUTTON1 },
	{ "mouse5", VK_XBUTTON2 },
	{ "shift", VK_SHIFT },
	{ "alt", VK_MENU },
	{ "ctrl", VK_CONTROL },
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
	{ "mwheelup", VK_OEM_FJ_MASSHOU},
	{ "mwheeldown", VK_OEM_FJ_LOYA},
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
	{ "[", '[' },
	{ "]", ']' },
};

LRESULT __stdcall Stub_ProcessKeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code == HC_ACTION)
	{
		//bool isDown = GetAsyncKeyState(wParam) <  0;
		bool isDown = (lParam >> 31) == 0;
		hConsole.ProcessKeyPress(wParam, isDown);
		if(hConsole.IsOpen())
		{
			PostMessage(nullptr, WM_NULL, 0, 0);
			return TRUE;
		}
	}
	return CallNextHookEx(hConsole.GetHook(), code, wParam, lParam);
}

HHOOK mouseHook;
LRESULT __stdcall Stub_ProcessMouseEvent(int code, WPARAM wParam, LPARAM lParam)
{
	if(code == HC_ACTION && wParam == WM_MOUSEWHEEL)
	{
		auto mouseInfo = (MSLLHOOKSTRUCT*)lParam;
		if(HIWORD(mouseInfo->mouseData)>0)
		{
			//wheel rotated forward
			hConsole.ProcessKeyPress(VK_OEM_FJ_MASSHOU, true);
			hConsole.ProcessKeyPress(VK_OEM_FJ_MASSHOU, false);
		}
		else
		{
			hConsole.ProcessKeyPress(VK_OEM_FJ_LOYA, true);
			hConsole.ProcessKeyPress(VK_OEM_FJ_LOYA, false);
		}
	}
	return CallNextHookEx(mouseHook, code, wParam, lParam);
}

void MouseProxyProc(int keyNum, bool isDown)
{
	hConsole.ProcessKeyPress(keyNum, isDown);
}

void Stub_ConsoleInitialize()
{
	hConsole.Initialize();
}

void Stub_ConsoleUnload()
{
	hConsole.Unload();
}

void Stub_RenderConsole()
{
	hConsole.Render();
}

__forceinline void HookConsole::NEW_CMD(const char * x, void * y) { commands[string(x)] = (void (__cdecl *)(eLine *))y; }

void HookConsole::Initialize()
{
	if(!bInitialized)
	{
		NEW_CMD(XORSTR("bind"), &Cmd_Bind);
		NEW_CMD(XORSTR("console"), &Cmd_ToggleConsole);
		NEW_CMD(XORSTR("unbind"), &Cmd_Unbind);
		NEW_CMD(XORSTR("exec"), &Cmd_Exec);
		NEW_CMD(XORSTR("toggle"), &Cmd_Toggle);
		NEW_CMD(XORSTR("timer"), &Cmd_AddTimer);
		_hookHandle = SetWindowsHookEx(WH_KEYBOARD, Stub_ProcessKeyPress, 0, GetCurrentThreadId());
		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, Stub_ProcessMouseEvent, 0, GetCurrentThreadId());
		hConsole.LoadSettings();
		bInitialized = true;
	}
}

void HookConsole::Unload()
{
	isOpen = false;
	bInitialized = false;
	UnhookWindowsHookEx(_hookHandle);
	commands.clear();
	binds.clear();
}

void HookConsole::Open()
{
	isOpen = true;
}

void HookConsole::Close()
{
	isOpen = false;
}

void HookConsole::Render()
{
	if(isOpen)
	{
		DrawUtils::FillRectangle(80, 60, 480, 360, consoleColor);
		for (vector<string>::size_type i = 0; i < lines.size() ; ++i) {
			DrawUtils::DrawStringExt(82, 60 + (i * 14) + 2, lines[i].c_str(), Colors::White, false, false);
		}
		DrawUtils::FillRectangle(80, 405, 480, 1, Colors::White);

		DrawUtils::FillRectangle(80,60 - 3,480,3, Colors::White);
		DrawUtils::FillRectangle(80,60 + 360 + 1,480,3, Colors::White);
		DrawUtils::FillRectangle(77,60 - 3,3,360 + 7, Colors::White);
		DrawUtils::FillRectangle(80 + 480 - 3,60,3,360 + 3, Colors::White);

		string sDisplayLine = currentLine;
		string::iterator it = sDisplayLine.begin() + cursorPosition;

		sDisplayLine.insert(it,1, (Engine::GetTime() % 400 > 200) ? '_' : ' ');

		DrawUtils::DrawStringExt(82, 408, sDisplayLine.c_str(), Colors::Yellow, false, false);
	}
}

bool HookConsole::CVarCommand(const eLine& currentCmd)
{
	var_t cvar = ImportExport::FindVarId(currentCmd.getWord(0));
	if(cvar != VAR_NONE)
	{
		auto cv = ImportExport::Variables[cvar];
		if(currentCmd.wordCount() >= 2)
			cv->Value = (strstr(currentCmd.getSource().c_str(), currentCmd.getWord(0)) + strlen(currentCmd.getWord(0)) + 1);
		else
			AddLine(cv->Value.c_str());
		return true;
	}
	return false;
}

void HookConsole::ProcessCommand( const string& originalCmd ) {
	string args = "";
	string cmd = originalCmd;
	if(!cmd.length()) {
		return;
	}
	
	if(isOpen) {
		AddLine(cmd, true);
	}
	//convert the command string to lowercase
	transform(cmd.begin(), cmd.end(), cmd.begin(), tolower);
	
	string nextCmd;
	do {
		nextCmd = "";
		//if we aren't inside a quote block
		if (!ScanForQuotes(cmd.c_str()))
		{
			//search for the first command termination
			auto newCmdPos = cmd.find_first_of(';');
			if(newCmdPos != string::npos)
			{
				//if a command termination is found, use the following part of the string as another command
				newCmdPos++;
				nextCmd = cmd.substr(newCmdPos, cmd.length() - newCmdPos);
				cmd = cmd.substr(0, newCmdPos-1);
				if(nextCmd[0] == ' ')
					nextCmd.erase(nextCmd.begin(), nextCmd.begin()+1);
			}
		}
		
		eLine currentCmd = eLine(cmd);
		if(!currentCmd.wordCount()) {
			return;
		}

		if(cmd.find(';') == string::npos && cmd[0] == '"' && cmd[cmd.length()-1] == '"') {
			cmd.substr(1, cmd.length()-2);
		}
		string commandName = string(currentCmd.getWord(0));
		auto itCmd = commands.find(commandName);
		if(itCmd == commands.end()) { //not a registered command
			if(!CVarCommand(currentCmd))  { //not even a valid cvar
				AddLine(XORSTR("Unknown command"));
			}
		} else { //valid command, parse the args if it hasn't already been done
			if(!args.length()) {
				size_t argStart = cmd.find_first_of(' ');
				if(argStart != string::npos) {
					argStart++;
					args = cmd.substr(argStart, cmd.length() - argStart);
				}
			}
			if(!itCmd->second) {
				continue;
			}
			if (args.length()) {
				((cmdFunc_t)itCmd->second)(&eLine(args));
			} else {
				((cmdFunc_t)itCmd->second)(nullptr);
			}
		}
		cmd = nextCmd;
		args = "";
	} while (nextCmd.length());
}

#define B2S(x) x ? "true" : "false"
void HookConsole::ProcessKeyPress(WPARAM wParam, bool isDown)
{
	string::iterator i1,i2;
	if( isOpen )
	{
		if(!isDown)
			return;
		switch ( wParam ) 
		{ 
		case VK_F9:
			SaveBinds();
			isOpen = false;
			cursorPosition = currentLine.length();
		break;
		case VK_LEFT:
			if (--cursorPosition < 0)
				cursorPosition = 0;
		break;
		case VK_RIGHT:
			cursorPosition++;
			if ((size_t)cursorPosition > currentLine.length())
				cursorPosition = currentLine.length();
		break;
		case VK_DELETE:
			if (currentLine.length())
			{
				i1 = currentLine.begin() + cursorPosition;
				i2 = i1 + 1;
				currentLine.erase(i1,i2);
			}
			break;
		case VK_BACK: // Process a backspace. 
			if (currentLine.length() && cursorPosition)
			{
				i2 = currentLine.begin() + cursorPosition;
				i1 = i2 - 1;
				currentLine.erase(i1,i2);
				if (--cursorPosition < 0)
					cursorPosition = 0;
			}
			break; 
		case VK_TAB: // Process a tab
		case 0x0A: // Process a linefeed
			break; 

		case VK_RETURN: // Process a carriage return
			ProcessCommand(currentLine);
			currentLine = "";
			cursorPosition = 0;
			break; 

		default:
			BYTE btKeyboardState[256];
			unsigned char cPressedKey[2] = "";
			GetKeyboardState ( btKeyboardState );
			ToAscii( wParam, 0, btKeyboardState, ( LPWORD )cPressedKey, 0 );
			//we only wanna render easily printable chars, from ! to z in the ASCII table
			if((cPressedKey[0] > 0x20 && cPressedKey[0] < 0x80) || cPressedKey[0] == ' ')
			{
				currentLine.insert(cursorPosition,1,cPressedKey[0]);
				cursorPosition++;
			}
			break; 
		}
	}
	else
	{
		if(Engine::IsClientPaused())
			return;
		if(wParam == VK_F9 && isDown)
		{
			isOpen = true;
			return;
		}
		char searchTerm;
		if(KeyCodeToName(wParam))
			searchTerm = wParam;
		else
		{
			BYTE btKeyboardState[256];
			char cPressedKey[2] = "";
			GetKeyboardState ( btKeyboardState );
			ToAscii( wParam, 0, btKeyboardState, ( LPWORD )cPressedKey, 0 );
			searchTerm = /*cPressedKey[0] > 0x20 && cPressedKey[0] < 0x80 ? cPressedKey[0] :*/ wParam;
		}
		map<int, HookBind*>::iterator it = binds.find(searchTerm);
		if(it != binds.end())
		{
			const char * boundCommand = it->second->getCmd(isDown);
			if( boundCommand ) ProcessCommand(boundCommand);
		}
	}
}

void HookConsole::AddLine( const string& lineText, bool isCurrentLine )
{
	if(lines.size() > MAX_LINES)
		lines.erase(lines.begin(), lines.begin()+1);
	lines.push_back(lineText);
	if(isCurrentLine)
	{
		currentLine = "";
		cursorPosition = 0;
	}
}

void HookConsole::RemoveBind( int vKey )
{
	map<int, HookBind*>::iterator it = binds.find(vKey);
	if(it != binds.end())
		binds.erase(it);
	else
		Print(XORSTR("Couldn't find a bind for the speficied key"));
}

void HookConsole::RestoreBinds()
{
	std::string fileContents = ImportExport::ReadConfig (L"binds.cfg");
	if(fileContents != "")
	{
		eString bindList = eString(fileContents.c_str());
		eLine cur;
		int i = 0;
		while(bindList.getLine(i, cur))
		{
			ProcessCommand(cur.getSource());
			++i;
		}
	}
}

void HookConsole::LoadSettings()
{
	//LOAD THE BINDS
	RestoreBinds();
	ProcessCommand(XORSTR("exec ql.cfg"));
}

void HookConsole::SaveBinds()
{
	stringstream cfg;
	for(map<int, HookBind*>::const_iterator it = binds.begin(); it != binds.end(); ++it)
	{
		if(!it->second)
			continue;
	
		string boundTo;
		if(it->second->IsNamedKey())
		{
			const char * keyName = KeyCodeToName(it->first);
			if (keyName)
				boundTo = keyName;
			else
				continue;
		}
		else
			boundTo = it->first;
		cfg << XORSTR("bind ") << boundTo << " " << it->second->saveBind () << endl;
	}
	if(!ImportExport::SaveBinds (cfg.str ()))
	{
		MessageBox(0, XORSTR("Couldn't write to the current folder. Aborting."), "", MB_ICONERROR);
		ExitProcess(0);
	}
}

int HookConsole::KeyNameToCode(const char * key)
{
	for (int i = 0; i < EXTRA_BINDABLE_KEYS; ++i)
		if(!_stricmp(key, keyNames[i].keyName))
			return keyNames[i].ID;
	return -1;
}

const char * HookConsole::KeyCodeToName(int key)
{
	for (int i = 0; i < EXTRA_BINDABLE_KEYS; ++i)
		if(keyNames[i].ID == key)
			return keyNames[i].keyName;
	return nullptr;
} 

HookConsole::HookConsole()
{
	bInitialized = false;
	isOpen = false;
	cursorPosition = 0;
	currentLine = "";
	lines.clear();
	static float defaultColor[4] = { 0.5f, 0.5f, 1.0f, 0.5};
	consoleColor = defaultColor;
}


std::string HookBind::saveBind()
{
	if (keyUpCmd.length())
	{
		stringstream bind;
		bind << XORSTR("\"") << cmd << XORSTR("\" ") << XORSTR("\"") << keyUpCmd << XORSTR("\""); 
		return bind.str ();
	}
	else if (strchr(cmd.c_str(), ';'))
	{
		string returnString = "\"";
		returnString += cmd;
		returnString += "\"";
		return returnString;
	} 
	else
		return cmd;
}

const char * HookBind::getCmd( bool isKeyDown /*= true*/)
{
	bool shouldChange = (isKeyDown != isKeyDown);
	if (shouldChange)
	{
		isKeyDown = isKeyDown;
		if( isKeyDown )
			return cmd.c_str();
		else if ( keyUpCmd.length() > 0 )
			return keyUpCmd.c_str();
	}
	return nullptr;
}

HookBind::HookBind(const string& bindText, const string& pkeyUpCmd, bool isNamedKey)
{
	cmd = bindText;
	isKeyDown = false;
	this->isNamedKey = isNamedKey;
	if(pkeyUpCmd.length() > 0)
		keyUpCmd = pkeyUpCmd;
}

void Stub_ConsolePrint(const char * lineTxt)
{
	hConsole.Print(lineTxt);
}