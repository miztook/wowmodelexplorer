#include "StdAfx.h"
#include "CWorld.h"
#include "mywow.h"

CWorld::CWorld()
{
	MapTileSceneNode = NULL;
	WdtSceneNode = NULL;
}

CWorld::~CWorld()
{

}

void CWorld::tick( u32 delta )
{

}

void CWorld::setWorldSceneNode( IMapTileSceneNode* node )
{
	MapTileSceneNode = node;
	WdtSceneNode = NULL;
}

void CWorld::setWorldSceneNode( IWDTSceneNode* node )
{
	WdtSceneNode = node;
	MapTileSceneNode = NULL;
}

f32 CWorld::getHeightNormal( f32 x, f32 z, vector3df* normal ) const
{
	f32 height;
	if (MapTileSceneNode && MapTileSceneNode->getHeightNormal(x, z, &height, normal))
		return height;

	if (WdtSceneNode && WdtSceneNode->getHeightNormal(x, z, &height, normal))
		return height;

	if(normal)
		*normal = vector3df::UnitY();
	return 0.0f;
}
