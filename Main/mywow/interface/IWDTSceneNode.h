#pragma once

#include "ISceneNode.h"

struct STile;
class CMapEnvironment;

//地图，动态加载tile，场景
class IWDTSceneNode : public ISceneNode
{
public:
	explicit IWDTSceneNode(ISceneNode* parent) : ISceneNode(parent) {  Type = EST_WDT; }

	virtual ~IWDTSceneNode() { }

public:
	virtual void setCurrentTile(s32 row, s32 col) = 0;
	virtual vector3df getCenter() const = 0;
	virtual bool getHeightNormal(f32 x, f32 z, f32* height, vector3df* normal) const = 0;
	virtual vector3df getPositionByTile(s32 row, s32 col) const = 0;
	virtual bool getTileByPosition(vector3df pos, s32& row, s32& col) const= 0;
	virtual void setAdtLoadSize(E_ADT_LOAD size) = 0;
};