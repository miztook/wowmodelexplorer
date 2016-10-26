#pragma once

#include "max.h"

#define M2_BONE_EXPORT_ID	Class_ID(0x20f65db2, 0x19066960)

class CM2BoneExport : public SceneExport
{
public:

public:
	int				ExtCount()	{ return 1; }
	const TCHAR *	Ext(int n);				
	const TCHAR *	LongDesc() { return _T("m2骨骼文件"); }				
	const TCHAR *	ShortDesc() { return _T("m2骨骼文件"); }				
	const TCHAR *	AuthorName() { return _T("miaoyu"); }		
	const TCHAR *	CopyrightMessage() { return _T("Copyright (C) 2014, miaoyu"); }	
	const TCHAR *	OtherMessage1()	{ return _T(""); }
	const TCHAR *	OtherMessage2() { return _T(""); }			
	unsigned int	Version() { return 100; }		
	void			ShowAbout(HWND hWnd) {}
	int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options);	// Export file
	BOOL			SupportsOptions(int ext, DWORD options);

private:
	int	bone_save(const TCHAR *filename, ExpInterface *ei, Interface *gi);

};

class CM2BoneExportDesc : public ClassDesc {
public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL loading = FALSE) { return new CM2BoneExport; }
	const TCHAR *	ClassName() { return _T("M2 Bone Export"); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		ClassID() { return M2_BONE_EXPORT_ID; }
	const TCHAR* 	Category() { return _T("Game Utilities"); }
};

extern CM2BoneExportDesc	g_M2BoneExportDesc;

INT_PTR CALLBACK BoneExportOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
