#pragma once

#include "ISceneNode.h"
#include "IM2SceneNode.h"

struct SVertex_PCT;

class IParticleSystemSceneNode : public ISceneNode
{
public:
	explicit IParticleSystemSceneNode(IM2SceneNode* parent)
		: ISceneNode(parent) { Type = EST_PARTICLE; }

	virtual ~IParticleSystemSceneNode() { }

public:

#ifdef FULL_INTERFACE
	virtual uint32_t onFillVertexBuffer(SVertex_PCT* vertices, uint32_t vcount) = 0;
#endif

	virtual void setEmitting(bool on) = 0;
	virtual void setAnimationFrame(uint32_t anim, uint32_t frame) = 0;
	virtual void setSpeed(float speed) = 0;

	virtual void setOwnView(matrix4* view) = 0;
	virtual void setOwnProjection(matrix4* projection) = 0;

	virtual void setWholeAlpha(bool enable, float val) = 0;
	virtual void setWholeColor(bool enable, SColor color) = 0;

	virtual void setEmitDensity(float density) = 0;
};