#pragma once

#include "ISceneRenderServices.h"
#include "ISceneNode.h"
#include <list>

class IVideoDriver;
class ISceneRenderer;

class CSceneRenderServices : public ISceneRenderServices
{
public:
	CSceneRenderServices();
	~CSceneRenderServices();

public:
	virtual void addSceneNode(ISceneNode* node);

	virtual void tickAllSceneNodes(u32 timeSinceStart, u32 timeSinceLastFrame);
	virtual void renderAllSceneNodes();

	virtual void addRenderUnit(const SRenderUnit* unit, E_RENDERINST_TYPE type);
	virtual void renderAll(E_RENDERINST_TYPE type, ICamera* cam);

	virtual SRenderUnit* getCurrentUnit() const { return CurrentUnit; }

private:
	struct SEntry 
	{
		ISceneNode*		node;

		bool operator<(const SEntry& c) const
		{
			return node->Generation < c.node->Generation;
		}
	};
private:
	SEntry*				SceneNodes;
	u32			SceneNodeCount;

	SRenderUnit*			RenderUnits;
	u32			RenderUnitCount;

	//渲染顺序: 场景静态模型，地表，动态模型，特效
	ISceneRenderer*		StaticMesh_Solid_Renderer;
	ISceneRenderer*		StaticMesh_Transprent_Renderer;
	ISceneRenderer*		Terrain_Renderer;
	ISceneRenderer*		DynMesh_Solid_Renderer;
	ISceneRenderer*		DynMesh_Transparent_Renderer;
	ISceneRenderer*		Effect_Renderer;
	ISceneRenderer*		Wire_Renderer;

	SRenderUnit*			CurrentUnit;

};