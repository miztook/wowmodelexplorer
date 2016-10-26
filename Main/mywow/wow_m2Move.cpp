#include "stdafx.h"
#include "wow_m2Move.h"
#include "mywow.h"


wow_m2Move::wow_m2Move( IM2SceneNode* m2Node )
{
	M2SceneNode = m2Node;
	Height = 0.0f;
}

wow_m2Move::~wow_m2Move()
{

}

vector3df wow_m2Move::getDir() const
{
	quaternion q(PI/2, vector3df::UnitY());
	vector3df v = M2SceneNode->getDir();
	return q * v;
}

void wow_m2Move::setDir( const vector3df& dir )
{
	quaternion q(-PI/2, vector3df::UnitY());
	vector3df vDir = q * dir;
	M2SceneNode->setDir(vDir);
}

vector3df wow_m2Move::getPos() const
{
	return M2SceneNode->getPos();
}

void wow_m2Move::setPos( const vector3df& pos )
{
	M2SceneNode->setPos(pos);
}

vector3df wow_m2Move::getUp() const
{
	return M2SceneNode->getUp();
}

void wow_m2Move::setUp(const vector3df& up)
{
	M2SceneNode->setUp(up);
}

vector3df wow_m2Move::getScale() const
{
	return M2SceneNode->getScale();
}

void wow_m2Move::setScale( const vector3df& scale )
{
	M2SceneNode->setScale(scale);
}

void wow_m2Move::rotate( const quaternion& q )
{
	quaternion q0(PI/2, vector3df::UnitY());
	vector3df v = M2SceneNode->getDir();
	vector3df vDir = (q + q0) * v;
	M2SceneNode->setDir(vDir);
}

void wow_m2Move::rotateAxisY( f32 radians )
{
	quaternion q(radians, vector3df::UnitY());
	rotate(q);
}

void wow_m2Move::move( const vector3df& offset )
{
	setPos(getPos() + offset);
}


