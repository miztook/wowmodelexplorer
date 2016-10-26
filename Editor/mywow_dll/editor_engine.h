#pragma once

#include "editor_base.h"

MW_API void Engine_create();
MW_API void  Engine_destroy();

MW_API HWND Engine_getHWnd();
MW_API bool  Engine_initDriver(E_DRIVER_TYPE driverType, u32 adapter, bool fullscreen, bool vsync, u32 antialias, bool multithread);
MW_API void  Engine_initSceneManager();

MW_API void Engine_applyVideoSetting(engineSetting::SVideoSetting videosetting);
MW_API void Engine_getVideoSetting(engineSetting::SVideoSetting* videosetting);
MW_API void Engine_applyAdvancedSetting(engineSetting::SAdvancedSetting advancedSetting);
MW_API void Engine_getAdvancedSetting(engineSetting::SAdvancedSetting* advancedSetting);
MW_API void Engine_loadSetting();
MW_API void Engine_saveSetting();

MW_API bool Engine_isAboveVista();
MW_API IVideoDriver*  Engine_getDriver();
MW_API ISceneManager*  Engine_getSceneManager();
MW_API wowDatabase*  Engine_getWowDatabase();
MW_API wowEnvironment*  Engine_getWowEnvironment();
MW_API IManualMeshServices*  Engine_getManualMeshServices();
MW_API IAudioPlayer* Engine_getAudioPlayer();
MW_API IResourceLoader* Engine_getResourceLoader();
MW_API ISceneEnvironment* Engine_getSceneEnvironment();
MW_API IDrawServices* Engine_getDrawServices();
MW_API IFontServices* Engine_getFontServices();

MW_API void Engine_getBaseDirectory(c8* dirname, u32 size);