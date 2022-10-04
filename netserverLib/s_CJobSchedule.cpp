#include "StdAfx.h"
#include "s_CJobSchedule.h"
#include <ctime>

using namespace CLIENT_JOB;

///////////////////////////////////////////////////////////////////////////////
CScheduler* CScheduler::SelfInstance = NULL;

CScheduler::CScheduler()
{
}

CScheduler::~CScheduler()
{
	std::list<CAction*>::const_iterator pos;
	for (pos = m_vJob.begin(); pos != m_vJob.end(); ++pos)
	{	
		CAction* pTemp = static_cast <CAction*> (*pos);
		delete pTemp;
	}
}

CScheduler* CScheduler::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CScheduler();

	return SelfInstance;
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
void CScheduler::AddJob(CAction* t)
{
	if (t)	m_vJob.push_back(t);
}

// 전체 Job 중에 현재 실행해야 할 Job 을 실행하고
// 실행이 끝난후 지워야 할 Job 을 제거한다.
void CScheduler::Execute()
{
	time_t curr;
	tm local;
	time(&curr); // get current time_t value
	local=*(localtime(&curr));

	int nHour = local.tm_hour;
	int nMin = local.tm_min;
	 
	// CTime theTime = CTime::GetCurrentTime();		
	// int nHour = theTime.GetHour();
	// int nMin = theTime.GetMinute();
	// int nType = 0;

	// 현재 실행해야할 Job 이면 실행한다.
	std::list<CAction*>::const_iterator pos;
	for (pos = m_vJob.begin(); pos != m_vJob.end(); ++pos)
	{	
		CAction* pTemp = static_cast <CAction*> (*pos);
		if (pTemp != NULL)
			CheckExecute(nHour, nMin, pTemp);
	}

	// 실행이 끝난 Job 은 삭제한다.
	EraseEndedJob(nHour, nMin);	
}

// 실행이 끝난 Job 을 찾아서 삭제한다.
void CScheduler::EraseEndedJob(int nHour, int nMin)
{
	std::list<CAction*>::iterator pos;
	for (pos = m_vJob.begin(); pos != m_vJob.end(); ++pos)
	{	
		CAction* pTemp = static_cast <CAction*> (*pos);
		if (pTemp != NULL)
		{
			if (pTemp->GetType() == JOB_ONCE)
			{
				if ((nHour == pTemp->GetHour()) && (nMin == pTemp->GetMin()))
				{
					m_vJob.erase(pos);
					delete pTemp;
				}
			}
		}
	}
}

// 특정 Job 을 삭제한다
void CScheduler::EraseJob(CAction* t)
{
	m_vJob.remove(t);
}

// 전체 Job 중에 현재 실행해야할 Job 을 찾아 실행한다
void CScheduler::CheckExecute(int nHour, int nMin, CAction* t)
{	
	int nType = t->GetType();
	switch (nType)
	{
	case JOB_ONCE:
	case JOB_DAILY:
		// 실행할 시간이 정확하면 실행한다.
		if ((nHour == t->GetHour()) && (nMin == t->GetMin()))
			t->Execute();
		break;
	case JOB_HOUR:
		if (nHour == t->GetHour())
			t->Execute();
		break;
	default:
		break;	
	}
}

///////////////////////////////////////////////////////////////////////////////
CAction::CAction(int nHour, int nMin, int nType) :
	m_nHour(0),
	m_nMin(0),
	m_nType(0)
{
	m_nHour = nHour;
	m_nMin  = nMin;
	m_nType = nType;
}

void CAction::Execute() 
{ 
	printf("CAction Execute \n"); 
}

///////////////////////////////////////////////////////////////////////////////
CServerStop::CServerStop(int nHour, int nMin, int nType) : 
	CAction(nHour, nMin, nType)
{
}

void CServerStop::Execute() 
{ 
	printf("ServerStop Execute \n"); 
}

///////////////////////////////////////////////////////////////////////////////
CBroadCast::CBroadCast(CString* strTemp, int nHour, int nMin, int nType) :
	CAction(nHour, nMin, nType)
{ 
	m_pMsg = strTemp; 
}
CBroadCast::~CBroadCast() 
{ 
	delete m_pMsg; 
}

void CBroadCast::Execute() 
{ 
	printf("BroadCast Execute %s \n", m_pMsg->GetBuffer()); 
}