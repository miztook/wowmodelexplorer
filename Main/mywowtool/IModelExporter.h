#pragma once

#include "base.h"

class IM2SceneNode;
class IWMOSceneNode;

class IModelExporter
{
public:
	virtual bool exportM2SceneNode( IM2SceneNode* node, const char* filename ) = 0;
	virtual bool exportWMOSceneNode( IWMOSceneNode* node, const char* filename ) = 0;
	virtual bool exportWMOSceneNodeGroups( IWMOSceneNode* node, const char* filename) = 0;
};