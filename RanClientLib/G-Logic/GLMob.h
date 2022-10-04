#pragma once

#include "GLCharDefine.h"
#include "Actor.h"
#include "DxSkinChar.h"
#include "GLMobSchedule.h"

class GLCharacter;

#ifndef RANDOM_NUM
	#define RANDOM_NUM      (((FLOAT)rand()-(FLOAT)rand())/RAND_MAX)
#endif

class GLMob : public DxSkinChar, public Actor
{
protected:
	char			m_szSkinObj[128];

	D3DXVECTOR3		m_vDir;
	D3DXVECTOR3		m_vDirOrig;
	D3DXVECTOR3		m_vPos;
	D3DXMATRIX		m_matTrans;

	//	Note : 속성값.
	//
	BOOL			m_bRun;

	//	Note : 현제 상태.
	//
	EMACTIONTYPE	m_Action;
	float			m_fIdleTime;

	//	Note : 설정.
	//
protected:
	float			m_fMAXATRANGE;		//	2
	float			m_fMAXVIEWRANGE;	//	3

	float			m_fWalkVelo;		//	4
	float			m_fRunVelo;			//	5
	float			m_fAttackDelay;		//	6

	//	Note : 스케쥴러 데이타.
	//
protected:
	void Acting ( float fElapsedTime );

protected:
	GLCharacter*			m_pCharacter;

	BOOL					m_bTarget;
	BOOL					m_bAttackAble;
	BOOL					m_bTracing;

	BOOL					m_bPreOrder;
	float					m_fStayTimer;
	float					m_fAttackTimer;
	DWORD					m_dwUnderShockCount;

	GLMobSchedule*			m_pMobSchedule;
	SGLNODE<SMOBACTION>*	m_pCurrentAct;

	//	Note : AABB
	//
protected:
	D3DXVECTOR3		m_vMaxOrg;
	D3DXVECTOR3		m_vMinOrg;

public:
	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

protected:
	void TurnAction ( EMACTIONTYPE toAction );

public:
	void ActionMoveTo ( D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt );
	void UnderShock ();

public:
	D3DXVECTOR3 GetPosition ()	{ return m_vPos; }
	BOOL IsDead () { return FALSE; }

public:
	void SetSkinObj ( char *szFile )	{ strcpy ( m_szSkinObj, szFile ); }
	HRESULT Create ( LPDIRECT3DDEVICE8 pd3dDevice, NavigationMesh* pNavi );
	void SetSchedule ( GLMobSchedule* pMobSch );
	void SetCharacter ( GLCharacter* pChar ) { m_pCharacter = pChar; }

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB );
	HRESULT RenderShadow ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	static char		m_szGLActionType[8][128];

public:
	GLMob ();
	~GLMob ();
};

class GLMobMan
{
protected:
	LPDIRECT3DDEVICE8	m_pd3dDevice;
	CGLLIST<GLMob*>		m_MobList;

	GLCharacter*		m_pCharacter;
	NavigationMesh*		m_pNaviMesh;

public:
	void SetCharacter ( GLCharacter* pChar ) { m_pCharacter = pChar; }
	void SetNaviMesh ( NavigationMesh* pNaviMesh );
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice );

public:
	GLMob* DetectMob ();

public:
	HRESULT NewMob ( SGLNODE<GLMobSchedule*>* pMobNode );
	void CleanUp ();

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv );
	HRESULT RenderShadow ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv );

public:
	GLMobMan ();
	~GLMobMan ();
};