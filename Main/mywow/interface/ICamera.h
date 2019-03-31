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
	ICamera( const vector3df& position, const vector3df& lookat, const vector3df& up, float nearValue, float farValue, float fov ) : Dir((lookat - position).normalize()), Up(up), LookAt(lookat)
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
	float getClipDistance() const  { return ClipDistance; }
	void setClipDistance(float distance);
	void makeClipPlane( const plane3df& plane, plane3df& clip );

	virtual void onKeyMove(float speed, const SKeyControl& keycontrol) = 0;

	virtual void recalculateAll() = 0;
	virtual void pitch_yaw_Maya(float pitchDegree, float yawDegree) = 0;
	virtual void move_offset_Maya(float xOffset, float yOffset) = 0;
	virtual void pitch_yaw_FPS(float pitchDegree, float yawDegree) = 0;

	virtual vector2di getScreenPositionFrom3DPosition(const vector3df& pos, const recti& screensize) = 0;
	virtual line3df getRayFromScreenPosition(const vector2di& pos, const recti& screensize) = 0;
	virtual vector3df get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, float distance) = 0;

public:
	float		NearValue;
	float		FarValue;
	float		AspectRatio;
	float		FOV;

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
	float		ClipDistance;

	bool		IsOrthogonal;
};

inline void ICamera::setClipDistance( float distance )
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