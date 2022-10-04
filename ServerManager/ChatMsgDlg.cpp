// ChatMsgDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ChatMsgDlg.h"


// CChatMsgDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CChatMsgDlg, CDialog)
CChatMsgDlg::CChatMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatMsgDlg::IDD, pParent)
	, m_strMessage(_T(""))
{
}

CChatMsgDlg::~CChatMsgDlg()
{
}

void CChatMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strMessage);
}


BEGIN_MESSAGE_MAP(CChatMsgDlg, CDialog)
END_MESSAGE_MAP()


// CChatMsgDlg 메시지 처리기입니다.
