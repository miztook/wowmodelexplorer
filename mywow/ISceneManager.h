#pragma once

#include "core.h"
#include "ISceneNode.h"
#include "SColor.h"

class ICamera;
class ISceneNode;
class IMeshSceneNode;
class IFileM2;
class IM2SceneNode;
class IFileWDT;
struct STile;
class IMapTileSceneNode;
class IFileWMO;
class IWMOSceneNode;
class IWDTSceneNode;

class ISceneManager 
{
public:
	ISceneManager() : BackgroundColor(64,64,64), ShowDebugBase(true) 
	{ ::memset(AreaName, 0, sizeof(c16)*MAX_TEXT_LENGTH);
	 ::memset(AreaDebug, 0, sizeof(c16)*MAX_TEXT_LENGTH); }

	virtual ~ISceneManager() {}

public:
	virtual void registerNodeForRendering(ISceneNode* node) = 0;
	virtual void drawAll() = 0;
	virtual void removeSceneNode(ISceneNode* node) = 0;
	virtual void removeAllSceneNodes() = 0;
	virtual void removeAllCameras() = 0;

	virtual u32 getTimeSinceStart() const = 0;
	virtual f32 getFPS() const = 0;

	virtual void setActiveCamera(ICamera* camera) = 0;
	virtual ICamera* getActiveCamera() = 0;

	virtual ICamera* addCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov) = 0;

	virtual IMeshSceneNode*	addMeshSceneNode(string64 name, ISceneNode* parent) = 0;
	virtual IM2SceneNode* addM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc= false) = 0;

public:
	SColor		BackgroundColor;
	bool			ShowDebugBase;
	c16	 AreaName[MAX_TEXT_LENGTH];		//current area
	c16	 AreaDebug[MAX_TEXT_LENGTH];
};