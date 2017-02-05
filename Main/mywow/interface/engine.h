#pragma once

#include "core.h"
#include "SWindowInfo.h"

class COSInfo;
class wowEnvironment;
class wowDatabase;
class IFileSystem;
class IVideoDriver;
class IHardwareBufferServices;
class IDrawServices;
class ISceneRenderServices;
class ISceneEnvironment;
class ISceneManager;
class IFontServices;
class IFTFont;
class IResourceLoader;
class IGeometryCreator;
class ITextureWriteServices;
class IManualMeshServices;
class IManualTextureServices;
class ISpecialTextureServices;
class IParticleSystemServices;
class IRibbonEmitterServices;
class IMeshDecalServices;
class ITerrainServices;
class IInputReader;
class IGestureReader;
class IAudioPlayer;
class engineSetting;
class CTimer;

class IMessageHandler
{
public:
	virtual void onExit(window_type hwnd) = 0;
	virtual void onSize(window_type hwnd, int width, int height) = 0;
};

struct SEngineInitParam
{
	SEngineInitParam()
	{
		memset(baseDir, 0, sizeof(baseDir));
		memset(logDir, 0, sizeof(logDir));
        osversion = 0.0f;
		ignoreSetting = false;
		useCompress = true;
		outputFileName = false;
	}

	c8 baseDir[QMAX_PATH];
	c8 logDir[QMAX_PATH];
    float osversion;
	bool ignoreSetting;
	bool useCompress;
	bool outputFileName;
};

class Engine
{
public:
	Engine(const SEngineInitParam& param, const SWindowInfo& wndInfo);
	~Engine();

	static SWindowInfo createWindow( const char* caption, const dimension2du& windowSize, f32 scale = 1.0f, bool fullscreen=false, bool hide=false );

	bool initDriver(E_DRIVER_TYPE driverType, u32 adapter, bool fullscreen, bool vsync, u8 antialias, bool multithread);
    
	void initSceneManager();

	void setMessageHandler(IMessageHandler* messageHandler) { MessageHandler = messageHandler; }
	IMessageHandler* getMessageHandler() { return MessageHandler; }

	IFTFont* getDefaultFont();
	IFTFont* getFont(u32 fontSize, int fontStyle, int outlineWidth);

	void setSceneManager(ISceneManager* smgr) { SceneManager = smgr; }

public:	
	SWindowInfo getWindowInfo() const { return WindowInfo; }
	COSInfo* getOSInfo() const { return OSInfo; }
	wowDatabase* getWowDatabase() const { return WowDatabase; }
	wowEnvironment* getWowEnvironment() const { return WowEnvironment; }
	IFileSystem*	getFileSystem() const { return FileSystem; }
	IVideoDriver*	getDriver() const { return Driver; }
	IHardwareBufferServices*		getHardwareBufferServices() const { return HardwareBufferServices; }
	IDrawServices*		getDrawServices() const { return DrawServices; }
	ISceneRenderServices*		getSceneRenderServices() const { return SceneRenderServices; }
	ISceneEnvironment*	getSceneEnvironment() const { return SceneEnvironment; }
	ISceneManager*		getSceneManager() const { return SceneManager; }
	IFontServices*		getFontServices() const { return FontServices; }
	IResourceLoader*		getResourceLoader() const { return ResourceLoader; }
	IGeometryCreator*		getGeometryCreator() const { return GeometryCreator; }
	ITextureWriteServices*		getTextureWriteServices() const { return TextureWriteServices; }
	IManualMeshServices*		getManualMeshServices() const { return ManualMeshServices; }
	IManualTextureServices*		getManualTextureServices() const { return ManualTextureServices; }
	ISpecialTextureServices*		getSpecialTextureServices() const { return SpecialTextureServices; }
	IParticleSystemServices*		getParticleSystemServices() const { return ParticleSystemServices; }
	IRibbonEmitterServices*		getRibbonEmitterServices() const { return RibbonEmitterServices; }
	IMeshDecalServices*			getMeshDecalServices() const { return MeshDecalServices; }
	ITerrainServices*	getTerrainServices() const { return TerrainServices; }
	IInputReader*	getInputReader() const { return InputReader; }
	IGestureReader*		getGestureReader() const { return GestureReader; }
	IAudioPlayer*   getAudioPlayer() const { return AudioPlayer; }
	engineSetting*		getEngineSetting() const { return EngineSetting; } 
	CTimer*		getTimer() const { return Timer; }
	bool isDXFamily() const { return m_IsDXFamily; }

private:
	SWindowInfo WindowInfo;
	COSInfo*	OSInfo;
	wowDatabase*		WowDatabase;
	wowEnvironment*		WowEnvironment;
	IFileSystem*		FileSystem;
	IInputReader*		InputReader;
	IGestureReader*		GestureReader;
	IVideoDriver*		Driver;
	IHardwareBufferServices*		HardwareBufferServices;
	IDrawServices*		DrawServices;
	IFontServices*		FontServices;
	IResourceLoader*		ResourceLoader;
	ISceneRenderServices*		SceneRenderServices;
	ISceneEnvironment*		SceneEnvironment;
	ISceneManager*			SceneManager;
	IGeometryCreator*		GeometryCreator;
	ITextureWriteServices*		TextureWriteServices;
	IManualMeshServices*		ManualMeshServices;
	IManualTextureServices*		ManualTextureServices;
	ISpecialTextureServices*		SpecialTextureServices;
	IParticleSystemServices*		ParticleSystemServices;
	IRibbonEmitterServices*		RibbonEmitterServices;
	IMeshDecalServices*			MeshDecalServices;
	ITerrainServices*		TerrainServices;
	IAudioPlayer*		AudioPlayer;
	engineSetting*		EngineSetting;
	CTimer*			Timer;
	IMessageHandler*		MessageHandler;
	bool			m_IsDXFamily;	
};

extern Engine* g_Engine;