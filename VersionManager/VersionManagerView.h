// VersionManagerView.h : iCVersionManagerView 클래스의 인터페이스
//


#pragma once


class CVersionManagerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CVersionManagerView();
	DECLARE_DYNCREATE(CVersionManagerView)

public:
	enum{ IDD = IDD_VERSIONMANAGER_FORM };

// 특성
public:
	CVersionManagerDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원
//	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현
public:
	virtual ~CVersionManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // VersionManagerView.cpp의 디버그 버전
inline CVersionManagerDoc* CVersionManagerView::GetDocument() const
   { return reinterpret_cast<CVersionManagerDoc*>(m_pDocument); }
#endif

