// PatchPrimeMan.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "PatchPrimeManDlg.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int VERSION;
extern int SGAMEVER;

// CPatchPrimeManApp

BEGIN_MESSAGE_MAP(CPatchPrimeManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPatchPrimeManApp 생성

CPatchPrimeManApp::CPatchPrimeManApp()
{
}


// 유일한 CPatchPrimeManApp 개체입니다.

CPatchPrimeManApp theApp;


// CPatchPrimeManApp 초기화

BOOL CPatchPrimeManApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	
	if( m_lpCmdLine != NULL )
	{
		TCHAR szCmdLine[MAX_PATH] = {0};

		StringCchCopy( szCmdLine, MAX_PATH, m_lpCmdLine );

		TCHAR *token(NULL);
		token = _tcstok( szCmdLine, _T(" ") );
		if( token == NULL )	return FALSE;
		VERSION = _ttoi( token );

		token = _tcstok( NULL, _T(" ") );
		if( token == NULL )	return FALSE;
		SGAMEVER = _ttoi( token );
	}
	else
	{
		//	잘못된 인자
		VERSION = -1; // 인자 없이 실행할 경우, 런쳐에서 값이 넘어오지 않은 경우
	}

	CPatchPrimeManDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 대화 상자가 확인을 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 대화 상자가 취소를 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
	return FALSE;
}