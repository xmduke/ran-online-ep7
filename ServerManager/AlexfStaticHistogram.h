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

#if !defined(AFX_ALEXFSTATICHISTOGRAM_H__EE1F7AD3_2661_11D3_9865_868E4C4284CA__INCLUDED_)
#define AFX_ALEXFSTATICHISTOGRAM_H__EE1F7AD3_2661_11D3_9865_868E4C4284CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlexfStaticHistogram.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlexfStaticHistogram window

class CAlexfStaticHistogram : public CStatic
{
// Construction
public:
	CAlexfStaticHistogram();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlexfStaticHistogram)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Add(DOUBLE dValue);
	BOOL IsOK() {return m_bOK;}
	BOOL SetTextLeft(BOOL bSet = TRUE);
	BOOL SetMotionLtoR(BOOL bSet = TRUE);
	BOOL SetTextWidth(LONG lWidth);
	BOOL SetTextLines(LONG lLines);
	BOOL SetMaxValue(DOUBLE dValue);
	CString text[10];		// text strings
	virtual ~CAlexfStaticHistogram();

	// Generated message map functions
protected:
	BOOL m_bOK;				// control operational
    LONG m_lTextLines;		// number of text lines
	LONG m_lTextWidth;		// width of text field
    LONG m_lStack;
    DOUBLE m_dMaxValue;		// maximum value
    DOUBLE * m_pdArray;		// array of values
    LONG m_lArraySize;		// array size
	BOOL m_bTextLeft;		// text placed at ledt corner
	BOOL m_bNeedFullRedraw;	//
	BOOL m_bMotionLToR;		// motion from left to right


	//{{AFX_MSG(CAlexfStaticHistogram)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALEXFSTATICHISTOGRAM_H__EE1F7AD3_2661_11D3_9865_868E4C4284CA__INCLUDED_)
