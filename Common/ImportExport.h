#pragma once
#include "ImportInterfaces.h"
#include "Aimbot.h"
#include "Variable.h"
#include <map>
#include <string>

enum var_t
{
	VAR_NONE = -1,
	AIM,
	BOX,
	FOV,
	HUMAN,
	SMOOTHTRANSPOSITION,
	SMOOTHMULTIPLIER,
	SMOOTHADVANCEDSCALING,
	NEWHUMANIZATION,
	AUTOFIRE,
	WALLHACK,
	QUADENTITIES,
	MUTESELF,
	ESP,
	AUTOFIRE_DELAY,
	AUTOFIRE_DELAY_RANDOM,
	RADAR,
	RADAR_Y,
	SPECLIST,
	VARSTATUSNOTIFIER,
	TIMERS,
	ACCURACYCOUNTER,
	BOXFOV,
	RECTS,
	ESPFONTSIZE,

	VAR_COUNT
};

class ImportExport
{
	static std::map<std::string, var_t> varNames;

public:
	static void Initialize(IEntityList * list);
	static bool SaveVariables();
	static var_t FindVarId(const char * varName);
	static const string FindVarName( const var_t varId );
	static void RegisterVariable(const string& varName, var_t varNum, const string& defaultValue);
	static string ReadConfig(const wstring& cfgName);
	static IPlayer* User;

	static bool Initialized;
	static IEntityList* Entities;
	static IAimbot * AimBot;
	static Variable * Variables[VAR_COUNT];
	static wstring ConfigPath;
	static bool (__cdecl * CanSee) (idVec3 start, idVec3 end, int skipNum);
	static bool SaveBinds(const string& binds);
};