#pragma once

#include "IRibbonSceneNode.h"
#include "wow_particle.h"
#include "SMaterial.h"

class ISceneManager;
class IRibbonEmitterServices;

class CRibbonSceneNode : public IRibbonSceneNode
{
public:
	CRibbonSceneNode(RibbonEmitter* re, IM2SceneNode* parent);
	virtual ~CRibbonSceneNode();

public:
	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck = true);
	virtual aabbox3df getBoundingBox() const { return aabbox3df(); }
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame);
	virtual void render();
	virtual void onPreRender();

private:
	ISceneManager*		SceneManager;
	IRibbonEmitterServices*		RibbonEmitterServices;

	wow_character*		Character;
	typedef std::list<RibbonSegment*, qzone_allocator<RibbonSegment*>>	T_LiveSegments;
	T_LiveSegments		LiveSegments;

	SMaterial		Material;
	ITexture*		Texture;

	RibbonEmitter*	Re;

	vector3df		tpos;
	SColorf		tcolor;
	float	tabove, tbelow;

};