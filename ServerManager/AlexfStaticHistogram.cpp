/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1999 by Alexander Fedorov
// All rights reserved
//
// Distribute and use freely, except:
// 1. Don't alter or remove this notice.
// 2. Mark the changes you made
//
// Send bug reports, bug fixes, enhancements, requests, etc. to:
//    alexander.fedorov@usa.net
/////////////////////////////////////////////////////////////////////////////

// AlexfStaticHistogram.cpp : implementation file

#include "stdafx.h"

#include "AlexfStaticHistogram.h"
#include <Winuser.h>
// #include "../=DX_Include/multimon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlexfStaticHistogram

CAlexfStaticHistogram::CAlexfStaticHistogram()
{
	m_bOK = FALSE;
	m_lTextLines = 0;
	m_lTextWidth = 0;
	m_lStack = 0;
	m_dMaxValue = 100;
	m_bTextLeft = TRUE;
	m_bNeedFullRedraw = FALSE;
	m_bMotionLToR = TRUE;

	m_lArraySize = 0;//GetSystemMetrics(SM_CXSCREEN);
	m_pdArray = new DOUBLE[m_lArraySize];
	if (!m_pdArray) 
		return;
	ZeroMemory(m_pdArray, m_lArraySize * sizeof(DOUBLE));
	m_bOK = TRUE;
}

CAlexfStaticHistogram::~CAlexfStaticHistogram()
{
	if (m_pdArray) 
		delete [] m_pdArray;
}


BEGIN_MESSAGE_MAP(CAlexfStaticHistogram, CStatic)
	//{{AFX_MSG_MAP(CAlexfStaticHistogram)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlexfStaticHistogram message handlers

void CAlexfStaticHistogram::OnPaint() 
{
	if (!m_bOK)
	{
		CStatic::OnPaint();
		return;
	}
	CPaintDC dc(this); // device context for painting
	CRect cr;
	GetClientRect(&cr);
	if (m_bNeedFullRedraw)
	{
		m_bNeedFullRedraw = FALSE;
		dc.FillRect(cr, & CBrush(GetSysColor(COLOR_BTNFACE)));
	}
	CRect cr1(cr);
	if (m_bTextLeft) 
		cr1.right = cr1.left + m_lTextWidth;
	else 
		cr1.left = cr1.right - m_lTextWidth;
	// text
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	int hh = 0;
	if (m_lTextLines != 0) 
		hh = cr.Height() / m_lTextLines;
	cr1.right = cr1.left + m_lTextWidth;
	dc.FillRect(cr1, & CBrush(GetSysColor(COLOR_BTNFACE)));
	cr1.bottom = cr1.top + hh;
	
	CFont* pOldFont = dc.SelectObject(GetFont());	
	for (int i = 0; i < m_lTextLines; i++)
	{
		dc.DrawText(text[i], (LPRECT)&cr1,
		DT_WORDBREAK | DT_CENTER | DT_VCENTER);
		cr1.OffsetRect(0, hh);
	}
	dc.SelectObject(pOldFont);

	// graph
	if (m_dMaxValue == 0) 
		return;
	if (m_bTextLeft) 
		cr.left += m_lTextWidth;
	else 
		cr.right -= m_lTextWidth;
	cr1 = cr;
	
	if (m_bMotionLToR) 
		cr1.right = cr1.left + 2;
	else 
		cr1.left = cr1.right - 2;
	
	i = m_lStack;
	cr1.bottom -= 1;
	int h = cr.Height() - 2;
	
	while (TRUE)
	{
		int h1 = 0;
		if (m_dMaxValue != 0) 
			h1 = (int)((DOUBLE) h * m_pdArray[i] / m_dMaxValue);
		if (h1 > h) 
			h1 = h;
		cr1.top = cr.top;
		cr1.bottom = cr.bottom - h1;
		dc.FillRect(cr1, & CBrush(GetSysColor(COLOR_BTNFACE)));
		cr1.top = cr.bottom - h1;
		cr1.bottom = cr.bottom;

		// Note : jgkim, change color to blue
		// Original code
		// dc.FillRect(cr1, & CBrush(GetSysColor(COLOR_BTNHIGHLIGHT)));
		// Changed code
		dc.FillRect(cr1, & CBrush(RGB(0,0,255)));
		
		if (m_bMotionLToR) 
			cr1.OffsetRect(3, 0);
		else 
			cr1.OffsetRect(-3, 0);
		
		if (i == 0) 
			i = m_lArraySize;
		i--;
		if (m_bMotionLToR)
		{
			if (cr1.left > cr.right - 3) 
				break;
		}
		else
		{
			if (cr1.left < cr.left) 
				break;
		}
	}
	// Do not call CStatic::OnPaint() for painting messages
}

BOOL CAlexfStaticHistogram::Add(DOUBLE dValue)
{
	if (!m_bOK) 
		return FALSE;
	m_lStack++;
	if (m_lStack >= m_lArraySize) 
		m_lStack = 0;
	m_pdArray[m_lStack] = dValue;
	RedrawWindow();
	return TRUE;
}

BOOL CAlexfStaticHistogram::SetTextLeft(BOOL bSet)
{
	if (!m_bOK) 
		return FALSE;
	m_bTextLeft = bSet;
	m_bNeedFullRedraw = TRUE;
	RedrawWindow();
	return TRUE;
}

BOOL CAlexfStaticHistogram::SetTextWidth(LONG lWidth)
{
	if (!m_bOK) 
		return FALSE;
	m_lTextWidth = lWidth;
	m_bNeedFullRedraw = TRUE;
	RedrawWindow();
	return TRUE;
}

BOOL CAlexfStaticHistogram::SetTextLines(LONG lLines)
{
	if (!m_bOK) 
		return FALSE;
	m_lTextLines = lLines;
	m_bNeedFullRedraw = TRUE;
	RedrawWindow();
	return TRUE;
}

BOOL CAlexfStaticHistogram::SetMotionLtoR(BOOL bSet)
{
	if (!m_bOK) 
		return FALSE;
	m_bMotionLToR = bSet;
	m_bNeedFullRedraw = TRUE;
	RedrawWindow();
	return TRUE;
}

BOOL CAlexfStaticHistogram::SetMaxValue(DOUBLE dValue)
{
	if (!m_bOK) 
		return FALSE;
	m_dMaxValue = dValue;
	m_bNeedFullRedraw = TRUE;
	RedrawWindow();
	return TRUE;
}
