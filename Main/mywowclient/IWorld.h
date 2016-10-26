#pragma once

#include "base.h"

class IMapTileSceneNode;
class IWDTSceneNode;

class IWorld
{
public:
	virtual ~IWorld() {}

public:
	virtual f32 getHeightNormal(f32 x, f32 z, vector3df* normal = NULL) const = 0;

	virtual void tick(u32 delta) = 0;

	virtual void setWorldSceneNode(IMapTileSceneNode* node) = 0;

	virtual void setWorldSceneNode(IWDTSceneNode* node) = 0;
};