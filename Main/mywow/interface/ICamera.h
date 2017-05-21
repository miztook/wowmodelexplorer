#pragma once

#include "core.h"

class ICamera
{
public:
	struct SKeyControl 
	{
		bool front;
		bool back;
		bool left;
		bool right;
		bool up;
		bool down;
	};

public:
	ICamera( const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov ) : Dir((lookat - position).normalize()), Up(up), LookAt(lookat)
	{
		IsOrthogonal = false;
		AspectRatio = 1.0f;

		Position = position;
		NearValue = nearValue;
		FarValue = farValue;
		FOV = fov;

		ClipDistance = FarValue * 0.75f;
	}

	virtual ~ICamera() {}

public:
	//
	const frustum& getViewFrustum() const { return ViewFrustum; }
	const matrix4& getViewMatrix() const { return ViewMatrix; }
	const matrix4& getProjectionMatrix() const { return ProjectionMatrix; }
	const matrix4& getViewProjectionMatrix() const { return ViewProjectionMatrix; }
	const matrix4& getInverseViewProjectionMatrix() const { return InverseViewProjectionMatrix; }
	const matrix4& getInverseViewMatrix() const { return InverseViewMatrix; }
	const plane3df& getTerrainClipPlane() const { return ClipPlane; }
	const vector3df& getPosition() const { return Position; }
	void setPosition(const vector3df& pos) { Position = pos; }
	const vector3df& getDir() const { return Dir; }
	void setDir(const vector3df& dir) { Dir = dir; Dir.normalize(); }
	const vector3df& getRight() const { return Right; }
	const vector3df& getUp() const { return Up; }
	void setLookat(const vector3df& lookat) { LookAt = lookat; }
	const vector3df& getLookat() const { return LookAt; }
	f32 getClipDistance() const  { return ClipDistance; }
	void setClipDistance(f32 distance);
	void makeClipPlane( const plane3df& plane, plane3df& clip );

	virtual void onKeyMove(f32 speed, const SKeyControl& keycontrol) = 0;

	virtual void recalculateAll() = 0;
	virtual void pitch_yaw_Maya(f32 pitchDegree, f32 yawDegree) = 0;
	virtual void move_offset_Maya(f32 xOffset, f32 yOffset) = 0;
	virtual void pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree) = 0;

	virtual vector2di getScreenPositionFrom3DPosition(const vector3df& pos, const recti& screensize) = 0;
	virtual line3df getRayFromScreenPosition(const vector2di& pos, const recti& screensize) = 0;
	virtual vector3df get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, f32 distance) = 0;

public:
	f32		NearValue;
	f32		FarValue;
	f32		AspectRatio;
	f32		FOV;

protected:
	matrix4		ViewMatrix;
	matrix4		ProjectionMatrix;
	matrix4		ViewProjectionMatrix;
	matrix4		InverseViewProjectionMatrix;
	matrix4		InverseViewMatrix;
	matrix4		ClipProjectionMatrix;
	frustum		ViewFrustum;
	plane3df	ClipPlane;

	vector3df		Position;
	vector3df		Dir;
	vector3df		Up;
	vector3df		Right;
	vector3df		LookAt;
	f32		ClipDistance;

	bool		IsOrthogonal;
};

inline void ICamera::setClipDistance( f32 distance )
{
	ClipDistance = min_(FarValue, distance);
	recalculateAll();
}

inline void ICamera::makeClipPlane( const plane3df& plane, plane3df& clip )
{
	if(&clip != &plane)
		clip = plane;

	matrix4 mat = ViewMatrix * ProjectionMatrix;
	mat.transformForClipPlane(clip);
}