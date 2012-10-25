#include "stdafx.h"
#include "CWmoRenderer.h"
#include "mywow.h"

CWmoRenderer::CWmoRenderer(u32 quota)
	: Quota(quota)
{
	RenderUnits = new SRenderUnit[Quota];
	RenderEntries = new SEntry[Quota];
	CurrentRenderCount = 0;
}

CWmoRenderer::~CWmoRenderer()
{
	delete[] RenderEntries;
	delete[] RenderUnits;
}

void CWmoRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if (unit->material.isTransparent() || CurrentRenderCount == Quota)
		return;

	SRenderUnit* s = &RenderUnits[CurrentRenderCount];
	*s = *unit;

	SEntry entry;
	entry.Unit = s;
	RenderEntries[CurrentRenderCount++] = entry;
}

void CWmoRenderer::render(SRenderUnit*& currentUnit,  ICamera* cam)
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
			unit->sceneNode->onPreRender();			//ÔÚäÖÈ¾Ç°

		if (!unit->primCount)
			continue;

		driver->setTransform(ETS_VIEW, cam->getViewMatrix());
		driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
		driver->setTransform(ETS_WORLD, unit->matWorld ? *unit->matWorld : matrix4(true));

		driver->setMaterial(unit->material);
		for (u32 t=0; t<MATERIAL_MAX_TEXTURES; ++t)
			driver->setTexture(t, unit->textures[t]);

		driver->draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}

	CurrentRenderCount = 0;
}

void CWmoRenderer::begin_setupLightFog( ICamera* cam )
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();

	//fog

	//dlight

	//ambient
	sceneService->setAmbientLight(SColor(255,255,255));
}

void CWmoRenderer::end_setupLightFog( )
{

}
