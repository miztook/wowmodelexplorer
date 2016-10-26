#include "stdafx.h"
#include "editor_MapTileSceneNode.h"
#include "CEditMapTileSceneNode.h"
#include "CFileWDT.h"

void MapTileSceneNode_getCenter( IMapTileSceneNode* node, vector3df* pos )
{
	CEditMapTileSceneNode* editNode = (CEditMapTileSceneNode*)node;
	*pos = editNode->getCenter();
}
