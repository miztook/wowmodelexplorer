#pragma once

//一个动作的动画，起始结束帧，循环，播放速度
class animation	
{
public:
	 animation()
		: StartFrame(0), EndFrame(0), MaxFrameCount(0), FramesPerMs(1.0f), 
		CurrentFrameNumber(0.0f), Looping(false) {}
public:
	void setMaxFrame(s32 maxFrame) { MaxFrameCount = abs_(maxFrame); }
	void setCurrentFrame(f32 frame);
	void setFrameLoop(s32 begin, s32 end);
	void setAnimationSpeed(f32 framePerMs) { FramesPerMs = framePerMs; }
	f32 getAnimationSpeed() const { return FramesPerMs; }

	f32 getCurrentFrame() const { return CurrentFrameNumber; }
	s32 getStartFrame() const { return StartFrame; }
	s32 getEndFrame() const { return EndFrame; }
	s32 getFrameLength() const { return abs_(EndFrame - StartFrame); }
	void setLoopMode(bool on) { Looping = on; }
	bool isLoop() const { return Looping; }
	//
	f32 buildFrameNumber( u32 timeMs, bool* animEnd = NULL_PTR );			//返回frame delta

	bool isFrameStart() const;
	bool isFrameEnd() const;

private:
	f32	FramesPerMs;
	f32	CurrentFrameNumber;
	s32	StartFrame;
	s32	EndFrame;
	s32  MaxFrameCount;		
	bool	Looping;
};

 inline void animation::setCurrentFrame( f32 frame )
{
	CurrentFrameNumber = clamp_<f32>( frame, (f32)StartFrame, (f32)EndFrame );
}

inline  void animation::setFrameLoop( s32 begin, s32 end )
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

 inline f32 animation::buildFrameNumber( u32 timeMs, bool* animEnd )
{
	f32 last = CurrentFrameNumber;
	bool end = false;

	if (StartFrame==EndFrame)
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
				end = true;
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (f32)StartFrame)	
			{
				CurrentFrameNumber += (EndFrame-StartFrame);
				end = true;
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
				end = true;
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (f32)StartFrame)
			{
				CurrentFrameNumber = (f32)StartFrame;	
				end = true;
			}
		}
	}

	if (animEnd)
		*animEnd = end;

	return CurrentFrameNumber - last;
}

 inline bool animation::isFrameStart() const
{
	if (FramesPerMs > 0.f) //forwards...
	{
		return CurrentFrameNumber == StartFrame;
	}
	else
	{
		return CurrentFrameNumber == EndFrame;
	}
}

 inline bool animation::isFrameEnd() const
{
	if (FramesPerMs > 0.f) //forwards...
	{
		return CurrentFrameNumber == EndFrame;
	}
	else
	{
		return CurrentFrameNumber == StartFrame;
	}
}

