#pragma once

#include "core.h"
#include "SLight.h"
#include "SColor.h"

struct SFogParam
{
	float FogStart;
	float FogEnd;
	float FogDensity;
	SColor	FogColor;
	E_FOG_TYPE FogType;
	bool PixelFog;
	bool RangeFog;
};

struct SSceneState
{
	SSceneState()
	{
		FogParam.FogStart = 50.0f;
		FogParam.FogEnd =100.0f;
		FogParam.FogDensity = 0.01f;
		FogParam.FogType = EFT_FOG_LINEAR;
		FogParam.PixelFog = false;
		FogParam.RangeFog = false;
		resetLights();
	}

	static const uint32_t MAX_LIGTHTS = 8;
	static const uint32_t MAX_CLIPPLANES = 2;

	void resetLights()
	{
		memset(LightsOn, 0, sizeof(LightsOn)); 
		memset(enableclips, 0, sizeof(enableclips));
		memset(clipplanes, 0, sizeof(clipplanes));
	}

	SLight		Lights[MAX_LIGTHTS];
	SColor		AmbientLightColor;
	SFogParam	FogParam;
	plane3df		clipplanes[MAX_CLIPPLANES];
	bool		LightsOn[MAX_LIGTHTS];
	bool		enableclips[MAX_CLIPPLANES];

};

//设置光，雾等和场景相关的渲染状态
class ISceneStateServices
{
public:
	virtual ~ISceneStateServices() {}

public:	
	virtual void setLight_Fog(uint32_t lightIndex, bool enable, const SLight& dirLight, 
		SColor color, const SFogParam& fogParam) = 0;

	virtual void setLight(uint32_t lightIndex, bool enable, const SLight& dirLight, SColor color) = 0;

};