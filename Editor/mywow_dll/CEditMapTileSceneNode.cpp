#include "stdafx.h"
#include "CEditMapTileSceneNode.h"
#include "CFileWDT.h"

CEditMapTileSceneNode::CEditMapTileSceneNode( IFileWDT* wdt, STile* tile, ISceneNode* parent )
	: CMapTileSceneNode(wdt, tile, parent)
{

}

CEditMapTileSceneNode::~CEditMapTileSceneNode()
{

}
