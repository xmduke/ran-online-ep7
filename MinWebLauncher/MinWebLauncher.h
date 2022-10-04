#pragma once

// MinWebLauncher.h : MinWebLauncher.DLL의 기본 헤더 파일입니다.

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // 주 기호입니다.

class CMinWebLauncherApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

// install 설치 함수
void DonwloadInst();