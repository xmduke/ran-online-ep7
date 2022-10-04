#pragma once

class GLTestRemoteChar
{
public:
	static BOOL	m_bTRACKING_CAMERA;

public:
	enum
	{
		EM_NULL_CHAR	= 0xFFFFFFFF,
	};

protected:
	DWORD	m_dwActState;

	DWORD	m_dwCharID;
	float	m_fMoveTimer;

public:
	HRESULT SetPos ( D3DXVECTOR3 vPos );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	GLTestRemoteChar(void);
	~GLTestRemoteChar(void);
};
