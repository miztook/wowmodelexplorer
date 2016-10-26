#pragma once

#include "base.h"

#define MPQ_AUDIOINPUT	

//一个音频输入
class IAudioInput
{
public:
	IAudioInput()
	{
		DataSize = 0;
		BitsPerSample = 0;
		NumChannels = 0;
		SampleRate = 0;
	}
	virtual ~IAudioInput() {}

public:
	virtual bool reset() = 0;

	virtual bool openFile(const c8* filename) = 0;
	virtual void closeFile() = 0;
	virtual u32 fillBuffer(void* buffer, u32 size) = 0;			//填充应用缓存(src: 音频缓存数据)

	u32 getSampleSize() const;
	u32 getTotalSamples() const;
	u32 getTotalTime() const;

public:
	u32 DataSize;
	u32 BitsPerSample;
	u32	NumChannels;
	u32	SampleRate;
	
};

inline u32 IAudioInput::getSampleSize() const
{
	return  BitsPerSample / 8 * NumChannels;
}

inline u32 IAudioInput::getTotalSamples() const
{
	u32 sampleSize = getSampleSize();

	if (sampleSize)
		return DataSize / sampleSize;

	return 0;
}

inline u32 IAudioInput::getTotalTime() const
{
	if(SampleRate)
		return getTotalSamples() / SampleRate;
	
	return 0;
}
