#pragma once
#ifndef HOOK_H
#define HOOK_H

#include <map>
#include <string>
#include "xorClass.h"
#include "SDKIncludes.h"

#define QL_NAMECHANGE 0xCCCD3

#define ENTITYSTATE_CLIENTNUM 0xB0
#define REFENTITY_CUSTOMSHADER 0x70
#define REFENTITY_SHADERRGBA (REFENTITY_CUSTOMSHADER + 7)

enum Syscalls
{
	CG_S_STARTSOUND = 0x25, ///
	CG_S_STARTLOCALSOUND = 0x26, ///
	CG_S_REGISTERSOUND = 0x2C, ///
	CG_R_REGISTERSHADER = 0x32, ///
	CG_R_ADDREFENTITYTOSCENE = 0x45,///
	CG_R_ADDPOLYTOSCENE = 0x46,//?
	CG_R_RENDERSCENE = 0x4A,///
	CG_R_SETCOLOR = 0x4C,///
	CG_R_DRAWSTRETCHPIC = 0x4D,//?
	CG_GETCURRENTCMDNUMBER = 0x56,
	CG_GETUSERCMD = 0x57,
};

typedef int (__cdecl * tSyscall)(int, ...);
typedef int (__cdecl * tVmMain)( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  );
extern void (__cdecl * pDllEntry)(unsigned long *** res, void * syscalladdr, int * v);

extern tVmMain vmMainPtr;
extern unsigned long * syscallptr;
extern int Original_Syscall(int call, ...);


int Hook_vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  );
void Hook_dllEntry(unsigned long ***, unsigned long *, int*);


extern int (__cdecl * CG_DrawPic)(float x, float y, float width, float height, int shaderHandle);

extern float * cgs_screenXScale;


extern std::map<std::string, int> Shaders;
extern int g_EntityBeingAdded;
extern bool g_firstRenderScene;

//used for the health ESP
extern int healths[MAX_CLIENTS];

#endif