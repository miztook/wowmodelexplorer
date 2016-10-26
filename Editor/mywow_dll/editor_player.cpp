#include "stdafx.h"
#include "editor_player.h"

ICamera* Player_getTargetCamera( IPlayer* player )
{
	wow_m2TargetCamera* targetCamera = player->getTargetCamera();
	if (!targetCamera)
		return NULL;

	return targetCamera->getCamera();
}

void Player_setTargetCamera( IPlayer* player, f32 nearValue, f32 farValue, f32 fov )
{
	player->setTargetCamera(nearValue, farValue, fov);
}

void Player_setM2AsTarget( IPlayer* player, IM2SceneNode* m2Node, f32 distance, f32 rad )
{
	player->setM2AsTarget(m2Node, distance, rad);
}

void Player_setMoveSpeed( IPlayer* player, f32 walk, f32 run, f32 backwards, f32 roll )
{
	player->setMoveSpeed(walk, run, backwards, roll);
}

void Player_getMoveSpeed( IPlayer* player, f32* walk, f32* run, f32* backwards, f32* roll )
{
	player->getMoveSpeed(*walk, *run, *backwards, *roll);
}

void Player_rideOnModel( IPlayer* player, s32 npcid, E_M2_STATES state )
{
	player->rideOnModel(npcid, state);
}

s32 Player_getRideNpcId( IPlayer* player )
{
	return player->getRideNpcId();
}
