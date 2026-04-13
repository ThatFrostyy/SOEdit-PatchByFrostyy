// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CANIMPROPS_H__7FC197FF_7AD9_4359_8A8D_A814892569B9__INCLUDED_)
#define AFX_CANIMPROPS_H__7FC197FF_7AD9_4359_8A8D_A814892569B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAnimProps.h : header file
//

#include "soedit.h"

/////////////////////////////////////////////////////////////////////////////
// CAnimProps dialog

class CAnimProps : public CDialog
{
// Construction
public:
	CAnimProps(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnimProps)
	enum { IDD = IDD_ANIM_PROPS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimProps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimProps)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANIMPROPS_H__7FC197FF_7AD9_4359_8A8D_A814892569B9__INCLUDED_)

