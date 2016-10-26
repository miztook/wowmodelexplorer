#pragma once

#include "core.h"
#include <vector>

enum SkyColorNames : int32_t
{
	LIGHT_GLOBAL_DIFFUSE = 0,
	LIGHT_GLOBAL_AMBIENT,
	SKY_COLOR_0,
	SKY_COLOR_1,
	SKY_COLOR_2,
	SKY_COLOR_3,
	SKY_COLOR_4,
	FOG_COLOR,
	SKY_UNKNOWN_1,
	SUN_COLOR,
	SUN_HALO_COLOR,
	SKY_UNKNOWN_2,
	CLOUD_COLOR,
	SKY_UNKNOWN_3,
	SKY_UNKNOWN_4,
	WATER_COLOR_DARK,
	WATER_COLOR_LIGHT,
	SHADOW_COLOR,

	COLOR_COUNT,
};


struct SSkyColor
{
	vector3df color;
	u32 time;

	void set(u32 t, u32 col)
	{
		time = t;
		color.Z = ((col & 0x0000ff)) / 255.0f;
		color.Y = ((col & 0x00ff00) >> 8) / 255.0f;
		color.X = ((col & 0xff0000) >> 16) / 255.0f;
	}
};

struct SSkyLight
{
	SSkyLight() 
	{ 
		radiusInner = 0;
		radiusOuter = 0;
		::memset(colorRows, 0, sizeof(SSkyColor*)*18);
		::memset(colorNums, 0, sizeof(u32)*18);
	}
	~SSkyLight()
	{
		for (u32 i=0; i<18; ++i)
			delete[] colorRows[i];
	}

	vector3df		position;
	f32		radiusInner;
	f32		radiusOuter;

	SSkyColor*	colorRows[18];
	u32	colorNums[18];

	s32 getColor( u32 row, u32 time, vector3df& v, s32 hint=0 );

	bool operator<(const SSkyLight& other) const
	{
		return radiusOuter < other.radiusOuter;
	}
};

//每个map的light, skybox等环境
class CMapEnvironment
{
public:
	explicit CMapEnvironment(s32 mapid);
	~CMapEnvironment();

	void computeSkyLights(vector3df pos, s32 time);

	vector3df ColorSet[COLOR_COUNT];

private:
	s32	MapId;

	struct SSkyLightEntry
	{
		SSkyLightEntry(SSkyLight* l) : light(l), weight(0.0f) {}
		SSkyLight* light;
		f32 weight;

		bool operator<(const SSkyLightEntry& other) const
		{
			return *light < *other.light;
		}
	};

private:
	void initSkyLights();

	bool findSkyWeights(vector3df pos);

private:
	std::vector<SSkyLightEntry>		LocalSkyLights;	

	SSkyLight*		GlobalLight;

};