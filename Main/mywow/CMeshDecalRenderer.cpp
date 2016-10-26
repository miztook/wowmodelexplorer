#include "stdafx.h"
#include "CMeshDecalRenderer.h"
#include "mywow.h"
#include "CMeshDecalServices.h"
#include "CM2SceneNode.h"

CMeshDecalRenderer::CMeshDecalRenderer(u32 quota)
	: Quota(quota)
{
	RenderUnits.reserve(Quota);
	RenderEntries.reserve(Quota);

	MeshDecalServices = static_cast<CMeshDecalServices*>(g_Engine->getMeshDecalServices());
}

CMeshDecalRenderer::~CMeshDecalRenderer()
{

}

void CMeshDecalRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if (unit->material.isTransparent() || unit->material.isAlphaTest())
		return;

	bool needRealloc = RenderUnits.size() == RenderUnits.capacity();

	RenderUnits.emplace_back(*unit);

	if (needRealloc)
	{
		RenderEntries.resize(RenderUnits.size());
		for (u32 i=0; i<RenderUnits.size(); ++i)
		{
			RenderEntries[i].unit = &RenderUnits[i];
		}
	}
	else
	{
		SEntry entry;
		entry.unit = &RenderUnits.back();
		RenderEntries.emplace_back(entry);
	}

}

void CMeshDecalRenderer::render(const SRenderUnit*& currentUnit,  ICamera* cam)
{
	if (RenderUnits.empty())
		return;

	std::sort(RenderEntries.begin(), RenderEntries.end());

	//collect batches
	RenderBatches.clear();

	u32 size = (u32)RenderEntries.size();
	for (u32 i=0; i<size; ++i)
	{
		const SRenderUnit* unit  = RenderEntries[i].unit;

		if (!unit->drawParam.numVertices)
			continue;

		if (RenderBatches.empty())
		{
			addNewBatch(unit);
		}
		else
		{
			SBatch& batch = RenderBatches.back();
			bool nextExceed =  isBatchExceed(batch, unit->drawParam.numVertices);		
			if (nextExceed || !isInBatch(batch, unit))
			{
				//finish current batch, commint to hw buffer
				MeshDecalServices->updateVertices(batch.vcount);

				renderAllBatches(currentUnit, cam);

				addNewBatch(unit);
			}
		}

		//fill current batch's vertex, not commit to hw buffer
		SBatch& batch = RenderBatches.back();
		SVertex_PCT* vertices = (SVertex_PCT*)&MeshDecalServices->Vertices[batch.vbase + batch.vcount];		

		u32 nFill = 0;
		if (unit->sceneNode->getType() == EST_M2)
		{		
			const CM2SceneNode* node = static_cast<const CM2SceneNode*>(unit->sceneNode);
			nFill = node->onFillVertexBuffer(unit->u.geoset, vertices, unit->drawParam.numVertices);
		}
		batch.vcount += nFill;
	}

	if (!RenderBatches.empty())
	{
		SBatch& batch = RenderBatches.back();
		MeshDecalServices->updateVertices(batch.vcount);
		renderAllBatches(currentUnit, cam);
	}

	RenderUnits.clear();
	RenderEntries.clear();
}

void CMeshDecalRenderer::begin_setupLightFog( ICamera* cam ) const
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();
	//fog

	//dlight
	SLight light;
	light.Type = ELT_DIRECTIONAL;
	light.DiffuseColor = sceneEnv->DirLightColor;
	light.Direction = sceneEnv->LightDir;

	sceneService->setLight(ISceneEnvironment::INDEX_DIR_LIGHT, sceneEnv->EnableDirLight, light, sceneEnv->AmbientColor);
}

void CMeshDecalRenderer::end_setupLightFog() const
{

}

void CMeshDecalRenderer::renderAllBatches( const SRenderUnit*& currentUnit, ICamera* cam )
{
	IVideoDriver* driver = g_Engine->getDriver();

	driver->helper_renderAllBatches(this, currentUnit, cam);
}

bool CMeshDecalRenderer::isInBatch( const SBatch& batch, const SRenderUnit* unit ) const
{
	return batch.material == unit->material &&
		batch.texture0 == unit->textures[0] &&
		batch.matWorld == unit->matWorld &&
		batch.matView == unit->matView &&
		batch.matProjection == unit->matProjection;
}

bool CMeshDecalRenderer::isBatchExceed( const SBatch& batch, u32 vcount ) const
{
	return batch.vbase + batch.vcount + vcount + vcount > MeshDecalServices->getMaxVertexCount();
}

void CMeshDecalRenderer::addNewBatch( const SRenderUnit* unit )
{
	SBatch batch;

	batch.vbase = 0;
	batch.vcount = 0;

	batch.firstUnit = unit;

	batch.material = unit->material;
	batch.texture0 = unit->textures[0];
	batch.matWorld = unit->matWorld;
	batch.matView = unit->matView;
	batch.matProjection = unit->matProjection;

	RenderBatches.emplace_back(batch);
}
