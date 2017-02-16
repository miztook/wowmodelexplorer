#include "StdAfx.h"
#include "engine.h"
#include "mywow.h"
#include "COSInfo.h"
#include "CTimer.h"
#include "CFileSystem.h"

//dx9
#include "CD3D9Driver.h"
#include "CD3D9HardwareBufferServices.h"
#include "CD3D9DrawServices.h"
#include "CD3D9ResourceLoader.h"
#include "CD3D9TextureWriteServices.h"
#include "CD3D9ManualTextureServices.h"
#include "CD3D9Helper.h"

//dx11
#include "CD3D11Driver.h"
#include "CD3D11HardwareBufferServices.h"
#include "CD3D11DrawServices.h"
#include "CD3D11ResourceLoader.h"
#include "CD3D11TextureWriteServices.h"
#include "CD3D11ManualTextureServices.h"
#include "CD3D11Helper.h"

//opengl
#include "COpenGLDriver.h"
#include "COpenGLHardwareBufferServices.h"
#include "COpenGLDrawServices.h"
#include "COpenGLResourceLoader.h"
#include "COpenGLTextureWriteServices.h"
#include "COpenGLManualTextureServices.h"
#include "COpenGLHelper.h"

#ifdef MW_PLATFORM_WINDOWS
#include "CDSAudioPlayer.h"
#include "CX2AudioPlayer.h"
#include "CDxInputReader.h"
#endif

#include "CInputReader.h"
#include "CGestureReader.h"
#include "CFontServices.h"
#include "CGeometryCreator.h"
#include "CManualMeshServices.h"
#include "CSpecialTextureServices.h"
#include "CParticleSystemServices.h"
#include "CRibbonEmitterServices.h"
#include "CMeshDecalServices.h"
#include "CTerrainServices.h"
#include "CSceneRenderServices.h"
#include "CSceneManager.h"
#include "CSceneEnvironment.h"

Engine*		g_Engine = NULL_PTR;

Engine::Engine(const SEngineInitParam& param, const SWindowInfo& wndInfo) : MessageHandler(NULL_PTR)
{
	WindowInfo = wndInfo;

#ifdef MW_PLATFORM_WINDOWS
	QMem_Init(2, 100, 10);	
#else
	QMem_Init(1, 4, 1);
#endif

	OSInfo = new COSInfo(param.osversion);

	Timer = new CTimer;
	FileSystem = new CFileSystem(param.baseDir, param.logDir, param.ignoreSetting);

	FileSystem->writeLog(ELOG_GX, "OS: %s  VERSION: %d.%d", OSInfo->getOSName(), OSInfo->MajorVersion, OSInfo->MinorVersion);

	FileSystem->writeLog(ELOG_RES, "start loading casc files...\n");

	WowEnvironment = new wowEnvironment(FileSystem, param.useCompress, param.outputFileName);

	FileSystem->writeLog(ELOG_RES, "start loading db files...\n");

	WowDatabase = new wowDatabase(WowEnvironment);

	EngineSetting = new engineSetting();

	Driver = NULL_PTR;
	HardwareBufferServices = NULL_PTR;
	DrawServices = NULL_PTR;
	ResourceLoader = NULL_PTR;
	GeometryCreator = NULL_PTR;
	ManualMeshServices = NULL_PTR;
	InputReader = NULL_PTR;
	GestureReader = NULL_PTR;
	AudioPlayer = NULL_PTR;
	FontServices = NULL_PTR;
	TextureWriteServices = NULL_PTR;
	ManualTextureServices = NULL_PTR;
	SpecialTextureServices = NULL_PTR;
	ParticleSystemServices = NULL_PTR;
	RibbonEmitterServices = NULL_PTR;
	MeshDecalServices = NULL_PTR;
	TerrainServices = NULL_PTR;

	SceneRenderServices = NULL_PTR;
	SceneEnvironment = NULL_PTR;
	SceneManager = NULL_PTR;
		
	m_IsDXFamily = false;
}

Engine::~Engine()
{
	if(ResourceLoader)
		ResourceLoader->endLoading();
	
	delete SceneManager;

	delete SceneEnvironment;
	delete SceneRenderServices;

	delete TerrainServices;
	delete MeshDecalServices;
	delete RibbonEmitterServices;
	delete ParticleSystemServices;
	delete SpecialTextureServices;
	delete ManualMeshServices;
	delete GeometryCreator;
	delete FontServices;
	delete AudioPlayer;
	delete GestureReader;
	delete InputReader;

	delete ManualTextureServices;
	delete TextureWriteServices;
	delete ResourceLoader;
	delete DrawServices;
	delete HardwareBufferServices;
	delete Driver;

	delete EngineSetting;
	delete WowDatabase;
	delete WowEnvironment;
	delete FileSystem;
	delete Timer;
	delete OSInfo;

	QMem_End();

	/*
	int low, high;
	QMem_TempInfo(low, high);
	char msg[128];
	sprintf_s(msg, 128, "最大临时内存使用: %d", high);
	MessageBoxA(NULL_PTR, msg, "", 0);
	*/
}

SWindowInfo Engine::createWindow( const char* caption, const dimension2du& windowSize, f32 scale, bool fullscreen, bool hide )
{
	return CSysUtility::createWindow(caption, windowSize, scale, fullscreen, hide);
}

bool Engine::initDriver( E_DRIVER_TYPE driverType, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread )
{
	if (WindowInfo.width == 0 || WindowInfo.height == 0)
	{
		ASSERT(false);
		return false;
	}

	switch (driverType)
	{
	case EDT_DIRECT3D9:

#ifdef MW_COMPILE_WITH_DIRECT3D9
		if (!CD3D9Helper::init())
		{
			goto fail;
		}

		Driver = new CD3D9Driver;
		if(!static_cast<CD3D9Driver*>(Driver)->initDriver(WindowInfo, adapter, fullscreen, vsync, antialias, multithread))
		{
			goto fail;
		}

		HardwareBufferServices = new CD3D9HardwareBufferServices;
 		DrawServices = new CD3D9DrawServices;
 		ResourceLoader = new CD3D9ResourceLoader;
		TextureWriteServices = new CD3D9TextureWriteServices;
 		ManualTextureServices = new CD3D9ManualTextureServices;

#endif
		m_IsDXFamily = true;

		break;
	case EDT_DIRECT3D11:

#ifdef MW_COMPILE_WITH_DIRECT3D11
		if (!CD3D11Helper::init())
		{
			goto fail;
		}

 		Driver = new CD3D11Driver;
  		if(!static_cast<CD3D11Driver*>(Driver)->initDriver(WindowInfo, adapter, fullscreen, vsync, antialias, multithread))
		{
			goto fail;
		}
		HardwareBufferServices = new CD3D11HardwareBufferServices;
 		DrawServices = new CD3D11DrawServices;
		ResourceLoader = new CD3D11ResourceLoader;
		TextureWriteServices = new CD3D11TextureWriteServices;
		ManualTextureServices = new CD3D11ManualTextureServices;

#endif
		m_IsDXFamily = true;

		break;
	case EDT_OPENGL:

#ifdef MW_COMPILE_WITH_OPENGL
		if (!COpenGLHelper::init())
		{
			goto fail;
		}
 		Driver = new COpenGLDriver;
 		if (!static_cast<COpenGLDriver*>(Driver)->initDriver(WindowInfo, adapter, fullscreen, vsync, antialias, multithread))
		{
 			goto fail;
 		}
		HardwareBufferServices = new COpenGLHardwareBufferServices;
		DrawServices = new COpenGLDrawServices;
		ResourceLoader = new COpenGLResourceLoader;
		TextureWriteServices = new COpenGLTextureWriteServices;
		ManualTextureServices = new COpenGLManualTextureServices;

#endif
		break;

	default:
		ASSERT(false);
		goto fail;
		break;
	}

	//loading all shaders
	ASSERT(Driver->getShaderServices());
	Driver->getShaderServices()->loadAll();

	InputReader = new CInputReader;
	GestureReader = new CGestureReader;

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_AUDIO
//		InputReader = new CDxInputReader;
// 	if(!static_cast<CDxInputReader*>(InputReader)->initDevice(WindowInfo.hwnd))
// 	{
// 		FileSystem->writeLog(ELOG_GX, "InputReader initDevice Failed!");
// 		ASSERT(false);
// 		goto fail;
// 	}

	AudioPlayer = new CDSAudioPlayer;
	if(!static_cast<CDSAudioPlayer*>(AudioPlayer)->initDevice(WindowInfo.hwnd, 2, 22050, 16))
	{
		ASSERT(false);
	}

#endif

#endif

// 	AudioPlayer = new CX2AudioPlayer;
// 	if(!static_cast<CX2AudioPlayer*>(AudioPlayer)->initDevice(2, 22050))
// 	{
// 		ASSERT(false);
// 	}
	
	FontServices = new CFontServices(0, 0, 0, 12);
	FontServices->createDefaultFonts();

	GeometryCreator = new CGeometryCreator;
 	ManualMeshServices = new CManualMeshServices;
	SpecialTextureServices = new CSpecialTextureServices;
 	ParticleSystemServices = new CParticleSystemServices(5000, 512, 0.5f);
	RibbonEmitterServices = new CRibbonEmitterServices(5000, 512);
	MeshDecalServices = new CMeshDecalServices(512);
	TerrainServices = new CTerrainServices;
 	SceneRenderServices = new CSceneRenderServices;
	SceneEnvironment = new CSceneEnvironment;

	if (multithread && ResourceLoader)
	{
		ResourceLoader->beginLoading();
	}

    FileSystem->writeLog(ELOG_GX, "Engine init successfully :)");
    
	return true;

fail:
	ASSERT(false);

	SAFE_DELETE(SceneEnvironment);
	SAFE_DELETE(SceneRenderServices);
	SAFE_DELETE(TerrainServices);
	SAFE_DELETE(RibbonEmitterServices);
	SAFE_DELETE(ParticleSystemServices);
	SAFE_DELETE(SpecialTextureServices);
	SAFE_DELETE(ManualMeshServices);
	SAFE_DELETE(GeometryCreator);
	SAFE_DELETE(FontServices);
	SAFE_DELETE(AudioPlayer);
	SAFE_DELETE(GestureReader);
	SAFE_DELETE(InputReader);
	SAFE_DELETE(ManualTextureServices);
	SAFE_DELETE(TextureWriteServices);
	SAFE_DELETE(ResourceLoader);
	SAFE_DELETE(DrawServices);
	SAFE_DELETE(HardwareBufferServices);
	SAFE_DELETE(Driver);
	
	return false;
}

void Engine::initSceneManager()
{
	delete SceneManager;
	SceneManager = new CSceneManager();
}

IFTFont* Engine::getDefaultFont()
{
	return FontServices->getDefaultFont();
}


IFTFont* Engine::getFont( u32 fontSize, int fontStyle, int outlineWidth )
{
	return FontServices->getFont(fontSize, fontStyle, outlineWidth);
}