/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* IGUH - "Internal graphic used here" engine.
* MIT-License
**********************************************************
* Module Name: IGUH Audio file load
**********************************************************
* IG_LoadFile.cpp
* Filesystem methods
*********************************************************/

#include "IG_Audio.h"
IGKernel::FileSystem fileSystem;

const RIFFChunk* FindSoundChunk(
	_In_reads_bytes_(sizeBytes) const uint8_t* data,
	_In_ size_t sizeBytes,
	_In_ UINT tag
)
{
	if (!data)
		return NULL;

	const uint8_t* ptr = data;
	const uint8_t* end = data + sizeBytes;

	while (end > (ptr + sizeof(RIFFChunk)))
	{
		const RIFFChunk* header = reinterpret_cast<const RIFFChunk*>(ptr);
		if (header->tag == tag)
			return header;

		ptrdiff_t offset = header->size + sizeof(RIFFChunk);
		ptr += offset;
	}

	return NULL;
}

//#TODO:
IGCODE 
IGAudio::FileSystem::LoadToAsyncBuffer(
	_In_ LPCSTR lpPath,
	_Out_ BYTE** lpBuffer
)
{
	std::unique_ptr<uint8_t[]> buffers[MAX_BUFFER_COUNT];
	DWORD currentDiskReadBuffer = NULL;
	DWORD currentPosition = NULL;

	// load file to temp buffer
	while (TRUE)
	{
		R_CFAILED(fileSystem.LoadFileToMapBuffer(lpPath, STREAMING_BUFFER_SIZE, NULL, NULL, lpBuffer));
	}
	return IG_SUCCESS;
}

IGCODE
IGAudio::FileSystem::LoadToSimpleBuffer(
	_In_ LPCSTR lpPath,
	_In_ DWORD dwSize,
	_Out_ BYTE** lpBuffer,
	_Out_ LPWAVEFORMATEX waveFormat
)
{
	if (!(*lpBuffer) && !(dwSize) && lpPath)
	{
		R_CFAILED(fileSystem.LoadFileToBuffer(lpPath, FILE_INFO_FLAG, lpBuffer, &dwSize));
	}
	const RIFFChunk* riffChunk = FindSoundChunk(*lpBuffer, dwSize, FOURCC_RIFF_TAG);

	// if chunk is empty or size smaller than 4 - take message
	if (!(!riffChunk || riffChunk->size < 4))
	{
		// get RIFF chunk header info
		const uint8_t* wavEnd = *lpBuffer + dwSize;
		const RIFFChunkHeader* riffHeader = reinterpret_cast<const RIFFChunkHeader*>(riffChunk);

		// if this file isn't RIFF - take message
		if (riffHeader->riff != FOURCC_WAVE_FILE_TAG && riffHeader->riff != FOURCC_XWMA_FILE_TAG)
		{
			return IGFS_BAD_FILE;
		}

		// locate 'fmt ' at file
		const uint8_t* ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);

		if ((ptr + sizeof(RIFFChunk)) > wavEnd)
		{
			return IGFS_BAD_FILE;
		}

		// find fmt chunk
		const RIFFChunk* fmtChunk = FindSoundChunk(ptr, riffHeader->size, FOURCC_FORMAT_TAG);

		// if chunk is empty or size smaller than size of PCMWAVEFORMAT - take message
		if (!fmtChunk || fmtChunk->size < sizeof(PCMWAVEFORMAT))
		{
			return IGFS_BAD_FILE;
		}

		// reinterpretate fmt chunk to pointer
		ptr = reinterpret_cast<const uint8_t*>(fmtChunk) + sizeof(RIFFChunk);

		if (ptr + fmtChunk->size > wavEnd)
		{
			return IGFS_BAD_FILE;
		}

		const WAVEFORMAT* wf = reinterpret_cast<const WAVEFORMAT*>(ptr);

		// check formatTag
		switch (wf->wFormatTag)
		{
		case WAVE_FORMAT_PCM:
		case WAVE_FORMAT_IEEE_FLOAT:
			break;
		default:
		{
			if (fmtChunk->size < sizeof(WAVEFORMATEX))
			{
				return IGFS_BAD_FILE;
			}
			const WAVEFORMATEX* wfx = reinterpret_cast<const WAVEFORMATEX*>(ptr);

			if (fmtChunk->size < (sizeof(WAVEFORMATEX) + wfx->cbSize))
			{
				return IGFS_BAD_FILE;
			}
			switch (wfx->wFormatTag)
			{
			case WAVE_FORMAT_WMAUDIO2:
			case WAVE_FORMAT_WMAUDIO3:
				break;
			case WAVE_FORMAT_ADPCM:
				if ((fmtChunk->size < (sizeof(WAVEFORMATEX) + 32)) || (wfx->cbSize < 32))
				{
					return IGFS_BAD_FILE;
				}
				break;

			case WAVE_FORMAT_EXTENSIBLE:
				if ((fmtChunk->size < sizeof(WAVEFORMATEXTENSIBLE)) ||
					(wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))))
				{
					return IGFS_BAD_FILE;
				}
				else
				{
					static const GUID s_wfexBase =
					{ 0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };

					const WAVEFORMATEXTENSIBLE* wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(ptr);

					R_ASSERT(!memcmp(
						reinterpret_cast<const BYTE*>(&wfex->SubFormat) +
						sizeof(DWORD),
						reinterpret_cast<const BYTE*>(&s_wfexBase) + sizeof(DWORD),
						sizeof(GUID) - sizeof(DWORD)
					));

					switch (wfex->SubFormat.Data1)
					{
					case WAVE_FORMAT_PCM:
					case WAVE_FORMAT_IEEE_FLOAT:
					case WAVE_FORMAT_WMAUDIO2:
					case WAVE_FORMAT_WMAUDIO3:
						break;

					default:
						return IGFS_BAD_FILE;
					}
				}
				break;

			default:
				return IGFS_BAD_FILE;
			}
		}
		}

		// reinterpretate RIFF header to pointer
		ptr = reinterpret_cast<const uint8_t*>(riffHeader) + sizeof(RIFFChunkHeader);
		R_ASSERT(!((ptr + sizeof(RIFFChunk)) > wavEnd));

		// find 'data' chunk
		const RIFFChunk* dataChunk = FindSoundChunk(ptr, riffChunk->size, FOURCC_DATA_TAG);
		R_ASSERT(!(!dataChunk || !dataChunk->size));

		// reinterpretate 'data' header to pointer
		ptr = reinterpret_cast<const uint8_t*>(dataChunk) + sizeof(RIFFChunk);
		R_ASSERT(!(ptr + dataChunk->size > wavEnd));

		const WAVEFORMATEX* wfexA = reinterpret_cast<const WAVEFORMATEX*>(wf);

		WAVEFORMATEX formatFile = { NULL };
		formatFile.cbSize = sizeof(WAVEFORMATEX);
		formatFile.nAvgBytesPerSec = wfexA->nAvgBytesPerSec;
		formatFile.nBlockAlign = wfexA->nBlockAlign;
		formatFile.nChannels = wfexA->nChannels;
		formatFile.nSamplesPerSec = wfexA->nSamplesPerSec;
		formatFile.wBitsPerSample = wfexA->wBitsPerSample;
		formatFile.wFormatTag = wfexA->wFormatTag;
		*waveFormat = formatFile;
	}
	else
	{
		return IGFS_BAD_FILE;
	}
	return IG_SUCCESS;
}
