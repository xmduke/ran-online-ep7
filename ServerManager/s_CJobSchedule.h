#pragma once

#include <list>
#include "s_CNetCtrl.h"		 

namespace CLIENT_JOB
{
	static CNetCtrl* pNetCtrl;

	//		
	//      JOB_LOOP=3,  // Job execute forever
	enum JOB_TYPE 
	{
		JOB_ONCE=0, // Job execute once
		JOB_HOUR=1, // Job execute every hour
		JOB_DAILY=2, // Job execute every daily
		JOB_SUN=3, 	 // Job execute every Sunday
		JOB_MON=4, 	 // Job execute every Monday
		JOB_TUE=5, 	 // Job execute every Tuesday
		JOB_WED=6, 	 // Job execute every Wednesday
		JOB_THU=7, 	 // Job execute every Thursday
		JOB_FRI=8, 	 // Job execute every Friday
		JOB_SAT=9, 	 // Job execute every Saturday
		JOB_SPECIALDAY=10 // Job execute special day
	};

	//////////////////////////////////////////////////////////////////////////////
	class CAction
	{
	protected:
		int m_nHour; // execute hour, default 0
		int m_nMin;  // execute miniute, default 0
		int m_nType; // execute times, default 0 (JOB_ONCE)        
		int m_nSvrGrp;
		int m_nSvrNum;
		int m_nSvrField;
		int m_nSvrType;
		int m_nCmdType;
	public :         
		CAction(int nHour=0, 
				int nMin=0, 
				int nType=JOB_ONCE,
				int nSvrGrp=0, 
				int nSvrNum=0, 
				int nSvrField=0, 
				int nSvrType=0, 
				int nCmdType=0);
		virtual ~CAction() {};
		virtual void Execute();
		virtual CString GetDescription() { CString strTemp("Action"); return strTemp; };
		int GetHour() { return m_nHour; };
		int GetMin()  { return m_nMin; };
		int GetType() { return m_nType; };		
	};

	//////////////////////////////////////////////////////////////////////////////
	class CServerStop : public CAction
	{
	public :
		CServerStop(int nHour=0, 
					int nMin=0, 
					int nType=JOB_ONCE,
					int nSvrGrp=0, 
					int nSvrNum=0, 
					int nSvrField=0, 
					int nSvrType=0, 
					int nCmdType=0);
		virtual ~CServerStop() {};
		virtual void Execute();
		virtual CString GetDescription();
	};

	//////////////////////////////////////////////////////////////////////////////
	class CBroadCast : public CAction
	{
	protected:
		CString m_strMsg;
		CString m_strSvrIP;
		int		m_nPort;
		int		m_nDayOfWeek; // 요일
		int		m_nYear;	  // 년도
		int		m_nMonth;	  // 달
		int		m_nDay;		  // 날짜
		std::vector<int> m_vecGroupNum; // Group을 체크가 가능하므로 벡터에 넣어서 관리한다
		std::vector<int> m_vecGroupList;

	public:
		CBroadCast();
		void SetValue( 
			std::vector<int> vecGroupList,
			CString strMsg,
			int nType,
			std::vector<int> vecGroupNum,
			int nHour=0, 
			int nMin=0,
			int nDayOfWeek=0, 
			int	nMonth=0,
			int	nDay=0,
			int	nYear=0,
			int nSvrField=0,
			int nSvrType=0,
			int nCmdType=0 );
		virtual ~CBroadCast();
		virtual void Execute();
		virtual CString GetDescription();

		int	GetDayOfWeek() { return	m_nDayOfWeek; }// 요일
		int	GetYear()	   { return	m_nYear; }	  // 년도
		int	GetMonth()	   { return	m_nMonth; }	  // 달
		int	GetDay()	   { return	m_nDay;	}	  // 날짜
		CString GetStr()   { return m_strMsg; }   // 보내는 메시지
		std::vector<int> GetGroupNum() { return m_vecGroupNum; } // 보내는 Group List
		std::vector<int> GetGroupList() { return m_vecGroupList; } // 보내는 Group List


	};

	//////////////////////////////////////////////////////////////////////////////
	class CScheduler
	{
	protected:

	public:
		static CScheduler* GetInstance();
		static void	ReleaseInstance();
		void SetNetCtrl(CNetCtrl* pTemp);
		
		void AddBrocastJob(CBroadCast t);
		void EraseEndedJob(int nHour, int nMin);
		std::vector<CBroadCast> GetJobList();
		void SetJobList( std::vector<CBroadCast> vecJobList ) { m_vecBrocastJob = vecJobList; }
		void ClearJobList() { m_vecBrocastJob.clear(); }
		void Execute();
	private:
		CScheduler();		
		~CScheduler();

		static CScheduler* SelfInstance;
		std::vector<CBroadCast> m_vecBrocastJob;
	};
}