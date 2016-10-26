#pragma once

#include "base.h"

class ICamera;
class IM2SceneNode;

class wow_m2TargetCamera
{
public:
	wow_m2TargetCamera(f32 nearValue, f32 farValue, f32 fov);
	~wow_m2TargetCamera();

public:
	ICamera* getCamera() const { return Camera; }

	//rad cam dir to m2 dir
	void setM2AsTarget(IM2SceneNode* node);
	void setM2AsTarget(IM2SceneNode* node, f32 distance, f32 rad);
	IM2SceneNode* getM2Target() const { return M2Node; }

	void tick(u32 delta);

	void setMinMaxDistance(f32 minDistance, f32 maxDistance);

	void onMouseWheel(f32 fDelta);

	void onMouseMove(f32 pitchDegree, f32 yawDegree);

	void makeTargetFollowCamera(u32 deltaTime, bool front, bool back, bool left, bool right);
	void makeCameraFollowTarget(u32 deltaTime, bool front, bool back, bool left, bool right);

	void onTargetMove();

private:
	void setDestDir(const vector3df& dir);

private:
	ICamera*		Camera;
	IM2SceneNode*		M2Node;
	f32	MaxDistance;
	f32	MinDistance;

	f32	CurrentDistance;
	f32	CurrentHeight;

	vector3df		DestDir;
	bool		NeedAdjustDir;
	bool		AdjustRight;
};

