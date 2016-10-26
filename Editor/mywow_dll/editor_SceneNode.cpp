#include "stdafx.h"
#include "editor_SceneNode.h"

void  SceneNode_setVisible( ISceneNode* sceneNode, bool visible )
{
	sceneNode->Visible = visible;
}

bool  SceneNode_getVisible( ISceneNode* sceneNode )
{
	return sceneNode->Visible;
}

void  SceneNode_setRelativeTransformation( ISceneNode* sceneNode, const matrix4& mat )
{
	sceneNode->setRelativeTransformation(mat);
}

void SceneNode_getRelativeTransformation( ISceneNode* sceneNode, matrix4* mat )
{
	*mat = sceneNode->getRelativeTransformation();
}

void SceneNode_update( ISceneNode* sceneNode, bool includeChildren )
{
	sceneNode->NeedUpdate = true;
	sceneNode->update(includeChildren);
}
