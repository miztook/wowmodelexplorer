#pragma once

#include "base.h"
#include "IInputReader.h"

class wow_m2TargetCamera;
class IM2SceneNode;

//玩家逻辑，输入模型控制
class IPlayer
{
public:
	struct SMoveControl 
	{
		bool front;
		bool back;
		bool left;
		bool right;
		bool up;
		bool down;
		bool leftMouse;
		bool rightMouse;
	};

public:
	virtual ~IPlayer() {}

public:
	virtual wow_m2TargetCamera* getTargetCamera() const = 0;
	
	virtual void setTargetCamera(f32 nearValue, f32 farValue, f32 fov) = 0;

	virtual void setM2AsTarget(IM2SceneNode* node, f32 distance, f32 rad) = 0;

	virtual void setMoveSpeed(f32 walk, f32 run, f32 backwards, f32 roll) = 0;

	virtual void getMoveSpeed(f32& walk, f32& run, f32& backwards, f32& roll) = 0;

	virtual void tick(u32 delta) = 0;

	virtual void onMouseWheel(f32 fDelta) = 0;

	virtual void onMouseMove(f32 pitchDegree, f32 yawDegree) = 0;

	virtual void rideOnModel(s32 npcid, E_M2_STATES state) = 0;

	virtual s32 getRideNpcId() const = 0;
};