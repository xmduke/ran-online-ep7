// VersionManagerDoc.h : CVersionManagerDoc 클래스의 인터페이스
//


#pragma once

#include "FormLeft.h"
#include "FormRight.h"
#include "MIN_CAB_UTIL.h"

class CVersionManagerDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CVersionManagerDoc();
	DECLARE_DYNCREATE(CVersionManagerDoc)

// 특성
public:
	CFormLeft*	m_pLeft;
	CFormRight* m_pRight;

// 작업
public:

// 재정의
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
    void 	GetAppPath();
    CString m_strFullPath;

// 구현
public:
	virtual ~CVersionManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
    afx_msg void OnFtp();
    afx_msg void OnCheck();
    afx_msg void OnFileSave();
};


