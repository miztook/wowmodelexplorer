#include "stdafx.h"
#include "COSInfo.h"

#ifdef MW_PLATFORM_IOS

#include <sys/utsname.h>

COSInfo::COSInfo(float fVersion)
	: MajorVersion(0), MinorVersion(0), DeviceType(Dev_Unknown)
{

	OSName = "IOS";

#ifdef MW_PLATFORM_64BIT
	OSName.append(" 64bit ");
#else
	OSName.append(" 32bit ");
#endif

    SetVersion(fVersion);
    
	retrieveDeviceInfo();
}

bool COSInfo::IsAeroSupport() const
{
	return false;
}

void COSInfo::retrieveDeviceInfo()
{
	struct utsname systemInfo;
    uname(&systemInfo);

	string256 str = systemInfo.machine;
    str.make_lower();
    
    if (str.findString("iphone") >= 0)
    {
        if(str.findString("iphone1") >= 0 ||
           str.findString("iphone2") >= 0)
            DeviceType = Dev_IPhone3;
        else if(str.findString("iphone3") >= 0)
            DeviceType = Dev_IPhone4;
        else if(str.findString("iphone4") >= 0)
            DeviceType = Dev_IPhone4S;
        else if(str.findString("iphone5") >= 0)
            DeviceType = Dev_IPhone5;
        else if(str.findString("iphone6") >= 0)
            DeviceType = Dev_IPhone5S;
		else if(str.findString("iphone7") >= 0)
		{
			if(str.findString(",1") >= 0)
				DeviceType = Dev_IPhone6Plus;
			else if(str.findString(",2") >= 0)
				DeviceType = Dev_IPhone6;
		}
    }
    else if(str.findString("ipad") >= 0)
    {
        if(str.findString("ipad1") >= 0)
            DeviceType = Dev_IPad;
        else if(str.findString("ipad2") >= 0)
        {
            if(str.findString(",1") >= 0 ||
               str.findString(",2") >= 0 ||
               str.findString(",3") >= 0 ||
               str.findString(",4") >= 0)
               DeviceType = Dev_IPad2;
            else if(str.findString(",5") >= 0 ||
                    str.findString(",6") >= 0 ||
                    str.findString(",7") >= 0)
                DeviceType = Dev_IPadMini;
        }
        else if(str.findString("ipad3") >= 0)
        {
            if(str.findString(",1") >= 0 ||
               str.findString(",2") >= 0 ||
               str.findString(",3") >= 0)
                DeviceType = Dev_IPad3;
            else if(str.findString(",4") >= 0 ||
                    str.findString(",5") >= 0 ||
                    str.findString(",6") >= 0)
                DeviceType = Dev_IPad4;
        }
        else if(str.findString("ipad4") >= 0)
        {
            if(str.findString(",1") >= 0 ||
               str.findString(",2") >= 0 )
                DeviceType = Dev_IPadAir;
            else if(str.findString(",4") >= 0 ||
                    str.findString(",5") >= 0)
                DeviceType = Dev_IPadMini2;
        }
    }
    else if(str.findString("ipod") >= 0)
    {
        if(str.findString("ipod1") >= 0)
            DeviceType = Dev_ITouch1;
        else if(str.findString("ipod2") >= 0)
            DeviceType = Dev_ITouch2;
        else if(str.findString("iphone3") >= 0)
            DeviceType = Dev_ITouch3;
        else if(str.findString("iphone4") >= 0)
            DeviceType = Dev_ITouch4;
        else if(str.findString("iphone5") >= 0)
            DeviceType = Dev_ITouch5;
    }
    else if(str.findString("i386") >= 0 ||
            str.findString("x86_64") >= 0)
    {
        DeviceType = Dev_IOSSimulator;
    }
}

#endif
