#include "stdafx.h"
#include "CRibbonRenderer.h"
#include "mywow.h"
#include "CRibbonEmitterServices.h"
#include "CRibbonSceneNode.h"

CRibbonRenderer::CRibbonRenderer( u32 quota )
	: Quota(quota)
{
	RenderUnits.reserve(Quota);
	RenderEntries.reserve(Quota);

	RibbonServices = static_cast<CRibbonEmitterServices*>(g_Engine->getRibbonEmitterServices());
}

CRibbonRenderer::~CRibbonRenderer()
{

}

void CRibbonRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if ( !unit->material.isTransparent())
		return;

	ASSERT(unit->sceneNode->getType() == EST_RIBBON);

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

void CRibbonRenderer::render( const SRenderUnit*& currentUnit, ICamera* cam )
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
				//finish current batch, commit to hw buffer
				RibbonServices->updateVertices(batch.vcount);

				renderAllBatches(currentUnit, cam);

				addNewBatch(unit);
			}
		}

		//fill current batch's vertex, not commit to hw buffer
		SBatch& batch = RenderBatches.back();
		const CRibbonSceneNode* node = static_cast<const CRibbonSceneNode*>(unit->sceneNode);
		SVertex_PCT* vertices = (SVertex_PCT*)&RibbonServices->Vertices[batch.vbase + batch.vcount];
		u32 nFill = node->onFillVertexBuffer(vertices, unit->drawParam.numVertices);
		batch.vcount += nFill;
	}

	if (!RenderBatches.empty())
	{
		SBatch& batch = RenderBatches.back();
		RibbonServices->updateVertices(batch.vcount);
		renderAllBatches(currentUnit, cam);
	}

	RenderUnits.clear();
	RenderEntries.clear();
}

void CRibbonRenderer::renderAllBatches( const SRenderUnit*& currentUnit, ICamera* cam )
{
	IVideoDriver* driver = g_Engine->getDriver();

	driver->helper_renderAllBatches(this, currentUnit, cam);
}

void CRibbonRenderer::begin_setupLightFog( ICamera* cam ) const
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

void CRibbonRenderer::end_setupLightFog() const
{

}

bool CRibbonRenderer::isInBatch( const SBatch& batch, const SRenderUnit* unit ) const
{
	return batch.material == unit->material &&
		batch.texture0 == unit->textures[0] &&
		batch.matWorld == unit->matWorld &&
		batch.matView == unit->matView &&
		batch.matProjection == unit->matProjection;
}

bool CRibbonRenderer::isBatchExceed( const SBatch& batch, u32 vcount ) const
{
	return batch.vbase + batch.vcount + vcount > RibbonServices->getMaxVertexCount();
}

void CRibbonRenderer::addNewBatch( const SRenderUnit* unit )
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

