#pragma once

#include "ISceneEnvironment.h"
#include <vector>

class MPQFile;

class CSceneEnvironment : public ISceneEnvironment
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSceneEnvironment);

public:
	CSceneEnvironment();
	~CSceneEnvironment();

	virtual void computeOutdoorLight(s32 time);

	virtual void setDayTime(s32 hour, s32 minute);
	virtual void setDayTime(s32 time);
	virtual s32 getDayTime() const { return DayTime; }

private:

	struct SOutDoorLightEntry
	{
		s32 time;
		SOutDoorLight light;

		bool operator<(const SOutDoorLightEntry& other) const
		{
			return time < other.time;
		}
	};

private:

	void initOutDoorLights();

	void readOutDoorLightEntry(MPQFile* file, SOutDoorLightEntry* outdoorlight);

	s32 getOurDoorLightInfo(s32 time, SOutDoorLight& v, s32 hint);

	void interpolate(SOutDoorLight& info, const SOutDoorLightEntry& entry1, const SOutDoorLightEntry& entry2, f32 r);

private:
	std::vector<SOutDoorLightEntry>		OutDoorLights;

	s32	DayTime;

	s32	LastTime;
	s32	Hint;
};