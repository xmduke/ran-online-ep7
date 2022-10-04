// SoundSourceManager.h : SoundSourceManager 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // 주 기호


// CSoundSourceManagerApp:
// 이 클래스의 구현에 대해서는 SoundSourceManager.cpp을 참조하십시오.
//

class CSoundSourceManagerApp : public CWinApp
{
public:
	CSoundSourceManagerApp();

public:
	CString m_strAppPath;

// 재정의
public:
	virtual BOOL InitInstance();

// 구현
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSoundSourceManagerApp theApp;
