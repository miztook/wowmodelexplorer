#include "stdafx.h"
#include "CTranslucentRenderer.h"
#include "mywow.h"

CTransluscentRenderer::CTransluscentRenderer(u32 quota)
	: Quota(quota)
{
	RenderUnits = new SRenderUnit[Quota];
	RenderEntries = new SEntry[Quota];
	CurrentRenderCount = 0;
}

CTransluscentRenderer::~CTransluscentRenderer()
{
	delete[] RenderEntries;
	delete[] RenderUnits;
}

void CTransluscentRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if ( !unit->material.isTransparent() || 
		CurrentRenderCount == Quota)
		return;

	SRenderUnit* s = &RenderUnits[CurrentRenderCount];
	*s = *unit;

	SEntry entry;
	entry.Unit = s;
	RenderEntries[CurrentRenderCount++] = entry;
}

void CTransluscentRenderer::render(SRenderUnit*& currentUnit, ICamera* cam)
{
	if (CurrentRenderCount == 0)
		return;

	heapsort<SEntry>(RenderEntries, CurrentRenderCount);

	IVideoDriver* driver = g_Engine->getDriver();

	for (u32 i=0; i<CurrentRenderCount; ++i)
	{
		SRenderUnit* unit  = RenderEntries[i].Unit;
		currentUnit = unit;

		if (unit->sceneNode)
			unit->sceneNode->onPreRender();			//在渲染前

		if (!unit->primCount)
			continue;

		if (unit->matView)
			driver->setTransform(ETS_VIEW, *unit->matView);
		else
			driver->setTransform(ETS_VIEW, cam->getViewMatrix());

		if (unit->matProjection)				//渲染单元允许有独立的projection, 改变深度值
			driver->setTransform(ETS_PROJECTION, *unit->matProjection);
		else
			driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());

		driver->setTransform(ETS_WORLD, unit->matWorld ? *unit->matWorld : matrix4(true));

		driver->setMaterial(unit->material);
		for (u32 t=0; t<MATERIAL_MAX_TEXTURES; ++t)
			driver->setTexture(t, unit->textures[t]);

		driver->draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}

	CurrentRenderCount = 0;
}

void CTransluscentRenderer::begin_setupLightFog( ICamera* cam )
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();

	//fog

	//dlight

	//ambient
	sceneService->setAmbientLight(SColor(255,255,255));
}

void CTransluscentRenderer::end_setupLightFog( )
{

}

