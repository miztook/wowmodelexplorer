#pragma once

#include "IAudioPlayer.h"
#include <list>
#include "CSysThread.h"

#ifdef MW_PLATFORM_WINDOWS

#if 0

#include <xaudio2.h>

class CX2Sound;

class CX2AudioPlayer : public IAudioPlayer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CX2AudioPlayer);

public:
	CX2AudioPlayer();
	~CX2AudioPlayer();

public:
	bool initDevice(u32 primaryChannels, u32 primarySampleRate);

	virtual ISound* getEmptyWavSound() const;
	virtual ISound* getEmptyOggSound() const;
	virtual ISound* getEmptyMp3Sound() const;

	virtual ISound* getWavSound(u32 index) const;
	virtual ISound* getOggSound(u32 index) const;
	virtual ISound* getMp3Sound(u32 index) const;

	virtual void setAudioSetting(const SAudioSetting& setting);
	virtual const SAudioSetting& getAudioSetting() const { return Setting; }

	virtual void fadeoutSound(ISound* sound, u32 fadetime);
	virtual void tickFadeOutSounds(u32 timeSinceLastFrame);

private:
	static int NotificationProc( void* lpParameter );

	volatile static bool g_StopThread;

private:
	HMODULE		HLib;
	IXAudio2*		XAudio2;
	IXAudio2MasteringVoice*		MasterVoice;

	CX2Sound*	WavSounds[NUM_WAV_SOUNDS];
	CX2Sound*	OggSounds[NUM_OGG_SOUNDS];
	CX2Sound*	Mp3Sounds[NUM_MP3_SOUNDS];

	thread_type	NotifyThread;

	SAudioSetting	Setting;

	bool	AudioAvailable;

	struct SEntry
	{
		ISound*	sound;
		u32 fadetime;
	};

	typedef std::list<SEntry, qzone_allocator<SEntry> >	T_FadeoutList;
	T_FadeoutList		FadeoutList;
};

#endif

#endif