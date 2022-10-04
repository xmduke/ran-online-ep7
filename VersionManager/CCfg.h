///////////////////////////////////////////////////////////////////////////////
// s_CCfg.h
//
// class CCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CCFG_H_
#define CCFG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class CCfg
{
protected:
	
public:
	static CCfg* GetInstance();
	static void	ReleaseInstance();

	void		SetDefault(void);
	int			Process(char* sLine);
	int			Load(const char* filename);	
	CString		GetAppPath();

private:
	CCfg();
	CCfg(const char* filename);
	virtual ~CCfg();

	static CCfg* SelfInstance;
public:
	char m_szOdbcName[100];
	char m_szOdbcDbName[100];
	char m_szOdbcUserName[100];
	char m_szOdbcPassword[100];
	
	std::vector<CString> m_vFtpIP;
	// char m_szFtpIP[100];

	int  m_nFtpPort;
	char m_szFtpUserName[100];
	char m_szFtpPassword[100];
	char m_szTargetPath[256];

};

#endif // CCFG_H_