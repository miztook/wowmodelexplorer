#include "stdafx.h"
#include "editor_m2Move.h"

void m2Move_getDir( wow_m2Move* move, vector3df* dir )
{
	*dir = move->getDir();
}

void m2Move_setDir( wow_m2Move* move, const vector3df& dir )
{
	move->setDir(dir);
}

void m2Move_getPos( wow_m2Move* move, vector3df* pos )
{
	*pos = move->getPos();
}

void m2Move_setPos( wow_m2Move* move, const vector3df& pos )
{
	move->setPos(pos);
}

void m2Move_getScale( wow_m2Move* move, vector3df* scale )
{
	*scale = move->getScale();
}

void m2Move_setScale( wow_m2Move* move, const vector3df& scale )
{
	move->setScale(scale);
}
