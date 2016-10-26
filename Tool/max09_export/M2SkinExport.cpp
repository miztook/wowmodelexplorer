#include "stdafx.h"
#include "M2SkinExport.h"
#include "DlgErrorMsgs.h"

CM2SkinExportDesc	g_M2SkinExportDesc;

static BOOL showPrompts;
static BOOL exportSelected;

const TCHAR * CM2SkinExport::Ext( int n )
{
	switch(n)
	{
	case 0:
		return _T("m2skin");
	default:
		return _T("");
	}
}

BOOL CM2SkinExport::SupportsOptions( int ext, DWORD options )
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

int CM2SkinExport::DoExport( const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options )
{
	showPrompts = suppressPrompts ? FALSE : TRUE;
	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;

	int status;

	g_msgList.SetCount(0);

	status = skin_save(name, ei, i);

	// gdf output errors
	if (g_msgList.Count()) {
		if(showPrompts)
			CreateDialog(hInstance, MAKEINTRESOURCE(IDD_ERROR_MSGS), GetActiveWindow(), ErrorMsgs_DlgProc);
		for (int i = 0; i < g_msgList.Count(); i++) {
			delete g_msgList[i];
			g_msgList[i] = NULL;
		}
		g_msgList.Delete(0, g_msgList.Count());
	}

	if(status == 0)
		return 1;		// Dialog cancelled
	if(status < 0)
		return 0;		// Real, honest-to-goodness error
	return(status);
}

int CM2SkinExport::skin_save( const TCHAR *filename, ExpInterface *ei, Interface *gi )
{
	if(showPrompts) {
		int result = DialogBoxParam(hInstance,
			MAKEINTRESOURCE(IDD_BONE_EXPORT_OPTIONS),
			//									GetActiveWindow(),
			gi->GetMAXHWnd(),
			SkinExportOptionsDlgProc,
			(LPARAM) this);
		if (result <= 0) 
			return 0;
	}
	else {	// Set default parameters here
		//MaxUVs = TRUE;
	}

	return 1;	
}


INT_PTR CALLBACK SkinExportOptionsDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	static CM2SkinExport* exp;

	switch(message) {
	case WM_INITDIALOG:
		exp = (CM2SkinExport*) lParam;
		CenterWindow(hDlg,GetParent(hDlg));
		SetFocus(hDlg); // For some reason this was necessary.  DS-3/4/96
		//CheckDlgButton(hDlg, IDC_MAX_UVS, TRUE);
		return FALSE;
	case WM_DESTROY:
		return FALSE;
	case WM_COMMAND:
		switch(LOWORD(wParam)) {
		case IDOK:
			//MaxUVs = IsDlgButtonChecked(hDlg, IDC_MAX_UVS);
			EndDialog(hDlg, 1);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
	}
	return FALSE;
}
