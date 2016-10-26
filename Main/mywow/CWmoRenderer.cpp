#include "stdafx.h"
#include "CWmoRenderer.h"
#include "mywow.h"

CWmoRenderer::CWmoRenderer(u32 quota)
	: Quota(quota)
{
	RenderUnits.reserve(Quota);
	RenderEntries.reserve(Quota);
}

CWmoRenderer::~CWmoRenderer()
{
}

void CWmoRenderer::addRenderUnit( const SRenderUnit* unit)
{
	if (unit->material.isTransparent() || unit->material.isAlphaTest())
	{
		return;
	}

	ASSERT(unit->sceneNode->getType() == EST_WMO);

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

void CWmoRenderer::render(const SRenderUnit*& currentUnit,  ICamera* cam)
{
	if (RenderUnits.empty())
		return;

	std::sort(RenderEntries.begin(), RenderEntries.end());

	IVideoDriver* driver = g_Engine->getDriver();
	driver->helper_render(this, currentUnit, cam);

	RenderUnits.clear();
	RenderEntries.clear();
}

void CWmoRenderer::begin_setupLightFog( ICamera* cam ) const
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//fog
	SFogParam param;
	param.FogColor = g_Engine->getSceneEnvironment()->getFogColor();
	param.FogStart = cam->getClipDistance() - 300.0f;
	param.FogEnd = cam->getClipDistance() - 50.0f;
	param.FogDensity = 0.8f;
	param.FogType = EFT_FOG_LINEAR;
	param.PixelFog = false;

	//dlight
	SLight light;
	light.Type = ELT_DIRECTIONAL;
	light.DiffuseColor = sceneEnv->DirLightColor;
	light.Direction = sceneEnv->LightDir;

	sceneService->setLight_Fog(ISceneEnvironment::INDEX_DIR_LIGHT, sceneEnv->EnableDirLight, light, sceneEnv->AmbientColor, param);
}

void CWmoRenderer::end_setupLightFog() const
{

}
