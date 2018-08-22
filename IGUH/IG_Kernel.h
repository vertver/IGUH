/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH kernel header
**********************************************************
* IG_Kernel.h
* Master-include for igKernel project
*********************************************************/

#pragma once
#include <windows.h>
#include <string>

#ifdef IG_EXPORTS
#define IGAPI __declspec(dllexport)
#else
#define IGAPI __declspec(dllimport)
#endif

typedef enum
{
	// standart codes
	IG_SUCCESS = 1,
	IG_BAD_STACK = 2,
	IG_BAD_ARGUMENT = 3,
	IG_BAD_HANDLE = 4,
	IG_UNKNOWN_DEVICE = 5,
	IG_INVALID_DEVICE = 6,

	// DirectX codes
	IGDX_BAD_DEVICE = 20,
	IGDX_BAD_ARGUMENT = 21,
	IGDX_BAD_STATE = 22,
	IGDX_THREAD_ERROR = 23,
	IGDX_COMMAND_LIST_ERROR = 24,

	// filesystem codes
	IGFS_BAD_PATH = 40,
	IGFS_BAD_ALLOC = 41,
	IGFS_BAD_FILE = 42,
	IGFS_BAD_POINTER = 43,
	IGFS_BAD_MAPPING = 44,
	IGFS_NO_ACCESS = 45,
	IGFS_BIG_FILE = 46,
	IGFS_WINDOWS_FAILED = 47,

	// threadsystem codes
	IGTH_THREAD_FAILED = 60

} IGCODE, *P_IGCODE;

IGAPI LPCSTR GetInfoFromErrorCode(_In_ IGCODE errorCode);
IGAPI LPCWSTR GetUnicodeStringFromAnsi(_In_ LPCSTR lpString);

struct CLOSE_HANDLE				{ void operator()(HANDLE h) { if (h) CloseHandle(h); } };
using SCOPE_HANDLE				= std::unique_ptr<void, CLOSE_HANDLE>;
using SET_THREAD_DESCRIPTION_CALL = HRESULT(WINAPI *)(HANDLE handle, PCWSTR name);
static SET_THREAD_DESCRIPTION_CALL lpSetThreadDescription = NULL;

#define DEBUG_MESSAGE(x)		OutputDebugStringA(x); OutputDebugStringA("\n");
#define CSUCCEEDED(x)			(x == IG_SUCCESS)	
#define CFAILED(x)				(!(x == IG_SUCCESS))	
#define _RELEASE(x)				if (x)						{ x->Release(); x = NULL; }
#ifndef DEBUG
#define R_CFAILED(x)			if (x != IG_SUCCESS)		{ DEBUG_MESSAGE(GetInfoFromErrorCode(x)); }
#define R_ASSERT(x)				if (!x)						{ DEBUG_MESSAGE("R_ASSERT"); }
#else
#define R_CFAILED(x)			if (x != IG_SUCCESS)		{ DEBUG_MESSAGE(GetInfoFromErrorCode(x)); __debugbreak(); }
#define R_ASSERT(x)				if (!x)						{ DEBUG_MESSAGE("R_ASSERT"); __debugbreak(); }
#endif

#define FILE_INFO_FLAG			(1 << 0)
#define TRIPLE_BUFFERING_FLAG	(1 << 1)

typedef struct  
{
	DWORD dwType;
	DWORD dwSize;
	BYTE* lpFile;
	LPCSTR lpPath;
} FILE_DATA;

typedef struct
{
	DWORD dwType;
	LPCSTR szName;
	DWORD dwThreadID;
	DWORD dwFlags;	
} THREADNAME_INFO;

namespace IGKernel
{
	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		IGAPI IGCODE OpenFileDialog(_Out_ BYTE** lpBuffer, _Out_ LPDWORD dwSize);

		IGAPI IGCODE LoadFileToBuffer(_In_ LPCSTR lpPath, _In_ DWORD dwFlags, _Out_ BYTE** lpBuffer, _Out_ LPDWORD dwSize);
		IGAPI IGCODE LoadFileToFileStruct(_In_ LPCSTR lpPath, _In_ DWORD dwFlags, _Out_ FILE_DATA* fileData);
		IGAPI IGCODE LoadFileToMapBuffer(_In_ LPCSTR lpPath, _In_ DWORD dwPointerSize, _In_ DWORD dwStartAdress, _In_ DWORD dwEndAdress, _Out_ BYTE** lpBuffer);

		IGAPI IGCODE CreateFileFromBuffer(_In_ LPCSTR lpName, _In_ DWORD dwFullFileSize, _In_ LPCSTR lpPath, _In_ BYTE* lpFile);
		IGAPI IGCODE CreateFileFromStruct(_In_ LPCSTR lpName, _In_ LPCSTR lpPath, _In_ FILE_DATA fileData);

		IGAPI IGCODE FreeMapBuffer(_In_opt_ BYTE** lpBuffer);
		IGAPI IGCODE FreeFile(_In_opt_ BYTE** lpFile);
	private:
		FILE_DATA fData;
		LPVOID lpData;
		std::unique_ptr<FileSystem> lpFS;
		HANDLE hHeap;
	};
	class Thread
	{
	public:
		IGAPI IGCODE CreateCustomThread(_In_ LPVOID lpFunc, _In_ LPVOID lpArg, _In_ LPCSTR lpMutexName, _Out_ void** hMutex, _Out_ LPDWORD lpThreadId);
		IGAPI IGCODE SetThreadName(_In_ DWORD dwThreadID, _In_ LPCSTR lpThreadName);
	};
}
