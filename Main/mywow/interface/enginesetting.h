#pragma once

#include "base.h"

class IFileSystem;
class IConfigs;

class engineSetting
{
private:
	DISALLOW_COPY_AND_ASSIGN(engineSetting);

public:
	engineSetting();

	void load();
	void save();

public:
	//
	struct SVideoSetting 
	{
		SVideoSetting()
		{
			antialias = 1;
			vsync = false;
			fullscreen = false;
			textureResolution = EL_LOW;
			textureFiltering = ETF_TRILINEAR;
			textureProjection = false;
			viewDistance = envDetail = groundClutter = shadowQuality = liquidDetail =
				sunShafts = particleDensity = EL_LOW;
			SSAO = EL_DISABLE;
		}
		uint32_t antialias;
		bool vsync;
		bool fullscreen;
		bool textureProjection;							//材质投射
		E_LEVEL	textureResolution;	//材质分辨率
		E_TEXTURE_FILTER	textureFiltering;		//材质过滤
		E_LEVEL	viewDistance;			//视野距离
		E_LEVEL	envDetail;		//环境细节
		E_LEVEL	groundClutter;		//地面景观
		E_LEVEL	shadowQuality;		//阴影质量
		E_LEVEL	liquidDetail;	//液体细节
		E_LEVEL	sunShafts;		//阳光效果
		E_LEVEL		particleDensity;		//粒子密度
		E_LEVEL		SSAO;
	};

	//
	struct SAdvancedSetting
	{
		SAdvancedSetting()
		{
			driverType = EDT_DIRECT3D9;
			tripleBuffering = false;
			reduceInputLag = false;
			hardwareCursor = false;
			maxForegroundFPS = 120;
			maxBackgroundFPS = 50;
		}

		E_DRIVER_TYPE driverType;
		int32_t maxForegroundFPS;
		int32_t maxBackgroundFPS;
		bool tripleBuffering;
		bool reduceInputLag;
		bool hardwareCursor;
	};

public:

	void retrieveVideoSetting(SVideoSetting* videoSetting) const;
	void applyVideoSetting(const SVideoSetting& videosetting);

	void applyAdvancedSetting(const SAdvancedSetting& advancedSetting);

	const SVideoSetting& getVideoSetting() const { return VideoSetting; }
	const SAdvancedSetting& getAdvcanedSetting() const { return AdvancedSetting; }

	void setDriverType(E_DRIVER_TYPE driverType) { AdvancedSetting.driverType = driverType; }
	E_DRIVER_TYPE getDriverType() const { return AdvancedSetting.driverType; }

	//材质分辨率
	void setTextureResolution(E_LEVEL level);
	E_LEVEL getTextureResolution() const;

	//材质过滤
	void setTextureFiltering(E_TEXTURE_FILTER filter);
	E_TEXTURE_FILTER getTextureFiltering() const;

	//材质投射
	void setTextureProjection(bool projection);
	bool getTextureProjection() const;

	//视野距离
	void setViewDistance(E_LEVEL level);
	E_LEVEL getViewDistance() const;

	//环境细节
	void setEnvironmentDetail(E_LEVEL level);
	E_LEVEL getEnvironmentDetail() const;

	//地面景观
	void setGroundClutter(E_LEVEL level);
	E_LEVEL getGroundClutter() const;

	//阴影质量
	void setShadowQuality(E_LEVEL level);
	E_LEVEL getShadowQuality() const;

	//液体细节
	void setLiquidDetail(E_LEVEL level);
	E_LEVEL getLiquidDetail() const;

	//阳光效果
	void setSunShafts(E_LEVEL level);
	E_LEVEL getSunShafts() const;

	//粒子密度
	void setParticleDensity(E_LEVEL level);
	E_LEVEL getParticleDensity() const;

	//ssao
	void setSSAO(E_LEVEL level);
	E_LEVEL getSSAO() const;

	//三倍缓冲
	void setTripleBuffering(bool tripleBuffers);
	bool getTripleBuffering() const { return AdvancedSetting.tripleBuffering; }

	//减少输入延迟
	void setReduceInputLag(bool reduce);
	bool getReduceInputLag() const { return AdvancedSetting.reduceInputLag; }

	//使用硬件指针
	void setHardwareCursor(bool use);
	bool getHardwareCursor() const { return AdvancedSetting.hardwareCursor; }

	//前台，后台fps
	void setForegroundFPSLimit(int32_t limit) { AdvancedSetting.maxForegroundFPS = limit; }
	int32_t getForegroundFPSLimit() const { return AdvancedSetting.maxForegroundFPS; }
	void setBackgroundFPSLimit(int32_t limit) { AdvancedSetting.maxBackgroundFPS = limit; }
	int32_t getBackgroundFPSLimit() const { return AdvancedSetting.maxBackgroundFPS; }

private:
	bool getValue(IConfigs* config, const char* key, bool& v);
	bool getValue(IConfigs* config, const char* key, int32_t& v);
	bool getValue(IConfigs* config, const char* key, uint32_t& v);

	bool setValue(IConfigs* config, const char* key, bool v);
	bool setValue(IConfigs* config, const char* key, int32_t v);
	bool setValue(IConfigs* config, const char* key, uint32_t v);

private:
	SVideoSetting		VideoSetting;
	SAdvancedSetting	AdvancedSetting;
};