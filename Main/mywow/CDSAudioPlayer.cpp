#include "stdafx.h"
#include "CDSAudioPlayer.h"
#include "mywow.h"
#include "CDSSound.h"

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_AUDIO

volatile bool CDSAudioPlayer::g_StopThread = false;

CDSAudioPlayer::CDSAudioPlayer( ) : AudioAvailable(false)
{
	HLib = NULL_PTR;
	pIDSound = NULL_PTR;
	::memset(WavSounds, 0, sizeof(CDSSound*)*NUM_WAV_SOUNDS);
	::memset(OggSounds, 0, sizeof(CDSSound*)*NUM_OGG_SOUNDS);
	::memset(Mp3Sounds, 0, sizeof(CDSSound*)*NUM_MP3_SOUNDS);

	Setting.global = true;
	Setting.hardware = false;

	INIT_THREAD(&NotifyThread, NotificationProc, this, true);
}

CDSAudioPlayer::~CDSAudioPlayer()
{
	CDSAudioPlayer::g_StopThread = true;
	if (AudioAvailable)
		WAIT_THREAD(&NotifyThread);

	DESTROY_THREAD(&NotifyThread);

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i) delete WavSounds[i];
	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i) delete OggSounds[i];
	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i) delete Mp3Sounds[i];

	if (pIDSound)
		pIDSound->Release();

	if (HLib)
		FreeLibrary(HLib);
}

typedef HRESULT (_stdcall *DSCREATE)(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);

bool CDSAudioPlayer::initDevice( HWND hwnd, u32 primaryChannels, u32 primarySampleRate, u32 primayBitsPerSample)
{
	HLib = ::LoadLibraryA("dsound.dll");
	if (!HLib)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "Cannot find dsound.dll !");
		ASSERT(false);
		return false;
	}

	DSCREATE dsCreate = (DSCREATE)::GetProcAddress(HLib, "DirectSoundCreate");
	if (!dsCreate || FAILED(dsCreate(NULL_PTR, &pIDSound, NULL_PTR)))
	{
		SAFE_RELEASE(pIDSound);
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "DSAudioPlayer DirectSoundCreate Failed!");
		ASSERT(false);
		return false;
	}

	if (FAILED(pIDSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)))
	{
		SAFE_RELEASE(pIDSound);
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "DSAudioPlayer SetCooperativeLevel Failed!");
		ASSERT(false);
		return false;
	}

	//set primary buffer format
	bool ret = setPrimaryBufferFormat(primaryChannels, primarySampleRate, primayBitsPerSample);

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
		WavSounds[i] = new CDSSound(pIDSound, EST_WAV, i);

	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
		OggSounds[i] = new CDSSound(pIDSound, EST_OGG, i);

	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i) 
		Mp3Sounds[i] = new CDSSound(pIDSound, EST_MP3, i);

	if (ret)
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "DSAudioPlayer successfully created!");
	else
		g_Engine->getFileSystem()->writeLog(ELOG_SOUND, "DSAudioPlayer setPrimaryBufferFormat Failed!");

	AudioAvailable = ret;
		
	RESUME_THREAD(&NotifyThread);

	return ret;
}

bool CDSAudioPlayer::setPrimaryBufferFormat( u32 primaryChannels, u32 primarySampleRate, u32 primayBitsPerSample )
{
	LPDIRECTSOUNDBUFFER	pPrimaryBuffer = NULL_PTR;

	DSBUFFERDESC desc = {0};
	desc.dwSize = sizeof(DSBUFFERDESC);
	desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	desc.dwBufferBytes = 0;
	desc.lpwfxFormat = NULL_PTR;

	if (FAILED(pIDSound->CreateSoundBuffer(&desc, &pPrimaryBuffer, NULL_PTR)))
	{
		SAFE_RELEASE(pPrimaryBuffer);

		ASSERT(false);
		return false;
	}

	WAVEFORMATEX wfx = {0};
	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (WORD)primaryChannels;
	wfx.nSamplesPerSec = (DWORD)primarySampleRate;
	wfx.wBitsPerSample = (WORD)primayBitsPerSample;
	wfx.nBlockAlign = (WORD)wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if (FAILED(pPrimaryBuffer->SetFormat(&wfx)))
	{
		SAFE_RELEASE(pPrimaryBuffer);

		ASSERT(false);
		return false;
	}

	pPrimaryBuffer->Release();

	return true;
}

int CDSAudioPlayer::NotificationProc( void* lpParameter )
{
	CDSAudioPlayer* player = static_cast<CDSAudioPlayer*>(lpParameter);
	if (!player->AudioAvailable)
		return 0;

	while(!CDSAudioPlayer::g_StopThread)
	{
		for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
		{
			CDSSound* sound = player->WavSounds[i];
			if (sound->pIDSBuffer)
			{
				if( WAIT_EVENT(&sound->NotificationEvent, 10))
					sound->handleStreamNotification();
			}
		}
		for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
		{
			CDSSound* sound = player->OggSounds[i];
			if (sound->pIDSBuffer)
			{
				if( WAIT_EVENT(&sound->NotificationEvent, 10))
					sound->handleStreamNotification();
			}
		}
		for (u32 i=0; i<NUM_MP3_SOUNDS; ++i)
		{
			CDSSound* sound = player->Mp3Sounds[i];
			if (sound->pIDSBuffer)
			{
				if( WAIT_EVENT(&sound->NotificationEvent, 10))
					sound->handleStreamNotification();
			}
		}

		SLEEP(1);
	}

	for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
	{
		CDSSound* sound = player->WavSounds[i];
		sound->stop();
	}

	for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
	{
		CDSSound* sound = player->OggSounds[i];
		sound->stop();
	}

	for (u32 i=0; i<NUM_MP3_SOUNDS; ++i)
	{
		CDSSound* sound = player->Mp3Sounds[i];
		sound->stop();
	}

	return 0;
}

ISound* CDSAudioPlayer::getEmptyWavSound() const
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

ISound* CDSAudioPlayer::getEmptyOggSound() const
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

ISound* CDSAudioPlayer::getEmptyMp3Sound() const
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

void CDSAudioPlayer::setAudioSetting( const SAudioSetting& setting )
{
	if (setting != Setting)
	{
		Setting = setting;

		if (AudioAvailable)
		{
			for (u32 i=0; i<NUM_WAV_SOUNDS; ++i)
			{
				CDSSound* sound = WavSounds[i];
				if (sound->pIDSBuffer)
					sound->recreateDSBuffer(Setting);
			}
			for (u32 i=0; i<NUM_OGG_SOUNDS; ++i)
			{
				CDSSound* sound = OggSounds[i];
				if (sound->pIDSBuffer)
					sound->recreateDSBuffer(Setting);
			}
			for (u32 i=0; i<NUM_MP3_SOUNDS; ++i)
			{
				CDSSound* sound = Mp3Sounds[i];
				if (sound->pIDSBuffer)
					sound->recreateDSBuffer(Setting);
			}
		}
	}
}

void CDSAudioPlayer::fadeoutSound( ISound* sound, u32 fadetime )
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

void CDSAudioPlayer::tickFadeOutSounds( u32 timeSinceLastFrame )
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

ISound* CDSAudioPlayer::getWavSound( u32 index ) const
{
	if (index >= NUM_WAV_SOUNDS)
		return NULL_PTR;
	return WavSounds[index];
}

ISound* CDSAudioPlayer::getOggSound( u32 index ) const
{
	if (index >= NUM_OGG_SOUNDS)
		return NULL_PTR;
	return OggSounds[index];
}

ISound* CDSAudioPlayer::getMp3Sound( u32 index ) const
{
	if (index >= NUM_MP3_SOUNDS)
		return NULL_PTR;
	return Mp3Sounds[index];
}

#endif

#endif