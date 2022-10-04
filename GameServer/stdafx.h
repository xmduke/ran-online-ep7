// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

//#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//// Windows 헤더 파일입니다.
//#include <windows.h>
//// C의 런타임 헤더 파일입니다.
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
//#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define STRSAFE_NO_DEPRECATE
#include "strsafe.h"

//	Note : 기본 해더 파일.
//
#include <windows.h>
#include <windowsx.h>
#include <basetsd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>

#include <malloc.h> // _alloca
#include <mmsystem.h>
#include <objbase.h>
#include <assert.h>

bool _gassert ( bool bExp, const char *const szExp, const char *const szFile, const long nLine  );

#ifdef _RELEASED
	#undef assert
	#define assert(exp) \
		if ( _gassert ( (exp)?true:false, #exp, __FILE__, __LINE__ ) ) \
			{ _asm { int 3 } }
#endif

#include <string>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

#ifndef STRICT
	#define STRICT
#endif //STRICT

//	Note : DX 관련 해더 파일.
//
#include <ddraw.h>

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif //DIRECTINPUT_VERSION
#include <dinput.h>

#include <d3d8.h>
#include <D3d8types.h>
#include <D3DX8.h>
#include <D3dx8math.h>
#include <DXErr8.h>
#include <rmxfguid.h>


//	Note : DX 유틸 해더 파일.
//
#include "xrmxftmpl.h"
#include "dxutil.h"
#include "d3dutil.h"
#include "d3dfile.h"
#include "d3dfont.h"
#include "d3dapp.h"

//	Note : engine lib 해더 파일.
//
#include "DebugSet.h"
#include "profile.h"

#include <afxdlgs.h>