#pragma once

#ifndef	__DX_NAVI_MESH__
#define	__DX_NAVI_MESH__

#include "actor.h"

struct	DxFrame;
class DxNaviMesh
{
public:
	DxNaviMesh() {};
	virtual	~DxNaviMesh() {};

	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE8 pd3dDevice);	
	HRESULT	Update(void);
	HRESULT	Render(void);
	HRESULT DeleteDeviceObjects();	

	//	<--	메쏘드 작성
	//		*메쉬를 주면 그것의 정보를 뜯어 NavigationMesh 클래스에
	//		등록하는 메쏘드 작성*
	//		*이미 원하는 DxFrame이 넘어온다고 가정함*
	//		*무결성 검사 - 트라이앵글 노말의 각이 90도를 넘어가는지 체크 - 를 수행한다.
	//		만약, 있다면 E_FAIL, 없다면 S_OK를 반환한다.*
	HRESULT	CreateNaviMesh ( DxFrame *pNaviFrame );
	//	-->	메쏘드 작성

	//	<--	마우스를 이용한 캐릭터 이동
	void	IsCollision ( const D3DXVECTOR3 &vP1, const D3DXVECTOR3 &vP2 );
	void	Follow ();
	void	GotoError ();// { NaviMesh.GotoErrorPosition(  ); }
	//	-->	마우스를 이용한 캐릭터 이동

	void	SaveFile ( );
	void	LoadFile ( );


	//	<--	임시 메쏘드 및 변수들
protected:	
	HRESULT	LoadDxFrameNaviMesh ();

private:
	LPDIRECT3DDEVICE8			m_pd3dDevice;
	Actor PathObject;		// red cone that does the pathfinding
	Actor ControlObject;	// blue cone controlled by the player	
	NavigationMesh NaviMesh;// our navigation mesh
	//	-->	임시 메쏘드 및 변수들
};

#endif	//	__DX_NAVI_MESH__