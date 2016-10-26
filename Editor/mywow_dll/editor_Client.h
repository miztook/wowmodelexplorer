#pragma once

#include "editor_base.h"
#include "editor_structs.h"

MW_API void  Client_create();
MW_API void  Client_destroy();
MW_API IPlayer*  Client_getPlayer();
MW_API IWorld*  Client_getWorld();

MW_API void Client_tick(u32 delta);
MW_API void Client_onMouseWheel(f32 fDelta);
MW_API void Client_onMouseMove(f32 pitchDegree, f32 yawDegree);

