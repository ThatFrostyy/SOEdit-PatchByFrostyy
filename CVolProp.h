// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CVOLPROP_H__3441CFBD_B243_495F_871C_6AE4FC0BFE81__INCLUDED_)
#define AFX_CVOLPROP_H__3441CFBD_B243_495F_871C_6AE4FC0BFE81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CVolProp.h : header file
//

#include <afxtempl.h>
#include "CVolTab.h"
#include "CBone.h"

/////////////////////////////////////////////////////////////////////////////
// CVolProp dialog

class CVolProp : public CDialog
{
// Construction
public:
	CVolProp(CWnd* pParent = NULL);   // standard constructor
	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CVolProp)
	enum { IDD = IDD_VOLDATA };
	CVolTab	m_TabCtrl;
	CString	m_Name;
	CString	m_VolBone;
	float	m_VolPosX;
	float	m_VolPosY;
	float	m_VolPosZ;
	float	m_Matrix1_1;
	float	m_Matrix1_2;
	float	m_Matrix1_3;
	float	m_Matrix2_1;
	float	m_Matrix2_2;
	float	m_Matrix2_3;
	float	m_Matrix3_1;
	float	m_Matrix3_2;
	float	m_Matrix3_3;
	float	m_Matrix4_1;
	float	m_Matrix4_2;
	float	m_Matrix4_3;
	//}}AFX_DATA

    float   m_BoxLength;
    float   m_BoxWidth;
    float   m_BoxHeight;

    float   m_CylLength;
    float   m_CylDiameter;

    CString m_PolyName;

    int m_VolType;
	char text_buff[_MAX_PATH];

	int nitems;
	char **sitems;

    void SkeletonScanning(CBone *basis, bool mod);

	// Array of all the property-page dialogs
	CTypedPtrArray<CObArray, CDialog*> m_DlgArray;

	// Call this function to create all pages for the tab view window.
	void CreatePages();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolProp)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolProp)
	afx_msg void OnSelchangeVolBones();
	afx_msg void OnCheckerMatrix11();
	afx_msg void OnCheckerMatrix12();
	afx_msg void OnCheckerMatrix13();
	afx_msg void OnCheckerMatrix21();
	afx_msg void OnCheckerMatrix22();
	afx_msg void OnCheckerMatrix23();
	afx_msg void OnCheckerMatrix31();
	afx_msg void OnCheckerMatrix32();
	afx_msg void OnCheckerMatrix33();
	afx_msg void OnCheckerMatrix41();
	afx_msg void OnCheckerMatrix42();
	afx_msg void OnCheckerMatrix43();
	afx_msg void OnM_Name();
	afx_msg void OnRoundVolumeClick();
	virtual void OnOK();
	afx_msg void OnChangeBone();
	float FixVolMX(int id);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CVOLPROP_H__3441CFBD_B243_495F_871C_6AE4FC0BFE81__INCLUDED_)
