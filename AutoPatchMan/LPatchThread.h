#pragma once

class CLPatchThread : public CWinThread
{
	DECLARE_DYNCREATE(CLPatchThread)

// Attributes
protected:
	BOOL m_bFail;
	BOOL m_bExtractError;

public:
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;

// Operations
public:
	void KillThread();

protected:
	virtual void ThreadMain(){}

public:
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

protected:
	CLPatchThread();           // 동적 만들기에 사용되는 protected 생성자입니다.

public:
	virtual ~CLPatchThread();
	virtual void Delete();

protected:
	virtual BOOL InitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};