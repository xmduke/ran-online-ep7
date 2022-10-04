#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "PatchPrimeManDlg.h"

#include <afxinet.h>
#include "s_NetGlobal.h"
#include "s_NetClient.h"
#include "s_CConsoleMessage.h"
#include "s_CPatch.h"
#include "RANPARAM.h"
#include ".\patchprimemandlg.h"
#include "s_CHttpPatch.h"
#include "LogControl.h"
#include "GlobalVariable.h"
#include "LPatchThread.h"
#include "HttpPatchThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CPatchPrimeManDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch ( nIDEvent )
	{
	//case E_FTP_CONNECT:
	//	{
	//		UPDATE_TIME ( nFTP_CONNECT_TIME_ELAPSE );

	//		UpdateControls ();
	//		UpdateFtpConnect ();
	//	}	
	//	break;

	case E_START_PATCH:
		{
			UpdateControls(); // 프로그래스바 진행 업데이트
			
			if ( m_pThread->IsForceTerminate() )	//	Canceled
			{
				KillTimer ( E_START_PATCH );
			}
			else if ( !m_pThread->IsRunning() )	//	Not Running
			{
				KillTimer ( E_START_PATCH );

				if ( m_pThread->IsFail () )	//	Failed
				{
					//	파일 압축풀기를 실패할 경우, 처음부터 다시 시작한다.
					//	압축풀기에 실패했다는것은, 클라이언트에 다운된 파일이 손상되었음을 의미한다.					
					if ( m_pThread->IsExtractError() )
					{
						OnKillThread(); // 쓰레드를 죽이고...
						BEGIN_PATCH(); // 다시 패치 시작... 
						// MEMO : 지금은 실패하면 계속 반복된다. 제한을 뒤야겠다.
					}
				}
				else //	Succeed
				{
					END_PATCH ();
					PostMessage( WM_QUIT ); // OnOK, SendMessage, WM_CLOSE 등은 종료 카드가 0이 아니다.
				}
			}			
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//void	CPatchPrimeManDlg::BEGIN_FTP_THREAD ()
//{
//	const int nIndex = m_nFtpTry % RANPARAM::MAX_FTP;
//
//	static S_FTP_THREAD_PARAM sParam;
//	sParam.pFtpPatch = m_pFtpPatch;
//	sParam.strFtpAddress = RANPARAM::FtpAddressTable[nIndex];
//	
//	m_nFtpTry++;
//	NS_FTP_THREAD::BEGIN ( &sParam );
//	SET_TIME ( NS_GLOBAL_VAR::nFTP_CONNECT_TIMELEFT * n1000_MILISEC );
//	SetTimer ( E_FTP_CONNECT, nFTP_CONNECT_TIME_ELAPSE, NULL );
//}

//void CPatchPrimeManDlg::UpdateFtpConnect ()
//{
//	//	NOTE
//	//		강제종료
//	if ( NS_FTP_THREAD::IsForceTerminate () )	//	Canceled
//	{	
//		//	NOTE
//		//		여기에서 하는것이 맞지만,
//		//		만약, 스레드가 동작중인데 걸렸다면,
//		//		프로그램이 멈춘것처럼 보여지기 때문에,
//		//		눈에 보이지 않을때로 END작업을 미뤄둔다.
//		//NS_FTP_THREAD::END (); 
//		KillTimer ( E_FTP_CONNECT );
//		return ;
//	}
//
//	//	NOTE
//	//		실패한 경우
//	if ( NS_FTP_THREAD::IsFail () )
//	{
//		//	타이머 & 스레드 종료
//		NS_FTP_THREAD::STOP ();
//		NS_FTP_THREAD::END ();
//		KillTimer ( E_FTP_CONNECT );	
//
//		//	최대 시도를 넘을 경우
//		if ( NS_GLOBAL_VAR::nFTP_TRY == m_nFtpTry )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_005 );
//			return ;
//		}
//
//		BEGIN_FTP_THREAD ();
//
//		return ;
//	}
//
//	//	NOTE
//	//		접속 시도
//	if ( NS_FTP_THREAD::IsRunning () )
//	{
//		const UINT nLeftTime = GET_TIME ();
//		if ( 1 <= nLeftTime )
//		{
//			static	int	LeftSecBack = -1;
//			static	int	FtpTryBack= -1;
//
//			const UINT nLeftSec = (UINT)(nLeftTime / n1000_MILISEC);
//
//			if ( nLeftSec != LeftSecBack || m_nFtpTry != FtpTryBack )
//			{
//				CString	strTemp;
//				CString strMsg;
//				strMsg.LoadString( IDS_MESSAGE_006 );
//				strTemp.Format ( "%s %d sec [%d/%d]", strMsg.GetString(),
//					nLeftSec, m_nFtpTry, NS_GLOBAL_VAR::nFTP_TRY );
//				NS_LOG_CONTROL::Write ( strTemp );
//				
//				LeftSecBack = nLeftSec;
//				FtpTryBack= m_nFtpTry;
//			}
//		}
//		else
//		{			
//			//	타이머 & 스레드 종료
//			NS_FTP_THREAD::STOP ();				 
//			NS_FTP_THREAD::END ();
//			KillTimer ( E_FTP_CONNECT );		
//
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_007 );
//		}
//
//		return ;
//	}
//
//	//	타이머 & 스레드 종료
//	NS_FTP_THREAD::END ();
//	KillTimer ( E_FTP_CONNECT );		
//
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_008 );
//	BEGIN_PATCH ();
//}