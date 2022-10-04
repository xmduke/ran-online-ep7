// SoundSourceManagerDoc.cpp : CSoundSourceManagerDoc 클래스의 구현
//

#include "stdafx.h"
#include "SoundSourceManager.h"

#include "SoundSourceManagerDoc.h"
#include "MainFrm.h"

#include <d3d8types.h>
#include "SerialFile.h"
#include "SoundSourceMan.h"
#include ".\soundsourcemanagerdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSoundSourceManagerDoc

IMPLEMENT_DYNCREATE(CSoundSourceManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSoundSourceManagerDoc, CDocument)
END_MESSAGE_MAP()


// CSoundSourceManagerDoc 생성/소멸

CSoundSourceManagerDoc::CSoundSourceManagerDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
}

CSoundSourceManagerDoc::~CSoundSourceManagerDoc()
{
}

BOOL CSoundSourceManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 다시 초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	return TRUE;
}

// CSoundSourceManagerDoc serialization

void CSoundSourceManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CSoundSourceManagerDoc 진단

#ifdef _DEBUG
void CSoundSourceManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSoundSourceManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSoundSourceManagerDoc 명령
//
//BOOL	CSoundSourceManagerDoc::DefaultSaveFile ()
//{
//	CString	FullPath = m_pSoundSourceMan->GetDataDirectory() + g_szFileName;
//	return m_pSoundSourceMan->SaveFile ( FullPath.GetString() );
//}
//
//BOOL	CSoundSourceManagerDoc::DefaultLoadFile ()
//{
//	CString	FullPath = m_pSoundSourceMan->GetDataDirectory() + g_szFileName;
//	return m_pSoundSourceMan->LoadFile ( FullPath.GetString() );
//}



