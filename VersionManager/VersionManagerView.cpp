// VersionManagerView.cpp : CVersionManagerView 클래스의 구현
//

#include "stdafx.h"
#include "VersionManager.h"

#include "MainFrm.h"
#include "VersionManagerDoc.h"
#include "VersionManagerView.h"
#include ".\versionmanagerview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVersionManagerView

IMPLEMENT_DYNCREATE(CVersionManagerView, CFormView)

BEGIN_MESSAGE_MAP(CVersionManagerView, CFormView)
END_MESSAGE_MAP()

// CVersionManagerView 생성/소멸

CVersionManagerView::CVersionManagerView()
	: CFormView(CVersionManagerView::IDD)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	

}

CVersionManagerView::~CVersionManagerView()
{
}

void CVersionManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CVersionManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.	
	return CFormView::PreCreateWindow(cs);
}

//void CVersionManagerView::OnInitialUpdate()
//{
//	CFormView::OnInitialUpdate();
//	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();
//
//	
//}


// CVersionManagerView 진단

#ifdef _DEBUG
void CVersionManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVersionManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVersionManagerDoc* CVersionManagerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVersionManagerDoc)));
	return (CVersionManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CVersionManagerView 메시지 처리기

BOOL CVersionManagerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CVersionManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.	
}

void CVersionManagerView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.	
}
