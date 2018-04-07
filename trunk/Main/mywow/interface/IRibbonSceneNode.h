#pragma once

#include "ISceneNode.h"
#include "IM2SceneNode.h"

struct SVertex_PCT;

class IRibbonSceneNode : public ISceneNode
{
public:
	explicit IRibbonSceneNode(ISceneNode* parent)
		: ISceneNode(parent) { Type = EST_RIBBON; }

	virtual ~IRibbonSceneNode() { }

public:
};