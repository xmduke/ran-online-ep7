#pragma once

class CEdit_IME : public CEdit
{
protected:
	BOOL	m_bStat;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL IsCOMPOSITION ()	{ return m_bStat; }

public:
	CEdit_IME(void);
	~CEdit_IME(void);
};
