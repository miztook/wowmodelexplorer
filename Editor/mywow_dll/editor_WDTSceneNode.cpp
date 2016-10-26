#include "stdafx.h"
#include "editor_WDTSceneNode.h"
#include "CEditWDTSceneNode.h"
#include "CFileWDT.h"

void WDTSceneNode_setCurrentTile( IWDTSceneNode* node, s32 row, s32 col )
{
	CEditWDTSceneNode* editNode = (CEditWDTSceneNode*)node;
	editNode->setCurrentTile(row, col);
}

void WDTSceneNode_getCenter( IWDTSceneNode* node, vector3df* pos )
{
	CEditWDTSceneNode* editNode = (CEditWDTSceneNode*)node;
	*pos = editNode->getCenter();
}
