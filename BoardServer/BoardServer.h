#ifndef BOARD_SERVER_H_
#define BOARD_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "s_CBoardServer.h"

// value
HINSTANCE			g_hInst;
HWND				g_hWnd;

HWND				g_hEditBox = NULL;
HWND				g_hWindow = NULL;
BOOL				g_bConnect = FALSE;

CBoardServer*		g_pServer;

// function
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif // BOARD_SERVER_H_