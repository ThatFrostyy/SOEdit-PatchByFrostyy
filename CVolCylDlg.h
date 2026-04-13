// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CVOLCYLDLG_H__8F7D49E1_8D5E_4D5C_BD25_4794F178511A__INCLUDED_)
#define AFX_CVOLCYLDLG_H__8F7D49E1_8D5E_4D5C_BD25_4794F178511A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CVolCylDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVolCylDlg dialog

class CVolCylDlg : public CDialog
{
// Construction
public:
	CVolCylDlg(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CVolCylDlg)
	enum { IDD = IDD_VOL_CYL };
	float	m_CylDiameter;
	float	m_CylLength;
	int		m_XYZ;
	//}}AFX_DATA

    int m_XYZval;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolCylDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolCylDlg)
	afx_msg void OnCylFit();
	afx_msg void OnXyz1();
	afx_msg void OnXyz2();
	afx_msg void OnXyz3();
	afx_msg void OnEditChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVOLCYLDLG_H__8F7D49E1_8D5E_4D5C_BD25_4794F178511A__INCLUDED_)
