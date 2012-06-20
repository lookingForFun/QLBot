#include <Windows.h>
#include "hook.h"
#include "euDetours.h"
#include <winnt.h>
#include "../Common/ImportExport.h"
#include <time.h>

extern "C"
{
extern void __cdecl _initp_misc_cfltcvt_tab();
extern void __cdecl _fpmath(int /*initPrecision*/);
}

memoryDetour * g_GPAHook = nullptr;

#define MB(x) MessageBox(0, ###x, "", 0);
#define MBA(x) MessageBox(0, x, "", 0);

__declspec(noinline) FARPROC WINAPI Hook_GetProcAddress(HMODULE mod, LPCSTR fn)
{
	// Get the original return value
	FARPROC ret = ((FARPROC (WINAPI*) (HMODULE, LPCSTR))g_GPAHook->GetTrampoline())(mod, fn);
	// If GPA was called for cgamex86.dll and the procedure to be found is dllEntry
	if(mod != nullptr && fn != nullptr && !IsBadReadPtr(fn, 1) && (*fn) != 0 && !strcmp(XORSTR("dllEntry"), fn) && GetModuleHandle(XORSTR("cgamex86.dll")) == mod)
	{
		pDllEntry = (void (__cdecl*)(DWORD ***, void*, int*))GetProcAddress(mod, XORSTR("dllEntry"));
		return (FARPROC)&Hook_dllEntry;
	}
	return ret;
}

FARPROC WINAPI Stub_Hook_GetProcAddress(HMODULE mod, LPCSTR fn)
{
	return Hook_GetProcAddress(mod, fn);
}

#include <ShlObj.h>

BOOL APIENTRY DllMain(_In_ HANDLE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID)
{
	if(_Reason == DLL_PROCESS_ATTACH)
	{
		// Check for admin privileges
		if(!IsUserAnAdmin())
		{
			MessageBox(0, XORSTR("Your browser must be running with administrative privileges"), "", MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), 0);
			ExitProcess(0);
			return FALSE;
		}
		//INIT FP MATH
		_fpmath(TRUE);
		_initp_misc_cfltcvt_tab();
		srand(time(nullptr));

		// Create and enable GetProcAddress hook
		g_GPAHook = new memoryDetour((LPBYTE)GetProcAddress(GetModuleHandle(XORSTR("kernel32.dll")), XORSTR("GetProcAddress")), (LPBYTE)Stub_Hook_GetProcAddress);
		g_GPAHook->Enable();
	}
	return TRUE;
}
