#include "stdafx.h"
#include "CAlphaTestMeshRenderer.h"
#include "mywow.h"

CAlphaTestMeshRenderer::CAlphaTestMeshRenderer(uint32_t quota)
	: Quota(quota)
{
	RenderUnits.reserve(Quota);
	RenderEntries.reserve(Quota);
}

CAlphaTestMeshRenderer::~CAlphaTestMeshRenderer()
{
}

void CAlphaTestMeshRenderer::addRenderUnit(const SRenderUnit* unit)
{
	if (!unit->material.isAlphaTest())
		return;

	bool needRealloc = RenderUnits.size() == RenderUnits.capacity();

	RenderUnits.emplace_back(*unit);

	if (needRealloc)
	{
		RenderEntries.resize(RenderUnits.size());
		for (uint32_t i=0; i<RenderUnits.size(); ++i)
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

void CAlphaTestMeshRenderer::render(const SRenderUnit*& currentUnit, ICamera* cam)
{
	if (RenderUnits.empty())
		return;

	std::sort(RenderEntries.begin(), RenderEntries.end());

	IVideoDriver* driver = g_Engine->getDriver();
	driver->helper_render(this, currentUnit, cam);

	RenderUnits.clear();
	RenderEntries.clear();
}

void CAlphaTestMeshRenderer::begin_setupLightFog( ICamera* cam ) const
{
	ISceneStateServices* sceneService = g_Engine->getDriver()->getSceneStateServices();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//fog

	//dlight
	SLight light;
	light.Type = ELT_DIRECTIONAL;
	light.LightColor = sceneEnv->DirLightColor;
	light.Direction = sceneEnv->LightDir;

	sceneService->setLight(ISceneEnvironment::INDEX_DIR_LIGHT, sceneEnv->EnableDirLight, light, sceneEnv->AmbientColor);
}

void CAlphaTestMeshRenderer::end_setupLightFog() const
{

}
