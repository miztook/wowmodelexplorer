#pragma once

#include "base.h"

class ICamera;
class IM2SceneNode;

class wow_m2TargetCamera
{
public:
	wow_m2TargetCamera(float nearValue, float farValue, float fov);
	~wow_m2TargetCamera();

public:
	ICamera* getCamera() const { return Camera; }

	//rad cam dir to m2 dir
	void setM2AsTarget(IM2SceneNode* node);
	void setM2AsTarget(IM2SceneNode* node, float distance, float rad);
	IM2SceneNode* getM2Target() const { return M2Node; }

	void tick(uint32_t delta);

	void setMinMaxDistance(float minDistance, float maxDistance);

	void onMouseWheel(float fDelta);

	void onMouseMove(float pitchDegree, float yawDegree);

	void makeTargetFollowCamera(uint32_t deltaTime, bool front, bool back, bool left, bool right);
	void makeCameraFollowTarget(uint32_t deltaTime, bool front, bool back, bool left, bool right);

	void onTargetMove();

private:
	void setDestDir(const vector3df& dir);

private:
	ICamera*		Camera;
	IM2SceneNode*		M2Node;
	float	MaxDistance;
	float	MinDistance;

	float	CurrentDistance;
	float	CurrentHeight;

	vector3df		DestDir;
	bool		NeedAdjustDir;
	bool		AdjustRight;
};

