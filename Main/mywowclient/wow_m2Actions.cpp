#include "stdafx.h"
#include "wow_m2Actions.h"

wow_m2Action::SAnimationEntry animStand02[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 2, 1 },
};

wow_m2Action::SAnimationEntry animStand023[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 2, 1 },
	{ "Stand", 3, 1 },
};

wow_m2Action::SAnimationEntry animStand01[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 1, 1 },
};

wow_m2Action::SAnimationEntry animStand012[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 1, 1 },
	{ "Stand", 2, 1 },
};

wow_m2Action::SAnimationEntry animStand0123[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 1, 1 },
	{ "Stand", 2, 1 },
	{ "Stand", 3, 1 },
};

wow_m2Action::SAnimationEntry animStand01234[] =
{
	{ "Stand", 0, 3 },
	{ "Stand", 1, 1 },
	{ "Stand", 2, 1 },
	{ "Stand", 3, 1 },
	{ "Stand", 4, 1 },
};

wow_m2Action::SAnimationEntry animWalk[] = 
{
	{"Walk", 0, 1},
};

wow_m2Action::SAnimationEntry animWalkbackwards[] =
{
	{"Walkbackwards", 0, 1},
};

wow_m2Action::SAnimationEntry animRun[] = 
{
	{"Run", 0, 1},
};

wow_m2Action::SAnimationEntry animAttack1H[] =
{
	{"Ready1H", 0, 1},
	{"Attack1H", 0, 1},
	{"Ready1H", 0, 1},
	{"Attack1H", 1, 1},
	{"Ready1H", 0, 1},
	{"Special1H", 0, 1},
	{"Ready1H", 0, 1},
};

wow_m2Action::SAnimationEntry animAttack2H[] =
{
	{"Ready2H", 0, 1},
	{"Attack2H", 0, 1},
	{"Ready2H", 0, 1},
	{"Attack2H", 1, 1},
	{"Ready2H", 0, 1},
	{"Special2H", 0, 1},
	{"Ready2H", 0, 1},
};

wow_m2Action::SAnimationEntry animRoll[] =
{
	{ "RollStart", 0, 1 },
	{ "Roll", 0, 4 },
	{ "RollEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animJump[] =
{
	{ "JumpStart", 0, 1 },
	{ "Jump", 0, 1 },
	{ "JumpEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animEmoteDance[] =
{
	{ "EmoteDance", 0, 1 },
	{ "EmoteDance", 1, 1 },
	{ "EmoteDance", 2, 1 },
	{ "EmoteDance", 3, 1 },
	{ "EmoteDance", 4, 1 },
};

wow_m2Action::SAnimationEntry animFlyingKick[] =
{
	{ "FlyingKickStart", 0, 1 },
	{ "FlyingKick", 0, 1 },
	{ "FlyingKickEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animKneel[] =
{
	{ "KneelStart", 0, 1 },
	{ "KneelLoop", 0, 3 },
	{ "KneelEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animEmoteRead[] =
{
	{ "EmoteReadStart", 0, 1 },
	{ "EmoteReadLoop", 0, 2 },
	{ "EmoteReadEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animCrane[] =
{
	{ "CraneStart", 0, 1 },
	{ "CraneLoop", 0, 3 },
	{ "CraneEnd", 0, 1 },
};

wow_m2Action::SAnimationEntry animSitGround[] =
{
	{ "SitGroundDown", 0, 1 },
	{ "SitGround", 0, 2 },
	{ "SitGroundUp", 0, 1 },
};

wow_m2Action::SAnimationEntry animMount[] =
{
	{ "Mount", 0, 3 },
	{ "Mount", 1, 1 },
};

wow_m2Action::SAnimationEntry animBattleRoar[] = { {"BattleRoar", 0, 1},};

wow_m2Action::SAnimationEntry animSpinningKick[] = { {"SpinningKick", 0, 3},};

wow_m2Action::SAnimationEntry animTorpedo[] = { {"Torpedo", 0, 6},};

wow_m2Action::SAnimationEntry animSpellCastOmni[] = { {"SpellCastOmni", 0, 1}, };

wow_m2Action::SAnimationEntry animSpellCastDirected[] = { {"SpellCastDirected", 0, 1}, };

wow_m2Action::SAnimationEntry animSpellCast[] = { {"SpellCast", 0, 1}, };

wow_m2Action::SAnimationEntry animSpellPrecast[] = { {"SpellPrecast", 0, 1}, };

wow_m2Action::SAnimationEntry animReclinedMount[] = { {"ReclinedMount", 0, 1}, };

wow_m2Action::SAnimationEntry animReclinedMountPassenger[] = { {"ReclinedMountPassenger", 0, 1}, };

wow_m2Action::SAnimationEntry animStealthStand[] = { {"StealthStand", 0, 1}, };

wow_m2Action::SAnimationEntry animReadySpellOmni[] = { {"ReadySpellOmni", 0, 1}, };

wow_m2Action::SAnimationEntry animReadySpellDirected[] = { {"animReadySpellDirected", 0, 1}, };

wow_m2Actions::wow_m2Actions()
{
#define BUILD_ANIMATION(x, y) x = SAnimationEntries(y, ARRAY_COUNT(y))

	BUILD_ANIMATION(AnimStand02, animStand02);
	BUILD_ANIMATION(AnimStand023, animStand023);
	BUILD_ANIMATION(AnimStand01, animStand01);
	BUILD_ANIMATION(AnimStand012, animStand012);
	BUILD_ANIMATION(AnimStand0123, animStand0123);
	BUILD_ANIMATION(AnimStand01234, animStand01234);

	BUILD_ANIMATION(AnimWalk, animWalk);
	BUILD_ANIMATION(AnimWalkBackwards, animWalkbackwards);
	BUILD_ANIMATION(AnimRun, animRun);
	BUILD_ANIMATION(AnimRoll, animRoll);
	BUILD_ANIMATION(AnimJump, animJump);
	BUILD_ANIMATION(AnimEmoteDance, animEmoteDance);
	BUILD_ANIMATION(AnimBattleRoar, animBattleRoar);
	BUILD_ANIMATION(AnimSpinningKick, animSpinningKick);
	BUILD_ANIMATION(AnimTorpedo, animTorpedo);
	BUILD_ANIMATION(AnimFlyingKick, animFlyingKick);
	BUILD_ANIMATION(AnimKneel, animKneel);
	BUILD_ANIMATION(AnimEmoteRead, animEmoteRead);
	BUILD_ANIMATION(AnimCrane, animCrane);
	BUILD_ANIMATION(AnimSitGround, animSitGround);
	BUILD_ANIMATION(AnimMount, animMount);
	BUILD_ANIMATION(AnimReclinedMount, animReclinedMount);
	BUILD_ANIMATION(AnimReclinedMountPassenger, animReclinedMountPassenger);
	BUILD_ANIMATION(AnimStealthStand, animStealthStand);
	BUILD_ANIMATION(AnimAttack1H, animAttack1H);
	BUILD_ANIMATION(AnimAttack2H, animAttack2H);
	BUILD_ANIMATION(AnimSpellCastOmni, animSpellCastOmni);
	BUILD_ANIMATION(AnimSpellCastDirected, animSpellCastDirected);
	BUILD_ANIMATION(AnimSpellCast, animSpellCast);
	BUILD_ANIMATION(AnimSpellPrecast, animSpellPrecast);
	BUILD_ANIMATION(AnimReadySpellOmni, animReadySpellOmni);
	BUILD_ANIMATION(AnimReadySpellDirected, animReadySpellDirected);
}

wow_m2Actions::~wow_m2Actions()
{

}