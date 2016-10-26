#include "stdafx.h"
#include "wow_m2Action.h"
#include "mywow.h"
#include "CFileM2.h"

wow_m2Action::wow_m2Action(IFileM2* filem2, const c8* name, E_PLAY_TYPE playType, s32 maxPlayTime)
	: PlayType(playType), MaxPlayTime(maxPlayTime)
{
	FileM2 = static_cast<CFileM2*>(filem2);

	Q_strcpy(Name, DEFAULT_SIZE, name);

	TotalLoopTime = 0;
}

wow_m2Action::~wow_m2Action()
{
	
}

bool wow_m2Action::pushAnimationEntry( const c8* animName, u32 subIdx, s32 loopTime )
{
	s16 animIndex = FileM2->getAnimationIndex(animName, subIdx);
	if (animIndex == -1)
		return false;

	SAnimEntry entry;
	entry.entryIndex = (u16)AnimationList.size();
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