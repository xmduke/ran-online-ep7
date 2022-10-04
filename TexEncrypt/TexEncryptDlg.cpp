// TexEncryptDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TexEncrypt.h"
#include "TexEncryptDlg.h"
#include ".\texencryptdlg.h"
#include "../EngineLib/DxCommon/EncryptTextureDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTexEncryptDlg 대화 상자



CTexEncryptDlg::CTexEncryptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTexEncryptDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTexEncryptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPTION_COMBO, m_cCombo);
	DDX_Control(pDX, IDC_EDIT_NOTE, m_cEdit);
}

BEGIN_MESSAGE_MAP(CTexEncryptDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_OPEN_BUTTON, OnBnClickedOpenButton)
	ON_BN_CLICKED(ID_RUN_BUTTON, OnBnClickedRunButton)
	ON_CBN_SELCHANGE(IDC_OPTION_COMBO, OnCbnSelchangeOptionCombo)
END_MESSAGE_MAP()


// CTexEncryptDlg 메시지 처리기

BOOL CTexEncryptDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	m_cCombo.InsertString( 0, "Encrypt File" );
	m_cCombo.InsertString( 1, "Encrypt Forder" );
	m_cCombo.InsertString( 2, "Decrypt File" );
	m_cCombo.InsertString( 3, "Decrypt Forder" );
	m_cCombo.SetCurSel( 0 );
	m_cEdit.SetLimitText( 0 );

	SetAppPath();

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CTexEncryptDlg::SetAppPath()
{
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );
		
		if ( !strAppPath.IsEmpty() )
		if ( strAppPath.GetAt(0) == '"' )
			strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

        strAppPath += '\\';
		strAppPath += "textures\\";
		m_strAppPath = strAppPath.GetString();
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CTexEncryptDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CTexEncryptDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTexEncryptDlg::Init()
{
	SetDlgItemText( IDC_INPUT_EDIT, "" );
	SetDlgItemText( IDC_OUTPUT_EDIT, "" );
	m_cEdit.SetWindowText("");
}

void CTexEncryptDlg::OnBnClickedOpenButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Init();


	int nSelect = m_cCombo.GetCurSel();
	if ( nSelect < ENCRYPT_FILE || nSelect >= OPTION_SIZE )	return;

	switch ( nSelect )
	{
	case ENCRYPT_FILE :
		{
			OpenEncryptFile();
		}
		break;
	case ENCRYPT_FORDER :
		{
			OpenSelectForder();
		}
		break;
	case DECRYPT_FILE :
		{
			OpenDecryptFile();
		}
		break;
	case DECRYPT_FORDER :
		{
			OpenSelectForder();
		}
		break;
	}
}

void CTexEncryptDlg::OnBnClickedRunButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString strInput; 
	CString strOutput;
	
	GetDlgItemText( IDC_INPUT_EDIT, strInput );
	GetDlgItemText( IDC_OUTPUT_EDIT, strOutput );

	if ( strInput.IsEmpty() || strOutput.IsEmpty() )	return;
	
	int nSelect = m_cCombo.GetCurSel();
	if ( nSelect < ENCRYPT_FILE || nSelect >= OPTION_SIZE )	return;

	switch( nSelect )
	{
	case ENCRYPT_FILE :
		{
			if ( RunEncryptFile(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Encrypt File" );
				AddEditString( strInput );
			}

			
		}
		break;
	case ENCRYPT_FORDER :
		{
			if ( RunEncryptForder(strInput, strOutput) )
			{
				AddEditString( "Complate Encrypt File in Forder" );
			}
			else
			{
				AddEditString( "Complate Encrypt File in Forder" );
			}
			
		}
		break;
	case DECRYPT_FILE :
		{
			if ( RunDecryptFile(strInput, strOutput) )
			{
				AddEditString( "Complage Decrypt File" );
				AddEditString( strInput );
			}
			else
			{
				AddEditString( "Error Encrypt File" );
				AddEditString( strInput );
			}
			
		}
		break;
	case DECRYPT_FORDER :
		{
			if ( RunDecryptForder(strInput, strOutput) ) 
			{
				AddEditString( "Complage Decrypt File in Forder" );				
			}
			else
			{
				AddEditString( "Complage Decrypt File in Forder" );
			}
		}
		break;
	}
}

void CTexEncryptDlg::OpenEncryptFile()
{
	CString szFilterInput = "Texture File (*.dds,*.tga)|*.dds;*.tga|";

	CFileDialog dlgInput(TRUE,".dds",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterInput,this);
	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "Texture File (*.mtf)|*.mtf|";
	CFileDialog dlgOutput( FALSE,".mtf",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterOutPut,this );

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );
}

void CTexEncryptDlg::OpenDecryptFile()
{
	CString szFilterInput = "Texture File (*.mtf)|*.mtf|";
	CFileDialog dlgInput( TRUE,".mtf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilterInput,this );

	dlgInput.m_ofn.lpstrInitialDir = m_strAppPath;

	if ( dlgInput.DoModal() != IDOK ) return;
	
	CString strFileName = dlgInput.GetFileName();
	strFileName = strFileName.Left( strFileName.ReverseFind( '.' ) );

	CString szFilterOutPut = "Texture File (*.dds,*.tga)|*.dds;*.tga|";
	CFileDialog dlgOutput(FALSE,".dds",strFileName,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilterOutPut,this);			

	if ( dlgOutput.DoModal() != IDOK ) return;
		
	SetDlgItemText( IDC_INPUT_EDIT, dlgInput.GetPathName() );
	SetDlgItemText( IDC_OUTPUT_EDIT, dlgOutput.GetPathName() );
}

void CTexEncryptDlg::OpenSelectForder()
{
	BROWSEINFO biInput;
	ZeroMemory(&biInput, sizeof(BROWSEINFO));
	biInput.hwndOwner = m_hWnd; // 현재 핸들
	biInput.lpszTitle = "Select Input Forder"; // 폴더선택창 열었을때 보여지는 문구
	biInput.pidlRoot = NULL;
	biInput.lpfn = BrowseCallbackProc; // 콜백함수
	biInput.lParam = (LPARAM)(LPCSTR)m_strAppPath;
	biInput.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_NONEWFOLDERBUTTON | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlFolderInput = SHBrowseForFolder(&biInput);
	
	if (pidlFolderInput == NULL) return;

	TCHAR szPathInput[_MAX_PATH] = {0};
	SHGetPathFromIDList(pidlFolderInput, szPathInput);

	
	BROWSEINFO biOutput;
	ZeroMemory(&biOutput, sizeof(BROWSEINFO));
	biOutput.hwndOwner = m_hWnd; // 현재 핸들
	biOutput.lpszTitle = "Select Ouput Forder"; // 폴더선택창 열었을때 보여지는 문구
	biOutput.pidlRoot = NULL;
	biOutput.lpfn = BrowseCallbackProc; // 콜백함수
	biOutput.lParam = (LPARAM)(LPCSTR)szPathInput;
	biOutput.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_NONEWFOLDERBUTTON | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidlFolderOutput = SHBrowseForFolder(&biOutput);
	
	if (pidlFolderOutput == NULL) return;

	TCHAR szPathOutput[_MAX_PATH] = {0};
	SHGetPathFromIDList(pidlFolderOutput, szPathOutput);

	
	SetDlgItemText( IDC_INPUT_EDIT, szPathInput );
	SetDlgItemText( IDC_OUTPUT_EDIT, szPathOutput );

}


int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage( hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM) dwData );
		break;
	}
	return 0;
}


void CTexEncryptDlg::OnCbnSelchangeOptionCombo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Init();
}

BOOL CTexEncryptDlg::RunEncryptFile( const CString& strInput, const CString& strOutput )
{
	// 파일오픈
	FILE* pFileInput = NULL;
	pFileInput = fopen( strInput, "rb" );
	if ( !pFileInput )
	{
		return FALSE;
	}

	fseek ( pFileInput, 0, SEEK_END );
	int nSize = ftell ( pFileInput );

	fseek ( pFileInput, 0, SEEK_SET );	
	
	// 헤더추가	
	BYTE* pBuffer = new BYTE[TEX_HEADER_SIZE+nSize];

	int nVersion = TEX_VERSION;
	int nFileType = TEXTURE_DDS;

	CString strFileExt = strInput;

	strFileExt = strFileExt.Right( strFileExt.GetLength() - strFileExt.ReverseFind( '.' ) );
	strFileExt.MakeLower();

	if ( !strcmp( strFileExt, g_szFileExt[TEXTURE_DDS] ) )	nFileType = TEXTURE_DDS;
	else if ( !strcmp( strFileExt, g_szFileExt[TEXTURE_TGA] ) ) nFileType = TEXTURE_TGA;
	
	memcpy( pBuffer, &nVersion, sizeof( int ) );
	memcpy( pBuffer+4 , &nSize, sizeof(int) );
	memcpy( pBuffer+8, &nFileType, sizeof(int) );	
	
	// 붙이기
	fread( pBuffer+TEX_HEADER_SIZE, sizeof(BYTE), nSize, pFileInput );
	fclose( pFileInput );

	// XOR변환
	for ( int i = 0; i < nSize; ++i )
	{
		pBuffer[TEX_HEADER_SIZE+i] = pBuffer[TEX_HEADER_SIZE+i] ^ TEX_XOR_DATA;
		pBuffer[TEX_HEADER_SIZE+i] -= TEX_DIFF_DATA;
	}

	// 파일저장
	FILE* pFileOutput = NULL;
	pFileOutput = fopen( strOutput, "wb" );
	if ( !pFileOutput )
	{
		delete[] pBuffer;
		return FALSE;
	}
	
	fwrite( pBuffer, sizeof(BYTE), nSize+TEX_HEADER_SIZE, pFileOutput );
	fclose( pFileOutput );

	delete[] pBuffer;
	return TRUE;
}

BOOL CTexEncryptDlg::RunDecryptFile( const CString& strInput, const CString& strOutput )
{
	// 파일오픈
	FILE* pFileInput = NULL;
	pFileInput = fopen( strInput, "rb" );
	if ( !pFileInput )
	{
		return FALSE;
	}
	
	fseek ( pFileInput, 0, SEEK_END );
	int nSize = ftell ( pFileInput );

	fseek ( pFileInput, 0, SEEK_SET );	
	
	// 헤더읽기

	int nVersion = 0;
	int nReadSize = 0;
	int nFileType = 0;
	
	fread( &nVersion, sizeof(int), 1, pFileInput );
    fread( &nReadSize, sizeof(int), 1, pFileInput );
	fread( &nFileType, sizeof(int), 1, pFileInput );

	if ( nVersion != TEX_VERSION || nSize != nReadSize + TEX_HEADER_SIZE )
	{
		return FALSE;
	}	
	
	// 내용읽기
	BYTE* pBuffer = new BYTE[nReadSize];

	fread( pBuffer, sizeof(BYTE), nReadSize, pFileInput );
	fclose( pFileInput );	

	// XOR변환
	for ( int i = 0; i < nReadSize; ++i )
	{
		pBuffer[i] += TEX_DIFF_DATA;
		pBuffer[i] = pBuffer[i] ^ TEX_XOR_DATA;		
	}

	CString strOutFile = strOutput;
	strOutFile = strOutFile.Left( strOutFile.ReverseFind( '.' ) );
	strOutFile += g_szFileExt[nFileType];

	// 파일저장
	FILE* pFileOutput = NULL;
	pFileOutput = fopen( strOutFile, "wb" );
	if ( !pFileOutput )
	{
		delete[] pBuffer;
		return FALSE;
	}
	
	fwrite( pBuffer, sizeof(BYTE), nReadSize, pFileOutput );
	fclose( pFileOutput );

	delete[] pBuffer;
	return TRUE;
}

BOOL CTexEncryptDlg::RunEncryptForder( const CString& strInput, const CString& strOutput )
{
	int nCntFile=0, nCntSuccess = 0;

	AddEditString( "Run Encrypt File in Forder" );
	AddEditString( strInput );

	for ( int i = 0; i < TEXTURE_TYPE; ++i )
	{
		CString strFind = strInput;
		strFind += "\\*";
		strFind += g_szFileExt[i];

		CString strOutFile, strOutPath;
		strOutPath = strOutput;
		strOutPath += "\\";

		CString strInFile, strInPath;
		strInPath = strInput;
		strInPath += "\\";

		CString strFile;
		
		CFileFind finder;	
		BOOL bFind ( TRUE );

		if ( !finder.FindFile( strFind ) )
		{
			continue;
		}

		CString strTemp;

		while( bFind )
		{	
			nCntFile++;

			bFind = finder.FindNextFile();
			strFile = finder.GetFileName();
			strInFile = strInPath;
			strInFile += strFile;
			strOutFile = strOutPath;
			strOutFile += strFile.Left( strFile.ReverseFind('.'));
			strOutFile += ".mtf";
			
			if ( !RunEncryptFile( strInFile, strOutFile ) )
			{
//				CDebugSet::ToLogFile( "Texture File Encrypt Error : %s", strInFile );
				strTemp.Format( " Error Encrypt File : %s ", strFile );
				AddEditString( strTemp );
				continue;
			}
			
			strTemp.Format( " Success Encrypt File : %s ", strFile );
			AddEditString( strTemp );

			nCntSuccess++;
		}
	}

	CString strResult;
	strResult.Format( "%d in %d File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

BOOL CTexEncryptDlg::RunDecryptForder( const CString& strInput, const CString& strOutput )
{
	AddEditString( "Run Decrypt File in Forder" );
	AddEditString( strInput );

	// DDS FILE 변환
	CString strFind = strInput;
	strFind += "\\*.mtf";

	CString strOutFile, strOutPath;
	strOutPath = strOutput;
	strOutPath += "\\";

	CString strInFile, strInPath;
	strInPath = strInput;
	strInPath += "\\";

	CString strFile;
	
	CFileFind finder;	
	BOOL bFind ( TRUE );

	if ( !finder.FindFile( strFind ) )
	{
		bFind = FALSE;
	}

	int nCntFile =0, nCntSuccess = 0;
	CString strTemp;

	while( bFind )
	{
		nCntFile++;
		bFind = finder.FindNextFile();
		strFile = finder.GetFileName();
		strInFile = strInPath;
		strInFile += strFile;
		strOutFile = strOutPath;
		strOutFile += strFile.Left( strFile.ReverseFind('.'));
		strOutFile += ".dds";
		
		if ( !RunDecryptFile( strInFile, strOutFile ) )
		{
//			CDebugSet::ToLogFile( "Texture File Decrypt Error : %s", strInFile );	
			strTemp.Format( " Error Decrypt File : %s ", strFile );
			AddEditString( strTemp );
			continue;
		}
		
		strTemp.Format( " Success Decrypt File : %s ", strFile );
		AddEditString( strTemp );

		nCntSuccess++;
	}

	CString strResult;
	strResult.Format( "%d in %d File Encrypt Success", nCntSuccess, nCntFile );
	AddEditString( strResult );

	return TRUE;
}

void CTexEncryptDlg::AddEditString( const char* szInfo )
{
    int len = m_cEdit.GetWindowTextLength();

	m_cEdit.SetSel( len, len );
	m_cEdit.ReplaceSel( szInfo );	

	len = m_cEdit.GetWindowTextLength();
	m_cEdit.SetSel( len, len );
	m_cEdit.ReplaceSel( "\r\n" );

	return;
}
