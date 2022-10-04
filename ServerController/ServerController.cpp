// ServerController.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "ServerController.h"
#include "ServerControllerMsgDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void printDesciption();
int runCommand(TCHAR* szFileName, TCHAR* szCommand);

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
		if (argc < 3)
		{
			cout << "Error:Parameter missing" << endl;
			printDesciption();
			return 0;
		}
		else
		{
			TCHAR szParam1[MAX_PATH] = {0};
			TCHAR szParam2[MAX_PATH] = {0};

			_tcscpy(szParam1, argv[1]);
			_tcscpy(szParam2, argv[2]);

			runCommand(szParam1, szParam2);	
		}
	}

	return nRetCode;
}

int runCommand(TCHAR* szFileName, TCHAR* szCommand)
{
	HWND hWnd = NULL;
	
	// Check Server File Name
	int nFindTitleIndex = -1;
	if (_tcscmp(SERVER_CONTROLLER::szServerFileName[0], szFileName) == 0) 
	{
		nFindTitleIndex = 0;
	}
	else if (_tcscmp(SERVER_CONTROLLER::szServerFileName[1], szFileName) == 0) 
	{
		nFindTitleIndex = 1;
	}
	else if (_tcscmp(SERVER_CONTROLLER::szServerFileName[2], szFileName) == 0) 
	{
		nFindTitleIndex = 2;
	}
	else if (_tcscmp(SERVER_CONTROLLER::szServerFileName[3], szFileName) == 0) 
	{
		nFindTitleIndex = 3;
	}
	else
	{
		cout << "ERROR:Can't find filename" << endl;
		printDesciption();
		return 0;
	}

	// Check Command
	int nCommandIndex = -1;
	if (_tcscmp(SERVER_CONTROLLER::szCommandOption[0], szCommand) == 0)
	{
		nCommandIndex = 0;
	}
	else if(_tcscmp(SERVER_CONTROLLER::szCommandOption[1], szCommand) == 0)
	{
		nCommandIndex = 1;
	}
	else if(_tcscmp(SERVER_CONTROLLER::szCommandOption[2], szCommand) == 0)
	{
		nCommandIndex = 2;
	}
	else if(_tcscmp(SERVER_CONTROLLER::szCommandOption[3], szCommand) == 0)
	{
		nCommandIndex = 3;
	}
	else
	{
		cout << "ERROR:Can't find command" << endl;
		printDesciption();
		return 0;
	}
	
	hWnd = ::FindWindow(NULL, SERVER_CONTROLLER::szServerTitle[nFindTitleIndex]);
	if (hWnd == NULL)
	{
		CString strPath(SERVER_CONTROLLER::getAppPath());
		CString strCommand(SERVER_CONTROLLER::szServerFileName[nFindTitleIndex]);

		strCommand = strPath + "\\" + strCommand;
		// strCommand = strTest + "\\" + strCommand;
		strCommand += " ";
		strCommand += SERVER_CONTROLLER::szCommandOption[nCommandIndex];
				
		STARTUPINFO si;
		memset ( &si, 0, sizeof ( STARTUPINFO ) );
		si.cb = sizeof ( STARTUPINFO );
		si.dwFlags = 0;

		PROCESS_INFORMATION pi;
		pi.hProcess = NULL;
		pi.hThread = NULL;

		cout << strCommand.GetBuffer() << endl;

#ifdef UNICODE
		::CreateProcess(
			NULL, 
			(LPWSTR) strCommand.GetString(),
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL, // m_szFullDir,
			&si,
			&pi);
#else
		::CreateProcess(
			NULL, 
			(LPSTR) strCommand.GetString(),
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL, // m_szFullDir,
			&si,
			&pi);
#endif
	}
	else
	{
		if (nCommandIndex == 0) ::SendMessage(hWnd, RAN_MSG_START, NULL, NULL);
		if (nCommandIndex == 1) ::SendMessage(hWnd, RAN_MSG_STOP, NULL, NULL);
		if (nCommandIndex == 2) ::SendMessage(hWnd, RAN_MSG_EXIT, NULL, NULL);
		if (nCommandIndex == 3) ::SendMessage(hWnd, RAN_MSG_RESTART, NULL, NULL);
	}
	return 0;
}

void printDesciption()
{
	cout << "Usage:ServerController [Filename] [Option]" << endl;
	cout << "EX) ServerController AgentServer.exe start" << endl;
	cout << " " << endl;
	cout << "[Filename]" << endl;
	cout << "AgentServer.exe : Ran-Online Agent Server" << endl;
	cout << "FieldServer.exe : Ran-Online Field Server" << endl;
	cout << "SessionServer.exe : Ran-Online Session Server" << endl;
	cout << "LoginServer.exe : Ran-Online Login Server" << endl;
	cout << " " << endl;
	cout << "[Option]" << endl;
	cout << "start : start the server" << endl;
	cout << "stop : stop the server" << endl;
	cout << "exit : exit the server" << endl;
	cout << "restart : restart the server" << endl;
}