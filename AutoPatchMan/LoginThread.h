#pragma	once

class	CNetClient;

namespace	NS_LOGIN_THREAD
{
	BOOL	BEGIN ( CNetClient* pNetClient );
	void	STOP ();
	void	END ();

	BOOL	IsFail ();
	void	SetFail ();

	BOOL	IsRunning ();	
	BOOL	IsForceTerminate ();
};

unsigned __stdcall ConnectLoginThread ( LPVOID pData );