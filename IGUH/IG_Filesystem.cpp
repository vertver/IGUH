/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH filesystem
**********************************************************
* IG_Filesystem.cpp
* Filesystem implementation
*********************************************************/
#pragma once

#include "IG_Kernel.h"

WCHAR szBuf[48];
CHAR szName[MAX_PATH];

IGKernel::FileSystem::FileSystem()
{
	// create heap with 64kb page
	hHeap = HeapCreate(NULL, 0x010000, NULL);
}

IGKernel::FileSystem::~FileSystem()
{
	// destroy custom heap
	HeapDestroy(hHeap);
}

IGCODE 
IGKernel::FileSystem::OpenFileDialog(
	_Out_ BYTE** lpBuffer, 
	_Out_ LPDWORD dwSize
)
{
	// set filedialog struct
	szName[0] = '\0';		// needy for correct filedialog work

	OPENFILENAMEA oFN = {};
	// get params to our struct
	ZeroMemory(&oFN, sizeof(OPENFILENAMEA));
	oFN.lStructSize = sizeof(OPENFILENAMEA);
	oFN.hwndOwner = NULL;
	oFN.nMaxFile = MAX_PATH;
	oFN.lpstrFile = szName;
	oFN.lpstrFilter = "All Files\0*.*\0\0";
	oFN.lpstrTitle = "Open file";
	oFN.lpstrFileTitle = NULL;
	oFN.lpstrInitialDir = NULL;
	oFN.nFilterIndex = 1;
	oFN.nMaxFileTitle = 0;
	oFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// if we can't open filedialog - exit
	if (!GetOpenFileNameA(&oFN))
	{
		return IGFS_BAD_PATH;
	}

	return LoadFileToBuffer(szName, FILE_INFO_FLAG, lpBuffer, dwSize);
}

IGCODE
IGKernel::FileSystem::LoadFileToBuffer(
	_In_ LPCSTR lpPath,
	_In_ DWORD dwFlags,
	_Out_ BYTE** lpBuffer, 
	_Out_ LPDWORD dwSize
)
{
	DWORD dwSizeWritten = NULL;

	// open file to handle 
	SCOPE_HANDLE hFile(CreateFileA(
		lpPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
	);	
	if (!hFile)
	{
		// if file is empty or CreateFile is failed
		return IGFS_BAD_PATH;
	}

	DWORD dwFileSize = NULL;

	// if we want to use without any information about file - get here
	if (!(dwFlags == FILE_INFO_FLAG))
	{
		dwFileSize = GetFileSize(hFile.get(), NULL);
		*lpBuffer = (BYTE*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwFileSize);

		if (!ReadFile(hFile.get(), *lpBuffer, dwFileSize, &dwSizeWritten, NULL))
		{
			return IGFS_BAD_ALLOC;
		}
	}
	else
	{
		FILE_STANDARD_INFO fileInfo = { NULL };

		// get file information
		if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(FILE_STANDARD_INFO)))
		{
			return IGFS_BAD_FILE;
		}
		
		dwFileSize = fileInfo.EndOfFile.LowPart;

		if (fileInfo.EndOfFile.HighPart > NULL)
		{
			// the file is too big to be allocated
			return IGFS_BIG_FILE;
		}

		// allocate our buffer 
		*lpBuffer = (BYTE*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, dwFileSize);

		if (!ReadFile(hFile.get(), *lpBuffer, dwFileSize, &dwSizeWritten, NULL))
		{
			return IGFS_BAD_ALLOC;
		}
	}
	*dwSize = dwSizeWritten;
	return IG_SUCCESS;
}

IGCODE
IGKernel::FileSystem::LoadFileToFileStruct(
	_In_ LPCSTR lpPath,
	_In_ DWORD dwFlags,
	_Out_ FILE_DATA* fileData
)
{
	DWORD dwSizeWritten = NULL;

	// open file to handle 
	SCOPE_HANDLE hFile(CreateFileA(
		lpPath,
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,
		NULL)
	);
	if (!hFile)
	{
		// if file is empty or CreateFile is failed
		return IGFS_BAD_PATH;
	}

	// if we want to use without any information about file - get here
	if (!(dwFlags == FILE_INFO_FLAG))
	{
		fData.dwType = GetFileType(hFile.get());
		fData.dwSize = GetFileSize(hFile.get(), NULL);
		fData.lpFile = (BYTE*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, fData.dwSize);
		fData.lpPath = lpPath;

		if (!ReadFile(hFile.get(), fData.lpFile, fData.dwSize, &dwSizeWritten, NULL))
		{
			return IGFS_BAD_ALLOC;
		}
	}
	else
	{
		FILE_STANDARD_INFO fileInfo = { NULL };

		// get file information
		if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(FILE_STANDARD_INFO)))
		{
			return IGFS_BAD_FILE;
		}

		if (fileInfo.EndOfFile.HighPart > NULL)
		{
			// the file is too big to be allocated
			return IGFS_BIG_FILE;
		}

		// allocate our buffer 
		fData.dwType = GetFileType(hFile.get());
		fData.dwSize = fileInfo.EndOfFile.LowPart;
		fData.lpFile = (BYTE*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, fData.dwSize);

		if (!ReadFile(hFile.get(), fData.lpFile, fData.dwSize, &dwSizeWritten, NULL))
		{
			return IGFS_BAD_ALLOC;
		}
	}

	*fileData = fData;
	return IG_SUCCESS;
}

IGCODE 
IGKernel::FileSystem::CreateFileFromBuffer(
	_In_ LPCSTR lpName,
	_In_ DWORD dwFullFileSize,
	_In_ LPCSTR lpPath,
	_In_ BYTE* lpFile
)
{
	DWORD dwFileSize = NULL;

	// get handle to create file
	std::string lpPathFile = lpPath + std::string("\\") + lpName;
	SCOPE_HANDLE hFile(CreateFileA(
		lpPath,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
	);
	if (!hFile)
	{
		return IGFS_WINDOWS_FAILED;
	}

	// write this file from our pointer
	if (!WriteFile(hFile.get(), lpFile, dwFullFileSize, &dwFileSize, NULL))
	{
		return IGFS_WINDOWS_FAILED;
	}
	return IG_SUCCESS;
}

IGCODE
IGKernel::FileSystem::CreateFileFromStruct(
	_In_ LPCSTR lpName,
	_In_ LPCSTR lpPath,
	_In_ FILE_DATA fileData
)
{
	DWORD dwFileSize = NULL;

	// get handle to create file
	SCOPE_HANDLE hFile(CreateFileA(
		lpPath,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
	);
	if (!hFile)
	{
		return IGFS_WINDOWS_FAILED;
	}

	// write file from file info
	if (!WriteFile(hFile.get(), fileData.lpFile, fileData.dwSize, &dwFileSize, NULL))
	{
		return IGFS_WINDOWS_FAILED;
	}
	return IG_SUCCESS;
}

IGCODE
IGKernel::FileSystem::LoadFileToMapBuffer(
	_In_ LPCSTR lpPath, 
	_In_ DWORD dwPointerSize, 
	_In_ DWORD dwStartAdress,
	_In_ DWORD dwEndAdress,
	_Out_ BYTE** lpBuffer
)
{
	HANDLE hFileMap = NULL;
	DWORD dwSizeWritten = NULL;

	// load file handle
	SCOPE_HANDLE hFile(CreateFileA(
		lpPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)
	);
	if (!hFile)
	{
		return IGFS_BAD_PATH;
	}

	// create map with our file
	hFileMap = CreateFileMappingA(hFile.get(), NULL, PAGE_READWRITE, NULL, dwPointerSize, NULL);
	if (hFileMap)
	{
		return IGFS_BAD_MAPPING;
	}

	// map file in current process
	*lpBuffer = (BYTE*)MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, dwStartAdress, dwEndAdress, dwPointerSize);
	if (!lpBuffer)
	{
		return IGFS_BAD_POINTER;
	}

	return IG_SUCCESS;
}

IGCODE
IGKernel::FileSystem::FreeMapBuffer(
	_In_ BYTE** lpBuffer
)
{ 
	// unmap our buffer
	if (!UnmapViewOfFile(*lpBuffer))
	{
		return IGFS_BAD_POINTER;
	}
	else
	{
		return IG_SUCCESS;
	}
}

IGCODE
IGKernel::FileSystem::FreeFile(
	_In_opt_ BYTE** lpFile
)
{
	if (*lpFile)
	{
		if (!HeapFree(hHeap, HEAP_NO_SERIALIZE, *lpFile))
		{
			return IG_BAD_STACK;
		}
		*lpFile = NULL;
	}
	return IG_SUCCESS;
}

LPCWSTR
GetUnicodeStringFromAnsi(
	_In_ LPCSTR lpString
)
{
	size_t uSize = strlen(lpString);

	mbstowcs_s(&uSize, szBuf, lpString, uSize);
	return szBuf;
}
