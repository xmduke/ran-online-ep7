#pragma	once

class	CPatch;

struct S_FTP_THREAD_PARAM
{
	CPatch*		pFtpPatch;
	std::string	strFtpAddress;

	S_FTP_THREAD_PARAM () :		
		pFtpPatch ( NULL )
	{
	}
};

namespace	NS_FTP_THREAD
{
	BOOL	BEGIN ( S_FTP_THREAD_PARAM* pParam );
	void	STOP ();
	void	END ();

	BOOL	IsFail ();
	void	SetFail ();

	BOOL	IsRunning ();	
	BOOL	IsForceTerminate ();
};

unsigned int WINAPI ConnectFtpThread( LPVOID pData );