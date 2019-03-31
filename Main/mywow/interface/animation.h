#pragma once

//一个动作的动画，起始结束帧，循环，播放速度
class animation	
{
public:
	 animation()
		: StartFrame(0), EndFrame(0), MaxFrameCount(0), FramesPerMs(1.0f), 
		CurrentFrameNumber(0.0f), Looping(false) {}
public:
	void setMaxFrame(int32_t maxFrame) { MaxFrameCount = abs_(maxFrame); }
	void setCurrentFrame(float frame);
	void setFrameLoop(int32_t begin, int32_t end);
	void setAnimationSpeed(float framePerMs) { FramesPerMs = framePerMs; }
	float getAnimationSpeed() const { return FramesPerMs; }

	float getCurrentFrame() const { return CurrentFrameNumber; }
	int32_t getStartFrame() const { return StartFrame; }
	int32_t getEndFrame() const { return EndFrame; }
	int32_t getFrameLength() const { return abs_(EndFrame - StartFrame); }
	void setLoopMode(bool on) { Looping = on; }
	bool isLoop() const { return Looping; }
	//
	float buildFrameNumber( uint32_t timeMs, bool* animEnd = nullptr );			//返回frame delta

	bool isFrameStart() const;
	bool isFrameEnd() const;

private:
	float	FramesPerMs;
	float	CurrentFrameNumber;
	int32_t	StartFrame;
	int32_t	EndFrame;
	int32_t  MaxFrameCount;		
	bool	Looping;
};

 inline void animation::setCurrentFrame( float frame )
{
	CurrentFrameNumber = clamp_<float>( frame, (float)StartFrame, (float)EndFrame );
}

inline  void animation::setFrameLoop( int32_t begin, int32_t end )
{
	if (end < begin)
	{
		StartFrame = clamp_<int32_t>(end, 0, MaxFrameCount);
		EndFrame = clamp_<int32_t>(begin, StartFrame, MaxFrameCount);
	}
	else
	{
		StartFrame = clamp_<int32_t>(begin, 0, MaxFrameCount);
		EndFrame = clamp_<int32_t>(end, StartFrame, MaxFrameCount);
	}
	if (FramesPerMs < 0)
		setCurrentFrame((float)EndFrame);
	else
		setCurrentFrame((float)StartFrame);
}

 inline float animation::buildFrameNumber( uint32_t timeMs, bool* animEnd )
{
	float last = CurrentFrameNumber;
	bool end = false;

	if (StartFrame==EndFrame)
	{
		CurrentFrameNumber = (float)StartFrame; //Support for non animated meshes
	}
	else if (Looping)
	{
		// play animation looped
		CurrentFrameNumber += timeMs * FramesPerMs;
		if (FramesPerMs > 0.f) //forwards...
		{
			if (CurrentFrameNumber > (float)EndFrame)
			{
				CurrentFrameNumber -= (EndFrame-StartFrame);	
				end = true;
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (float)StartFrame)	
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
			if (CurrentFrameNumber > (float)EndFrame)
			{
				CurrentFrameNumber = (float)EndFrame;
				end = true;
			}
		}
		else //backwards...
		{
			if (CurrentFrameNumber < (float)StartFrame)
			{
				CurrentFrameNumber = (float)StartFrame;	
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

