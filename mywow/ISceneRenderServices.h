#pragma once

#include "core.h"

class ISceneNode;
class ICamera;
struct SRenderUnit;

//先按场景结点排序分类，再按material类型分类
class ISceneRenderServices
{
public:
	virtual ~ISceneRenderServices() 	{}

public:
	virtual void addSceneNode(ISceneNode* node) = 0;
	virtual void tickAllSceneNodes(u32 timeSinceStart, u32 timeSinceLastFrame) = 0;
	virtual void renderAllSceneNodes() = 0;

	virtual void addRenderUnit(const SRenderUnit* unit, E_RENDERINST_TYPE type) = 0;
	virtual void renderAll(E_RENDERINST_TYPE type, ICamera* cam) = 0;

	virtual void begin_setupLightFog(E_RENDERINST_TYPE type, ICamera* cam) = 0;
	virtual void end_setupLightFog(E_RENDERINST_TYPE type) = 0;

	virtual SRenderUnit* getCurrentUnit() const = 0; 

	virtual void setModelLodBias(s32 lodbias) = 0;
	virtual s32 getModelLodBias() const = 0;

	virtual void setTerrainLodBias(s32 lodbias) = 0;
	virtual s32 getTerrainLodBias() const = 0;

	virtual void setObjectVisibleDistance(f32 distance) = 0;
	virtual f32 getObjectVisibleDistance() const = 0;
	virtual f32 getObjectAlphaDistance() const = 0;

	virtual f32 getClipDistance() const = 0;
	virtual void setClipDistance(f32 distance) = 0;
};
