///////////////////////////////////////////////////////////////////////////////
// CConsoleMessage.cpp
//
// class CConsoleMessage
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include "CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConsoleMessage* CConsoleMessage::SelfInstance = NULL;

CConsoleMessage::CConsoleMessage(int nType)
{	
	m_pEditBox = NULL;	
}

CConsoleMessage::~CConsoleMessage()
{	
}

CConsoleMessage* CConsoleMessage::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CConsoleMessage();

	return SelfInstance;
}

void CConsoleMessage::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CConsoleMessage::SetControl(HWND hControl)
{
	m_pEditBox = hControl;	
	// only accept n k of text
	// n * 1024 = n k
	SendMessage(m_pEditBox,     EM_LIMITTEXT, (WPARAM) (64 * 1024), (LPARAM) 0);	
}

HWND CConsoleMessage::GetControl(void)
{
	return m_pEditBox;
}

void CConsoleMessage::Write(const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	Write(C_MSG_CONSOLE, C_MSG_NORMAL, sbuf);
}

void CConsoleMessage::Write(int nType, const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	Write(nType, C_MSG_NORMAL, sbuf);
}

void CConsoleMessage::Write(int nType, int nPosition, const char* msg, ...)
{		
	// Memory allocation for buffer
	char sbuf[C_BUFFER_SIZE];	
	
	// va_start            va_arg              va_end
    // va_list             va_dcl (UNIX only)
	va_list ap;
	va_start(ap, msg);
	StringCbVPrintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	switch (nType)
	{
	case C_MSG_CONSOLE :
		UpdateEditControl(sbuf); // Write to Console		
		break;
		
	default:
		break;
	}
}

void CConsoleMessage::UpdateEditControl(char* sbuf)
{
	if ( !sbuf ) return;
	if ( !m_pEditBox ) return;

	DWORD dwRetCode;
	
	// Update Control and Scroll to end of the line	
	::strcat(sbuf, "\r\n");
	
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX,	SMTO_BLOCK, 100, &dwRetCode);    
	::SendMessageTimeout(m_pEditBox, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			SMTO_BLOCK, 100, &dwRetCode);    
	::SendMessageTimeout(m_pEditBox, EM_SCROLLCARET,(WPARAM) 0,				(LPARAM) 0,			SMTO_BLOCK, 100, &dwRetCode);    
	::SendMessageTimeout(m_pEditBox, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) sbuf,		SMTO_BLOCK, 100, &dwRetCode);    
	::SendMessageTimeout(m_pEditBox, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1,			SMTO_BLOCK, 100, &dwRetCode);    
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX,	SMTO_BLOCK, 100, &dwRetCode);    
}