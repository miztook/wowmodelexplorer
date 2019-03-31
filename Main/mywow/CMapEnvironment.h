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
	uint32_t time;

	void set(uint32_t t, uint32_t col)
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
		::memset(colorNums, 0, sizeof(uint32_t)*18);
	}
	~SSkyLight()
	{
		for (uint32_t i=0; i<18; ++i)
			delete[] colorRows[i];
	}

	vector3df		position;
	float		radiusInner;
	float		radiusOuter;

	SSkyColor*	colorRows[18];
	uint32_t	colorNums[18];

	int32_t getColor( uint32_t row, uint32_t time, vector3df& v, int32_t hint=0 );

	bool operator<(const SSkyLight& other) const
	{
		return radiusOuter < other.radiusOuter;
	}
};

//每个map的light, skybox等环境
class CMapEnvironment
{
public:
	explicit CMapEnvironment(int32_t mapid);
	~CMapEnvironment();

	void computeSkyLights(vector3df pos, int32_t time);

	vector3df ColorSet[COLOR_COUNT];

private:
	int32_t	MapId;

	struct SSkyLightEntry
	{
		SSkyLightEntry(SSkyLight* l) : light(l), weight(0.0f) {}
		SSkyLight* light;
		float weight;

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