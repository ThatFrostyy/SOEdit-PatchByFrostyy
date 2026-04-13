// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CTOOLTAB_H__67B31A77_6B31_4E4D_9C9E_1BF810EFD79B__INCLUDED_)
#define AFX_CTOOLTAB_H__67B31A77_6B31_4E4D_9C9E_1BF810EFD79B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CToolTab.h : header file
//
#include "resource.h"
#include "CMainTab.h"
#include "CModelTree.h"
#include "VolPropDlg.h"
#include "CBonePropDlg.h"
#include "CAnimProps.h"

/////////////////////////////////////////////////////////////////////////////
// CToolTab dialog

class CToolTab : public CDialogBar
{
// Construction
public:
	//CToolTab(CWnd* pParent = NULL);   // standard constructor
	CToolTab();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolTab)
	enum { IDD = IDD_TOOLTAB };
	CMainTab	m_TabCtrl;
	//}}AFX_DATA

    //Variable to hold the dialog in focus
    CDialog m_DialogFocus;
    //Function to activate the tab dialog boxes 
    void ActivateTabDialogs();

    CModelTree   m_ModelTree;
    CBonePropDlg m_BonePropDlg;
    CVolPropDlg  m_VolPropDlg;
    CAnimProps   m_AnimPropDlg;

    CTabCtrl   *m_MyTab;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual ~CToolTab();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolTab)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
    afx_msg LONG OnInitDialog( UINT, LONG );   // <-Add this line.

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTOOLTAB_H__67B31A77_6B31_4E4D_9C9E_1BF810EFD79B__INCLUDED_)
