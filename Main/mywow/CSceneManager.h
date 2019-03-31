#pragma once

#include "base.h"
#include "ISceneManager.h"
#include "CFPSCounter.h"
#include <list>

class IVideoDriver;
class CSceneRenderServices;
class IDrawServices;
class ITexture;
class IRenderTarget;
class CTimer;

class CSceneManager : public ISceneManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSceneManager);

public:
	CSceneManager();
	virtual ~CSceneManager();

public:
	virtual void registerNodeForRendering(ISceneNode* node, bool visible, int sequence );
	virtual bool beginFrame(bool foreground);
	virtual void endFrame();
	virtual void drawAll(bool foreground);
	virtual void addSceneNode(ISceneNode* node);
	virtual void removeSceneNode(ISceneNode* node, bool del);
	virtual void removeCamera(ICamera* cam);
	virtual void removeAllSceneNodes();
	virtual void removeAllCameras();

	virtual void onWindowSizeChanged(const dimension2du& size);

	virtual ICamera* addCamera(const vector3df& position, const vector3df& lookat, const vector3df& up, float nearValue, float farValue, float fov);

	virtual ISkySceneNode* addSkySceneNode(CMapEnvironment* mapEnv);
	virtual ICoordSceneNode* addCoordSceneNode(ISceneNode* parent);
	virtual IMeshSceneNode*	addMeshSceneNode(const char* name, ISceneNode* parent);
	virtual IM2SceneNode* addM2SceneNode(IFileM2* mesh, ISceneNode* parent, bool npc = false);
	virtual IWMOSceneNode* addWMOSceneNode(IFileWMO* wmo, ISceneNode* parent);
	virtual IMapTileSceneNode* addMapTileSceneNode(IFileWDT* wdt, STile* tile, ISceneNode* parent);
	virtual IWDTSceneNode* addWDTSceneNode(IFileWDT* wdt, ISceneNode* parent);

	virtual void registerM2SceneNodeLoad(IM2SceneNodeAddCallback* callback);
	virtual void removeM2SceneNodeLoad(IM2SceneNodeAddCallback* callback);

protected:
	void drawAreaInfo();

	void drawDebugText();

	void drawSceneInfo();

	void drawDebugTexture();

private:
	void renderNormal();
	void renderRT();

	void doRender();

protected:
	LENTRY		SceneNodeList[MAX_SCENENODE_SEQUENCE];			//scene nodes
	int		m_nCurSequence;

	typedef std::list<ICamera*, qzone_allocator<ICamera*>  > TCameras;
	TCameras		Cameras;

	typedef std::list<IM2SceneNodeAddCallback*, qzone_allocator<IM2SceneNodeAddCallback*>  > T_M2SceneNodeAddList;
	T_M2SceneNodeAddList	M2SceneNodeAddCallbackList;

	IVideoDriver*			Driver;
	CSceneRenderServices*			SceneRenderServices;
	IDrawServices*		DrawServices;
	IRenderTarget*		FrameRT;

	ITexture*		DebugTexture;

	char Text[MAX_TEXT_LENGTH];
	char SceneInfo[MAX_TEXT_LENGTH];

	//performance
	uint32_t		PerfCalcTime;
	uint32_t		Perf_GPUTime;
	uint32_t		Perf_tickTime;
	uint32_t		Perf_renderTime;
	uint32_t		Perf_terrainTime;
	uint32_t		Perf_wmoTime;
	uint32_t		Perf_meshTime;
	uint32_t		Perf_alphaTestTime;
	uint32_t		Perf_transparentTime;
	uint32_t		Perf_3DwireTime;
	uint32_t		Perf_2DTime;

};