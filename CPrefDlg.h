// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CPREFDLG_H__90D9AC3D_8513_4000_BFC2_C1BB9393E898__INCLUDED_)
#define AFX_CPREFDLG_H__90D9AC3D_8513_4000_BFC2_C1BB9393E898__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPrefDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrefDlg dialog

class CPrefDlg : public CDialog
{
// Construction
public:
	CPrefDlg(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CPrefDlg)
	enum { IDD = IDD_PREFER };
	CString	m_ResPath[6];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

public:
	// Generated message map functions
	//{{AFX_MSG(CPrefDlg)
	afx_msg void OnGdbrowse();
	afx_msg void OnGdbrowse2();
	afx_msg void OnGdbrowse3();
	afx_msg void OnGdbrowse4();
	afx_msg void OnGdbrowse5();
	afx_msg void OnGdbrowse6();
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CPREFDLG_H__90D9AC3D_8513_4000_BFC2_C1BB9393E898__INCLUDED_)
