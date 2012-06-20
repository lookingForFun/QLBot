#include "OffsetManager.h"
#include "xorClass.h"

ModManager offsets;

#define GAME_MODULE_SIZE 0x90000
#define QL_MODULE_SIZE 0x400000

#define EXIT_ON_nullptr(x, label) if(!x)\
{ /*MessageBox(0, "error " label, "", MB_ICONERROR); */ExitProcess(0); }

LPBYTE ModManager::LocalizePattern(LPBYTE pAddr, DWORD dwSize, unsigned const char * pPattern, LPCSTR szMask)
{
	DWORD dwLen = (DWORD)strlen(szMask);
	DWORD dwIndex;
	for (dwIndex = 0; dwIndex < dwSize; ++dwIndex)
	{
		if (Compare(pAddr+dwIndex, dwLen, pPattern, szMask))
			return pAddr+dwIndex;
	}
	return nullptr;
}
bool ModManager::Compare(LPBYTE pAddr, DWORD dwSize, unsigned const char * pPattern, LPCSTR szMask)
{
	DWORD dwIndex;
	__try
	{
		for (dwIndex = 0; dwIndex < dwSize; ++dwIndex, ++szMask)
			if (*szMask != '?' && pAddr[dwIndex] != pPattern[dwIndex])
				return false;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}
	return true;
}

void ModManager::FindConsoleFunc()
{
	LPBYTE txt = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)XORSTR("vtell_target"), XORSTR("xxxxxxxxxxxx"));
	EXIT_ON_nullptr(txt, "findconsolefunc");
	LPBYTE stringRef = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)&txt, "xxxx");
	EXIT_ON_nullptr(stringRef, "fcf 1");
	vtell_target = *(DWORD*)(stringRef+4);
}

byte * ModManager::FindStringPush(const char * str, byte * moduleBase, DWORD moduleSize, byte * startAddress)
{
	auto slen = strlen(str);
	char * stringPattern = (char*)malloc(slen+1);
	for(size_t i = 0; i < slen; ++i) stringPattern[i] = 'x';
	stringPattern[slen] = '\0';
	auto saddress = LocalizePattern(moduleBase, moduleSize, (byte*)str, stringPattern);
	if(saddress != nullptr)
	{
		byte push[5];
		push[0] = 0x68;
		*(DWORD*)(push+1) = (DWORD)saddress;
		return LocalizePattern(startAddress, moduleSize - (startAddress - moduleBase), push, XORSTR("xxxxx"));
	}
	return nullptr;
}

BYTE* ModManager::FollowCall(const BYTE* callInstr)
{
	return (BYTE*)((DWORD)callInstr + *(DWORD*)(callInstr+1) + 5);
}

void ModManager::ProcessObituary()
{
	LPBYTE stringRef = FindStringPush(XORSTR("CG_Obituary: target out of range"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	EXIT_ON_nullptr(stringRef, "po");
	cg_obituary = (DWORD)stringRef-0x3F;
	cg_clientnum = *(DWORD*)(stringRef+0xF);
	clientinfo_size = *(DWORD*)(stringRef+0x1F);
	cgs_clientinfo = *(DWORD*)(stringRef+0x25);
	
	stringRef = FindStringPush(XORSTR("CG_TransitionSnapshot: nullptr cg.snap"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	cg_snap = *(DWORD*)(stringRef - 8);
}

void ModManager::FindCGTime()
{
	LPBYTE res = LocalizePattern((LPBYTE)vtell_target,0x300, (byte*)"\x05\xE8\x03\x00\x00\x39\xFF\xFF\xFF\xFF\xFF", XORSTR("xxxxxx?????"));
	EXIT_ON_nullptr(res, "cgt");
	cg_time = *(DWORD*)(res+7);
}

void ModManager::FindRefdef()
{
	auto str = FindStringPush(XORSTR("(%i %i %i) : %i"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	cg_refdef_vieworg = *reinterpret_cast<LPBYTE*>(str-0x7);
	cg_refdefviewangles = cg_refdef_vieworg+0x158;
}

void ModManager::FindGameType()
{
	LPBYTE push = FindStringPush(XORSTR("sound/player/announce/%s.wav"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	EXIT_ON_nullptr(push, "fgt");
	cgs_gametype = *(DWORD*)(push-16);
	cgs_screenxscale = cgs_gametype - 0x18;
}

void ModManager::FindCGTrace()
{
	LPBYTE cmp = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x44", XORSTR("xxxxxxxxx"));
	EXIT_ON_nullptr(cmp, "fcgt");
	cg_trace = (DWORD)cmp;
}

void ModManager::FindDamageFeedback()
{
	LPBYTE cmp = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)"\x8B\x83\xB0\x00\x00\x00\x85\xC0\x74\x17", XORSTR("xxxxxxxxxx"));
	EXIT_ON_nullptr(cmp, "fdfb");
	cg_damagefeedback_jnz = (DWORD)(cmp+8);
}

void ModManager::FindAddCEntity()
{
	LPBYTE cmp = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)"\x8B\x46\x04\x83\xF8\x0D\x7C\x09", XORSTR("xxxxxxxx"));
	EXIT_ON_nullptr(cmp, "face");
	cg_addcentity = (DWORD)cmp;
}

void ModManager::FindEntityEvent()
{
	LPBYTE cmp = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)"\x83\xEC\x10\x0F\x57\xC0", XORSTR("xxxxx"));
	EXIT_ON_nullptr(cmp, "fee");
	cg_entityevent = (DWORD)cmp;
}

void ModManager::FindEntities()
{
	LPBYTE cmp = LocalizePattern(gameModuleBase, GAME_MODULE_SIZE, (byte*)"\xFF\x00\xA0\x02\x00\xBF\x40\xDC\x65\x10\xF3\xAB", XORSTR("?xxxx?????xx"));
	EXIT_ON_nullptr(cmp, "fe");
	cg_entities = *(DWORD*)(cmp+6);
}


void ModManager::FindEngineAngles()
{
	LPBYTE stringRef = FindStringPush(XORSTR("centerview"), qlModuleBase, QL_MODULE_SIZE, qlModuleBase);
	EXIT_ON_nullptr(stringRef, "fea");
	fy = *(DWORD*)(*(DWORD*)(stringRef-4) + 0x10);
	fx = fy+4;
}

void ModManager::FindButton0()
{
	LPBYTE txt = LocalizePattern(qlModuleBase, QL_MODULE_SIZE, (byte*)XORSTR("+attack"), XORSTR("xxxxxxx"));
	EXIT_ON_nullptr(txt, "fb0 1");
	byte push[5];
	push[0] = 0x68;
	*(DWORD*)(push+1) = (DWORD)txt;
	LPBYTE stringRef = LocalizePattern(qlModuleBase, QL_MODULE_SIZE, push, XORSTR("xxxxx"));
	EXIT_ON_nullptr(stringRef, "fb0");
	button0 = *(DWORD*)(*(DWORD*)(stringRef-4) + 2);
}

void ModManager::FindBgItemList()
{
	auto stringRef = FindStringPush(XORSTR("Couldn't find weapon %i"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	EXIT_ON_nullptr(stringRef, "fbil");
	bg_itemList = *(DWORD*)(stringRef+11);
}

void ModManager::FindPickupEvent()
{
	auto firstRef = FindStringPush(XORSTR("CG_EntityEvent: invalid item index %d\n"), gameModuleBase, GAME_MODULE_SIZE, gameModuleBase);
	EXIT_ON_nullptr(firstRef, "fpev");
	auto secondRef = FindStringPush(XORSTR("CG_EntityEvent: invalid item index %d\n"), gameModuleBase, GAME_MODULE_SIZE, firstRef+1);
	EXIT_ON_nullptr(secondRef, "fpev1");
	cg_pickupevent = (DWORD)secondRef + 0xD;
}

void ModManager::UpdateOffsets(HINSTANCE gamebase, HINSTANCE qlbase)
{
	gameModuleBase = (LPBYTE)gamebase+0x1000;
	qlModuleBase = (LPBYTE)qlbase+0x1000;
	FindEntities();
	ProcessObituary();
	FindConsoleFunc();
	FindCGTime();
	FindCGTrace();
	FindDamageFeedback();
	FindGameType();
	FindAddCEntity();
	FindRefdef();
	FindEntityEvent();
	FindEngineAngles();
	FindButton0();
}

ModManager::ModManager()
{
}