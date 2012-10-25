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
	virtual const plane3df& getTerrainClipPlane() const { return ClipPlane; }
	virtual const vector3df& getDir() const { return Dir; }
	virtual void setDir(const vector3df& dir) { Dir = dir; Dir.normalize(); }

	virtual void onKeyMove(f32 speed, SKeyControl keycontrol);

	virtual void recalculateAll();
	virtual void pitch_yaw_Maya(f32 pitchDegree, f32 yawDegree);
	virtual void move_offset_Maya(f32 xOffset, f32 yOffset);
	virtual void pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree);

	virtual f32 getClipDistance() const  { return ClipDistance; }
	virtual void setClipDistance(f32 distance);
	virtual void makeClipPlane( const plane3df& plane, plane3df& clip );

private:

	void recalculateViewMatrix();
	void recalculateProjectionMatrix();
	void recalculateFrustum();

private:
	matrix4		ViewMatrix;
	matrix4  ProjectionMatrix;
	matrix4		InverseViewMatrix;
	frustum	ViewFrustum;
	matrix4		ClipProjectionMatrix;
	plane3df	ClipPlane;

	vector3df		Dir;
	f32		ClipDistance;
};