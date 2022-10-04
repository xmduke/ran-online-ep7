////////////////////////////////////////////////////////////////////////////////
//	이름	:	CSoundSourceManagerView
//	목적	:	화면에 뿌려줌. 데이타는 Doc에서 각 레코드별로 참고함.
//	부가설명:
//	작성자	:	성기엽
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	로그
//	$2.	2002-10-23T17:22	DirectSound 더블클릭후 바로 들어보기 모듈 작성;
//	$1.	2002-10-22T21:07	최초 작성;
//
////////////////////////////////////////////////////////////////////////////////

// SoundSourceManagerView.h : iCSoundSourceManagerView 클래스의 인터페이스
//


#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class	CSoundManager;	//Directsound 객체
class	CSound;			//DirectSoundBuffer
class	CSoundSourceManagerDoc;
class	CSoundSourceMan;
class CSoundSourceManagerView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CSoundSourceManagerView();
	DECLARE_DYNCREATE(CSoundSourceManagerView)

public:
	enum{ IDD = IDD_SOUNDSOURCEMANAGER_FORM };

// 특성
public:
	CSoundSourceManagerDoc* GetDocument() const;

private:
	CSoundSourceMan	*m_pSoundSourceMan;

public:
	CSoundSourceMan*	GetSSM ( void ) { return m_pSoundSourceMan; }

protected:
	CSoundManager*			m_pSoundManager;	//Directsound 객체
	CSound*					m_pSound;			//DirectSoundBuffer
	BOOL					m_bNormal;			//DirectSound 객체가 정상 초기화되어 있다.

// 재정의
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현
public:
	virtual ~CSoundSourceManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList	m_ImageList;

protected:
	void	ReloadAllItems();
	void	DeleteSelectItems();
	void	CopySelectItems ( const CString& strOriginDir, const CString& strTargetDir );
	void	ChangeState( BOOL State );
	BOOL	CompareAllToken ( CString Left, CString Right );	

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlSoundSourceTable;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_ctrlButtonDelete;
	CButton m_ctrlButtonModify;
	CButton m_ctrlButtonFind;
	CButton m_ctrlButtonWrite;
	CEdit m_ctrlEditKeyword;
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnNMDblclkListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);	
	CString m_valKeyword;
	afx_msg void OnBnClickedButtonFind();
	CComboBox m_ctrlCategory;
	CStatic m_ctrlStaticCategory;
	afx_msg void OnCbnSelchangeComboCategory();
	
public:
	afx_msg void OnMenuitemTextout();
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemIntegrity();
	afx_msg void OnMenuitemFile2listTrace();
};

#ifndef _DEBUG  // SoundSourceManagerView.cpp의 디버그 버전
inline CSoundSourceManagerDoc* CSoundSourceManagerView::GetDocument() const
   { return reinterpret_cast<CSoundSourceManagerDoc*>(m_pDocument); }
#endif

