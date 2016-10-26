#pragma once

#include "ICamera.h"

class IVideoDriver;

class CCamera : public ICamera
{
public:
	CCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov);

public:
	virtual void onKeyMove(f32 speed, const SKeyControl& keycontrol);

	virtual void recalculateAll();
	virtual void pitch_yaw_Maya(f32 pitchDegree, f32 yawDegree);
	virtual void move_offset_Maya(f32 xOffset, f32 yOffset);
	virtual void pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree);

	virtual vector2di getScreenPositionFrom3DPosition(const vector3df& pos, const recti& screensize);
	virtual line3df getRayFromScreenPosition(const vector2di& pos, const recti& screensize);
	virtual vector3df get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, f32 distance);

private:
	void recalculateViewMatrix();
	void recalculateProjectionMatrix();
	void recalculateFrustum();

};