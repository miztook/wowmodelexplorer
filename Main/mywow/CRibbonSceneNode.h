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
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;

	uint32_t onFillVertexBuffer(SVertex_PCT* vertices, uint32_t vertexCount) const;

private:
	SMaterial		Material;

	CRibbonEmitterServices*		RibbonEmitterServices;
	const wow_m2instance*		Character;
	RibbonEmitter*	Re;
	ITexture*		Texture;
	LENTRY	LiveSegmentList;
	uint32_t		LiveSegmentCount;

	vector3df		tpos;
	SColorf		tcolor;
	float	tabove;
	float	tbelow;

};