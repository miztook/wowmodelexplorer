#include "stdafx.h"
#include "CSceneManager.h"
#include "mywow.h"
#include "CCamera.h"
#include "CMeshSceneNode.h"
#include "CM2SceneNode.h"

CSceneManager::CSceneManager( )
	: PerfCalcTime(0), 
	Perf_registerTime(0), Perf_tickTime(0), Perf_renderTime(0), 
	Perf_terrainTime(0), Perf_wmoTime(0), Perf_doodadTime(0), Perf_meshTime(0), Perf_effectTime(0), Perf_wireTime(0),
	Perf_GPUTime(0)
{
	ActiveCamera = NULL;
	Driver = g_Engine->getDriver();
	SceneRenderServices = g_Engine->getSceneRenderServices();
	DrawServices = g_Engine->getDrawServices();

	::QueryPerformanceFrequency(&PerfFreq);
}

CSceneManager::~CSceneManager()
{
	removeAllSceneNodes();
	removeAllCameras();
}

void CSceneManager::registerNodeForRendering( ISceneNode* node )
{
	_ASSERT(node);
	SceneRenderServices->addSceneNode(node);
}

void CSceneManager::drawAll()
{
	calculateTime();

	bool calcPerf = false;
	if (timeSinceStart - PerfCalcTime > 1000)
	{
		calcPerf = true;
		PerfCalcTime = timeSinceStart;
	}
	
	//3d mode	
	beginPerf(calcPerf);
	//visible nodes...
	for (TSceneNodes::iterator itr = SceneNodes.begin(); itr != SceneNodes.end(); ++itr)
	{
		_ASSERT((*itr)->Parent == NULL);
		(*itr)->registerSceneNode(true);
	}
	endPerf(calcPerf, Perf_registerTime);

	beginPerf(calcPerf);
	SceneRenderServices->tickAllSceneNodes(timeSinceStart, timeSinceLastFrame);
	endPerf(calcPerf, Perf_tickTime);

	//draw
	if (!Driver->beginScene())
		return;

	Driver->clear(true, true, false, BackgroundColor);

	if (!ActiveCamera)
	{
		sprintf_s(Text, MAX_TEXT_LENGTH, "无摄像机");
		Driver->drawDebugInfo(Text);

		Driver->endScene();
		return;
	}

	beginPerf(calcPerf);
	SceneRenderServices->renderAllSceneNodes();
	endPerf(calcPerf, Perf_renderTime);

	//terrain
	beginPerf(calcPerf);
	SceneRenderServices->begin_setupLightFog(ERT_TERRAIN, ActiveCamera);
	SceneRenderServices->renderAll(ERT_TERRAIN, ActiveCamera);
	SceneRenderServices->end_setupLightFog(ERT_TERRAIN);
	endPerf(calcPerf, Perf_terrainTime);

	//wmo
	beginPerf(calcPerf);
	SceneRenderServices->begin_setupLightFog(ERT_WMO, ActiveCamera);
	SceneRenderServices->renderAll(ERT_WMO, ActiveCamera);
	SceneRenderServices->end_setupLightFog(ERT_WMO);
	endPerf(calcPerf, Perf_wmoTime);

	//doodad
	beginPerf(calcPerf);
	SceneRenderServices->begin_setupLightFog(ERT_DOODAD, ActiveCamera);
	SceneRenderServices->renderAll(ERT_DOODAD, ActiveCamera);
	SceneRenderServices->end_setupLightFog(ERT_DOODAD);
	endPerf(calcPerf, Perf_doodadTime);

	beginPerf(calcPerf);
	SceneRenderServices->begin_setupLightFog(ERT_MESH, ActiveCamera);
	SceneRenderServices->renderAll(ERT_MESH, ActiveCamera);
	SceneRenderServices->end_setupLightFog(ERT_MESH);
	endPerf(calcPerf, Perf_meshTime);

	SceneRenderServices->renderAll(ERT_SKY, ActiveCamera);

	beginPerf(calcPerf);
	SceneRenderServices->begin_setupLightFog(ERT_EFFECT, ActiveCamera);
	SceneRenderServices->renderAll(ERT_EFFECT, ActiveCamera);
	SceneRenderServices->end_setupLightFog(ERT_EFFECT);
	endPerf(calcPerf, Perf_effectTime);
	
	beginPerf(calcPerf);
	SceneRenderServices->renderAll(ERT_WIRE, ActiveCamera);
	DrawServices->flushAll3DLines(ActiveCamera);
	DrawServices->flushAll2DLines();
	endPerf(calcPerf, Perf_wireTime);

	if (ShowDebugBase)
	{
		vector3df& Pos = ActiveCamera->Position;

// 		sprintf_s(Text, MAX_TEXT_LENGTH, "摄像机位置: (%0.2f, %0.2f, %0.2f)\n活动粒子数:%d\n", 
// 			Pos.X, Pos.Y, Pos.Z,
// 			g_Engine->getParticleSystemServices()->getActiveParticlesCount());

		sprintf_s(Text, MAX_TEXT_LENGTH, 
			"\nregister time: %d\ntick time: %d\nrender time: %d\nterrain: %d\nwmo: %d\ndoodad: %d\nmesh: %d\neffect: %d\nwire: %d\nGPU time: %d\n",
			Perf_registerTime, Perf_tickTime, Perf_renderTime, Perf_terrainTime, Perf_wmoTime, Perf_doodadTime, Perf_meshTime, Perf_effectTime, Perf_wireTime, Perf_GPUTime);

// 		sprintf_s(Text, MAX_TEXT_LENGTH, 
// 			"\nmain percent:%0.2f\nsmall percent:%0.2f\nrender percent:%0.2f\n",
// 			Z_AvailableMainMemoryPercent(), Z_AvailableSmallMemoryPercent(), Z_AvailableRenderMemoryPercent());
		Driver->drawDebugInfo(Text);
	}

	drawAreaInfo();

	beginPerf(calcPerf);
	Driver->endScene();
	endPerf(calcPerf, Perf_GPUTime);

	FPSCounter.registerFrame( getTime() );
}

void CSceneManager::removeSceneNode( ISceneNode* node )
{
	for ( TSceneNodes::iterator itr = SceneNodes.begin(); itr!=SceneNodes.end(); ++itr )
	{
		if ((*itr) == node)
		{
			(*itr)->removeAllChildren();
			delete (*itr);
			SceneNodes.erase(itr);
			break;
		}	
	}
}

void CSceneManager::removeAllSceneNodes()
{
	for ( TSceneNodes::iterator itr = SceneNodes.begin(); itr!=SceneNodes.end(); )
	{
		(*itr)->removeAllChildren();
		delete (*itr);
		itr = SceneNodes.erase(itr);
	}
}

void CSceneManager::removeAllCameras()
{
	for ( TCameras::iterator itr = Cameras.begin(); itr!=Cameras.end(); )
	{
		delete (*itr);
		itr = Cameras.erase(itr);
	}
}

void CSceneManager::calculateTime()
{
	//time
	u32 tick = getTime();

	static bool bFirst = true;	
	if ( bFirst || tick - timelastframe > 2000)
	{
		timelastframe = tick;
		timeSinceStart = timeSinceLastFrame = 0;
		bFirst = false;
	}
	
	timeSinceLastFrame = tick - timelastframe;
	timeSinceStart += timeSinceLastFrame;
	timelastframe = tick;
}

ICamera* CSceneManager::addCamera( const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov )
{
	ICamera* camera = new CCamera(position, lookat, up, nearValue, farValue, fov);
	Cameras.push_back(camera);
	ActiveCamera = camera;

	return camera;
}

IMeshSceneNode* CSceneManager::addMeshSceneNode( string64 name, ISceneNode* parent /*= NULL*/ )
{
	IMesh* m = g_Engine->getManualMeshServices()->getMesh(name);
	if (!m)
		return NULL;

	CMeshSceneNode* node = new CMeshSceneNode(m, parent);

	if (!parent)
		SceneNodes.push_back(node);

	return node;
}

IM2SceneNode* CSceneManager::addM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc/* = false*/ )
{
	if (!mesh)
		return NULL;

	CM2SceneNode* node = new CM2SceneNode(mesh, parent, npc);
	mesh->drop();

	mesh->buildVideoResources();

	if (!parent)
		SceneNodes.push_back(node);

	return node;
}

void CSceneManager::drawAreaInfo()
{
	if (!wcslen(AreaName))
		return;

	s32 center = Driver->getViewPort().getWidth() / 2;
	dimension2du szAreaName = g_Engine->getFont()->getDimension(AreaName);
	g_Engine->getFont()->drawW(AreaName, SColor(255,255,0), vector2di(center - szAreaName.Width/2, 0));

	if (wcslen(AreaDebug))
	{
		dimension2du szAreaDebug = g_Engine->getFont()->getDimension(AreaDebug);
		g_Engine->getFont()->drawW(AreaDebug, SColor(255,255,0), vector2di(center - szAreaDebug.Width/2, szAreaName.Height + 2));
	}
}

