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

	bool initDriver(HWND hwnd, E_DRIVER_TYPE driverType, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);

	void initSceneManager();

public:	
	wowDatabase* getWowDatabase() const { return WowDatabase; }
	wowEnvironment* getWowEnvironment() const { return WowEnvironment; }
	IFileSystem*	getFileSystem() const { return FileSystem; }
	IVideoDriver*	getDriver() const { return Driver; }
	IHardwareBufferServices*		getHardwareBufferServices() const { return HardwareBufferServices; }
	IDrawServices*		getDrawServices() const { return DrawServices; }
	ISceneRenderServices*		getSceneRenderServices() const { return SceneRenderServices; }
	ISceneEnvironment*	getSceneEnvironment() const { return SceneEnvironment; }
	ISceneManager*		getSceneManager() const { return SceneManager; }
	IFTFont*		getFont() const { return Font; }
	IResourceLoader*		getResourceLoader() const { return ResourceLoader; }
	IGeometryCreator*		getGeometryCreator() const { return GeometryCreator; }
	IManualMeshServices*		getManualMeshServices() const { return ManualMeshServices; }
	IManualTextureServices*		getManualTextureServices() const { return ManualTextureServices; }
	IParticleSystemServices*		getParticleSystemServices() const { return ParticleSystemServices; }
	IRibbonEmitterServices*		getRibbonEmitterServices() const { return RibbonEmitterServices; }
	ITerrainServices*	getTerrainServices() const { return TerrainServices; }

	void setSceneManager(ISceneManager* smgr) { SceneManager = smgr; }

private:
	wowDatabase*		WowDatabase;
	wowEnvironment*		WowEnvironment;
	IFileSystem*		FileSystem;
	IVideoDriver*		Driver;
	IHardwareBufferServices*		HardwareBufferServices;
	IDrawServices*		DrawServices;
	IFTFont*		Font;
	IResourceLoader*		ResourceLoader;
	ISceneRenderServices*		SceneRenderServices;
	ISceneEnvironment*		SceneEnvironment;
	ISceneManager*			SceneManager;
	IGeometryCreator*		GeometryCreator;
	IManualMeshServices*		ManualMeshServices;
	IManualTextureServices*		ManualTextureServices;
	IParticleSystemServices*		ParticleSystemServices;
	IRibbonEmitterServices*		RibbonEmitterServices;
	ITerrainServices*		TerrainServices;

	_CrtMemState oldState;
	_CrtMemState newState;
	_CrtMemState diffState;

};

extern Engine* g_Engine;
extern u32 g_MainThreadId;