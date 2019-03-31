#include "stdafx.h"
#include "editor_player.h"

ICamera* Player_getTargetCamera( IPlayer* player )
{
	wow_m2TargetCamera* targetCamera = player->getTargetCamera();
	if (!targetCamera)
		return NULL;

	return targetCamera->getCamera();
}

void Player_setTargetCamera( IPlayer* player, float nearValue, float farValue, float fov )
{
	player->setTargetCamera(nearValue, farValue, fov);
}

void Player_setM2AsTarget( IPlayer* player, IM2SceneNode* m2Node, float distance, float rad )
{
	player->setM2AsTarget(m2Node, distance, rad);
}

void Player_setMoveSpeed( IPlayer* player, float walk, float run, float backwards, float roll )
{
	player->setMoveSpeed(walk, run, backwards, roll);
}

void Player_getMoveSpeed( IPlayer* player, float* walk, float* run, float* backwards, float* roll )
{
	player->getMoveSpeed(*walk, *run, *backwards, *roll);
}

void Player_rideOnModel( IPlayer* player, int32_t npcid, E_M2_STATES state )
{
	player->rideOnModel(npcid, state);
}

int32_t Player_getRideNpcId( IPlayer* player )
{
	return player->getRideNpcId();
}
