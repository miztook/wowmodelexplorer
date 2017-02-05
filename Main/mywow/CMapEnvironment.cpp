#include "stdafx.h"
#include "CMapEnvironment.h"
#include "mywow.h"
#include "wowEnvironment.h"

#define SKYMUL		36.0f
#define GLOBALS_SEQEUNCE		2880

s32 SSkyLight::getColor( u32 row, u32 time, vector3df& v, s32 hint )
{
	if (row >= COLOR_COUNT)
		return -1;

	s32 pos = -1;
	const SSkyColor* entry = colorRows[row];
	u32 numKeys = colorNums[row];
	if (numKeys > 1)
	{
		s32 last = numKeys - 1;

		if (time <= entry[0].time)		//小于最小帧
		{
			u32 t1 = entry[0].time + GLOBALS_SEQEUNCE;
			u32 t2 = entry[last].time;
			time += GLOBALS_SEQEUNCE;
			float r = (time-t2)/(float)(t1-t2);

			v = interpolate<vector3df>(r, entry[last].color, entry[0].color);	
			return 0;
		}

		if (time >= entry[last].time)		//大于最大帧
		{
			u32 t1 = entry[0].time + GLOBALS_SEQEUNCE;
			u32 t2 = entry[last].time;
			float r = (time-t2)/(float)(t1-t2);

			v = interpolate<vector3df>(r, entry[last].color, entry[0].color);	
			return last;
		}

		u32 i = 1;
		if (hint >1 && time > entry[hint-1].time)					//加速
			i = hint;			

		for (; i<numKeys; ++i )
		{
			if (time <= entry[i].time)
			{
				pos = i;
				break;
			}
		}

		if (pos != -1)
		{
			u32 t1 = entry[pos].time;
			u32 t2 = entry[pos-1].time;
			float r = (time-t2)/(float)(t1-t2);

			v = interpolate<vector3df>(r, entry[pos-1].color, entry[pos].color);	
		}
	}
	else if (numKeys == 1)
	{
		v = entry[0].color;
		pos = 0;
	}

	return pos;
}

CMapEnvironment::CMapEnvironment( s32 mapid )
	: MapId(mapid)
{
	GlobalLight = NULL_PTR;

	::memset(ColorSet, 0, sizeof(vector3df) * COLOR_COUNT);

	initSkyLights();
}

CMapEnvironment::~CMapEnvironment()
{
	for (u32 i=0; i<LocalSkyLights.size(); ++i)
		delete LocalSkyLights[i].light;

	delete GlobalLight;
}

void CMapEnvironment::initSkyLights()
{
	/*
	const lightDB* db = g_Engine->getWowDatabase()->getLightDB();

	for (u32 i=0; i<db->getNumRecords(); ++i)
	{
		dbc::record r = db->getRecord(i);
		if (MapId != r.getInt(lightDB::Map))
			continue;
			
		SSkyLight* light = new SSkyLight;
		//position
		light->position.set( r.getFloat(lightDB::PositionX) / SKYMUL,
			r.getFloat(lightDB::PositionY) / SKYMUL,
			r.getFloat(lightDB::PositionZ) / SKYMUL);

		light->radiusInner = r.getFloat(lightDB::RadiusInner) / SKYMUL;
		light->radiusOuter = r.getFloat(lightDB::RadiusOuter) / SKYMUL;

		for (u32 k=0; k<COLOR_COUNT; ++k)
		{
			//dbc::record r = bandDB->getByID(firstId + k);
			s32 entries= 1; //r.getInt(lightIntBandDB::Entries);
			if (entries < 0)
				continue;
				
			light->colorNums[k] = (u32)entries;

			light->colorRows[k] = new SSkyColor[entries];
			
			for (s32 n=0; n<entries; ++n)
			{
				u32 t = 0; //r.getUInt(lightIntBandDB::Times + n);
				u32 c = 0xff000000; //r.getUInt(lightIntBandDB::Values + n);
				light->colorRows[k][n].set(t, c);
			}
		}
		light->colorRows[SKY_COLOR_0][0].set(0, SColor(10,10,10).color);
		light->colorRows[SKY_COLOR_1][0].set(0, SColor(20,20,20).color);
		light->colorRows[SKY_COLOR_2][0].set(0, SColor(30,30,30).color);
		light->colorRows[SKY_COLOR_3][0].set(0, SColor(40,40,40).color);
		light->colorRows[SKY_COLOR_4][0].set(0, SColor(50,50,50).color);
		light->colorRows[FOG_COLOR][0].set(0, SColor(64,64,64).color);

		if (light->position.isZero() && !GlobalLight)
			GlobalLight = light;
		else
			LocalSkyLights.emplace_back(SSkyLightEntry(light));
	}
	*/

	SSkyLight* light = new SSkyLight;
	light->colorRows[SKY_COLOR_0][0].set(0, SColor(10,10,10).color);
	light->colorRows[SKY_COLOR_1][0].set(0, SColor(20,20,20).color);
	light->colorRows[SKY_COLOR_2][0].set(0, SColor(30,30,30).color);
	light->colorRows[SKY_COLOR_3][0].set(0, SColor(40,40,40).color);
	light->colorRows[SKY_COLOR_4][0].set(0, SColor(50,50,50).color);
	light->colorRows[FOG_COLOR][0].set(0, SColor(64,64,64).color);
	GlobalLight = light;

	//std::sort(LocalSkyLights.begin(), LocalSkyLights.end());
}

bool CMapEnvironment::findSkyWeights(vector3df pos)
{
	bool weight = false;
	for (s32 i=(s32)LocalSkyLights.size()-1; i>=0; --i)
	{
		SSkyLight* light = LocalSkyLights[i].light;
		f32 dist = (pos - light->position).getLength();
		if (dist < light->radiusInner)
		{
			LocalSkyLights[i].weight = 1.0f;
			weight = true;
		}
		else if(dist < light->radiusOuter)
		{
			f32 r = (dist - light->radiusInner) / (light->radiusOuter - light->radiusInner);
			LocalSkyLights[i].weight = 1.0f - r;
			weight = true;
		}
		else
		{
			LocalSkyLights[i].weight = 0.0f;
		}
	}

	return weight;
}

void CMapEnvironment::computeSkyLights( vector3df pos, s32 time )
{
	if(LocalSkyLights.empty())
		return;

	for (u32 k=0; k<COLOR_COUNT; ++k)
	{
		vector3df v;
		GlobalLight->getColor(k, time, v);
		ColorSet[k] = v;
	}

	if(!findSkyWeights(pos))
		return;

	for (u32 i=0; i<LocalSkyLights.size(); ++i)
	{
		if (LocalSkyLights[i].weight > 0)
		{
			for (u32 k=0; k<18; ++k)
			{
				vector3df v;
				LocalSkyLights[k].light->getColor(k, time, v);
				if (v.X > 1.0f || v.Y > 1.0f || v.Z > 1.0f)
					continue;
				ColorSet[k] += v * LocalSkyLights[k].weight;
			}
		}
	}
}

