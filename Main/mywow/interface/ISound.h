#pragma once

#include "base.h"

enum E_SOUND_TYPE
{
	EST_WAV = 0,
	EST_OGG,
	EST_MP3
};

typedef void (*FN_SOUND_CALLBACK)();

class ISound
{
public:
	explicit ISound(E_SOUND_TYPE type) : Type(type) {}
	virtual ~ISound() {}

public:
	virtual bool load(const c8* filename) = 0;
	virtual void unload() = 0;
	virtual bool play(bool loop, FN_SOUND_CALLBACK callback = NULL_PTR) = 0;
	virtual void stop() = 0;
	virtual void reset() = 0;
	virtual bool isPlaying() const = 0;
	virtual bool isStopped() const = 0;

	//volume范围从0 - 1.0
	virtual void setVolume(f32 vol) = 0;
	virtual f32 getVolume() const = 0;

	virtual u32 getIndex() const = 0;		//声音的索引,有同时播放的声音数限制

public:
	u32 Type;
};