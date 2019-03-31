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
	float hour;
	float minute;
	float dayIntensity;
	float nightIntensity;
	float ambientIntensity;
	float fogIntensity;
	float fogDepth;
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
	static const uint32_t MAX_DAYTIME = 2880;			//24 * 60 * 2
	
	virtual void computeOutdoorLight(int32_t time) = 0;
	virtual void setDayTime(int32_t hour, int32_t minute) = 0;
	virtual void setDayTime(int32_t time) = 0;
	virtual int32_t getDayTime() const = 0;

	SColor getFogColor() { return SColorf(OutDoorLightInfo.fogColor).toSColor(); }

public:
	SOutDoorLight		OutDoorLightInfo;

	//light
	vector3df	LightDir;
	SColor	AmbientColor;
	SColor	DirLightColor;	

	float		WmoMaterialParams[EML_COUNT];
	float		TerrainMaterialParams[EML_COUNT];

	bool EnableDirLight;
};