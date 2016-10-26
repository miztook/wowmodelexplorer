#pragma once

#include "max.h"
#include <windows.h>
#include "resource.h"

static Tab<TSTR*> g_msgList;

INT_PTR CALLBACK ErrorMsgs_DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);