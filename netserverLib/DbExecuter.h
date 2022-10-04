#ifndef _DB_EXECUTER_H_
#define _DB_EXECUTER_H_

#pragma once

#include <queue>
#include "./EventThread.h"

class CServer;
class CDbAction;

//! class CDbExecuter
class CDbExecuter : public EventThread
{
public:
	static CDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CDbExecuter();
	CDbExecuter( CServer* pServer );
	virtual ~CDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function
	virtual void process();
	virtual void endThread();

public:
	void SetServer( CServer* pServer );
	void AddJob( CDbAction* t );
	//void ExecuteJob();
	void ExecuteJobLast();
};

class CLogDbExecuter : public EventThread
{
public:
	static CLogDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CLogDbExecuter();
	CLogDbExecuter(CServer* pServer);
	virtual ~CLogDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function
	virtual void process();
	virtual void endThread();

public:
	void SetServer(CServer* pServer);
	void AddJob(CDbAction* t);
	//void ExecuteJob();
	void ExecuteJobLast();
};

class CUserDbExecuter : public EventThread
{
public:
	static CUserDbExecuter* GetInstance();
	static void ReleaseInstance();

private:
	CUserDbExecuter();
	CUserDbExecuter(CServer* pServer);
	virtual ~CUserDbExecuter() {};

protected:
	std::queue<CDbAction*> m_vJob;
	CServer* m_pServer;

public:
	//! -----------------------------------------------------------------------
	//! Virtual Function
	virtual void process();
	virtual void endThread();

public:
	void SetServer( CServer* pServer );
	void AddJob( CDbAction* t );
	//void ExecuteJob();
	void ExecuteJobLast();
};

#endif // _DB_EXECUTER_H_