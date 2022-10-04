#pragma once

class	CHttpPatch;


class CLPatchThread : public CWinThread
{
	DECLARE_DYNCREATE(CLPatchThread)

protected:
	CLPatchThread() {}           // 동적 만들기에 사용되는 protected 생성자입니다.
	
	
public:
	CLPatchThread( CHttpPatch* pHttpPatch, const TCHAR* DownloadURL, const TCHAR* SavePath, DWORD nThreadID );
	virtual ~CLPatchThread();

public:
	virtual BOOL InitInstance();
	virtual void Delete();
	virtual void DownloadStart() {}
	void KillThread();


	BOOL IsFail () { return m_bFail; }
	void SetFail () { m_bFail = TRUE; }

	BOOL IsRunning ();
	BOOL IsForceTerminate ();
	
	void SetExtractError ()
	{
		SetFail ();
		m_bExtractError = TRUE;
	}
	BOOL IsExtractError () { return m_bExtractError; }

public:
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;
	bool m_bForceTerminate;
protected:
	TCHAR m_DownloadURL[_MAX_PATH];
	TCHAR m_SavePath[_MAX_PATH];

	BOOL m_bFail;
	BOOL m_bExtractError;

	DWORD m_nDlgThreadID;
	CHttpPatch*	m_pHttpPatch;

protected:
	DECLARE_MESSAGE_MAP()
};


