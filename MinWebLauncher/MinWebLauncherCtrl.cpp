// MinWebLauncherCtrl.cpp : CMinWebLauncherCtrl ActiveX 컨트롤 클래스의 구현입니다.

#include "stdafx.h"
#include "MinWebLauncher.h"
#include "MinWebLauncherCtrl.h"
#include "MinWebLauncherPropPage.h"
#include "helpers.h"
#include "Registry.h"
#include "strsafe.h"
#include ".\minweblauncherctrl.h"
#include "./DaumParam/DaumGameParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMinWebLauncherCtrl, COleControl)

// 메시지 맵입니다.
BEGIN_MESSAGE_MAP(CMinWebLauncherCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// 디스패치 맵입니다.
BEGIN_DISPATCH_MAP(CMinWebLauncherCtrl, COleControl)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "DownloadURL", dispidDownloadURL, m_DownloadURL, OnDownloadURLChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "GameType", dispidGameType, m_GameType, OnGameTypeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CMinWebLauncherCtrl, "UUID", dispidUUID, m_UUID, OnUUIDChanged, VT_BSTR)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "StartGame", dispidStartGame, StartGame, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "GoDownloadPage", dispidGoDownloadPage, GoDownloadPage, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CMinWebLauncherCtrl, "CheckRegi", dispidCheckRegi, CheckRegi, VT_BOOL, VTS_NONE)
END_DISPATCH_MAP()

// 이벤트 맵입니다.
BEGIN_EVENT_MAP(CMinWebLauncherCtrl, COleControl)
END_EVENT_MAP()

// 속성 페이지입니다.

// TODO: 필요에 따라 속성 페이지를 추가합니다. 카운트도 이에 따라 증가해야 합니다.
BEGIN_PROPPAGEIDS(CMinWebLauncherCtrl, 1)
	PROPPAGEID(CMinWebLauncherPropPage::guid)
END_PROPPAGEIDS(CMinWebLauncherCtrl)

// 클래스 팩터리와 GUID를 초기화합니다.
IMPLEMENT_OLECREATE_EX(CMinWebLauncherCtrl, "MINWEBLAUNCHER.MinWebLauncherCtrl.1",
	0x5fffa267, 0xb81, 0x42b4, 0xbe, 0x64, 0x77, 0xb5, 0xc9, 0xfe, 0x28, 0x7f)

// 형식 라이브러리 ID와 버전입니다.
IMPLEMENT_OLETYPELIB(CMinWebLauncherCtrl, _tlid, _wVerMajor, _wVerMinor)

// 인터페이스 ID입니다.
const IID BASED_CODE IID_DMinWebLauncher =
		{ 0xB0242861, 0x6F9F, 0x499F, { 0x8A, 0x8D, 0x9B, 0x53, 0xF2, 0xA8, 0x5, 0x68 } };
const IID BASED_CODE IID_DMinWebLauncherEvents =
		{ 0x55FE2265, 0x552A, 0x4624, { 0xA7, 0x1A, 0xBD, 0xA0, 0xB9, 0x66, 0xDA, 0x9C } };

// 컨트롤 형식 정보입니다.
static const DWORD BASED_CODE _dwMinWebLauncherOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CMinWebLauncherCtrl, IDS_MINWEBLAUNCHER, _dwMinWebLauncherOleMisc)

// CMinWebLauncherCtrl::CMinWebLauncherCtrlFactory::UpdateRegistry -
// CMinWebLauncherCtrl에서 시스템 레지스트리 항목을 추가하거나 제거합니다.
BOOL CMinWebLauncherCtrl::CMinWebLauncherCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 컨트롤이 아파트 모델 스레딩 규칙을 준수하는지
	// 확인합니다. 자세한 내용은 MFC Technical Note 64를
	// 참조하십시오. 컨트롤이 아파트 모델 규칙에
	// 맞지 않는 경우 다음에서 여섯 번째 매개 변수를 변경합니다.
	// afxRegApartmentThreading을 0으로 설정합니다.

	// Mark as safe for scripting-failure OK.
    HRESULT hr = CreateComponentCategory(CATID_SafeForScripting, 
        L"Controls that are safely scriptable");
    
    if (SUCCEEDED(hr))
        // Only register if category exists.
        RegisterCLSIDInCategory(m_clsid, CATID_SafeForScripting);
        // Don't care if this call fails.
    
    // Mark as safe for data initialization.
    hr = CreateComponentCategory(CATID_SafeForInitializing, 
        L"Controls safely initializable from persistent data");
    
    if (SUCCEEDED(hr))
        // Only register if category exists.
        RegisterCLSIDInCategory(m_clsid, CATID_SafeForInitializing);
        // Don't care if this call fails.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MINWEBLAUNCHER,
			IDB_MINWEBLAUNCHER,
			afxRegApartmentThreading,
			_dwMinWebLauncherOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

// CMinWebLauncherCtrl::CMinWebLauncherCtrl - 생성자입니다.
CMinWebLauncherCtrl::CMinWebLauncherCtrl()
	: m_pIWeb( NULL )
{
	InitializeIIDs(&IID_DMinWebLauncher, &IID_DMinWebLauncherEvents);
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 초기화합니다.
}

// CMinWebLauncherCtrl::~CMinWebLauncherCtrl - 소멸자입니다.
CMinWebLauncherCtrl::~CMinWebLauncherCtrl()
{
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 정리합니다.
}

// CMinWebLauncherCtrl::OnDraw - 그리기 함수입니다.
void CMinWebLauncherCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: 다음 코드를 사용자가 직접 작성한 그리기 코드로 바꾸십시오.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CMinWebLauncherCtrl::DoPropExchange - 지속성 지원입니다.
void CMinWebLauncherCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));

	// 브라우저에서 파라미터로 넘기면 입력되게 하기 위해서는 이곳에 지정해 주어야 한다.
	// 다운로드 URL 을 넘긴다.
	PX_String( pPX, _T("DownloadURL"), m_DownloadURL );
	PX_String( pPX, _T("UUID"), m_UUID );
	PX_Long  ( pPX, _T("GameType"), m_GameType, 0 );
	
	COleControl::DoPropExchange(pPX);

	// TODO: 지속적인 사용자 지정 속성 모두에 대해 PX_ functions를 호출합니다.
}

// CMinWebLauncherCtrl::GetControlFlags -
// MFC의 ActiveX 컨트롤 구현을 사용자 지정하는 플래그입니다.
//
DWORD CMinWebLauncherCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// 활성 및 비활성 상태 사이에서 전환할 때는
	// 컨트롤을 다시 그릴 수 없습니다.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}

// CMinWebLauncherCtrl::OnResetState - 컨트롤을 기본 상태로 다시 설정합니다.
void CMinWebLauncherCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange에 들어 있는 기본값을 다시 설정합니다.

	// TODO: 여기에서 다른 모든 컨트롤의 상태를 다시 설정합니다.
}

/////////////////////////////////////////////////////////////
// Interface map
BEGIN_INTERFACE_MAP( CMinWebLauncherCtrl, COleControl )
	INTERFACE_PART(CMinWebLauncherCtrl, IID_IObjectSafety, ObjSafe)
//	INTERFACE_PART(COcxPuzCtrl, IID_IObjectSafety, ObjSafe)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////
// IObjectSafety member functions
ULONG FAR EXPORT CMinWebLauncherCtrl::XObjSafe::AddRef()
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return pThis->ExternalAddRef();
}

ULONG FAR EXPORT CMinWebLauncherCtrl::XObjSafe::Release()
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return pThis->ExternalRelease();
}

HRESULT FAR EXPORT CMinWebLauncherCtrl::XObjSafe::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    return (HRESULT) pThis->ExternalQueryInterface(&iid, ppvObj);
}

const DWORD dwSupportedBits = 
        INTERFACESAFE_FOR_UNTRUSTED_CALLER |
        INTERFACESAFE_FOR_UNTRUSTED_DATA;
const DWORD dwNotSupportedBits = ~ dwSupportedBits;
        
HRESULT STDMETHODCALLTYPE 
    CMinWebLauncherCtrl::XObjSafe::GetInterfaceSafetyOptions( 
        /* [in] */ REFIID riid,
        /* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
        /* [out] */ DWORD __RPC_FAR *pdwEnabledOptions)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)

    HRESULT retval = ResultFromScode(S_OK);

    // does interface exist?
    IUnknown FAR* punkInterface;
    retval = pThis->ExternalQueryInterface(&riid, 
                    (void * *)&punkInterface);
    if (retval != E_NOINTERFACE) {  // interface exists
        punkInterface->Release(); // release it--just checking!
    }
    
    // we support both kinds of safety and have always both set,
    // regardless of interface
    *pdwSupportedOptions = *pdwEnabledOptions = dwSupportedBits;

    return retval; // E_NOINTERFACE if QI failed
}

HRESULT STDMETHODCALLTYPE 
    CMinWebLauncherCtrl::XObjSafe::SetInterfaceSafetyOptions( 
        /* [in] */ REFIID riid,
        /* [in] */ DWORD dwOptionSetMask,
        /* [in] */ DWORD dwEnabledOptions)
{
    METHOD_PROLOGUE(CMinWebLauncherCtrl, ObjSafe)
    
    // does interface exist?
    IUnknown FAR* punkInterface;
    pThis->ExternalQueryInterface(&riid,
        (void**)&punkInterface);
    if (punkInterface) {    // interface exists
        punkInterface->Release(); // release it--just checking!
    }
    else { // interface doesn't exist
        return ResultFromScode(E_NOINTERFACE);
    }

    // can't set bits we don't support
    if (dwOptionSetMask & dwNotSupportedBits) { 
        return ResultFromScode(E_FAIL);
    }
    
    // can't set bits we do support to zero
    dwEnabledOptions &= dwSupportedBits;
    // (we already know there are no extra bits in mask )
    if ((dwOptionSetMask & dwEnabledOptions) !=
            dwOptionSetMask) {
        return ResultFromScode(E_FAIL);
    }                               
    
    // don't need to change anything since we're always safe
    return ResultFromScode(S_OK);
}

// CMinWebLauncherCtrl 메시지 처리기입니다.

// 레지스트리를 체크해서 게임이 설치되었는지 확인한다.
bool CMinWebLauncherCtrl::CheckRegistry()
{
	if (m_GameType < 0 || m_GameType >= 10)
		m_GameType = 0;

	CRegString regGame(GAME_REGISTRY::szGameRegistry[m_GameType], _T(""), TRUE, HKEY_LOCAL_MACHINE);
	CString strReg = regGame.read();

	if (strReg == _T(""))
	{
		// 레지스트리에 없음...
		// 설치되지 않았다고 판단하고 다운로드 페이지로 이동시킨다.
		WebMessageBox( _T("게임이 설치되어있지 않습니다.") );

		return false;

	}	

	return true;
}

void CMinWebLauncherCtrl::StartGame2( TCHAR* szUUID )
{
	if (m_GameType < 0 || m_GameType >= 10)
	m_GameType = 0;

	CRegString regGame(GAME_REGISTRY::szGameRegistry[m_GameType], _T(""), TRUE, HKEY_LOCAL_MACHINE);
	CString strReg = regGame.read();

    if (strReg == _T(""))
	{
		WebMessageBox( _T("게임이 설치가 안되어있습니다.") );
		GoDownloadPage();
	}
	else
	{
		// 레지스트리에 있음...		
		CString strCmd = strReg;

		CDaumGameParameter cDGP;
		TCHAR szParameter[MAX_DGPARAMETER_LENGTH] = {0};
		size_t length = 0;
		::StringCchLength( szUUID, MAX_DGPARAMETER_LENGTH, &length );
		::StringCchCopy( szParameter, length+1 , szUUID );	

		if ( cDGP.Set ( "Parameter", szParameter ) == FALSE )
		{
			return;
		}

		// 런쳐실행
		int result = (int)ShellExecute( NULL, _T("open"), strCmd, NULL, NULL, SW_SHOWNORMAL);
		if ( result < 32 )
		{
			WebMessageBox( _T("게임 런쳐를 실행할수 없습니다.") );
			return;
		}

	}

}

void CMinWebLauncherCtrl::WebMessageBox(CString strMsg)
{
	if (m_pIWeb != NULL)
	{
		HWND hWnd = NULL;
		m_pIWeb->get_HWND( (long*) &hWnd );
		if (hWnd != NULL)
		{
			::MessageBox( hWnd,  strMsg, _T("Error"), MB_OK );
		}
	}
}

///< 특정 사이트로 이동시킨다.
void CMinWebLauncherCtrl::WebNavigate(CString strURL)
{
	if (m_pIWeb != NULL)
	{		
		COleVariant varURL( strURL );
		m_pIWeb->Navigate2( varURL, NULL, NULL, NULL, NULL );
	}
}

void CMinWebLauncherCtrl::OnDownloadURLChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 속성 처리기 코드를 추가합니다.
	SetModifiedFlag();
}

int CMinWebLauncherCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	// 자신을 로드한 웹브라우저의 포인터 저장
	m_pIWeb = GetIWebPointer();
	return 0;
}

void CMinWebLauncherCtrl::OnDestroy()
{
	COleControl::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// 컨트롤이 사라질때 OnCreate 에서 가져온 Web Browser 의 포인터를 Release 해 준다.
	if (m_pIWeb != NULL)
	    m_pIWeb->Release() ;
}

IWebBrowser2* CMinWebLauncherCtrl::GetIWebPointer(void)
{
	HRESULT hr;
	IOleContainer *pIContainer = NULL;
	IWebBrowser2 *pIWeb = NULL;
	IServiceProvider *pISP = NULL;
	
	// Get IOleClientSite interface pointer.
	LPOLECLIENTSITE pIClientSite = GetClientSite();
	
	// Get IOleContainer interface poineter.
	hr = pIClientSite->GetContainer( &pIContainer );
	if (hr != S_OK) 
	{
	    pIClientSite->Release();
	    return NULL;
	}
	
	// Get IServiceProvider interface pointer.
	hr = pIClientSite->QueryInterface(
							IID_IServiceProvider,
							(void **) &pISP );
	if (hr != S_OK) 
	{
	    pIContainer->Release();
	    pIClientSite->Release();
	    return NULL;
	}

	// Get IWebBrowser2 interface pointer.
	hr = pISP->QueryService(
					IID_IWebBrowserApp,
					IID_IWebBrowser2,
					(void**) &pIWeb );
	if (hr != S_OK) 
	{
	    pIContainer->Release();
	    pIClientSite->Release();
	    pISP->Release();
	    return NULL;
	}
	
	// release interface.
	pIContainer->Release();
	pIClientSite->Release();
	pISP->Release();
	return pIWeb;	
}

void CMinWebLauncherCtrl::OnGameTypeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 속성 처리기 코드를 추가합니다.
	// WebMessageBox( _T("OnGameTypeChanged") );
	SetModifiedFlag();
}

void CMinWebLauncherCtrl::OnUUIDChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 속성 처리기 코드를 추가합니다.
	// WebMessageBox( _T("OnUUIDChanged") );
	SetModifiedFlag();
}

void CMinWebLauncherCtrl::StartGame(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

   	// TODO: 여기에 디스패치 처리기를 추가합니다.
	if ( m_UUID.GetLength() < (UUID_LENGTH-1) )
	{
		WebMessageBox( _T("Please login at the official website to start Ran-Online.") );
		return;
	}

	TCHAR szUUID[UUID_LENGTH] = {0};
	::StringCchCopy( szUUID, UUID_LENGTH, m_UUID.GetBuffer() );	
	StartGame2( szUUID );
}
void CMinWebLauncherCtrl::GoDownloadPage(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 디스패치 처리기를 추가합니다.
	if (m_pIWeb != NULL)
	{
		WebNavigate( m_DownloadURL );
	}
}

BOOL CMinWebLauncherCtrl::CheckRegi(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if ( CheckRegistry() )
	{
		return TRUE;
	}
	
	return FALSE;

}



