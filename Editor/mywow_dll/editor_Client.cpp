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

void Client_tick( u32 delta )
{
	g_Client->tick(delta);
}

void Client_onMouseWheel( f32 fDelta )
{
	g_Client->onMouseWheel(fDelta);
}

void Client_onMouseMove( f32 pitchDegree, f32 yawDegree )
{
	g_Client->onMouseMove(pitchDegree, yawDegree);
}

