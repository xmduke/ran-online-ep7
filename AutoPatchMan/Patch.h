#pragma once

// 실제 파일과 파일리스트 비교( 서버, 클라이언트 )
// 패치 서버와 클라이언트 파일리스트 버전 비교
// 파일 설치 실패 원인 확인( extract로 추정 )
//
// 최초 클라이언트 설치 이후 '자동패치'시 모든 파일을 패치한다.
// 클라이언트 파일리스트와 패치서버 파일리스트를 비교해야함.
// 패치서버 버전관리가 잘못되었거나, 배포된 파일리스트가 잘못되었다.
// 배포된 파일리스트가 잘못 되었다는 이야기는 클라이언트 '파일리스트'가 제작된 이후에
// 패치서버쪽 버전을 완전히 바꿨거나 클라이언트 '파일리스트'가 현재 이용중인 패치서버와는
// 다른 곳에서 제작된 것이다. (수동패치가 한번이라도 진행되었는가? )
// 마지막으로 패치된 '수동패치'를 확인한 후, 위 과정을 봐야함
//
// 패치 리스트에 포함되어서는 안되는 파일이 있지는 않는가?
// (루트폴더 '\'에 '*.dll, Launcher*.exe, cFileList.bin, cVer.bin'가 존재하면 안된다. )
// 위 파일이 포함되어 있을 경우, 설치하다가 실패하거나 경우에 따라 잘못된 패치를 할
// 가능성이 있음.

#include "GlobalVariable.h"

const int nFILENAME = 64;
const int nSUBPATH = 128;

struct	SFILENODE
{
	char	FileName[nFILENAME];
	char	SubPath[nSUBPATH];	
	int		Ver;

public:
	SFILENODE()
	{
		memset ( FileName, 0, sizeof ( char ) * nFILENAME );
		memset ( SubPath, 0, sizeof ( char ) * nSUBPATH );
		Ver = 0;
	}
};

typedef	std::vector<SFILENODE*>		FILEVECTOR;
typedef	FILEVECTOR::iterator		FILEVECTOR_ITER;

typedef	std::map<std::string, SFILENODE*>	FILEMAP;
typedef	FILEMAP::iterator					FILEMAP_ITER;


class	CPatch;
class	CHttpPatch;

BOOL	Initialize ();
BOOL	LoadList ();
BOOL	MakeNewList ( const int cPatchVer, const int sPatchVer,
					 const int cGameVer, const int sGameVer );
//BOOL	DownloadFilesByFtp ( CPatch* pFtpPatch );
BOOL	DownloadFilesByHttp ( CHttpPatch* pHttpPatch );
BOOL	Installing ();
BOOL	Extract ();
BOOL	DeleteNotFoundFile();
BOOL	DeleteDownFiles ();
BOOL	Destroy ();
BOOL	VersionUp ( int sGameVer );
//BOOL	CheckIntegrity ( CString strPath );

//	Note : Download Success List ( DS LIst ) 구조
//		버전 + 파일 개수 + 리스트
//		모두 SFILENODE 구조체를 이용한다.
BOOL	SaveDownList ( int sGameVer );
BOOL	LoadDownList ();
BOOL	DeleteDownList ();

BOOL	SaveCopyList ( int sGameVer );
BOOL	LoadCopyList ();
BOOL	DeleteCopyList ();

//	NOTE
//		
BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir = NS_GLOBAL_VAR::strDownloadTemp );

//	NOTE
//		패치 메인
//struct	S_PATCH_THREAD_PARAM;
//DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam );
//DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam );