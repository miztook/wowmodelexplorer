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
	virtual void setTargetCamera(f32 nearValue, f32 farValue, f32 fov);

	virtual void setM2AsTarget(IM2SceneNode* node, f32 distance, f32 rad);

	virtual wow_m2TargetCamera* getTargetCamera() const { return TargetCamera; }

	virtual void setMoveSpeed(f32 walk, f32 run, f32 backwards, f32 roll);

	virtual void getMoveSpeed(f32& walk, f32& run, f32& backwards, f32& roll);

	virtual void tick(u32 delta);

	virtual void onMouseWheel(f32 fDelta);

	virtual void onMouseMove(f32 pitchDegree, f32 yawDegree);

	virtual void rideOnModel(s32 npcid, E_M2_STATES state);

	virtual s32 getRideNpcId() const { return RideNpcId; }

private:
	void onPlayerSpellAction(u32 delta);
	void onPlayerMoveAction(u32 delta, const SMoveControl& moveControl);		//控制移动和动作
	void onPlayerDirection(u32 delta, const SMoveControl& moveControl);		//控制方向

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
	f32		WalkSpeed;
	f32		RunSpeed;
	f32		RollSpeed;
	f32		BackwardsSpeed;
	IM2SceneNode*		MainModel;
	IM2SceneNode*		RideModel;
	s32		RideNpcId;
};