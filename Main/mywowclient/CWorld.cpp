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

void CWorld::tick( uint32_t delta )
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

float CWorld::getHeightNormal( float x, float z, vector3df* normal ) const
{
	float height;
	if (MapTileSceneNode && MapTileSceneNode->getHeightNormal(x, z, &height, normal))
		return height;

	if (WdtSceneNode && WdtSceneNode->getHeightNormal(x, z, &height, normal))
		return height;

	if(normal)
		*normal = vector3df::UnitY();
	return 0.0f;
}
