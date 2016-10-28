#include "stdafx.h"
#include "COSInfo.h"

#ifdef MW_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

COSInfo::COSInfo(float version)
	: MajorVersion(0), MinorVersion(0), DeviceType(Dev_Unknown)
{

	OSName = "Windows";

	if (sizeof(void*) == 8)
		OSName.append(" 64bit");
	else if (sizeof(void*) == 4)
		OSName.append(" 32bit");
	else
		OSName.append(" unknown bit");

	OSVERSIONINFO osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	MajorVersion = osvi.dwMajorVersion;
	MinorVersion = osvi.dwMinorVersion;

	retrieveDeviceInfo();
}

bool COSInfo::IsAeroSupport() const
{
	return MajorVersion >= 6;
}

void COSInfo::retrieveDeviceInfo()
{
	if (MajorVersion >= 6)
	{
		if (MinorVersion >= 2)
			DeviceType = Dev_Win8;
		else if (MinorVersion >= 1)
			DeviceType = Dev_Win7;
		else
			DeviceType = Dev_WinVista;
	}
	else if (MajorVersion >= 5)
	{
		if (MinorVersion >= 1)
			DeviceType = Dev_WinXP;
		else
			DeviceType = Dev_Win2K;
	}
	else
	{
		DeviceType = Dev_Unknown;
	}
}

#endif