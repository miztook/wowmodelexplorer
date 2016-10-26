#pragma once

#include "wow_m2Logic.h"
#include "wow_m2TargetCamera.h"
#include "IPlayer.h"
#include "IWorld.h"

void createClient();
void destroyClient();

#include "base.h"

class IPlayer;
class IWorld;
class wow_m2Logic;

class Client
{
private:
	DISALLOW_COPY_AND_ASSIGN(Client);

public:
	Client();
	~Client();

public:
	IPlayer* getPlayer() const { return Player; }
	IWorld* getWorld() const { return World; }

	wow_m2Logic*	 getWowM2Logic() const { return WowM2Logic; }

	void tick(u32 delta);
	void onMouseWheel(f32 fDelta);
	void onMouseMove(f32 pitchDegree, f32 yawDegree);

private:
	IPlayer* Player;
	IWorld* World;

	wow_m2Logic*		WowM2Logic;
};

extern Client* g_Client;