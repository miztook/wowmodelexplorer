#pragma once

#include "core.h"
#include "SLight.h"
#include "SColor.h"

struct SFogParam
{
	SFogParam()
		: FogStart(50.0f), FogEnd(100.0f), FogDensity(0.01f), FogType(EFT_FOG_LINEAR), PixelFog(false), RangeFog(false)
	{ }

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
	SSceneState() { resetLights(); }

	static const u32 MAX_LIGTHTS = 8;
	static const u32 MAX_CLIPPLANES = 2;

	void resetLights()
	{
		memset(LightsOn, 0, sizeof(LightsOn)); 
		memset(enableclips, 0, sizeof(enableclips));
		memset(clipplanes, 0, sizeof(clipplanes));
	}

	SLight		Lights[MAX_LIGTHTS];
	bool		LightsOn[MAX_LIGTHTS];
	u32			LightCount;
	SColor		AmbientLightColor;
	SFogParam	FogParam;
	plane3df		clipplanes[MAX_CLIPPLANES];
	bool		enableclips[MAX_CLIPPLANES];
};

//设置光，雾等和场景相关的渲染状态
class ISceneStateServices
{
public:
	virtual ~ISceneStateServices() {}

public:
	virtual SColor getAmbientLight() const = 0;
	virtual void setAmbientLight( SColor color ) = 0;
	virtual void deleteAllDynamicLights() = 0;
	virtual bool setDynamicLight( u32 index, const SLight& light )  = 0;
	virtual u32 getDynamicLightCount() const = 0;
	virtual const SLight& getDynamicLight(u32 index) const  = 0;
	virtual void turnLightOn(u32 lightIndex, bool turnOn) = 0;

	virtual void setFog(SFogParam fogParam) = 0;
	virtual SFogParam getFog() const = 0;

	virtual bool setClipPlane( u32 index, const plane3df& plane ) = 0;
	virtual void enableClipPlane( u32 index, bool enable ) = 0;

	virtual bool reset() = 0;

};