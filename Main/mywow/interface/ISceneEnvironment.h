#pragma once

#include "core.h"
#include "SColor.h"

struct SOutDoorLight
{
	SOutDoorLight() : fogColor(0.3f,0.3f,0.3f)
	{
		hour = 0;
		minute = 0;
		dayIntensity = 0;
		nightIntensity = 0;
		ambientIntensity = 0;
		fogIntensity = 0;
		fogDepth = 0;
	}
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
	ISceneEnvironment() 
		: AmbientColor(255, 255, 255), DirLightColor(SColor::White()), LightDir(-vector3df::UnitY())
	{
		EnableDirLight = false;

		WmoMaterialParams[EML_AMBIENT] = 0.85f;
		WmoMaterialParams[EML_DIFFUSE] = 0.15f;
		WmoMaterialParams[EML_SPECULAR] = 0.1f;

		TerrainMaterialParams[EML_AMBIENT] = 0.9f;
		TerrainMaterialParams[EML_DIFFUSE] = 0.1f;
		TerrainMaterialParams[EML_SPECULAR] = 0.7f;
	}
	virtual ~ISceneEnvironment() {}

public:
	static const int INDEX_DIR_LIGHT = 0;
	static const u32 MAX_DAYTIME = 2880;			//24 * 60 * 2
	
	virtual void computeOutdoorLight(s32 time) = 0;
	virtual void setDayTime(s32 hour, s32 minute) = 0;
	virtual void setDayTime(s32 time) = 0;
	virtual s32 getDayTime() const = 0;

	SColor getFogColor() { return SColorf(OutDoorLightInfo.fogColor).toSColor(); }

public:
	SOutDoorLight		OutDoorLightInfo;

	//light
	vector3df	LightDir;
	SColor	AmbientColor;
	SColor	DirLightColor;	

	f32		WmoMaterialParams[EML_COUNT];
	f32		TerrainMaterialParams[EML_COUNT];

	bool EnableDirLight;
};