#include "stdafx.h"
#include "CCamera.h"
#include "mywow.h"

CCamera::CCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov) : ICamera(position, lookat, up, nearValue, farValue, fov)
{
	recalculateAll();
}

void CCamera::recalculateAll()
{
	recti viewport = g_Engine->getDriver()->getViewPort();
	AspectRatio = (f32)viewport.getWidth() / (f32)viewport.getHeight();

	recalculateViewMatrix();
	recalculateProjectionMatrix();
	recalculateFrustum();
}

void CCamera::recalculateViewMatrix()
{
	ViewMatrix.buildCameraLookAtMatrixLH(Position, Position + Dir, Up);

	ViewMatrix.getInverse(InverseViewMatrix);

	Up.normalize();
	Right.set(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);
	Right.normalize();
}

void CCamera::recalculateProjectionMatrix()
{
	if (IsOrthogonal)
		ProjectionMatrix.buildProjectionMatrixOrthoFovLH(FOV, AspectRatio, NearValue, FarValue);
	else
		ProjectionMatrix.buildProjectionMatrixPerspectiveFovLH(FOV, AspectRatio, NearValue, FarValue);

	ClipProjectionMatrix.buildProjectionMatrixPerspectiveFovLH(FOV, AspectRatio, NearValue, NearValue + ClipDistance);
}

void CCamera::recalculateFrustum()
{
	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;
	ViewProjectionMatrix.getInverse(InverseViewProjectionMatrix);

	ViewFrustum.setFrom(ViewProjectionMatrix);

	frustum::makePlane(ViewMatrix * ClipProjectionMatrix, VF_FAR_PLANE, ClipPlane);
}

void CCamera::onKeyMove(f32 speed, const SKeyControl& keycontrol)
{
	//不改变摄像机方向
	if (keycontrol.front || keycontrol.back || keycontrol.left ||
		keycontrol.right || keycontrol.up || keycontrol.down)
	{
		vector3df dir(ViewMatrix[2], ViewMatrix[6], ViewMatrix[10]);
		vector3df up(ViewMatrix[1], ViewMatrix[5], ViewMatrix[9]);
		vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

		if (keycontrol.front) Position += dir * speed;
		else if (keycontrol.back) Position -= dir * speed;

		if (keycontrol.left) Position -= right * speed;
		else if (keycontrol.right)  Position += right * speed;

		if (keycontrol.up) Position += up * speed;
		else if (keycontrol.down) Position -= up * speed;

		recalculateViewMatrix();
		recalculateFrustum();
	}
}

void CCamera::pitch_yaw_Maya(f32 pitchDegree, f32 yawDegree)
{
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	quaternion quatX(pitchDegree * DEGTORAD, right);			//euler和左手坐标系相反
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	vector3df oppDir = -Dir;
	oppDir = q * oppDir;

	f32 d = -oppDir.dotProduct(vector3df::UnitY());
	if (abs_(d) > 0.99f)
	{
		q = quatY;
		oppDir = -Dir;
		oppDir = q * oppDir;
	}

	Position = LookAt + q * (Position - LookAt);
	Dir = -oppDir;

	recalculateViewMatrix();
	recalculateFrustum();
}

void CCamera::move_offset_Maya(f32 xOffset, f32 yOffset)
{
	vector3df up(ViewMatrix[1], ViewMatrix[5], ViewMatrix[9]);
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	vector3df offset = -right * xOffset + up * yOffset;
	Position += offset;

	recalculateViewMatrix();
	recalculateFrustum();
}

void CCamera::pitch_yaw_FPS(f32 pitchDegree, f32 yawDegree)
{
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	quaternion quatX(pitchDegree * DEGTORAD, right);
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	vector3df v = q * Dir;
	f32 d = v.dotProduct(vector3df::UnitY());
	if (abs_(d) > 0.99f)
	{
		v = quatY * Dir;
	}

	Dir = v;

	recalculateViewMatrix();
	recalculateFrustum();
}

vector2di CCamera::getScreenPositionFrom3DPosition(const vector3df& pos, const recti& screensize)
{
	dimension2du dim(screensize.getWidth(), screensize.getHeight());
	dim.Width /= 2;
	dim.Height /= 2;

	const matrix4& trans = getViewProjectionMatrix();
	vector3df transformedPos = pos;
	f32 z;
	trans.transformVect(transformedPos, z);
	if (z < 0)
		return vector2di(-1, -1);

	const f32 zDiv = z == 0.0f ? 1.0f : reciprocal_(z);

	return vector2di(
		round32_(dim.Width * transformedPos.X * zDiv) + dim.Width,
		dim.Height - round32_(dim.Height * (transformedPos.Y * zDiv)));
}

line3df CCamera::getRayFromScreenPosition(const vector2di& pos, const recti& screensize)
{
	const matrix4& m = getInverseViewMatrix();

	f32 y = tan(FOV*0.5f);
	f32 x = AspectRatio*y;
	f32 fary = y * FarValue;
	f32 farx = x * FarValue;

	vector3df vLeftUp(-farx, fary, FarValue);
	m.transformVect(vLeftUp);
	vector3df vRightUp(farx, fary, FarValue);
	m.transformVect(vRightUp);
	vector3df vLeftDown(-farx, -fary, FarValue);
	m.transformVect(vLeftDown);

	vector3df farLeftUp = vLeftUp;
	vector3df left2right = vRightUp - farLeftUp;
	vector3df up2down = vLeftDown - farLeftUp;

	f32 dx = pos.X / (f32)screensize.getWidth();
	f32 dy = pos.Y / (f32)screensize.getHeight();

	line3df ln;

	if (IsOrthogonal)
		ln.start = Position + left2right * (dx - 0.5f) + up2down * (dy - 0.5f);
	else
		ln.start = Position;

	ln.end = farLeftUp + left2right * dx + up2down * dy;

	return ln;
}

vector3df CCamera::get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, f32 distance)
{
	line3df ln = getRayFromScreenPosition(pos, screensize);

	f32 len = ln.getLength();

	return (ln.end - ln.start) * (distance / len) + ln.start;
}