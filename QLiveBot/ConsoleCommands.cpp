#include <regex>
#include "hookConsole.h"
#include "Timers.h"
#include "../Common/ImportExport.h"
#include "Utils.h"

extern HookConsole hConsole;

bool ProcessRegex(const char * txt, const char * rgx, cmatch &matches)
{
	regex re(rgx);
	return regex_search(txt, matches, re, regex_constants::match_default);
}

void Cmd_AddTimer(eLine * p_cmd)
{
	if(!p_cmd || p_cmd->wordCount() < 2)
		return;

	cmatch match;
	if(!ProcessRegex(p_cmd->getSource().c_str(), "((?:\\\".+?\\\")|(?:\\w+)) (\\d+)", match))
		return;

	auto timerName = match.str(1);
	if(timerName[0] == '"')
		timerName = timerName.substr(1, timerName.length() - 2);
	timers.Add(timerName, atoi(match.str(2).c_str())*1000);
}

void Cmd_ToggleConsole(eLine *)
{
	if(hConsole.IsOpen())
	{
		hConsole.Close();
	}
	else
	{
		hConsole.Open();
	}
}

void Cmd_Exec(eLine * p_cmd)
{
	if(!p_cmd)
		return;
	string buffer = ImportExport::ReadConfig( Utils::StringToWString(p_cmd->getWord(0)).c_str() );
	if( buffer.length() > 0 )
	{
		eString cvs(buffer);
		unsigned int i = 0;
		eLine curLine;
		while(cvs.getLine(i, curLine))
		{
			if(curLine.wordCount() < 2)
				break;
			hConsole.ProcessCommand(curLine.getSource());
			++i;
		}
	}
}

void Cmd_Toggle(eLine * cmd)
{
	if(!cmd)
		return;
	if(const char * varName = cmd->getWord(0))
	{
		var_t cvar = ImportExport::FindVarId(varName);
		if(cvar != VAR_NONE)	
		{
			Variable * var = ImportExport::Variables[cvar];
			if(var->IsEnabled())
				var->Value = "0";
			else
				var->Value = "1";
		}
	}
}

void Cmd_Unbind(eLine * cmd) {
	if(cmd && cmd->wordCount()) {
		int virtualKey;
		if(strlen(cmd->getWord(0)) == 1) {
			virtualKey = islower(cmd->getWord(0)[0]) ? _toupper(cmd->getWord(0)[0]) : cmd->getWord(0)[0];
		} else {
			virtualKey = HookConsole::KeyNameToCode(cmd->getWord(0));
			if(virtualKey == -1) {
				hConsole.Print(XORSTR("Invalid key specified!"));
				return;
			}
		}
		hConsole.RemoveBind(virtualKey);
	}
}

bool ScanForQuotes(_In_z_ const char * str) {
	auto quotesStart = strchr(str, '\"');
	return (quotesStart && strchr(quotesStart+1, '\"'));
}

void Cmd_Bind(eLine * cmd) {
	if(!cmd || cmd->wordCount() < 2) {
		hConsole.Print(XORSTR("Bad command syntax. Argument missing"));
		return;
	}

	bool isNamedKey = false;
	int virtualKey = -1;
	if(strlen(cmd->getWord(0)) == 1) {
		virtualKey = islower(cmd->getWord(0)[0]) ? _toupper(cmd->getWord(0)[0]) : cmd->getWord(0)[0];
	} else {
		virtualKey = HookConsole::KeyNameToCode(cmd->getWord(0));
		if(virtualKey == -1)
		{
			hConsole.Print(XORSTR("Invalid key specified!"));
			return;
		}
		isNamedKey = true;
	}

	cmatch l_match;
	if(ProcessRegex(cmd->getSource().c_str(), XORSTR("(?:\\w+|.) \"([^\"]+)\" \"([^\"]+)\""), l_match)) {
		hConsole.AddBind(virtualKey, new HookBind(l_match.str(1), l_match.str(2), isNamedKey));
	} else {
		if(ScanForQuotes(cmd->getSource().c_str())) {
			string cmdSrc = cmd->getSource();
			size_t pos = 0;
			cmdSrc.erase(cmdSrc.begin(), cmdSrc.begin()+cmdSrc.find('"')+1);
			while((pos = cmdSrc.find('\"')) != string::npos)
				cmdSrc.erase(cmdSrc.begin()+pos, cmdSrc.begin()+pos+1);
			hConsole.AddBind(virtualKey, new HookBind(cmdSrc, "", isNamedKey));
			return;
		}

		if(cmd->getWord(1)[0] == '+') {
			string downStr(&cmd->getWord(1)[1]);
			string upStr = downStr;
			downStr.append(" 1", 2);
			upStr.append(" 0", 2);
			hConsole.AddBind( virtualKey, new HookBind(downStr.c_str(), upStr.c_str(), isNamedKey) );
		} else {
			string bindString = cmd->getWord(1);
			for (unsigned int i = 2; i < cmd->wordCount() ; ++i) {
				const char * word = cmd->getWord(i);
				if(!word)
					break;
				bindString.append(" ", 1);
				bindString.append(word, strlen(word));
			}
			hConsole.AddBind(virtualKey, new HookBind(bindString, "", isNamedKey));
		}
	}
}
