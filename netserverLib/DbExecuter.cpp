#include "stdafx.h"
#include "./s_CDbAction.h"
#include "./DbExecuter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//! ---------------------------------------------------------------------------
//! class CDbExecuter
CDbExecuter::CDbExecuter()
	: m_pServer(NULL)
{
}

CDbExecuter::CDbExecuter(
	CServer* pServer )
{
	m_pServer = pServer;
}

CDbExecuter* CDbExecuter::GetInstance()
{
	static CDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CDbExecuter::ReleaseInstance()
{
}

void CDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CDbExecuter::AddJob(
	CDbAction* t )
{
	LockOn();
    m_vJob.push(t);
	LockOff();
	runThread();
}

void CDbExecuter::process()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{

		/*for(size_t i=0; i< m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}*/
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute( m_pServer );
		delete pTemp;
	}
}

void CDbExecuter::endThread()
{
	ExecuteJobLast();
	EventThread::endThread();
}

void CDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		for(i=0; i<(int) m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}

//! ---------------------------------------------------------------------------
//! CLogDbExecuter
CLogDbExecuter::CLogDbExecuter() :
	m_pServer(NULL)
{
}

CLogDbExecuter::CLogDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CLogDbExecuter* CLogDbExecuter::GetInstance()
{
	static CLogDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CLogDbExecuter::ReleaseInstance()
{
}

void CLogDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CLogDbExecuter::endThread()
{
	ExecuteJobLast();
	EventThread::endThread();
}

void CLogDbExecuter::AddJob(
	CDbAction* t )
{
	LockOn();
    m_vJob.push(t);	
	LockOff();
	runThread();
}

void CLogDbExecuter::process()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
	/*	for(size_t i=0; i< m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}*/
	

		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();		
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CLogDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();		
		return;
	}
	else
	{
		for(i=0; i<(int) m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}

//! ---------------------------------------------------------------------------
//! CUserDbExecuter
CUserDbExecuter::CUserDbExecuter()
	: m_pServer(NULL)
{
}

CUserDbExecuter::CUserDbExecuter(CServer* pServer)
{
	m_pServer = pServer;
}

CUserDbExecuter* CUserDbExecuter::GetInstance()
{
	static CUserDbExecuter cSelfInstance;
	return &cSelfInstance;
}

void CUserDbExecuter::ReleaseInstance()
{
}

void CUserDbExecuter::SetServer(CServer* pServer)
{
	m_pServer = pServer;
}

void CUserDbExecuter::endThread()
{
	ExecuteJobLast();
	EventThread::endThread();
}

void CUserDbExecuter::AddJob(
	CDbAction* t )
{
	LockOn();
    m_vJob.push(t);	
	LockOff();
	runThread();
}

void CUserDbExecuter::process()
{
	CDbAction* pTemp = NULL;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		/*for( size_t i=0; i< m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}*/
		pTemp = (CDbAction*) m_vJob.front();
		m_vJob.pop();
		LockOff();
	}

	if (pTemp)
	{
		pTemp->Execute(m_pServer);
		delete pTemp;
	}
}

void CUserDbExecuter::ExecuteJobLast()
{
	CDbAction* pTemp = NULL;
	int i = 0;

	LockOn();
	if (m_vJob.empty())
	{
		LockOff();
		return;
	}
	else
	{
		for(i=0; i<(int) m_vJob.size(); ++i)
		{
			pTemp = (CDbAction*) m_vJob.front();
			if (pTemp)
			{
				pTemp->Execute(m_pServer);
				delete pTemp;
			}
			m_vJob.pop();
			pTemp = NULL;
		}
		LockOff();
	}
}