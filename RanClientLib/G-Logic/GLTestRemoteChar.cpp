#include "StdAfx.h"
#include "GLGaeaServer.h"
#include "DxInputDevice.h"
#include "DxViewPort.h"
#include "GLContrlMsg.h"
#include "GLChar.h"

#include "GLTestRemoteChar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

BOOL	GLTestRemoteChar::m_bTRACKING_CAMERA = TRUE;

GLTestRemoteChar::GLTestRemoteChar(void) :
	m_dwActState(NULL),
	m_dwCharID(EM_NULL_CHAR),
	m_fMoveTimer(0.0f)
{
}

GLTestRemoteChar::~GLTestRemoteChar(void)
{
}

HRESULT GLTestRemoteChar::SetPos ( D3DXVECTOR3 vPos )
{
	if ( EM_NULL_CHAR!=m_dwCharID )
	{
		PGLCHAR pChar = GLGaeaServer::Instance.GetChar ( m_dwCharID );
		pChar->SetPosition ( vPos );
	}

	return S_OK;
}

HRESULT GLTestRemoteChar::FrameMove ( float fTime, float fElapsedTime )
{
	DxInputDevice &dxInputDev = DxInputDevice::Instance;

	DWORD dwKeyR = dxInputDev.GetKeyState ( DIK_R );
	DWORD dwKeyLSHIFT = dxInputDev.GetKeyState ( DIK_LSHIFT );
	DWORD dwMLeft = dxInputDev.GetMouseState ( DXMOUSE_LEFT );
	DWORD dwMMiddle = dxInputDev.GetMouseState ( DXMOUSE_MIDDLE );
	DWORD dwMRight = dxInputDev.GetMouseState ( DXMOUSE_RIGHT );
	//CDebugSet::ToView ( 18, "%03d", dwMRight );

	if ( m_dwCharID != EM_NULL_CHAR && GLTestRemoteChar::m_bTRACKING_CAMERA )
	{
		PGLCHAR pChar = GLGaeaServer::Instance.GetChar ( m_dwCharID );
		if ( pChar )
		{
			D3DXVECTOR3 vCamera = pChar->GetPosition();
			vCamera.y += 10.0f;
			DxViewPort::Instance.CameraJump ( vCamera );
		}
	}

	if ( (dwKeyLSHIFT&DXKEY_PRESSED) && (dwMLeft&DXKEY_UP) )
	{
		D3DXVECTOR3 vTargetPt, vFromPt;
		vFromPt = DxViewPort::Instance.GetFromPt ();
		BOOL bTargetted = DxViewPort::Instance.GetMouseTargetPosWnd ( vTargetPt );

		if ( bTargetted )
		{
			GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
			if ( pLandMan )
			{
				PGLCHAR pChar = pLandMan->DetectCollisonPC ( vFromPt, vTargetPt );
				if ( pChar )	m_dwCharID = pChar->m_dwGaeaID;
			}
		}
	}

	if ( m_dwCharID != EM_NULL_CHAR )
	{
		PGLCHAR pChar = GLGaeaServer::Instance.GetChar ( m_dwCharID );
		if ( !pChar )
		{
			m_dwCharID = EM_NULL_CHAR;
			return S_FALSE;
		}

		if ( dwKeyR&DXKEY_UP )
		{
			if ( pChar->IsSTATE(EM_ACT_RUN) )
			{
				m_dwActState &= ~EM_ACT_RUN;

				//	Note : 테스트 메세지 발생.
				//
				GLMSG::SNETPC_MOVESTATE MoveState;
				MoveState.dwActState = m_dwActState;
				pChar->MsgProcess ( (NET_MSG_GENERIC*) &MoveState );
			}
			else
			{
				m_dwActState |= EM_ACT_RUN;

				//	Note : 테스트 메세지 발생.
				//
				GLMSG::SNETPC_MOVESTATE MoveState;
				MoveState.dwActState = m_dwActState;
				pChar->MsgProcess ( (NET_MSG_GENERIC*) &MoveState );
			}
		}

		if ( dwMRight&DXKEY_DOWNED )
		{
			if ( !pChar->IsACTION(GLAT_ATTACK) )
			{
				D3DXVECTOR3 vTargetPt, vFromPt;
				vFromPt = DxViewPort::Instance.GetFromPt ();
				BOOL bTargetted = DxViewPort::Instance.GetMouseTargetPosWnd ( vTargetPt );		

				if ( bTargetted )
				{
					GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
					STARGETID sTargetID = pLandMan->DetectEnemy ( EMD_MOB, vFromPt, vTargetPt );

					if ( sTargetID.dwID!=EMTARGET_NULL )
					{
						GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
						D3DXVECTOR3 vTarPos = GLGaeaServer::Instance.GetTargetPos ( pLandMan, sTargetID );

						D3DXVECTOR3 vDistance = pChar->GetPosition() - vTarPos;
						float fDistance = D3DXVec3Length ( &vDistance );
					
						WORD wTarBodyRadius = GLGaeaServer::Instance.GetTargetBodyRadius ( pLandMan, sTargetID );

						WORD wAttackRange = wTarBodyRadius + pChar->GETBODYRADIUS () + pChar->GETATTACKRANGE();
						WORD wAttackAbleDis = wAttackRange + 2;

						if ( fDistance<wAttackAbleDis  )
						{
							//	Note : 공격 메시지 발생.
							//
							GLMSG::SNETPC_ATTACK NetMsg;
							NetMsg.emTarCrow = sTargetID.emCrow;
							NetMsg.dwTarID = sTargetID.dwID;

							pChar->MsgProcess ( (NET_MSG_GENERIC*) &NetMsg );
						}
						else
						{
							//	Note : 이동 수행.
							//
							D3DXVECTOR3 vPos = vTarPos - pChar->GetPosition();
							D3DXVec3Normalize ( &vPos, &vPos );
							vPos = vTarPos - vPos*wAttackRange;

							GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
							if ( pLandMan )
							{
								BOOL bColl=FALSE;
								DWORD dwCollID;
								D3DXVECTOR3 vCollPos;
								pLandMan->GetNavi()->IsCollision ( vPos-D3DXVECTOR3(0,10,0), vPos-D3DXVECTOR3(0,-10,0), vCollPos, dwCollID, bColl );
								if ( bColl )
								{
									//	Note : 테스트 메세지 발생.
									//
									GLMSG::SNETPC_GOTO NetGoto;
									NetGoto.dwActState = m_dwActState;
									NetGoto.vCurPos = pChar->GetPosition();
									NetGoto.vTarPos = vCollPos;
									pChar->MsgProcess ( (NET_MSG_GENERIC*) &NetGoto );
								}
							}
						}
					}
				}
			}
		}

		if ( dwMLeft&DXKEY_UP )
		{
			D3DXVECTOR3 vTargetPt, vFromPt;
			vFromPt = DxViewPort::Instance.GetFromPt ();
			BOOL bTargetted = DxViewPort::Instance.GetMouseTargetPosWnd ( vTargetPt );		

			if ( bTargetted )
			{
				GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
				if ( pLandMan )
				{
					BOOL bColl=FALSE;
					DWORD dwCollID;
					D3DXVECTOR3 vCollPos;
					pLandMan->GetNavi()->IsCollision ( vFromPt, vTargetPt, vCollPos, dwCollID, bColl );
					if ( bColl )
					{
						//	Note : 테스트 메세지 발생.
						//
						GLMSG::SNETPC_GOTO NetGoto;
						NetGoto.dwActState = m_dwActState;
						NetGoto.vCurPos = pChar->GetPosition();
						NetGoto.vTarPos = vCollPos;
						pChar->MsgProcess ( (NET_MSG_GENERIC*) &NetGoto );
					}
				}
			}
		}
		else if ( dwMLeft&DXKEY_DOWNED )
		{
			D3DXVECTOR3 vTargetPt, vFromPt;
			vFromPt = DxViewPort::Instance.GetFromPt ();
			BOOL bTargetted = DxViewPort::Instance.GetMouseTargetPosWnd ( vTargetPt );		

			if ( bTargetted )
			{
				m_fMoveTimer -= fElapsedTime; 
				if ( m_fMoveTimer < 0.0f )
				{
					m_fMoveTimer = 0.3f;

					GLLandMan* pLandMan = GLGaeaServer::Instance.GetRootMap();
					if ( pLandMan )
					{
						BOOL bColl=FALSE;
						DWORD dwCollID;
						D3DXVECTOR3 vCollPos;
						pLandMan->GetNavi()->IsCollision ( vFromPt, vTargetPt, vCollPos, dwCollID, bColl );
						if ( bColl )
						{
							//	Note : 테스트 메세지 발생.
							//
							GLMSG::SNETPC_GOTO NetGoto;
							NetGoto.dwActState = m_dwActState;
							NetGoto.vCurPos = pChar->GetPosition();
							NetGoto.vTarPos = vCollPos;
							pChar->MsgProcess ( (NET_MSG_GENERIC*) &NetGoto );
						}
					}
				}
			}
		}
	}

	//CDebugSet::ToView ( 12, "%d", m_dwCharID );

	return S_OK;
}







