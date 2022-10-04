#pragma once

#include "DxFrameMesh.h"

class DxEditMan
{
public:
	const static DWORD	VERSION;

protected:
	char			m_szMapName[128];

	DxFrameMesh		m_FrameMesh;

public:
	HRESULT ActiveMap ();

public:
	HRESULT OneTimeSceneInit();
	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE8 pd3dDevice);
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE8 pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT FinalCleanup();

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	}

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	void LoadSet ( char	*pFileName, LPDIRECT3DDEVICE8 pd3dDevice );
	void SaveSet ( char	*pFileName );

public:
	DxEditMan(void);
	~DxEditMan(void);
};
