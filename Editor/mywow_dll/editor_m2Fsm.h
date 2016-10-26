#pragma once

#include "editor_base.h"

//logic
MW_API void m2Fsm_resetState(wow_m2FSM* fsm);
MW_API bool m2Fsm_changeState(wow_m2FSM* fsm, E_M2_STATES state);
MW_API bool m2Fsm_isStateValid(wow_m2FSM* fsm, E_M2_STATES state);