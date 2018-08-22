/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH threadsystem
**********************************************************
* IG_Thread.cpp
* Threadsystem implementation
*********************************************************/

#include "IG_Kernel.h"
const DWORD MS_VC_EXCEPTION = 0x406D1388;

IGCODE
IGKernel::Thread::CreateCustomThread(
	_In_ LPVOID lpFunc, 
	_In_ LPVOID lpArg, 
	_In_ LPCSTR lpMutexName, 
	_Out_ void** hMutex,
	_Out_ LPDWORD lpThreadId
)
{
	DWORD dwThreadID;
	HANDLE hThread;

	// create mutex and thread
	*hMutex = CreateMutexA(NULL, NULL, lpMutexName);
	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)lpFunc, lpArg, NULL, &dwThreadID);
	if (!hThread)
	{
		return IGTH_THREAD_FAILED;
	}

	*lpThreadId = dwThreadID;
	return IG_SUCCESS;
}

IGCODE 
IGKernel::Thread::SetThreadName(
	_In_ DWORD dwThreadID,
	_In_ LPCSTR lpThreadName
)
{
	// load kerner32 library for check our method
	HMODULE hLib = GetModuleHandle("kernel32.dll");

	lpSetThreadDescription = (SET_THREAD_DESCRIPTION_CALL)GetProcAddress(hLib, "SetThreadDescription");

	// if our Windows version is 10.0.1603 or greater - 
	// try to use SetThreadDescription function
	if (lpSetThreadDescription)
	{
		// set thread name by Windows 10 function
		if (FAILED(lpSetThreadDescription(GetCurrentThread(), GetUnicodeStringFromAnsi(lpThreadName))))
		{
			return IGTH_THREAD_FAILED;
		}
	}
	else
	{
		// else make by custom method
		THREADNAME_INFO stName;
		ZeroMemory(&stName, sizeof(THREADNAME_INFO));
		stName.dwType = 0x1000;
		stName.dwThreadID = dwThreadID;
		stName.szName = lpThreadName;

		// try to raise exception to set name of thread
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(stName) / sizeof(ULONG_PTR), (ULONG_PTR*)&stName);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION)
		{
		}
	}
	return IG_SUCCESS;
}
