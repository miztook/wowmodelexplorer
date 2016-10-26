#include "stdafx.h"
#include "wow_m2TargetCamera.h"
#include "wow_m2Logic.h"

#define ANGLE_SPEED 0.001f
#define ADJUSTING_SPEED 0.02f
#define WHEEL_SPEED 0.5f
#define LOOK_POINT	0.7f

wow_m2TargetCamera::wow_m2TargetCamera(f32 nearValue, f32 farValue, f32 fov)
	: M2Node(NULL), 
	CurrentDistance(0), CurrentHeight(0), 
	MaxDistance(35), MinDistance(1), 
	NeedAdjustDir(false), AdjustRight(true)
{
	Camera = g_Engine->getSceneManager()->addCamera(vector3df(1,1,1), vector3df::Zero(), vector3df::UnitY(), nearValue, farValue, fov);
}


wow_m2TargetCamera::~wow_m2TargetCamera()
{
	g_Engine->getSceneManager()->removeCamera(Camera);
}

void wow_m2TargetCamera::setMinMaxDistance( f32 minDistance, f32 maxDistance )
{
	MaxDistance = maxDistance;
	MinDistance = minDistance;
}

void wow_m2TargetCamera::setM2AsTarget( IM2SceneNode* node, f32 distance, f32 rad )
{
	M2Node = node;

	if (!M2Node)
		return;

	wow_m2Move* move = M2Node->getM2Move();
	if (!move)
		return;

	CurrentHeight = M2Node->getWorldBoundingBox().getExtent().Y * LOOK_POINT;

	f32 horizon = distance * cosf(rad);
	f32 vertical = distance * sinf(rad);

	vector3df camRelative = -move->getDir() * horizon + vector3df::UnitY() * vertical;
	Camera->setPosition(move->getPos() + vector3df(0, CurrentHeight, 0) + camRelative);
	Camera->setDir(-camRelative);

	Camera->recalculateAll();

	CurrentDistance = distance;

	//adjust dir
	DestDir = M2Node->getM2Move()->getDir();
	NeedAdjustDir = false;
}

void wow_m2TargetCamera::setM2AsTarget( IM2SceneNode* node )
{
	if (!M2Node)
		return;

	M2Node = node;

	if (!M2Node)
		return;

	wow_m2Move* move = M2Node->getM2Move();
	if (!move)
		return;

	CurrentHeight = M2Node->getWorldBoundingBox().getExtent().Y * LOOK_POINT;
	Camera->setPosition(move->getPos() + vector3df(0, CurrentHeight, 0) - Camera->getDir() * CurrentDistance);

	Camera->recalculateAll();

	//adjust dir
	DestDir = M2Node->getM2Move()->getDir();
	NeedAdjustDir = false;
}

void wow_m2TargetCamera::tick(u32 delta)
{
	_ASSERT(M2Node);

	//dir adjust
	if (NeedAdjustDir)
	{
		vector3df dir = M2Node->getM2Move()->getDir();
		f32 dot = DestDir.dotProduct(dir);
		if ( dot > 0.9995f)		//same
		{
			M2Node->getM2Move()->setDir(DestDir);
			NeedAdjustDir = false;
		}
		else if (dot < -0.9995f)	//oppsite
		{
			quaternion q(delta * ADJUSTING_SPEED, AdjustRight ? vector3df::UnitY() : -vector3df::UnitY());
			dir = q * dir;
			M2Node->getM2Move()->setDir(dir);
		}
		else
		{
			vector3df up = dir.crossProduct(DestDir);
			bool right = up.Y >= 0;

			if (right != AdjustRight)
			{
				M2Node->getM2Move()->setDir(DestDir);
				NeedAdjustDir = false;
			}
			else
			{
				quaternion q(delta * ADJUSTING_SPEED, up);
				dir = q * dir;
				M2Node->getM2Move()->setDir(dir);
			}
		}
	}
	
	//height adjust
	f32 height = M2Node->getWorldBoundingBox().getExtent().Y * LOOK_POINT;
	if (height != CurrentHeight)
	{
		Camera->setPosition(Camera->getPosition() + vector3df(0, height - CurrentHeight, 0));
		Camera->recalculateAll();

		CurrentHeight = height;
	}

}

void wow_m2TargetCamera::onMouseWheel( f32 fDelta )
{
	f32 distance = CurrentDistance;
	f32 delta = WHEEL_SPEED * fabs(fDelta);
	if (fDelta > 0)
		distance -= delta;
	else
		distance += delta;

	if (distance < MaxDistance && distance > MinDistance)
	{
		ICamera::SKeyControl keyControl = {0};
		keyControl.front = fDelta > 0.0f;
		keyControl.back = fDelta < 0.0f;

		Camera->onKeyMove(delta, keyControl);

		CurrentDistance = distance;
	}
}

void wow_m2TargetCamera::makeTargetFollowCamera(u32 deltaTime, bool front, bool back, bool left, bool right)
{
	vector3df dir = Camera->getDir();

	if ((front && back) || (left && right))
	{
		setDestDir(vector3df(dir.X, 0, dir.Z));
		return;
	}

	if (front)
	{
		if (left)
		{
			quaternion q(-PI/4, vector3df::UnitY());
			dir = q * dir;
		}
		else if (right)
		{
			quaternion q(PI/4, vector3df::UnitY());
			dir = q * dir;
		}
	}
	else if (back)
	{
		if (left)
		{
			quaternion q(PI/4, vector3df::UnitY());
			dir = q * dir;
		}
		else if (right)
		{
			quaternion q(-PI/4, vector3df::UnitY());
			dir = q * dir;
		}
	}
	else if (left)
	{
		quaternion q(-PI/2, vector3df::UnitY());
		dir = q * dir;
	}
	else if (right)
	{
		quaternion q(PI/2, vector3df::UnitY());
		dir = q * dir;
	}

	setDestDir(vector3df(dir.X, 0, dir.Z));
}

void wow_m2TargetCamera::makeCameraFollowTarget(u32 deltaTime, bool front, bool back, bool left, bool right)
{
	if ((front && back) || (left && right))
		return;

	vector3df dir = Camera->getDir();
	vector3df target = M2Node->getM2Move()->getPos() + vector3df(0, CurrentHeight, 0);

	vector3df oppDir = -dir;

	if (left)
	{
		quaternion q(deltaTime * ANGLE_SPEED, -vector3df::UnitY());
		oppDir = q * oppDir;
	}
	else if (right)
	{
		quaternion q(deltaTime * ANGLE_SPEED, vector3df::UnitY());
		oppDir = q * oppDir;
	}
	else
	{
		return;
	}

	Camera->setPosition(target + oppDir * CurrentDistance);
	Camera->setDir(-oppDir);

	Camera->recalculateAll();

	setDestDir(vector3df(Camera->getDir().X, 0, Camera->getDir().Z));
}

void wow_m2TargetCamera::onMouseMove( f32 pitchDegree, f32 yawDegree )
{
	vector3df right = Camera->getRight();
	vector3df dir = Camera->getDir();
	vector3df target = M2Node->getM2Move()->getPos() + vector3df(0, CurrentHeight, 0);

	quaternion quatX(pitchDegree * DEGTORAD, right);			//euler和左手坐标系相反 
	quaternion quatY(yawDegree * DEGTORAD, vector3df::UnitY());
	quaternion q = (quatX * quatY);

	vector3df oppDir = -dir;
	oppDir = q * oppDir;

	f32 d = -oppDir.dotProduct(vector3df::UnitY());
	if (abs_(d) > 0.99f)
	{
		q = quatY;
		oppDir = -dir;
		oppDir = q * oppDir;
	}

	Camera->setPosition(target + oppDir * CurrentDistance);
	Camera->setDir(-oppDir);

	Camera->recalculateAll();
}

void wow_m2TargetCamera::setDestDir( const vector3df& dir )
{
	DestDir = dir;
	DestDir.normalize();
	NeedAdjustDir = true;

	vector3df m2Dir = M2Node->getM2Move()->getDir();

	f32 dot = DestDir.dotProduct(m2Dir);
	if ( dot > 0.9995f)		//same
	{
		M2Node->getM2Move()->setDir(DestDir);
		NeedAdjustDir = false;
	}
	else if (dot < -0.9995f)	//oppsite
	{
		vector3df turnUp = m2Dir.crossProduct(Camera->getDir().getXZ());
		AdjustRight = turnUp.Y >= 0;
	}
	else
	{
		vector3df up = m2Dir.crossProduct(DestDir);
		AdjustRight = up.Y >= 0;
	}
}

void wow_m2TargetCamera::onTargetMove()
{
	wow_m2Move* move = M2Node->getM2Move();
	Camera->setPosition(move->getPos() + vector3df(0, CurrentHeight, 0) - Camera->getDir() * CurrentDistance);

	Camera->recalculateAll();
}
