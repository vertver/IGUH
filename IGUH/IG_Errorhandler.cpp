/******************************************************
* Copyright(C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT - License
* *********************************************************
* Module Name : IGUH error handler
**********************************************************
* IG_Errorhandler.cpp
* Error handler
*********************************************************/

#include "IG_Kernel.h"

LPCSTR lpErrorString;

LPCSTR 
GetInfoFromErrorCode(
	_In_ IGCODE errorCode
)
{
	switch (errorCode)
	{
	case IG_BAD_ARGUMENT:
		lpErrorString = "Bad function argument";
		break;
	case IG_BAD_HANDLE:
		lpErrorString = "Bad handle";
		break;
	case IG_BAD_STACK:
		lpErrorString = "Bad stack";
		break;
	case IG_INVALID_DEVICE:
		lpErrorString = "Invalid internal device";
		break;
	case IG_UNKNOWN_DEVICE:
		lpErrorString = "Unknown internal device";
		break;
	case IGDX_BAD_DEVICE:
		lpErrorString = "Bad DirectX device";
		break;
	case IGDX_BAD_STATE:
		lpErrorString = "Bad DirectX state";
		break;
	case IGDX_BAD_ARGUMENT:
		lpErrorString = "Bad DirectX function argument";
		break;
	case IGDX_THREAD_ERROR:
		lpErrorString = "Bad DirectX thread";
		break;
	case IGFS_BAD_ALLOC:
		lpErrorString = "Can't allocate file";
		break;
	case IGFS_BAD_FILE:
		lpErrorString = "Bad file";
		break;
	case IGFS_BAD_PATH:
		lpErrorString = "Bad file path";
		break;
	case IGFS_BAD_POINTER:
		lpErrorString = "Bad file pointer";
		break;
	case IGFS_BIG_FILE:
		lpErrorString = "File is too big to be opened";
		break;
	case IGFS_NO_ACCESS:
		lpErrorString = "Can't access to file";
		break;
	case IGFS_WINDOWS_FAILED:
		lpErrorString = "Unknown Windows error";
		break;
	case IGTH_THREAD_FAILED:
		lpErrorString = "Failed to create thread";
		break;
	case IG_SUCCESS:
	default:
		lpErrorString = "No error";
		break;
	}

	return lpErrorString;
}
