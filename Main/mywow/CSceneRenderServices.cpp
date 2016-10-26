#include "stdafx.h"
#include "CSceneRenderServices.h"
#include "mywow.h"
#include "CTerrainRenderer.h"
#include "CWmoRenderer.h"
#include "CMeshRenderer.h"
#include "CTransluscentRenderer.h"
#include "CParticleRenderer.h"
#include "CRibbonRenderer.h"
#include "CMeshDecalRenderer.h"
#include "CTransluscentDecalRenderer.h"
#include "CAlphaTestWmoRenderer.h"
#include "CAlphaTestMeshRenderer.h"
#include "CAlphaTestDecalRenderer.h"

CSceneRenderServices::CSceneRenderServices()
{
	Sky_Renderer = new CMeshRenderer(5);
	Terrain_Renderer = new CTerrainRenderer(256 * 25, 256 * 25);
	Wmo_Renderer = new CWmoRenderer(2000);
	Wmo_AlphaTest_Renderer = new CAlphaTestWmoRenderer(500);
	Doodad_Solid_Renderer = new CMeshRenderer(4000);
	Doodad_Decal_Renderer = new CMeshDecalRenderer(2000);
	Mesh_Solid_Renderer = new CMeshRenderer(4000);
	Mesh_Decal_Renderer = new CMeshDecalRenderer(2000);
	AlphaTest_Renderer = new CAlphaTestMeshRenderer(1000);
	AlphaTest_Decal_Renderer = new CAlphaTestDecalRenderer(1000);
	Transparent_Renderer = new CTransluscentRenderer(5000);
	Transparent_Decal_Renderer = new CTransluscentDecalRenderer(4000);
	ParticleRenderer = new CParticleRenderer(2000);
	RibbonRenderer = new CRibbonRenderer(2000);
	Wire_Renderer = new CMeshRenderer(100);

	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		m_SceneNodes[i].reserve(100);
	}

	ClipDistance = 1000.0f;
	ModelLodBias = 0;
	TerrainLodBias = 0;
	ObjectVisibleDistance = 24;
	M2InvisibleTickDistance = 30;
	M2SlowTickStart = 0.5f;
	AdtLoadSize = EAL_3X3;
}

CSceneRenderServices::~CSceneRenderServices()
{
	delete Wire_Renderer;
	delete RibbonRenderer;
	delete ParticleRenderer;
	delete Transparent_Decal_Renderer;
	delete Transparent_Renderer;
	delete AlphaTest_Decal_Renderer;
	delete AlphaTest_Renderer;
	delete Mesh_Decal_Renderer;
	delete Mesh_Solid_Renderer;
	delete Doodad_Decal_Renderer;
	delete Doodad_Solid_Renderer;
	delete Wmo_AlphaTest_Renderer;
	delete Wmo_Renderer;
	delete Terrain_Renderer;
	delete Sky_Renderer;
}

void CSceneRenderServices::addSceneNode( ISceneNode* node, bool visible, int sequence )
{
	if (sequence >= MAX_SCENENODE_SEQUENCE)
	{
		ASSERT(false);
		return;
	}

	SEntry entry;
	entry.node = node;
	entry.visible = visible;
	entry.skip = false;

	m_SceneNodes[sequence].emplace_back(entry);
}

void CSceneRenderServices::skipSceneNode(ISceneNode* node)
{
	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		for(std::vector<SEntry>::iterator itr = m_SceneNodes[i].begin(); itr != m_SceneNodes[i].end(); ++itr)
		{
			if(itr->node == node)
			{
				itr->skip = true;
				break;
			}
		}
	}
}

void CSceneRenderServices::clearAllSceneNodes()
{
	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		m_SceneNodes[i].clear();
	}
}

void CSceneRenderServices::tickAllSceneNodes( u32 timeSinceStart, u32 timeSinceLastFrame, int sequence )
{
	if (sequence >= MAX_SCENENODE_SEQUENCE ||
		m_SceneNodes[sequence].empty())
		return;

	std::vector<SEntry>& sceneNodes = m_SceneNodes[sequence];
	std::sort(sceneNodes.begin(), sceneNodes.end());
	u32 size = (u32)sceneNodes.size();
	for (u32 i=0; i<size; ++i)
	{		
		if (sceneNodes[i].skip)
			continue;
		ISceneNode* node = sceneNodes[i].node;
		node->tick(timeSinceStart, timeSinceLastFrame, sceneNodes[i].visible);
	}
}

void CSceneRenderServices::renderAllSceneNodes() const
{
	for (int n=0; n<MAX_SCENENODE_SEQUENCE; ++n)
	{
		const std::vector<SEntry>& sceneNodes = m_SceneNodes[n];

		u32 size = (u32)sceneNodes.size();
		for (u32 i=0; i<size; ++i)
		{
			if (sceneNodes[i].skip)
				continue;

			ISceneNode* node = sceneNodes[i].node;
			if(sceneNodes[i].visible)
				node->render();
		}
	}
}

void CSceneRenderServices::addRenderUnit( const SRenderUnit* unit, E_RENDERINST_TYPE type )
{
	switch(type)
	{
	case ERT_SKY:
		Sky_Renderer->addRenderUnit(unit);
		break;
	case ERT_TERRAIN:
		Terrain_Renderer->addRenderUnit(unit);
		break;
	case ERT_WMO:
		Wmo_Renderer->addRenderUnit(unit);
		Wmo_AlphaTest_Renderer->addRenderUnit(unit);
		break;
	case ERT_DOODAD:
		Doodad_Solid_Renderer->addRenderUnit(unit);
		AlphaTest_Renderer->addRenderUnit(unit);
		Transparent_Renderer->addRenderUnit(unit);
		break;
	case ERT_DOODADDECAL:
		Doodad_Decal_Renderer->addRenderUnit(unit);
		AlphaTest_Decal_Renderer->addRenderUnit(unit);
		Transparent_Decal_Renderer->addRenderUnit(unit);
		break;
	case ERT_MESH:
		Mesh_Solid_Renderer->addRenderUnit(unit);
		AlphaTest_Renderer->addRenderUnit(unit);
		Transparent_Renderer->addRenderUnit(unit);
		break;	
	case ERT_MESHDECAL:
		Mesh_Decal_Renderer->addRenderUnit(unit);
		AlphaTest_Decal_Renderer->addRenderUnit(unit);
		Transparent_Decal_Renderer->addRenderUnit(unit);
		break;
	case ERT_PARTICLE:	
		ParticleRenderer->addRenderUnit(unit);
		break;
	case ERT_RIBBON:
		RibbonRenderer->addRenderUnit(unit);
		break;
	case ERT_WIRE:
		Wire_Renderer->addRenderUnit(unit);
		break;
	default:
		ASSERT(false);
	}
}

void CSceneRenderServices::renderAll(E_RENDERINST_TYPE type, ICamera* cam)
{
	CurrentUnit = NULL_PTR;

	switch(type)
	{
	case ERT_SKY:
		{
			Sky_Renderer->begin_setupLightFog(cam);
			Sky_Renderer->render(CurrentUnit, cam);
			Sky_Renderer->end_setupLightFog();
		}
		break;
	case ERT_TERRAIN:
		{
			Terrain_Renderer->begin_setupLightFog(cam);
			Terrain_Renderer->render(CurrentUnit, cam);
			Terrain_Renderer->end_setupLightFog();
		}
		break;
	case ERT_WMO:
		{
			Wmo_Renderer->begin_setupLightFog(cam);
			Wmo_Renderer->render(CurrentUnit, cam);
			Wmo_Renderer->end_setupLightFog();
		}
		break;
	case ERT_DOODAD:
		{
			Doodad_Solid_Renderer->begin_setupLightFog(cam);
			Doodad_Solid_Renderer->render(CurrentUnit, cam);
			Doodad_Solid_Renderer->end_setupLightFog();

			Doodad_Decal_Renderer->begin_setupLightFog(cam);
			Doodad_Decal_Renderer->render(CurrentUnit, cam);
			Doodad_Decal_Renderer->end_setupLightFog();
		}
		break;
	case ERT_MESH:
		{
			Mesh_Solid_Renderer->begin_setupLightFog(cam);
			Mesh_Solid_Renderer->render(CurrentUnit, cam);
			Mesh_Solid_Renderer->end_setupLightFog();

			Mesh_Decal_Renderer->begin_setupLightFog(cam);
			Mesh_Decal_Renderer->render(CurrentUnit, cam);
			Mesh_Decal_Renderer->end_setupLightFog();
		}
		break;
	case ERT_ALPHATEST:
		{
			Wmo_AlphaTest_Renderer->begin_setupLightFog(cam);
			Wmo_AlphaTest_Renderer->render(CurrentUnit, cam);
			Wmo_AlphaTest_Renderer->end_setupLightFog();

			AlphaTest_Renderer->begin_setupLightFog(cam);
			AlphaTest_Renderer->render(CurrentUnit, cam);
			AlphaTest_Renderer->end_setupLightFog();

			AlphaTest_Decal_Renderer->begin_setupLightFog(cam);
			AlphaTest_Decal_Renderer->render(CurrentUnit, cam);
			AlphaTest_Decal_Renderer->end_setupLightFog();
		}
		break;
	case ERT_TRANSPARENT:
		{
			Transparent_Renderer->begin_setupLightFog(cam);
			Transparent_Renderer->render(CurrentUnit, cam);
			Transparent_Renderer->end_setupLightFog();
			
			Transparent_Decal_Renderer->begin_setupLightFog(cam);
			Transparent_Decal_Renderer->render(CurrentUnit, cam);
			Transparent_Decal_Renderer->end_setupLightFog();
		}
		break;
	case ERT_PARTICLE:
		{
			ParticleRenderer->begin_setupLightFog(cam);
			ParticleRenderer->render(CurrentUnit, cam);
			ParticleRenderer->end_setupLightFog();
		}
		break;
	case ERT_RIBBON:
		{
			RibbonRenderer->begin_setupLightFog(cam);
			RibbonRenderer->render(CurrentUnit, cam);
			RibbonRenderer->end_setupLightFog();
		}
		break;
	case ERT_WIRE:
		{
			Wire_Renderer->begin_setupLightFog(cam);
			Wire_Renderer->render(CurrentUnit, cam);
			Wire_Renderer->end_setupLightFog();
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	CurrentUnit = NULL_PTR;
}


