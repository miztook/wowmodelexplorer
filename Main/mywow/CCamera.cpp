#include "stdafx.h"
#include "CCamera.h"
#include "mywow.h"

CCamera::CCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, float nearValue, float farValue, float fov) : ICamera(position, lookat, up, nearValue, farValue, fov)
{
	recalculateAll();
}

void CCamera::recalculateAll()
{
	recti viewport = g_Engine->getDriver()->getViewPort();
	AspectRatio = (float)viewport.getWidth() / (float)viewport.getHeight();

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

void CCamera::onKeyMove(float speed, const SKeyControl& keycontrol)
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

void CCamera::pitch_yaw_Maya(float pitchDegree, float yawDegree)
{
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	quaternion quatX(pitchDegree * DEGTORAD, right);			//euler和左手坐标系相反
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	vector3df oppDir = -Dir;
	oppDir = q * oppDir;

	float d = -oppDir.dotProduct(vector3df::UnitY());
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

void CCamera::move_offset_Maya(float xOffset, float yOffset)
{
	vector3df up(ViewMatrix[1], ViewMatrix[5], ViewMatrix[9]);
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	vector3df offset = -right * xOffset + up * yOffset;
	Position += offset;

	recalculateViewMatrix();
	recalculateFrustum();
}

void CCamera::pitch_yaw_FPS(float pitchDegree, float yawDegree)
{
	vector3df right(ViewMatrix[0], ViewMatrix[4], ViewMatrix[8]);

	quaternion quatX(pitchDegree * DEGTORAD, right);
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	vector3df v = q * Dir;
	float d = v.dotProduct(vector3df::UnitY());
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
	float z;
	trans.transformVect(transformedPos, z);
	if (z < 0)
		return vector2di(-1, -1);

	const float zDiv = z == 0.0f ? 1.0f : reciprocal_(z);

	return vector2di(
		round32_(dim.Width * transformedPos.X * zDiv) + dim.Width,
		dim.Height - round32_(dim.Height * (transformedPos.Y * zDiv)));
}

line3df CCamera::getRayFromScreenPosition(const vector2di& pos, const recti& screensize)
{
	const matrix4& m = getInverseViewMatrix();

	float y = tan(FOV*0.5f);
	float x = AspectRatio*y;
	float fary = y * FarValue;
	float farx = x * FarValue;

	vector3df vLeftUp(-farx, fary, FarValue);
	m.transformVect(vLeftUp);
	vector3df vRightUp(farx, fary, FarValue);
	m.transformVect(vRightUp);
	vector3df vLeftDown(-farx, -fary, FarValue);
	m.transformVect(vLeftDown);

	vector3df farLeftUp = vLeftUp;
	vector3df left2right = vRightUp - farLeftUp;
	vector3df up2down = vLeftDown - farLeftUp;

	float dx = pos.X / (float)screensize.getWidth();
	float dy = pos.Y / (float)screensize.getHeight();

	line3df ln;

	if (IsOrthogonal)
		ln.start = Position + left2right * (dx - 0.5f) + up2down * (dy - 0.5f);
	else
		ln.start = Position;

	ln.end = farLeftUp + left2right * dx + up2down * dy;

	return ln;
}

vector3df CCamera::get3DPositionFromScreenPosition(const vector2di& pos, const recti& screensize, float distance)
{
	line3df ln = getRayFromScreenPosition(pos, screensize);

	float len = ln.getLength();

	return (ln.end - ln.start) * (distance / len) + ln.start;
}