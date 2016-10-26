#pragma once

#include "ISceneRenderServices.h"
#include "ISceneNode.h"
#include "ISceneManager.h"
#include <list>
#include <vector>

class CMeshRenderer;
class CTerrainRenderer;
class CWmoRenderer;
class CTransluscentRenderer;
class CParticleRenderer;
class CRibbonRenderer;
class CMeshDecalRenderer;
class CTransluscentDecalRenderer;
class CAlphaTestWmoRenderer;
class CAlphaTestMeshRenderer;
class CAlphaTestDecalRenderer;

class CSceneRenderServices : public ISceneRenderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSceneRenderServices);

public:
	CSceneRenderServices();
	~CSceneRenderServices();

public:
	void addSceneNode(ISceneNode* node, bool visible, int sequence);
	void skipSceneNode(ISceneNode* node);

	void clearAllSceneNodes();
	void tickAllSceneNodes(u32 timeSinceStart, u32 timeSinceLastFrame, int sequence);
	void renderAllSceneNodes() const;

	void addRenderUnit(const SRenderUnit* unit, E_RENDERINST_TYPE type);
	void renderAll(E_RENDERINST_TYPE type, ICamera* cam);

private:
	struct SEntry 
	{
		ISceneNode*		node;
		bool visible;
		bool skip;

		bool operator<(const SEntry& c) const
		{
			if (node->Generation != c.node->Generation)
				return node->Generation < c.node->Generation;
			if (visible != c.visible)
				return visible > c.visible;
			else
				return node < c.node;
		}
	};

private:
	std::vector<SEntry>		m_SceneNodes[MAX_SCENENODE_SEQUENCE];

	//渲染顺序: 天空, 场景地表，wmo, doodads, 角色模型，特效
	CMeshRenderer*		Sky_Renderer;
	CTerrainRenderer*		Terrain_Renderer;
	CWmoRenderer*		Wmo_Renderer;
	CAlphaTestWmoRenderer*	Wmo_AlphaTest_Renderer;
	CMeshRenderer*		Doodad_Solid_Renderer;
	CMeshDecalRenderer*		Doodad_Decal_Renderer;
	CMeshRenderer*		Mesh_Solid_Renderer;
	CMeshDecalRenderer*		Mesh_Decal_Renderer;
	CAlphaTestMeshRenderer*		AlphaTest_Renderer;
	CAlphaTestDecalRenderer*		AlphaTest_Decal_Renderer;
	CTransluscentRenderer*		Transparent_Renderer;
	CTransluscentDecalRenderer*		Transparent_Decal_Renderer;
	CParticleRenderer*		ParticleRenderer;
	CRibbonRenderer*		RibbonRenderer;
	CMeshRenderer*		Wire_Renderer;
};
