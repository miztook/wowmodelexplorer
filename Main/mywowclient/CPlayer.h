#pragma once

#include "IPlayer.h"

class CPlayer : public IPlayer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CPlayer);

public:
	CPlayer();

	~CPlayer();

public:
	virtual void setTargetCamera(float nearValue, float farValue, float fov);

	virtual void setM2AsTarget(IM2SceneNode* node, float distance, float rad);

	virtual wow_m2TargetCamera* getTargetCamera() const { return TargetCamera; }

	virtual void setMoveSpeed(float walk, float run, float backwards, float roll);

	virtual void getMoveSpeed(float& walk, float& run, float& backwards, float& roll);

	virtual void tick(uint32_t delta);

	virtual void onMouseWheel(float fDelta);

	virtual void onMouseMove(float pitchDegree, float yawDegree);

	virtual void rideOnModel(int32_t npcid, E_M2_STATES state);

	virtual int32_t getRideNpcId() const { return RideNpcId; }

private:
	void onPlayerSpellAction(uint32_t delta);
	void onPlayerMoveAction(uint32_t delta, const SMoveControl& moveControl);		//控制移动和动作
	void onPlayerDirection(uint32_t delta, const SMoveControl& moveControl);		//控制方向

	vector3df getMoveVector() const;

	bool isJumpEnd() const;
	bool isAboutLand() const;
	void setNextLandRun();
	void setJumpDir(const vector3df& dir);
	vector3df getJumpDir() const;

	bool isStateCanMove(E_M2_STATES state, bool onKey) const;
	bool isStateInterruptible(E_M2_STATES state) const;

private:
	wow_m2TargetCamera*		TargetCamera;
	float		WalkSpeed;
	float		RunSpeed;
	float		RollSpeed;
	float		BackwardsSpeed;
	IM2SceneNode*		MainModel;
	IM2SceneNode*		RideModel;
	int32_t		RideNpcId;
};