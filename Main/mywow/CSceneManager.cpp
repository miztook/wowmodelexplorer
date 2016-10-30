#include "stdafx.h"
#include "CSceneManager.h"
#include "mywow.h"
#include "CTimer.h"
#include "CCamera.h"
#include "CSceneRenderServices.h"
#include "CMeshSceneNode.h"
#include "CM2SceneNode.h"
#include "CMapTileSceneNode.h"
#include "CWMOSceneNode.h"
#include "CWDTSceneNode.h"
#include "CSkySceneNode.h"
#include "CCoordSceneNode.h"
#include "CFTFont.h"

CSceneManager::CSceneManager( )
	: PerfCalcTime(0), 
	Perf_tickTime(0), Perf_renderTime(0), 
	Perf_terrainTime(0), Perf_wmoTime(0), Perf_meshTime(0),
	Perf_alphaTestTime(0), Perf_transparentTime(0), Perf_3DwireTime(0), Perf_2DTime(0),
	Perf_GPUTime(0), FrameRT(NULL_PTR)
{
	::memset(Text, 0, sizeof(Text));
	::memset(SceneInfo, 0, sizeof(SceneInfo));

	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		InitializeListHead(&SceneNodeList[i]);
	}

	Driver = g_Engine->getDriver();
	SceneRenderServices = static_cast<CSceneRenderServices*>(g_Engine->getSceneRenderServices());
	DrawServices = g_Engine->getDrawServices();

	DebugTexture = NULL_PTR;
	
// 	IImage* image = g_Engine->getResourceLoader()->loadJPGAsImage("D:\\myeditor\\2.jpg");
// 	if (image)
// 	{
// 		DebugTexture = g_Engine->getManualTextureServices()->createTextureFromImage(dimension2du(800,600), image, false);
// 		image->drop();
// 	}

//	DebugTexture = g_Engine->getResourceLoader()->loadTexture("Item\\ObjectComponents\\Shoulder\\shoulder_leather_raiddruid_l_01_yellow.blp", true);
//  	DebugTexture = g_Engine->getResourceLoader()->loadTexture("Tileset\\Durotar\\DurotarDirt_s.blp", false);
 //	DebugTexture->createVideoTexture();

	Timer = g_Engine->getTimer();

	dimension2du size = g_Engine->getDriver()->getDisplayMode();

#ifdef MW_USE_FRAME_RT
	FrameRT = g_Engine->getManualTextureServices()->addRenderTarget(size, ECF_UNKNOWN, ECF_D16);
#endif

	m_nCurSequence = 0;
}

CSceneManager::~CSceneManager()
{
#ifdef MW_USE_FRAME_RT
	g_Engine->getManualTextureServices()->removeRenderTarget(FrameRT);
#endif

	if (DebugTexture)
		DebugTexture->drop();

	removeAllSceneNodes();
	removeAllCameras();
}

void CSceneManager::registerNodeForRendering( ISceneNode* node, bool visible, int sequence )
{
	ASSERT(node);
	SceneRenderServices->addSceneNode(node, visible, sequence);
}

bool CSceneManager::beginFrame(bool foreground)
{
	Timer->calculateTime();

#ifndef MW_PLATFORM_IOS
	const engineSetting::SAdvancedSetting& setting = g_Engine->getEngineSetting()->getAdvcanedSetting();

	//frame limit
	s32 limit = foreground ? setting.maxForegroundFPS : setting.maxBackgroundFPS;
	if (limit > 0 && !Timer->checkFrameLimit(limit))
		return false;
#endif
    
	return true;
}

void CSceneManager::endFrame()
{
	FPSCounter.registerFrame( Timer->getMillisecond() );
}

void CSceneManager::drawAll(bool foreground)
{
	u32 timeSinceStart = Timer->getTimeSinceStart();
	u32 timeSinceLastFrame = Timer->getTimeSinceLastFrame();

	CalcPerf = false;
	if (timeSinceStart - PerfCalcTime > 1000)
	{
		CalcPerf = true;
		PerfCalcTime = timeSinceStart;
	}
	
	SceneRenderServices->clearAllSceneNodes();

	//3d mode	
	// normal nodes
	m_nCurSequence = 0;
	Timer->beginPerf(CalcPerf);
	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		m_nCurSequence = i+1;

		for(PLENTRY e = SceneNodeList[i].Flink; e != &SceneNodeList[i]; )
		{
			ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
			e = e->Flink;

			ASSERT(node->Parent == NULL_PTR);
			node->registerSceneNode(true, i);
		}

		SceneRenderServices->tickAllSceneNodes(timeSinceStart, timeSinceLastFrame, i);
	}
	m_nCurSequence = 0;

	Timer->endPerf(CalcPerf, Perf_tickTime);

	if (foreground && Driver->checkValid())
	{
		Timer->beginPerf(CalcPerf);
		SceneRenderServices->renderAllSceneNodes();
		Timer->endPerf(CalcPerf, Perf_renderTime);

		//render
#ifdef MW_USE_FRAME_RT
		renderRT();
#else
		renderNormal();
#endif
	}
	
#ifdef MW_USE_AUDIO
	g_Engine->getAudioPlayer()->tickFadeOutSounds(timeSinceLastFrame);
#endif

	g_Engine->getParticleSystemServices()->adjustParticles();
}

void CSceneManager::renderNormal()
{
	if (!Driver->beginScene())
		return;

	Driver->clear(true, true, false, BackgroundColor);

	if (!ActiveCamera)
	{
		Q_sprintf(Text, MAX_TEXT_LENGTH, "no camera");
		Driver->drawDebugInfo(Text);

		Driver->endScene();
		return;
	}

	doRender();
	
	Timer->beginPerf(CalcPerf);
	Driver->endScene();
	Timer->endPerf(CalcPerf, Perf_GPUTime);
}

void CSceneManager::renderRT()
{
	if (!Driver->beginScene())
		return;

	//Driver->clear(true, true, false, BackgroundColor);

	if(!Driver->setRenderTarget(FrameRT))
		return;

	Driver->clear(true, true, false, BackgroundColor);

	if (!ActiveCamera)
	{
		Q_sprintf(Text, MAX_TEXT_LENGTH, "no camera");
		Driver->drawDebugInfo(Text);
	}
    else
    {
		doRender();
	}
    
	//render target to screen texture
	FrameRT->writeToRTTexture();

	Driver->setRenderTarget(NULL_PTR);

	//draw screen texture
	vector2di upperLeft(0, 0);
	vector2di lowerRight((s32)FrameRT->getSize().Width, (s32)FrameRT->getSize().Height);
	recti rc = recti(upperLeft, lowerRight);

	E_DRIVER_TYPE dType = Driver->getDriverType();
	if(dType == EDT_OPENGL || dType == EDT_GLES2)
		g_Engine->getDrawServices()->draw2DImageRect(FrameRT->getRTTexture(), &rc, NULL_PTR, SColor(), ERU_01_10);
	else
		g_Engine->getDrawServices()->draw2DImageRect(FrameRT->getRTTexture(), &rc);

	Timer->beginPerf(CalcPerf);
	Driver->endScene();
	Timer->endPerf(CalcPerf, Perf_GPUTime);
}

void CSceneManager::doRender()
{
	//clip plane for terrain and wmo
	f32 clip = SceneRenderServices->getClipDistance();
	if (ActiveCamera->getClipDistance() != clip)
		ActiveCamera->setClipDistance(clip);

	//terrain
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_TERRAIN, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_terrainTime);

	//wmo
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_WMO, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_wmoTime);

	//doodad, mesh
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_DOODAD, ActiveCamera);
	SceneRenderServices->renderAll(ERT_MESH, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_meshTime);

	//sky
	SceneRenderServices->renderAll(ERT_SKY, ActiveCamera);

	//alpha test
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_ALPHATEST, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_alphaTestTime);

	//alpha
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_TRANSPARENT, ActiveCamera);

	SceneRenderServices->renderAll(ERT_PARTICLE, ActiveCamera);

	SceneRenderServices->renderAll(ERT_RIBBON, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_transparentTime);

	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_WIRE, ActiveCamera);
	DrawServices->flushAll3DLines(ActiveCamera);
	DrawServices->flushAll3DVertices(ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_3DwireTime);

	Timer->beginPerf(CalcPerf);
	DrawServices->flushAll2DQuads();
	DrawServices->flushAll2DLines();
	g_Engine->getDefaultFont()->flushText();
	Timer->endPerf(CalcPerf, Perf_2DTime);

	if (ShowDebugBase)
	{
		Q_sprintf(Text, MAX_TEXT_LENGTH, 
			"\ntick time: %u\nrender time: %u\nterrain: %u\nwmo: %u\nmesh: %u\nalphatest: %u\ntransparent: %u\n3Dwire: %u\n2D: %u\nGPU time: %u\n",
			Perf_tickTime, Perf_renderTime, Perf_terrainTime, Perf_wmoTime, Perf_meshTime, Perf_alphaTestTime, Perf_transparentTime, Perf_3DwireTime, Perf_2DTime, Perf_GPUTime);

		Driver->drawDebugInfo(Text);
	}

	drawAreaInfo();

	//drawDebutText();

	drawSceneInfo();
	//drawDebugTexture();
}

void CSceneManager::addSceneNode( ISceneNode* node)
{
	if (!node)
		return;

	if (m_nCurSequence >= MAX_SCENENODE_SEQUENCE)
	{
		ASSERT(false);
		return;
	}

	if (!node->Parent)
		InsertTailList(&SceneNodeList[m_nCurSequence], &node->Link);
}

void CSceneManager::removeSceneNode( ISceneNode* node, bool del )
{
	if (!node)
		return;

	SceneRenderServices->skipSceneNode(node);

	if (node->Parent)
		node->Parent->removeChild(node);
	else
		RemoveEntryList(&node->Link);

	if(del)
	{
		node->removeAllChildren();
		delete node;
	}
}

void CSceneManager::removeAllSceneNodes()
{
	for (int i=0; i<MAX_SCENENODE_SEQUENCE; ++i)
	{
		for(PLENTRY e = SceneNodeList[i].Blink; e != &SceneNodeList[i]; )
		{
			ISceneNode* node = reinterpret_cast<ISceneNode*>CONTAINING_RECORD(e, ISceneNode, Link);
			e = e->Blink;

			ASSERT(node->Parent == NULL_PTR);
			RemoveEntryList(&node->Link);
			node->removeAllChildren();
			delete node;
		}

		ASSERT(IsListEmpty(&SceneNodeList[i]));
		InitializeListHead(&SceneNodeList[i]);
	}
}


void CSceneManager::removeCamera( ICamera* cam )
{
	for ( TCameras::iterator itr = Cameras.begin(); itr!=Cameras.end(); )
	{
		if (*itr == cam)
		{
			delete (*itr);
			itr = Cameras.erase(itr);
		}
		else
		{
			++itr;
		}
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

ICamera* CSceneManager::addCamera( const vector3df& position, const vector3df& lookat, const vector3df& up, f32 nearValue, f32 farValue, f32 fov )
{
	ICamera* camera = new CCamera(position, lookat, up, nearValue, farValue, fov);
	Cameras.push_back(camera);
	ActiveCamera = camera;

	return camera;
}


ISkySceneNode* CSceneManager::addSkySceneNode(CMapEnvironment* mapEnv)
{
	CSkySceneNode* node = new CSkySceneNode(mapEnv);

	addSceneNode(node);

	return node;
}

ICoordSceneNode* CSceneManager::addCoordSceneNode(ISceneNode* parent)
{
	CCoordSceneNode* node = new CCoordSceneNode(parent);

	addSceneNode(node);

	return node;
}

IMeshSceneNode* CSceneManager::addMeshSceneNode( const c8* name, ISceneNode* parent)
{
	IMesh* m = g_Engine->getManualMeshServices()->getMesh(name);
	if (!m)
		return NULL_PTR;

	CMeshSceneNode* node = new CMeshSceneNode(m, parent);

	addSceneNode(node);

	return node;
}

IM2SceneNode* CSceneManager::addM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc/* = false*/ )
{
	if (!mesh)
		return NULL_PTR;

	CM2SceneNode* node = new CM2SceneNode(mesh, parent, npc);
	mesh->drop();

	addSceneNode(node);

	//callback
	for (T_M2SceneNodeAddList::const_iterator itr = M2SceneNodeAddCallbackList.begin(); itr != M2SceneNodeAddCallbackList.end(); ++itr)
	{
		(*itr)->onM2SceneNodeAdd(node);
	}

	return node;
}

IMapTileSceneNode* CSceneManager::addMapTileSceneNode( IFileWDT* wdt, STile* tile, ISceneNode* parent )
{
	if (!tile)
		return NULL_PTR;

	CMapTileSceneNode* node = new CMapTileSceneNode(wdt, tile, parent);

	addSceneNode(node);

	return node;
}

IWMOSceneNode* CSceneManager::addWMOSceneNode( IFileWMO* wmo, ISceneNode* parent )
{
	if (!wmo)
		return NULL_PTR;

	CWMOSceneNode* node = new CWMOSceneNode(wmo, parent);
	wmo->drop();

	addSceneNode(node);

	return node;
}

void CSceneManager::drawAreaInfo()
{
	if (!Q_strlen(AreaName))
		return;

	CFTFont* ftFont = static_cast<CFTFont*>(g_Engine->getDefaultFont());

	c16 str16[MAX_TEXT_LENGTH/4];
	utf8to16(AreaName, str16, MAX_TEXT_LENGTH/4);

	s32 center = Driver->getViewPort().getWidth() / 2;
	dimension2du szAreaName = ftFont->getWTextExtent(str16);
	ftFont->drawW(str16, SColor(255,255,0), vector2di(center - szAreaName.Width/2, 0));
}

void CSceneManager::drawDebugText()
{
	if (!utf16len(DebugText))
		return;

	CFTFont* ftFont = static_cast<CFTFont*>(g_Engine->getDefaultFont());

	s32 center = Driver->getViewPort().getWidth() / 2;
	dimension2du szDebugText = ftFont->getWTextExtent(DebugText);
	ftFont->drawW(DebugText, SColor(255,255,0), vector2di(center - szDebugText.Width/2, 0));
}

void CSceneManager::drawSceneInfo()
{
	const vector3df& Pos = ActiveCamera->getPosition();
	vector3df& Dir = g_Engine->getSceneEnvironment()->LightDir;
	

	Q_sprintf(SceneInfo, MAX_TEXT_LENGTH, 
		"camera pos: (%0.2f, %0.2f, %0.2f)\nlight dir: (%0.2f, %0.2f, %0.2f)\nactive particles: %d\navailable main memory: %0.4f\navailable small memory: %0.4f\navailable temp memory: %0.4f",
		Pos.X, Pos.Y, Pos.Z,
		Dir.X, Dir.Y, Dir.Z,
		g_Engine->getParticleSystemServices()->getActiveParticlesCount(),
		Z_AvailableMainMemoryPercent(), Z_AvailableSmallMemoryPercent(), Z_AvailableTempMemoryPercent() );

	s32 right = Driver->getViewPort().getWidth() - 300;
	g_Engine->getDefaultFont()->drawA(SceneInfo, SColor(255,255,0), vector2di(right, 0));
}

IWDTSceneNode* CSceneManager::addWDTSceneNode( IFileWDT* wdt, ISceneNode* parent )
{
	if (!wdt)
		return NULL_PTR;

	CWDTSceneNode* node = new CWDTSceneNode(wdt, parent);

	addSceneNode(node);

	return node;
}

void CSceneManager::drawDebugTexture()
{
	if (DebugTexture)
	{
		DrawServices->draw2DImage(DebugTexture, vector2di(0,0), NULL_PTR, SColor(), ERU_00_11, 1.0f);
	}
}

void CSceneManager::onWindowSizeChanged( const dimension2du& size )
{
// 	if (size.Width > 0 && size.Height > 0 && Driver->checkValid())
// 	{
// 		if(DebugTexture)
// 			DebugTexture->drop();
// 		IImage* image = g_Engine->getResourceLoader()->loadJPGAsImage("D:\\myeditor\\2.jpg");
// 		if (image)
// 		{
// 			DebugTexture = g_Engine->getManualTextureServices()->createTextureFromImage(size, image, false);
// 			image->drop();
// 		}
// 	}

#ifdef MW_USE_FRAME_RT
	if (size.Width > 0 && size.Height > 0 && Driver->checkValid())
	{
		g_Engine->getManualTextureServices()->removeRenderTarget(FrameRT);
		FrameRT = g_Engine->getManualTextureServices()->addRenderTarget(size, ECF_UNKNOWN, ECF_D16);
	}
#endif
}

void CSceneManager::registerM2SceneNodeLoad( IM2SceneNodeAddCallback* callback )
{
	if (std::find(M2SceneNodeAddCallbackList.begin(), M2SceneNodeAddCallbackList.end(), callback) == M2SceneNodeAddCallbackList.end())
	{
		M2SceneNodeAddCallbackList.push_back(callback);
	}
}

void CSceneManager::removeM2SceneNodeLoad( IM2SceneNodeAddCallback* callback )
{
	M2SceneNodeAddCallbackList.remove(callback);
}




