#pragma once

#include "editor_base.h"
#include "editor_structs.h"

MW_API void  Client_create();
MW_API void  Client_destroy();
MW_API IPlayer*  Client_getPlayer();
MW_API IWorld*  Client_getWorld();

MW_API void Client_tick(uint32_t delta);
MW_API void Client_onMouseWheel(float fDelta);
MW_API void Client_onMouseMove(float pitchDegree, float yawDegree);

