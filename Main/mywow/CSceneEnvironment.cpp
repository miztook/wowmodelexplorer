#include "stdafx.h"
#include "CSceneEnvironment.h"
#include "mywow.h"
#include "wowEnvironment.h"

#define GLOBALS_SEQEUNCE		2880

CSceneEnvironment::CSceneEnvironment()
{
	DayTime = 0;
	LastTime = -1;
	Hint = 0;

	//initOutDoorLights();
}

CSceneEnvironment::~CSceneEnvironment()
{

}

void CSceneEnvironment::computeOutdoorLight( int32_t time )
{
	if(time < LastTime)
		Hint = 0;

	Hint = getOurDoorLightInfo(time, OutDoorLightInfo, Hint);

	LastTime = time;
}

void CSceneEnvironment::initOutDoorLights()
{
	IMemFile* file = g_Engine->getWowEnvironment()->openFile("World\\dnc.db");
	if (!file)
	{
		ASSERT(false);
		return;
	}

	uint32_t n, d;
	file->seek(4, true);
	file->read(&n, sizeof(uint32_t));
	file->seek(4, true);
	file->read(&d, sizeof(uint32_t));

	file->seek(8 + n*8);

	uint32_t num = (d - file->getPos()) / (2 * sizeof(SOutDoorLight));
	for (uint32_t i=0; i<num; ++i)
	{
		SOutDoorLightEntry entry;
		readOutDoorLightEntry(file, &entry);
		OutDoorLights.push_back(entry);
	}
	delete file;

	std::sort(OutDoorLights.begin(), OutDoorLights.end());
}

void CSceneEnvironment::readOutDoorLightEntry( IMemFile* file, SOutDoorLightEntry* outdoorlight )
{
	SOutDoorLight& light = outdoorlight->light;
	file->seek(4, true);
	file->read(&light.hour, sizeof(float));
	file->seek(4, true);
	file->read(&light.minute, sizeof(float));

	//day
	file->seek(4, true);
	file->read(&light.dayIntensity, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayColor.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayColor.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayColor.Z, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayDirection.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayDirection.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.dayDirection.Z, sizeof(float));

	//night
	file->seek(4, true);
	file->read(&light.nightIntensity, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightColor.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightColor.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightColor.Z, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightDirection.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightDirection.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.nightDirection.Z, sizeof(float));

	//ambient
	file->seek(4, true);
	file->read(&light.ambientIntensity, sizeof(float));
	file->seek(4, true);
	file->read(&light.ambientColor.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.ambientColor.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.ambientColor.Z, sizeof(float));

	//fog
	file->seek(4, true);
	file->read(&light.fogDepth, sizeof(float));
	file->seek(4, true);
	file->read(&light.fogIntensity, sizeof(float));
	file->seek(4, true);
	file->read(&light.fogColor.X, sizeof(float));
	file->seek(4, true);
	file->read(&light.fogColor.Y, sizeof(float));
	file->seek(4, true);
	file->read(&light.fogColor.Z, sizeof(float));

	outdoorlight->time = (int32_t)light.hour * 60 * 2 + (int32_t)light.minute * 2;
}

int32_t CSceneEnvironment::getOurDoorLightInfo( int32_t time, SOutDoorLight& v, int32_t hint )
{
	int32_t pos = -1;
	uint32_t numKeys = (uint32_t)OutDoorLights.size();
	if (numKeys > 1)
	{
		int32_t last = numKeys - 1;

		if (time <= OutDoorLights[0].time)		//小于最小帧
		{
			uint32_t t1 = OutDoorLights[0].time + GLOBALS_SEQEUNCE;
			uint32_t t2 = OutDoorLights[last].time;
			time += GLOBALS_SEQEUNCE;
			float r = (time-t2)/(float)(t1-t2);

			interpolate(v, OutDoorLights[last], OutDoorLights[0], r);
			return 0;
		}

		if (time >= OutDoorLights[last].time)		//大于最大帧
		{
			uint32_t t1 = OutDoorLights[0].time + GLOBALS_SEQEUNCE;
			uint32_t t2 = OutDoorLights[last].time;
			float r = (time-t2)/(float)(t1-t2);

			interpolate(v, OutDoorLights[last], OutDoorLights[0], r);
			return last;
		}

		uint32_t i = 1;
		if (hint >1 && time > OutDoorLights[hint-1].time)					//加速
			i = hint;			

		for (; i<numKeys; ++i )
		{
			if (time <= OutDoorLights[i].time)
			{
				pos = i;
				break;
			}
		}

		if (pos != -1)
		{
			uint32_t t1 = OutDoorLights[pos].time;
			uint32_t t2 = OutDoorLights[pos-1].time;
			float r = (time-t2)/(float)(t1-t2);

			interpolate(v, OutDoorLights[pos-1], OutDoorLights[pos], r);
		}
	}
	else if (numKeys == 1)
	{
		v = OutDoorLights[0].light;
		pos = 0;
	}

	return pos;
}

void CSceneEnvironment::interpolate( SOutDoorLight& info, const SOutDoorLightEntry& entry1, const SOutDoorLightEntry& entry2, float r )
{
	vector3df c;
	const SOutDoorLight& v1 = entry1.light;
	const SOutDoorLight& v2 = entry2.light;

	info.dayIntensity = ::interpolate<float>(r, v1.dayIntensity, v2.dayIntensity);
	info.dayDirection = ::interpolate<vector3df>(r, v1.dayDirection, v2.dayDirection);
	info.dayColor = ::interpolate<vector3df>(r, v1.dayColor, v2.dayColor);
	
	info.nightIntensity = ::interpolate<float>(r, v1.nightIntensity, v2.nightIntensity);
	info.nightDirection = ::interpolate<vector3df>(r, v1.nightDirection, v2.nightDirection);
	info.nightColor = ::interpolate<vector3df>(r, v1.nightColor, v2.nightColor);

	info.fogIntensity = ::interpolate<float>(r, v1.fogIntensity, v2.fogIntensity);
	info.fogDepth = ::interpolate<float>(r, v1.fogDepth, v2.fogDepth);
	info.fogColor = ::interpolate<vector3df>(r, v1.fogColor, v2.fogColor);

	info.ambientIntensity = ::interpolate<float>(r, v1.ambientIntensity, v2.ambientIntensity);
	info.ambientColor = ::interpolate<vector3df>(r, v1.ambientColor, v2.ambientColor);
}

void CSceneEnvironment::setDayTime( int32_t hour, int32_t minute )
{
	hour = abs(hour) % 24;
	minute = abs(minute) % 60;
	DayTime = (hour * 60 + minute) * 2;

	OutDoorLightInfo.hour = (float)((DayTime/2) / 60);
	OutDoorLightInfo.minute = (float)((DayTime/2) % 60);
}

void CSceneEnvironment::setDayTime( int32_t time )
{
	DayTime = abs(time) % MAX_DAYTIME; 
	OutDoorLightInfo.hour = (float)((DayTime/2) / 60);
	OutDoorLightInfo.minute = (float)((DayTime/2) % 60);
}


