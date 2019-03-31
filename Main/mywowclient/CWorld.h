#pragma once

#include "IWorld.h"

class CWorld : public IWorld
{
private:
	DISALLOW_COPY_AND_ASSIGN(CWorld);

public:
	CWorld();

	~CWorld();

public:
	virtual float getHeightNormal(float x, float z, vector3df* normal = NULL) const;

	virtual void tick(uint32_t delta);

	virtual void setWorldSceneNode(IMapTileSceneNode* node);

	virtual void setWorldSceneNode(IWDTSceneNode* node);

private:
	IMapTileSceneNode*		MapTileSceneNode;
	IWDTSceneNode*	WdtSceneNode;
};