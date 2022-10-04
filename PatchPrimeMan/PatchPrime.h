#pragma once

class	CHttpPatch;

BOOL	VersionUp ( int sPatchProgramVer );
BOOL	DELETEFILE( BOOL bCab = TRUE );

BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir );

//	NOTE
//		패치 메인
struct	S_PATCH_THREAD_PARAM;
DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam );
DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam );

BOOL	MAKE_ALL_FILE_NORMAL_ATTRIBUTE ( CString strPath );