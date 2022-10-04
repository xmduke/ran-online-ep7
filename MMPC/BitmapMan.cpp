#include "stdafx.h"
#include "BitmapMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBitmapMan CBitmapMan::m_cBitmapMan;

CBitmapMan::CBitmapMan() :
	m_hBitmap(NULL)
{
	SecureZeroMemory( m_szFilePath, sizeof( m_szFilePath ) );
	SecureZeroMemory( m_szFileName, sizeof( m_szFileName ) );
	SecureZeroMemory( m_szExt, sizeof( m_szExt ) );

	SecureZeroMemory( &m_sBitmap, sizeof( m_sBitmap ) );
}

CBitmapMan::~CBitmapMan()
{
	Delete();
}

void CBitmapMan::Delete()
{
	if( NULL != m_hBitmap )
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
}

BOOL CBitmapMan::LOAD( CString & strFileName )
{
	Delete();

	// 비트맵 정보를 읽는다.
	m_hBitmap = (HBITMAP)LoadImage(NULL,
			strFileName,
			IMAGE_BITMAP,
			NULL,
			NULL,
			LR_LOADFROMFILE|LR_CREATEDIBSECTION);

	strcpy( m_szFilePath, strFileName );
	_splitpath(m_szFilePath, NULL, NULL, m_szFileName, m_szExt);
	strcat(m_szFileName, m_szExt); // 파일 이름에 확장자를 붙인다.

	GetObject(m_hBitmap, sizeof(BITMAP), &m_sBitmap); // 비트맵 정보를 얻는다.
	
	return TRUE;
}