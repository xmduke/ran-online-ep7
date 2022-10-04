// SoundSourceDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SoundSourceManager.h"
#include "SoundSourceDlg.h"

#include "MainFrm.h"
#include "SoundSourceManagerView.h"

#include "dsutil.h"
#include "SoundSourceMan.h"

// CSoundSourceDlg 대화 상자입니다.
static	int		g_SamplesPerSec	= 22050;
static	int		g_Channels		= 1;
static	int		g_Bits			= 16;

IMPLEMENT_DYNAMIC(CSoundSourceDlg, CDialog)
CSoundSourceDlg::CSoundSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundSourceDlg::IDD, pParent)
	, m_valPathFile(_T(""))
	, m_valComment(_T(""))
	, m_valStaticGroup(_T(""))
	, m_valBufferCount(4)
{
	m_eDlgType = eNew;
}

CSoundSourceDlg::~CSoundSourceDlg()
{
}

void CSoundSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATHFILE, m_valPathFile);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctrlType);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_valComment);
	DDX_Text(pDX, IDC_STATIC_GROUP, m_valStaticGroup);
	DDX_Control(pDX, IDC_BUTTON_FINDFILE, m_ctrlButtonFindFile);
	DDX_Text(pDX, IDC_EDIT_BUFFER, m_valBufferCount);
}


BEGIN_MESSAGE_MAP(CSoundSourceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FINDFILE, OnBnClickedButtonFindfile)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSoundSourceDlg 메시지 처리기입니다.

void CSoundSourceDlg::OnBnClickedButtonFindfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//	<--	써 놓은 내용들 저장	-->	//
	//	<**	이렇게 하지 않으면, 파일 다이얼로그가 뜨면서
	//		써 놓은 내용들이 모두 날아가 버린다.
	//	**>
	UpdateData ( TRUE );

	//	<--	파일 필터 정의 -->	//
	CString szFilter = "Wav Files( *.wav ) |*.wav|";
	
	//	<--	파일열기	-->	//
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

    if ( dlg.DoModal() == IDOK )
	{
		//	<--	음원 테스트	-->	//
		if ( !ValidateWaveFile ( dlg.GetFileName().GetString() ) )
		{		
			return;
		}

		if ( strcmp ( m_valPathFile.GetString (), dlg.GetPathName () ) )
		{
			m_bChangeFile = TRUE;
		}

		m_OriginFileName = dlg.GetFileName();
		m_valPathFile = dlg.GetPathName();

		UpdateData ( FALSE );
	}
}

BOOL CSoundSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	for ( int i = 0; i < NTypeDesc::DescCount; i++ )
	{
		CString	Temp = NTypeDesc::Desc[i];
		m_ctrlType.AddString ( Temp.GetString() );
	}

	//	<--	데이타 생성	-->	//
	if ( m_eDlgType == eNew )
	{
		m_valStaticGroup = "입력";
		m_ctrlType.SetCurSel ( 0 );		
	}
	else
	{
		CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView ();		

		m_valStaticGroup = "수정";

		SSoundSource	*pSoundSource;
        pSoundSource = pView->GetSSM()->FindRecord ( m_ID );

		CString	FullPath = pView->GetSSM()->GetDataDirectory();
		m_valPathFile = FullPath + pSoundSource->FileName;		
		m_OriginFileName = pSoundSource->OriginFileName;

		m_valBufferCount = pSoundSource->BufferCount;

		m_ctrlType.SetCurSel ( (int)pSoundSource->TYPE );
		m_valComment = pSoundSource->Comment;		
	}

	m_bChangeFile = FALSE;

	UpdateData ( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CSoundSourceDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

    if ( !m_valPathFile.GetLength() || !m_valComment.GetLength() )
	{
		return;
	}

	CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView();

	//	<--	데이타 생성	-->	//
	if ( m_eDlgType == eNew )
	{
		SSoundSource	*pNewSoundSource = new SSoundSource;

	////////////////////////////////////////////////////////////////////////////////
	//	번호	:	$1
	//	요약	:	고유 ID 생성
	//	설명	:	고유 ID를 만들고 레코드에 등록
		pView->GetSSM()->GenerateUniqueKey();
		WORD	UniqueID = pView->GetSSM()->GetUniqueKey();
	////////////////////////////////////////////////////////////////////////////////	

		//////////////////////////////////////////////
		//	<--	현재 시간 확인
		char	szTime[128];
		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time 가져오기
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time 가져오기

		//	<--0	TimeZone 가져오기
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone 가져오기

		//	<--0	Universal Time과 TimeZone의 합성
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time과 TimeZone의 합성

		sprintf ( szTime, "%d-%02d-%02d %02d:%02d", LocalTime.wYear, LocalTime.wMonth,
			LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
		//	-->	현재 시간 확인
		////////////////////////////////////////////////


		//	<--	파일 이름 포맷
		//	<**	TYPE 2자리 + UniqueID 6자리
		//	**>
		CString	FileName;
		FileName.Format ( "%02d%06d.wav",
			(int)m_ctrlType.GetCurSel(), UniqueID );
		//	-->
		
		pNewSoundSource->State = TRUE;
		pNewSoundSource->ID = UniqueID;
		pNewSoundSource->TYPE = (ESoundType)m_ctrlType.GetCurSel();
		pNewSoundSource->FileName = FileName;
		pNewSoundSource->LastUpdated = szTime;
		pNewSoundSource->Comment = m_valComment;
		pNewSoundSource->OriginFileName = m_OriginFileName;
//		NewSoundSource.FullPath = m_valPathFile;
        pNewSoundSource->BufferCount = (WORD)m_valBufferCount;


		pView->GetSSM()->AddRecord ( pNewSoundSource, m_valPathFile );
	}
	//	<--	이미 있던 데이타 수정	-->	//
	else
	{
        SSoundSource	*pModifySoundSource = pView->GetSSM()->GetRecord ( );

		//////////////////////////////////////////////
		//	<--	현재 시간 확인
		char	szTime[128];
		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time 가져오기
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time 가져오기

		//	<--0	TimeZone 가져오기
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone 가져오기

		//	<--0	Universal Time과 TimeZone의 합성
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time과 TimeZone의 합성

		sprintf ( szTime, "%d-%02d-%02d %02d:%02d", LocalTime.wYear, LocalTime.wMonth,
			LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
		//	-->	현재 시간 확인
		////////////////////////////////////////////////

		//	<--	파일 이름 포맷
		//	<**	TYPE 2자리 + UniqueID 6자리
		//	**>
		CString	FileName;
		FileName.Format ( "%02d%06d.wav",
			(int)(ESoundType)m_ctrlType.GetCurSel(), pModifySoundSource->ID );
		//	-->		
		
		pModifySoundSource->State = TRUE;
		//pModifySoundSource->ID = UniqueID;	<--	일부러 표시해 둔것임	-->	//
		pModifySoundSource->TYPE = (ESoundType)m_ctrlType.GetCurSel();		
		pModifySoundSource->FileName = FileName;
		pModifySoundSource->LastUpdated = szTime;
		pModifySoundSource->Comment = m_valComment;
		pModifySoundSource->OriginFileName = m_OriginFileName;
//		pModifySoundSource->FullPath = m_valPathFile;

		pModifySoundSource->BufferCount = (WORD)m_valBufferCount;

		pView->GetSSM()->SetRecord ( pModifySoundSource->ID, m_bChangeFile, m_valPathFile );
	}

	OnOK();
}

void	CSoundSourceDlg::SetState ( EDlgType DlgType, WORD ID )
{
	m_eDlgType = DlgType;
	m_ID = ID;
}

BOOL	CSoundSourceDlg::ValidateWaveFile( const TCHAR* strFileName )
{    
	CWaveFile waveFile;

	if( -1 == GetFileAttributes(strFileName) )
		return FALSE;

	// Load the wave file
	if( FAILED( waveFile.Open( (char*)strFileName, NULL, WAVEFILE_READ ) ) )
	{
		waveFile.Close();
		MessageBox ("웨이브 파일이 아닙니다.");
		return FALSE;
	}
	else // The load call succeeded
	{
		WAVEFORMATEX* pwfx = waveFile.GetFormat();
		if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
		{
			MessageBox ("웨이브 파일은 반드시 PCM형식 이어야 합니다.");
			return FALSE;
		}

		// Update the UI controls to show the sound as the file is loaded
		waveFile.Close();


		// Get the samples per sec from the wave file
		DWORD dwSamplesPerSec = waveFile.m_pwfx->nSamplesPerSec;
		DWORD dwChannels = waveFile.m_pwfx->nChannels;
		DWORD dwBits = waveFile.m_pwfx->wBitsPerSample;

		//	<--	음원 포맷 비교	-->	//		
		if ( (dwSamplesPerSec == g_SamplesPerSec) &&
			(dwChannels == g_Channels )	&&
			(dwBits == g_Bits ) )
		{
			return TRUE;
		}
		
		CString	TempString;
		TempString.Format ( "음원을 괄호안과 같은 포맷으로 맞춰주셔야 합니다.\n"
			"SamplesPerSec : %d[%d], Channels : %d[%d], Bits : %d[%d]",
			dwSamplesPerSec, g_SamplesPerSec, dwChannels, g_Channels, dwBits, g_Bits );

		MessageBox ( TempString.GetString() );
        
		return FALSE;
	}
}
