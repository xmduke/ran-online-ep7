// ServerManagerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ServerViewDlg.h"
#include "ServerManager.h"
#include "ServerManagerDlg.h"
#include "Winuser.h"
#include "ChatMsgDlg.h"
#include "s_CClientConsoleMsg.h"
#include "s_CJobSchedule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void CServerManagerDlg::SendAllSMS(CString strText)
{
	if (m_nPhoneSendCount < 5)
	{
		for (int nCount=0; nCount < CServerManagerCfg::GetInstance()->GetSmsPhoneListCount(); nCount++)
		{
			SendSMS( CServerManagerCfg::GetInstance()->GetSmsPhoneList(nCount), strText );
		}
	}
	m_nPhoneSendCount++;
}

#if defined(KRT_PARAM) || defined(KR_PARAM)
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
	CString strSender, strReceiver, strContents, strTemp;

	strSender	= strHP;
	strReceiver	= "RanServer";
	strContents	= strText;

	CString str("Insert Into em_tran( tran_phone, tran_callback, tran_msg ) Values ('" );
	CString str1("','");
	CString str2("')");
	strTemp = str + strHP + str1 + strReceiver + str1 + strContents + str2;	

	int nReturn = COdbcManager::GetInstance()->ExecuteSQL( strTemp );

	/*
    if (m_PPurioX == NULL)
        return;

    USES_CONVERSION;
	BSTR szReturn;
	CString strIp, strPort, strSender, strReceiver, strContents, strId, strPass;    
    
    // New SMS Server
    strIp       = CServerManagerCfg::GetInstance()->GetSmsIP();
    strPort.Format("%d", CServerManagerCfg::GetInstance()->GetSmsPort());
    strId       = CServerManagerCfg::GetInstance()->GetSmsID();
    strPass     = CServerManagerCfg::GetInstance()->GetSmsPassword();
    strSender   = strHP;
    strReceiver = "RanServer";
    strContents = strText;	
	
	m_PPurioX->ConnectServer((_bstr_t)strIp, atoi((LPCSTR)strPort));
	szReturn = m_PPurioX->SendAuth((_bstr_t)strId, (_bstr_t)strPass);
	szReturn = m_PPurioX->SendStart();
	szReturn = m_PPurioX->SendSms((_bstr_t)"1.0", 
		                          (_bstr_t)"sms", 
								  "", 
								  (_bstr_t)strSender, 
								  (_bstr_t)"test", 
								  (_bstr_t)"test", 
								  "", 
								  (_bstr_t)strReceiver, 
								  (_bstr_t)strContents,	
								  "",
								  "");

    CClientConsoleMessage::GetInstance()->WriteWithTime("%s", OLE2A(szReturn));	
	szReturn=m_PPurioX->SendEnd();
	m_PPurioX->DisConnectServer();*/
}
#endif

#if defined(TEST) || defined(_RELEASED)
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
}
#endif

#ifdef JP_PARAM
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
}
#endif

#if defined(MY_PARAM) || defined(MYE_PARAM) || defined(PH_PARAM) || defined(ID_PARAM) || defined ( VN_PARAM ) || defined( TW_PARAM ) || defined(HK_PARAM) || defined ( GS_PARAM )
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
}
#endif

#ifdef CH_PARAM
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
}
#endif

#ifdef TH_PARAM
void CServerManagerDlg::SendSMS(CString strHP, CString strText)
{
}
#endif

// SMS Test Message
void CServerManagerDlg::OnBnClickedButton4()
{    
    SendAllSMS(_T("Ran SMS Test Messsage"));
}

// Session->Server Manager : Server crash!
void CServerManagerDlg::SMS_ServerDown(NET_MSG_GENERIC* nmg)
{
    if (nmg == NULL) return;

	PlayServerDownSound();

    CString strMsg;
    NET_SERVER_INFO* nsi = reinterpret_cast<NET_SERVER_INFO*> (nmg);    
#if defined ( KRT_PARAM ) || defined ( KR_PARAM ) 
    strMsg.Format(_T("(%s)란 %d 서버에 이상이 발견되었습니다"), nsi->gsi.szServerIP, nsi->gsi.nServerGroup);
#endif
#if defined ( TW_PARAM ) || defined(HK_PARAM) 
    strMsg.Format(_T("(%s)RAN %d SERVER ERROR DETECTED"), nsi->gsi.szServerIP, nsi->gsi.nServerGroup);
#endif

	CClientConsoleMessage::GetInstance()->WriteWithTime(strMsg.GetString());
    SendAllSMS(strMsg);
}

// Session->Server Manager : Server crash!
void CServerManagerDlg::SMS_ServerCloseClient(NET_MSG_GENERIC* nmg)
{
	if (nmg == NULL) return;

	PlayServerDownSound();

	CString strMsg;
	NET_SERVER_INFO* nsi = reinterpret_cast<NET_SERVER_INFO*> (nmg);    

#if defined ( KRT_PARAM ) || defined ( KR_PARAM ) 
	strMsg.Format(_T("(%s)란 %d 서버가 종료 되었습니다."), nsi->gsi.szServerIP, nsi->gsi.nServerGroup);
#endif

	CClientConsoleMessage::GetInstance()->WriteWithTime(strMsg.GetString());
	SendAllSMS(strMsg);
}
