// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CBONEPROPDLG_H__63885D17_0D23_471E_AE24_8DD7DC531728__INCLUDED_)
#define AFX_CBONEPROPDLG_H__63885D17_0D23_471E_AE24_8DD7DC531728__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CBonePropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBonePropDlg dialog

class CBonePropDlg : public CDialog
{
// Construction
public:
	CBonePropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBonePropDlg)
	enum { IDD = IDD_BONE_PROPS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBonePropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBonePropDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBONEPROPDLG_H__63885D17_0D23_471E_AE24_8DD7DC531728__INCLUDED_)
