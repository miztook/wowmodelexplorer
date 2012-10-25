#pragma once

#include "core.h"
#include "SColor.h"

struct SOutDoorLight
{
	f32 hour;
	f32 minute;
	f32 dayIntensity;
	f32 nightIntensity;
	f32 ambientIntensity;
	f32 fogIntensity;
	f32 fogDepth;
	vector3df	dayColor;
	vector3df	nightColor;
	vector3df	ambientColor;
	vector3df	fogColor;
	vector3df dayDirection;
	vector3df nightDirection;
};

class ISceneEnvironment
{
public:
	virtual ~ISceneEnvironment() {}

public:
	static const u32 MAX_DAYTIME = 2880;			//24 * 60 * 2
	
	virtual void computeOutdoorLight(s32 time) = 0;
	virtual void setDayTime(s32 hour, s32 minute) = 0;
	virtual void setDayTime(s32 time) = 0;
	virtual s32 getDayTime() const = 0;

	virtual SColor getFogColor() { return SColorf(OutDoorLightInfo.fogColor).toSColor(); }
public:
	SOutDoorLight		OutDoorLightInfo;
};