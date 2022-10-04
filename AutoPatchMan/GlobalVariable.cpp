#include "stdafx.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_GLOBAL_VAR
{	
	UINT	nLOGIN_CONNECT_TIMELEFT = 10;
	int		nLOGIN_TRY = 5;
	UINT	nFTP_CONNECT_TIMELEFT = 6;
	int		nFTP_TRY = 5;
	UINT	nFORCE_EXIT_DELAYTIME = 3;	
	CString	strAppPath;
	CString strProFile;

#ifdef CH_PARAM
	CString	strTitle = "Á÷ÐÇ»¨Ô° Launcher";
#elif TW_PARAM
	CString	strTitle = "«i online Launcher";
#elif HK_PARAM 
	CString	strTitle = "«i online Launcher";
#else 
	CString	strTitle = "Ran Launcher";
#endif

	CString	strDownloadTemp = "Temp\\";
	CString	strServerCabFileList = "filelist.bin.cab";
	CString	strDownList = "cDownList.bin";
	CString strCopyList = "cCopyList.bin";
	
//	CString strCompDir = "package\\"; // ¾ÐÃàÇÒ ÆÄÀÏµéÀÇ °æ·Î

	TCHAR	g_szPatchMessage[128] = {0};
	
	#ifdef KR_PARAM
		TCHAR*	g_szRanFileName	= "RanOnline.exe";
//	#elif defined(CH_PARAM)
//		TCHAR*	g_szRanFileName	= "2046.exe";
	#else
		TCHAR*	g_szRanFileName	= "Game.exe";
	#endif

	//TCHAR*	g_szAlphaFtpServerIP = "mincontrol.mincoms.co.kr";
	//TCHAR*	g_szAlphaFtpID = "ranalpha";
	//TCHAR*	g_szAlphaFtpPW = "ranalpha";

	//TCHAR*	g_szBetaFtpID = "ranbeta";
	//TCHAR*	g_szBetaFtpPW = "tjdrlduq";	
	
	TCHAR*	g_szServerFileList = "FileList.bin";
	TCHAR*	g_szClientFileList = "cFileList.bin";	

	DWORD	g_DxVersion = 0x801;	

	TCHAR*	g_szLauncherPatchCabFile = "launcherpatch.exe.cab";
	TCHAR*	g_szLauncherPatchWndName = "Launcher Patch";
};