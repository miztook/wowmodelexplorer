#pragma once

#include "core.h"

class ISceneNode;
class ICamera;
class ISceneRenderer;
struct SRenderUnit;

//先按场景结点排序分类，再按material类型分类
class ISceneRenderServices
{
public:
	ISceneRenderServices() : SCENE_SCALE(3.0f), CurrentUnit(NULL_PTR)
	{
		ClipDistance = 1000.0f;
		ModelLodBias = 0;
		TerrainLodBias = 0;
		ObjectVisibleDistance = 24;
		M2InvisibleTickDistance = 30;
		M2SlowTickStart = 0.5f;
		AdtLoadSize = EAL_3X3;
	}
	virtual ~ISceneRenderServices() 	{}

	//整个场景缩放比例： 地形，建筑，物体，天空
	const f32 SCENE_SCALE;

public:

#ifdef FULL_INTERFACE

	virtual void addSceneNode(ISceneNode* node, bool visible, int sequence) = 0;
	virtual void skipSceneNode(ISceneNode* node) = 0;

	virtual void clearAllSceneNodes() = 0;
	virtual void tickAllSceneNodes(u32 timeSinceStart, u32 timeSinceLastFrame, int sequence) = 0;
	virtual void renderAllSceneNodes() const = 0;

	virtual void addRenderUnit(const SRenderUnit* unit, E_RENDERINST_TYPE type) = 0;
	virtual void renderAll(E_RENDERINST_TYPE type, ICamera* cam) = 0;

#endif

	const SRenderUnit* getCurrentUnit() const { return CurrentUnit; }

	//scene
	void setModelLodBias(s32 lodbias) { ModelLodBias = lodbias; }
	s32 getModelLodBias() const { return ModelLodBias; }

	void setTerrainLodBias(s32 lodbias) { TerrainLodBias = lodbias; }
	s32 getTerrainLodBias() const { return TerrainLodBias; }

	//地形上小物体由可见 -> alpha ->不可见
	void setObjectVisibleDistance(f32 distance) { ObjectVisibleDistance = distance; }
	f32 getObjectVisibleDistance() const { return ObjectVisibleDistance; }
	f32 getObjectAlphaDistance() const { return ObjectVisibleDistance * 0.8f; }

	void setM2InvisibleTickDistance(f32 distance) { M2InvisibleTickDistance = distance; }
	f32 getM2InvisibleTickDistance() const { return M2InvisibleTickDistance; }
	f32 getM2SlowTickBegin() const { return M2SlowTickStart; }

	//被雾的clip距离
	f32 getClipDistance() const { return ClipDistance; }
	void setClipDistance(f32 distance) { ClipDistance = distance; }

	E_ADT_LOAD getAdtLoadSize() const { return AdtLoadSize; }
	void setAdtLoadSize(E_ADT_LOAD adt);

	void registerAdtLoadSizeChanged(IAdtLoadSizeChangedCallback* callback);
	void removeAdtLoadSizeChanged(IAdtLoadSizeChangedCallback* callback);

protected:
	s32		ModelLodBias;
	s32		TerrainLodBias;
	f32		ObjectVisibleDistance;		//距离/半径比例，超过此值时物体不可见
	f32		M2SlowTickStart;				//
	f32		M2InvisibleTickDistance;		//距离/半径比例, 超过此值时m2最慢tick
	f32		ClipDistance;
	E_ADT_LOAD		AdtLoadSize;

	const SRenderUnit*			CurrentUnit;

	typedef std::list<IAdtLoadSizeChangedCallback*, qzone_allocator<IAdtLoadSizeChangedCallback*>  > T_AdtLoadSizeChangedList;
	T_AdtLoadSizeChangedList	AdtLoadSizeChangedList;

};

inline void ISceneRenderServices::setAdtLoadSize( E_ADT_LOAD adt )
{
	if (AdtLoadSize != adt)
	{
		AdtLoadSize = adt;
		for (T_AdtLoadSizeChangedList::const_iterator itr = AdtLoadSizeChangedList.begin(); itr != AdtLoadSizeChangedList.end(); ++itr)
		{
			(*itr)->OnAdtLoadSizeChanged(AdtLoadSize);
		}
	}
}

inline void ISceneRenderServices::registerAdtLoadSizeChanged( IAdtLoadSizeChangedCallback* callback )
{
	if (std::find(AdtLoadSizeChangedList.begin(), AdtLoadSizeChangedList.end(), callback) == AdtLoadSizeChangedList.end())
	{
		AdtLoadSizeChangedList.push_back(callback);
	}
}

inline void ISceneRenderServices::removeAdtLoadSizeChanged( IAdtLoadSizeChangedCallback* callback )
{
	AdtLoadSizeChangedList.remove(callback);
}

