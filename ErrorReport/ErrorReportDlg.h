// ErrorReportDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include <msxml2.h>
#include "./tinyxml/tinyxml.h"
#include "./export/StackEntry.h"
#include "afxwin.h"


class CBaseProcessor;


// CErrorReportDlg 대화 상자
class CErrorReportDlg : public CDialog
{
// 생성
public:
	CErrorReportDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_ERRORREPORT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원

public:
	BOOL OpenFolderDialog( CString& strFolderPath );

	void ReportResearch( CString& strDumpFilePath, CString& strMabPdpPath );
	BOOL LoadXmlDocument( CString& strLogFile );
	void StackTraceList();
	void UpdateStackEntryFromMap(CStackEntry& rStackEntry);

	boost::shared_ptr<CBaseProcessor> GetModuleInfo(const CString& strModule);

	TiXmlElement* GetChildElement( TiXmlElement* rootElem, CString& strChildPath );
	void GetStackEntry( TiXmlElement* rootElem, CStackEntry& rStackEntry );
	bool GetXMLNodeText(TiXmlElement* rootElem, CString& strChildPath, CString& strNodeText );


	static bool FindFileByPattern(PCTSTR pszMapPdbFolder, PCTSTR pszModuleName, PCTSTR pszFileExt, PTSTR pszFileName);



	void ClearTree();
	void AddTree();
	HTREEITEM GetTreeChildItem( HTREEITEM hParentItem, CString& strItem );


	TiXmlElement* m_pXMLNodeProcess;


	typedef std::map<CString, boost::shared_ptr<CBaseProcessor> > CModuleMap;
	
	std::list<CStackEntry> m_stackList;
	boost::regex m_rxFunctionName;	
	CModuleMap m_mapModules;	
	boost::shared_ptr<CBaseProcessor> m_pPdbProcessor;

public:
	TiXmlDocument m_XmlDoc;
	HTREEITEM		m_TreeRoot;


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ctrTree;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedOk();
	CEdit m_txtMapPdbFolder;
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData);
