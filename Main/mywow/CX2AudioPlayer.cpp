#include "stdafx.h"
#include "CX2AudioPlayer.h"
#include "mywow.h"
#include "CX2Sound.h"

#ifdef MW_PLATFORM_WINDOWS

#if 0

volatile bool CX2AudioPlayer::g_StopThread  = false;

CX2AudioPlayer::CX2AudioPlayer()
{
	HLib = NULL_PTR;
	XAudio2 = NULL_PTR;
	MasterVoice = NULL_PTR;

	 CoInitializeEx( NULL_PTR, COINIT_MULTITHREADED );

	::memset(WavSounds, 0, sizeof(CX2Sound*)*NUM_WAV_SOUNDS);
	::memset(OggSounds, 0, sizeof(CX2Sound*)*NUM_OGG_SOUNDS);
	::memset(Mp3Sounds, 0, sizeof(CX2Sound*)*NUM_MP3_SOUNDS);

	Setting.global = true;
	Setting.hardware = false;

	INIT_THREAD(&NotifyThread, NotificationProc, this, true);
}

CX2AudioPlayer::~CX2AudioPlayer()
{
	CX2AudioPlayer::g_StopThread = true;
	if (AudioAvailable)
		WAIT_THREAD(&NotifyThread);

	DESTROY_THREAD(&NotifyThread);

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i) delete WavSounds[i];
	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i) delete OggSounds[i];
	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i) delete Mp3Sounds[i];

	if (MasterVoice)
	{
		MasterVoice->DestroyVoice();
		MasterVoice = NULL_PTR;
	}

	SAFE_RELEASE( XAudio2 );
	CoUninitialize();

	if (HLib)
		FreeLibrary(HLib);

}

int CX2AudioPlayer::NotificationProc( void* lpParameter )
{
	CX2AudioPlayer* player = static_cast<CX2AudioPlayer*>(lpParameter);
	if (!player->AudioAvailable)
		return 0;

	while( !CX2AudioPlayer::g_StopThread )
	{
		SLEEP(1);
	}

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
	{
		CX2Sound* sound = player->WavSounds[i];
		sound->stop();
	}

	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
	{
		CX2Sound* sound = player->OggSounds[i];
		sound->stop();
	}

	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i)
	{
		CX2Sound* sound = player->Mp3Sounds[i];
		sound->stop();
	}

	return 0;
}

bool CX2AudioPlayer::initDevice(u32 primaryChannels, u32 primarySampleRate)
{
	HLib = NULL_PTR;
	c8 libName[DEFAULT_SIZE];
	for ( s32 version = 7; version >= 0; --version )
	{
		Q_sprintf(libName, DEFAULT_SIZE, "XAudio2_%d.dll", version);
		HLib = ::LoadLibraryA(libName);
		if (HLib)
			break;
	}

	if (!HLib)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "Cannot find xaudio2.dll !");
		ASSERT(false);
		return false;
	}

	if (FAILED(XAudio2Create(&XAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR )))
	{
		SAFE_RELEASE(XAudio2);
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "X2AudioPlayer XAudio2Create Failed!");
		ASSERT(false);
		return false;
	}

	bool ret = true;
	HRESULT hr;
	if ( FAILED(hr = XAudio2->CreateMasteringVoice( &MasterVoice, primaryChannels,
		primarySampleRate, 0, 0, NULL_PTR ) ) )
	{
		if ( FAILED(hr = XAudio2->CreateMasteringVoice( &MasterVoice, XAUDIO2_DEFAULT_CHANNELS,
			XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL_PTR ) ) )
		{
			MasterVoice = NULL_PTR;
			ASSERT(false);
			
			ret = false;
		}
	}

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
		WavSounds[i] = new CX2Sound(XAudio2, EST_WAV, i);

	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
		OggSounds[i] = new CX2Sound(XAudio2, EST_OGG, i);

	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i) 
		Mp3Sounds[i] = new CX2Sound(XAudio2, EST_MP3, i);

	if (ret)
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "X2AudioPlayer successfully created!");
	else
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "X2AudioPlayer CreateMasteringVoice Failed!");

	AudioAvailable = ret;

	RESUME_THREAD(&NotifyThread);

	return ret;
}

ISound* CX2AudioPlayer::getEmptyWavSound() const
{
	if (!AudioAvailable)
		return NULL_PTR;

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
	{
		if (!WavSounds[i]->isPlaying())
			return WavSounds[i];
	}
	return NULL_PTR;
}

ISound* CX2AudioPlayer::getEmptyOggSound() const
{
	if (!AudioAvailable)
		return NULL_PTR;

	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
	{
		if (!OggSounds[i]->isPlaying())
			return OggSounds[i];
	}
	return NULL_PTR;
}

ISound* CX2AudioPlayer::getEmptyMp3Sound() const
{
	if (!AudioAvailable)
		return NULL_PTR;

	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i)
	{
		if (!Mp3Sounds[i]->isPlaying())
			return Mp3Sounds[i];
	}
	return NULL_PTR;
}

ISound* CX2AudioPlayer::getWavSound( u32 index ) const
{
	if (index >= NUM_WAV_SOUNDS)
		return NULL_PTR;
	return WavSounds[index];
}

ISound* CX2AudioPlayer::getOggSound( u32 index ) const
{
	if (index >= NUM_OGG_SOUNDS)
		return NULL_PTR;
	return OggSounds[index];
}

ISound* CX2AudioPlayer::getMp3Sound( u32 index ) const
{
	if (index >= NUM_MP3_SOUNDS)
		return NULL_PTR;
	return Mp3Sounds[index];
}

void CX2AudioPlayer::setAudioSetting( const SAudioSetting& setting )
{

}

void CX2AudioPlayer::fadeoutSound( ISound* sound, u32 fadetime )
{
	if (!AudioAvailable || !sound->isPlaying())
		return;

	for (T_FadeoutList::const_iterator itr = FadeoutList.begin(); itr != FadeoutList.end(); ++itr)
	{
		if (itr->sound == sound)
			return;
	}

	SEntry entry;
	entry.sound = sound;
	entry.fadetime = fadetime;
	FadeoutList.emplace_back(entry);
}

void CX2AudioPlayer::tickFadeOutSounds( u32 timeSinceLastFrame )
{
	if (!AudioAvailable)
		return;

	for (T_FadeoutList::iterator itr = FadeoutList.begin(); itr != FadeoutList.end();)
	{
		if(itr->fadetime > timeSinceLastFrame && itr->sound->isPlaying())
		{
			f32 time = (f32)itr->fadetime;
			itr->fadetime -= timeSinceLastFrame;
			f32 vol = itr->sound->getVolume() * (itr->fadetime / time);
			itr->sound->setVolume(vol);
			++itr;
		}
		else
		{
			itr->sound->stop();
			itr = FadeoutList.erase(itr);
		}
	}
}

#endif

#endif