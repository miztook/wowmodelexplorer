#pragma once

#include "ISceneNode.h"
#include "IM2SceneNode.h"

class IRibbonSceneNode : public ISceneNode
{
public:
	IRibbonSceneNode(ISceneNode* parent)
		: ISceneNode(parent) {}

	virtual ~IRibbonSceneNode() {}
};