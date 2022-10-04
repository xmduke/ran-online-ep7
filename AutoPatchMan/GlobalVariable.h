#pragma once

class	CPatch;
class	CHttpPatch;

struct S_PATCH_THREAD_PARAM
{
	bool		bUseHttp;

	CPatch*		pFtpPatch;
	CHttpPatch*	pHttpPatch;

	int		cPatchVer;
	int		sPatchVer;
	int		cGameVer;
	int		sGameVer;	

	S_PATCH_THREAD_PARAM () :
	bUseHttp ( false ),
		pFtpPatch ( NULL ),
		pHttpPatch ( NULL ),
		cPatchVer ( 0 ),
		sPatchVer ( 0 ),
		cGameVer ( 0 ),
		sGameVer ( 0 )
	{
	}
};

namespace	NS_GLOBAL_VAR
{
	extern	UINT	nLOGIN_CONNECT_TIMELEFT;	//	로그인 서버 접속 타임
	extern	int		nLOGIN_TRY;					//	로그인 시도 최대 회수
	extern	UINT	nFTP_CONNECT_TIMELEFT;		//	FTP 서버 접속 타임
	extern	int		nFTP_TRY;					//	FTP 시도 최대 회수
	extern	UINT	nFORCE_EXIT_DELAYTIME;		//	강제종료 대기 시간	

	extern	CString	strAppPath;					//	프로그램이 실행되는 경로
	extern  CString strProFile;					//  사용자 Document 디렉토리
	extern	CString	strTitle;					//	윈도우 타이틀
	extern	CString	strDownloadTemp;			//	임시 다운로드 경로
	extern	CString	strServerCabFileList;		//	filelist.bin.cab (파일 리스트)
	extern	CString	strDownList;				//	cFileList.bin (클라이언트 파일 리스트)
	extern	CString strCopyList;				//	cCopyList.bin (클라이언트 카피 리스트)
//	extern	CString strCompDir;

	extern	TCHAR	g_szPatchMessage[128];
	extern	TCHAR*	g_szRanFileName;	
	
	//extern	TCHAR*	g_szAlphaFtpServerIP;
	//extern	TCHAR*	g_szAlphaFtpID;
	//extern	TCHAR*	g_szAlphaFtpPW;

	//extern	TCHAR*	g_szBetaFtpID;
	//extern	TCHAR*	g_szBetaFtpPW;

	extern	TCHAR*	g_szLauncherPatchCabFile;
	extern	TCHAR*	g_szLauncherPatchWndName;
	
	extern	TCHAR*	g_szServerFileList;
	extern	TCHAR*	g_szClientFileList;

	extern	DWORD	g_DxVersion;
	extern	DWORD	g_NotInitDxVer;
};