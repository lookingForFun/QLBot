#include <windows.h>
#include "euDetours.h"


#define GET_PAGE(a) ((LPBYTE)(((unsigned long)a)&0xfffff000))
#define MakePtr( cast, ptr, addValue )( cast )( ( DWORD )( ptr ) + ( DWORD )( addValue ) )
#define uint unsigned int


DWORD detourFuncs::unprotect(LPBYTE addr, size_t len) { 
	DWORD tmp; 
	VirtualProtect( addr, len, PAGE_EXECUTE_READWRITE, &tmp);
	return tmp;
}

void detourFuncs::reprotect(LPBYTE addr, size_t len, DWORD protection) { 
	DWORD tmp;
	if(!protection)
		protection = PAGE_EXECUTE_READ;
	VirtualProtect(addr,len,protection,&tmp);
}

unsigned detourLengths[detourFuncs::DT_COUNT] = { 5, 6, 6, 7, 8 };

LPBYTE detourFuncs::detour_function(LPBYTE orig, LPBYTE det, size_t len, int p_trampolineType)
{
	unsigned char *tramp;
	DWORD protectionBackup;
	unsigned l_minLen = detourLengths[p_trampolineType];
	if(len < l_minLen)
		return nullptr;
	tramp = new unsigned char[len + 5];
	if(tramp != nullptr)
	{
		// ensure that the trampoline is executable
		unprotect(tramp, len + 5);
		// fill the trampoline with each instruction from the target
		DWORD tmp = 0;
		while(tmp < len) {
			// copy instruction to the trampoline
			tmp = DetourCopyInstruction(tramp+tmp, orig+tmp, nullptr) - orig;
		}
		// add a jump to go back to the original code
		tramp[len] = 0xE9;
		*reinterpret_cast<LPVOID*>(tramp + len + 1) = reinterpret_cast<LPVOID>((orig + len) - (tramp + len + 5));
		// make the original code writable
		protectionBackup = unprotect (GET_PAGE (orig), 4096);
		switch(p_trampolineType)
		{
		case DT_JMP:
			*orig = 0xE9;
			*reinterpret_cast<LPVOID*>(orig + 1) = (void *) (((uint) det) - (((uint) orig) + 5));
			break;
		case DT_NOP:
			*orig = 0x90;
			*(orig + 1) = 0xE9;
			*reinterpret_cast<LPVOID*>(orig + 2) = (void *) (((uint) det) - (((uint) orig) + 6));
			break;
		case DT_PUSHRET:
			*orig = 0x68;
			*reinterpret_cast<LPVOID*>(orig + 1) = det;
			*(orig + 5) = 0xC3;
			break;
		case DT_CLCJNC:
			*orig = 0xF8;
			*(orig + 1) = 0x0F;
			*(orig + 2) = 0x83;
			*reinterpret_cast<LPVOID*>(orig + 3) = det;
			break;
		default:
			return nullptr;
		}
		reprotect (GET_PAGE(orig), 4096, protectionBackup);
	}
	return tramp;
}

void * detourFuncs::InterceptDllCall( HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction, void* pOldFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect;
	DWORD dwOldProtect2;
	if(!pOldFunction)
		if( !( pOldFunction = GetProcAddress( GetModuleHandle( szDllName ), szFunctionName ) ) )
			return 0;

	pDosHeader = ( PIMAGE_DOS_HEADER )hModule;
	if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return( nullptr );

	pNTHeader = MakePtr( PIMAGE_NT_HEADERS, pDosHeader,pDosHeader->e_lfanew );
	if( pNTHeader->Signature != IMAGE_NT_SIGNATURE
		|| ( pImportDesc = MakePtr( PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ) ) == ( PIMAGE_IMPORT_DESCRIPTOR )pNTHeader )
		return( nullptr );

	while( pImportDesc->Name )
	{
		char *szModuleName = MakePtr( char *, pDosHeader,	pImportDesc->Name );
		if( !_stricmp( szModuleName, szDllName ) )
			break;
		pImportDesc++;
	}
	if( pImportDesc->Name == NULL )
		return( nullptr );

	pThunk = MakePtr( PIMAGE_THUNK_DATA, pDosHeader,	pImportDesc->FirstThunk );
	while( pThunk->u1.Function )
	{
		if( pThunk->u1.Function == ( DWORD )pOldFunction )
		{
			VirtualProtect( ( void * )&pThunk->u1.Function, sizeof( DWORD ), PAGE_EXECUTE_READWRITE, &dwOldProtect );
			pThunk->u1.Function = ( DWORD )pNewFunction;
			VirtualProtect( ( void * )&pThunk->u1.Function, sizeof( DWORD ), dwOldProtect, &dwOldProtect2 );
			return( pOldFunction );
		}
		pThunk++;
	}
	return( nullptr ); 
}


void detourFuncs::undetour_function(LPBYTE orig, void* tramp, size_t len, bool bReleaseTramp) {
	DWORD protection = unprotect (GET_PAGE (orig), 4096);
	memcpy(orig,tramp,len);
	//refix any jmp at the beginning of the trampoline
	if(((unsigned char*)orig)[0] == 0xE9)
	{
		uint offset = *(unsigned int*)((uint)tramp+1);
		uint target = (offset+(uint)tramp+5);
		*(unsigned int*)(((unsigned char*)orig)+1) = (target - (uint)(((unsigned char*)orig)+5));
	}
	reprotect (GET_PAGE (orig), 4096, protection);
	if(bReleaseTramp)
		delete[]tramp;
}

void memoryDetour::Enable()
{
	if(!_isActive)
	{
		if(!_length)
		{
			unsigned l_size = 0;
			byte l_tmpBuf[16] = "";
			// Determine the required length of the trampoline
			do
			{
				BYTE * l_nextInstr = DetourCopyInstruction(l_tmpBuf, (byte*)_offset + l_size, nullptr);
				l_size = l_nextInstr - (byte*)_offset;
			} while(l_size < detourLengths[_trampolineType]);
			_length = l_size;
		}
		_trampoline = detourFuncs::detour_function(_offset, _target, _length, _trampolineType);
		if(_trampoline != nullptr) {
			_isActive = true;
		}
	}
}

void memoryDetour::Disable(bool bReleaseTrampoline)
{
	if(_isActive)
	{
		//DetourRemove((PBYTE)GetTrampoline(), (PBYTE)_target);
		/*if(bReleaseTrampoline)
			free(_trampoline);*/
		detourFuncs::undetour_function(_offset, _trampoline, _length, bReleaseTrampoline);
		_isActive = false;
	}
}

IATDetour::IATDetour( char * funcName, char * sourceDll, char * targetDll, unsigned char * Replacement ) : Detour()
{
	this->funcName = funcName;
	this->sourceDll = sourceDll;
	this->targetDll = targetDll;
	_target = Replacement;
}

void IATDetour::Enable()
{
	if(!_isActive)
	{
		_trampoline = reinterpret_cast<LPBYTE>(detourFuncs::InterceptDllCall(LoadLibrary(targetDll), sourceDll, funcName, (DWORD)_target));
		_isActive = true;
	}
}

void IATDetour::Disable(bool)
{
	if(_isActive)
	{
		detourFuncs::InterceptDllCall(GetModuleHandle(targetDll), sourceDll, funcName, (DWORD)_trampoline, _target);
		_isActive = false;
	}
}

