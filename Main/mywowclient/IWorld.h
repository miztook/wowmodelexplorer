#pragma once

#include "base.h"

class IMapTileSceneNode;
class IWDTSceneNode;

class IWorld
{
public:
	virtual ~IWorld() {}

public:
	virtual float getHeightNormal(float x, float z, vector3df* normal = NULL) const = 0;

	virtual void tick(uint32_t delta) = 0;

	virtual void setWorldSceneNode(IMapTileSceneNode* node) = 0;

	virtual void setWorldSceneNode(IWDTSceneNode* node) = 0;
};