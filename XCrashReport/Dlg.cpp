// Dlg.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XCrashReport.h"
#include "Dlg.h"
#include "MiniVersion.h"
#include <io.h>
#include "SendEmail.h"
#include "XZip.h"
#include "EmailDefines.h"
#include "CrashFileNames.h"
#include "BackupFile.h"
#include "GetFileSizeAsString.h"
#include "WriteRegistry.h"
#include "RegistryDefines.h"
#include "IniDefines.h"
#include "RanFtp.h"
#include <fstream>
#include <mapi.h>
#include "imapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)   // From WINUSER.H
#endif

#define MAX_USER_COMMENTS_SIZE	(64 * 1024)

/////////////////////////////////////////////////////////////////////////////
// CDlg dialog

BEGIN_MESSAGE_MAP(CDlg, CDialog)
	//{{AFX_MSG_MAP(CDlg)
	ON_BN_CLICKED(IDC_SEND_ERROR, OnSend)
	ON_BN_CLICKED(IDC_DO_NOT_SEND_ERROR, OnDoNotSend)
	ON_BN_CLICKED(IDC_APP_ICON, OnAppIcon)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP

	// this message is sent by XHyperLink
	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnClickHere)

END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CDlg::CDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strZipFile  = _T("");
	m_nFilesInZip = 0;
	m_bOverIcon   = FALSE;
	m_hLinkCursor = NULL;
	m_hPrevCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CDlg::~CDlg()
{
	for (int i = 0; i < m_aFileDetails.GetSize(); i++)
	{
		FILEDETAILS *fd = (FILEDETAILS *) m_aFileDetails[i];
		if (fd)
			delete fd;
	}

	if (m_hLinkCursor)
		DestroyCursor(m_hLinkCursor);
	m_hLinkCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg)
	DDX_Control(pDX, IDC_WHAT, m_What);
	DDX_Control(pDX, IDC_CLICK_HERE, m_ClickHere);
	//DDX_Control(pDX, IDC_PLEASE_TELL_US, m_PleaseTellUs);
	DDX_Control(pDX, IDC_BANNER, m_Banner);
	DDX_Control(pDX, IDC_APP_ICON, m_Icon);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// get our path name - we assume that crashed exe is in same directory
	ZeroMemory(m_szModulePath, sizeof(m_szModulePath));
	::GetModuleFileName(NULL, m_szModulePath, _countof(m_szModulePath)-2);
	TCHAR *cp = _tcsrchr(m_szModulePath, _T('\\'));
	if (*cp)
	{
		*(cp+1) = _T('\0');		// remove file name
	}

	InitializeDisplay();

	GetRegistryDetails();

	//GetFileDetails();

	//m_nFilesInZip = ZipFiles();

	LoadHandCursor();

	SetTimer(1, 80, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
// InitializeDisplay
void CDlg::InitializeDisplay()
{
	m_Banner.SetTextColor(RGB(0,0,0));
	m_Banner.SetBackgroundColor(RGB(255,255,255));
	m_Banner.SetBold(TRUE);

	// get path to exe that crashed - we assume it is in same directory
	CString strExe = m_szModulePath;
	strExe += theApp.m_pszModule;

	// get version resource
	CMiniVersion ver(strExe);
	TCHAR szBuf[1000];
	szBuf[0] = _T('\0');

	// try to get description
	ver.GetFileDescription(szBuf, _countof(szBuf)-2);

	// if that failed try to get product name
	if (szBuf[0] == _T('\0'))
		ver.GetProductName(szBuf, _countof(szBuf)-2);

	ver.Release();

	// if that failed just use exe name
	if (szBuf[0] == _T('\0'))
		_tcscpy(szBuf, theApp.m_pszModule);

	SetWindowText(szBuf);

	CString strBanner(szBuf), strMsg;
	strMsg.LoadString( IDS_STRING104 );
	strBanner += ' ' + strMsg; // 배너 메세지 : 모듈 이름 + 메세지

	m_Banner.SetWindowText(strBanner);
	m_Banner.SetMargins(22, 8);

	// get icon of crashed app
	HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), strExe, 0);

	m_Icon.SetBackgroundColor(RGB(255,255,255), FALSE);
	if (hIcon)
		m_Icon.SetIcon(hIcon, FALSE);

	//m_PleaseTellUs.SetBold(TRUE);

	m_What.SetLimitText(128*1024);	// set edit box to 128 KB
	CString strInitMsg( "ID : \r\nPhone : \r\nE-Mail : " );
	m_What.SetWindowText( strInitMsg );

    m_ClickHere.SetColours(m_ClickHere.GetLinkColour(),
						   RGB(0,0,255), 
						   m_ClickHere.GetHoverColour());
	m_ClickHere.EnableURL(FALSE);
	m_ClickHere.EnableTooltip(FALSE);
	m_ClickHere.SetNotifyParent(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
// GetFileDetails
void CDlg::GetFileDetails()
{
	// first try to read file details from INI file - 
	// INI file has format:
	//     [FilesToAdd]
	//     File001=<file-path>,<description>,<file-type>
	//     File002=<file-path>,<description>,<file-type>
	//     etc.

	TCHAR szBuf[2000];
	CString strEntry       = _T("");
	CString strPath        = _T("");
	CString strDescription = _T("");
	CString strType        = _T("");
	CString strFileSize    = _T("");

	CString strIniPath = m_szModulePath;
	strIniPath += INI_FILE_NAME;

	int nCount = 0;

	for (int n = 0; n <= MAX_INI_ITEMS; n++)
	{
		szBuf[0]       = _T('\0');
		strPath        = _T("");
		strDescription = _T("");
		strType        = _T("");
		strFileSize    = _T("");

		strEntry.Format(INI_FILE_TEMPLATE, n);

		::GetPrivateProfileString(INI_FILE_SECTION, 
								  strEntry, 
								  _T(""), 
								  szBuf, 
								  _countof(szBuf)-10,
								  strIniPath);

		if (szBuf[0] == _T('\0'))
			break;

		// get path
		TCHAR *delims = _T(",\r\n");
		TCHAR *cp = _tcstok(szBuf, delims);
		if (!cp)
		{
			TRACE0(_T("ERROR - bad file entry\n"));
			break;
		}
		strPath = cp;

		if( strEntry == "File000" )
		{
			m_strLocalType = strPath;
			continue;
		}

		// get description
		cp = _tcstok(NULL, delims);
		if (!cp)
		{
			TRACE0(_T("ERROR - bad file entry\n"));
			break;
		}
		strDescription = cp;

		// get type
		cp = _tcstok(NULL, delims);
		if (!cp)
		{
			TRACE0(_T("ERROR - bad file entry\n"));
			break;
		}
		strType = cp;

		if (strPath.IsEmpty())
			continue;

		CString strFilePath = strPath;
		if (strPath.Find(_T('\\')) == -1)
		{
			// no path, so use exe's directory
			strFilePath = m_szModulePath;
			strFilePath += strPath;
		}

		if (!GetFileSizeAsString(strFilePath, strFileSize))
			continue;

		TRACE1(_T("adding file %s from ini\n"), strFilePath);

		FILEDETAILS *fd    = new FILEDETAILS;
		ASSERT(fd);
		fd->strFilePath    = strFilePath;
		fd->strDescription = strDescription;
		fd->strType        = strType;
		fd->strFileSize    = strFileSize;

		m_aFileDetails.Add((DWORD) fd);

		nCount++;
	}

	// if there are no files, try to add the default files -
	// these names are defined in CrashFileNames.h.
	if (nCount == 0)
	{
		TRACE0(_T("no files added from ini, using defaults\n"));

		CString strFilePath = m_szModulePath;
		strFilePath += XCRASHREPORT_ERROR_LOG_FILE;
		if (_taccess(strFilePath, 00) == 0)
		{
			if (GetFileSizeAsString(strFilePath, strFileSize))
			{
				TRACE1(_T("adding default file %s\n"), strFilePath);
				FILEDETAILS *fd    = new FILEDETAILS;
				ASSERT(fd);
				fd->strFilePath    = strFilePath;
				fd->strDescription = _T("Crash log");
				fd->strType        = _T("Text Document");
				fd->strFileSize    = strFileSize;
				m_aFileDetails.Add((DWORD) fd);
			}
		}

		strFilePath = m_szModulePath;
		strFilePath += XCRASHREPORT_MINI_DUMP_FILE;
		if (_taccess(strFilePath, 00) == 0)
		{
			if (GetFileSizeAsString(strFilePath, strFileSize))
			{
				TRACE1(_T("adding default file %s\n"), strFilePath);
				FILEDETAILS *fd    = new FILEDETAILS;
				ASSERT(fd);
				fd->strFilePath    = strFilePath;
				fd->strDescription = _T("Crash Dump");
				fd->strType        = _T("DMP File");
				fd->strFileSize    = strFileSize;
				m_aFileDetails.Add((DWORD) fd);
			}
		}

		strFilePath = m_szModulePath;
		strFilePath += XCRASHREPORT_DESCRIPTION_FILE;
		if (_taccess(strFilePath, 00) == 0)
		{
			if (GetFileSizeAsString(strFilePath, strFileSize))
			{
				TRACE1(_T("adding default file %s\n"), strFilePath);
				FILEDETAILS *fd    = new FILEDETAILS;
				ASSERT(fd);
				fd->strFilePath    = strFilePath;
				fd->strDescription = _T("Crash Description");
				fd->strType        = _T("Text Document");
				fd->strFileSize    = strFileSize;
				m_aFileDetails.Add((DWORD) fd);
			}
		}

		strFilePath = m_szModulePath;
		strFilePath += XCRASHREPORT_DXDIAG_FILE;
		if (_taccess(strFilePath, 00) == 0)
		{
			if (GetFileSizeAsString(strFilePath, strFileSize))
			{
				TRACE1(_T("adding default file %s\n"), strFilePath);
				FILEDETAILS *fd    = new FILEDETAILS;
				ASSERT(fd);
				fd->strFilePath    = strFilePath;
				fd->strDescription = _T("Crash Dxdiag");
				fd->strType        = _T("Text Document");
				fd->strFileSize    = strFileSize;
				m_aFileDetails.Add((DWORD) fd);
			}
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < m_aFileDetails.GetSize(); i++)
	{
		FILEDETAILS *fd = (FILEDETAILS *) m_aFileDetails[i];
		TRACE2(_T("fd[%d] = %s\n"), i, fd->strFilePath);
	}
#endif

}

///////////////////////////////////////////////////////////////////////////////
// GetRegistryDetails
void CDlg::GetRegistryDetails()
{
	TRACE0(_T("in CDlg::GetRegistryDetails\n"));

#ifdef XCRASHREPORT_DUMP_REGISTRY

	// first try to read registry details from INI file - 
	// INI file has format:
	//     [RegistryToAdd]
	//     Registry001=<registry-path>,<description>
	//     Registry002=<registry-path>,<description>
	//     etc.
	//
	// For each entry, registry file RegistryNNN.txt
	// will be created.

	TCHAR szBuf[2000];
	CString strEntry       = _T("");
	CString strPath        = _T("");
	CString strDescription = _T("");
	CString strType        = _T("Text Document");	// default for reg files
	CString strFileSize    = _T("");
	CString strFilePath    = _T("");

	CString strIniPath = m_szModulePath;
	strIniPath += INI_FILE_NAME;

	int nCount = 0;

	for (int n = 1; n <= MAX_INI_ITEMS; n++)
	{
		szBuf[0]       = _T('\0');
		strPath        = _T("");
		strDescription = _T("");
		strFileSize    = _T("");

		strEntry.Format(INI_REG_TEMPLATE, n);

		::GetPrivateProfileString(INI_REG_SECTION, 
								  strEntry, 
								  _T(""), 
								  szBuf, 
								  _countof(szBuf)-10,
								  strIniPath);

		if (szBuf[0] == _T('\0'))
			break;

		// get registry path
		TCHAR *delims = _T(",\r\n");
		TCHAR *cp = _tcstok(szBuf, delims);
		if (!cp)
		{
			TRACE0(_T("ERROR - bad registry entry\n"));
			break;
		}

		strPath = cp;

		if (strPath.IsEmpty())
			continue;

		// get description
		cp = _tcstok(NULL, delims);
		if (!cp)
		{
			TRACE0(_T("ERROR - bad registry entry\n"));
			break;
		}

		strDescription = cp;

		// dump registry to file - note we do not use ".reg"
		// because we don't want people double-clicking it
		// into the registry
		CString strTemp = _T("");
		strTemp.Format(INI_REG_TEMPLATE, n);
		strFilePath.Format(_T("%s%s.txt"),
			m_szModulePath, strTemp);

		if (!WriteRegistryTreeToFile(strPath, strFilePath))
		{
			TRACE1(_T("ERROR - Could not write registry hive %s\n"), strPath);
			continue;
		}

		if (!GetFileSizeAsString(strFilePath, strFileSize))
			continue;

		TRACE1(_T("adding reg file %s from ini\n"), strFilePath);

		FILEDETAILS *fd    = new FILEDETAILS;
		ASSERT(fd);
		fd->strFilePath    = strFilePath;
		fd->strDescription = strDescription;
		fd->strType        = strType;
		fd->strFileSize    = strFileSize;

		m_aFileDetails.Add((DWORD) fd);

		nCount++;
	}

	// if there are no files, try to add the default reg entry -
	// this reg path is defined in RegistryDefines.h.
	if (nCount == 0)
	{
		TRACE0(_T("no registry files added from ini, using default\n"));

		// dump registry to file
		CString strKey = XCRASHREPORT_REGISTRY_KEY;
		if (!strKey.IsEmpty())
		{
			CString strFile = XCRASHREPORT_REGISTRY_DUMP_FILE;
			if (!strFile.IsEmpty())
			{
				strFilePath = m_szModulePath;
				strFilePath += strFile;

				if (!WriteRegistryTreeToFile(strKey, strFilePath))
				{
					TRACE1(_T("ERROR - Could not write registry hive %s\n"), 
						strKey);
				}
				else
				{
					if (GetFileSizeAsString(strFilePath, strFileSize))
					{
						TRACE1(_T("adding default reg file %s\n"), 
							strFilePath);
						FILEDETAILS *fd    = new FILEDETAILS;
						ASSERT(fd);
						fd->strFilePath    = strFilePath;
						fd->strDescription = _T("Registry File");
						fd->strType        = strType;
						fd->strFileSize    = strFileSize;
						m_aFileDetails.Add((DWORD) fd);
					}
				}
			}
		}
	}

#ifdef _DEBUG
	for (int i = 0; i < m_aFileDetails.GetSize(); i++)
	{
		FILEDETAILS *fd = (FILEDETAILS *) m_aFileDetails[i];
		TRACE2(_T("fd[%d] = %s\n"), i, fd->strFilePath);
	}
#endif

#endif	// XCRASHREPORT_DUMP_REGISTRY

}

///////////////////////////////////////////////////////////////////////////////
// ZipFiles
int CDlg::ZipFiles()
{
	int nFiles = 0;

	// generate zip file name
	m_strZipFile = m_szModulePath;
	m_strZipFile += theApp.m_pszModule;
	int index = m_strZipFile.ReverseFind(_T('.'));
	if (index > 0)
		m_strZipFile = m_strZipFile.Left(index);
	m_strZipFile += _T(".zip");
	TRACE1(_T("m_strZipPath = <%s>\n"), m_strZipFile);

	// backup old zip if it exists
	BackupFile(m_strZipFile);

	HZIP hZip = NULL;

	for (int i = 0; i < m_aFileDetails.GetSize(); i++)
	{
		FILEDETAILS *fd = (FILEDETAILS *) m_aFileDetails[i];

		if (fd)
		{
			// make sure file exists
			if ((_taccess(fd->strFilePath, 00) == 0) && fd->bSend)
			{
				// if this is first file, create zip now
				if (!hZip)
				{
					hZip = CreateZip((LPVOID)(LPCTSTR)m_strZipFile, 0, ZIP_FILENAME);

					if (!hZip)
					{
						TRACE1(_T("ERROR - failed to create %s\n"), m_strZipFile);
						break;
					}
				}

				// zip is open, add file

				CString strName = fd->strFilePath;
				int index = strName.ReverseFind(_T('\\'));
				if (index > 0)
					strName = strName.Right(strName.GetLength()-index-1);
				TRACE1(_T("strName=<%s>\n"), strName);

				ZRESULT zr = ZipAdd(hZip, 
									strName, 
									(LPVOID)(LPCTSTR)fd->strFilePath, 
									0, 
									ZIP_FILENAME);
				if (zr == ZR_OK)
				{
					nFiles++;
				}
				else
				{
					TRACE1(_T("ERROR - failed to add '%s' to zip\n"), fd->strFilePath);
				}
			}
		}
	}

	if (hZip)
		CloseZip(hZip);

	return nFiles;
}

///////////////////////////////////////////////////////////////////////////////
// OnClickHere
LRESULT CDlg::OnClickHere(WPARAM, LPARAM)
{
	if (m_aFileDetails.GetSize() > 0)
	{
		CDetails dlg(&m_aFileDetails);
		dlg.DoModal();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnSend
void CDlg::OnSend() 
{
	TCHAR szSubject[1000];
	memset(szSubject, 0, sizeof(szSubject));
	_sntprintf(szSubject, _countof(szSubject)-2, 
		_T("Error report for %s"), theApp.m_pszModule);

	TCHAR szMessage[MAX_USER_COMMENTS_SIZE];
	memset(szMessage, 0, sizeof(szMessage));
	m_What.GetWindowText(szMessage, _countof(szMessage)-2);
	
	// By 경대
	//
	if( szMessage[0] != _T('\0') )
	{
		std::ofstream oFile;
		CString strDesFile( m_szModulePath );
		strDesFile += "DESCRIPTION.TXT";
		oFile.open( strDesFile.GetString() );

		if ( oFile )
		{
			oFile << szMessage;
			oFile.close();
		}
	}

	TCHAR szAttachmentPath[MAX_PATH*2];
	memset(szAttachmentPath, 0, sizeof(szAttachmentPath));

	// Dxdiag 파일을 생성한다.
	BeginWaitCursor();
	{
		MakeDxdiagTxt();
	}
	EndWaitCursor();

	GetFileDetails();

	// create new zip file, in case user unchecked some files
	m_nFilesInZip = ZipFiles();

	if (m_nFilesInZip)
	{
		_tcsncpy(szAttachmentPath, m_strZipFile, _countof(szAttachmentPath)-2);
	}
	else
	{
		// no files in zip - check if there is a message
		if (szMessage[0] == _T('\0'))
		{
			AfxMessageBox(_T("Please select a file to send, or enter  \r\n")
						  _T("a message describing the problem."));
			return;
		}
	}

	//BOOL bRet = FALSE;

	//CIMapi mail;

	//if ( mail.Error() == CIMapi::IMAPI_SUCCESS )
	//{
	//	mail.To(XCRASHREPORT_SEND_TO_ADDRESS);		//  Set recipient name (me)
	//	//mail.To("someoneelse@somewhereelse.com");	//  Second recipient
	//	//mail.Cc("cc@cc.com");						//  CC recipient
	//	//mail.From("user@somewhere.com");			//  Identify sender (not strictly 
	//												//  necessary since
	//												//  MAPI will fill this in for you)
	//	mail.Subject(szSubject);					//  Subject of this email
	//	mail.Attach(szAttachmentPath);				//  Attaching a file
	//	//mail.Attach("someotherfile", "different_name_for_recipient");
	//												// Attach another file but give it
	//												// a different name inside the 
	//												// email itself

	//	// Put text of message in body
	//	mail.Text(szMessage);						//  Set body text
	//	bRet = mail.Send();							//  Now send the mail! 
	//}
	
	CRanFtp RanFtp;

	if ( !RanFtp.Connect("RanFtpReport", "59.25.186.109", INTERNET_DEFAULT_FTP_PORT,
						"anonymous", "anonymous@ran-online.co.kr") )
	{
		// error - tell user to send file
		CString strMsg, strTemp;
		strMsg.LoadString( IDS_STRING102 );
		strTemp = strMsg;
		strMsg.LoadString( IDS_STRING103 );
		strTemp += ' ' + strMsg;

		TCHAR szMsg[2000];
		memset(szMsg, 0, sizeof(szMsg));
		_sntprintf( szMsg, _countof(szMsg)-2, _T("%s\r\n\r\nFile : %s\r\nE-Mail : %s"),
					strTemp.GetString(), szAttachmentPath, XCRASHREPORT_SEND_TO_ADDRESS);
		AfxMessageBox(szMsg);
	}
	else
	{
		// FTP 의 최초 접속 ROOT 디렉토리 위치
		CString strFirstDir;
		RanFtp.GetCurrentDirectory( strFirstDir );

		// 날짜와 시간 얻기
		CString strNewFolder, strServerTime;
		//
		BOOL bRet = RanFtp.GetServerTime("time.kriss.re.kr");
		if ( bRet )
		{
			bRet = RanFtp.GetServerDateString( strNewFolder );
			if ( bRet )
			{
				bRet = RanFtp.GetServerTimeString( strServerTime );
				if ( bRet )
				{
					// 폴더 생성
					RanFtp.CreateDirectory( strNewFolder );

					CString strCurrentDir = strFirstDir + strNewFolder;
					RanFtp.SetCurrentDirectory( strCurrentDir.GetString() );

					// IP
					CString strIPAdress;
					RanFtp.GetIPAdress( strIPAdress );
					
					// 파일 이름 생성
					CString strRemoteName;
					if( !m_strLocalType.IsEmpty() )
					{
						strRemoteName.Format( "%sI%sT%s%s", m_strLocalType, strIPAdress.GetString(),
												strServerTime.GetString(), _T(".zip") );
					}
					else
					{
						strRemoteName.Format( "I%sT%s%s", strIPAdress.GetString(),
												strServerTime.GetString(), _T(".zip") );
					}

					bRet = RanFtp.PutFile( szAttachmentPath, strRemoteName.GetString() );	
				}
			}
		}

		if ( !bRet )
		{
			// error - tell user to send file
			TCHAR szMsg[2000]={0};
			_sntprintf(szMsg, _countof(szMsg)-2, _T("The file uploading failed.\r\n")
						_T("Sending file '%s' which used by email '%s' to us."),
						szAttachmentPath, XCRASHREPORT_SEND_TO_ADDRESS);
			AfxMessageBox(szMsg);
		}
		
		RanFtp.DisConnect();
	}

	CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// OnDoNotSend
void CDlg::OnDoNotSend() 
{
	CDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////
// OnCancel
void CDlg::OnCancel() 
{
	// do not let Cancel close the dialog	
	//CDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////
// OnOK
void CDlg::OnOK() 
{
	// do not let Enter close the dialog	
	//CDialog::OnOK();
}

///////////////////////////////////////////////////////////////////////////////
// OnAppIcon
void CDlg::OnAppIcon() 
{
	CString strExe = m_szModulePath;
	strExe += theApp.m_pszModule;

	SHELLEXECUTEINFO sei;

	ZeroMemory(&sei,sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = strExe;
	sei.lpVerb = _T("properties");
	sei.fMask  = SEE_MASK_INVOKEIDLIST;
	sei.nShow  = SW_SHOWNORMAL;

	ShellExecuteEx(&sei); 
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CDlg::OnTimer(UINT /*nIDEvent*/) 
{
	CPoint point(GetMessagePos());
	ScreenToClient(&point);

	CRect rect;
	m_Icon.GetWindowRect(&rect);
	ScreenToClient(&rect);

	// check if cursor is over icon
	if (!m_bOverIcon && rect.PtInRect(point))
	{
		m_bOverIcon = TRUE;
		if (m_hLinkCursor)
			m_hPrevCursor = ::SetCursor(m_hLinkCursor);
	}
	else if (m_bOverIcon && !rect.PtInRect(point))
	{
		m_bOverIcon = FALSE;
		if (m_hPrevCursor)
			::SetCursor(m_hPrevCursor);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnSetCursor
BOOL CDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bOverIcon && m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////////////////////////////////
// LoadHandCursor
void CDlg::LoadHandCursor()
{
	if (m_hLinkCursor == NULL)				// No cursor handle - try to load one
	{
		// First try to load the Win98 / Windows 2000 hand cursor

		TRACE0(_T("loading from IDC_HAND\n"));
		m_hLinkCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);

		if (m_hLinkCursor == NULL)			// Still no cursor handle - 
											// load the WinHelp hand cursor
		{
			// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
			// It loads a "hand" cursor from the winhlp32.exe module.

			TRACE0(_T("loading from winhlp32\n"));

			// Get the windows directory
			CString strWndDir;
			GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
			strWndDir.ReleaseBuffer();

			strWndDir += _T("\\winhlp32.exe");

			// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
			HMODULE hModule = LoadLibrary(strWndDir);
			if (hModule) 
			{
				HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
				if (hHandCursor)
					m_hLinkCursor = CopyCursor(hHandCursor);
				FreeLibrary(hModule);
			}
		}
	}
}

// Dxdiag 파일 생성 By 경대
//
bool CDlg::MakeDxdiagTxt()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	char szCmdLine[2048]={0};
	wsprintf(szCmdLine, "dxdiag.exe /t %s\\DXDIAG.TXT", m_szModulePath );

	// Start the child process. 
	if( !CreateProcess( NULL, // No module name (use command line). 
		szCmdLine,		  // Command line. 
		NULL,             // Process handle not inheritable. 
		NULL,             // Thread handle not inheritable. 
		FALSE,            // Set handle inheritance to FALSE. 
		CREATE_NO_WINDOW, // No creation flags. 
		NULL,             // Use parent's environment block. 
		NULL,             // Use parent's starting directory. 
		&si,              // Pointer to STARTUPINFO structure.
		&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
	{
		//ErrorExit( "CreateProcess failed." );
		// UPX 실행 실패
		return false;
	}

	// Wait until child process exits.
	WaitForSingleObject( pi.hProcess, INFINITE );

	// Close process and thread handles. 
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );

	return true;
}