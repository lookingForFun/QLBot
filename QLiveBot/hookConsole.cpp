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
	bOpen = false;
	bInitialized = false;
	UnhookWindowsHookEx(_hookHandle);
	commands.clear();
	binds.clear();
}

void HookConsole::Open()
{
	bOpen = true;
}

void HookConsole::Close()
{
	bOpen = false;
}

void HookConsole::Render()
{
	if(bOpen)
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

		string sDisplayLine = sCurrentLine;
		string::iterator it = sDisplayLine.begin() + iCursorPosition;

		sDisplayLine.insert(it,1, (Engine::GetTime() % 400 > 200) ? '_' : ' ');

		DrawUtils::DrawStringExt(82, 408, sDisplayLine.c_str(), Colors::Yellow, false, false);
	}
}

bool HookConsole::CVarCommand(eLine currentCmd)
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

void HookConsole::ProcessCommand( const string& szOriginalCmd )
{
	string args = "";
	string l_cmd = string(szOriginalCmd);
	if(!l_cmd.length())
		return;
	
	if(bOpen)
		AddLine(l_cmd, true);
	//convert the command string to lowercase
	transform(l_cmd.begin(), l_cmd.end(), l_cmd.begin(), tolower);
	
	string nextCmd;
	do
	{
		nextCmd = "";
		//if we aren't inside a quote block
		if (!ScanForQuotes(l_cmd.c_str()))
		{
			//search for the first command termination
			auto l_newCmdPos = l_cmd.find_first_of(';');
			if(l_newCmdPos != string::npos)
			{
				//if a command termination is found, use the following part of the string as another command
				l_newCmdPos++;
				nextCmd = l_cmd.substr(l_newCmdPos, l_cmd.length() - l_newCmdPos);
				l_cmd = l_cmd.substr(0, l_newCmdPos-1);
				if(nextCmd[0] == ' ')
					nextCmd.erase(nextCmd.begin(), nextCmd.begin()+1);
			}
		}
		
		eLine currentCmd = eLine(l_cmd);
		if(!currentCmd.wordCount())
			return;

		if(l_cmd.find(';') == string::npos && l_cmd[0] == '"' && l_cmd[l_cmd.length()-1] == '"')
		{
			l_cmd.substr(1, l_cmd.length()-2);
		}
		string l_szCommandName = string(currentCmd.getWord(0));
		map<string, void (*)(eLine*)>::iterator i = commands.find(l_szCommandName);
		if(i == commands.end()) //not a registered command
		{
			if(!CVarCommand(currentCmd)) //not even a valid cvar
			{
				AddLine(XORSTR("Unknown command"));
			}
		}
		else //valid command, parse the args if it hasn't already been done
		{
			if(!args.length())
			{
				size_t l_argstart = l_cmd.find_first_of(' ');
				if(l_argstart != string::npos)
				{
					l_argstart++;
					args = l_cmd.substr(l_argstart, l_cmd.length() - l_argstart);
				}
			}
			if(!i->second)
				continue;
			if (args.length())
			{
				((cmdFunc_t)i->second)(&eLine(args));
			} 
			else
				((cmdFunc_t)i->second)(nullptr);
		}
		l_cmd = nextCmd;
		args = "";
	} while (nextCmd.length());
}

#define B2S(x) x ? "true" : "false"
void HookConsole::ProcessKeyPress(WPARAM wParam, bool isDown)
{
	string::iterator i1,i2;
	if( bOpen )
	{
		if(!isDown)
			return;
		switch ( wParam ) 
		{ 
		case VK_F9:
			SaveBinds();
			bOpen = false;
			iCursorPosition = sCurrentLine.length();
		break;
		case VK_LEFT:
			if (--iCursorPosition < 0)
				iCursorPosition = 0;
		break;
		case VK_RIGHT:
			iCursorPosition++;
			if ((size_t)iCursorPosition > sCurrentLine.length())
				iCursorPosition = sCurrentLine.length();
		break;
		case VK_DELETE:
			if (sCurrentLine.length())
			{
				i1 = sCurrentLine.begin() + iCursorPosition;
				i2 = i1 + 1;
				sCurrentLine.erase(i1,i2);
			}
			break;
		case VK_BACK: // Process a backspace. 
			if (sCurrentLine.length() && iCursorPosition)
			{
				i2 = sCurrentLine.begin() + iCursorPosition;
				i1 = i2 - 1;
				sCurrentLine.erase(i1,i2);
				if (--iCursorPosition < 0)
					iCursorPosition = 0;
			}
			break; 
		case VK_TAB: // Process a tab
		case 0x0A: // Process a linefeed
			break; 

		case VK_RETURN: // Process a carriage return
			ProcessCommand(sCurrentLine);
			sCurrentLine = "";
			iCursorPosition = 0;
			break; 

		default:
			BYTE btKeyboardState[256];
			unsigned char cPressedKey[2] = "";
			GetKeyboardState ( btKeyboardState );
			ToAscii( wParam, 0, btKeyboardState, ( LPWORD )cPressedKey, 0 );
			//we only wanna render easily printable chars, from ! to z in the ASCII table
			if((cPressedKey[0] > 0x20 && cPressedKey[0] < 0x80) || cPressedKey[0] == ' ')
			{
				sCurrentLine.insert(iCursorPosition,1,cPressedKey[0]);
				iCursorPosition++;
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
			bOpen = true;
			return;
		}
		char l_searchTerm;
		if(KeyCodeToName(wParam))
			l_searchTerm = wParam;
		else
		{
			BYTE btKeyboardState[256];
			char cPressedKey[2] = "";
			GetKeyboardState ( btKeyboardState );
			ToAscii( wParam, 0, btKeyboardState, ( LPWORD )cPressedKey, 0 );
			l_searchTerm = /*cPressedKey[0] > 0x20 && cPressedKey[0] < 0x80 ? cPressedKey[0] :*/ wParam;
		}
		map<int, HookBind*>::iterator it = binds.find(l_searchTerm);
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
		sCurrentLine = "";
		iCursorPosition = 0;
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
	
		string l_boundTo;
		if(it->second->IsNamedKey())
		{
			const char * l_keyName = KeyCodeToName(it->first);
			if (l_keyName)
				l_boundTo = l_keyName;
			else
				continue;
		}
		else
			l_boundTo = it->first;
		cfg << XORSTR("bind ") << l_boundTo << " " << it->second->saveBind () << endl;
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
	bOpen = false;
	iCursorPosition = 0;
	sCurrentLine = "";
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

const char * HookBind::getCmd( bool p_isKeyDown /*= true*/)
{
	bool l_bAction = (isKeyDown != p_isKeyDown);
	if (l_bAction)
	{
		isKeyDown = p_isKeyDown;
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

void Stub_ConsolePrint(const char * p_lineTxt)
{
	hConsole.Print(p_lineTxt);
}