// AutoPatchMan.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "Custsite.h"
#include "DaumGameParameter.h"
#include "RANPARAM.h"
#include "LauncherText.h"

#include "GlobalVariable.h"
#include "../MfcExLib/MinBugTrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAutoPatchManApp

BEGIN_MESSAGE_MAP(CAutoPatchManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoPatchManApp 생성

CAutoPatchManApp::CAutoPatchManApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	m_pDispOM = NULL;

#ifdef _DEBUG
	m_memState = NULL;
#endif

	BUG_TRAP::BugTrapInstall( std::string(_T("Launcher")));
}


CAutoPatchManApp::~CAutoPatchManApp()
{
	if ( m_pDispOM != NULL)
	{
		//This is the way that the driller sample disposes 
		//the m_pDispOM object.
		//Note that the CImpIDispatch::Release() method 
		//is (intentionally?) never disposing its object! 
		delete m_pDispOM;
	}

#ifdef _DEBUG
	if (m_memState != NULL)
	{
		m_memState->DumpAllObjectsSince();

		delete m_memState;
	}
#endif
}


// 유일한 CAutoPatchManApp 개체입니다.

CAutoPatchManApp theApp;


// CAutoPatchManApp 초기화

BOOL CAutoPatchManApp::InitInstance()
{

	#ifdef KR_PARAM
	{
		//	Note : 다음 게임 파라메타 확인.
		//
		CDaumGameParameter cDGP;
		char szParameter[MAX_DGPARAMETER_LENGTH];
		if ( cDGP.Get ( "Parameter", szParameter, MAX_DGPARAMETER_LENGTH ) == FALSE )
		{
			CString strMsg;
//			strMsg = ID2LAUNCHERTEXT( "IDS_MESSAGE",1 ); // 웹에서 실행 
			strMsg.LoadString(IDS_MESSAGE1);
			MessageBox ( NULL, strMsg.GetString() , "RAN-Online", MB_OK );
			return FALSE;
		}
		//	다음 게임 파라메타의 내용을 지워줍니다.
		//cDGP.Empty();
	}
	#endif	// KR_PARAM

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(new CCustomOccManager);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef _DEBUG
	m_memState = new CMemoryState;
	m_memState->Checkpoint();
#endif

	CAutoPatchManDlg dlg;
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

BOOL CAutoPatchManApp::InitApplication()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ( FindWindow ( NULL, NS_GLOBAL_VAR::strTitle.GetString() ) )
	{		
		return FALSE;
	}

	return CWinApp::InitApplication();
}
