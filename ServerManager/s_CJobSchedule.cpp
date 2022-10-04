#include "StdAfx.h"
#include <ctime>

#include "s_CJobSchedule.h"
#include "s_CClientConsoleMsg.h"
#include "../netclientLib/s_NetGlobal.h"

using namespace CLIENT_JOB;

///////////////////////////////////////////////////////////////////////////////
CScheduler* CScheduler::SelfInstance = NULL;

CScheduler::CScheduler()
{
}

CScheduler::~CScheduler()
{
	/*std::list<CAction*>::const_iterator pos;
	for (pos = m_vJob.begin(); pos != m_vJob.end(); ++pos)
	{	
		CAction* pTemp = static_cast <CAction*> (*pos);
		delete pTemp;
	}*/
}

CScheduler* CScheduler::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CScheduler();

	return SelfInstance;
}

std::vector<CBroadCast> CScheduler::GetJobList()
{ 
	return m_vecBrocastJob; 
};

void CScheduler::SetNetCtrl(CNetCtrl* pTemp)
{
	pNetCtrl = pTemp;
}

void CScheduler::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

// 새로운 Job 을 등록한다.
void CScheduler::AddBrocastJob(CBroadCast t)
{
	m_vecBrocastJob.push_back(t);
}

// 전체 Job 중에 현재 실행해야 할 Job 을 실행하고
// 실행이 끝난후 지워야 할 Job 을 제거한다.
void CScheduler::Execute()
{

	if( m_vecBrocastJob.size() == 0 ) return;
	CTime crtTime = CTime::GetCurrentTime();

	int nHour	   = crtTime.GetHour();
	int nMin	   = crtTime.GetMinute();
	int nDayOfWeek = crtTime.GetDayOfWeek();
	int nDay	   = crtTime.GetDay();
	int nMonth	   = crtTime.GetMonth();
	int nYear	   = crtTime.GetYear();


	 
	// CTime theTime = CTime::GetCurrentTime();		
	// int nHour = theTime.GetHour();
	// int nMin = theTime.GetMinute();
	// int nType = 0;

	// 현재 실행해야할 Job 이면 실행한다.
	size_t i;
	for( i = 0; i < m_vecBrocastJob.size(); i++ )
	{	
		
		// 전체 Job 중에 현재 실행해야할 Job 을 찾아 실행한다
		int nType = m_vecBrocastJob[i].GetType();
		switch (nType)
		{
		case JOB_ONCE:
		case JOB_DAILY:
			// 실행할 시간이 정확하면 실행한다.
			if ((nHour == m_vecBrocastJob[i].GetHour()) && (nMin == m_vecBrocastJob[i].GetMin()))
				m_vecBrocastJob[i].Execute();
			break;
		case JOB_HOUR:
			if (nMin == m_vecBrocastJob[i].GetMin())
				m_vecBrocastJob[i].Execute();
			break;
		case JOB_SUN: 	 // Job execute every Sunday
		case JOB_MON: 	 // Job execute every Monday
		case JOB_TUE: 	 // Job execute every Tuesday
		case JOB_WED: 	 // Job execute every Wednesday
		case JOB_THU: 	 // Job execute every Thursday
		case JOB_FRI: 	 // Job execute every Friday
		case JOB_SAT: 	 // Job execute every Saturday
			if ((nHour == m_vecBrocastJob[i].GetHour()) && (nMin == m_vecBrocastJob[i].GetMin()) && (nDayOfWeek == m_vecBrocastJob[i].GetDayOfWeek()) )
				m_vecBrocastJob[i].Execute();		
			break;
		case JOB_SPECIALDAY:
			if ( (nYear == m_vecBrocastJob[i].GetYear()) && (nMonth == m_vecBrocastJob[i].GetMonth()) && (nDay == m_vecBrocastJob[i].GetDay()) &&
				 (nHour == m_vecBrocastJob[i].GetHour()) && (nMin == m_vecBrocastJob[i].GetMin()) )
				m_vecBrocastJob[i].Execute();		
			break;

		}
	}

	// 실행이 끝난 Job 은 삭제한다.
	EraseEndedJob(nHour, nMin);	
}

// 실행이 끝난 Job 을 찾아서 삭제한다.
void CScheduler::EraseEndedJob(int nHour, int nMin)
{
	CAction temp;
	size_t i;
	for( i = 0; i < m_vecBrocastJob.size(); i++ )
	{	
		temp = m_vecBrocastJob[i];
		if (temp.GetType() == JOB_ONCE)
		{
			if ((nHour == temp.GetHour()) && (nMin == temp.GetMin()))
			{
				m_vecBrocastJob.erase( m_vecBrocastJob.begin() + i );
				i--;
			}
		}
	}
}






///////////////////////////////////////////////////////////////////////////////
CAction::CAction(int nSvrGrp,
				int nSvrNum,
				int nSvrField, 
				int nSvrType, 
				int nCmdType,
				int nHour, 
				int nMin, 
				int nType)
{
	m_nSvrGrp	= nSvrGrp;
	m_nSvrNum	= nSvrNum;
	m_nSvrField = nSvrField;
	m_nSvrType	= nSvrType;
	m_nCmdType	= nCmdType;
	m_nHour		= nHour;
	m_nMin		= nMin;
	m_nType		= nType;
}

void CAction::Execute() 
{ 
	// printf(_("CAction Execute \n")); 
}

///////////////////////////////////////////////////////////////////////////////
CServerStop::CServerStop(int nHour, 
						int nMin, 
						int nType,
						int nSvrGrp, 
						int nSvrNum, 
						int nSvrField, 
						int nSvrType, 
						int nCmdType)
{
	m_nSvrGrp = nSvrGrp;
	m_nSvrNum = nSvrNum;
	m_nSvrField = nSvrField;
	m_nSvrType = nSvrType;
	m_nCmdType = nCmdType;
	m_nHour = nHour;
	m_nMin = nMin;
	m_nType = nType;
}

void CServerStop::Execute() 
{ 
    if (pNetCtrl == NULL) return;

	pNetCtrl->SndSvrCmd(m_nSvrGrp, m_nSvrNum, m_nSvrField, m_nSvrType, m_nCmdType);
}

CString CServerStop::GetDescription() 
{
	CString strTemp;
	strTemp.Format(_T("Server stop at %d:%d (%d,%d,%d,%d,%d)"),
					m_nHour, 
					m_nMin,
					m_nSvrGrp, 
					m_nSvrNum, 
					m_nSvrField, 
					m_nSvrType,					
					m_nCmdType);
	return strTemp;
}

///////////////////////////////////////////////////////////////////////////////
CBroadCast::CBroadCast()
				
{ 
}


CBroadCast::~CBroadCast() 
{ 	
}

void CBroadCast::SetValue( std::vector<int> vecGroupList, CString strMsg, int nType, std::vector<int> vecGroupNum, int nHour/* =0 */, int nMin/* =0 */, int nDayOfWeek/* =0 */, 
						   int nMonth/* =0 */, int nDay/* =0 */, int nYear/* =0 */, int nSvrField/* =0 */, int nSvrType/* =0 */, int nCmdType/* =0 */ )
{
    m_vecGroupList = vecGroupList;
	m_strMsg	  = strMsg;
	m_nHour		  = nHour;
	m_nMin		  = nMin;
	m_nType		  = nType;
	m_vecGroupNum = vecGroupNum;
	m_nSvrField = nSvrField;
	m_nSvrType	= nSvrType;
	m_nCmdType	= nCmdType;
	m_nDayOfWeek = nDayOfWeek; // 요일
	m_nYear		 = nYear;	   // 년도
	m_nMonth	 = nMonth;	   // 달
	m_nDay		 = nDay;	   // 날짜

}

void CBroadCast::Execute() 
{ 
	if (pNetCtrl == NULL) return;

	// 10글자 이하면 전송안함
//	if (m_strMsg.GetLength() < 10) return;

	//if (m_nPort > 0)
	//{	
	//	// 메시지 구성
	//	NET_CHAT_CTRL ncc;		
	//	ncc.emType			= CHAT_TYPE_GLOBAL;
	//	ncc.nServicePort	= m_nPort;
	//	strcpy(ncc.szServerIP, m_strSvrIP);
	//	strcpy(ncc.szChatMsg, m_strMsg.GetString());
	//	// 메시지 전송
	//	CLIENT_JOB::pNetCtrl->Send((char *) &ncc, ncc.nmg.dwSize);
	//}
	//else
	{
		BYTE i;
		if( m_vecGroupNum[0] == -1 )
		{
			for( i = 0; i < m_vecGroupList.size(); i++ )
			{
				NET_CHAT_CTRL2 ncc2;		
				ncc2.emType		= CHAT_TYPE_CTRL_GLOBAL2;
				ncc2.nSvrGrp	= m_vecGroupList[i];
				StringCchCopy(ncc2.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, m_strMsg.GetString());
				// 메시지 전송
				CLIENT_JOB::pNetCtrl->Send((char *) &ncc2, ncc2.nmg.dwSize);
			}
		}else{
			for( i = 0; i < m_vecGroupNum.size(); i++ )
			{
				NET_CHAT_CTRL2 ncc2;		
				ncc2.emType		= CHAT_TYPE_CTRL_GLOBAL2;
				ncc2.nSvrGrp	= m_vecGroupNum[i]+1;
				StringCchCopy(ncc2.szChatMsg, GLOBAL_CHAT_MSG_SIZE+1, m_strMsg.GetString());
				// 메시지 전송
				CLIENT_JOB::pNetCtrl->Send((char *) &ncc2, ncc2.nmg.dwSize);
			}
		}
	}

	CClientConsoleMessage::GetInstance()->Write("Send broadcast message");	
}

CString CBroadCast::GetDescription()
{
	CString strTemp;
	CString	strTemp2;
	strTemp.Format( _T("message \"%s\""),m_strMsg.GetString() );
	if( m_vecGroupNum[0] == -1 )
	{
        strTemp2.Format( _T("( Send All Server )") );
		strTemp += strTemp2;
	}else{
		size_t i;
		strTemp2.Format( _T("( Send "  ) );
		strTemp += strTemp2;
		for( i = 0; i < m_vecGroupNum.size(); i++ )
		{
			strTemp2.Format( _T("%d:" ), m_vecGroupNum[i]+1 );
			strTemp += strTemp2;
		}
		strTemp2.Format( _T(" Server )"  ) );
		strTemp += strTemp2;
	}


	return strTemp;
}