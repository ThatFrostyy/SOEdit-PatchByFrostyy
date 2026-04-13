// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CCOLORS_H__DB5E3D36_EFE1_433A_9C03_FFFE0D41AAA5__INCLUDED_)
#define AFX_CCOLORS_H__DB5E3D36_EFE1_433A_9C03_FFFE0D41AAA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "general.h"

// CColors.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColors dialog

class CColors : public CDialog
{
// Construction
public:
	CColors(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColors)
	enum { IDD = IDD_COLORS };
	CListBox	m_Elements;
	CString	m_Selected;
	//}}AFX_DATA
    void DrawSwatch(void);
    void BankColors(void);

    color_t   m_Colors[frame_colors];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColors)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeElements();
	afx_msg void OnColorSel();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCOLORS_H__DB5E3D36_EFE1_433A_9C03_FFFE0D41AAA5__INCLUDED_)
