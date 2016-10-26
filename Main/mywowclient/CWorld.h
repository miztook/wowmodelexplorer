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
	virtual f32 getHeightNormal(f32 x, f32 z, vector3df* normal = NULL) const;

	virtual void tick(u32 delta);

	virtual void setWorldSceneNode(IMapTileSceneNode* node);

	virtual void setWorldSceneNode(IWDTSceneNode* node);

private:
	IMapTileSceneNode*		MapTileSceneNode;
	IWDTSceneNode*	WdtSceneNode;
};