#pragma once

#include "editor_base.h"

MW_API ICamera* Player_getTargetCamera(IPlayer* player);
MW_API void Player_setTargetCamera(IPlayer* player, f32 nearValue, f32 farValue, f32 fov);
MW_API void Player_setM2AsTarget(IPlayer* player, IM2SceneNode* m2Node, f32 distance, f32 rad);
MW_API void Player_setMoveSpeed(IPlayer* player, f32 walk, f32 run, f32 backwards, f32 roll);
MW_API void Player_getMoveSpeed(IPlayer* player, f32* walk, f32* run, f32* backwards, f32* roll);
MW_API void Player_rideOnModel(IPlayer* player, s32 npcid, E_M2_STATES state);
MW_API s32 Player_getRideNpcId(IPlayer* player);