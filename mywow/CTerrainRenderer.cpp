#include "stdafx.h"
#include "CTerrainRenderer.h"
#include "mywow.h"

CTerrainRenderer::CTerrainRenderer(u32 lowresQuota, u32 highresQuota)
	: LowResQuota(lowresQuota), HighResQuota(highresQuota)
{
	RenderUnits = new SRenderUnit[LowResQuota + HighResQuota];
	LowRenderEntries = new SEntry[LowResQuota];
	HighRenderEntries = new SEntry[HighResQuota];
	CurrentRenderCount = 0;
	LowCount = HighCount = 0;
}

CTerrainRenderer::~CTerrainRenderer()
{
	delete[] HighRenderEntries;
	delete[] LowRenderEntries;
	delete[] RenderUnits;
}

void CTerrainRenderer::addRenderUnit( const SRenderUnit* unit )
{
	if (CurrentRenderCount == LowResQuota + HighResQuota)
		return;

	SRenderUnit* s = &RenderUnits[CurrentRenderCount];
	*s = *unit;

	SEntry entry;
	entry.Unit = s;
	
	if (unit->u.lowres)
	{
		if (LowCount < LowResQuota)
			LowRenderEntries[LowCount++] = entry;
		++CurrentRenderCount;
	}
	else
	{
		if(HighCount < HighResQuota)
			HighRenderEntries[HighCount++] = entry;
		++CurrentRenderCount;
	}
}

void CTerrainRenderer::render(SRenderUnit*& currentUnit, ICamera* cam)
{
	if (CurrentRenderCount == 0)
		return;

	IVideoDriver* driver = g_Engine->getDriver();
	ISceneStateServices* sceneService = driver->getSceneStateServices();
	IMaterialRenderServices* services = driver->getMaterialRenderServices();

	plane3df clipPlane = cam->getTerrainClipPlane();
 	sceneService->setClipPlane(0, clipPlane);
 	sceneService->enableClipPlane(0, true);

	//high res
	heapsort<SEntry>(HighRenderEntries, HighCount);
	//services->setDepthBias( 0.000002f );
	
	driver->setTransform(ETS_VIEW, cam->getViewMatrix());
	driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	for (u32 i=0; i<HighCount; ++i)
	{
		SRenderUnit* unit  = HighRenderEntries[i].Unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		driver->setTransform(ETS_WORLD, unit->matWorld ? *unit->matWorld : matrix4(true));

		driver->setMaterial(unit->material);
		if (services->isFFPipeline())		//shader¿Ô…Ë÷√texture
		{
			for (u32 t=0; t<MATERIAL_MAX_TEXTURES; ++t)
				driver->setTexture(t, unit->textures[t]);
		}

		driver->draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);	
	}

	sceneService->enableClipPlane(0, false);

	//low res
	heapsort<SEntry>(LowRenderEntries, LowCount);

	driver->setTransform(ETS_VIEW, cam->getViewMatrix());
	driver->setTransform(ETS_PROJECTION, cam->getProjectionMatrix());
	for (u32 i=0; i<LowCount; ++i)
	{
		SRenderUnit* unit  = LowRenderEntries[i].Unit;
		currentUnit = unit;

		if (!unit->primCount)
			continue;

		driver->setTransform(ETS_WORLD, unit->matWorld ? *unit->matWorld : matrix4(true));

		driver->setMaterial(unit->material);
		if (services->isFFPipeline())
		{
			for (u32 t=0; t<MATERIAL_MAX_TEXTURES; ++t)
				driver->setTexture(t, unit->textures[t]);
		}
		
		driver->draw3DMode(unit->bufferParam, unit->primType, unit->primCount, unit->drawParam);
	}

	//services->setDepthBias( 0.0f );

	LowCount = HighCount = 0;
	CurrentRenderCount = 0;
}

void CTerrainRenderer::begin_setupLightFog(ICamera* cam)
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();
	ISceneRenderServices*	sceneRenderService = g_Engine->getSceneRenderServices();

	//
	f32 clip = sceneRenderService->getClipDistance();
	if (cam->getClipDistance() != clip)
		cam->setClipDistance(clip);

	//fog
	SFogParam param;
	param.FogColor = g_Engine->getSceneEnvironment()->getFogColor();
	param.FogStart = cam->getClipDistance() - 250;
	param.FogEnd = cam->getClipDistance() - 100;
	param.FogType = EFT_FOG_LINEAR;
	param.PixelFog = true;
	sceneService->setFog(param);

	//ambient
	sceneService->setAmbientLight(SColor(220,220,220));


}

void CTerrainRenderer::end_setupLightFog()
{

}

