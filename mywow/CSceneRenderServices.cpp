#include "stdafx.h"
#include "CSceneRenderServices.h"
#include "mywow.h"
#include "CTerrainRenderer.h"
#include "CWmoRenderer.h"
#include "CMeshRenderer.h"
#include "CTranslucentRenderer.h"

CSceneRenderServices::CSceneRenderServices()
	: CurrentUnit(NULL)
{
	Sky_Renderer = new CMeshRenderer(5);
	Terrain_Renderer = new CTerrainRenderer(256 * 5, 256 * 5);
	Wmo_Renderer = new CWmoRenderer(2000);
	Doodad_Solid_Renderer = new CMeshRenderer(20000);
	Mesh_Solid_Renderer = new CMeshRenderer(20000);
	Transparent_Renderer = new CTransluscentRenderer(20000);
	Wire_Renderer = new CMeshRenderer(1000);

	SceneNodes = new SEntry[2000];
	SceneNodeCount = 0;

	ModelLodBias = 0;
	TerrainLodBias = 0;
	ObjectVisibleDistance = 24;
}

CSceneRenderServices::~CSceneRenderServices()
{
	delete[] SceneNodes;

	delete Wire_Renderer;
	delete Transparent_Renderer;
	delete Mesh_Solid_Renderer;
	delete Doodad_Solid_Renderer;
	delete Wmo_Renderer;
	delete Terrain_Renderer;
	delete Sky_Renderer;
}

void CSceneRenderServices::addSceneNode( ISceneNode* node )
{
	_ASSERT(SceneNodeCount < 2000);
	SceneNodes[SceneNodeCount++].node = node;
}

void CSceneRenderServices::tickAllSceneNodes( u32 timeSinceStart, u32 timeSinceLastFrame )
{
	heapsort(SceneNodes, SceneNodeCount);
	for (u32 i=0; i<SceneNodeCount; ++i)
	{
		ISceneNode* node = SceneNodes[i].node;
		node->tick(timeSinceStart, timeSinceLastFrame);
	}
}

void CSceneRenderServices::renderAllSceneNodes( )
{
	for (u32 i=0; i<SceneNodeCount; ++i)
	{
		ISceneNode* node = SceneNodes[i].node;
		node->render();
	}

	SceneNodeCount = 0;
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
		break;
	case ERT_DOODAD:
		Doodad_Solid_Renderer->addRenderUnit(unit);
		Transparent_Renderer->addRenderUnit(unit);
		break;
	case ERT_MESH:
		Mesh_Solid_Renderer->addRenderUnit(unit);
		Transparent_Renderer->addRenderUnit(unit);
		break;
	case ERT_EFFECT:	
		Transparent_Renderer->addRenderUnit(unit);
		break;
	case ERT_WIRE:
		Wire_Renderer->addRenderUnit(unit);
		break;
	default:
		_ASSERT(false);
	}
}

void CSceneRenderServices::renderAll(E_RENDERINST_TYPE type, ICamera* cam)
{
	ISceneRenderer* sceneRenderer = getSceneRenderer(type);
	sceneRenderer->render(CurrentUnit, cam);
}

void CSceneRenderServices::begin_setupLightFog( E_RENDERINST_TYPE type, ICamera* cam )
{
	ISceneRenderer* sceneRenderer = getSceneRenderer(type);
	sceneRenderer->begin_setupLightFog(cam);
}

void CSceneRenderServices::end_setupLightFog( E_RENDERINST_TYPE type )
{
	ISceneRenderer* sceneRenderer = getSceneRenderer(type);
	sceneRenderer->end_setupLightFog();
}

ISceneRenderer* CSceneRenderServices::getSceneRenderer( E_RENDERINST_TYPE type )
{
	switch(type)
	{
	case ERT_SKY:
		return Sky_Renderer;
	case ERT_TERRAIN:
		return Terrain_Renderer;
	case ERT_WMO:
		return Wmo_Renderer;
	case ERT_DOODAD:
		return Doodad_Solid_Renderer;
	case ERT_MESH:
		return Mesh_Solid_Renderer;
	case ERT_EFFECT:
		return Transparent_Renderer;
	case ERT_WIRE:
		return Wire_Renderer;
	}
	_ASSERT(false);
	return NULL;
}
