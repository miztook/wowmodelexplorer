#pragma once

#include "base.h"

class ISound;

#define		NUM_WAV_SOUNDS	8
#define		NUM_OGG_SOUNDS	8
#define		NUM_MP3_SOUNDS	2

struct SAudioSetting
{
	bool global;
	bool hardware;

	bool operator!=(const SAudioSetting& b) const
	{
		return global != b.global ||
			hardware != b.hardware;
	}

	bool operator==(const SAudioSetting& b) const
	{ return !(b!=*this); }
};

class IAudioPlayer
{
public:
	virtual ~IAudioPlayer() {}

public:
	virtual ISound* getEmptyWavSound() const = 0;
	virtual ISound* getEmptyOggSound() const = 0;
	virtual ISound* getEmptyMp3Sound() const = 0;

	virtual ISound* getWavSound(u32 index) const = 0;
	virtual ISound* getOggSound(u32 index) const = 0;
	virtual ISound* getMp3Sound(u32 index) const = 0;

	virtual void setAudioSetting(const SAudioSetting& setting) = 0;
	virtual const SAudioSetting& getAudioSetting() const = 0;

	virtual void fadeoutSound(ISound* sound, u32 fadetime) = 0;

	virtual void tickFadeOutSounds(u32 timeSinceLastFrame) = 0;

};