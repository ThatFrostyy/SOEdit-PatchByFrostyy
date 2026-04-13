// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_VOLPROPDLG_H__41D83088_51A6_4704_B079_36455997DD97__INCLUDED_)
#define AFX_VOLPROPDLG_H__41D83088_51A6_4704_B079_36455997DD97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VolPropDlg.h : header file
//

#include "CVolPolyDlg.h"
#include "CVolBoxDlg.h"
#include "CVolCylDlg.h"
#include "CBone.h"
#include "CVolume.h"

/////////////////////////////////////////////////////////////////////////////
// CVolPropDlg dialog

class CVolPropDlg : public CDialog
{
// Construction
public:
	CVolPropDlg(CWnd* pParent = NULL);   // standard constructor
    ~CVolPropDlg();

// Dialog Data
	//{{AFX_DATA(CVolPropDlg)
	enum { IDD = IDD_VOL_PROPS };
	CString	m_Name;
	CString	m_Bone;
	int		m_VolType;
	//}}AFX_DATA

    CVolume    *m_pVolume;

    float       m_Box[3];
    float       m_Cylinder[2];

    void        HideBox();
    void        HideCyl();
    void        HidePoly();

    void        ShowBox();
    void        ShowCyl();
    void        ShowPoly();

    void        Activate();
    void        BuildBoneList(CBone *basis);
    void        AddBoneToList(CBone *basis);

    CString     m_PolyhedronName;

    bool        m_Created[3];

    CVolPolyDlg m_VolPolyDlg;
    CVolBoxDlg  m_VolBoxDlg;
    CVolCylDlg  m_VolCylDlg;

    void        LoadValues();
    void        SaveValues();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolPropDlg)
	afx_msg void OnVoltype1();
	afx_msg void OnVoltype2();
	afx_msg void OnVoltype3();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeVolBones();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLPROPDLG_H__41D83088_51A6_4704_B079_36455997DD97__INCLUDED_)
