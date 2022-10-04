#pragma once

#include <list>

namespace CLIENT_JOB
{
	enum JOB_TYPE 
	{
		JOB_ONCE=0, // Job execute once
		JOB_HOUR=1, // Job execute every hour
		JOB_DAILY=2 // Job execute every daily
	};

	//////////////////////////////////////////////////////////////////////////////
	class CAction
	{
	protected:
		int m_nHour; // execute hour, default 0
		int m_nMin;  // execute miniute, default 0
		int m_nType; // execute times, default 1
	public :         
		CAction(int nHour=0, int nMin=0, int nType=JOB_ONCE);
		virtual ~CAction() {};
		virtual void Execute();
		int GetHour() { return m_nHour; };
		int GetMin()  { return m_nMin; };
		int GetType() { return m_nType; };
	};

	//////////////////////////////////////////////////////////////////////////////
	class CServerStop : public CAction
	{
	public :
		CServerStop(int nHour=0, int nMin=0, int nType=JOB_ONCE);
		virtual ~CServerStop() {};
		virtual void Execute();
	};

	//////////////////////////////////////////////////////////////////////////////
	class CBroadCast : public CAction
	{
	protected:
		CString* m_pMsg;
	public:
		CBroadCast(CString* strTemp, int nHour=0, int nMin=0, int nType=JOB_ONCE);

		virtual ~CBroadCast();
		virtual void Execute();
	};

	//////////////////////////////////////////////////////////////////////////////
	class CScheduler
	{
	protected:
		std::list<CAction*> m_vJob;
		void CheckExecute(int nHour, int nMin, CAction* t);

	public:
		static CScheduler* GetInstance();
		static void	ReleaseInstance();
		
		void AddJob(CAction* t);
		void EraseEndedJob(int nHour, int nMin);
		void EraseJob(CAction* t);
		void Execute();
	private:
		CScheduler();
		~CScheduler();

		static CScheduler* SelfInstance;
	};
}