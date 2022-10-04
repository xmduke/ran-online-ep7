#pragma once

#include "objsafe.h"

#define UUID_LENGTH 37

// 각 게임별로 설치될 레지스트리를 등록해 놓는다.
// 이 레지스트리를 이용해서 설치되었는지 아닌지를 검사한다.
namespace GAME_REGISTRY {

	// 지원할 게임리스트
	TCHAR szGameName[][256] = {
		_T("Ran Online"),
		_T("Ran Online Test") };
	
	// 각 게임이 설치되어 있는 레지스트리 위치
	TCHAR szGameRegistry[][256] = {
		_T("SOFTWARE\\DaumGame\\RAN\\ExePath"),
		_T("SOFTWARE\\DaumGame\\RANTEST\\ExePath") };

}; // End of namespace GAME_REGISTRY

// MinWebLauncherCtrl.h : CMinWebLauncherCtrl ActiveX 컨트롤 클래스의 선언입니다.

// CMinWebLauncherCtrl : 구현을 보려면 MinWebLauncherCtrl.cpp을(를) 참조하십시오.
class CMinWebLauncherCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMinWebLauncherCtrl)

// 생성자입니다.
public:
	CMinWebLauncherCtrl();

// 재정의
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

// 구현
protected:
	~CMinWebLauncherCtrl();

	DECLARE_OLECREATE_EX(CMinWebLauncherCtrl)    // 클래스 팩터리와 GUID입니다.
	DECLARE_OLETYPELIB(CMinWebLauncherCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMinWebLauncherCtrl)     // 속성 페이지 ID입니다.
	DECLARE_OLECTLTYPE(CMinWebLauncherCtrl)		// 형식 이름과 기타 상태입니다.

// 메시지 맵입니다.
	DECLARE_MESSAGE_MAP()	

// 디스패치 맵입니다.
	DECLARE_DISPATCH_MAP()

// 이벤트 맵입니다.
	DECLARE_EVENT_MAP()

// Interface map
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(ObjSafe, IObjectSafety)
	STDMETHOD_(HRESULT, GetInterfaceSafetyOptions) ( 
		/* [in] */ REFIID riid,
		/* [out] */ DWORD __RPC_FAR *pdwSupportedOptions,
		/* [out] */ DWORD __RPC_FAR *pdwEnabledOptions
	);

	STDMETHOD_(HRESULT, SetInterfaceSafetyOptions) ( 
		/* [in] */ REFIID riid,
		/* [in] */ DWORD dwOptionSetMask,
		/* [in] */ DWORD dwEnabledOptions
	);
	END_INTERFACE_PART(ObjSafe);

// 디스패치와 이벤트 ID입니다.
public:
	enum {
		dispidInstallURL = 7,		dispid = 7,	dispidCheckRegi = 6L,	dispidGoDownloadPage = 5L,		dispidStartGame = 4L,		dispidUUID = 3,		dispidGameType = 2,		dispidDownloadURL = 1
	};

public:
	// static CString m_bstrDownloadURL;
	// 자신을 로드한 웹브라우저의 포인터
	IWebBrowser2* m_pIWeb;
	// 자신을 로드한 웹브라우저의 포인터를 가져온다
	IWebBrowser2* GetIWebPointer(void);	
	
	// 레지스트리를 체크해서 게임이 설치되었는지 확인한다.
	bool CheckRegistry();
	void StartGame2( TCHAR* szUUID );
	void WebMessageBox( CString strMsg ); ///< 메시지 박스를 띄운다.
	void WebNavigate( CString strURL ); ///< 특정 사이트로 이동시킨다.		

public: // Wiziard Code
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

protected:
	CString m_DownloadURL;
	LONG m_GameType;
	CString m_UUID;

	void OnDownloadURLChanged(void);	
	void OnGameTypeChanged(void);	
	void OnUUIDChanged(void);	
	void StartGame(void); ///< 게임을 실행시킨다.
	void GoDownloadPage(void); ///< 게임다운로드 페이지로 이동시킨다.
	BOOL CheckRegi(void); // Registery 체크한다...
};