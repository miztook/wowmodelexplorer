#pragma once

#include "core.h"
#include <list>

class IFileM2;
class CFileM2;

//一个由基本animation组成的列表
class wow_m2Action
{
public:
	enum E_PLAY_TYPE
	{
		EPT_RANDOM = 0,
		EPT_SEQUENCE,
		EPT_CUSTOM,
	};

	struct SAnimationEntry
	{
		c8 strName[DEFAULT_SIZE];
		u32 subIdx;
		s32 loopTime;
	};

	struct SAnimEntry 
	{
		u16 entryIndex;
		s16 animIndex;
		s32 loopTime;
	};

public:
	wow_m2Action(IFileM2* filem2, const c8* name, E_PLAY_TYPE playType, s32 maxPlayTime = -1);
	~wow_m2Action();

public:
	u32 getAnimationCount() const { return (u32)AnimationList.size(); }
	
	bool pushAnimationEntry(const c8* animName, u32 subIdx, s32 loopTime);

	void clear();

	const c8* getActionName() const { return Name; }

	E_PLAY_TYPE getPlayType() const { return PlayType; }

	bool isInfinite() const { return MaxPlayTime == -1; }

	std::vector<SAnimEntry>		AnimationList;

	//random
	u32 TotalLoopTime;
	s32	MaxPlayTime;

private:
	c8		Name[DEFAULT_SIZE];
	CFileM2* FileM2;

	E_PLAY_TYPE		PlayType;
};
