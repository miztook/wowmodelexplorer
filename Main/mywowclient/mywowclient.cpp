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

void Client::tick( uint32_t delta )
{	
	World->tick(delta);
	Player->tick(delta);
}

void Client::onMouseWheel( float fDelta )
{
	Player->onMouseWheel(fDelta);
}

void Client::onMouseMove( float pitchDegree, float yawDegree )
{
	Player->onMouseMove(pitchDegree, yawDegree);
}
