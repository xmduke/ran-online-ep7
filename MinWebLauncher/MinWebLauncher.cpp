// MinWebLauncher.cpp : CMinWebLauncherApp 및 DLL 등록의 구현입니다.

#include "stdafx.h"
#include "MinWebLauncher.h"
#include "s_CHttpPatch.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMinWebLauncherApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xD4EF61A3, 0x18E5, 0x4FB8, { 0xAE, 0x48, 0xD, 0x47, 0xF8, 0x4F, 0x50, 0xC3 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 4;
static const int MAX_HTTP = 24;

const TCHAR szInstallerRegistry[256] = 
		{_T("SOFTWARE\\Microsoft\\Internet Explorer\\Low Rights\\ElevationPolicy") };
const TCHAR szInstallerGUID[256] = 
		{_T("\\{C307A96E-6877-41a4-BA49-861F9CBFAC9C}")};
	// static const GUID <<name>> 	= 
	//		{ 0xc307a96e, 0x6877, 0x41a4, { 0xba, 0x49, 0x86, 0x1f, 0x9c, 0xbf, 0xac, 0x9c } };

// RanInstaller 주소
const TCHAR szDownloadURL[MAX_PATH] = { _T("image.ran-online.co.kr/Active") };
const TCHAR szInstallURL[MAX_PATH] = {_T( "/RanOnlineInstaller.exe") };

// CMinWebLauncherApp::InitInstance - DLL 초기화입니다.

BOOL CMinWebLauncherApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: 여기에 직접 작성한 모듈 초기화 코드를 추가합니다.
	}

	return bInit;
}



// CMinWebLauncherApp::ExitInstance - DLL 종료입니다.

int CMinWebLauncherApp::ExitInstance()
{
	// TODO: 여기에 직접 작성한 모듈 종료 코드를 추가합니다.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - 시스템 레지스트리에 항목을 추가합니다.

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

//	DonwloadInst();

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 시스템 레지스트리에서 항목을 제거합니다.

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}

void DonwloadInst()
{
	CHttpPatch* pHttpPatch;
	pHttpPatch = new CHttpPatch;


	if ( !pHttpPatch )
	{
		return;
	}

	CString DownloadURL = szDownloadURL;
	CString InstallURL = szInstallURL;
	
	CString strRealAddress = DownloadURL;
	CString strFileName = InstallURL;
	
	CString strFilePath = "";
	
	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_WINDOWS, FALSE );

	strFilePath = szPROFILE;
	strFilePath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;

	while ( nERROR_RETRY < 3 )
	{
			//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == MAX_HTTP ) break;

		if ( MAX_HTTP == nTRY ) nTRY = 0;

		static const CString strHTTP = "http://";

		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress;

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strFileName.GetString(), strFilePath.GetString() ) )
		{
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		break;
	}

	SAFE_DELETE( pHttpPatch );


	// Register 등록

	CString strRegKey = szInstallerRegistry;
	strRegKey += szInstallerGUID;
	strRegKey += "\\AppName";

	CRegString regGame(strRegKey, _T(""), TRUE, HKEY_CURRENT_USER);
	regGame = InstallURL.Trim(_T("/"));
	
	regGame.m_key = "AppPath";
	strFilePath.Replace(_T("/"), _T("\\"));
	regGame = szPROFILE;

	CRegDWORD regGame2(strRegKey, 0, TRUE, HKEY_CURRENT_USER);
	regGame2.m_key = "Policy";
	regGame2 = 3;

	return;
}

