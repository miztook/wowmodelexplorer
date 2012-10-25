#pragma once

#include "base.h"
#include "ISceneManager.h"
#include "CFPSCounter.h"
#include <list>

class IVideoDriver;
class ISceneRenderServices;
class IDrawServices;

class CSceneManager : public ISceneManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSceneManager);

public:
	CSceneManager();
	virtual ~CSceneManager();

public:
	virtual void registerNodeForRendering(ISceneNode* node);
	virtual void drawAll();
	virtual void removeSceneNode(ISceneNode* node);
	virtual void removeAllSceneNodes();
	virtual void removeAllCameras();

	virtual u32 getTimeSinceStart() const { return timeSinceStart; }
	virtual f32 getFPS() const { return FPSCounter.getFPS(); }

	virtual void setActiveCamera(ICamera* camera) { ActiveCamera = camera; }
	virtual ICamera* getActiveCamera() { return ActiveCamera; }

	virtual ICamera* addCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov);

	virtual IMeshSceneNode*	addMeshSceneNode(string64 name, ISceneNode* parent);
	virtual IM2SceneNode* addM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc = false);

protected:
	void calculateTime();

	void beginPerf(bool enable)
	{
		if(enable)
			QueryPerformanceCounter(&PerfStart);
	}

	void endPerf(bool enble, u32& time)
	{
		if (enble)
		{
			QueryPerformanceCounter(&PerfEnd);
			time = (u32)((PerfEnd.QuadPart - PerfStart.QuadPart) * 1000000 / PerfFreq.QuadPart);
		}
	}

	u32 getTime()
	{
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);
		return (u32)(time.QuadPart * 1000 / PerfFreq.QuadPart);
	}

	void drawAreaInfo();

protected:
	ICamera*		ActiveCamera;
	u32 timelastframe;
	u32		timeSinceStart, timeSinceLastFrame;

	typedef std::list<ISceneNode*, qzone_allocator<ISceneNode*>>	TSceneNodes;
	TSceneNodes		SceneNodes;

	typedef std::list<ICamera*, qzone_allocator<ICamera*>> TCameras;
	TCameras		Cameras;

	IVideoDriver*			Driver;
	ISceneRenderServices*			SceneRenderServices;
	IDrawServices*		DrawServices;

	c8 Text[MAX_TEXT_LENGTH];

	CFPSCounter		FPSCounter;

	//performance
	LARGE_INTEGER	PerfFreq;
	LARGE_INTEGER	PerfStart;
	LARGE_INTEGER	PerfEnd;
	u32		PerfCalcTime;

	u32		Perf_GPUTime;
	u32		Perf_registerTime, Perf_tickTime, Perf_renderTime;
	u32		Perf_terrainTime, Perf_wmoTime, Perf_doodadTime, Perf_meshTime, Perf_effectTime, Perf_wireTime;
};