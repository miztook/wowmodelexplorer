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
		char strName[DEFAULT_SIZE];
		uint32_t subIdx;
		int32_t loopTime;
	};

	struct SAnimEntry 
	{
		uint16_t entryIndex;
		int16_t animIndex;
		int32_t loopTime;
	};

public:
	wow_m2Action(IFileM2* filem2, const char* name, E_PLAY_TYPE playType, int32_t maxPlayTime = -1);
	~wow_m2Action();

public:
	uint32_t getAnimationCount() const { return (uint32_t)AnimationList.size(); }
	
	bool pushAnimationEntry(const char* animName, uint32_t subIdx, int32_t loopTime);

	void clear();

	const char* getActionName() const { return Name; }

	E_PLAY_TYPE getPlayType() const { return PlayType; }

	bool isInfinite() const { return MaxPlayTime == -1; }

	std::vector<SAnimEntry>		AnimationList;

	//random
	uint32_t TotalLoopTime;
	int32_t	MaxPlayTime;

private:
	char		Name[DEFAULT_SIZE];
	CFileM2* FileM2;

	E_PLAY_TYPE		PlayType;
};
