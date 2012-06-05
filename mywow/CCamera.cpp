#include "stdafx.h"
#include "CCamera.h"
#include "mywow.h"

CCamera::CCamera( const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov )
{
	Position = position;
	Dir = (lookat - position).normalize();
	Up = up;
	NearValue = nearValue;
	FarValue = farValue;
	FOV = fov;

	recalculateAll();
}


void CCamera::recalculateAll()
{
	recti viewport = g_Engine->getDriver()->getViewPort();
	AspectRatio = (f32)viewport.getWidth() / (f32)viewport.getHeight();

	recalculateViewMatrix();
	recalculateProjectionMatrix(IsOrthogonal);
	recalculateFrustum();
}

void CCamera::recalculateViewMatrix()
{
	ViewMatrix.buildCameraLookAtMatrixLH(Position, Position+Dir, Up);

	ViewMatrix.getInverse(InverseViewMatrix);
}

void CCamera::recalculateProjectionMatrix( bool orthogonal )
{
	if (orthogonal)
		ProjectionMatrix.buildProjectionMatrixOrthoFovLH( FOV, AspectRatio, NearValue, FarValue );
	else
		ProjectionMatrix.buildProjectionMatrixPerspectiveFovLH( FOV, AspectRatio, NearValue, FarValue );
}

void CCamera::recalculateFrustum()
{
	ViewFrustum.setFrom( ProjectionMatrix * ViewMatrix );
}

void CCamera::onKeyMove( f32 speed, SKeyControl keycontrol )
{
	//不改变摄像机方向
	if ( keycontrol.front || keycontrol.back || keycontrol.left ||
		keycontrol.right || keycontrol.up || keycontrol.down)
	{
		vector3df dir( ViewMatrix[2], ViewMatrix[6], ViewMatrix[10] );
		vector3df up( ViewMatrix[1], ViewMatrix[5], ViewMatrix[9] );
		vector3df right( ViewMatrix[0], ViewMatrix[4], ViewMatrix[8] );
	
		if( keycontrol.front ) Position += dir * speed;
		else if( keycontrol.back ) Position -= dir * speed;

		if( keycontrol.left ) Position -= right * speed; 
		else if( keycontrol.right )  Position += right * speed; 

		if (keycontrol.up) Position += up * speed;
		else if(keycontrol.down) Position -= up * speed;

		recalculateViewMatrix();
		recalculateFrustum();
	}
}

void CCamera::pitch_yaw_Maya( f32 pitchDegree, f32 yawDegree )
{
	vector3df right( ViewMatrix[0], ViewMatrix[4], ViewMatrix[8] );

	quaternion quatX(pitchDegree * DEGTORAD, right);			//euler和左手坐标系相反 
	quaternion quatY(yawDegree * DEGTORAD, vector3df(0,1,0));
	quaternion q = (quatX * quatY);

	vector3df oppDir = -Dir;
	oppDir = q * oppDir;
	Position = q * Position;
	Dir = -oppDir;

	Up = q * Up;						//改变up

	recalculateViewMatrix();
	recalculateFrustum();
}

void CCamera::move_offset_Maya( f32 xOffset, f32 yOffset )
{
	vector3df up( ViewMatrix[1], ViewMatrix[5], ViewMatrix[9] );
	vector3df right( ViewMatrix[0], ViewMatrix[4], ViewMatrix[8] );

	vector3df offset = -right * xOffset + up * yOffset;
	Position += offset;
	//LookAt += offset;

	recalculateViewMatrix();
	recalculateFrustum();
}

void CCamera::pitch_yaw_FPS( f32 pitchDegree, f32 yawDegree )
{
	vector3df right( ViewMatrix[0], ViewMatrix[4], ViewMatrix[8] );

	quaternion quatX(pitchDegree * DEGTORAD, right);			//euler和左手坐标系相反 
	quaternion quatY(yawDegree * DEGTORAD, vector3df(0,1,0));
	quaternion q = (quatX * quatY);

	Dir = q * Dir;
	Up = q * Up;

	recalculateViewMatrix();
	recalculateFrustum();
}

/*
void CCamera::makeClipPlane( const plane3df& plane, plane3df& clip )
{
	clip = plane;

	matrix4 mat ;
	ViewMatrix.getInverse(mat);
	mat.transpose();
	mat.transformPlane(clip);

	ProjectionMatrix.getInverse(mat);
	mat.transpose();
	mat.transformPlane(clip);
}
*/