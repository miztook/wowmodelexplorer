#include "stdafx.h"
#include "CWavInput.h"
#include "mywow.h"

#ifdef MW_PLATFORM_WINDOWS

CWavInput::CWavInput()
{
	File = NULL_PTR;
	Type = EWT_UNSUPPORTED;
	::memset(&Header, 0, sizeof(Header));
	StreamStart = sizeof(SWaveHeader);
}

CWavInput::~CWavInput()
{
	closeFile();
}

bool CWavInput::reset()
{
	if (!File || Type == EWT_UNSUPPORTED)
		return false;

	File->seek(StreamStart, false);

	return true;
}

bool CWavInput::readRIFFHeader()
{
	File->read(&Header, sizeof(SWaveHeader));
	
	switch(Header.FormatTag)
	{
	case WAVE_FORMAT_PCM:
		Type = EWT_PCM;
		break;
	default:
		Type = EWT_UNSUPPORTED;
		break;
	}

	DataSize = Header.DataSize;
	BitsPerSample = Header.BitsPerSample;
	NumChannels = Header.Channels;
	SampleRate = Header.SampleRate;

	ASSERT(StreamStart + DataSize <= (u32)File->getSize());

	ASSERT(DataSize && BitsPerSample && NumChannels && SampleRate);

	return true;
}

bool CWavInput::openFile( const c8* filename )
{
	if (File)
		return true;

	c8 ext[10] = {0};
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "wav") != 0)
		return false;

#ifdef MPQ_AUDIOINPUT
	File = g_Engine->getWowEnvironment()->openFile(filename, false);
#else
	const c8* base = g_Engine->getFileSystem()->getDataDirectory();
	c8 tmp[QMAX_PATH];
	Q_strcpy(tmp, QMAX_PATH, base);
	Q_strcat(tmp, QMAX_PATH, filename);

	File = g_Engine->getFileSystem()->createAndOpenFile(tmp, true);
#endif
	
	if (!File || File->getSize() < sizeof(SWaveHeader))
		return false;

	readRIFFHeader();

	reset();

	return true;
}

void CWavInput::closeFile()
{
	if (File)
	{
		delete File;
		File = NULL_PTR;
	}
}

u32 CWavInput::fillBuffer( void* buffer, u32 size )
{
	if (!File || Type == EWT_UNSUPPORTED)
		return 0;

	size = size - (size % getSampleSize());

	u32 current = (u32)File->getPos();
	if (current >= StreamStart + DataSize)
		return 0;

	if (size > StreamStart + DataSize - current)		//不要读取非data部分
	{
		size = StreamStart + DataSize - current;
	}

	s32 ret = File->read(buffer, size);
	if (ret < 0)
		ret = 0;
	return (u32)ret;
}

#endif
