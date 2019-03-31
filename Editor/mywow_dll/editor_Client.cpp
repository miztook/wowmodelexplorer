#include "stdafx.h"
#include "editor_Client.h"
#include "CSysUtility.h"

void Client_create()
{
	createClient();
}

void Client_destroy()
{
	destroyClient();
}

IPlayer* Client_getPlayer()
{
	return g_Client->getPlayer();
}

IWorld* Client_getWorld()
{
	return g_Client->getWorld();
}

void Client_tick( uint32_t delta )
{
	g_Client->tick(delta);
}

void Client_onMouseWheel( float fDelta )
{
	g_Client->onMouseWheel(fDelta);
}

void Client_onMouseMove( float pitchDegree, float yawDegree )
{
	g_Client->onMouseMove(pitchDegree, yawDegree);
}

