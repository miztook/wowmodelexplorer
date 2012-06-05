#pragma once

#include "ISceneNode.h"
#include "IM2SceneNode.h"

class IRibbonSceneNode : public ISceneNode
{
public:
	IRibbonSceneNode(ISceneNode* parent, E_RENDERINST_TYPE renderType = ERT_EFFECT)
		: ISceneNode(parent, renderType) {}

	virtual ~IRibbonSceneNode() {}
};