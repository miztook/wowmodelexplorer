#pragma once

#include "core.h"
#include "SLight.h"
#include "SColor.h"

struct SFogParam
{
	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
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

	static const u32 MAX_LIGTHTS = 8;
	static const u32 MAX_CLIPPLANES = 2;

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
	virtual void setLight_Fog(u32 lightIndex, bool enable, const SLight& dirLight, 
		SColor color, const SFogParam& fogParam) = 0;

	virtual void setLight(u32 lightIndex, bool enable, const SLight& dirLight, SColor color) = 0;

#ifdef FULL_INTERFACE	
	virtual bool setClipPlane( u32 index, const plane3df& plane ) = 0;
	virtual void enableClipPlane( u32 index, bool enable ) = 0;
	virtual void deleteAllDynamicLights() = 0;
	virtual void setAmbientLight( SColor color ) = 0;
	virtual bool setDynamicLight( u32 index, const SLight& light )  = 0;
	virtual void turnLightOn(u32 lightIndex, bool turnOn) = 0;
	virtual void setFog(const SFogParam& fogParam) = 0;
	virtual bool isClipPlaneEnable( u32 index ) const = 0;
	virtual SColor getAmbientLight() const = 0;
	virtual const SLight* getDynamicLight(u32 index) const  = 0;
	virtual SFogParam getFog() const = 0;
	virtual bool getClipPlane( u32 index, plane3df& plane ) const = 0;
	virtual bool reset() = 0;
#endif

};