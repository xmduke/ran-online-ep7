#include "stdafx.h"
#include "DxMethods.h"
#include "DxEffectMan.h"
#include "EDITMESHS.h"

#include "StringUtils.h"
#include "StringFile.h"
#include "DxSkinCharData.h"

#include "GLCharacter.h"
#include "GLMob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

char GLMob::m_szGLActionType[8][128] =
{
	"GLAT_IDLE",
	"GLAT_GUARD",
	"GLAT_TALK",
	"GLAT_MOVE",
	"GLAT_ATTACK",
	"GLAT_SHOCK",
	"GLAT_DIE"
};


GLMob::GLMob () :
	m_vDir(0,0,-1),
	m_vDirOrig(0,0,-1),
	m_vPos(0,0,0),
	m_Action(GLAT_IDLE),
	m_fIdleTime(0.0f),
	m_bRun(FALSE),
	m_pCharacter(NULL),
	m_pMobSchedule(NULL),
	m_pCurrentAct(NULL),
	m_fStayTimer(0.0f),
	m_fAttackTimer(0.0f),
	m_dwUnderShockCount(0),
	
	m_fMAXVIEWRANGE(60.0f),	//	2
	m_fMAXATRANGE(16.0f),	//	3
	m_fWalkVelo(10.0f),		//	4
	m_fRunVelo(28.3f),		//	5
	m_fAttackDelay(2.0f),	//	6

	m_bTarget(FALSE),
	m_bAttackAble(FALSE),
	m_bTracing(FALSE),
	m_bPreOrder(FALSE),
	m_vMaxOrg(3,18,3),
	m_vMinOrg(-3,0,-3),
	m_vMax(3,18,3),
	m_vMin(-3,0,-3)
{
	strcpy ( m_szSkinObj, "test.chf" );
	D3DXMatrixIdentity ( &m_matTrans );
}

GLMob::~GLMob ()
{

}

HRESULT GLMob::Create ( LPDIRECT3DDEVICE8 pd3dDevice, NavigationMesh* pNavi )
{
	DxSkinCharData* pCharData = DxSkinCharDataContainer::Instance.LoadData ( m_szSkinObj, pd3dDevice );
	if ( !pCharData )	return E_FAIL;

	DxSkinChar::SetCharData ( pCharData, pd3dDevice );

	Actor::SetMaxSpeed ( m_fWalkVelo );
	if ( pNavi ) Actor::Create ( pNavi, m_vPos, -1 );


	//	Note : 세부 설정을 파일에서 읽음.
	//
	char szFullPath[256];
//	strcpy ( szFullPath, GLMobScheduleMan::GetPath() );
	strcat ( szFullPath, "mob.txt" );


	CStringFile StrFile;
	if ( !StrFile.Open ( szFullPath ) )
	{
		MessageBox ( NULL, "'mob.txt' 파일이 존제하지 않습니다.", "ERROR", MB_OK );
		return E_FAIL;
	}

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "\t" );
	STRUTIL::RegisterSeparator ( " " );

	CString strLine;
	while ( StrFile.GetNextLine(strLine) )
	{
		CStringArray strArray;

		STRUTIL::StringSeparate ( strLine, strArray );
		int nCount = (int) strArray.GetCount();

		if ( nCount==0 )	continue;

		if ( strArray.GetAt(0) == m_szSkinObj )
		{
			for ( int i=0; i<nCount; i++ )
			{
				CString tempString = strArray.GetAt(i);
				
				switch ( i )
				{
				case 1:
					m_fMAXATRANGE = (float) atof ( tempString.GetBuffer() );
					break;

				case 2:
					m_fMAXVIEWRANGE = (float) atof ( tempString.GetBuffer() );
					break;

				case 3:
					m_fWalkVelo = (float) atof ( tempString.GetBuffer() );
					break;

				case 4:
					m_fRunVelo = (float) atof ( tempString.GetBuffer() );
					break;

				case 5:
					m_fAttackDelay = (float) atof ( tempString.GetBuffer() );
					break;
				};
			}
			break;
		}
	}

	return S_OK;
}

void GLMob::SetSchedule ( GLMobSchedule* pMobSch )
{
	m_vPos = pMobSch->m_pAffineParts->vTrans;

	//	NEED : 기타 설정 필요.
	//
	m_pMobSchedule = pMobSch;
	m_pCurrentAct = m_pMobSchedule->m_sMobActList.m_pHead;
}

void GLMob::ActionMoveTo ( D3DXVECTOR3 vFromPt, D3DXVECTOR3 vTargetPt )
{
	TurnAction ( GLAT_MOVE );
	Actor::IsCollision ( vFromPt, vTargetPt );
}

void GLMob::TurnAction ( EMACTIONTYPE toAction )
{
	m_Action = toAction;

	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime = 0.0f;
		}
		break;

	case GLAT_ATTACK:
		break;
	};

	if ( m_Action!=GLAT_MOVE && Actor::PathIsActive() )		Actor::Stop();
}

void GLMob::UnderShock ()
{
	m_dwUnderShockCount++;

	if ( m_dwUnderShockCount > 3 )
	{
		m_dwUnderShockCount = 0;
		TurnAction ( GLAT_SHOCK );
	}
}

void GLMob::Acting ( float fElapsedTime )
{
	m_fStayTimer -= fElapsedTime;
	if ( m_fStayTimer<0.0f && ( m_Action==GLAT_IDLE || m_Action==GLAT_GUARD ) && !m_bTarget )
	{
		if ( m_pCurrentAct )
		{
			const SMOBACTION &MobAction = m_pCurrentAct->Data;

			switch ( MobAction.emAction )
			{
			case EMACTION_MOVE:
				{
					TurnAction ( GLAT_MOVE );
					
					Actor::IsCollision
					(
						MobAction.vPos+D3DXVECTOR3(0,1000,0),
						MobAction.vPos+D3DXVECTOR3(0,-1000,0)
					);
				}
				break;

			case EMACTION_ACTSTAY:
				{
					m_fStayTimer = MobAction.fLife;
				}
				break;
			};

			if ( !m_bPreOrder )
			{
				if ( m_pCurrentAct->pNext )
				{
					m_pCurrentAct = m_pCurrentAct->pNext;
				}
				else
				{
					m_bPreOrder = TRUE;
					m_pCurrentAct = m_pCurrentAct->pPrev;
				}
			}
			else
			{
				if ( m_pCurrentAct->pPrev )
				{
					m_pCurrentAct = m_pCurrentAct->pPrev;
				}
				else
				{
					m_bPreOrder = FALSE;
					m_pCurrentAct = m_pCurrentAct->pNext;
				}
			}
		}
	}

	if ( m_pCharacter )
	{
		D3DXVECTOR3 vCharPos = m_pCharacter->GetPosition ();
		D3DXVECTOR3 vDistance = m_vPos-vCharPos;
		float fDistance = D3DXVec3Length ( &vDistance );

		if ( fDistance < m_fMAXVIEWRANGE )		m_bTarget = TRUE;
		else									m_bTarget = FALSE;

		if ( m_bTarget )
		{
			m_fStayTimer -= fElapsedTime;

			if ( fDistance<m_fMAXATRANGE+3.0f )
			{
				m_bAttackAble = TRUE;
				m_fStayTimer = 0.0f;
			}
			else
			{
				m_bAttackAble = FALSE;
			}
		}
		else
		{
			m_bAttackAble = FALSE;
		}

		D3DXVECTOR3 vEndPos = Actor::GetTargetPosition ();
		D3DXVECTOR3 vEndDist = m_vPos-vCharPos;
		float fEndDist = D3DXVec3Length ( &vEndDist );

		if ( !m_bTarget )							m_bTracing = FALSE;
		if ( m_bTracing )
		{
			if ( m_Action!=GLAT_MOVE )				m_bTracing = FALSE;
			if ( fEndDist>m_fMAXATRANGE*4.0f )		m_bTracing = FALSE;
		}


		BOOL bToTrace = FALSE;

		if ( m_bTarget )
		{
			if ( m_fStayTimer<=0.0f && !m_bAttackAble )		bToTrace = TRUE;
			if ( m_Action==GLAT_ATTACK && !m_bAttackAble )	bToTrace = TRUE;
		}
		
		if ( bToTrace && !m_bTracing )
		{
			D3DXVECTOR3 vPos = vCharPos - m_vPos;
			D3DXVec3Normalize ( &vPos, &vPos );

			float fThetaY = DXGetThetaYFromDirection ( vPos, m_vDirOrig );
			float fRotateY = fThetaY + RANDOM_NUM * D3DX_PI*2.0f;

			D3DXMATRIX matY;
			D3DXMatrixRotationY ( &matY, fRotateY );
			D3DXVec3TransformCoord ( &vPos, &vPos, &matY );

			vPos = vCharPos - vPos*m_fMAXATRANGE;

			m_bTracing = TRUE;
			TurnAction ( GLAT_MOVE );
			Actor::IsCollision
			(
				vPos+D3DXVECTOR3(0,1000,0),
				vPos+D3DXVECTOR3(0,-1000,0)
			);

			//	다음 길찾기 시간 유격 지정.
			m_fStayTimer = RANDOM_NUM * 1.5f;
		}
		
		if ( m_bAttackAble && !m_bTracing && m_fAttackTimer <= 0.0f )
		{
			if ( m_Action!=GLAT_ATTACK )
			{
				TurnAction ( GLAT_ATTACK );

				m_pCharacter->UnderShock ();
				m_fAttackTimer = m_fAttackDelay;
			}
		}

		//CDebugSet::ToView ( 5, "m_bTracing	= %d", m_bTracing );
		//CDebugSet::ToView ( 6, "m_Action	= %s", m_szGLActionType[m_Action] );
	}

	m_fAttackTimer -= fElapsedTime;
}

HRESULT GLMob::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : 스케쥴 적용.
	//
	Acting ( fElapsedTime );

	//	Note : 행동 상태에 따른 재어.
	//
	switch ( m_Action )
	{
	case GLAT_IDLE:
		{
			m_fIdleTime += fElapsedTime;
			if ( GetCurrentAnimMType() != AN_IDLE )				SelectAnimCont ( AN_IDLE );
			if ( m_fIdleTime > GetCurrentAnimTime() * 3.0f )	TurnAction ( GLAT_GUARD );
		}
		break;

	case GLAT_GUARD:
		{
			if ( GetCurrentAnimMType() != AN_GUARD )			SelectAnimCont ( AN_GUARD );
			if ( m_bEndAnim )									TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_TALK:
		{
			if ( GetCurrentAnimMType() != AN_TALK )				SelectAnimCont ( AN_TALK );
			if ( m_bEndAnim )									TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_MOVE:
		{
			if ( m_bRun )
			{
				if ( GetCurrentAnimMType() != AN_RUN )			SelectAnimCont ( AN_RUN );
				Actor::SetMaxSpeed ( m_fRunVelo );
			}
			else
			{
				if ( GetCurrentAnimMType() != AN_WALK )			SelectAnimCont ( AN_WALK );
				Actor::SetMaxSpeed ( m_fWalkVelo );
			}

			//	Note : Mob 위치 업데이트.
			//
			Actor::Update ( fElapsedTime );
			if ( !Actor::PathIsActive() )						TurnAction ( GLAT_IDLE );
		}
		break;

	case GLAT_ATTACK:
		if ( GetCurrentAnimMType() != AN_ATTACK )				SelectAnimCont ( AN_ATTACK );
		if ( m_bEndAnim )										TurnAction ( GLAT_IDLE );
		break;

	case GLAT_SHOCK:
		if ( GetCurrentAnimMType() != AN_SHOCK )				SelectAnimCont ( AN_SHOCK );
		if ( m_bEndAnim )										TurnAction ( GLAT_IDLE );
		break;

	case GLAT_DIE:
		if ( GetCurrentAnimMType() != AN_DIE )					SelectAnimCont ( AN_DIE );
		break;
	};

	
	//	Note : Mob 스킨 업데이트.
	//
	DxSkinAniControl::SetPosition ( m_vPos );
	DxSkinChar::FrameMove ( fTime, fElapsedTime );

	//	Note : Mob의 현제 위치 업데이트.
	//
	m_vPos = Actor::Position();

	//	Note : Mob의 현제 방향 업데이트.
	//
	if ( m_Action==GLAT_MOVE )
	{
		D3DXVECTOR3 vMovement = Actor::NextPosition();
		if ( vMovement.x != FLT_MAX && vMovement.y != FLT_MAX && vMovement.z != FLT_MAX )
		{
			D3DXVECTOR3 vDirection = vMovement - m_vPos;
			if ( !DxIsMinVector(vDirection,0.2f) )
			{
				D3DXVec3Normalize ( &vDirection, &vDirection );
				m_vDir = vDirection;
			}
		}
	}

	//	Note : 공격 방향으로 회전.
	//
	if ( m_Action==GLAT_ATTACK )
	{
		D3DXVECTOR3 vCharPos = m_pCharacter->GetPosition ();
		D3DXVECTOR3 vDirection = vCharPos - m_vPos;
		D3DXVec3Normalize ( &vDirection, &vDirection );
		m_vDir = vDirection;
	}

	//	Note : 현제 위치와 방향으로 Transform 메트릭스 계산.
	//
	D3DXMATRIX matTrans, matYRot;
	D3DXMatrixTranslation ( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );
	float fThetaY = DXGetThetaYFromDirection ( m_vDir, m_vDirOrig );
	D3DXMatrixRotationY ( &matYRot, fThetaY );

	m_matTrans = matYRot * matTrans;

	//	Note : AABB 계산.
	//
	m_vMax = m_vPos + m_vMaxOrg;
	m_vMin = m_vPos + m_vMinOrg;

	return S_OK;
}

HRESULT GLMob::Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv, BOOL bRendAABB )
{
	if ( !COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin ) )	return S_OK;
	if ( bRendAABB ) EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );	//	테스트용.
	
	DxSkinChar::Render ( pd3dDevice, m_matTrans );
	//Actor::Render ( pd3dDevice );

	return S_OK;
}

HRESULT GLMob::RenderShadow ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	//	Note : 그림자 랜더링.
	//
	DxEffectMan::Instance.RenderShadowMob ( this, m_matTrans, pd3dDevice );

	return S_OK;
}


//---------------------------------------------------------------------------[GLMobMan]
//
GLMobMan::GLMobMan () :
	m_pd3dDevice(NULL),
	m_pNaviMesh(NULL),
	m_pCharacter(NULL)
{
}

GLMobMan::~GLMobMan ()
{
	CleanUp ();
}

void GLMobMan::SetNaviMesh ( NavigationMesh* pNaviMesh )
{
	m_pNaviMesh = pNaviMesh;
}

HRESULT GLMobMan::InitDeviceObjects ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT GLMobMan::NewMob ( SGLNODE<GLMobSchedule*>* pMobNode )
{
	HRESULT hr;
	assert(m_pd3dDevice&&"D3D DEVICE 가 초기화 되지 않았음.");
	assert(m_pNaviMesh&&"네비게이션 메쉬가 초기화 되지 않았음.");

	GLMobSchedule* pMobSch = pMobNode->Data;

	GLMob* pMob = new GLMob;
	pMob->SetSchedule ( pMobSch );

	pMob->SetCharacter ( m_pCharacter );
//	pMob->SetSkinObj ( pMobSch->m_szSkinFile );
	hr = pMob->Create ( m_pd3dDevice, m_pNaviMesh );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pMob);
		return hr;
	}

	//	Note : 몹 리스트에 삽입.
	//
	m_MobList.ADDHEAD ( pMob );

	return S_OK;
}

void GLMobMan::CleanUp ()
{
	m_pNaviMesh = NULL;

	SGLNODE<GLMob*>* pCur = m_MobList.m_pHead;
	while ( pCur )
	{
		SAFE_DELETE(pCur->Data);
		pCur = pCur->pNext;
	};

	m_MobList.DELALL ();
}

GLMob* GLMobMan::DetectMob ()
{
	D3DXVECTOR3 vFrom;
	D3DXVECTOR3 vTarget;
	
	vFrom = DxViewPort::Instance.GetFromPt ();
	BOOL bTargetPos = DxViewPort::Instance.GetMouseTargetPosWnd ( vTarget );
	if ( !bTargetPos )		return NULL;

	BOOL bCollision = FALSE;
	SGLNODE<GLMob*>* pCur = m_MobList.m_pHead;
	while ( pCur )
	{
		GLMob* pMob = pCur->Data;
		bCollision = COLLISION::IsCollisionLineToAABB ( vFrom, vTarget, pMob->m_vMax, pMob->m_vMin );
		if ( bCollision )	return pMob;

		pCur = pCur->pNext;
	}
	
	return NULL;
}

HRESULT GLMobMan::FrameMove ( float fTime, float fElapsedTime )
{
	SGLNODE<GLMob*>* pCur = m_MobList.m_pHead;
	while ( pCur )
	{
		GLMob* pMob = pCur->Data;
		pMob->FrameMove ( fTime, fElapsedTime );
		
		pCur = pCur->pNext;

		//	Note : 삭제는 커서 이동후에.
		if ( pMob->IsDead() )
		{
			m_MobList.DELNODE ( pCur );
		}
	}

	return S_OK;
}

HRESULT GLMobMan::Render ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;

	SGLNODE<GLMob*>* pCur = m_MobList.m_pHead;
	while ( pCur )
	{
		GLMob* pMob = pCur->Data;
		hr = pMob->Render ( pd3dDevice, cv, FALSE );
		if ( FAILED(hr) )	return hr;

		pCur = pCur->pNext;
	}

	return S_OK;
}


HRESULT GLMobMan::RenderShadow ( LPDIRECT3DDEVICE8 pd3dDevice, CLIPVOLUME &cv )
{
	HRESULT hr;

	SGLNODE<GLMob*>* pCur = m_MobList.m_pHead;
	while ( pCur )
	{
		GLMob* pMob = pCur->Data;
		if ( COLLISION::IsCollisionVolume ( cv, pMob->m_vMax, pMob->m_vMin ) )
		{
			hr = pMob->RenderShadow ( pd3dDevice );
			if ( FAILED(hr) )	return hr;
		}

		pCur = pCur->pNext;
	}

	return S_OK;
}
