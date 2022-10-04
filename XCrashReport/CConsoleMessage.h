///////////////////////////////////////////////////////////////////////////////
// s_CConsoleMessage.h
//
// class CConsoleMessage
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCONSOLEMESSAGE_H_
#define S_CCONSOLEMESSAGE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define C_MSG_DEFAULT		1
#define C_MSG_CONSOLE		1
#define C_MSG_FILE			2
#define C_MSG_FILE_CONSOLE	3
#define C_MSG_DB			4
#define C_MSG_DB_FILE		5
#define C_MSG_DB_CONSOLE	6
#define C_MSG_ALL			7

#define C_MSG_NORMAL		1
#define C_MSG_INFO			2
#define C_MSG_ERROR			3

// define
#define C_MAX_QUEUE_SIZE	100
#define C_BUFFER_SIZE		2048

class CConsoleMessage 
{
public:
	static CConsoleMessage* GetInstance();
	static void	ReleaseInstance();
	
	virtual void Write		(const char* msg, ...);
	virtual void Write		(int nType, const char* msg, ...);	
	virtual void Write		(int nType, int nPositon, const char* msg, ...);	

	void UpdateEditControl(char* msgbuf);	
	void SetControl(HWND hControl);
	HWND GetControl(void);

protected:		
	HWND m_pEditBox;
	int	 m_nDefaultType;

private:
	CConsoleMessage(int nType = C_MSG_DEFAULT);
	virtual ~CConsoleMessage();

	static CConsoleMessage* SelfInstance;
};

#endif // S_CCONSOLEMESSAGE_H_