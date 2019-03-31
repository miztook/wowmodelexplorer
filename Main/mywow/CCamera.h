#pragma once

#include "ICamera.h"

class IVideoDriver;

class CCamera : public ICamera
{
public:
	CCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, float nearValue, float farValue, float fov);

public:
	virtual void onKeyMove(float speed, const SKeyControl& keycontrol);

	virtual void recalculateAll();
	virtual void pitch_yaw_Maya(float pitchDegree, float yawDegree);
	virtual void move_offset_Maya(float xOffset, float yOffset);
	virtual void pitch_yaw_FPS(float pitchDegree, float yawDegree);

	virtual vector2di getScreenPositionFrom3DPosition(const vector3df& pos, const recti& screensize);
	virtual line3df getRayFromScreenPosition(const vector2di& pos, const recti& screensize);
	virtual vector3df get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, float distance);

private:
	void recalculateViewMatrix();
	void recalculateProjectionMatrix();
	void recalculateFrustum();

};