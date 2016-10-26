#pragma once

#include "IAudioInput.h"

#ifdef MW_PLATFORM_WINDOWS

class IReadFile;
class IMemFile;

enum E_WAVE_TYPE
{
	EWT_UNSUPPORTED = 0,
	EWT_PCM,
};

struct SWaveHeader
{
	c8 RIFF[4];
	u32 FileSize;
	c8 RIFFType[4];
	c8 FmtChunkId[4];
	u32 FmtChunkSize;
	u16 FormatTag;
	u16 Channels;
	u32 SampleRate;
	u32 BytesPerSecond;
	u16 BlockAlign;
	u16 BitsPerSample;
	c8 DataChunkId[4];
	u32 DataSize;
};

class CWavInput : public IAudioInput
{
private:
	DISALLOW_COPY_AND_ASSIGN(CWavInput);

public:
	CWavInput();
	~CWavInput();

public:
	virtual bool reset();

	virtual bool openFile(const c8* filename);
	virtual void closeFile();

	virtual u32 fillBuffer(void* buffer, u32 size);

private:
	bool readRIFFHeader();

private:

#ifdef MPQ_AUDIOINPUT
	IMemFile*		File;
#else
	IReadFile*		File;
#endif

	E_WAVE_TYPE	Type;
	SWaveHeader		Header;
	u32		StreamStart;
};

#endif
