#pragma once

#include "ISceneNode.h"
#include "IM2SceneNode.h"

class IParticleSystemSceneNode : public ISceneNode
{
public:
	IParticleSystemSceneNode(IM2SceneNode* parent)
		: ISceneNode(parent) {}

	virtual ~IParticleSystemSceneNode() {}

public:
	virtual void setEmitting(bool on) = 0;
	virtual void setAnimationFrame(u32 anim, u32 frame) = 0;
	virtual void setSpeed(f32 speed) = 0;

	virtual void setOwnView(matrix4* view) = 0;
	virtual void setOwnProjection(matrix4* projection) = 0;

	virtual void setWholeAlpha(bool enable, f32 val) = 0;
	virtual void setWholeColor(bool enable, SColor color) = 0;
};