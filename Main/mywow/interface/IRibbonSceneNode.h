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

#ifdef FULL_INTERFACE
	virtual u32 onFillVertexBuffer(SVertex_PCT* vertices, u32 vertexCount) = 0;
#endif
};