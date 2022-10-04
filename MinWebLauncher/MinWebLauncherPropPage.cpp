// MinWebLauncherPropPage.cpp : CMinWebLauncherPropPage 속성 페이지 클래스의 구현입니다.

#include "stdafx.h"
#include "MinWebLauncher.h"
#include "MinWebLauncherPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMinWebLauncherPropPage, COlePropertyPage)



// 메시지 맵입니다.

BEGIN_MESSAGE_MAP(CMinWebLauncherPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 클래스 팩터리와 GUID를 초기화합니다.

IMPLEMENT_OLECREATE_EX(CMinWebLauncherPropPage, "MINWEBLAUNCHER.MinWebLauncherPropPage.1",
	0xd18689d1, 0xf771, 0x4d49, 0xbd, 0xa9, 0x9, 0xd4, 0x8f, 0xb6, 0x28, 0xb6)



// CMinWebLauncherPropPage::CMinWebLauncherPropPageFactory::UpdateRegistry -
// CMinWebLauncherPropPage에서 시스템 레지스트리 항목을 추가하거나 제거합니다.

BOOL CMinWebLauncherPropPage::CMinWebLauncherPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MINWEBLAUNCHER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CMinWebLauncherPropPage::CMinWebLauncherPropPage - 생성자입니다.

CMinWebLauncherPropPage::CMinWebLauncherPropPage() :
	COlePropertyPage(IDD, IDS_MINWEBLAUNCHER_PPG_CAPTION)
{
}



// CMinWebLauncherPropPage::DoDataExchange - 페이지와 속성 사이에서 데이터를 이동시킵니다.

void CMinWebLauncherPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CMinWebLauncherPropPage 메시지 처리기입니다.
