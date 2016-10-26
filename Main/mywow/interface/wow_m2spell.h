#pragma once

#include "core.h"
#include "wow_enums.h"

class IM2SceneNode;
class wow_m2instance;

//IM2SceneNodeµÄ¼¼ÄÜ
class wow_m2spell
{
public:
	explicit wow_m2spell(IM2SceneNode* node);

public:
	bool takeSpellVisualKit(u32 id, u32 time);

	void tick(u32 timeSinceStart, u32 timeSinceLastFrame);

public:
	struct SDynSpell 
	{
		u32 time;
		u32 currentTime;
		IM2SceneNode* nodes[SVK_COUNT];
		bool spellAnimation;
	};

	SDynSpell	DynSpell;

private:
	IM2SceneNode*			M2SceneNode;
	u32 CurrentId;
	

};