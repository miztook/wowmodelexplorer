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
	
	virtual void setTargetCamera(float nearValue, float farValue, float fov) = 0;

	virtual void setM2AsTarget(IM2SceneNode* node, float distance, float rad) = 0;

	virtual void setMoveSpeed(float walk, float run, float backwards, float roll) = 0;

	virtual void getMoveSpeed(float& walk, float& run, float& backwards, float& roll) = 0;

	virtual void tick(uint32_t delta) = 0;

	virtual void onMouseWheel(float fDelta) = 0;

	virtual void onMouseMove(float pitchDegree, float yawDegree) = 0;

	virtual void rideOnModel(int32_t npcid, E_M2_STATES state) = 0;

	virtual int32_t getRideNpcId() const = 0;
};