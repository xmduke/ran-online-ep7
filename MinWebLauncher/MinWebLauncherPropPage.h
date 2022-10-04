#pragma once

// MinWebLauncherPropPage.h : CMinWebLauncherPropPage 속성 페이지 클래스의 선언입니다.


// CMinWebLauncherPropPage : 구현을 보려면 MinWebLauncherPropPage.cpp을 참조하십시오.

class CMinWebLauncherPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMinWebLauncherPropPage)
	DECLARE_OLECREATE_EX(CMinWebLauncherPropPage)

// 생성자입니다.
public:
	CMinWebLauncherPropPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPPAGE_MINWEBLAUNCHER };

// 구현
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 메시지 맵입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

