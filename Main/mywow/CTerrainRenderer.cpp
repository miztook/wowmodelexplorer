#include "stdafx.h"
#include "CTerrainRenderer.h"
#include "mywow.h"

CTerrainRenderer::CTerrainRenderer(u32 lowresQuota, u32 highresQuota)
	: LowResQuota(lowresQuota), HighResQuota(highresQuota)
{
	RenderUnits.reserve(LowResQuota + HighResQuota);
	LowRenderEntries.reserve(LowResQuota);
	HighRenderEntries.reserve(HighResQuota);
}

CTerrainRenderer::~CTerrainRenderer()
{
}

void CTerrainRenderer::addRenderUnit( const SRenderUnit* unit )
{
	ASSERT(unit->sceneNode->getType() == EST_WDT || unit->sceneNode->getType() == EST_MAPTILE);

	bool needRealloc = RenderUnits.size() == RenderUnits.capacity();

	RenderUnits.emplace_back(*unit);

	if (needRealloc)
	{
		LowRenderEntries.clear();
		HighRenderEntries.clear();
		for (u32 i=0; i<RenderUnits.size(); ++i)
		{
			SEntry entry;
			entry.unit = &RenderUnits[i];

			std::vector<SEntry>& entries = RenderUnits[i].u.lowres ? LowRenderEntries : HighRenderEntries;
			entries.emplace_back(entry);
		}
	}
	else
	{
		SEntry entry;
		entry.unit = &RenderUnits.back();

		std::vector<SEntry>& entries = unit->u.lowres ? LowRenderEntries : HighRenderEntries;
		entries.emplace_back(entry);
	}
}

void CTerrainRenderer::render(const SRenderUnit*& currentUnit, ICamera* cam)
{
	if (RenderUnits.empty())
		return;

	//high res, low res
	std::sort(HighRenderEntries.begin(), HighRenderEntries.end());
	std::sort(LowRenderEntries.begin(), LowRenderEntries.end());

	IVideoDriver* driver = g_Engine->getDriver();
	driver->helper_render(this, currentUnit, cam);

	HighRenderEntries.clear();
	LowRenderEntries.clear();
	RenderUnits.clear();
}

void CTerrainRenderer::begin_setupLightFog(ICamera* cam) const
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//fog
	SFogParam param;
	param.FogColor = sceneEnv->getFogColor();
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

void CTerrainRenderer::end_setupLightFog() const
{

}

