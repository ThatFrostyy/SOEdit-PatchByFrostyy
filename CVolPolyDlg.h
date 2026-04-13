// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CVOLPOLYDLG_H__B2938004_D4E4_4E95_B53E_27451D073C2D__INCLUDED_)
#define AFX_CVOLPOLYDLG_H__B2938004_D4E4_4E95_B53E_27451D073C2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CVolPolyDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CVolPolyDlg dialog

class CVolPolyDlg : public CDialog
{
// Construction
public:
	CVolPolyDlg(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CVolPolyDlg)
	enum { IDD = IDD_VOL_POLY };
	CString	m_PolyName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolPolyDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolPolyDlg)
	afx_msg void OnVolChoose();
	afx_msg void OnM_Name();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVOLPOLYDLG_H__B2938004_D4E4_4E95_B53E_27451D073C2D__INCLUDED_)
