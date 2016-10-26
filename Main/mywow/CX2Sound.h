#pragma once

#include "ISound.h"

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_AUDIO

#include <XAudio2.h>

class IAudioInput;
struct SAudioSetting;

//表示一个音频文件的xaudio2缓冲管理部分，数据部分由CXXXInput完成 
class CX2Sound : public ISound
{
public:
	CX2Sound(IXAudio2* pXAudio2, E_SOUND_TYPE type, u32 index);
	~CX2Sound();

public:
	virtual bool load(const c8* filename);
	virtual void unload();
	virtual bool play(bool loop, FN_SOUND_CALLBACK callback = NULL_PTR);
	virtual void stop();
	virtual void reset();
	virtual bool isPlaying() const;
	virtual bool isStopped() const;
	virtual void setVolume(f32 vol);
	virtual f32 getVolume() const { return Volume; }
	virtual u32 getIndex() const { return Index; }

private:
	IXAudio2*		pXAudio2;
	IAudioInput*		AudioInput;

	IXAudio2SourceVoice*		SourceVoice;

	FN_SOUND_CALLBACK	Callback;

	mutable CRITICAL_SECTION		cs;

	u32		Index;

	f32		Volume;


	WAVEFORMATEX		WFX;

	friend class CX2AudioPlayer;
};

#endif

#endif