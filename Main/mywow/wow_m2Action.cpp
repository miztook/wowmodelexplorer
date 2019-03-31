#include "stdafx.h"
#include "wow_m2Action.h"
#include "mywow.h"
#include "CFileM2.h"

wow_m2Action::wow_m2Action(IFileM2* filem2, const char* name, E_PLAY_TYPE playType, int32_t maxPlayTime)
	: PlayType(playType), MaxPlayTime(maxPlayTime)
{
	FileM2 = static_cast<CFileM2*>(filem2);

	Q_strcpy(Name, DEFAULT_SIZE, name);

	TotalLoopTime = 0;
}

wow_m2Action::~wow_m2Action()
{
	
}

bool wow_m2Action::pushAnimationEntry( const char* animName, uint32_t subIdx, int32_t loopTime )
{
	int16_t animIndex = FileM2->getAnimationIndex(animName, subIdx);
	if (animIndex == -1)
		return false;

	SAnimEntry entry;
	entry.entryIndex = (uint16_t)AnimationList.size();
	entry.animIndex = animIndex;
	entry.loopTime = loopTime;

	AnimationList.emplace_back(entry);

	if(PlayType == EPT_RANDOM)
		TotalLoopTime += loopTime;

	return true;
}

void wow_m2Action::clear()
{
	AnimationList.clear();
	TotalLoopTime = 0;
}