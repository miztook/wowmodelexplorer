#include "stdafx.h"
#include "wow_m2FSM.h"
#include "wow_m2action.h"
#include "mywow.h"
#include "IM2SceneNode.h"

wow_m2FSM::wow_m2FSM( IM2SceneNode* node )
{
	M2SceneNode = node;

	CurrentState = NULL_PTR;
	PreviousState = NULL_PTR;
	GlobalState = NULL_PTR;

	::memset(States, 0, sizeof(States));
}

wow_m2FSM::~wow_m2FSM()
{
	clearStates();
}

void wow_m2FSM::onAnimationEnd()
{
 	if (!DynAction.finished)
	{
		ASSERT(DynAction.action);

		switch(DynAction.action->getPlayType())
		{
		case wow_m2Action::EPT_RANDOM:
			onAnimationEndRandom();
			break;
		case wow_m2Action::EPT_SEQUENCE:
			onAnimationEndSequence();
			break;
		case wow_m2Action::EPT_CUSTOM:
			onAnimationEndCustom();
			break;
		}
	}
}

bool wow_m2FSM::playAction( wow_m2Action* action, u32 blendtime )
{
	if (action->getAnimationCount() == 0)
		return false;

	DynAction.playedTime = 0;

	switch (action->getPlayType())
	{
	case wow_m2Action::EPT_RANDOM:
		{
			return playActionRandom(action, blendtime);
		}
		break;
	case wow_m2Action::EPT_SEQUENCE:
	case wow_m2Action::EPT_CUSTOM:
		{
			return playActionSequence(action, blendtime);
		}
		break;
	default:
		ASSERT(false);
		return false;
	}
}

void wow_m2FSM::finishAction()
{
	DynAction.action = NULL_PTR;
	DynAction.playedTime = 0;
	DynAction.currentAnimLoop = 0;
	DynAction.finished = true;
}

bool wow_m2FSM::isPlaying( wow_m2Action* action ) const
{
	return DynAction.action == action && !DynAction.finished;
}

void wow_m2FSM::tick( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	if (GlobalState)
		GlobalState->tick(timeSinceStart, timeSinceLastFrame);

	if (CurrentState)
		CurrentState->tick(timeSinceStart, timeSinceLastFrame);
}

void wow_m2FSM::resetState()
{
	if (CurrentState)
		CurrentState->exit();

	CurrentState = NULL_PTR;
	PreviousState = NULL_PTR;
	GlobalState = NULL_PTR;

	finishAction();
}

bool wow_m2FSM::changeState( wow_m2State<IM2SceneNode>* newstate, bool restart )
{
	ASSERT(newstate);
	if (!newstate)
		return false;

	if (restart || newstate != CurrentState)
	{
		PreviousState = CurrentState;

		if (CurrentState)
			CurrentState->exit();

		if (newstate->enter())
			CurrentState = newstate;
	}

	return true;
}

E_M2_STATES wow_m2FSM::getCurrentState() const
{
	if (CurrentState)
		return CurrentState->getType();
	return EMS_INVALID;
}

bool wow_m2FSM::changeState( E_M2_STATES state, bool restart )
{
	wow_m2State<IM2SceneNode>* newstate = getState(state);

	return changeState(newstate, restart);
}

void wow_m2FSM::revertToPreviousState()
{
	changeState(PreviousState);
}

bool wow_m2FSM::playActionSequence( wow_m2Action* action, u32 blendtime /*= 200*/ )
{
	s32 animIndex = action->AnimationList[0].animIndex;

	if(!M2SceneNode->playAnimationByIndex(animIndex, false, blendtime))
	{
		return false;
	}
	else
	{
		DynAction.action = action;
		DynAction.currentIndex = 0;
		DynAction.currentAnimLoop = 0;
		DynAction.finished = false;
	}

	return true;
}

bool wow_m2FSM::playActionRandom( wow_m2Action* action, u32 blendtime /*= 200*/ )
{
	u32 r = randint(0, action->TotalLoopTime);
	u32 t = 0;
	u32 idx = (u32)action->AnimationList.size();
	for (u32 i=0; i<(u32)action->AnimationList.size(); ++i)
	{
		t += action->AnimationList[i].loopTime;
		if (r > t)
			continue;
		else
		{
			idx = i;
			break;
		}
	}

	ASSERT( idx != action->AnimationList.size() );
	s32 animIndex = action->AnimationList[idx].animIndex;

	if(!M2SceneNode->playAnimationByIndex(animIndex, false, blendtime))
	{
		return false;
	}
	else
	{
		++DynAction.playedTime;
		DynAction.action = action;
		DynAction.currentIndex = idx;
		DynAction.currentAnimLoop = 0;
		DynAction.finished = false;
	}

	return true;
}

void wow_m2FSM::onAnimationEndSequence()
{
	ASSERT(!DynAction.finished);

	wow_m2Action* action = DynAction.action;
	const wow_m2Action::SAnimEntry& animEntry = action->AnimationList[DynAction.currentIndex];

	if (animEntry.loopTime == -1)		//infinite
		return;

	if ((s32)DynAction.currentAnimLoop + 1 < animEntry.loopTime )
	{
		++DynAction.currentAnimLoop;
	}
	else
	{
		++DynAction.currentIndex;
		DynAction.currentAnimLoop = 0;

		if (DynAction.action->MaxPlayTime != -1)
		{
			if(DynAction.currentIndex == action->AnimationList.size())		//a seqence end
			{
				++DynAction.playedTime;

				if (DynAction.playedTime < action->MaxPlayTime)		//continue
				{
					DynAction.currentIndex = 0;	
				}
				else		//finish
				{
					finishAction();
					return;
				}	
			}
		}
		else if (DynAction.currentIndex == action->AnimationList.size())			//infinite
		{
			++DynAction.playedTime;
			DynAction.currentIndex = 0;	
		}
	}

	if(!M2SceneNode->playAnimationByIndex(getCurrentAnimationIndex(), false))
	{
		finishAction();
	}
}

bool wow_m2FSM::advanceAnimation()
{
	wow_m2Action* action = DynAction.action;
	if (DynAction.currentIndex + 1 == action->AnimationList.size())
		return false;

	++DynAction.currentIndex;
	DynAction.currentAnimLoop = 0;

	return true;
}

s16 wow_m2FSM::getCurrentAnimationIndex() const
{
	return DynAction.action->AnimationList[DynAction.currentIndex].animIndex;
}

void wow_m2FSM::onAnimationEndCustom()
{
	if (CurrentState)
		CurrentState->onAnimationEnd(DynAction.currentIndex);
}

void wow_m2FSM::onAnimationEndRandom()
{
	ASSERT(!DynAction.finished);

	if (DynAction.action->MaxPlayTime != -1 &&
		DynAction.playedTime >= DynAction.action->MaxPlayTime)
	{
		finishAction();
	}
	else
	{
		++DynAction.playedTime;
		playActionRandom(DynAction.action);
	}
}

wow_m2State<IM2SceneNode>* wow_m2FSM::getState( E_M2_STATES state ) const
{
	if (state < 0 || state >= EMS_COUNT)
		return NULL_PTR;

	return States[state];
}

void wow_m2FSM::clearStates()
{
	for (u32 i=0; i<EMS_COUNT; ++i)
	{
		if (States[i])
		{
			delete States[i];
			States[i] = NULL_PTR;
		}
	}
}

bool wow_m2FSM::setState( E_M2_STATES state, wow_m2State<IM2SceneNode>* m2state )
{
	if (state < 0 || state >= EMS_COUNT)
		return false;

	delete	States[state];
	States[state] = m2state;

	return true;
}

