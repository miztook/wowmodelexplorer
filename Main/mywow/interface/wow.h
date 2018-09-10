#pragma once

#include "wow_enums.h"
#include "wow_def.h"
#include "wow_dbc.h"
#include "wow_dbc70.h"

#if WOW_VER >= 70
using dbtable = dbc70;
#else
using dbtable = dbc;
#endif

#include "wow_database.h"
#include "wow_database70.h"
#include "wowEnvironment.h"
#include "wow_animation.h"
#include "wow_particle.h"

#include "IFileM2.h"
#include "IFileWDT.h"
#include "IFileADT.h"
#include "IFileWMO.h"

#include "wow_m2instance.h"
#include "wow_m2appearance.h"
#include "wow_m2action.h"
#include "wow_m2FSM.h"
#include "wow_m2Move.h"
#include "wow_m2State.h"