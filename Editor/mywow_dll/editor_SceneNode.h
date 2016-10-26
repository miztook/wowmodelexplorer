#pragma once

#include "editor_base.h"
#include "matrix4.h"

MW_API void  SceneNode_setVisible(ISceneNode* sceneNode, bool visible);
MW_API bool  SceneNode_getVisible(ISceneNode* sceneNode);

MW_API void  SceneNode_setRelativeTransformation(ISceneNode* sceneNode, const matrix4& mat);
MW_API void  SceneNode_getRelativeTransformation(ISceneNode* sceneNode, matrix4* mat);
MW_API void SceneNode_update(ISceneNode* sceneNode, bool includeChildren);