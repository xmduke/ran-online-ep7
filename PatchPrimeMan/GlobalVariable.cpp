#include "stdafx.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_GLOBAL_VAR
{
	CString	strAppPath;
	CString strProFile;

#ifdef DAUMPARAM
	TCHAR * g_szLauncherCabFile = "launcher4d.exe.cab";
#else
	TCHAR * g_szLauncherCabFile = "launcher.exe.cab";
#endif

	//TCHAR*	g_szAlphaFtpServerIP = "mincontrol.mincoms.co.kr";
	//TCHAR*	g_szAlphaFtpID = "ranalpha";
	//TCHAR*	g_szAlphaFtpPW = "ranalpha";

	//TCHAR*	g_szBetaFtpID = "ranbeta";
	//TCHAR*	g_szBetaFtpPW = "tjdrlduq";	
};