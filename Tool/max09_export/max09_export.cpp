#include "stdafx.h"
#include "max09_export.h"
#include "M2BoneExport.h"
#include "M2SkinExport.h"

HINSTANCE hInstance = 0;

#define LIB_DESCRIPTION "max09_test_export"

//entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {
	switch(fdwReason) {
	case DLL_PROCESS_ATTACH:
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
		break;
	}
	return(TRUE);
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i)
	{
	case 0:  
		return &g_M2BoneExportDesc;
	case 1:
		return &g_M2SkinExportDesc;
	default: 
		return 0;
	}
}

__declspec( dllexport ) const TCHAR * LibDescription() 
{ 
	return LIB_DESCRIPTION;
}

__declspec( dllexport ) int LibNumberClasses() 
{ 
	return 2; 
}

__declspec( dllexport ) ULONG LibVersion() 
{ 
	return VERSION_3DSMAX; 
}

__declspec( dllexport ) ULONG CanAutoDefer()
{
	return 1;
}