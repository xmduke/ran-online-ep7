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

void CServerManagerDlg::OnBnClickedBtnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CString strIPAddress;
	CString strPort;
	int		nPort;
	int		nRetCode;

    if (m_pNetClient != NULL)
    {
        if (m_pNetClient->IsOnline())
	    {   
		    nRetCode = m_pNetClient->CloseConnect();		
		    if (nRetCode == NET_OK)
			    CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Connection closed"));
	    }
        SAFE_DELETE(m_pNetClient);
    }

    m_pNetClient = new CNetCtrl(m_hWnd);
	m_pNetClient->SetCtrlHandle(this);

	m_IPAddress.GetWindowText(strIPAddress);
	m_Port.GetWindowText(strPort);
	nPort = ::atoi(strPort);

	nRetCode = m_pNetClient->ConnectServer(strIPAddress, nPort);

    if (nRetCode == NET_OK)
	{
		CString s;
		CString strTimer;
		s.LoadString(IDS_STRING102);
		CClientConsoleMessage::GetInstance()->WriteWithTime(s);
		m_Led1.SetLed( CLed::LED_COLOR_GREEN, CLed::LED_ON, CLed::LED_ROUND );
		m_EditRefreshRate.GetWindowText(strTimer);
		if (strTimer.IsEmpty() == true)
            SetTimer(1, 60000, 0);
		else
			SetTimer(1, atoi(strTimer) * 1000, 0);
		
		CLIENT_JOB::CScheduler::GetInstance()->SetNetCtrl(m_pNetClient);

		GetDlgItem( IDC_BTN_JOB_SCHEDULE)->EnableWindow(TRUE);
	}
	else
	{
		CString s;
		s.LoadString(IDS_STRING103);
		CClientConsoleMessage::GetInstance()->WriteWithTime(s);
		m_Led1.SetLed( CLed::LED_COLOR_RED, CLed::LED_ON, CLed::LED_ROUND );
		return;
	}
	Sleep( 500 );
	m_pNetClient->SndReqAllSvrInfo();

	m_vecTracingData.clear();
	UpdateTracingList();
	SendTracingList();
}


void CServerManagerDlg::MsgProcess(NET_MSG_GENERIC* nmg)
{
	switch (nmg->nType)
	{
	case NET_MSG_HEARTBEAT_SERVER_REQ: // Session->Login:Heartbeat check request
		m_pNetClient->SessionMsgHeartbeatServerAns();
		break;
    case NET_MSG_SVR_DOWN: // 서버 다운알림
        SMS_ServerDown(nmg);
        break;
	case NET_MSG_SVR_CLOSECLIENT:
		SMS_ServerCloseClient(nmg);
		break;
	case NET_MSG_SND_ALL_SVR_INFO:		
		MsgAllSvrInfo(nmg);
		break;
	case NET_MSG_SND_ALL_SVR_INFO_S: // 서버 정보 전송 시작		
		UpdateSvrListStart();
		break;
	case NET_MSG_SND_ALL_SVR_INFO_E: // 서버 정보 전송이 끝남		
		UpdateSvrListEnd();
		break;
	//case NET_MSG_UPDATE_TRACING_ALL:
	//	if( m_bFirstRcvTracing == TRUE ) MsgUpdateTracingChar(nmg,TRUE);
	//	break;
	case NET_MSG_UPDATE_TRACING_CHARACTER:
		MsgUpdateTracingChar(nmg,FALSE);
		break;
	default:
		break;
	}
}


void CServerManagerDlg::UpdateSvrListStart()
{
	POSITION pos = NULL;
	G_SERVER_INFO_DLG* gsid;

	pos = m_DlgList.GetHeadPosition();

	for (int i=0; i<m_DlgList.GetCount(); i++)
	{
		gsid = m_DlgList.GetAt(pos);
		gsid->bCheck = FALSE;
		m_DlgList.GetNext(pos);
		Sleep( 1 );
	}
}

void CServerManagerDlg::PlayServerDownSound()
{
	CString	strPath = GetAppPath();

	CString strSND1 = strPath + "\\help.wav";
	CString strSND2 = strPath + "\\im_in_trouble.wav";

	PlaySound(strSND1, NULL, SND_SYNC);
	PlaySound(strSND2, NULL, SND_SYNC);	
}

void CServerManagerDlg::PlayTracingUserLogOnOff()
{
	CString	strPath = GetAppPath();

	CString strSND1 = strPath + "\\ringin.wav";
	CString strSND2 = strPath + "\\help.wav";

	PlaySound(strSND1, NULL, SND_SYNC);
	PlaySound(strSND2, NULL, SND_SYNC);	
}

// 서버 정보 전송이 끝남
void CServerManagerDlg::UpdateSvrListEnd()
{
	// 전체사용자수를 계산한다.
	POSITION pos = NULL;
	int nMaxUser = 0;
	int nMaxSvrUser = 0;
	G_SERVER_INFO_DLG* gsid;
	int nMaxAgent = 0;

	pos = m_DlgList.GetHeadPosition();
    
	for (int i=0; i<m_DlgList.GetCount(); i++)
	{
		gsid = (G_SERVER_INFO_DLG*) m_DlgList.GetAt(pos);
		if (gsid->bCheck == FALSE)
		{
			PlayServerDownSound();

			CString strMsg;
            strMsg.Format(_T("(%d)(%s)Server have problem"),
						  gsid->gsi.nServerGroup,
						  gsid->gsi.szServerIP);            

            SendAllSMS(strMsg); // 서버다운 SMS 전송
			CClientConsoleMessage::GetInstance()->WriteWithTime(strMsg.GetString());
		}

		// Agent 서버일때만 동시접속자수에 계산을 한다.
		if (gsid->gsi.nServerType == NET_SERVER_AGENT)
		{
			nMaxAgent++; // Agent 서버 갯수 계산			
			nMaxUser    += gsid->gsi.nServerCurrentClient;
			nMaxSvrUser += gsid->gsi.nServerMaxClient;
		}
		m_DlgList.GetNext(pos);

		if (m_pLogFile)
		{
#ifdef CH_PARAM

#else
			m_pLogFile->WriteWithTime(_T("%d,%d,%d"), 
                                      gsid->gsi.nServerType,
								      gsid->gsi.nServerGroup,
								      gsid->gsi.nServerCurrentClient);
#endif
		}

		Sleep( 1 );
	}
	
	
	// 현재 전체사용자수가 과거 최대 사용자수 보다 많으면 업데이트 한다.
	if (nMaxUser > m_nMaxUser)
		m_nMaxUser = nMaxUser;

    // 로그파일 쓰기
#ifdef CH_PARAM
	
#else
	m_pLogFile->WriteWithTime(_T("Total, Max, Number,%d,%d"), nMaxUser, nMaxSvrUser);
#endif

	CString strTemp; 
	strTemp.Format("%d", m_nMaxUser);
#ifdef CH_PARAM // 중국은 최대 동접 삭제
	
#else
	m_MaxUser.SetText( strTemp );
#endif
	strTemp.Format("%d", nMaxUser);
#ifdef CH_PARAM // 중국은 현재 동접 삭제

#else
	m_CrtUser.SetText(strTemp);
#endif

	int nPercent=0;
	if (nMaxSvrUser > 0)
	{
		nPercent = static_cast<int> (nMaxUser*100/nMaxSvrUser);
		if (nPercent <= 5)
			nPercent = 5;
	}
	else
	{
        nPercent = 0;
	}
	m_Histogram.Add(nPercent);
}

void CServerManagerDlg::MsgAllSvrInfo(NET_MSG_GENERIC* nmg)
{
	NET_SERVER_INFO* nsi;
	CString s;

	nsi = reinterpret_cast<NET_SERVER_INFO*> (nmg);

	if (m_DlgList.IsEmpty())
	{
		// 리스트에 추가
		G_SERVER_INFO_DLG* gsid = new G_SERVER_INFO_DLG;	

		gsid->gsi    = nsi->gsi;
#ifdef CH_PARAM
		gsid->gsi.nServerChannellMaxClient = 1;
		gsid->gsi.nServerCurrentClient = 1;
#endif		
		gsid->pDlg   = NULL;
		gsid->bOpen  = FALSE;
		gsid->bCheck = TRUE;

		m_DlgList.AddHead(gsid);
		m_vecServerList.push_back(gsid);
		AddListCtrl(gsid);

		CClientConsoleMessage::GetInstance()->WriteWithTime(_T("MsgAllSvrInfo %s %d"),
			                                                gsid->gsi.szServerIP,
															gsid->gsi.nServicePort);
	}
	else
	{
		G_SERVER_INFO_DLG* gsid;
		POSITION pos = m_DlgList.GetHeadPosition();

		for (int i=0; i<m_DlgList.GetCount(); i++)
		{
			gsid = m_DlgList.GetAt(pos);
			if (gsid->gsi == nsi->gsi) // 이미리스트에 있음
			{
				gsid->gsi = nsi->gsi;
				gsid->bCheck = TRUE;
				
				// 리스트 컨트롤 업데이트
				CString strTemp;
				strTemp.Format(_T("%d/%d"), gsid->gsi.nServerCurrentClient, gsid->gsi.nServerMaxClient);
#ifdef CH_PARAM

#else
				m_ListServer.SetItemText(gsid->nRow, 5, strTemp);
#endif

				// s.LoadString(IDS_STRING104);
				// CClientConsoleMessage::GetInstance()->WriteWithTime(s);
				if (gsid->pDlg != NULL) // 상태창이 열려있음
				{
					// 상태창 업데이트
					gsid->pDlg->UpdateStatus(nsi->gsi);
				}
				return;
			}
			m_DlgList.GetNext(pos);
			Sleep( 1 );
		}
	
		// 리스트에 없음, 추가
		
		gsid = new G_SERVER_INFO_DLG;
		gsid->gsi    = nsi->gsi;
		gsid->pDlg   = NULL;		
		gsid->bOpen  = FALSE;
		gsid->bCheck = TRUE;

		CClientConsoleMessage::GetInstance()->WriteWithTime(_T("MsgAllSvrInfo %s %d"),
			                                                gsid->gsi.szServerIP,
															gsid->gsi.nServicePort);

		m_DlgList.AddHead(gsid);
		AddListCtrl(gsid);		
		m_vecServerList.push_back(gsid);
	}
}

void CServerManagerDlg::AddListCtrl(G_SERVER_INFO_DLG* gsid)
{
	
	if (gsid->gsi.nServerType != 0)
	{
		// 상태
		// 종류
		// 이름
		// 주소
		// 포트
		// 접속자
		// Progress
		
		CString strTemp;
		strTemp.LoadString(IDS_STRING106); // 정상		

		int nCount; 

		nCount = m_ListServer.GetItemCount();

		// Save row number
		gsid->nRow = nCount;		

		LV_ITEM lvItem;		
		ZeroMemory(&lvItem, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 이미지 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		lvItem.pszText = strTemp.GetBuffer();;		
		m_ListServer.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.		

		// gsid 의 pointer 저장
		m_ListServer.SetItemData(nCount, reinterpret_cast <DWORD_PTR> (gsid));

		if (gsid->gsi.nServerType == SERVER_LOGIN)			
			strTemp = "LOGIN";
		else if (gsid->gsi.nServerType == SERVER_SESSION)		
			strTemp = "SESSION";
		else if (gsid->gsi.nServerType == SERVER_FIELD)		
			strTemp = "FIELD";
		else if (gsid->gsi.nServerType == SERVER_AGENT)
			strTemp = "AGENT";	
		else											
			strTemp = "UNKNOWN";
		
		m_ListServer.SetItemText(nCount, 1, strTemp);

		strTemp.Format(_T("Grp %d:Num %d"), gsid->gsi.nServerGroup, gsid->gsi.nServerNumber);
		m_ListServer.SetItemText(nCount, 2, strTemp);

		m_ListServer.SetItemText(nCount, 3, gsid->gsi.szServerIP);

		strTemp.Format(_T("%d"), gsid->gsi.nServicePort);
		m_ListServer.SetItemText(nCount, 4, strTemp);
		
		if (gsid->gsi.nServerCurrentClient <= 0 || gsid->gsi.nServerMaxClient <= 0)
		{
			gsid->gsi.nServerCurrentClient = 0;
			gsid->gsi.nServerMaxClient = 0;
			m_ListServer.SetItemText(nCount, 5, _T("0"));
			m_ListServer.SetItemText(nCount, 6, _T("0 %"));
		}
		else
		{
#ifdef CH_PARAM
			m_ListServer.SetItemText(nCount, 5, _T("0"));
			m_ListServer.SetItemText(nCount, 6, _T("0 %"));
#else
			strTemp.Format(_T("%d/%d"), gsid->gsi.nServerCurrentClient, gsid->gsi.nServerMaxClient);
			m_ListServer.SetItemText(nCount, 5, strTemp);
			strTemp.Format(_T("%d %%"), (int) (gsid->gsi.nServerCurrentClient*100/gsid->gsi.nServerMaxClient));
			m_ListServer.SetItemText(nCount, 6, strTemp);
#endif
		}
	}
	// UpdateData(FALSE);
}

void CServerManagerDlg::UpdateListCtrl(G_SERVER_INFO& gsi)
{

}

void CServerManagerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1) // 서버 정보요청 타이머
	{
		m_pNetClient->SndReqAllSvrInfo();	
	
		CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Execute schduled job"));
		CLIENT_JOB::CScheduler::GetInstance()->Execute(); // 예약된 Job 을 실행한다.
		// 작업관리창 리플래쉬
		if (m_pJobDlg != NULL) m_pJobDlg->Refresh();
	}
	CDialog::OnTimer(nIDEvent);
}

void CServerManagerDlg::OnStopTimer()
{
	CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Kill Timer"));
	KillTimer(1);
}

// 서버리스트를 더블 클릭 했을때...
void CServerManagerDlg::OnNMDblclkListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnContextmenuOpen();

	*pResult = 0;
}

// 서버리스트를 오른쪽 클릭했을때...
void CServerManagerDlg::OnNMRclickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	// 선택된 서버가 있는지 확인한다.
	int nSelected = m_ListServer.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // 선택된 서버가 있음
	{	
		// 마우스 좌표 얻기
		POINT point;
		GetCursorPos(&point);

		// 메뉴 표시
		CMenu menuTemp, *pContextMenu;
		menuTemp.LoadMenu(IDR_SERVER_LIST_MENU);
		pContextMenu = menuTemp.GetSubMenu(0);
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN,		
									point.x,          // 메뉴가 표시될 x 좌표
									point.y,          // 메뉴가 표시될 y 좌표
									AfxGetMainWnd()   // WM_COMMAND 메시지를 받을 윈도우
									);
	}
	*pResult = 0;
}

// 서버목록 오른쪽 마우스 버튼 열기실행
void CServerManagerDlg::OnContextmenuOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	// 선택된 서버가 있는지 확인한다.
	int nSelected = m_ListServer.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (nSelected != -1) // 선택된 서버가 있음
	{
		if (m_DlgList.IsEmpty()) // 서버리스트 비어있음
			return;

		// IP, Port 가져와서 이미 창이 열려있는지 확인한다.		
		CString strIP = m_ListServer.GetItemText(nSelected, 3);
		CString strPort = m_ListServer.GetItemText(nSelected, 4);
		int nPort = atoi(strPort);

		G_SERVER_INFO gsi;
		strcpy(gsi.szServerIP, strIP);
		gsi.nServicePort = nPort;

		// 이미 상태창이 open 되어 있는지 검사
		G_SERVER_INFO_DLG* gsid;
		POSITION pos = m_DlgList.GetHeadPosition();

		for (int i=0; i<m_DlgList.GetCount(); i++)
		{
			gsid = m_DlgList.GetAt(pos);
			if (gsid->gsi == gsi) // 이미리스트에 있음
			{
				if (gsid->pDlg == NULL) // 창이 열려 있지 않으면 오픈
				{
					CServerViewDlg* pDlg = new CServerViewDlg(this);
					gsid->pDlg = pDlg;
				}
				else // 창이 열려 있으면 포커스 가져가기
				{
					gsid->pDlg->SetFocus();
				}
				return;
			}
			m_DlgList.GetNext(pos);
		}
	}
}

void CServerManagerDlg::RemoveDlgList(CServerViewDlg* pDlg)
{
	G_SERVER_INFO_DLG* gsid;
	POSITION pos = m_DlgList.GetHeadPosition();
	for (int i=0; i<m_DlgList.GetCount(); i++)
	{
		gsid = m_DlgList.GetAt(pos);
		if (gsid->pDlg == pDlg) // 창이 열려 있지 않으면 오픈
		{
			gsid->pDlg = NULL;
			return;		
		}		
		m_DlgList.GetNext(pos);
		Sleep( 1 );
	}
}

void CServerManagerDlg::AddDlgList(CServerViewDlg* pDlg)
{
	//m_dlgList.AddHead(pDlg);
}

void CServerManagerDlg::RefreshDlgList()
{
	
}

void CServerManagerDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CServerManagerDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTimer;
	
	if (m_pNetClient->IsOnline() == FALSE)
		return;

	// Stop timer
	OnStopTimer();
	// Get new timer rate
	m_EditRefreshRate.GetWindowText(strTimer);
	// Set new timer
	if (strTimer.IsEmpty() == true)
		SetTimer(1, 60000, 0); // 1 분
	else
		SetTimer(1, atoi(strTimer) * 1000, 0);
}

// 해당 서버에 공지 메시지를 보낸다
void CServerManagerDlg::OnContextmenuMessage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CChatMsgDlg Dlg;
	int ret = (int) Dlg.DoModal();

	// OK = 1
	// CANCEL = 2
	if (ret == 2)	return;
	
	// 10글자 이하면 전송안함
	if (Dlg.m_strMessage.GetLength() < 10) return;
	
	char szMsg[GLOBAL_CHAT_MSG_SIZE+1] = {0};
	StringCchCopy(szMsg, GLOBAL_CHAT_MSG_SIZE+1, Dlg.m_strMessage.GetString());

	// 메시지를 전송한다
	G_SVR_SIMPLE gss;
	// 전송할 서버를 가져온다
	gss = GetSvr();

	// 메시지 구성
	NET_CHAT_CTRL ncc;	
	ncc.nmg.nType    = NET_MSG_CHAT;
	ncc.emType	     = CHAT_TYPE_CTRL_GLOBAL;
	ncc.nServicePort = gss.nServicePort;
	StringCchCopy(ncc.szServerIP, MAX_IP_LENGTH+1, gss.szServerIP);
	StringCchCopy(ncc.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, szMsg);
	// 메시지 전송
	m_pNetClient->Send((char *) &ncc, ncc.nmg.dwSize);
	CClientConsoleMessage::GetInstance()->WriteWithTime(_T("Sended broadcast message"));
}

// 서버리스트에서 선택된 해당서버를 가져온다
G_SVR_SIMPLE CServerManagerDlg::GetSvr()
{
	G_SVR_SIMPLE gss;

	int nSelected = m_ListServer.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	// 3, 4
	CString strTemp1;
	CString strTemp2;
	int nPort;
	strTemp1 = m_ListServer.GetItemText(nSelected, 3);
	strTemp2 = m_ListServer.GetItemText(nSelected, 4);

	nPort = atoi(strTemp2);

	strcpy(gss.szServerIP, strTemp1);
	gss.nServicePort = nPort;

	return gss;
}

void CServerManagerDlg::OnBnClickedBtnIdsearch()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}

void CServerManagerDlg::OnBnClickedBtnJobSchedule()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::vector<int> vecGroupList;
	size_t i;
	for( i = 0; i < m_vecServerList.size(); i++ )
	{
		if (m_vecServerList[i]->gsi.nServerType == NET_SERVER_AGENT)
		{
			vecGroupList.push_back( m_vecServerList[i]->gsi.nServerGroup );
		}
	}

	if (m_pJobDlg == NULL)
	{
		m_pJobDlg = new CJobManagerDlg(this);
		m_pJobDlg->SetGroupList( vecGroupList );
	}else{
		m_pJobDlg->ShowWindow(SW_SHOW || SW_RESTORE);
		m_pJobDlg->SetGroupList( vecGroupList );
	}
}


void CServerManagerDlg::OnEnMaxtextEditConsole()
{	
	GetDlgItem(IDC_EDIT_CONSOLE)->SetWindowText(_T(""));
}

// 서버에 메시지 보내기
void CServerManagerDlg::OnBnClickedBtnMsgSnd()
{
	POSITION pos = NULL;
	G_SERVER_INFO_DLG* gsid = NULL;

	if (!(m_pNetClient != NULL &&m_pNetClient->IsOnline()))
	{
		MessageBox(_T("Offline! Reconnect to server!"));
		return;
	}
     
	UpdateData(TRUE);
	m_strNoticeMsg = "";
	CString strSpace;
	BYTE i;
	char szTempChar[19][64];
	for( i = 0; i < m_SendMsgNum; i++ )
		m_EditMsg[i].GetWindowText( szTempChar[i], 64 );

	for( i = 0; i < m_SendMsgNum; i++ )
	{
		if( i != 0 )
		{
			if( strlen(szTempChar[i]) )
			{
				if( strSpace.GetLength() != 0 )
				{
					m_strNoticeMsg += strSpace;
					m_strNoticeMsg += "\r\n";
					strSpace = "";
				}else{
					m_strNoticeMsg += "\r\n";
				}
			}else{
				strSpace += "\r\n";
			}
		}
		m_strNoticeMsg += szTempChar[i];
	}


	// 10글자 이하면 전송안함
	sprintf( szTempChar[0], "%d", m_strNoticeMsg.GetLength() );
	if (m_strNoticeMsg.GetLength() < 10) 
	{
		MessageBox(_T("Message is too short! Message Must be longer than 10 words"));
		return;
	}


	// 특정서버만 전송
	CButton *pButton = (CButton *)GetDlgItem(IDC_SEND_TYPE1);
	if( pButton->GetCheck() == FALSE )
	{
	
		for( i = 0; i < m_vecServerList.size(); i++ )
		{
			gsid = m_vecServerList[i];
			if (gsid->gsi.nServerType == NET_SERVER_AGENT)
			{
				pButton = (CButton *)GetDlgItem(IDC_SVR_CHECK1+gsid->gsi.nServerGroup-1 );
				if( pButton != NULL && pButton->GetCheck() )
				{
					NET_CHAT_CTRL ncc;		
					ncc.emType		 = CHAT_TYPE_CTRL_GLOBAL;
					ncc.nServicePort = gsid->gsi.nServicePort;
					StringCchCopy(ncc.szServerIP, MAX_IP_LENGTH+1, gsid->gsi.szServerIP);
					StringCchCopy(ncc.szChatMsg,  GLOBAL_CHAT_MSG_SIZE+1, m_strNoticeMsg.GetString());						
					// 메시지 전송
					m_pNetClient->Send((char *) &ncc, ncc.nmg.dwSize);
					CClientConsoleMessage::GetInstance()->WriteWithTime(_T("%s %d Sended broadcast message"), ncc.szServerIP, ncc.nServicePort);
				}
			}
		}
	}
	else // 전체 서버에 전송
	{
		pos = m_DlgList.GetHeadPosition();
		for (int i=0; i<m_DlgList.GetCount(); i++)
		{
			gsid = (G_SERVER_INFO_DLG*) m_DlgList.GetAt(pos);
			// Agent 서버일때만 
			if (gsid->gsi.nServerType == NET_SERVER_AGENT)
			{
				NET_CHAT_CTRL ncc;
				ncc.emType		 = CHAT_TYPE_CTRL_GLOBAL;
				ncc.nServicePort = gsid->gsi.nServicePort;
				StringCchCopy(ncc.szServerIP, MAX_IP_LENGTH+1, gsid->gsi.szServerIP);
				StringCchCopy(ncc.szChatMsg,  GLOBAL_CHAT_MSG_SIZE+1, m_strNoticeMsg.GetString());
				// 메시지 전송
				m_pNetClient->Send((char *) &ncc, ncc.nmg.dwSize);
				CClientConsoleMessage::GetInstance()->WriteWithTime(_T("%s %d Sended broadcast message"), ncc.szServerIP, ncc.nServicePort);
			}
			m_DlgList.GetNext(pos);
		}
	}
    
}	

void CServerManagerDlg::OnJobAdd()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pJobDlg->OnJobAdd();
}

void CServerManagerDlg::OnJobEdit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pJobDlg->OnJobEdit();
}

void CServerManagerDlg::OnJobDelete()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	m_pJobDlg->OnJobDelete();
}



void CServerManagerDlg::OnBnClickedView1Button()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 19; i++ )
	{
		if( i == 0 )
		{
			m_EditMsg[i].ShowWindow(SW_SHOW);
		}else{
			m_EditMsg[i].ShowWindow(SW_HIDE);
		}
	}

	RECT rect;
	SetRect( &rect, 15, 174, 647, 510 );
	m_ListServer.MoveWindow( &rect, TRUE );
	SetRect( &rect, 15, 515, 647, 680 );
	m_EditConsole.MoveWindow( &rect, TRUE );
	m_EditConsole.ShowWindow( SW_SHOW );

	m_SendMsgNum = 1;

	m_TracingList.ShowWindow(SW_HIDE);
	m_EditTracingID.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_TYPE_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_HIDE);

	CButton *pButton = (CButton *)GetDlgItem(IDC_TRACING_BUTTON);
	pButton->SetCheck(FALSE);
}


void CServerManagerDlg::OnBnClickedView10Button()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 19; i++ )
	{
		if( i < 10 )
		{
			m_EditMsg[i].ShowWindow(SW_SHOW);
		}else{
			m_EditMsg[i].ShowWindow(SW_HIDE);
		}
	}

	RECT rect;
	SetRect( &rect, 15, 374, 647, 550 );
	m_ListServer.MoveWindow( &rect, TRUE );
	SetRect( &rect, 15, 555, 647, 680 );
	m_EditConsole.MoveWindow( &rect, TRUE );
	m_EditConsole.ShowWindow( SW_SHOW );

	m_SendMsgNum = 10;

	m_TracingList.ShowWindow(SW_HIDE);
	m_EditTracingID.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_TYPE_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_HIDE);

	CButton *pButton = (CButton *)GetDlgItem(IDC_TRACING_BUTTON);
	pButton->SetCheck(FALSE);
}

void CServerManagerDlg::OnBnClickedView20Button()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 19; i++ )
	{
		m_EditMsg[i].ShowWindow(SW_SHOW);
	}

	RECT rect;
	SetRect( &rect, 15, 580, 647, 680 );
	m_ListServer.MoveWindow( &rect, TRUE );
	/*SetRect( &rect, 15, 635, 647, 680 );
	m_EditConsole.MoveWindow( &rect, TRUE );*/
	m_EditConsole.ShowWindow( SW_HIDE );

	m_SendMsgNum = 19;

	m_TracingList.ShowWindow(SW_HIDE);
	m_EditTracingID.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_TYPE_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADD_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_HIDE);

	CButton *pButton = (CButton *)GetDlgItem(IDC_TRACING_BUTTON);
	pButton->SetCheck(FALSE);

}

void CServerManagerDlg::OnBnClickedAddTypeCheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *)GetDlgItem(IDC_ADD_TYPE_CHECK);
	if( pButton->GetCheck() == TRUE )
	{
		SetDlgItemText( IDC_ADD_TYPE_CHECK, "ID" );
	}else{
		SetDlgItemText( IDC_ADD_TYPE_CHECK, "UserNum" );
	}
}


void CServerManagerDlg::OnBnClickedTracingButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton *pButton = (CButton *)GetDlgItem(IDC_TRACING_BUTTON);
	if( pButton->GetCheck() == TRUE )
	{
		OnBnClickedView1Button();

		pButton->SetCheck(TRUE);

		m_TracingList.ShowWindow(SW_SHOW);
		m_EditTracingID.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ADD_TYPE_CHECK)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ADD_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_SHOW);
		RECT rect;
		SetRect( &rect, 15, 174, 285, 510 );
		m_ListServer.MoveWindow( &rect, TRUE );
	}else{	
		OnBnClickedView1Button();
	}

}

void CServerManagerDlg::UpdateTracingList()
{
	m_TracingList.DeleteAllItems();

	std::vector<STracingData>::iterator iter;
	int nCount = 0;
	CString strTemp;
	for( iter = m_vecTracingData.begin(); iter != m_vecTracingData.end(); ++iter )
	{
		STracingData data = *iter;

		LV_ITEM lvItem;		
		ZeroMemory(&lvItem, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // 이미지 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		if( data.strAccount.size() == 0 )
		{
			lvItem.pszText = "Unknown";
		}else{
			lvItem.pszText = (LPSTR)data.strAccount.c_str();
		}
		m_TracingList.InsertItem(&lvItem);

		if( data.dwUserNum == 0 )
		{
			strTemp = "Unknown";
		}else{
			strTemp.Format( "%d", data.dwUserNum );
		}

		m_TracingList.SetItemText( nCount, 1, strTemp );


		if( data.dwOnLine == 1 )
		{
			m_TracingList.SetItemText( nCount, 2, "OnLine" );
		}else{
			m_TracingList.SetItemText( nCount, 2, "OffLine" );
		}

		if( data.nConnectServer != -1 )
		{
			strTemp.Format( "%d", data.nConnectServer );
		}else{
			strTemp = "Unknown";
		}
		m_TracingList.SetItemText( nCount, 3, strTemp );

		nCount++;
	}

}

void CServerManagerDlg::SendTracingList()
{
	NET_UPDATE_TRACINGCHAR_ALL msg;	

	size_t i;
	for( i = 0; i < m_vecTracingData.size(); i++ )
	{
		msg.tracingData[i] = m_vecTracingData[i];
	}
	// 메시지 전송
	m_pNetClient->Send((char *) &msg, msg.nmg.dwSize);
}


void CServerManagerDlg::OnBnClickedAddButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szTempChar[128] = {0,};
	m_EditTracingID.GetWindowText( szTempChar, 128 );


	CButton *pButton = (CButton *)GetDlgItem(IDC_ADD_TYPE_CHECK);

	if( m_vecTracingData.size() >= MAX_TRACING_NUM ) 
	{
		MessageBox( "Too many Tracing list..", "Error" );
		return;
	}

	std::vector<STracingData>::iterator iter;
	for( iter = m_vecTracingData.begin(); iter != m_vecTracingData.end(); ++iter )
	{
		STracingData data = *iter;
		if( (pButton->GetCheck() == TRUE && data.strAccount == szTempChar) ||
			(pButton->GetCheck() == FALSE && data.dwUserNum  == atoi(szTempChar)) )
		{
			MessageBox( "Already add character", "Info" );
			return;
		}
	}

	STracingData data;
	if( pButton->GetCheck() == FALSE )
	{
		data.dwUserNum = atoi(szTempChar);
	}else{
		data.strAccount = szTempChar;
	}
	data.dwOnLine = FALSE;
	m_vecTracingData.push_back( data);
	UpdateTracingList();

	SendTracingList();
	
}

void CServerManagerDlg::OnBnClickedDelButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelected = m_TracingList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	char szTempChar[256];
	sprintf( szTempChar, "%d", nSelected );
	if( nSelected == -1 ) return;
	m_vecTracingData.erase( m_vecTracingData.begin() + nSelected );
	UpdateTracingList();

	SendTracingList();
}


void CServerManagerDlg::MsgUpdateTracingChar(NET_MSG_GENERIC* pMsg, bool bAll)
{
	if( pMsg == NULL ) return;

	if( bAll )
	{
		m_vecTracingData.clear();
		NET_UPDATE_TRACINGCHAR_ALL* msg = reinterpret_cast <NET_UPDATE_TRACINGCHAR_ALL*> (pMsg);
		BYTE i;
		bool bOnlineUser = FALSE;
		for( i = 0; i < MAX_TRACING_NUM; i++ )
		{
			if( msg->tracingData[i].dwUserNum == 0 && msg->tracingData[i].strAccount.size() == 0 ) break;
			if( msg->tracingData[i].dwOnLine == TRUE ) bOnlineUser = TRUE;
			m_vecTracingData.push_back(msg->tracingData[i]);
		}
		if( bOnlineUser )
			PlayTracingUserLogOnOff();

		m_bFirstRcvTracing = FALSE;
	}else{
		NET_UPDATE_TRACINGCHAR* msg = reinterpret_cast <NET_UPDATE_TRACINGCHAR*> (pMsg);
		m_vecTracingData[msg->updateNum] = msg->tracingData;
		PlayTracingUserLogOnOff();
	}

	
	UpdateTracingList();


	
}

void CServerManagerDlg::OnBnClickedBtnMsgClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE i;
	for( i = 0; i < 19; i++ )
	{
		m_EditMsg[i].SetWindowText( "" );
	}
}


