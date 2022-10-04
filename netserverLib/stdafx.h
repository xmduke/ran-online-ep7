// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

//#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.
//
//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#endif
//
//#include <afx.h>
//#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//	dx 관련과 기타 필수 인클루드 및 설정.
#include "dxstdafx.h"

#include <sys\types.h>
#include <sys\stat.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>     // WIN32 and ANSI 
#include <Mmsystem.h>
// #include <varargs.h> // UNIX

#include <fstream>
#include <string>
#include <vector>

#define STRSAFE_LIB
#define STRSAFE_NO_DEPRECATE

// Must Install Platform SDK
// Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
#include <strsafe.h> // Safe string function

