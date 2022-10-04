#pragma once

// strstream 을 이용하는게 좋을수도 있습니다.
//

class COutPutStream
{
private:
	CString m_strSteam;

public:
	void SetEmpty ()			{ m_strSteam.Empty(); }
	const char* GetBuffer ()	{ return m_strSteam.GetString(); }

public:
	COutPutStream& operator+ ( const int );
	COutPutStream& operator+ ( const DWORD );
	COutPutStream& operator+ ( const float );

	COutPutStream& operator+ ( const char* );

public:
	COutPutStream(void);
	~COutPutStream(void);
};

inline COutPutStream& COutPutStream::operator+ ( const int nArg )
{
	CString strArg;
	strArg.Format ( "%d", nArg);

	m_strSteam += strArg;

	return *this;
}

inline COutPutStream& COutPutStream::operator+ ( const DWORD dwArg )
{
	CString strArg;
	strArg.Format ( "%d", dwArg );

	m_strSteam += strArg;

	return *this;
}

inline COutPutStream& COutPutStream::operator+ ( const float fArg )
{
	CString strArg;
	strArg.Format ( "%f", fArg );

	m_strSteam += strArg;

	return *this;
}

inline COutPutStream& COutPutStream::operator+ ( const char* szArg )
{
	m_strSteam += szArg;

	return *this;
}

#define PSTREAM(arg)					( (COutPutStream()+arg).GetString() )
#define SSTREAM(szBuffer,nStrLen,arg)	( StringCchCopy(szBuffer,nStrLen,(COutPutStream()+arg).GetString()) )

