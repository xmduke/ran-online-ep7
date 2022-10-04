#pragma	  once

class CBitmapMan
{
protected:
	TCHAR	m_szFilePath[_MAX_PATH];	// 로드한 파일의 패스
	TCHAR	m_szFileName[_MAX_FNAME];	// 파일 이름
	TCHAR	m_szExt[_MAX_EXT];			// 확장자

	HBITMAP m_hBitmap;					// 로드한 이미지 핸들
	BITMAP	m_sBitmap;					// 비트맵 구조체

protected:
	CBitmapMan();

public:
	~CBitmapMan();

public:
	void	Delete();
	BOOL	LOAD( CString & strFileName );	// 파일 이름으로 비트맵을 읽는다.

	HBITMAP	GetHBitmap()	{return m_hBitmap;}		// 로드한 이미지 핸들 리턴
	BITMAP	GetBitmap()		{return m_sBitmap;}		// 비트맵 정보를 리턴
	PTCHAR	GetFileName()	{return m_szFileName;}	// 파일명을 넘긴다.
	PTCHAR	GetFilePath()	{return m_szFilePath;}

public:
	static	CBitmapMan	m_cBitmapMan;
};