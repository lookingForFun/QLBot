#pragma once

#ifndef EUDETOURSH
#define EUDETOURSH

#include "../Common/detours.h"

namespace detourFuncs
{
	LPBYTE detour_function(LPBYTE orig, LPBYTE det, size_t len, int p_trampolineType);
	void undetour_function(LPBYTE orig, void* tramp, size_t len, bool bReleaseTramp);
	void * InterceptDllCall( HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction, void* pOldFunction = nullptr );
	DWORD unprotect(LPBYTE addr,size_t len);
	void reprotect(LPBYTE addr,size_t len, DWORD protection);
	enum
	{
		DT_JMP = 0,
		DT_NOP,
		DT_PUSHRET,
		DT_CLCJNC,
		DT_CLASSFUNC,
		DT_COUNT //not for use
	};
}

class Detour
{
protected:
	LPBYTE _offset;
	LPBYTE _target;
	LPBYTE _trampoline;
	bool   _isActive;

public:
	Detour() { _isActive = false; }
	Detour(LPBYTE offset, LPBYTE target) {
		_offset = offset; 
		_target = target;
		_isActive = false;
	}

	//activators
	virtual void Enable() = 0;
	virtual void Disable(bool bReleaseTrampoline = true) = 0;
	
	//accessors
	void * GetOffset() const { return _offset; }
	void * GetReplacement() const { return _target; }
	void * GetTrampoline() const { return _trampoline; }
	void SetOffset(LPBYTE newOffset) { _offset = newOffset; }
	void SetReplacement(LPBYTE newReplacement) { _target = newReplacement; }
	bool IsActive() const { return _isActive; }
};

class memoryDetour : public Detour
{
private:
	size_t	_length;
	int		_trampolineType;

public:
	
	memoryDetour() : Detour() {}
	memoryDetour(LPBYTE Address, LPBYTE Replacement, int p_trampolineType = detourFuncs::DT_JMP, size_t length = 0) : Detour(Address, Replacement) { 
		_length = length;
		_trampolineType = p_trampolineType;
	}
	
	void Enable();
	void Disable(bool bReleaseTrampoline = true);
};

class IATDetour : public Detour
{
private:
	char * funcName;
	char * sourceDll;
	char * targetDll;
public:
	IATDetour(char * funcName, char * sourceDll, char * targetDll, unsigned char * Replacement);
	
	void Enable();
	void Disable(bool bReleaseTrampoline = true);
};

#endif