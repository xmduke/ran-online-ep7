// ProgressListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ServerManager.h"
#include "ProgressListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressListCtrl

CProgressListCtrl::CProgressListCtrl()
{
}

CProgressListCtrl::~CProgressListCtrl()
{
	CProgressEntry* pProgEntry	= 0;
	int				Index		= 0;
	POSITION		pos			= m_ProgEntries.GetStartPosition();
	while (pos != NULL) 
	{
		m_ProgEntries.GetNextAssoc(pos, Index, pProgEntry);
		if (pProgEntry) 
			delete pProgEntry;
	}
	m_ProgEntries.RemoveAll();
}


BEGIN_MESSAGE_MAP(CProgressListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CProgressListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//}}AFX_MSG_MAP	
	ON_NOTIFY(HDN_ITEMCHANGEDA, 0, OnHdnItemchanged)
	ON_NOTIFY(HDN_ITEMCHANGEDW, 0, OnHdnItemchanged)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressListCtrl message handlers

void CProgressListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{	
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW) pNMHDR;

	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT: // Requested notification
		if (lplvcd->nmcd.dwItemSpec)
		{
			lplvcd->clrText	= RGB(  0,   0,   0); // black

			switch (lplvcd->nmcd.lItemlParam)
			{
			case COLOR_RED :
				lplvcd->clrTextBk = RGB(255,   0,   0); // red
				break;
			case COLOR_GREEN :
				lplvcd->clrTextBk = RGB(  0, 255,   0); // green
				break;
			case COLOR_BLUE :
				lplvcd->clrTextBk = RGB(  0,   0, 255); // blue
				break;			
			case COLOR_BLACK :
				lplvcd->clrTextBk = RGB(  0,   0,   0); // black
				break;
			case COLOR_YELLOW :
				lplvcd->clrTextBk = RGB(255, 255,   0); // yellow
				break;
			case COLOR_WHITE :
			default :
				lplvcd->clrTextBk = RGB(255, 255,   255); // white
				break;
			}
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CProgressListCtrl::CreateColumns()
{
	LVCOLUMN Col;
	char pColStr[101];

	InitHdr();
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 125;
	sprintf(pColStr, "상태");
	Col.pszText		= pColStr;
	Col.cchTextMax	= 100;
	InsertColumn(0, &Col);

	Col.mask			|= LVCF_SUBITEM;
	Col.iSubItem		= 1;
	sprintf(pColStr, "종류");
	InsertColumn(1, &Col);

	Col.iSubItem		= 2;
	sprintf(pColStr, "이름");
	InsertColumn(2, &Col);

	Col.iSubItem		= 3;
	sprintf(pColStr, "주소");
	InsertColumn(3, &Col);

	Col.iSubItem		= 4;
	sprintf(pColStr, "포트");
	InsertColumn(4, &Col);

	Col.iSubItem		= 5;
	sprintf(pColStr, "접속자");
	InsertColumn(5, &Col);

	Col.iSubItem		= 6;
	sprintf(pColStr, "Progress");
	InsertColumn(6, &Col);
}

void CProgressListCtrl::CreateProgress(int Index)
{
	// can only create progress for an existing item
	if (Index >= GetItemCount()) 
		return;
	CProgressEntry* ProgEntry = new CProgressEntry(Index, PROGRESS_INDEX);
	CRect ItemRect;
	GetSubItemRect(Index, ProgEntry->m_SubIndex, LVIR_BOUNDS, ItemRect);
	
	(ProgEntry->m_Prog).Create(PBS_SMOOTH | WS_CHILD | WS_VISIBLE, ItemRect, this, 1);
	(ProgEntry->m_Prog).SetRange(0, 100);
	(ProgEntry->m_Prog).SetPos(0);
	m_ProgEntries[Index] = ProgEntry;
}

void CProgressListCtrl::InsertServer(int nState, 
									 const char* szType, 
									 const char* szName,
									 const char* szIP,
									 int nPort,
									 int nCurClient, 
									 int nMaxClient, 
									 int nColor)
{
	int nIndex = GetItemCount();
	
	char pItemText[101];
	
	LVITEM Item;

	sprintf(pItemText, "%d", nState);
	Item.mask		= LVIF_TEXT | LVIF_PARAM;
	Item.iItem		= nIndex;
	Item.iSubItem	= 0;
	Item.pszText	= pItemText;
	Item.cchTextMax	= 100;
	Item.lParam		= nColor;

	InsertItem(&Item);

	sprintf(pItemText, "%s", szType);
	SetItemText(nIndex, 1, pItemText);

	sprintf(pItemText, "%s", szName);
	SetItemText(nIndex, 2, pItemText);
	
	sprintf(pItemText, "%s", szIP);
	SetItemText(nIndex, 3, pItemText);

	sprintf(pItemText, "%d", nPort);
	SetItemText(nIndex, 4, pItemText);

	sprintf(pItemText, "%d/%d", nCurClient, nMaxClient);
	SetItemText(nIndex, 5, pItemText);

	if (nMaxClient != 0) 
	{
		CreateProgress(nIndex);
		SetProgress(nIndex, (int) (nCurClient*100/nMaxClient));	
	}
	else
	{
		CreateProgress(nIndex);
		SetProgress(nIndex, 1);
	}
}

void CProgressListCtrl::SetProgress(int Index, int prog)
{
	CProgressEntry* ProgEntry;
	if (m_ProgEntries.Lookup(Index, ProgEntry) == TRUE)
		(ProgEntry->m_Prog).SetPos(prog);
}

void CProgressListCtrl::ResizeProg()
{	
	CProgressEntry*		ProgEntry = NULL;
	CRect				ItemRect;
	POSITION			pos	;
	int					Index, left, top, right, bottom;

	Index = left = top = right = bottom = 0;

	pos = m_ProgEntries.GetStartPosition();
	while (pos != NULL) 
	{
		m_ProgEntries.GetNextAssoc(pos, Index, ProgEntry);
		GetSubItemRect(ProgEntry->m_Index, ProgEntry->m_SubIndex, LVIR_BOUNDS, ItemRect);
		left	= ItemRect.left;
		top		= ItemRect.top;
		right	= ItemRect.right;
		bottom	= ItemRect.bottom;
		(ProgEntry->m_Prog).MoveWindow(left, top, (right - left), (bottom - top));
		
		// InvalidateRect(&ItemRect);

		CRect	rect;
		GetHeaderCtrl()->GetClientRect(rect);

		if(top>=rect.bottom)
			(ProgEntry->m_Prog).MoveWindow(left, top, (right - left), (bottom - top));
		else
			(ProgEntry->m_Prog).MoveWindow(left, -20, (right - left), (bottom - top));

		GetItemRect(ProgEntry->m_Index, &rect,LVIR_BOUNDS);
		InvalidateRect(&rect);
	}
}

void CProgressListCtrl::InitHdr()
{
	m_AdvHdr.Init(GetHeaderCtrl());
}

void CProgressListCtrl::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ResizeProg();
	*pResult = 0;

	return;
}

CProgressListCtrl::CProgressEntry::CProgressEntry(int Index, int SubIndex) :
m_Index(Index), m_SubIndex(SubIndex)
{
}

CProgressListCtrl::CProgressEntry::~CProgressEntry()
{
}

void CProgressListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ResizeProg();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CProgressListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ResizeProg();

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CProgressListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ResizeProg();
}

void CProgressListCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CListCtrl::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	ResizeProg();
}
