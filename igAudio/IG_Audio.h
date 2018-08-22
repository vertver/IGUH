/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH Audio header
**********************************************************
* IG_Audio.h
* Audio master-include
*********************************************************/

#pragma once
#include "IG_Kernel.h"
#include <string>
#include <stdio.h>
#include <process.h>
#include <mmreg.h>
#include <mmeapi.h>
#include <mmdeviceapi.h>	// minimal MME API
#include <cguid.h>
#include <xaudio2.h>

#define STREAMING_BUFFER_SIZE 65536
#define MAX_BUFFER_COUNT 3

typedef struct
{
	uint32_t tag;				// tag of RIFF chunk
	uint32_t size;				// chunk size
} RIFFChunk;

typedef struct
{
	uint32_t tag;				// tag of RIFF chunk header
	uint32_t size;				// chunk header size
	uint32_t riff;				// RIFF info
} RIFFChunkHeader;

typedef struct
{
	static const uint32_t LOOP_TYPE_FORWARD = 0x00000000;
	static const uint32_t LOOP_TYPE_RELEASE = 0x00000001;

	uint32_t size;				// chunk size
	uint32_t loopType;			// chunk loop type
	uint32_t loopStart;			// chunk loop start
	uint32_t loopLength;		// chunk length
} DLSLoop;

typedef struct
{
	static const uint32_t OPTIONS_NOTRUNCATION = 0x00000001;
	static const uint32_t OPTIONS_NOCOMPRESSION = 0x00000002;

	uint32_t    size;
	uint16_t    unityNote;
	int16_t     fineTune;
	int32_t     gain;
	uint32_t    options;
	uint32_t    loopCount;
} RIFFDLSSample;

typedef struct
{
	static const uint32_t LOOP_TYPE_FORWARD = 0x00000000;
	static const uint32_t LOOP_TYPE_ALTERNATING = 0x00000001;
	static const uint32_t LOOP_TYPE_BACKWARD = 0x00000002;

	uint32_t cuePointId;
	uint32_t type;
	uint32_t start;
	uint32_t end;
	uint32_t fraction;
	uint32_t playCount;
} MIDILoop;

typedef struct
{
	uint32_t        manufacturerId;
	uint32_t        productId;
	uint32_t        samplePeriod;
	uint32_t        unityNode;
	uint32_t        pitchFraction;
	uint32_t        SMPTEFormat;
	uint32_t        SMPTEOffset;
	uint32_t        loopCount;
	uint32_t        samplerData;
} RIFFMIDISample;

static_assert(sizeof(RIFFChunk) == 8, "structure size mismatch");
static_assert(sizeof(RIFFChunkHeader) == 12, "structure size mismatch");
static_assert(sizeof(DLSLoop) == 16, "structure size mismatch");
static_assert(sizeof(RIFFDLSSample) == 20, "structure size mismatch");
static_assert(sizeof(MIDILoop) == 24, "structure size mismatch");
static_assert(sizeof(RIFFMIDISample) == 36, "structure size mismatch");

const uint32_t FOURCC_RIFF_TAG = MAKEFOURCC('R', 'I', 'F', 'F');
const uint32_t FOURCC_FORMAT_TAG = MAKEFOURCC('f', 'm', 't', ' ');
const uint32_t FOURCC_DATA_TAG = MAKEFOURCC('d', 'a', 't', 'a');
const uint32_t FOURCC_WAVE_FILE_TAG = MAKEFOURCC('W', 'A', 'V', 'E');
const uint32_t FOURCC_XWMA_FILE_TAG = MAKEFOURCC('X', 'W', 'M', 'A');
const uint32_t FOURCC_DLS_SAMPLE = MAKEFOURCC('w', 's', 'm', 'p');
const uint32_t FOURCC_MIDI_SAMPLE = MAKEFOURCC('s', 'm', 'p', 'l');
const uint32_t FOURCC_XWMA_DPDS = MAKEFOURCC('d', 'p', 'd', 's');
const uint32_t FOURCC_XMA_SEEK = MAKEFOURCC('s', 'e', 'e', 'k');

namespace IGAudio
{
	class FileSystem
	{
	public:
		IGAPI IGCODE LoadToAsyncBuffer(_In_ LPCSTR lpPath, _Out_ BYTE** lpBuffer);
		IGAPI IGCODE LoadToSimpleBuffer(_In_ LPCSTR lpPath, _In_ DWORD dwSize, _Out_ BYTE** lpBuffer, _Out_ LPWAVEFORMATEX waveFormat);
	};
	class XAudio
	{
	public:
		IGAPI IGCODE GetCurrentDevice(_Out_ LPDWORD dwDeviceID, _Out_ CONST CHAR** lpDeviceName);
		IGAPI IGCODE PlayAudioFile(_In_ BYTE* lpBuffer, _In_ DWORD dwSize);

	private:
		FileSystem audioFileSystem;
		IXAudio2* lpXAudio;
		IXAudio2SourceVoice* lpXAudioSourceVoice;
		IXAudio2MasteringVoice* lpXAudioMasterVoice;
		XAUDIO2_VOICE_STATE voiceState;
		XAUDIO2_VOICE_SENDS voiceSends;
	};

}
