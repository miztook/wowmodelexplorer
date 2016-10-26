#pragma once

#include <cstdint>

enum E_M2_STATES : int32_t
{
	EMS_INVALID = -1,
	EMS_STAND = 0,
	EMS_WALK,
	EMS_WALKBACKWARDS,
	EMS_RUN,
	EMS_ROLL,
	EMS_JUMP,
	EMS_DANCE,
	EMS_BATTLEROAR,
	EMS_SPINNINGKICK,
	EMS_TORPEDO,
	EMS_FLYINGKICK,
	EMS_KNEEL,
	EMS_EMOTEREAD,
	EMS_CRANE,
	EMS_SITGROUND,
	EMS_MOUNT,
	EMS_RECLINEDMOUNT,
	EMS_RECLINEDMOUNTPASSENGER,
	EMS_STEALTHSTAND,
	EMS_ATTACK1H,
	EMS_ATTACK2H,
	EMS_SPELLCASTOMNI,
	EMS_SPELLCASTDIRECTED,
	EMS_SPELLCAST,
	EMS_SPELLPRECAST,
	EMS_READYSPELLOMNI,
	EMS_READYSPELLDIRECTED,

	EMS_COUNT,
};

static const c8* g_actionNames[] =
{
	"Stand",
	"Walk",
	"WalkBackwards",
	"Run",
	"Roll",
	"Jump",
	"Dance",
	"BattleRoar",
	"SpinningKick",
	"Torpedo",
	"FlyingKick",
	"Kneel",
	"EmoteRead",	
	"Crane",
	"SitGround",
	"Mount",
	"ReclinedMount",
	"ReclinedMountPassenger",
	"StealthStand",
	"Attack1H",
	"Attack2H",
	"SpellCastOmni",
	"SpellDirected",
	"SpellCast",
	"SpellPrecast",
	"ReadySpellOmni",
	"ReadySpellDirected",
};

inline const c8* getActionName(E_M2_STATES action)
{
	if (static_cast<u32>(action) < ARRAY_COUNT(g_actionNames))
		return g_actionNames[action];

	ASSERT(false);
	return "";
}

inline E_M2_STATES getM2State(const c8* actionname)
{
	u32 count = ARRAY_COUNT(g_actionNames);
	for (u32 i=0; i<count; ++i)
	{
		if(Q_stricmp(actionname, g_actionNames[i]) == 0)
			return (E_M2_STATES)i;
	}

	return EMS_INVALID;
}

class wow_m2Action;

template <class entity_type>
class wow_m2State
{
public:
	explicit wow_m2State(entity_type* entity) : Entity(entity) {}
	virtual ~wow_m2State() {}

public:
	virtual bool isValid() const = 0;
	virtual bool enter() = 0;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame) = 0;
	virtual void exit() = 0;
	virtual E_M2_STATES getType() const = 0;

	virtual wow_m2Action* getM2Action() const  = 0;
	virtual void onAnimationEnd(u32 currentIndex) = 0;

protected:
	entity_type*	Entity;
};