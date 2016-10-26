#include "stdafx.h"
#include "CX2Sound.h"
#include "mywow.h"
#include "CWavInput.h"
#include "CVorbisInput.h"
#include "CMP3Input.h"

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_AUDIO

CX2Sound::CX2Sound( IXAudio2* pXA, E_SOUND_TYPE type, u32 index ) : pXAudio2(pXA), ISound(type), Callback(NULL_PTR)
{
	INIT_LOCK(&cs);

	switch (type)
	{
	case EST_WAV:
		AudioInput = new CWavInput;
		break;
	case EST_OGG:
		AudioInput = new CVorbisInput;
		break;
	case EST_MP3:
		AudioInput = new CMP3Input;
		break;
	default:
		ASSERT(false);
		AudioInput = NULL_PTR;
		break;
	}

}

CX2Sound::~CX2Sound()
{
	unload();
	delete AudioInput;

	DESTROY_LOCK(&cs);
}

bool CX2Sound::load( const c8* filename )
{
	return false;
}

void CX2Sound::unload()
{

}

bool CX2Sound::play( bool loop, FN_SOUND_CALLBACK callback /*= NULL_PTR*/ )
{
	return false;
}

void CX2Sound::stop()
{

}

void CX2Sound::reset()
{

}

bool CX2Sound::isPlaying() const
{
	return false;
}

bool CX2Sound::isStopped() const
{
	return true;
}

void CX2Sound::setVolume( f32 vol )
{

}

#endif

#endif