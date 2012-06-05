#include "stdafx.h"
#include "CSceneRenderServices.h"
#include "mywow.h"
#include "CTerrainRenderer.h"
#include "CMeshRenderer.h"
#include "CTranslucentRenderer.h"

CSceneRenderServices::CSceneRenderServices()
	: CurrentUnit(NULL)
{
	StaticMesh_Solid_Renderer = new CMeshRenderer(10000);
	StaticMesh_Transprent_Renderer = new CTransluscentRenderer(10000);
	Terrain_Renderer = new CTerrainRenderer(256 * 5);
	DynMesh_Solid_Renderer = new CMeshRenderer(2000);
	DynMesh_Transparent_Renderer = new CTransluscentRenderer(2000);
	Effect_Renderer = new CTransluscentRenderer(2000);
	Wire_Renderer = new CMeshRenderer(1000);

	SceneNodes = (SEntry*)Hunk_AllocateTempMemory(sizeof(SEntry) * 2000);
	SceneNodeCount = 0;
}

CSceneRenderServices::~CSceneRenderServices()
{
	Hunk_FreeTempMemory(SceneNodes);

	delete Wire_Renderer;
	delete Effect_Renderer;
	delete DynMesh_Transparent_Renderer;
	delete DynMesh_Solid_Renderer;
	delete Terrain_Renderer;
	delete StaticMesh_Transprent_Renderer;
	delete StaticMesh_Solid_Renderer;
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
	case ERT_STATIC_MESH:
		StaticMesh_Solid_Renderer->addRenderUnit(unit);
		StaticMesh_Transprent_Renderer->addRenderUnit(unit);
		break;
	case ERT_TERRAIN:
		Terrain_Renderer->addRenderUnit(unit);
		break;
	case ERT_MESH:
		DynMesh_Solid_Renderer->addRenderUnit(unit);
		DynMesh_Transparent_Renderer->addRenderUnit(unit);
		break;
	case ERT_EFFECT:
		Effect_Renderer->addRenderUnit(unit);
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
	switch(type)
	{
	case ERT_STATIC_MESH:
		StaticMesh_Solid_Renderer->render(CurrentUnit, cam);
		StaticMesh_Transprent_Renderer->render(CurrentUnit, cam);
		break;
	case ERT_TERRAIN:
		Terrain_Renderer->render(CurrentUnit, cam);
		break;
	case ERT_MESH:
		DynMesh_Solid_Renderer->render(CurrentUnit, cam);
		DynMesh_Transparent_Renderer->render(CurrentUnit, cam);
		break;
	case ERT_EFFECT:
		Effect_Renderer->render(CurrentUnit, cam);
		break;
	case ERT_WIRE:
		Wire_Renderer->render(CurrentUnit, cam);
		break;
	}

}