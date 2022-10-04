///////////////////////////////////////////////////////////////////////////////
// s_CDbmanager.cpp
//
// class CDbList
// class CDbmanager
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
#include "s_CDbmanager.h"
#include "GLogicData.h"
#include "s_CCfg.h"
#include <strstream>
#include "GLCharAG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FMTSTR    "%-8.8s %-8.8s %-8.8s %-8.8s\n"
#define FMTSTR1    "%-8.8s %-8.8s %8.8ld %8.8ld\n"
#define MONEYPRINT 12

///////////////////////////////////////////////////////////////////////////////
// DB_Initialize()
// Description 
//		Initialize Database
// Arguments
//		
// Return Value
//		Succeeded : DB_OK
//		Error : DB_ERROR
///////////////////////////////////////////////////////////////////////////////
int DB_Initialize(void)
{
	if (dbinit() == (char *)NULL)
	{		
		return DB_ERROR;
	}	
	// Install the user-supplied error-handling	and message-handling routines.
	//	dberrhandle((DBERRHANDLE_PROC)DB_err_handler);
	//	dbmsghandle((DBMSGHANDLE_PROC)DB_msg_handler);
	return DB_OK;
}

///////////////////////////////////////////////////////////////////////////////
// DB_Shutdown(void)
// Description 
//		Shutdown all database connections.
// Arguments
//		
// Return Value
//
///////////////////////////////////////////////////////////////////////////////
void DB_Shutdown(void)
{
	// Close the connection and exit database.
	dbexit();
	// windows only
	dbwinexit(); 
}

///////////////////////////////////////////////////////////////////////////////
// DB_err_handler()
// Description 
//	
// Arguments
//	
// Return Value
// 
///////////////////////////////////////////////////////////////////////////////
int DB_err_handler(PDBPROCESS dbproc, int severity, int dberr, int oserr, 
				char * dberrstr, char * oserrstr)
{	
	if (dberrstr != NULL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "INFO:DB-LIBRARY %s", dberrstr);		
		return DB_ERROR;		
	}
	if (oserr != DBNOERR)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:Operating-system error(%s)", oserrstr);
		return DB_ERROR;
	}
	/*
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
										"INFO:DB-Library %s", 
										dberrstr);

	if (severity == EXCOMM && (oserr != DBNOERR || oserrstr))
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, 
										"ERROR:Net-Lib (%d)%s", 
										oserr, 
										oserrstr);

	if (oserr != DBNOERR)
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,
										"ERROR:Operating-system error:%s",
										oserrstr);
	*/

	if ((dbproc == NULL) || (DBDEAD(dbproc)))
		return(INT_EXIT);
	else
		return(INT_CANCEL);
}

///////////////////////////////////////////////////////////////////////////////
// DB_msg_handler()
// Description 
//
// Arguments
//
// Return Value
// 
///////////////////////////////////////////////////////////////////////////////
int DB_msg_handler(PDBPROCESS dbproc, DBINT msgno, int msgstate, 
				int severity, char * msgtext)
{
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:SQL Server message %ld, state %d, severity %d %s", msgno, msgstate, severity, msgtext);
	return DB_ERROR;
}
