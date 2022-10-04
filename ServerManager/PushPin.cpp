/*
Module : PUSHPIN.H
Purpose: Implementation of a push pin button 
         (as seen on X-Windows & property dialogs in VC 4)
Created: PJN / 04-04-1996
History: PJN / 08-06-1996 / Removed win32sup.h include
         PJN / 20-11-1997 / Major Update to code, changes include
                            1. Support non default color schemes
                            2. Supports dynamic changes to color schemes
                            3. Complete restructuring of code
                            4. Can now use an edged bitmap if you want to
                            5. Dropped support for Win16
                            6. Redid the example program 
                            7. Simplified external usage of class
         PJN / 24-11-1997   1. Minor changes to support CPushPinFrame class
         PJN / 07-12-1997   Minor changes to fix a small redraw bug
         PJN / 22-02-1999   1. A number of small code updates
         PJN / 02-06-1999   1. Added a number of comments to improvde code
                            readibility.
                            2. Changed the resouce id of the button bitmaps to more
                            meaningful values. 
                            3. Class no longer uses message reflection to
                            catch changes in the clicked state. This means that
                            parent classes can now catch the clicked notification.
                            4. Code now uses mouse capturing which means that the
                            button appears pressed if you click and hold the left
                            mouse button over the push pin. This gives the pushpin
                            class this standard button behaviour.

         

Copyright (c) 1997 - 1999 by PJ Naughter.  
All rights reserved.
*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "PushPin.h"
#include "ServerViewDlg.h"

//////////////////////////////////  Macros  ///////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////// Implementation /////////////////////////////
BEGIN_MESSAGE_MAP(CPushPinButton, CButton)
  //{{AFX_MSG_MAP(CPushPinButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPushPinButton::CPushPinButton()
{
	//Initialise our variables to default values
	m_bPinned = FALSE;
	m_bUseEdge = FALSE;
	m_MaxRect = CRect(0, 0, 0, 0);
	m_bCaptured = FALSE;

	//Load up the bitmaps used in the push pin
	LoadBitmaps();
}

CPushPinButton::~CPushPinButton()
{
}

void CPushPinButton::ReloadBitmaps()
{
	//free the bitmap resources
	m_PinnedBitmap.DeleteObject();
	m_UnPinnedBitmap.DeleteObject();

	//Reload the bitmaps
	LoadBitmaps();

	//size to content
	SizeToContent();

	//Invalidate the maximum rect of the pushpin on the parent window
	GetParent()->InvalidateRect(m_MaxRect);

	//Force this button to redraw aswell
	Invalidate();
}

void CPushPinButton::LoadBitmaps()
{
	//Load up the necessary bitmaps using the SDK call LoadImage
	if (m_bUseEdge)
	{
		BOOL bLoad = m_PinnedBitmap.Attach((HBITMAP) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PP_PINNEDEDGE), 
															IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
		ASSERT(bLoad);

		bLoad = m_UnPinnedBitmap.Attach((HBITMAP) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PP_UNPINNEDEDGE), 
														IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
		ASSERT(bLoad);
	}
	else
	{
		BOOL bLoad = m_PinnedBitmap.Attach((HBITMAP) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PP_PINNED), 
															IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
		ASSERT(bLoad);

		bLoad = m_UnPinnedBitmap.Attach((HBITMAP) ::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PP_UNPINNED), 
														IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS));
		ASSERT(bLoad);
	}
}

void CPushPinButton::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	ASSERT(lpDIS != NULL);

	//select the bitmap to use depending on the pinned
	//and captured state
	CBitmap* pBitmap;
	if (m_bPinned || (m_bCaptured && m_bCapturePinned))
		pBitmap = &m_PinnedBitmap;
	else
		pBitmap = &m_UnPinnedBitmap;

	// draw the bitmap into a compatible memory DC
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOld = memDC.SelectObject(pBitmap);
	if (pOld == NULL)
		return;     // destructors will clean up

	//Then bitblt it onto the screen DC
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
	&memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CPushPinButton::SetPinned(BOOL bPinned)
{
	m_bPinned = bPinned;
	Invalidate();
}

void CPushPinButton::SetUseEdgeBitmap(BOOL bUseEdge)
{
	if (bUseEdge == m_bUseEdge)  //quick return
		return;
	
	m_bUseEdge = bUseEdge;   //toggle the option, reload and
	ReloadBitmaps();         //and force a redraw
}

void CPushPinButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();

	//Pushpin buttons must be owner draw
	ASSERT(GetWindowLong(m_hWnd, GWL_STYLE) & BS_OWNERDRAW);

	//size to content
	SizeToContent();
}

void CPushPinButton::SizeToContent()
{
	ASSERT(m_PinnedBitmap.m_hObject != NULL);
	CSize bitmapSize;
	BITMAP bmInfo;
	VERIFY(m_PinnedBitmap.GetObject(sizeof(bmInfo), &bmInfo) == sizeof(bmInfo));

	//Calculate the client rect in parent coordinates of the maximum size of the pushpin button
	m_MaxRect = CRect(0, 0, max(bmInfo.bmWidth, m_MaxRect.Width()), 
					max(bmInfo.bmHeight, m_MaxRect.Height()));
	ClientToScreen(&m_MaxRect);

	CPoint p1(m_MaxRect.left, m_MaxRect.top);
	CPoint p2(m_MaxRect.right, m_MaxRect.bottom);
	HWND hParent = ::GetParent(m_hWnd);
	::ScreenToClient(hParent, &p1);
	::ScreenToClient(hParent, &p2);
	m_MaxRect = CRect(p1, p2);

	//resize the button to match the size of the bitmap
	VERIFY(SetWindowPos(NULL, -1, -1, bmInfo.bmWidth, bmInfo.bmHeight, 
						SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));
}

void CPushPinButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//Capture the mouse
	SetCapture();
	m_bCaptured = TRUE;
	
	// Let the parent class do its thing
	CButton::OnLButtonDown(nFlags, point);
}

void CPushPinButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//Release the capture
	ReleaseCapture();
	m_bCaptured = FALSE;

	//Toggle the state of the pin and force a redraw
	//if over the button
	CRect r;
	GetClientRect(&r);
	if (r.PtInRect(point))
	{
		m_bPinned = !m_bPinned;
		Invalidate();
	}

	CServerViewDlg* pDlg;
	pDlg = (CServerViewDlg*) m_pDlg;
	pDlg->TopWindow();
	
	// Let the parent class do its thing
	CButton::OnLButtonUp(nFlags, point);
}

void CPushPinButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//Handle changing the pinned state as the mouse is 
	//moved over the push pin
	if (m_bCaptured)
	{
		CRect r;
		GetClientRect(&r);
		if (r.PtInRect(point))
		{
			if (!m_bCapturePinned)
				Invalidate();
			m_bCapturePinned = TRUE;
		}
		else
		{
			if (m_bCapturePinned)
				Invalidate();
			m_bCapturePinned = FALSE;
		}
	}
	//Let the parent class do its thing		
	CButton::OnMouseMove(nFlags, point);
}

void CPushPinButton::OnCaptureChanged(CWnd *pWnd) 
{
	//Release the capture
	if (pWnd != this)
	{
		ReleaseCapture();
		m_bCaptured = TRUE;
	}

	//Let the parent class do its thing	
	CButton::OnCaptureChanged(pWnd);
}

void CPushPinButton::SetWnd(CDialog* pDlg)
{
	m_pDlg = pDlg;
}