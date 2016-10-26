#include "stdafx.h"
#include "DlgErrorMsgs.h"

// gdf
void AddLineToListDlg(HWND hDlg, const TCHAR* buf) {
		SendDlgItemMessage(hDlg, IDC_ERROR_MSG_LIST, LB_ADDSTRING, NULL, (LPARAM) buf);
}

// gdf output errors
void BuildMsgList(HWND hDlg)
{
	AddLineToListDlg(hDlg, _T("\t------------------------------<start>-----------------------------"));
	for (int i = 0; i < g_msgList.Count(); i++) {
		AddLineToListDlg(hDlg, _T(g_msgList[i]->data()));
	}
	AddLineToListDlg(hDlg, _T("\t-------------------------------<end>------------------------------"));
}

//gdf
INT_PTR CALLBACK ErrorMsgs_DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int tabs[3] = {24,24,24};
	switch (iMsg)
	{
	case WM_INITDIALOG:
		CenterWindow(hDlg, GetWindow(hDlg, GW_OWNER));
		SendDlgItemMessage(hDlg, IDC_ERROR_MSG_LIST, LB_RESETCONTENT, 0, 0);
		SendDlgItemMessage(hDlg, IDC_ERROR_MSG_LIST, LB_SETTABSTOPS, (WPARAM) 3,(LPARAM) tabs);
		BuildMsgList(hDlg);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:	
			EndDialog(hDlg, TRUE); 
			break;
		}
		break;
	}
	return FALSE;
}
