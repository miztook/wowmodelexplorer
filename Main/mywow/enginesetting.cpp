#include "stdafx.h"
#include "enginesetting.h"
#include "mywow.h"

#define CONFIG_DRIVER_TYPE "driverType"

#define CONFIG_ANTIALIAS	"antialias"
#define CONFIG_VSYNC "vsync"
#define CONFIG_FULLSCREEN "fullscreen"
#define CONFIG_TEXTURE_RESOLUTION "textureResolution"
#define CONFIG_TEXTURE_FILTERING "textureFiltering"
#define CONFIG_TEXTURE_PROJECTION "textureProjection"
#define CONFIG_VIEW_DISTANCE "viewDistance"
#define CONFIG_ENV_DETAIL "envDetail"
#define CONFIG_GROUND_CLUTTER "groundClutter"
#define CONFIG_SHADOW_QUALITY "shadowQuality"
#define CONFIG_LIQUID_DETAIL "liquidDetail"
#define CONFIG_SUNSHAFTS "sunShafts"
#define CONFIG_PARTICLEDENSITY "particleDensity"
#define CONFIG_SSAO "SSAO"

#define CONFIG_MAX_FOREGROUND_FPS "maxForegroundFPS"
#define CONFIG_MAX_BACKGROUND_FPS "maxBackgroundFPS"
#define CONFIG_TRIPLE_BUFFERING "tripleBuffering"
#define CONFIG_REDUCE_INPUT_LAG "reduceInputLag"
#define CONFIG_HARDWARE_CURSOR "hardwareCursor"

engineSetting::engineSetting()
{

}

void engineSetting::applyVideoSetting( const SVideoSetting& videosetting )
{
	SDriverSetting newSetting;
	newSetting.fullscreen = videosetting.fullscreen;
	newSetting.antialias = (u8)videosetting.antialias;
	newSetting.vsync = videosetting.vsync;
	g_Engine->getDriver()->setDriverSetting(newSetting);

	setTextureResolution(videosetting.textureResolution);
	setTextureFiltering(videosetting.textureFiltering);
	setTextureProjection(videosetting.textureProjection);

	setViewDistance(videosetting.viewDistance);
	setEnvironmentDetail(videosetting.envDetail);
	setGroundClutter(videosetting.groundClutter);

	setShadowQuality(videosetting.shadowQuality);
	setLiquidDetail(videosetting.liquidDetail);
	setSunShafts(videosetting.sunShafts);
	setParticleDensity(videosetting.particleDensity);
	setSSAO(videosetting.SSAO);

	//retrieve
	retrieveVideoSetting(&VideoSetting);
}

void engineSetting::retrieveVideoSetting( SVideoSetting* videoSetting ) const
{
	const SDriverSetting& setting = g_Engine->getDriver()->getDriverSetting();

	videoSetting->antialias = setting.antialias;
	videoSetting->vsync = setting.vsync;
	videoSetting->fullscreen = setting.fullscreen;

	videoSetting->textureResolution = getTextureResolution();
	videoSetting->textureFiltering = getTextureFiltering();
	videoSetting->textureProjection = getTextureProjection();

	videoSetting->viewDistance = getViewDistance();
	videoSetting->envDetail = getEnvironmentDetail();
	videoSetting->groundClutter = getGroundClutter();

	videoSetting->shadowQuality = getShadowQuality();
	videoSetting->liquidDetail = getLiquidDetail();
	videoSetting->sunShafts = getSunShafts();
	videoSetting->particleDensity = getParticleDensity();
	videoSetting->SSAO = getSSAO();
}

void engineSetting::setTextureResolution( E_LEVEL level )
{
	SOverrideMaterial& material = g_Engine->getDriver()->getOverrideMaterial();
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	switch(level)
	{
	case EL_HIGH:
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				material.MipMapLodBias[i] = 2;
			}
			sceneRenderServices->setTerrainLodBias(2);
			sceneRenderServices->setModelLodBias(2);
		}
		break;
	case EL_GOOD:
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				material.MipMapLodBias[i] = 2;
			}
			sceneRenderServices->setTerrainLodBias(2);
			sceneRenderServices->setModelLodBias(0);
		}
		break;
	case EL_FAIR:
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				material.MipMapLodBias[i] = 2;
			}
			sceneRenderServices->setTerrainLodBias(0);
			sceneRenderServices->setModelLodBias(0);
		}
		break;
	case EL_LOW:
	default:
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				material.MipMapLodBias[i] = 0;
			}
			sceneRenderServices->setTerrainLodBias(0);
			sceneRenderServices->setModelLodBias(0);
		}
		break;
	}

	VideoSetting.textureResolution = level;
}

E_LEVEL engineSetting::getTextureResolution() const
{
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	s32 mipmaplodbias = g_Engine->getDriver()->getOverrideMaterial().MipMapLodBias[0];
	s32 terrainLodBias = sceneRenderServices->getTerrainLodBias();
	s32 modelLodBias = sceneRenderServices->getModelLodBias();
	if (mipmaplodbias == 0)
	{
		return EL_LOW;
	}
	else
	{
		if (modelLodBias > 0)
			return EL_HIGH;
		else if (terrainLodBias > 0)
			return EL_GOOD;
		else
			return EL_FAIR;
	}
}

void engineSetting::setTextureFiltering( E_TEXTURE_FILTER filter )
{
	SOverrideMaterial& material = g_Engine->getDriver()->getOverrideMaterial();
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		material.TextureFilters[i] = filter;
	}

	VideoSetting.textureFiltering = filter;
}

E_TEXTURE_FILTER engineSetting::getTextureFiltering() const
{
	SOverrideMaterial& material = g_Engine->getDriver()->getOverrideMaterial();
	return material.TextureFilters[0];
}

void engineSetting::setTextureProjection( bool projection )
{
	VideoSetting.textureProjection = projection;
}

bool engineSetting::getTextureProjection() const
{
	return VideoSetting.textureProjection;
}

void engineSetting::setViewDistance( E_LEVEL level )
{
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	IResourceLoader* resourceLoader = g_Engine->getResourceLoader();

	switch(level)
	{
	case EL_ULTRA:
		sceneRenderServices->setClipDistance(2200.0f);
		sceneRenderServices->setAdtLoadSize(EAL_5X5);
		resourceLoader->setCacheLimit(IResourceLoader::ECT_ADT, 9);
		break;
	case EL_HIGH:
		sceneRenderServices->setClipDistance(1900.0f);
		sceneRenderServices->setAdtLoadSize(EAL_5X5);
		resourceLoader->setCacheLimit(IResourceLoader::ECT_ADT, 9);
		break;
	case EL_GOOD:
		sceneRenderServices->setClipDistance(1600.0f);
		sceneRenderServices->setAdtLoadSize(EAL_5X5);
		resourceLoader->setCacheLimit(IResourceLoader::ECT_ADT, 9);
		break;
	case EL_FAIR:
		sceneRenderServices->setClipDistance(1300.0f);
		sceneRenderServices->setAdtLoadSize(EAL_3X3);
		resourceLoader->setCacheLimit(IResourceLoader::ECT_ADT, 5);
		break;
	case EL_LOW:
	default:
		sceneRenderServices->setClipDistance(1000.0f);
		sceneRenderServices->setAdtLoadSize(EAL_3X3);
		resourceLoader->setCacheLimit(IResourceLoader::ECT_ADT, 5);
		break;
	}

	VideoSetting.viewDistance = level;
}

E_LEVEL engineSetting::getViewDistance() const
{
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	f32 clipdistance = sceneRenderServices->getClipDistance();
	if (clipdistance >=2200.0f)
		return EL_ULTRA;
	else if (clipdistance >= 1900.0f)
		return EL_HIGH;
	else if (clipdistance >= 1600.0f)
		return EL_GOOD;
	else if (clipdistance >= 1300.0f)
		return EL_FAIR;
	else 
		return EL_LOW;
}

void engineSetting::setEnvironmentDetail( E_LEVEL level )
{
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	switch(level)
	{
	case EL_ULTRA:
		sceneRenderServices->setObjectVisibleDistance(40);
		break;
	case EL_HIGH:
		sceneRenderServices->setObjectVisibleDistance(35);
		break;
	case EL_GOOD:
		sceneRenderServices->setObjectVisibleDistance(30);
		break;
	case EL_FAIR:
		sceneRenderServices->setObjectVisibleDistance(25);
		break;
	case EL_LOW:
		sceneRenderServices->setObjectVisibleDistance(20);
	default:
		break;
	}

	VideoSetting.envDetail = level;
}

E_LEVEL engineSetting::getEnvironmentDetail() const
{
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	f32 objVisibleDistance = sceneRenderServices->getObjectVisibleDistance();
	if (objVisibleDistance >= 40)
		return EL_ULTRA;
	else if (objVisibleDistance >= 35)
		return EL_HIGH;
	else if (objVisibleDistance >= 30)
		return EL_GOOD;
	else if (objVisibleDistance >= 25)
		return EL_FAIR;
	else 
		return EL_LOW;
}

void engineSetting::setGroundClutter( E_LEVEL level )
{
	VideoSetting.groundClutter = level;
}

E_LEVEL engineSetting::getGroundClutter() const
{
	return VideoSetting.groundClutter;
}

void engineSetting::setShadowQuality( E_LEVEL level )
{
	VideoSetting.shadowQuality = level;
}

E_LEVEL engineSetting::getShadowQuality() const
{
	return VideoSetting.shadowQuality;
}

void engineSetting::setLiquidDetail( E_LEVEL level )
{
	VideoSetting.liquidDetail = level;
}

E_LEVEL engineSetting::getLiquidDetail() const
{
	return VideoSetting.liquidDetail;
}

void engineSetting::setSunShafts( E_LEVEL level )
{
	VideoSetting.sunShafts = level;
}

E_LEVEL engineSetting::getSunShafts() const
{
	return VideoSetting.sunShafts;
}

void engineSetting::setParticleDensity( E_LEVEL level )
{
	IParticleSystemServices* particleServices = g_Engine->getParticleSystemServices();
	switch(level)
	{
	case EL_ULTRA:
		particleServices->setParticleDensity(1.0f);
		break;
	case EL_HIGH:
		particleServices->setParticleDensity(0.8f);
		break;
	case  EL_GOOD:
		particleServices->setParticleDensity(0.6f);
		break;
	case EL_FAIR:
		particleServices->setParticleDensity(0.4f);
		break;
	case EL_LOW:
		particleServices->setParticleDensity(0.2f);
	default:
		break;
	}

	VideoSetting.particleDensity = level;
}

E_LEVEL engineSetting::getParticleDensity() const
{
	f32 particleDensity = g_Engine->getParticleSystemServices()->getParticleDensity();
	if (particleDensity >= 1.0f)
		return EL_ULTRA;
	else if (particleDensity >= 0.8f)
		return EL_HIGH;
	else if (particleDensity >= 0.6f)
		return EL_GOOD;
	else if (particleDensity >= 0.4f)
		return EL_FAIR;
	else 
		return EL_LOW;
}

void engineSetting::setSSAO( E_LEVEL level )
{
	VideoSetting.SSAO = level;
}

E_LEVEL engineSetting::getSSAO() const
{
	return VideoSetting.SSAO;
}

void engineSetting::applyAdvancedSetting( const SAdvancedSetting& advancedSetting )
{
	AdvancedSetting.driverType = advancedSetting.driverType;

	setTripleBuffering(advancedSetting.tripleBuffering);
	setReduceInputLag(advancedSetting.reduceInputLag);
	setHardwareCursor(advancedSetting.hardwareCursor);
	setForegroundFPSLimit(advancedSetting.maxForegroundFPS);
	setBackgroundFPSLimit(advancedSetting.maxBackgroundFPS);
}

void engineSetting::setTripleBuffering( bool tripleBuffers )
{
	AdvancedSetting.tripleBuffering = tripleBuffers;
}

void engineSetting::setReduceInputLag( bool reduce )
{
	AdvancedSetting.reduceInputLag = reduce;
}

void engineSetting::setHardwareCursor( bool use )
{
	AdvancedSetting.hardwareCursor = use;
}

void engineSetting::load()
{
	IConfigs* config = g_Engine->getFileSystem()->getConfigs();
	if (!config->readEngineSetting(g_Engine->getFileSystem()))
		return;

	s32 v = 0;

	//
	getValue(config, CONFIG_ANTIALIAS, VideoSetting.antialias);
	getValue(config, CONFIG_VSYNC, VideoSetting.vsync);
	getValue(config, CONFIG_FULLSCREEN, VideoSetting.fullscreen);

	if (getValue(config, CONFIG_TEXTURE_RESOLUTION, v))
		VideoSetting.textureResolution = (E_LEVEL)v;
	if (getValue(config, CONFIG_TEXTURE_FILTERING, v))
		VideoSetting.textureFiltering = (E_TEXTURE_FILTER)v;
	getValue(config, CONFIG_TEXTURE_PROJECTION, VideoSetting.textureProjection);

	if (getValue(config, CONFIG_VIEW_DISTANCE, v))
		VideoSetting.viewDistance = (E_LEVEL)v;
	if (getValue(config, CONFIG_ENV_DETAIL, v))
		VideoSetting.envDetail = (E_LEVEL)v;
	if (getValue(config, CONFIG_GROUND_CLUTTER, v))
		VideoSetting.groundClutter = (E_LEVEL)v;

	if (getValue(config, CONFIG_SHADOW_QUALITY, v))
		VideoSetting.shadowQuality = (E_LEVEL)v;
	if (getValue(config, CONFIG_LIQUID_DETAIL, v))
		VideoSetting.liquidDetail = (E_LEVEL)v;
	if (getValue(config, CONFIG_SUNSHAFTS, v))
		VideoSetting.sunShafts = (E_LEVEL)v;
	if (getValue(config, CONFIG_PARTICLEDENSITY, v))
		VideoSetting.particleDensity = (E_LEVEL)v;
	if (getValue(config, CONFIG_SSAO, v))
		VideoSetting.SSAO = (E_LEVEL)v;

	if (getValue(config, CONFIG_DRIVER_TYPE, v))
		AdvancedSetting.driverType = (E_DRIVER_TYPE)v;

	getValue(config, CONFIG_TRIPLE_BUFFERING, AdvancedSetting.tripleBuffering);
	getValue(config, CONFIG_REDUCE_INPUT_LAG, AdvancedSetting.reduceInputLag);
	getValue(config, CONFIG_HARDWARE_CURSOR, AdvancedSetting.hardwareCursor);
	getValue(config, CONFIG_MAX_BACKGROUND_FPS, AdvancedSetting.maxBackgroundFPS);
	getValue(config, CONFIG_MAX_FOREGROUND_FPS, AdvancedSetting.maxForegroundFPS);
}

void engineSetting::save()
{
	IConfigs* config = g_Engine->getFileSystem()->getConfigs();

	setValue(config, CONFIG_ANTIALIAS, VideoSetting.antialias);
	setValue(config, CONFIG_VSYNC, VideoSetting.vsync);
	setValue(config, CONFIG_FULLSCREEN, VideoSetting.fullscreen);

	setValue(config, CONFIG_TEXTURE_RESOLUTION, (s32)VideoSetting.textureResolution);
	setValue(config, CONFIG_TEXTURE_FILTERING, (s32)VideoSetting.textureFiltering);
	setValue(config, CONFIG_TEXTURE_PROJECTION, VideoSetting.textureProjection);
	setValue(config, CONFIG_VIEW_DISTANCE, (s32)VideoSetting.viewDistance);
	setValue(config, CONFIG_ENV_DETAIL, (s32)VideoSetting.envDetail);
	setValue(config, CONFIG_GROUND_CLUTTER, (s32)VideoSetting.groundClutter);
	setValue(config, CONFIG_SHADOW_QUALITY, (s32)VideoSetting.shadowQuality);
	setValue(config, CONFIG_LIQUID_DETAIL, (s32)VideoSetting.liquidDetail);
	setValue(config, CONFIG_SUNSHAFTS, (s32)VideoSetting.sunShafts);
	setValue(config, CONFIG_PARTICLEDENSITY, (s32)VideoSetting.particleDensity);
	setValue(config, CONFIG_SSAO, (s32)VideoSetting.SSAO);

	setValue(config, CONFIG_DRIVER_TYPE, (s32)AdvancedSetting.driverType);
	setValue(config, CONFIG_TRIPLE_BUFFERING, AdvancedSetting.tripleBuffering);
	setValue(config, CONFIG_REDUCE_INPUT_LAG, AdvancedSetting.reduceInputLag);
	setValue(config, CONFIG_HARDWARE_CURSOR, AdvancedSetting.hardwareCursor);
	setValue(config, CONFIG_MAX_BACKGROUND_FPS, AdvancedSetting.maxBackgroundFPS);
	setValue(config, CONFIG_MAX_FOREGROUND_FPS, AdvancedSetting.maxForegroundFPS);

	config->writeEngineSetting(g_Engine->getFileSystem());
}

bool engineSetting::getValue( IConfigs* config, const c8* key, bool& v )
{
	const c8* s = config->getSetting(ECT_ENGINESETTING, key);
	s32 len = 0;
	if (strlen(s) > 0)
	{
		s32 r = 0;
		len = Q_sscanf(s, "%d", &r);
		v = r != 0;
	}
	return len == 1;
}

bool engineSetting::getValue( IConfigs* config, const c8* key, s32& v )
{
	const c8* s = config->getSetting(ECT_ENGINESETTING, key);
	s32 len = 0;
	if (strlen(s) > 0)
	{
		len = Q_sscanf(s, "%d", &v);
	}
	return len == 1;
}

bool engineSetting::getValue( IConfigs* config, const c8* key, u32& v )
{
	const c8* s = config->getSetting(ECT_ENGINESETTING, key);
	s32 len = 0;
	if (strlen(s) > 0)
	{
		len = Q_sscanf(s, "%u", &v);	
	}
	return len == 1;
}

bool engineSetting::setValue( IConfigs* config, const c8* key, bool v )
{
	c8 val[64] = {0};
	s32 len = Q_sprintf(val, 64, "%d", v ? 1 : 0);
	
	if (len <= 0)
		return false;

	config->setSetting(ECT_ENGINESETTING, key, val);

	return true;
}

bool engineSetting::setValue( IConfigs* config, const c8* key, s32 v )
{
	c8 val[64] = {0};
	s32 len = Q_sprintf(val, 64, "%d", v);

	if (len <= 0)
		return false;

	config->setSetting(ECT_ENGINESETTING, key, val);

	return true;
}

bool engineSetting::setValue( IConfigs* config, const c8* key, u32 v )
{
	c8 val[64] = {0};
	s32 len = Q_sprintf(val, 64, "%u", v);

	if (len <= 0)
		return false;

	config->setSetting(ECT_ENGINESETTING, key, val);

	return true;
}

