#pragma once

#include "core.h"


class COSInfo
{
private:
	DISALLOW_COPY_AND_ASSIGN(COSInfo);

public:
	enum E_DEVICE_TYPE
	{
		Dev_Unknown = 0,
		
		Dev_Win2K = 1,
		Dev_WinXP,
		Dev_WinVista,
		Dev_Win7,
		Dev_Win8,

		Dev_IPhone3 = 1000,
		Dev_IPhone4,
		Dev_IPhone4S,
		Dev_IPhone5,
		Dev_IPhone5S,
		Dev_IPhone6,
		Dev_IPhone6Plus,

		Dev_IPad = 2000,
		Dev_IPad2,
		Dev_IPad3,
		Dev_IPad4,
		Dev_IPadAir,
		Dev_IPadMini,
		Dev_IPadMini2,

		Dev_ITouch1 = 3000,
		Dev_ITouch2,
		Dev_ITouch3,
		Dev_ITouch4,
		Dev_ITouch5,
        
        Dev_IOSSimulator = 4000,

		Dev_Android = 5000,

		Dev_WinRT = 6000,
	};

public:
	explicit COSInfo(float version);

public:
	const char* getOSName() const { return OSName.c_str(); }

	bool IsAeroSupport() const;

	void SetVersion(float version);

public:
	uint32_t	MajorVersion;
	uint32_t	MinorVersion;

private:
	void retrieveDeviceInfo();

private:
	string256	OSName;
	E_DEVICE_TYPE		DeviceType;
};

inline void COSInfo::SetVersion(float version)
{
	MajorVersion = (uint32_t)floor32_(version);
	MinorVersion = (uint32_t)round32_(fract_(version)*10.0f);
}