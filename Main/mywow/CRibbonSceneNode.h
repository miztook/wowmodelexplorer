#pragma once

#include "IRibbonSceneNode.h"
#include "wow_particle.h"
#include "SMaterial.h"

class CRibbonEmitterServices;

class CRibbonSceneNode : public IRibbonSceneNode
{
public:
	CRibbonSceneNode(RibbonEmitter* re, IM2SceneNode* parent);
	virtual ~CRibbonSceneNode();

public:
	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;

	u32 onFillVertexBuffer(SVertex_PCT* vertices, u32 vertexCount) const;

private:
	SMaterial		Material;

	CRibbonEmitterServices*		RibbonEmitterServices;
	const wow_m2instance*		Character;
	RibbonEmitter*	Re;
	ITexture*		Texture;
	LENTRY	LiveSegmentList;
	u32		LiveSegmentCount;

	vector3df		tpos;
	SColorf		tcolor;
	f32	tabove;
	f32	tbelow;

};