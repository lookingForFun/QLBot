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
			i++;
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

void Cmd_Unbind(eLine * p_cmd)
{
	if(!p_cmd || !p_cmd->wordCount())
		return;
	int l_virtualKey = -1;
	if(strlen(p_cmd->getWord(0)) == 1)
		l_virtualKey = islower(p_cmd->getWord(0)[0]) ? _toupper(p_cmd->getWord(0)[0]) : p_cmd->getWord(0)[0];
	else
	{
		l_virtualKey = HookConsole::KeyNameToCode(p_cmd->getWord(0));
		if(l_virtualKey == -1)
		{
			hConsole.Print(XORSTR("Invalid key specified!"));
			return;
		}
	}
	hConsole.RemoveBind(l_virtualKey);
}

bool ScanForQuotes(_In_z_ const char * p_szSrc)
{
	const char * l_szQuotesStart = strchr(p_szSrc, '\"');
	return (l_szQuotesStart && strchr(l_szQuotesStart+1, '\"'));
}

void Cmd_Bind(eLine * p_cmd)
{
	if(!p_cmd || p_cmd->wordCount() < 2)
	{
		hConsole.Print(XORSTR("Bad command syntax. Argument missing"));
		return;
	}

	bool l_isNamedKey = false;
	int l_virtualKey = -1;
	if(strlen(p_cmd->getWord(0)) == 1)
		l_virtualKey = islower(p_cmd->getWord(0)[0]) ? _toupper(p_cmd->getWord(0)[0]) : p_cmd->getWord(0)[0];
	else
	{
		l_virtualKey = HookConsole::KeyNameToCode(p_cmd->getWord(0));
		if(l_virtualKey == -1)
		{
			hConsole.Print(XORSTR("Invalid key specified!"));
			return;
		}
		l_isNamedKey = true;
	}

	cmatch l_match;
	if(ProcessRegex(p_cmd->getSource().c_str(), XORSTR("(?:\\w+|.) \"([^\"]+)\" \"([^\"]+)\""), l_match))
	{
		hConsole.AddBind(l_virtualKey, new HookBind(l_match.str(1), l_match.str(2), l_isNamedKey));
	}
	else
	{
		if(ScanForQuotes(p_cmd->getSource().c_str()))
		{
			string cmdSrc = p_cmd->getSource();
			size_t pos = 0;
			cmdSrc.erase(cmdSrc.begin(), cmdSrc.begin()+cmdSrc.find('"')+1);
			while((pos = cmdSrc.find('\"')) != string::npos)
				cmdSrc.erase(cmdSrc.begin()+pos, cmdSrc.begin()+pos+1);
			hConsole.AddBind(l_virtualKey, new HookBind(cmdSrc, "", l_isNamedKey));
			return;
		}

		if(p_cmd->getWord(1)[0] == '+')
		{
			string downStr(&p_cmd->getWord(1)[1]);
			string upStr = downStr;
			downStr.append(" 1", 2);
			upStr.append(" 0", 2);
			hConsole.AddBind( l_virtualKey, new HookBind(downStr.c_str(), upStr.c_str(), l_isNamedKey) );
		}
		else
		{
			string bindString = p_cmd->getWord(1);
			for (unsigned int i = 2; i < p_cmd->wordCount() ; ++i)
			{
				const char * word = p_cmd->getWord(i);
				if(!word)
					break;
				bindString.append(" ", 1);
				bindString.append(word, strlen(word));
			}
			hConsole.AddBind(l_virtualKey, new HookBind(bindString, "", l_isNamedKey));
		}
	}
}
