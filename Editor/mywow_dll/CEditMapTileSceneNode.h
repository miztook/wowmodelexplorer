#pragma once

#include "CMapTileSceneNode.h"
#include "editor_structs.h"

class CEditMapTileSceneNode : public CMapTileSceneNode
{
public:
	CEditMapTileSceneNode(IFileWDT* wdt, STile* tile, ISceneNode* parent);
	virtual ~CEditMapTileSceneNode();

public:

};