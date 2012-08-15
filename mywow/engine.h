#pragma once

#include "base.h"

class wowEnvironment;
class wowDatabase;

class Engine
{
public:
	Engine();
	~Engine();

	static HWND createWindow( const char* caption, WNDPROC proc, dimension2du &windowSize, bool fullscreen=false, bool hide=false );

	bool initDriver(HWND hwnd, E_DRIVER_TYPE driverType, bool fullscreen, bool vsync, bool multithread, u32 bits=32, u8 antialias=1, E_CSAA csaa = ECSAA_8x);

	void initSceneManager();

public:	
	u32 getCurrentThreadId() const { return CurrentThreadId; }
	wowDatabase* getWowDatabase() const { return WowDatabase; }
	wowEnvironment* getWowEnvironment() const { return WowEnvironment; }
	IFileSystem*	getFileSystem() const { return FileSystem; }
	IVideoDriver*	getDriver() const { return Driver; }
	IHardwareBufferServices*		getHardwareBufferServices() const { return HardwareBufferServices; }
	IDrawServices*		getDrawServices() const { return DrawServices; }
	ISceneRenderServices*		getSceneRenderServices() const { return SceneRenderServices; }
	ISceneManager*		getSceneManager() const { return SceneManager; }
	IFTFont*		getFont() const { return Font; }
	IResourceLoader*		getResourceLoader() const { return ResourceLoader; }
	IGeometryCreator*		getGeometryCreator() const { return GeometryCreator; }
	IManualMeshServices*		getManualMeshServices() const { return ManualMeshServices; }
	IManualTextureServices*		getManualTextureServices() const { return ManualTextureServices; }
	IParticleSystemServices*		getParticleSystemServices() const { return ParticleSystemServices; }
	IRibbonEmitterServices*		getRibbonEmitterServices() const { return RibbonEmitterServices; }

	void setSceneManager(ISceneManager* smgr) { SceneManager = smgr; }

private:
	u32		CurrentThreadId;
	wowDatabase*		WowDatabase;
	wowEnvironment*		WowEnvironment;
	IFileSystem*		FileSystem;
	IVideoDriver*		Driver;
	IHardwareBufferServices*		HardwareBufferServices;
	IDrawServices*		DrawServices;
	IFTFont*		Font;
	IResourceLoader*		ResourceLoader;
	ISceneRenderServices*		SceneRenderServices;
	ISceneManager*			SceneManager;
	IGeometryCreator*		GeometryCreator;
	IManualMeshServices*		ManualMeshServices;
	IManualTextureServices*		ManualTextureServices;
	IParticleSystemServices*		ParticleSystemServices;
	IRibbonEmitterServices*		RibbonEmitterServices;

	_CrtMemState oldState;
	_CrtMemState newState;
	_CrtMemState diffState;

	friend class CD3D9Driver;
};

extern Engine* g_Engine;