#include "stdafx.h"
#include "CEditSceneManager.h"
#include "CEditM2SceneNode.h"
#include "CEditWMOSceneNode.h"
#include "CEditWDTSceneNode.h"
#include "CEditMapTileSceneNode.h"
#include "CSceneRenderServices.h"

CEditSceneManager::CEditSceneManager()
	: CSceneManager(), 
	DebugM2Node(NULL), ShowDebugM2Geosets(false),
	DebugWMONode(NULL),
	DebugWDTNode(NULL),
	DebugMapTileNode(NULL),
	BackTexture(NULL), BackImage(NULL), CenterTexture(NULL)
{
	memset(DebugMsg, 0, sizeof(DebugMsg));

	g_Engine->getManualMeshServices()->addSphere("$sphere8X4", 1.0f, 8, 4, SColor(255,255,0) );

	TextureOpaque = true;
}

CEditSceneManager::~CEditSceneManager()
{
	if (CenterTexture)
		CenterTexture->drop();

	if (BackTexture)
		BackTexture->drop();

	if (BackImage)
		BackImage->drop();
}

IM2SceneNode* CEditSceneManager::addM2SceneNode( IFileM2* mesh, ISceneNode* parent, bool npc )
{
	if (!mesh)
		return NULL;

	CEditM2SceneNode* node = new CEditM2SceneNode(mesh, parent, npc);
	mesh->drop();

	addSceneNode(node);

	//callback
	for (T_M2SceneNodeAddList::const_iterator itr = M2SceneNodeAddCallbackList.begin(); itr != M2SceneNodeAddCallbackList.end(); ++itr)
	{
		(*itr)->onM2SceneNodeAdd(node);
	}

	return node;
}

IWMOSceneNode* CEditSceneManager::addWMOSceneNode( IFileWMO* wmo, ISceneNode* parent )
{
	if (!wmo)
		return NULL;

	CEditWMOSceneNode* node = new CEditWMOSceneNode(wmo, parent);
	wmo->drop();

	addSceneNode(node);

	return node;
}

IWDTSceneNode* CEditSceneManager::addWDTSceneNode( IFileWDT* wdt, ISceneNode* parent )
{
	if (!wdt)
		return NULL;

	CEditWDTSceneNode* node = new CEditWDTSceneNode(wdt, parent);

	addSceneNode(node);

	return node;
}

IMapTileSceneNode* CEditSceneManager::addMapTileSceneNode( IFileWDT* wdt, STile* tile, ISceneNode* parent )
{
	if (!wdt)
		return NULL;

	CEditMapTileSceneNode* node = new CEditMapTileSceneNode(wdt, tile, parent);

	addSceneNode(node);

	return node;
}

void CEditSceneManager::drawM2GeosetsDebugInfo(CEditM2SceneNode* node)
{
	if (!node || !node->Mesh->Skin)
		return;

	vector2di pos = vector2di(5,300);
	node->getGeosetDebugInfo(DebugMsg, 512);
	g_Engine->getDefaultFont()->drawW(DebugMsg, SColor(255,255,0), pos);
}

void CEditSceneManager::removeSceneNode( ISceneNode* node, bool del )
{
	if (del)
	{
		if (node == DebugM2Node)
			DebugM2Node = NULL;

		if (node == DebugWMONode)
			DebugWMONode = NULL;

		if (node == DebugWDTNode)
			DebugWDTNode = NULL;

		if (node == DebugMapTileNode)
			DebugMapTileNode = NULL;
	}
	
	CSceneManager::removeSceneNode(node, del);
}

void CEditSceneManager::onWindowSizeChanged( const dimension2du& size )
{
 	if (size.Width > 0 && size.Height > 0 && Driver->checkValid())
 	{
 		if(BackTexture)
		{
 			BackTexture->drop();
			BackTexture = NULL;
		}
	 	if (BackImage)
			BackTexture = g_Engine->getManualTextureServices()->createTextureFromImage(size, BackImage, false);
	}
	CSceneManager::onWindowSizeChanged(size);
}

void CEditSceneManager::drawAll(bool foreground)
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

			_ASSERT(node->Parent == NULL);
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

void CEditSceneManager::renderNormal()
{
	//render
	if (!Driver->beginScene())
		return;

	Driver->clear(true, true, false, BackgroundColor);

	if (!ActiveCamera)
	{
		sprintf_s(Text, MAX_TEXT_LENGTH, "no camera");
		Driver->drawDebugInfo(Text);

		Driver->endScene();
		return;
	}

	doRender();

	Timer->beginPerf(CalcPerf);
	Driver->endScene();
	Timer->endPerf(CalcPerf, Perf_GPUTime);
}

void CEditSceneManager::renderRT()
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

	Driver->setRenderTarget(NULL);

	//draw screen texture
	vector2di upperLeft(0, 0);
	vector2di lowerRight((s32)FrameRT->getSize().Width, (s32)FrameRT->getSize().Height);
	recti rc = recti(upperLeft, lowerRight);

	//rc.LowerRightCorner = rc.UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);

	// 	recti rcDest[4];
	// 	rcDest[0].UpperLeftCorner = rc.UpperLeftCorner;
	// 	rcDest[0].LowerRightCorner = rcDest[0].UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);
	// 	rcDest[1].UpperLeftCorner = rc.UpperLeftCorner + vector2di(rc.getWidth()/2, 0);
	// 	rcDest[1].LowerRightCorner = rcDest[1].UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);
	// 	rcDest[2].UpperLeftCorner = rc.UpperLeftCorner + vector2di(0, rc.getHeight()/2);
	// 	rcDest[2].LowerRightCorner = rcDest[2].UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);
	// 	rcDest[3].UpperLeftCorner = rc.UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);
	// 	rcDest[3].LowerRightCorner = rcDest[3].UpperLeftCorner + vector2di(rc.getWidth()/2, rc.getHeight()/2);
	// 
	// 	const recti* rcDestArray[] = {&rcDest[0], &rcDest[1], &rcDest[2], &rcDest[3] };
	// 
	// 	g_Engine->getDrawServices()->draw2DImageRectBatch(RenderTarget->getRTTexture(), rcDestArray, NULL, 4);


	E_DRIVER_TYPE dType = Driver->getDriverType();
	if(dType == EDT_OPENGL)
		g_Engine->getDrawServices()->draw2DImageRect(FrameRT->getRTTexture(), &rc, NULL, SColor(), ERU_01_10);
	else
		g_Engine->getDrawServices()->draw2DImageRect(FrameRT->getRTTexture(), &rc);

	Timer->beginPerf(CalcPerf);
	Driver->endScene();
	Timer->endPerf(CalcPerf, Perf_GPUTime);
}

void CEditSceneManager::doRender()
{
	drawBackTexture();

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

	//doodad
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_DOODAD, ActiveCamera);
	SceneRenderServices->renderAll(ERT_MESH, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_meshTime);

	//sky
	SceneRenderServices->renderAll(ERT_SKY, ActiveCamera);

	//m2 debug
	if (DebugM2Node)
	{
		CEditM2SceneNode* node = (CEditM2SceneNode*)DebugM2Node;

		bool modelCamera = node->getModelCamera() != -1;
		if (modelCamera)
		{
			OldView = Driver->getTransform(ETS_VIEW);
			OldProjection = Driver->getTransform(ETS_PROJECTION);
			Driver->setTransform(ETS_VIEW, node->CurrentView);
			Driver->setTransform(ETS_PROJECTION, node->CurrentProjection);
		}

		if (node->ShowBoundingBox)
			node->drawBoundingBox(SColor(255,0,0));

		if(node->ShowBoundingAABox)
			node->drawBoundingAABox(SColor(0, 255, 0));

		if (node->ShowCollisionAABox)
			node->drawCollisionAABox(SColor(255, 255, 0));

		if (node->ShowBones)
			node->drawBones(SColor(0,0,255));

		DrawServices->flushAll3DLines(modelCamera ? NULL : ActiveCamera);

		if (modelCamera)
		{
			Driver->setTransform(ETS_PROJECTION, OldProjection);
			Driver->setTransform(ETS_VIEW, OldView);
		}
	}

	//wmo debug
	if (DebugWMONode)
	{
		CEditWMOSceneNode* node = (CEditWMOSceneNode*)DebugWMONode;

		node->drawPortals();

		node->drawBoundingBox();

		DrawServices->flushAll3DLines(ActiveCamera);
	}

	//alpha test
	Timer->beginPerf(CalcPerf);
	SceneRenderServices->renderAll(ERT_ALPHATEST, ActiveCamera);
	Timer->endPerf(CalcPerf, Perf_alphaTestTime);

	//transparent
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
	drawCenterTexture();				//
	DrawServices->flushAll2DQuads();
	DrawServices->flushAll2DLines();
	g_Engine->getDefaultFont()->flushText();
	Timer->endPerf(CalcPerf, Perf_2DTime);

	if (DebugM2Node)
	{
		CEditM2SceneNode* node = (CEditM2SceneNode*)DebugM2Node;

		if (ShowDebugM2Geosets)
			drawM2GeosetsDebugInfo(node);
	}

	if (ShowDebugBase)
	{
		Q_sprintf(Text, MAX_TEXT_LENGTH, 
			"\ntick time: %u\nrender time: %u\nterrain: %u\nwmo: %u\nmesh: %u\nalphatest: %u\ntransparent: %u\n3Dwire: %u\n2D: %u\nGPU time: %u\n",
			Perf_tickTime, Perf_renderTime, Perf_terrainTime, Perf_wmoTime, Perf_meshTime, Perf_alphaTestTime, Perf_transparentTime, Perf_3DwireTime, Perf_2DTime, Perf_GPUTime);

		Driver->drawDebugInfo(Text);
	}

	drawAreaInfo();

	//drawSceneInfo();
}

void CEditSceneManager::setBackImage( const c8* filename )
{	
	BackImageFileName = filename;
	
	if (BackImage)
		BackImage->drop();

	BackImage = g_Engine->getResourceLoader()->loadJPGAsImage(filename);
	if (!BackImage)
		BackImage = g_Engine->getResourceLoader()->loadPNGAsImage(filename);

	const dimension2du& size = g_Engine->getDriver()->getDisplayMode();

	if (BackTexture)
		BackTexture->drop();

	if (BackImage)
		BackTexture = g_Engine->getManualTextureServices()->createTextureFromImage(size, BackImage, false);
	else
		BackTexture = NULL;
}

void CEditSceneManager::setCenterTexture(const c8* filename)
{
	if(CenterTexture)
	{
		CenterTexture->drop();
		CenterTexture = NULL;
	}
	ITexture* tex = g_Engine->getResourceLoader()->loadTexture(filename, false);
	if(!tex)
		return;
	tex->createVideoTexture();
	CenterTexture = tex;
}

void CEditSceneManager::drawBackTexture()
{
	if (BackTexture)
		DrawServices->draw2DImage(BackTexture, vector2di(0,0), NULL, SColor(), ERU_00_11, 1.0f);
}

void CEditSceneManager::drawCenterTexture()
{
	if (CenterTexture)
	{
		dimension2du texSize = CenterTexture->getSize();
		dimension2di backBufferSize = g_Engine->getDriver()->getViewPort().getSize();

		int x = backBufferSize.Width / 2 - (int)texSize.Width / 2;
		int y = backBufferSize.Height / 2 - (int)texSize.Height / 2;
		DrawServices->draw2DImage(CenterTexture, vector2di(x, y), NULL, SColor(), ERU_00_11, 1.0f, 
			TextureOpaque ? S2DBlendParam::OpaqueSource() : S2DBlendParam::UITextureBlendParam());
	}
}
