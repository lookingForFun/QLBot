#ifndef ENGINEH
#define ENGINEH

#include "cPlayer.h"
#include "hook.h"

extern kbutton_t * in_button0;

int getTime();
int getClientNum();
int getSnapshot();
int getSnapFlags();
bool trap_isClientPaused();

bool __cdecl CanSee(idVec3 start, idVec3 dest, int skipNum);

class Engine
{
public:
	static int GetTime() { return getTime(); }
	static void Click() {
		in_button0->active = 1;
		in_button0->wasPressed = 0;
	}

	static void Unclick() {
		in_button0->active = 0;
		in_button0->wasPressed = 0;
	}

	static bool IsShooting() {
		usercmd_t cmd;
		Original_Syscall(CG_GETUSERCMD, Original_Syscall(CG_GETCURRENTCMDNUMBER) - 63, &cmd);
		return (cmd.buttons & 1);
	}

	static void Print(const char * txt) {
		Original_Syscall(0, txt);
	}

	static bool IsClientPaused() {
		return trap_isClientPaused();
	}

	static idAngles * GetAngles() { return reinterpret_cast<idAngles*>(offsets.fy); };

	static void SetWallhack(LPINT entityPtr, bool set)
	{
		int* flags = reinterpret_cast<int*>(entityPtr + 4);
		if(set) *flags |= 8; //ent.renderfx |= RF_DEPTHHACK
		else *flags &= ~8; //ent.renderfx ^= RF_DEPTHHACK
	}

	static bool __cdecl HasCleanPath(idVec3 start, idVec3 dest, int skipNum)
	{
		return CanSee(start, dest, skipNum);
	}

	static int GetClientNum()
	{
		return getClientNum();
	}

	static bool HasValidSnapshot()
	{
		return getSnapshot() != nullptr && !(getSnapFlags() & 2);
	}

	static int GetGameType()
	{
		return getGameType();
	}
};

#endif