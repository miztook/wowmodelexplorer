#include "stdafx.h"
#include "wow_m2Logic.h"
#include "wow_m2action.h"
#include "wow_m2States.h"


wow_m2Logic::wow_m2Logic()
{
	g_Engine->getResourceLoader()->registerM2Loaded(this);
	g_Engine->getSceneManager()->registerM2SceneNodeLoad(this);
}

wow_m2Logic::~wow_m2Logic()
{
	g_Engine->getSceneManager()->removeM2SceneNodeLoad(this);
	g_Engine->getResourceLoader()->removeM2Loaded(this);
}

void wow_m2Logic::onM2Loaded( IFileM2* filem2 )
{
	M2Type type = filem2->getType();
	if (type == MT_CHARACTER || type == MT_CREATRUE || type == MT_INTERFACE)
	{
		buildActions(filem2);
	}
}

void wow_m2Logic::onM2SceneNodeAdd( IM2SceneNode* node )
{
	M2Type type = node->getFileM2()->getType();
	if (type == MT_CHARACTER || type == MT_CREATRUE || type == MT_INTERFACE)
	{
		buildStates(node);
	}
}

void wow_m2Logic::buildActions(IFileM2* fileM2)
{
	fileM2->clearAllActions();

	buildStandAction(fileM2);

	addAction(fileM2, EMS_JUMP, wow_m2Action::EPT_CUSTOM, 1, M2Actions.AnimJump);
	addAction(fileM2, EMS_WALK, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimWalk);
	addAction(fileM2, EMS_WALKBACKWARDS, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimWalkBackwards);
	addAction(fileM2, EMS_RUN, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimRun);
	addAction(fileM2, EMS_ROLL, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimRoll);
	addAction(fileM2, EMS_DANCE, wow_m2Action::EPT_SEQUENCE, 2, M2Actions.AnimEmoteDance);
	addAction(fileM2, EMS_BATTLEROAR, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimBattleRoar);
	addAction(fileM2, EMS_SPINNINGKICK, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSpinningKick);
	addAction(fileM2, EMS_TORPEDO, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimTorpedo);
	addAction(fileM2, EMS_FLYINGKICK, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimFlyingKick);
	addAction(fileM2, EMS_KNEEL, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimKneel);
	addAction(fileM2, EMS_EMOTEREAD, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimEmoteRead);
	addAction(fileM2, EMS_CRANE, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimCrane);
	addAction(fileM2, EMS_SITGROUND, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSitGround);
	addAction(fileM2, EMS_MOUNT, wow_m2Action::EPT_RANDOM, -1, M2Actions.AnimMount);
	addAction(fileM2, EMS_RECLINEDMOUNT, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimReclinedMount);
	addAction(fileM2, EMS_RECLINEDMOUNTPASSENGER, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimReclinedMountPassenger);
	addAction(fileM2, EMS_STEALTHSTAND, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimStealthStand);
	addAction(fileM2, EMS_ATTACK1H, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimAttack1H);
	addAction(fileM2, EMS_ATTACK2H, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimAttack2H);
	addAction(fileM2, EMS_SPELLCASTOMNI, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSpellCastOmni);
	addAction(fileM2, EMS_SPELLCASTDIRECTED, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSpellCastDirected);
	addAction(fileM2, EMS_SPELLCAST, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSpellCast);
	addAction(fileM2, EMS_SPELLPRECAST, wow_m2Action::EPT_SEQUENCE, 1, M2Actions.AnimSpellPrecast);
	addAction(fileM2, EMS_READYSPELLOMNI, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimReadySpellOmni);
	addAction(fileM2, EMS_READYSPELLDIRECTED, wow_m2Action::EPT_SEQUENCE, -1, M2Actions.AnimReadySpellDirected);
}

void wow_m2Logic::buildStates( IM2SceneNode* m2SceneNode )
{
	wow_m2FSM* fsm = m2SceneNode->getM2FSM();
	fsm->clearStates();

	fsm->setState(EMS_STAND, new wow_m2State_Stand(fsm->getM2SceneNode()));
	fsm->setState(EMS_JUMP, new wow_m2State_Jump(fsm->getM2SceneNode()));

	#define BUILD_STATE(state) fsm->setState(state, new wow_m2State_commonAction(fsm->getM2SceneNode(), state))

	BUILD_STATE(EMS_WALK);
	BUILD_STATE(EMS_WALKBACKWARDS);
	BUILD_STATE(EMS_RUN);
	BUILD_STATE(EMS_DANCE);
	BUILD_STATE(EMS_BATTLEROAR);
	BUILD_STATE(EMS_ROLL);
	BUILD_STATE(EMS_SPINNINGKICK);
	BUILD_STATE(EMS_TORPEDO);
	BUILD_STATE(EMS_FLYINGKICK);
	BUILD_STATE(EMS_KNEEL);
	BUILD_STATE(EMS_EMOTEREAD);
	BUILD_STATE(EMS_CRANE);
	BUILD_STATE(EMS_SITGROUND);
	BUILD_STATE(EMS_MOUNT);
	BUILD_STATE(EMS_RECLINEDMOUNT);
	BUILD_STATE(EMS_RECLINEDMOUNTPASSENGER);
	BUILD_STATE(EMS_STEALTHSTAND);

	BUILD_STATE(EMS_ATTACK1H);
	BUILD_STATE(EMS_ATTACK2H);
	BUILD_STATE(EMS_SPELLCASTOMNI);
	BUILD_STATE(EMS_SPELLCASTDIRECTED);
	BUILD_STATE(EMS_SPELLCAST);
	BUILD_STATE(EMS_SPELLPRECAST);
	BUILD_STATE(EMS_READYSPELLOMNI);
	BUILD_STATE(EMS_READYSPELLDIRECTED);
}

bool wow_m2Logic::addAction( IFileM2* filem2, E_M2_STATES act, wow_m2Action::E_PLAY_TYPE playType, s32 maxPlayTime, const SAnimationEntries& anims)
{
	const c8* name = getActionName(act);

	wow_m2Action* action = new wow_m2Action(filem2, name, playType, maxPlayTime);
	action->clear();
	for (u32 i = 0; i < anims.count; ++i)
	{
		action->pushAnimationEntry(anims.animations[i].strName, anims.animations[i].subIdx, anims.animations[i].loopTime);
	}

	if (action->getAnimationCount() == 0)
	{
		delete action;
		return false;
	}

	return filem2->addAction(action);
}

void wow_m2Logic::buildStandAction( IFileM2* fileM2 )
{
#define ADD_STAND_ACTION(arr) addAction(fileM2, EMS_STAND, wow_m2Action::EPT_RANDOM, -1, arr)

	u32 race;
	u32 gender;
	bool isHD;
	bool ret = g_Engine->getWowDatabase()->getRaceGender(fileM2->Name, race, gender, isHD);
	if (!ret)
	{
		addAction(fileM2, EMS_STAND, wow_m2Action::EPT_RANDOM, -1, M2Actions.AnimStand01234);
		return;
	}

	switch(race)
	{
	case RACE_HUMAN:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand01);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand02);
		}
		break;
	case RACE_NIGHTELF:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand01234);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_DWARF:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_GNOME:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_DRAENEI:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand01);
		}
		break;
	case RACE_WORGEN:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand01);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_PANDAREN:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand0123);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_ORC:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand02);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_TAUREN:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_NORTHREND_SKELETON:
	case RACE_SKELETON:
	case RACE_UNDEAD:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand012);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand023);
		}
		break;
	case RACE_TROLL:
	case RACE_FOREST_TROLL:
	case RACE_ICE_TROLL:
		{
			if (gender)
				ADD_STAND_ACTION(M2Actions.AnimStand01);
			else
				ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_BLOODELF:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	case RACE_GOBLIN:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand012);
		}
		break;
	default:
		{
			ADD_STAND_ACTION(M2Actions.AnimStand01234);
		}
		break;
	}

}

