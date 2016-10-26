#pragma once

#include "ISceneNode.h"

//负责map中的天空，光照，雾等环境设置
class ISkySceneNode : public ISceneNode
{
public:
	ISkySceneNode() : ISceneNode(NULL_PTR){ Type = EST_SKY; }

	virtual ~ISkySceneNode() { }

public:

};