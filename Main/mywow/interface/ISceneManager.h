#pragma once

#include "core.h"
#include "ISceneNode.h"
#include "SColor.h"
#include "CFPSCounter.h"
#include "CTimer.h"

class ICamera;
class ISceneNode;
class IMeshSceneNode;
class IFileM2;
class IM2SceneNode;
class IFileWDT;
struct STile;
class CMapEnvironment;
class ISkySceneNode;
class IMapTileSceneNode;
class IFileWMO;
class IWMOSceneNode;
class IWDTSceneNode;
class ICoordSceneNode;

class IM2SceneNodeAddCallback;

#define MAX_SCENENODE_SEQUENCE	16

class ISceneManager 
{
public:
	ISceneManager() : BackgroundColor(64,64,64), ShowDebugBase(true), CalcPerf(false)
	{
		::memset(AreaName, 0, sizeof(AreaName));
		::memset(DebugText, 0, sizeof(DebugText));

		ActiveCamera = NULL_PTR;
		Timer = NULL_PTR;
	}

	virtual ~ISceneManager() {}

public:
	virtual void registerNodeForRendering(ISceneNode* node, bool visible, int sequence ) = 0;
	virtual bool beginFrame(bool foreground) = 0;
	virtual void endFrame() = 0;
	virtual void drawAll(bool foreground) = 0;
	virtual void addSceneNode(ISceneNode* node)= 0;
	virtual void removeSceneNode(ISceneNode* node, bool del) = 0;
	virtual void removeCamera( ICamera* cam ) = 0;
	virtual void removeAllSceneNodes() = 0;
	virtual void removeAllCameras() = 0;

	virtual void onWindowSizeChanged(const dimension2du& size) = 0;

	f32 getFPS() const { return FPSCounter.getFPS(); }
	u32 getTimeSinceLastFrame() const { return Timer->getTimeSinceLastFrame(); }

	void setActiveCamera(ICamera* camera) { ActiveCamera = camera; }
	ICamera* getActiveCamera() { return ActiveCamera; }

	virtual ICamera* addCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov) = 0;

	virtual ISkySceneNode* addSkySceneNode(CMapEnvironment* mapEnv) = 0;
	virtual ICoordSceneNode* addCoordSceneNode(ISceneNode* parent) = 0;
	virtual IMeshSceneNode*	addMeshSceneNode(const c8* name, ISceneNode* parent) = 0;
	virtual IM2SceneNode* addM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc = false) = 0;
	virtual IWMOSceneNode* addWMOSceneNode(IFileWMO* wmo, ISceneNode* parent) = 0;
	virtual IMapTileSceneNode* addMapTileSceneNode(IFileWDT* wdt, STile* tile, ISceneNode* parent) = 0;
	virtual IWDTSceneNode* addWDTSceneNode(IFileWDT* wdt, ISceneNode* parent) = 0;

	virtual void registerM2SceneNodeLoad(IM2SceneNodeAddCallback* callback) = 0;
	virtual void removeM2SceneNodeLoad(IM2SceneNodeAddCallback* callback) = 0;

public:
	SColor		BackgroundColor;
	c8		AreaName[MAX_TEXT_LENGTH];		//current area
	c16		DebugText[MAX_TEXT_LENGTH];
	bool			ShowDebugBase;
	bool		CalcPerf;

protected:
	ICamera*		ActiveCamera;
	CTimer*		Timer;
	CFPSCounter		FPSCounter;
};