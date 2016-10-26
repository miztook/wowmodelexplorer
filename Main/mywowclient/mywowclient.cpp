#include "StdAfx.h"
#include "mywowclient.h"
#include "wow_m2Logic.h"
#include "CPlayer.h"
#include "CWorld.h"

Client*		g_Client = NULL;

void createClient()
{
	g_Client = new Client;	
}

void destroyClient()
{
	delete g_Client;
}

Client::Client()
{
	WowM2Logic = new wow_m2Logic;

	World = new CWorld;
	Player = new CPlayer;
}

Client::~Client()
{
	delete Player;
	delete World;
	delete WowM2Logic;
}

void Client::tick( u32 delta )
{	
	World->tick(delta);
	Player->tick(delta);
}

void Client::onMouseWheel( f32 fDelta )
{
	Player->onMouseWheel(fDelta);
}

void Client::onMouseMove( f32 pitchDegree, f32 yawDegree )
{
	Player->onMouseMove(pitchDegree, yawDegree);
}
