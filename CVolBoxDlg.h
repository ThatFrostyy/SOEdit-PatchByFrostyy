// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CVOLBOXDLG_H__D82ECEED_A2AA_40D3_BB04_0D16D9E9C678__INCLUDED_)
#define AFX_CVOLBOXDLG_H__D82ECEED_A2AA_40D3_BB04_0D16D9E9C678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CVolBoxDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVolBoxDlg dialog

class CVolBoxDlg : public CDialog
{
// Construction
public:
	CVolBoxDlg(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CVolBoxDlg)
	enum { IDD = IDD_VOL_BOX };
	float	m_BoxHeight;
	float	m_BoxLength;
	float	m_BoxWidth;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolBoxDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolBoxDlg)
	afx_msg void OnBoxFit();
	afx_msg void OnChangeBoxHeight();
	afx_msg void OnChangeBoxLength();
	afx_msg void OnChangeBoxWidth();
	afx_msg void OnEditChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVOLBOXDLG_H__D82ECEED_A2AA_40D3_BB04_0D16D9E9C678__INCLUDED_)
