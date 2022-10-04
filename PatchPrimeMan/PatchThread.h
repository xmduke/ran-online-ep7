#pragma	once

class	CPatch;
class	CHttpPatch;

struct S_PATCH_THREAD_PARAM
{
	bool		bUseHttp;

	CPatch*		pFtpPatch;
	CHttpPatch*	pHttpPatch;

//	int		cPatchVer;
	int		sPatchVer;
//	int		cGameVer;
//	int		sGameVer;	

	S_PATCH_THREAD_PARAM () :
		bUseHttp ( false ),
		pFtpPatch ( NULL ),
		pHttpPatch ( NULL ),
//		cPatchVer ( 0 ),
		sPatchVer ( 0 )
//		cGameVer ( 0 ),
//		sGameVer ( 0 )
	{
	}
};

namespace	NS_PATCH_THREAD
{	
	void	INIT ();
	BOOL	BEGIN ( S_PATCH_THREAD_PARAM* pParam );
	void	STOP ();
	void	END ();

	BOOL	IsFail ();
	void	SetFail ();

	BOOL	IsRunning ();	
	BOOL	IsForceTerminate ();

	void	SetExtractError ();
	BOOL	IsExtractError ();
};

unsigned __stdcall PatchThread ( LPVOID pData );