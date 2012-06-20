#include <Windows.h>
#include "../Common/AutoFire.h"
#include "Engine.h"
#include "euDetours.h"
#include "DrawUtils.h"
#include "HookedSyscalls.h"
#include "ClientEvents.h"
#include "AccuracyCounter.h"
#include "Visuals.h"
#include "Timers.h"

#define SECURITY_WIN32

#define SYSCALLTABLE_SIZE 0x294

#define CG_CVAR_VARIABLESTRINGBUFFER 9
#define CG_CVAR_GETFLOATVALUE 0x28

// These are needed, but not needed enough to be in the header
void Hook_CG_AddCEntity();

void ProcessBinds();
void LoadBinds();
void SaveBinds();

void Stub_RenderConsole();
void Stub_ConsoleInitialize();
void Stub_ConsoleUnload();
void MouseProxyProc(int keyNum, bool isDown);

////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	GLOBALS

DWORD * syscallptr;
DWORD * syscallTable = nullptr;
tVmMain vmMainPtr;

unique_ptr<memoryDetour> g_AddCEnt; //used to determine whether visual effects should be added on the following refentities or not

float * cgs_screenXScale;
void (__cdecl * pDllEntry)(DWORD *** result, void * syscalladdr, int * v);
int (__cdecl * CG_DrawPic)(float x, float y, float width, float height, int shaderHandle);

int g_EntityBeingAdded = 64;
void * g_AddCEntityTramp;

extern memoryDetour * g_GPAHook;

unique_ptr<memoryDetour> g_EntityEvent;

int healths[MAX_CLIENTS]; //for the health ESP
kbutton_t * in_button0;

map<string, int> Shaders = map<string, int>();
//////////////////////////////////////////////////////////////////////////


enum EntityEvent
{
	EV_BULLET_HIT_FLESH = 45,
	EV_FIRE_WEAPON = 20,
	EV_PAIN = 53,
	EV_OBITUARY = 58,
};

void * __stdcall Hook_CG_EntityEvent(DWORD cent)
{
#define ENTITYSTATE_OTHERENTITYNUM 0x94
#define ENTITYSTATE_EVENTPARM 0xC0
#define ENTITYSTATE_EVENT 0xBC
	int evt = *(int*)(cent+ENTITYSTATE_EVENT) & 0xFFFFFCFF;
	int eventParm = *(int*)(cent+ENTITYSTATE_EVENTPARM);
	int otherEntityNum = *(int*)(cent+ENTITYSTATE_OTHERENTITYNUM);
	int clientNum = *(int*)cent;
	if ( clientNum < 0 || clientNum >= MAX_CLIENTS )
		clientNum = 0;
//	syscallptr(0, va("ent: %2i evt: %3i\n", clientNum, evt));
	
	switch(evt)
	{
	case EV_PAIN:
		if(clientNum != Engine::GetClientNum ())
			healths[clientNum] = eventParm;
		break;
	case EV_OBITUARY:
		healths[otherEntityNum] = 100;
		break;
	case EV_FIRE_WEAPON:
		if(clientNum == Engine::GetClientNum ())
		{
			AccuracyManager::ShotFired(ImportExport::Entities->GetPlayer(Engine::GetClientNum ())->Weapon().Id());
		}
		break;
	default:
		break;
	}
	return g_EntityEvent->GetTrampoline();
}

#pragma warning(disable:4409 4410)
__declspec(naked) void Stub_CG_EntityEvent()
{
	__asm
	{
		pushad;
		push [esp+0x24];
		call Hook_CG_EntityEvent;
		push eax;
		add esp, 4
		popad;
		jmp [esp-0x24];
	}
}

struct {
	int (__cdecl * Init)(int, int, int);
	int (__cdecl * Shutdown)();
	int (__cdecl * DrawActiveFrame)(int, int, int);
} vmCalls;

__declspec(noinline) int CG_Init( int arg0, int arg1, int arg2 )
{
	ImportExport::CanSee = Engine::HasCleanPath;
	Stub_ConsoleInitialize();
	g_EntityEvent = unique_ptr<memoryDetour>(new memoryDetour((LPBYTE)(offsets.cg_entityevent), (LPBYTE)Stub_CG_EntityEvent, detourFuncs::DT_PUSHRET, 6));
	g_EntityEvent->Enable();
	g_AddCEnt = unique_ptr<memoryDetour>(new memoryDetour((LPBYTE)(offsets.cg_addcentity), (LPBYTE)Hook_CG_AddCEntity, detourFuncs::DT_PUSHRET) );
	g_AddCEnt->Enable();
	g_AddCEntityTramp = g_AddCEnt->GetTrampoline();
	Shaders[XORSTR("battleSuit")] = Original_Syscall(CG_R_REGISTERSHADER, XORSTR("powerups/battleSuit"));
	Shaders[XORSTR("regen")] = Original_Syscall(CG_R_REGISTERSHADER, XORSTR("powerups/regen"));
	Shaders[XORSTR("quad")] = Original_Syscall(CG_R_REGISTERSHADER, XORSTR("powerups/quad"));
	Shaders[XORSTR("white")] = Original_Syscall(CG_R_REGISTERSHADER, XORSTR("white"));
	Shaders[XORSTR("charset")] = Original_Syscall(CG_R_REGISTERSHADER, XORSTR("gfx/2d/bigchars"));
	Shaders[XORSTR("quadSound")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/items/quaddamage.wav"));
	Shaders[XORSTR("megaSound")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/items/m_health.wav"));
	Shaders[XORSTR("hs0")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/feedback/hit0.ogg"));
	Shaders[XORSTR("hs1")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/feedback/hit1.ogg"));
	Shaders[XORSTR("hs2")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/feedback/hit2.ogg"));
	Shaders[XORSTR("hs3")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/feedback/hit3.ogg"));
	Shaders[XORSTR("fightSound")] = Original_Syscall(CG_S_REGISTERSOUND, XORSTR("sound/feedback/fight.ogg"));
	timers.Clear();
	for(int i = 0; i < MAX_CLIENTS; ++i)
		healths[i] = 100;

	AccuracyManager::Init();

	return vmCalls.Init(arg0, arg1, arg2);
}

__declspec(noinline) int CG_Shutdown()
{
	Stub_ConsoleUnload();
	ImportExport::SaveVariables();
	g_GPAHook->Enable();
	g_EntityEvent->Disable();
	g_AddCEnt->Disable();
	vmCalls.Shutdown();
	return 0;
}



void __stdcall CG_DrawActiveFrame(int serverTime, int, int)
{
	int mbuttonList[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };
	for(auto i = 0; i < _countof(mbuttonList); ++i)
		MouseProxyProc(mbuttonList[i], (GetAsyncKeyState(mbuttonList[i]) < 0));
	g_firstRenderScene = true;
	ImportExport::User = ImportExport::Entities->GetPlayer (Engine::GetClientNum ());
	//DRAW 2D STUFF HERE
	if(Engine::HasValidSnapshot()
		&& Engine::GetTime() != 0)
	{
		Stub_RenderConsole();
		DoSpecList();
		DrawTimers();
		DrawRectangles();
		DoESP();
		DoRadar();
		if(ImportExport::Variables[VARSTATUSNOTIFIER]->IsEnabled())
		{
			DoVariableStatusNotifier();
		}
		if(ImportExport::Variables[ACCURACYCOUNTER]->IsEnabled())
		{
			DoAccuracyCounter();
		}
	}
}

__declspec(naked) int Stub_CG_DrawActiveFrame(int, int, int)
{
#define ARGS_SIZE 0xC
	__asm
	{
		push [esp+ARGS_SIZE];
		push [esp+ARGS_SIZE];
		push [esp+ARGS_SIZE];
		call vmCalls.DrawActiveFrame;
		add esp, ARGS_SIZE
		pushad;
		push [esp+0x20+ARGS_SIZE];
		push [esp+0x20+ARGS_SIZE];
		push [esp+0x20+ARGS_SIZE];
		call CG_DrawActiveFrame
		popad;
		ret
	}
}


__declspec(naked) int Original_Syscall(int, ...)
{
	static void * tmp_esp = 0;
	static void * tmp_ebx = 0;
	static void * tmp_ecx = 0;
	static void * tmp_edx = 0;
	__asm {
		mov tmp_edx, edx;
		mov tmp_ebx, ebx;
		mov tmp_ecx, ecx;
		mov edx, [esp];
		mov tmp_esp, edx;
		mov ebx, [esp+4];
		mov eax, retlabel;
		mov [esp+4], eax;
		mov ecx, [syscallptr];
		add esp, 4
		mov ecx, [ecx+ebx*4];
		push ecx;
		mov ebx, [tmp_ebx];
		mov ecx, [tmp_ecx];
		mov edx, [tmp_edx];
		add esp, 4;
		jmp [esp-4];
retlabel:
		mov tmp_edx, edx
		mov edx, [tmp_esp]
		sub esp, 4;
		push edx;
		mov edx, [tmp_edx];
		ret;
	}
}



__declspec(naked) void Hook_CG_AddCEntity()
{
	__asm
	{
		mov eax, [esi+4];
		cmp eax, 1;
		jnz isitem;
		push ebx;
		mov ebx, [esi+ENTITYSTATE_CLIENTNUM];
		call getClientNum;
		cmp eax, ebx;
		pop ebx;
		jz retr;
		mov eax, [esi+ENTITYSTATE_CLIENTNUM];
		mov [g_EntityBeingAdded], eax;
		jmp retr;
isitem:
		cmp eax, 2;
		jnz skip;
		mov [g_EntityBeingAdded], 2222;
		jmp retr;
skip:
		mov [g_EntityBeingAdded], 64;
retr:
		jmp g_AddCEntityTramp;
	}
}

void HookSyscalls()
{
#define HookTableEntry(idx, replacement) syscallTable[idx] = (DWORD)replacement;
	HookTableEntry(CG_R_RENDERSCENE, CG_R_RenderScene);
	HookTableEntry(CG_S_STARTSOUND, CG_S_StartSound);
	HookTableEntry(CG_S_STARTLOCALSOUND, CG_S_StartLocalSound);
	HookTableEntry(CG_R_ADDREFENTITYTOSCENE, CG_R_AddRefEntityToScene);
#undef HookTableEntry
}


void Hook_dllEntry(DWORD *** result, DWORD * pSyscalls, int * v)
{
	// We want extra room for this to grow
	syscallptr = pSyscalls; //store a pointer to the original syscall table
	//create our copy of the syscall table
	if(syscallTable)
		delete[] syscallTable;
	syscallTable = new DWORD[SYSCALLTABLE_SIZE];
	memcpy(syscallTable, pSyscalls, SYSCALLTABLE_SIZE);
	//replace the original entries of the syscalls we wanna hook
	HookSyscalls();
	// Perform basic initialization
	ImportExport::Initialize(new cEntList());
	// Get the cgamex86.dll module base
	auto CGDllBase = GetModuleHandle(XORSTR("cgamex86.dll"));
	// Get the ql dll module base
	auto QLDllBase = GetModuleHandle(XORSTR("quakelive.dll"));
	offsets.UpdateOffsets(CGDllBase, QLDllBase);

	// Get a pointer to CG_Trace
	g_CG_Trace = (tCG_Trace)(offsets.cg_trace);
	// Get a pointer to cgs_screenXScale (used for drawing)
	cgs_screenXScale = (float*)(offsets.cgs_screenxscale);
	// Get a pointer to button0, used for firing the weapon
	in_button0 = (kbutton_t*)(offsets.button0);
	// Disable damage feedback
	detourFuncs::unprotect((LPBYTE)(offsets.cg_damagefeedback_jnz), 1);
	*((byte*)(offsets.cg_damagefeedback_jnz)) = 0xEB;
	detourFuncs::reprotect((LPBYTE)(offsets.cg_damagefeedback_jnz), 1, PAGE_EXECUTE_READ);
	
	// Pass the new syscallTable to the original dllEntry
	pDllEntry(result, syscallTable, v);
	// Store real function pointers
	vmCalls.Init = (int (__cdecl *)(int,int,int))(*result)[0];
	vmCalls.Shutdown = (int (__cdecl *)(void))(*result)[1];
	vmCalls.DrawActiveFrame = (int (__cdecl *)(int,int,int))(*result)[3];
	// Redirect VM function pointers returned by dllEntry
	(*result)[0] = (DWORD*)&CG_Init;
	(*result)[1] = (DWORD*)&CG_Shutdown;
	(*result)[3] = (DWORD*)&Stub_CG_DrawActiveFrame;

	// No need to hook GetProcAddress until this VM is shut down
	g_GPAHook->Disable();
}