#include "stdafx.h"
#include "CMeshRenderer.h"
#include "mywow.h"

CMeshRenderer::CMeshRenderer(u32 quota)
	: ISceneRenderer(quota)
{
	RenderUnits = (SRenderUnit*)Hunk_AllocateTempMemory(sizeof(SRenderUnit) * Quota);
	RenderEntries = (SEntry*)Hunk_AllocateTempMemory(sizeof(SEntry) * Quota);
	CurrentRenderCount = 0;
}

CMeshRenderer::~CMeshRenderer()
{
	Hunk_FreeTempMemory(RenderEntries);
	Hunk_FreeTempMemory(RenderUnits);
}

void CMeshRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if (unit->material.isTransparent() || CurrentRenderCount == Quota)
		return;

	SRenderUnit* s = &RenderUnits[CurrentRenderCount];
	*s = *unit;

	SEntry entry;
	entry.Unit = s;
	RenderEntries[CurrentRenderCount++] = entry;
}

void CMeshRenderer::render(SRenderUnit*& currentUnit,  ICamera* cam)
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
		{
			driver->setTransform(ETS_VIEW, *unit->matView);
		}
		else
		{
			driver->setTransform(ETS_VIEW, cam->getViewMatrix());
		}

		if (unit->matProjection)				//渲染单元允许有独立的projection, 改变深度值
		{
			driver->setTransform(ETS_PROJECTION, *unit->matProjection);
		}
		else
		{
			driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
		}

		driver->setTransform(ETS_WORLD, unit->matWorld ? *unit->matWorld : matrix4(true));

		driver->setMaterial(unit->material);
		for (u32 t=0; t<MATERIAL_MAX_TEXTURES; ++t)
			driver->setTexture(t, unit->textures[t]);

		if(unit->ibuffer)
			driver->draw3DMode(unit->vbuffer, unit->ibuffer, unit->vbuffer2, unit->primType, unit->primCount, unit->drawParam);
		else
			driver->draw3DMode(unit->vbuffer, unit->primType, unit->primCount, unit->drawParam.voffset, unit->drawParam.startIndex);

	}

	CurrentRenderCount = 0;
}