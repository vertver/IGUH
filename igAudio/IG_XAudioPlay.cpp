/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH XAudio implementation
**********************************************************
* IG_XAudioPlay.cpp
* Audio master-include
*********************************************************/

#include "IG_Audio.h"
IGCODE 
IGAudio::XAudio::GetCurrentDevice(
	_Out_ LPDWORD dwDeviceID, 
	_Out_ CONST CHAR** lpDeviceName
)
{
	return IG_SUCCESS;
}

IGCODE 
IGAudio::XAudio::PlayAudioFile(
	_In_ BYTE* lpBuffer,
	_In_ DWORD dwSize
)
{
	WAVEFORMATEX waveFormatX;

	audioFileSystem.LoadToSimpleBuffer(NULL, dwSize, &lpBuffer, &waveFormatX);

	return IG_SUCCESS;
}