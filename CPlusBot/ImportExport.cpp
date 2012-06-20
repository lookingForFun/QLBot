#include "../Common/ImportExport.h"

#include <fstream>

bool ImportExport::Initialized = false;
IPlayer * ImportExport::User = nullptr;
IEntityList* ImportExport::Entities = nullptr;
IAimbot * ImportExport::AimBot = nullptr;
wstring ImportExport::ConfigPath;
std::map<std::string, var_t> ImportExport::varNames = std::map<std::string, var_t>();

Variable * ImportExport::Variables[VAR_COUNT];
bool (__cdecl * ImportExport::CanSee) (idVec3 start, idVec3 end, int skipNum);

void ImportExport::RegisterVariable(const string& varName, var_t varNum, const string& defaultValue)
{
	ImportExport::varNames[varName] = varNum;
	Variables[varNum] = new Variable(defaultValue);
}

void ImportExport::Initialize(IEntityList * list)
{ 
	if(Initialized)
		return;
	Entities = list;
	ImportExport::AimBot = new Aimbot();
	RegisterVariable("ab", AIM, "1");
	RegisterVariable("box", BOX, "1");
	RegisterVariable("fov", FOV, "360");
	RegisterVariable("af", AUTOFIRE, "1");
	RegisterVariable("afd", AUTOFIRE_DELAY, "0");
	RegisterVariable("afdr", AUTOFIRE_DELAY_RANDOM, "0");
	RegisterVariable("hm", HUMAN, "1");
	RegisterVariable("smm", SMOOTHMULTIPLIER, "6");
	RegisterVariable("smt", SMOOTHTRANSPOSITION, "1");
	RegisterVariable("sms", SMOOTHADVANCEDSCALING, "0.5");
	RegisterVariable("nh", NEWHUMANIZATION, "0");
	RegisterVariable("wh", WALLHACK, "1");
	RegisterVariable("gs", QUADENTITIES, "1");
	RegisterVariable("ms", MUTESELF, "1");
	RegisterVariable("esp", ESP, "1");
	RegisterVariable("rd", RADAR, "1");
	RegisterVariable("ry", RADAR_Y, "0");
	RegisterVariable("sl", SPECLIST, "0");
	RegisterVariable("vsn", VARSTATUSNOTIFIER, "0");
	RegisterVariable("tm", TIMERS, "1");
	RegisterVariable("acc", ACCURACYCOUNTER, "0");
	RegisterVariable("bfov", BOXFOV, "120");
	RegisterVariable("pr", RECTS, "0");
	RegisterVariable("efs", ESPFONTSIZE, "9");

	Initialized = true;
}

#include <stdio.h>

bool ImportExport::SaveVariables()
{
	wstring sp = ConfigPath + wstring(L"ql.cfg");
	ofstream f(sp);
	if(f.is_open ())
	{
		for(int i = 0; i < VAR_COUNT; ++i)
		{
			f << FindVarName(static_cast<var_t>(i)) << " " << Variables[i]->Value;
		}
		f.close ();
		return true;
	}
	return false;
}

bool ImportExport::SaveBinds(const string& binds)
{
	ofstream file(ConfigPath + wstring(L"binds.cfg"));
	if(file.is_open ())
	{
		file << binds;
		file.close();
		return true;
	}
	return false;
}

#include "../Common/eString.h"

string ImportExport::ReadConfig(const wstring &cfgName)
{
	wstring fp = ConfigPath + cfgName;
	ifstream file(fp, ios::binary);
	if(file.eof ()) {
		file.seekg(0, std::ios::end);
		std::ifstream::pos_type filesize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> bytes(filesize);

		file.read(&bytes[0], filesize);
		file.close ();
		return string(bytes.data ());
	}
	return "";
}

var_t ImportExport::FindVarId( const char * varName )
{
	std::map<std::string, var_t>::iterator it = ImportExport::varNames.find(varName);
	if(it != ImportExport::varNames.end())
		return it->second;
	return VAR_NONE;
}

const string ImportExport::FindVarName( const var_t varId )
{
	for(std::map<std::string, var_t>::const_iterator it = begin(ImportExport::varNames); it != end(ImportExport::varNames); ++it)
	{
		if(it->second == varId)
			return it->first;
	}
	return nullptr;
}