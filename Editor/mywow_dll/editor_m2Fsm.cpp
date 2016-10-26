#include "stdafx.h"
#include "editor_m2Fsm.h"

void m2Fsm_resetState( wow_m2FSM* fsm )
{
	fsm->resetState();
}

bool m2Fsm_changeState( wow_m2FSM* fsm, E_M2_STATES state )
{
	return fsm->changeState(state);
}

bool m2Fsm_isStateValid( wow_m2FSM* fsm, E_M2_STATES state )
{
	wow_m2State<IM2SceneNode>* st = fsm->getState(state);
	if (st && st->isValid())
		return true;

	return false;
}
