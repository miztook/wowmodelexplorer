#include "stdafx.h"
#include "CTransluscentRenderer.h"
#include "mywow.h"

CTransluscentRenderer::CTransluscentRenderer(u32 quota)
	: Quota(quota)
{
	RenderUnits.reserve(Quota);
	RenderEntries.reserve(Quota);
}

CTransluscentRenderer::~CTransluscentRenderer()
{
}

void CTransluscentRenderer::addRenderUnit(const SRenderUnit* unit)
{
	if ( !unit->material.isTransparent())
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

void CTransluscentRenderer::render(const SRenderUnit*& currentUnit, ICamera* cam)
{
	if (RenderUnits.empty())
		return;

	std::sort(RenderEntries.begin(), RenderEntries.end());

	IVideoDriver* driver = g_Engine->getDriver();
	driver->helper_render(this, currentUnit, cam);

	RenderUnits.clear();
	RenderEntries.clear();
}

void CTransluscentRenderer::begin_setupLightFog( ICamera* cam ) const
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

void CTransluscentRenderer::end_setupLightFog() const
{

}
