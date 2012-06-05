#pragma once

//一个动作的动画，起始结束帧，循环，播放速度
class animation	
{
public:
	animation()
		: MaxFrameCount(0), StartFrame(0), EndFrame(0), FramesPerMs(1.0f), 
		CurrentFrameNumber(0.0f), Looping(false) {}
public:
	void setMaxFrame(s32 maxFrame) { MaxFrameCount = abs(maxFrame); }
	void setCurrentFrame(f32 frame);
	void setFrameLoop(s32 begin, s32 end);
	void setAnimationSpeed(f32 framePerMs) { FramesPerMs = framePerMs; }
	f32 getAnimationSpeed() const { return FramesPerMs; }

	f32 getCurrentFrame() const { return CurrentFrameNumber; }
	s32 getStartFrame() const { return StartFrame; }
	s32 getEndFrame() const { return EndFrame; }
	void setLoopMode(bool on) { Looping = on; }
	bool isLoop() const { return Looping; }
	//
	f32 buildFrameNumber( u32 timeMs );			//返回frame delta

private:
	s32	StartFrame;
	s32	EndFrame;
	f32	FramesPerMs;
	f32	CurrentFrameNumber;
	bool	Looping;

	s32  MaxFrameCount;
};

inline void animation::setCurrentFrame( f32 frame )
{
	CurrentFrameNumber = clamp_<f32>( frame, (f32)StartFrame, (f32)EndFrame );
}

inline void animation::setFrameLoop( s32 begin, s32 end )
{
	if (end < begin)
	{
		StartFrame = clamp_<s32>(end, 0, MaxFrameCount);
		EndFrame = clamp_<s32>(begin, StartFrame, MaxFrameCount);
	}
	else
	{
		StartFrame = clamp_<s32>(begin, 0, MaxFrameCount);
		EndFrame = clamp_<s32>(end, StartFrame, MaxFrameCount);
	}
	if (FramesPerMs < 0)
		setCurrentFrame((f32)EndFrame);
	else
		setCurrentFrame((f32)StartFrame);
}

inline f32 animation::buildFrameNumber( u32 timeMs )
{
	f32 last = CurrentFrameNumber;

	if ((StartFrame==EndFrame))
	{
		CurrentFrameNumber = (f32)StartFrame; //Support for non animated meshes
	}
	else if (Looping)
	{
		// play animation looped
		CurrentFrameNumber += timeMs * FramesPerMs;
		if (FramesPerMs > 0.f) //forwards...
		{
			if (CurrentFrameNumber > (f32)EndFrame)
			{
				CurrentFrameNumber -= (EndFrame-StartFrame);	
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (f32)StartFrame)	
			{
				CurrentFrameNumber += (EndFrame-StartFrame);
			}
		}
	}
	else
	{
		// play animation non looped
		CurrentFrameNumber += timeMs * FramesPerMs;
		if (FramesPerMs > 0.f) //forwards...
		{
			if (CurrentFrameNumber > (f32)EndFrame)
			{
				CurrentFrameNumber = (f32)EndFrame;
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (f32)StartFrame)
			{
				CurrentFrameNumber = (f32)StartFrame;	
			}
		}
	}

	return CurrentFrameNumber - last;
}

