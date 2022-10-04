#pragma once

class	CPatch;
class	CHttpPatch;

struct S_PATCH_THREAD_PARAM
{
	bool bUseHttp;

	CPatch* pFtpPatch;
	CHttpPatch* pHttpPatch;

	//int cPatchVer;
	int sPatchVer;
	//int cGameVer;
	int sGameVer;	

	S_PATCH_THREAD_PARAM () :
		bUseHttp ( false ),
		pFtpPatch ( NULL ),
		pHttpPatch ( NULL ),
		sPatchVer ( 0 ),
		sGameVer ( 0 )
	{
	}
};

namespace	NS_GLOBAL_VAR
{
	extern	CString	strAppPath;					// 프로그램이 실행되는 경로
	extern	TCHAR*	g_szLauncherCabFile;
	extern	CString strProFile;

	//extern	TCHAR*	g_szAlphaFtpServerIP;
	//extern	TCHAR*	g_szAlphaFtpID;
	//extern	TCHAR*	g_szAlphaFtpPW;

	//extern	TCHAR*	g_szBetaFtpID;
	//extern	TCHAR*	g_szBetaFtpPW;
};