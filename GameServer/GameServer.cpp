// GameServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "s_NetGlobal.h"
#include "GameServer.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	g_hInst=hInstance;
	g_pServer = NULL;

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), HWND_DESKTOP, MainDlgProc);
	return 0;
}

BOOL CALLBACK MainDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	int nRetCode;
	g_hWindow = hDlg;
	char cExecText[100];

	switch(iMessage)
	{
	case WM_INITDIALOG :
		HWND pWnd;
		pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
		EnableWindow(pWnd, TRUE);
		pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
		EnableWindow(pWnd, FALSE);
		break;	
	case WM_COMMAND :
		switch (LOWORD(wParam))
		{		
		case IDC_BTN_EXECUTE : // 명령줄 해석
			if (g_pServer != NULL)
			{
				GetDlgItemText(hDlg, IDC_EDIT_EXEC, cExecText, 100);
				g_pServer->ExecuteCommand(cExecText);
			}
			return TRUE;
		case IDC_BTN_STOP :			
			// Server is running
			if (g_pServer != NULL && g_pServer->IsRunning())
			{					
				nRetCode = g_pServer->Stop();
				if ( nRetCode == NET_OK )
					SetDlgItemText(hDlg, IDC_BTN_STOP, "Start");
				delete g_pServer;
				g_pServer = NULL;
				HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
				EnableWindow(pWnd, FALSE);
			}
			// Server is not running
			else
			{				
				g_pServer = new CGameServer(hDlg, GetDlgItem(hDlg, IDC_EDITCONSOLE), GetDlgItem(hDlg, IDC_EDITCONSOLE_INFO));
				nRetCode = g_pServer->Start();
				if ( nRetCode == NET_OK )
				{
					SetDlgItemText(hDlg, IDC_BTN_STOP, "Stop");
					HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
					EnableWindow(pWnd, TRUE);
				}
				else 
				{
					g_pServer->Stop();
					delete g_pServer;
					g_pServer = NULL;
					HWND pWnd = GetDlgItem(hDlg, IDC_BTN_PAUSE);
					EnableWindow(pWnd, FALSE);
				}
			}	
			return TRUE;
		case IDC_BTN_PAUSE :
			if (g_pServer != NULL)
			{
				if (g_pServer->GetStatus() == S_SERVER_RUNING) // 가동중이면
				{
                    // 중단시킨다.
					g_pServer->Pause();
					SetDlgItemText(hDlg, IDC_BTN_PAUSE, "Resume");
					
					HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
					EnableWindow(pWnd, FALSE);
				}
				else if (g_pServer->GetStatus() == S_SERVER_PAUSE) // 중단중이면
				{
					// 가동시킨다.
					g_pServer->Resume();
					SetDlgItemText(hDlg, IDC_BTN_PAUSE, "Pause");

					HWND pWnd = GetDlgItem(hDlg, IDC_BTN_STOP);
					EnableWindow(pWnd, TRUE);
				}
			}
			return TRUE;
		case IDC_CLOSE :
			if (g_pServer != NULL)
			{
				g_pServer->Stop();
				delete g_pServer;
				g_pServer = NULL;
			}
			EndDialog(hDlg,0);
			return TRUE;
		case IDC_EDITCONSOLE :
			switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;
			break;		
        case IDC_EDITCONSOLE_INFO :
            switch (HIWORD(wParam))
			{			
			case EN_MAXTEXT:			
				SetWindowText((HWND)lParam, "");
				break;
			}
			return TRUE;
			break;
		return FALSE;
        }
	}
	return FALSE;
}