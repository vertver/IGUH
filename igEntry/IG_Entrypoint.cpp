/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH entry-point
**********************************************************
* IG_EntryPoint.cpp
* Main entry-point for application
*********************************************************/

#include "IG_Kernel.h"
#include "IG_Render.h"
#include "IG_Audio.h"

IGKernel::FileSystem fileSystem;
IGRender::BaseRender baseRender;
IGAudio::FileSystem audioFileSystem;
IGAudio::XAudio xAudio;

BOOL
WINAPI
WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	// we can't get previos instance
	if (hInstance == hPrevInstance)
	{
		return _SW_STACKUNDERFLOW;
	}

	BYTE* lpFile = nullptr;
	DWORD dwFileSize = NULL;
	fileSystem.OpenFileDialog(&lpFile, &dwFileSize);
	xAudio.PlayAudioFile(lpFile, dwFileSize);
	return NULL;
}
