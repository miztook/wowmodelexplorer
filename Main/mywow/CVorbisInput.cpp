#include "stdafx.h"
#include "CVorbisInput.h"
#include "mywow.h"

#ifdef MW_PLATFORM_WINDOWS

u32 vorbisRead(void *buffer, size_t size, size_t amount, void *file);
int vorbisSeek(void *file, ogg_int64_t newpos, int set);
int vorbisClose(void *file);
long vorbisTell(void *file);

CVorbisInput::CVorbisInput()
{
	File = NULL_PTR;
	memset(&OggFile, 0, sizeof(OggFile));
}

CVorbisInput::~CVorbisInput()
{
	closeFile();
}

bool CVorbisInput::reset()
{
	if (!File)
		return false;
	if( 0 != ov_pcm_seek(&OggFile, 0))
		return false;
	return true;
}

bool CVorbisInput::openFile( const c8* filename )
{
	if (File)
		return true;

	c8 ext[10] = {0};
	getFileExtensionA(filename, ext, 10);
	if (Q_stricmp(ext, "ogg") != 0)
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

	if (File)
		DataSize = File->getSize();
	else
	{
		DataSize = 0;
		return false;
	}

	ov_callbacks callbacks = {
		(size_t (*)(void *, size_t, size_t, void *)) vorbisRead,
		(int(*)(void *, ogg_int64_t, int)) vorbisSeek,
		(int(*)(void *)) vorbisClose,
		(long(*)(void *)) vorbisTell
	};

	if (ov_open_callbacks((void *)File, &OggFile, NULL_PTR, 0, callbacks) < 0)
	{
		closeFile();
		ASSERT(false);
		return false;
	}

	if (ov_streams(&OggFile) != 1)
	{
		closeFile();
		ASSERT(false);
		return false;
	}

	vorbis_info* pInfo;
	pInfo = ov_info(&OggFile, -1);
	if (pInfo == NULL_PTR || pInfo->channels > 2)
	{
		closeFile();
		ASSERT(false);
		return false;
	}

	reset();

	BitsPerSample = 16;
	NumChannels = pInfo->channels;
	SampleRate = pInfo->rate;

	return true;
}

void CVorbisInput::closeFile()
{
	if (File)
	{
		ov_clear(&OggFile);
		delete File;
		File = NULL_PTR;
	}
}

u32 CVorbisInput::fillBuffer( void* buffer, u32 size )
{
	if (!File)
		return 0;

	int current = 0;
	int ret = 0;
	u32 bytesRead = 0;
	while (bytesRead < (int)size)
	{
		ret = ov_read(&OggFile, (char*)buffer + bytesRead, size - bytesRead, 0, 2, 1, &current);
		if (ret <= 0)
			break;

		bytesRead += (u32)ret;
	}

	return bytesRead;
}

u32 vorbisRead( void *buffer, size_t size, size_t amount, void *file )
{
#ifdef MPQ_AUDIOINPUT
	IMemFile* f = (IMemFile*)file;
#else
	IReadFile* f = (IReadFile*)file;
#endif
	return f->read(buffer, (u32)size * (u32)amount);
}

int vorbisSeek( void *file, ogg_int64_t newpos, int set )
{
#ifdef MPQ_AUDIOINPUT
	IMemFile* f = (IMemFile*)file;
#else
	IReadFile* f = (IReadFile*)file;
#endif

	s32 pos = (s32)newpos;
	if (set == SEEK_END)
		pos = (s32)f->getSize() + pos;

	if(f->seek(pos, set == SEEK_CUR))
		return 0;
	return -1;
}

int vorbisClose( void *file )
{
	return 0;
}

long vorbisTell( void *file )
{
#ifdef MPQ_AUDIOINPUT
	IMemFile* f = (IMemFile*)file;
#else
	IReadFile* f = (IReadFile*)file;
#endif
	return f->getPos();
}

#endif
