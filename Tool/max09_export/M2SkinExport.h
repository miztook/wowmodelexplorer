#pragma once

#include "max.h"

#define M2_SKIN_EXPORT_ID	Class_ID(0x3d7838e0, 0x623a36e7)

class CM2SkinExport : public SceneExport
{
public:

public:
	int				ExtCount()	{ return 1; }
	const TCHAR *	Ext(int n);				
	const TCHAR *	LongDesc() { return _T("m2皮肤文件"); }				
	const TCHAR *	ShortDesc() { return _T("m2皮肤文件"); }				
	const TCHAR *	AuthorName() { return _T("miaoyu"); }		
	const TCHAR *	CopyrightMessage() { return _T("Copyright (C) 2014, miaoyu"); }	
	const TCHAR *	OtherMessage1()	{ return _T(""); }
	const TCHAR *	OtherMessage2() { return _T(""); }			
	unsigned int	Version() { return 100; }		
	void			ShowAbout(HWND hWnd) {}
	int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options);	// Export file
	BOOL			SupportsOptions(int ext, DWORD options);

private:
	int	skin_save(const TCHAR *filename, ExpInterface *ei, Interface *gi);
};

class CM2SkinExportDesc : public ClassDesc {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new CM2SkinExport; }
	const TCHAR *	ClassName() { return _T("M2 Skin Export"); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return M2_SKIN_EXPORT_ID; }
	const TCHAR* 	Category() { return _T("Game Utilities"); }
};

extern CM2SkinExportDesc	g_M2SkinExportDesc;

INT_PTR CALLBACK SkinExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);