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

void CSceneEnvironment::computeOutdoorLight( s32 time )
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

	u32 n, d;
	file->seek(4, true);
	file->read(&n, sizeof(u32));
	file->seek(4, true);
	file->read(&d, sizeof(u32));

	file->seek(8 + n*8);

	u32 num = (d - file->getPos()) / (2 * sizeof(SOutDoorLight));
	for (u32 i=0; i<num; ++i)
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
	file->read(&light.hour, sizeof(f32));
	file->seek(4, true);
	file->read(&light.minute, sizeof(f32));

	//day
	file->seek(4, true);
	file->read(&light.dayIntensity, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayColor.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayColor.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayColor.Z, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayDirection.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayDirection.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.dayDirection.Z, sizeof(f32));

	//night
	file->seek(4, true);
	file->read(&light.nightIntensity, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightColor.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightColor.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightColor.Z, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightDirection.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightDirection.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.nightDirection.Z, sizeof(f32));

	//ambient
	file->seek(4, true);
	file->read(&light.ambientIntensity, sizeof(f32));
	file->seek(4, true);
	file->read(&light.ambientColor.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.ambientColor.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.ambientColor.Z, sizeof(f32));

	//fog
	file->seek(4, true);
	file->read(&light.fogDepth, sizeof(f32));
	file->seek(4, true);
	file->read(&light.fogIntensity, sizeof(f32));
	file->seek(4, true);
	file->read(&light.fogColor.X, sizeof(f32));
	file->seek(4, true);
	file->read(&light.fogColor.Y, sizeof(f32));
	file->seek(4, true);
	file->read(&light.fogColor.Z, sizeof(f32));

	outdoorlight->time = (s32)light.hour * 60 * 2 + (s32)light.minute * 2;
}

s32 CSceneEnvironment::getOurDoorLightInfo( s32 time, SOutDoorLight& v, s32 hint )
{
	s32 pos = -1;
	u32 numKeys = (u32)OutDoorLights.size();
	if (numKeys > 1)
	{
		s32 last = numKeys - 1;

		if (time <= OutDoorLights[0].time)		//小于最小帧
		{
			u32 t1 = OutDoorLights[0].time + GLOBALS_SEQEUNCE;
			u32 t2 = OutDoorLights[last].time;
			time += GLOBALS_SEQEUNCE;
			float r = (time-t2)/(float)(t1-t2);

			interpolate(v, OutDoorLights[last], OutDoorLights[0], r);
			return 0;
		}

		if (time >= OutDoorLights[last].time)		//大于最大帧
		{
			u32 t1 = OutDoorLights[0].time + GLOBALS_SEQEUNCE;
			u32 t2 = OutDoorLights[last].time;
			float r = (time-t2)/(float)(t1-t2);

			interpolate(v, OutDoorLights[last], OutDoorLights[0], r);
			return last;
		}

		u32 i = 1;
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
			u32 t1 = OutDoorLights[pos].time;
			u32 t2 = OutDoorLights[pos-1].time;
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

void CSceneEnvironment::interpolate( SOutDoorLight& info, const SOutDoorLightEntry& entry1, const SOutDoorLightEntry& entry2, f32 r )
{
	vector3df c;
	const SOutDoorLight& v1 = entry1.light;
	const SOutDoorLight& v2 = entry2.light;

	info.dayIntensity = ::interpolate<f32>(r, v1.dayIntensity, v2.dayIntensity);
	info.dayDirection = ::interpolate<vector3df>(r, v1.dayDirection, v2.dayDirection);
	info.dayColor = ::interpolate<vector3df>(r, v1.dayColor, v2.dayColor);
	
	info.nightIntensity = ::interpolate<f32>(r, v1.nightIntensity, v2.nightIntensity);
	info.nightDirection = ::interpolate<vector3df>(r, v1.nightDirection, v2.nightDirection);
	info.nightColor = ::interpolate<vector3df>(r, v1.nightColor, v2.nightColor);

	info.fogIntensity = ::interpolate<f32>(r, v1.fogIntensity, v2.fogIntensity);
	info.fogDepth = ::interpolate<f32>(r, v1.fogDepth, v2.fogDepth);
	info.fogColor = ::interpolate<vector3df>(r, v1.fogColor, v2.fogColor);

	info.ambientIntensity = ::interpolate<f32>(r, v1.ambientIntensity, v2.ambientIntensity);
	info.ambientColor = ::interpolate<vector3df>(r, v1.ambientColor, v2.ambientColor);
}

void CSceneEnvironment::setDayTime( s32 hour, s32 minute )
{
	hour = abs(hour) % 24;
	minute = abs(minute) % 60;
	DayTime = (hour * 60 + minute) * 2;

	OutDoorLightInfo.hour = (f32)((DayTime/2) / 60);
	OutDoorLightInfo.minute = (f32)((DayTime/2) % 60);
}

void CSceneEnvironment::setDayTime( s32 time )
{
	DayTime = abs(time) % MAX_DAYTIME; 
	OutDoorLightInfo.hour = (f32)((DayTime/2) / 60);
	OutDoorLightInfo.minute = (f32)((DayTime/2) % 60);
}


