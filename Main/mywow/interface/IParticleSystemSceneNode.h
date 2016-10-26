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
	virtual u32 onFillVertexBuffer(SVertex_PCT* vertices, u32 vcount) = 0;
#endif

	virtual void setEmitting(bool on) = 0;
	virtual void setAnimationFrame(u32 anim, u32 frame) = 0;
	virtual void setSpeed(f32 speed) = 0;

	virtual void setOwnView(matrix4* view) = 0;
	virtual void setOwnProjection(matrix4* projection) = 0;

	virtual void setWholeAlpha(bool enable, f32 val) = 0;
	virtual void setWholeColor(bool enable, SColor color) = 0;

	virtual void setEmitDensity(f32 density) = 0;
};