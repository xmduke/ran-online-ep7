/*
Module : PUSHPIN.H
Purpose: Interface of a push pin button 
         (as seen on X-Windows & property dialogs in VC 4)
Created: PJN / 04-04-1996
History: None

Copyright (c) 1997 - 1999 by PJ Naughter.  
All rights reserved.

*/

#ifndef __PUSHPIN_H__
#define __PUSHPIN_H__


////////////////////////////////// Classes ///////////////////////////////////

class CPushPinButton : public CButton
{
public:
	//Constructors / Destructors
	CPushPinButton();
	~CPushPinButton();

protected:
	//{{AFX_MSG(CPushPinButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	//}}AFX_MSG

	//{{AFX_VIRTUAL(CPushPinButton)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	void SizeToContent();
	void LoadBitmaps();

	DECLARE_MESSAGE_MAP()

	CDialog*	m_pDlg;
	BOOL    m_bPinned;
	BOOL    m_bCapturePinned;
	BOOL    m_bUseEdge;
	CBitmap m_PinnedBitmap;
	CBitmap m_UnPinnedBitmap;
	CRect   m_MaxRect;
	BOOL    m_bCaptured;
public:
	void SetWnd(CDialog* pDlg);

	//Accessors & Mutators for the pinned state
	void SetPinned(BOOL bPinned);
	BOOL IsPinned() { return m_bPinned; };

	//Should be called in response to system color changes
	void ReloadBitmaps(); 

	//Use an edged version of the bitmap
	void SetUseEdgeBitmap(BOOL bUseEdge);
	BOOL GetUseEdgeBitmap() const { return m_bUseEdge; };
};

#endif //__PUSHPIN_H__