#pragma once

#include "editor_base.h"

MW_API ICamera* Player_getTargetCamera(IPlayer* player);
MW_API void Player_setTargetCamera(IPlayer* player, float nearValue, float farValue, float fov);
MW_API void Player_setM2AsTarget(IPlayer* player, IM2SceneNode* m2Node, float distance, float rad);
MW_API void Player_setMoveSpeed(IPlayer* player, float walk, float run, float backwards, float roll);
MW_API void Player_getMoveSpeed(IPlayer* player, float* walk, float* run, float* backwards, float* roll);
MW_API void Player_rideOnModel(IPlayer* player, int32_t npcid, E_M2_STATES state);
MW_API int32_t Player_getRideNpcId(IPlayer* player);
