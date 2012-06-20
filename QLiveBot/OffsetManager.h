#pragma once

#include <Windows.h>

class ModManager
{
	LPBYTE gameModuleBase;
	LPBYTE qlModuleBase;
	static BYTE*	FollowCall(const BYTE* callInstruction);
	static bool		Compare(LPBYTE pAddr, DWORD dwSize, unsigned const char * pPattern, LPCSTR szMask);
	static LPBYTE	LocalizePattern(LPBYTE pAddr, DWORD dwSize, unsigned const char * pPattern, LPCSTR szMask);
	static byte *   FindStringPush(const char * str, byte * moduleBase, DWORD moduleSize, byte * startAddress);
	void ProcessObituary();
	void FindGameType();
	void FindConsoleFunc();
	void FindCGTime();
	void FindCGTrace();
	void FindDamageFeedback();
	void FindAddCEntity();
	void FindPainEvent();
	void FindPickupEvent();
	void FindEntities();
	void FindEngineAngles();
	void FindButton0();
	void FindBgItemList();
	void FindEntityEvent();
	void FindRefdef();

public:
	ModManager();
	void UpdateOffsets(HINSTANCE gameModuleBase, HINSTANCE qlModuleBase);
	
	DWORD cg_clientnum;
	DWORD cg_entities;
	DWORD cgs_clientinfo;
	DWORD clientinfo_size;
	DWORD cgs_gametype;
	DWORD vtell_target;
	DWORD cg_time;
	DWORD cg_snap;
	LPBYTE cg_refdef_vieworg;
	LPBYTE cg_refdefviewangles;
	DWORD cg_trace;
	DWORD cg_damagefeedback_jnz;
	DWORD cgs_screenxscale;
	DWORD cg_addcentity;
	DWORD cg_painevent;
	DWORD cg_entityevent;
	DWORD cg_pickupevent;	
	DWORD cg_obituary;
	DWORD bg_itemList;
	DWORD cl_paused;
	DWORD fy;
	DWORD fx;
	DWORD button0;
};

extern ModManager offsets;