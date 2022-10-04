// paddoc.cpp : implementation of the CPadDoc class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "stdafx.h"
#include "superpad.h"
#include "paddoc.h"
#include "paditem.h"
#include "linkitem.h"
#include ".\paddoc.h"
#include "../EngineLib/Common/StringFile.h"
#include "../enginelib/Common/Rijndael.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CPadDoc

IMPLEMENT_DYNCREATE(CPadDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CPadDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CPadDoc)
	ON_COMMAND(ID_VIEW_UPDATENOW, OnViewUpdatenow)
	ON_COMMAND(ID_CANCEL_INPLACE, OnCancelInplace)
//	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
//	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
//	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPadDoc delegation to CEditView

CPadDoc::CPadDoc()
{
}

CPadDoc::~CPadDoc()
{
}

void CPadDoc::DeleteContents()
{
	COleServerDoc::DeleteContents();
	if (m_viewList.IsEmpty())
		return;
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->DeleteContents();
}

void CPadDoc::Serialize(CArchive& ar)
{
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->SerializeRaw(ar);
}

COleServerItem* CPadDoc::OnGetEmbeddedItem()
{
	CEmbeddedItem* pItem = new CEmbeddedItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

COleServerItem* CPadDoc::OnGetLinkedItem(LPCTSTR lpszItemName)
{
	CPadLinkItem *pItem =
		(CPadLinkItem*)COleServerDoc::OnGetLinkedItem(lpszItemName);
	if (pItem == NULL)
		pItem = new CPadLinkItem(this, lpszItemName);

	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////

void CPadDoc::OnViewUpdatenow()
{
	UpdateAllItems(NULL);
}

// Note: both the server and the container should have a keyboard method
//  of deactivating an active in-place item.

void CPadDoc::OnCancelInplace()
{
	if (IsInPlaceActive())
		OnDeactivateUI(FALSE);
}

void CPadDoc::SetSelection(int nBeg, int nEnd)
{
	CEditView* pView = (CEditView*)m_viewList.GetHead();
	ASSERT_KINDOF(CEditView, pView);
	pView->GetEditCtrl().SetSel(nBeg, nEnd);
}

BOOL CPadDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!COleServerDoc::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	char ext[_MAX_EXT] = {0};
	_splitpath( lpszPathName, NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	if( strExt != _T(".CHARSET") && strExt != _T(".CHARCLASS") &&
		strExt != _T(".CLASSCONST") && strExt != _T(".CROWSALE") &&
		strExt != _T(".GENITEM") && strExt != _T(".BIN") )
	{
		if( IDNO == AfxMessageBox( "디코딩 하시겠습니까?", MB_YESNO ) )
			return TRUE;
	}

	CStringFile StrFile;
	if ( !StrFile.Open ( lpszPathName, TRUE ) ) // 스트링파일 클래스 오픈, 디코딩한다.
		return FALSE;

	CString strTemp, strLine;
	while ( StrFile.GetNextLine(strLine) ) // 파일로 부터 한라인씩 읽어온다.
	{
		strLine += "\r\n";
		strTemp += strLine;
	}

	strTemp.SetAt(strTemp.GetLength()-2, NULL); // 마지막 \r\n은 제거한다.
	// 에디트뷰에 출력
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->SetWindowText(strTemp.GetString());

	return TRUE;
}

BOOL CPadDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	char ext[_MAX_EXT] = {0};
	_splitpath( lpszPathName, NULL, NULL, NULL, ext );

	CString strExt(ext);
	strExt.MakeUpper();

	if( strExt != _T(".CHARSET") && strExt != _T(".CHARCLASS") &&
		strExt != _T(".CLASSCONST") && strExt != _T(".CROWSALE") &&
		strExt != _T(".GENITEM") && strExt != _T(".BIN") )
	{
		if( IDNO == AfxMessageBox( "인코딩 하시겠습니까?", MB_YESNO ) )
			return COleServerDoc::OnSaveDocument(lpszPathName);
	}

	
	CString strTemp;
	// 에디트뷰에 텍스트를 읽어온다.
	reinterpret_cast<CEditView*>(m_viewList.GetHead())->GetWindowText(strTemp);

	CRijndael oRijndael;
	oRijndael.Initialize( CRijndael::sm_Version[CRijndael::VERSION-1].c_str(), CRijndael::sm_KeyLength[CRijndael::VERSION-1] );

	int nLen = oRijndael.GetEncryptStringLength( strTemp );
	char * szBuffer = new char[nLen];

	oRijndael.EncryptEx( strTemp.GetString(), szBuffer, strTemp.GetLength() );
	int nVersion = CRijndael::VERSION;

	CFile file;
	file.Open( lpszPathName, CFile::modeCreate|CFile::modeWrite );	// 파일 열기
	file.Write( &nVersion, sizeof(int) );							// 버젼 쓰기
	file.Write( szBuffer, nLen );		// 파일 쓰기
	file.Close();

	delete [] szBuffer;

	SetModifiedFlag(FALSE);
	return TRUE;
}