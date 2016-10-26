#pragma once

#include "base.h"
#include "wow_m2action.h"

struct SAnimationEntries
{
	SAnimationEntries()
		: animations(NULL), count(0) {}

	SAnimationEntries(wow_m2Action::SAnimationEntry* anims, u32 num) : animations(anims), count(num) {}

	wow_m2Action::SAnimationEntry* animations;
	u32 count;
};

class wow_m2Actions
{
private:
	DISALLOW_COPY_AND_ASSIGN(wow_m2Actions);

public:
	wow_m2Actions();
	~wow_m2Actions();

public:
	SAnimationEntries	AnimStand02;
	SAnimationEntries	AnimStand023;
	SAnimationEntries	AnimStand01;
	SAnimationEntries	AnimStand012;
	SAnimationEntries	AnimStand0123;
	SAnimationEntries	AnimStand01234;

	SAnimationEntries	AnimWalk;
	SAnimationEntries	AnimWalkBackwards;
	SAnimationEntries	AnimRun;
	SAnimationEntries	AnimRoll;
	SAnimationEntries	AnimJump;
	SAnimationEntries	AnimEmoteDance;
	SAnimationEntries	AnimBattleRoar;
	SAnimationEntries	AnimSpinningKick;
	SAnimationEntries	AnimTorpedo;
	SAnimationEntries	AnimFlyingKick;
	SAnimationEntries	AnimKneel;
	SAnimationEntries	AnimEmoteRead;
	SAnimationEntries	AnimCrane;
	SAnimationEntries	AnimSitGround;
	SAnimationEntries	AnimMount;
	SAnimationEntries	AnimReclinedMount;
	SAnimationEntries	AnimReclinedMountPassenger;
	SAnimationEntries	AnimStealthStand;
	SAnimationEntries	AnimAttack1H;
	SAnimationEntries	AnimAttack2H;
	SAnimationEntries	AnimSpellCastOmni;
	SAnimationEntries	AnimSpellCastDirected;
	SAnimationEntries	AnimSpellCast;
	SAnimationEntries	AnimSpellPrecast;
	SAnimationEntries	AnimReadySpellOmni;
	SAnimationEntries	AnimReadySpellDirected;
};