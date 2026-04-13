// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CVOLTAB_H__8B2E4F4D_7E4D_456A_BF9D_3A634CD1D07E__INCLUDED_)
#define AFX_CVOLTAB_H__8B2E4F4D_7E4D_456A_BF9D_3A634CD1D07E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CVolTab.h : header file
//
#include "CVolPolyDlg.h"
#include "CVolBoxDlg.h"
#include "CVolCylDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CVolTab window

class CVolTab : public CTabCtrl
{
// Construction
public:
	CVolTab();

    int  m_DialogID[3];
    bool m_Dialog[3];

    float m_BoxLength, m_BoxWidth, m_BoxHeight;

    float m_CylLength, m_CylDiameter;

    bool  m_CylRadioSet;

    int   m_VolType;

    CString m_PolyName;

    //Function to activate the tab dialog boxes 
    void ActivateTabDialogs();

    CVolPolyDlg m_VolPolyDlg;
    CVolBoxDlg  m_VolBoxDlg;
    CVolCylDlg  m_VolCylDlg;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolTab)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVolTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVolTab)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVOLTAB_H__8B2E4F4D_7E4D_456A_BF9D_3A634CD1D07E__INCLUDED_)
