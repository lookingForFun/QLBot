#ifndef _SHAREDMEMORY_H
#define	_SHAREDMEMORY_H

#include <string>
using namespace std;

class CSharedMemory
{
protected:
	HANDLE	m_hSharedMemoryFile;
	// Mu� z.Z. public bleiben,
	// Da Dynamark1 und ?? direkt auf m_pwData zugreifen.
public:
	void *	m_pwData;
protected:
	DWORD	dwMaximumSizeHigh;
	DWORD	dwMaximumSizeLow;
	DWORD	dwNumberOfBytesToMap;
	BOOL	m_bInit;
	BOOL	m_bAlreadyExist;

	string csMutexName;
	HANDLE m_hMutex;
	// Mu� z.Z. public bleiben,
	// Da Dynamark1 und ?? direkt auf csName zugreifen.
public:
	// Im Sytem eindeutiger Name f�r gemeinsamen Speicherbereich
	string csName;
public:
	// Pointer auf gemeinsamen Speicher
	void* GetData()
	{
		if( m_bInit )
			return m_pwData;
		else
			return NULL;
	}
	//Constr ohne Init
	CSharedMemory(LPCSTR cs){csName = cs;m_bInit = FALSE;m_bAlreadyExist = FALSE;csMutexName = csName + "Mutex";}
	//Constr mit Init
	CSharedMemory(LPCSTR cs, int size){csName = cs;m_bAlreadyExist = FALSE;csMutexName = csName + "Mutex"; Init(size);}
	BOOL Init(LPCSTR cs, int size){csName = cs;m_bAlreadyExist = FALSE;csMutexName = csName + "Mutex"; return Init(size);}
	BOOL Init(int size ,LPCSTR cs){csName = cs;m_bAlreadyExist = FALSE;csMutexName = csName + "Mutex"; return Init(size);}
	BOOL Init( int size )
	{
		m_hMutex = CreateMutex(NULL,FALSE,csMutexName.c_str());
		dwNumberOfBytesToMap = size;
		m_hSharedMemoryFile = CreateFileMapping((HANDLE)0xFFFFFFFF,
			NULL,
			PAGE_READWRITE,
			0/*dwMaximumSizeHigh*/,
			dwNumberOfBytesToMap/*dwMaximumSizeLow*/,
			csName.c_str());
		if(m_hSharedMemoryFile == NULL)
		{
			m_bAlreadyExist = FALSE;
			m_bInit = FALSE;
			return FALSE;
		}
		else
		{
			if(GetLastError() == ERROR_ALREADY_EXISTS)
				m_bAlreadyExist = TRUE;
		}
		m_pwData = MapViewOfFile(m_hSharedMemoryFile,
			FILE_MAP_WRITE,
			0/*dwFileOffsetHigh*/,
			0/*dwFileOffsetLow*/,
			dwNumberOfBytesToMap);
		if(m_pwData == NULL)
		{
			m_bInit = FALSE;
			CloseHandle(m_hSharedMemoryFile);
			return FALSE;
		}
		else
			m_bInit = TRUE;
		return TRUE;
	}
	~CSharedMemory()
	{
		if (m_bInit)
		{
			UnmapViewOfFile(m_pwData);
			CloseHandle(m_hSharedMemoryFile);
		}
	}
public:
	//return:TRUE if it is not the first Instance
	BOOL AlreadyExist()
	{
		return m_bAlreadyExist;
	}
	//Locks the Mem like CSingelLock locker(TRUE,pSyncObj)
	// Use:
	// {
	//  CSharedMemory::Locker locker(&MySharedMem);
	//  Modify(MySharedMem->GetData() );
	// }
	struct Locker
	{
		Locker(CSharedMemory* sm)
		{
			m_sm = sm;
			m_sm->Lock();
		}
		Locker(CSharedMemory& sm)
		{
			m_sm = &sm;
			m_sm->Lock();
		}
		~Locker()
		{
			m_sm->Unlock();
		}
		CSharedMemory* m_sm;
	};
	//Locks the Mem
	// Use:
	// if( MySharedMem.Lock(100) )
	// {
	//    Modify(MySharedMem->GetData() );
	//    MySharedMem.Unlock();
	// }
	BOOL Lock(DWORD dwMilliSec = INFINITE)
	{
		if( WaitForSingleObject(m_hMutex,dwMilliSec) == WAIT_OBJECT_0)
			return TRUE;
		return FALSE;
	}
	BOOL Unlock(DWORD dwMilliSec = INFINITE)
	{
		return ReleaseMutex(m_hMutex);
	}

};
#endif