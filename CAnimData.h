// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CANIMDATA_H__5EB793F0_4749_47A1_B381_95E317AC4085__INCLUDED_)
#define AFX_CANIMDATA_H__5EB793F0_4749_47A1_B381_95E317AC4085__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CAnimData.h : header file
//

#include "soedit.h"
#include "CAnimFrame.h"

static matrix34_t g_MatrixClipboard;

/////////////////////////////////////////////////////////////////////////////
// CAnimData dialog

class CAnimData : public CDialog
{
// Construction
public:
	CAnimData(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_ANIM_DATA };
	CSpinButtonCtrl	m_Framer;
	CListBox	m_BoneList;
	CString	m_Name;
	CString SelBone;
	CString	m_FileName;
	int		m_FrameCnt;
	int		m_Frame;
	float	m_M1_1;
	float	m_M1_2;
	float	m_M2_1;
	float	m_M1_3;
	float	m_M2_2;
	float	m_M2_3;
	float	m_M3_1;
	float	m_M3_2;
	float	m_M3_3;
	float	m_M4_1;
	float	m_M4_2;
	float	m_M4_3;
	BOOL	m_Visi;
	BOOL	m_RewritePermission, m_RewritePermissionBAnm;
	char    text_buff[_MAX_PATH];
	CAnimSub *m_pSub;

	int    AnmTaskNum;
    int    m_FrameNo;
	unsigned int m_ver;


    void CAnimData::SetMatrixData(int control, float value);
	void OnCheckerMatrix(int control);
	void OnCheckerAutoVol(int control);
	void ActionUPD();
	void MinMaxUPD();
	void LawUPD();
	void TaskNumUPD();
	void MatrixRewriteUPD();
	void AutoFramersUPD();
	void ListActionUPD();
	void AutoVolUPD(WPARAM id, float vol);
	void FramerUPD();



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnimData)
	virtual BOOL OnInitDialog();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSelchangeMbones();
	afx_msg void OnDeltaposFramer(NMHDR* pNMHDR, LRESULT* pResult);
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
	afx_msg void OnInsertFrame();
	//}}AFX_MSG

	afx_msg void OnBnClickedResetBone();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnCopyMatrix();
	afx_msg void OnPasteMatrix();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDeleteFrame();
	afx_msg void OnBnClickedVisi();
	afx_msg void OnBnClickedDeleteAnmBoneName();
	afx_msg void OnBnClickedAnmAuto();
	afx_msg void OnBnClickedAnmAxisX();
	afx_msg void OnBnClickedAnmAxisY();
	afx_msg void OnBnClickedAnmAxisZ();
	afx_msg void OnEnChangeAnmXMin();
	afx_msg void OnEnChangeAnmYMin();
	afx_msg void OnEnChangeAnmZMin();
	afx_msg void OnEnChangeAnmXMax();
	afx_msg void OnEnChangeAnmYMax();
	afx_msg void OnEnChangeAnmZMax();
	afx_msg void OnEnChangeAnmLaw3Ratio();
	afx_msg void OnBnClickedMatrixRewrite();
	afx_msg void OnEnChangeAdAnmBoneName();
	afx_msg void OnBnClickedAnmBoneClone();
	afx_msg void OnBnClickedAnmBoneExport();
	afx_msg void OnBnClickedAnmBoneImport();
	afx_msg void OnReverseAnimation();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANIMDATA_H__5EB793F0_4749_47A1_B381_95E317AC4085__INCLUDED_)
