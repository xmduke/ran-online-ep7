// AutoPatchManDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "ServerLoginThread.h"
#include "FtpThread.h"
#include "GlobalVariable.h"

#include "DebugSet.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAutoPatchManDlg::UpdateLoginConnect ()
{
	if( m_pLoginThread )
	{
		if( m_pLoginThread->IsForceTerminate () ) // 강제종료 Canceled
		{	
			KillTimer ( E_LOGIN_CONNECT );
			return ;
		}

		if( !m_pLoginThread->IsRunning() && m_pLoginThread->IsFail() ) // 실패한 경우
		{
			KillTimer ( E_LOGIN_CONNECT );

			//	최대 시도를 넘을 경우
			if ( NS_GLOBAL_VAR::nLOGIN_TRY == m_nLoginTry )
			{
				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 13 )  ); // 최대 접속회수 초과
			#ifdef CH_PARAM
				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
				m_ctrlRetry.EnableWindow( TRUE );
				m_ctrlOption.EnableWindow( TRUE );
			#endif
				return ;
			}

			EndThread( m_pLoginThread );
			BEGIN_LOGIN_THREAD (); // 다시 로그인 시도

			return ;
		}

		if( m_pLoginThread->IsRunning () ) // 접속 시도
		{
			const UINT nLeftTime = GET_TIME ();
			if( 1 <= nLeftTime )
			{
				static	int	LeftSecBack = -1;
				static	int	LoginTryBack= -1;

				const UINT nLeftSec = (UINT)(nLeftTime / n1000_MILISEC);

				if ( nLeftSec != LeftSecBack || m_nLoginTry != LoginTryBack )
				{
					CString	strTemp, strMsg;
					strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 14 );  // 접속 시도중
					strTemp.Format ( "%s %d sec [%d/%d]", strMsg.GetString(), nLeftSec, m_nLoginTry, NS_GLOBAL_VAR::nLOGIN_TRY );
					ListAddString( strTemp );

					LeftSecBack = nLeftSec;
					LoginTryBack= m_nLoginTry;
				}
			}
			else
			{			
				//	타이머 & 스레드 종료
				KillTimer( E_LOGIN_CONNECT );
				EndThread( m_pLoginThread );

				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 15 )  ); // 접속 시간 초과
			}

			return ;
		}
	}

	// note : 스레드의 정상 종료
	// 타이머 & 스레드 종료
	KillTimer( E_LOGIN_CONNECT );
	EndThread( m_pLoginThread );

	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 16 )  ); // 서버 접속, 정보 전송에 성공
	SetTimer ( E_CHECK_VERSION, nCHECK_VERSION_TIME_ELAPSE, NULL );

	return;
}

void CAutoPatchManDlg::UpdateCheckVersion ()
{
	switch ( CheckVersion () )
	{	
	case E_CHK_VER_NOT_READY:		//	아직 버전을 받지 못함
	case E_CHK_VER_SERVER_UPDATE:	//	서버 점검중
	case E_MSG_FAILED:				//	버전 비교 실패
		{
			KillTimer ( E_CHECK_VERSION );

		#ifdef CH_PARAM
			ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
			m_ctrlRetry.EnableWindow( TRUE );
			m_ctrlOption.EnableWindow( TRUE );
		#endif
		}
		break;

		//	버전 전송 성공
	case E_MSG_SUCCEED:
		{
			KillTimer ( E_CHECK_VERSION );

			//	Patch나, Game이 패치되어야 하는가?
			if ( m_emPatchState != E_VERSION_CUR || m_emGameState != E_VERSION_CUR )
			{	
				//	NOTE
				//		현재는 무조건 Http만 쓰는것으로 픽스
				m_bUseHttp = true;
				if ( m_bUseHttp )
				{
					BEGIN_PATCH ();
				}
			}		
			else
			{
			#ifdef CH_PARAM
				{
					static S_PATCH_THREAD_PARAM sParam;
					sParam.pHttpPatch = m_pHttpPatch;
					sParam.cPatchVer = m_cPatchVer;
					sParam.sPatchVer = m_sPatchVer;
					sParam.cGameVer = m_cGameVer;
					sParam.sGameVer = m_sGameVer;
					BeginThread( E_THREAD_HASH, m_pHashThread, &sParam );
					SetTimer( E_CHECK_HASH, 30, NULL );
				}
			#else
				END_PATCH (); // 아무 패치가 필요없을 경우
			#endif
			}
		}
		break;
	}
}

void	CAutoPatchManDlg::BEGIN_LOGIN_THREAD ()
{
	++m_nLoginTry;
	BeginThread( E_THREAD_LOGIN, m_pLoginThread, m_pNetClient, RANPARAM::LoginAddress );
	SET_TIME( NS_GLOBAL_VAR::nLOGIN_CONNECT_TIMELEFT * n1000_MILISEC );
	SetTimer( E_LOGIN_CONNECT, nLOGIN_CONNECT_TIME_ELAPSE, NULL );
}

void CAutoPatchManDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    switch ( nIDEvent )
	{
	case E_LOGIN_CONNECT:
		{
			UPDATE_TIME ( nLOGIN_CONNECT_TIME_ELAPSE );

			UpdateControls ();
			UpdateLoginConnect ();
		}
		break;

	case E_CHECK_VERSION:
		{
			UpdateControls ();
			UpdateCheckVersion ();
		}
		break;

	case E_START_PATCH:
		{
			UpdateControls();

			if( m_pPatchThread )
			{
				if( m_pPatchThread->IsForceTerminate() )
				{
					KillTimer ( E_START_PATCH );
					break;
				}
				
				if( !m_pPatchThread->IsRunning() )
				{
					KillTimer ( E_START_PATCH );
					if( m_pPatchThread->IsFail() )
					{
						if ( m_pPatchThread->IsExtractError() )
						{
							//	파일 압축풀기를 실패할 경우 처음부터 다시 시작한다.
							//	압축풀기에 실패했다는것은 클라이언트에 다운된 파일이 손상되었음을 의미한다.
							//	만약 DLL 같은 패치가 안되는 파일이 있다면 BEGIN_PATCH()를 할 경우
							//	무한 루프에 빠진다. 그래서 지금은 주석 처리를 해두었다.
							//BEGIN_PATCH ();
						}

						#ifdef CH_PARAM
							ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
							m_ctrlRetry.EnableWindow( TRUE );
							m_ctrlOption.EnableWindow( TRUE );
						#endif
					}
					else // Succeed
					{
					#ifdef CH_PARAM
						{
							KillTimer ( E_START_PATCH );

							static S_PATCH_THREAD_PARAM sParam;
							sParam.pHttpPatch = m_pHttpPatch;
							sParam.cPatchVer = m_cPatchVer;
							sParam.sPatchVer = m_sPatchVer;
							sParam.cGameVer = m_cGameVer;
							sParam.sGameVer = m_sGameVer;
							BeginThread( E_THREAD_HASH, m_pHashThread, &sParam );
							SetTimer( E_CHECK_HASH, 30, NULL );
						}
					#else
						END_PATCH (); // MEMO : 시작 버튼이 눌리기 전까지 종료가 아니다.

						if( m_ctrlCheckBox.GetCheck() == BST_CHECKED )
						{
							OnBnClickedButtonStart();
						}
					#endif
					}
				}
			}
		}
		break;

	case E_CHECK_HASH:
		{
			UpdateControls();

			if( m_pHashThread )
			{
				if( m_pHashThread->IsForceTerminate() )
				{
					KillTimer ( E_CHECK_HASH );
					break;
				}

				if( !m_pHashThread->IsRunning() )
				{
					KillTimer ( E_CHECK_HASH );
					if( m_pHashThread->IsFail() )
					{
						if ( m_pHashThread->IsExtractError() )
						{
						}

						#ifdef CH_PARAM
							ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
							m_ctrlRetry.EnableWindow( TRUE );
							m_ctrlOption.EnableWindow( TRUE );
						#endif
					}
					else // Succeed
					{
						END_PATCH ();

						if( m_ctrlCheckBox.GetCheck() == BST_CHECKED )
						{
							OnBnClickedButtonStart();
						}
					}
				}
			}
		}
		break;

	case E_WEB_BROWSER:
		{
			m_ctrlNotify.Navigate( m_strNewsURL.GetString(), NULL, NULL, NULL, NULL );
			KillTimer( E_WEB_BROWSER );
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}