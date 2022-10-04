========================================================================
    ActiveX 컨트롤 DLL : MinWebLauncher 프로젝트 개요
========================================================================

컨트롤 마법사에서 컨트롤 한 개가 들어 있는 MinWebLauncher
ActiveX 컨트롤 DLL에 대해 이 프로젝트를 만들었습니다.

이 기초 프로젝트는 ActiveX 컨트롤의 기본적인 작성법을
보여 줄 뿐아니라 사용자가 컨트롤의
특정 기능을 작성하는 데 있어 출발점이 될 수 있습니다.

이 파일에는 MinWebLauncher ActiveX 컨트롤 DLL을 구성하는 각각의 파일에
들어 있는 요약 설명이 포함되어 있습니다.

MinWebLauncher.vcproj
    응용 프로그램 마법사를 사용하여 생성한 VC++ 프로젝트의 기본 프로젝트 
    파일입니다. 해당 파일을 생성한 Visual C++의 버전 정보를 비롯하여 
    응용 프로그램 마법사에서 선택한 플랫폼, 구성 및
    프로젝트 기능에 대한 정보가 들어 있습니다.

MinWebLauncher.h
    ActiveX 컨트롤 DLL의 기본 포함 파일입니다.
    이 파일에는 기타 프로젝트 관련 포함 파일(예: resource.h)이 들어 있습니다.

MinWebLauncher.cpp
    DLL 초기화, 종료 및 기타 정리 작업에 필요한
    코드가 들어 있는 기본 소스 파일입니다.

MinWebLauncher.rc
    프로젝트에서 사용하는 Microsoft Windows 리소스의 목록입니다.
    이 파일은 Microsoft Visual C++ 리소스 편집기에서
    직접 편집할 수 있습니다.

MinWebLauncher.def
    이 파일에는 ActiveX 컨트롤 DLL에 대한 정보가 들어 있으며
    이 정보는 Microsoft Windows에서 실행되기 위해 반드시 필요합니다.

MinWebLauncher.idl
    이 파일에는 사용자 컨트롤의 형식 라이브러리에 대한
    개체 설명 언어 소스 코드가 들어 있습니다.

/////////////////////////////////////////////////////////////////////////////
CMinWebLauncherCtrl 컨트롤입니다.

MinWebLauncherCtrl.h
    이 파일에는 CMinWebLauncherCtrl C++ 클래스가 선언되어 있습니다.

MinWebLauncherCtrl.cpp
    이 파일에는  CMinWebLauncherCtrl C++ 클래스가 구현되어 있습니다.

MinWebLauncherPropPage.h
    이 파일에는 CMinWebLauncherPropPage C++ 클래스가 선언되어 있습니다.

MinWebLauncherPropPage.cpp
    이 파일에는 CMinWebLauncherPropPage C++ 클래스가 구현되어 있습니다.

CMinWebLauncherCtrl.bmp
    이 파일에는 CMinWebLauncherCtrl 컨트롤이 도구 팔레트에 표시될 때
    해당 컨트롤을 나타내기 위해 컨테이너에서 사용하는 비트맵이 들어 있습니다.
    해당 비트맵은 기본 리소스 파일인 MinWebLauncher.rc에 의해 포함됩니다.

/////////////////////////////////////////////////////////////////////////////
기타 표준 파일:

Stdafx.h 및 Stdafx.cpp는
    MinWebLauncher.pch라는 이름의 PCH(미리 컴파일된 헤더) 파일과
    Stdafx.obj라는 이름의 PCT(미리 컴파일된 형식) 파일을 빌드하는 데 사용됩니다.

resource.h
    새 리소스 ID를 정의하는 표준 헤더 파일입니다.
    Visual C++ 리소스 편집기에서 이 파일을 읽고 업데이트합니다.

/////////////////////////////////////////////////////////////////////////////
기타 참고:

컨트롤 마법사에서 사용자가 추가하거나 사용자 지정해야 하는
소스 코드 부분을 표시하기 위해 "TODO:"를 사용합니다.

/////////////////////////////////////////////////////////////////////////////
