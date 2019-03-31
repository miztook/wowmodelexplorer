#pragma once

#include "ISceneEnvironment.h"
#include <vector>

class IMemFile;

class CSceneEnvironment : public ISceneEnvironment
{
private:
	DISALLOW_COPY_AND_ASSIGN(CSceneEnvironment);

public:
	CSceneEnvironment();
	~CSceneEnvironment();

	virtual void computeOutdoorLight(int32_t time);

	virtual void setDayTime(int32_t hour, int32_t minute);
	virtual void setDayTime(int32_t time);
	virtual int32_t getDayTime() const { return DayTime; }

private:

	struct SOutDoorLightEntry
	{
		int32_t time;
		SOutDoorLight light;

		bool operator<(const SOutDoorLightEntry& other) const
		{
			return time < other.time;
		}
	};

private:

	void initOutDoorLights();

	void readOutDoorLightEntry(IMemFile* file, SOutDoorLightEntry* outdoorlight);

	int32_t getOurDoorLightInfo(int32_t time, SOutDoorLight& v, int32_t hint);

	void interpolate(SOutDoorLight& info, const SOutDoorLightEntry& entry1, const SOutDoorLightEntry& entry2, float r);

private:
	std::vector<SOutDoorLightEntry>		OutDoorLights;

	int32_t	DayTime;
	int32_t	LastTime;
	int32_t	Hint;
};