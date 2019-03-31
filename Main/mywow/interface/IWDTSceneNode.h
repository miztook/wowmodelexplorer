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
	virtual void setCurrentTile(int32_t row, int32_t col) = 0;
	virtual vector3df getCenter() const = 0;
	virtual bool getHeightNormal(float x, float z, float* height, vector3df* normal) const = 0;
	virtual vector3df getPositionByTile(int32_t row, int32_t col) const = 0;
	virtual bool getTileByPosition(vector3df pos, int32_t& row, int32_t& col) const= 0;
	virtual void setAdtLoadSize(E_ADT_LOAD size) = 0;
};