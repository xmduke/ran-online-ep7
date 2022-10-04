#ifndef	__GETDXVER__
#define	__GETDXVER__

BOOL	DxCheckVer();
DWORD	GetDXVersion();
HRESULT GetFileVersion( TCHAR* szPath, TCHAR* szVersion, DWORD cchVersion );


#endif	//	__GETDXVER__