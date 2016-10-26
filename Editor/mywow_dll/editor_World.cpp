#include "stdafx.h"
#include "editor_World.h"

f32 World_getHeightNormal( IWorld* world, f32 x, f32 z, vector3df* normal )
{
	return world->getHeightNormal(x, z, normal);
}

void World_setMapTileSceneNode( IWorld* world, IMapTileSceneNode* node )
{
	world->setWorldSceneNode(node);
}

void World_setWdtSceneNode( IWorld* world, IWDTSceneNode* node )
{
	world->setWorldSceneNode(node);
}
