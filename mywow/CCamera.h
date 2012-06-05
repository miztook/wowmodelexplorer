#pragma once

#include "ICamera.h"

class IVideoDriver;

class CCamera : public ICamera
{
public:
	CCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov);

public:
	//
	virtual const frustum& getViewFrustum() const { return ViewFrustum; }
	virtual const matrix4& getViewMatrix() const { return ViewMatrix; }
	virtual const matrix4& getProjectionMatrix() const { return ProjectionMatrix; }
	virtual const matrix4& getInverseViewMatrix() const { return InverseViewMatrix; }
	virtual const vector3df& getDir() const { return Dir; }

	virtual void onKeyMove(f32 speed, SKeyControl keycontrol);

	virtual void recalculateAll();
	virtual void pitch_yaw_Maya(f32 pitchDegree, f32 yawDegree);
	virtual void move_offset_Maya(f32 xOffset, f32 yOffset);
	virtual void pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree);

private:
	void recalculateViewMatrix();
	void recalculateProjectionMatrix(bool orthogonal);
	void recalculateFrustum();

private:
	matrix4	ViewMatrix;
	matrix4  ProjectionMatrix;
	matrix4	InverseViewMatrix;
	frustum	ViewFrustum;
	vector3df		Dir;
};