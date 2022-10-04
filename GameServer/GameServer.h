#ifndef GAME_SERVER_H_
#define GAME_SERVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "s_CGameServer.h"

// value
HINSTANCE			g_hInst;
HWND				g_hWnd;

HWND				g_hEditBox = NULL;
HWND				g_hWindow = NULL;
BOOL				g_bConnect = FALSE;

CGameServer*		g_pServer;

// function
BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);

#endif // GAME_SERVER_H_





