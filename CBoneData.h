// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CBONEDATA_H__11FC17CC_540A_43B8_8221_B805A7BE7F44__INCLUDED_)
#define AFX_CBONEDATA_H__11FC17CC_540A_43B8_8221_B805A7BE7F44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CBoneData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBoneData dialog

#include "CAnimation.h"

class CBoneData : public CDialog
{
// Construction
public:
	CBoneData(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CBoneData)
	enum { IDD = IDD_BONEDATA };
	CString	m_BoneName;
	BOOL	m_Ground;
	BOOL    m_NoCastShadows, m_DecalTarget, m_NoGroupMesh, m_NoGetShadows;
	float	m_LimitsCommon;
	float	m_LimitsHi;
	float	m_LimitsLo;
	float	m_Matrix11;
	float	m_Matrix12;
	float	m_Matrix13;
	float	m_Matrix21;
	float	m_Matrix22;
	float	m_Matrix23;
	float	m_Matrix31;
	float	m_Matrix32;
	float	m_Matrix33;
	float	m_Matrix41;
	float	m_Matrix42;
	float	m_Matrix43;
	CString	m_Parameters;
	float	m_PositionX;
	float	m_PositionY;
	float	m_PositionZ;
	float	m_Speed;
	
	enum TRotateType {TNone = 0, TRevolute, TPrizmatic, TSocket} RotateType;
	enum TWindowMode {TFull = 0, TSubBone, TAnimation} WindowMode;

	BOOL	HasLimits;
	BOOL	m_Terminator;
	BOOL	m_Speed2;
	BOOL    bone_colorized;
	CString	m_VolumeView;
	float	m_Orientation11;
	float	m_Orientation12;
	float	m_Orientation13;
	float	m_Orientation21;
	float	m_Orientation22;
	float	m_Orientation23;
	float	m_Orientation31;
	float	m_Orientation32;
	float	m_Orientation33;
	CString	m_ParentBone;
	int		m_Visible;
	char    text_buff[_MAX_PATH];
	DWORD   bone_color;
	BOOL    m_LODView;
	BOOL    m_LOD_OFF;
	BOOL    has_sub;
	int     m_Layer;
	CSequenceList *m_SequenceList;
	int     SequenceIndex;
	CSequence *pSeq;
	bool    forced_ply_upd, SeqSelMode;
	BOOL	m_RewritePermission;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoneData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRevolute();
	afx_msg void OnPrizmatic();
	afx_msg void OnSocket();
	afx_msg void OnNone();
	afx_msg void OnBrowse();
	afx_msg void OnCheckerBoneName();
	afx_msg void OnCheckerParent_Bone();
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
	afx_msg void OnCheckerLimits_Common();
	afx_msg void OnCheckerLimits_Hi();
	afx_msg void OnCheckerLimits_Lo();
	afx_msg void OnBoneSpeed();
	afx_msg void OnRoundBoneClick();
	afx_msg void OnCheckerParameters();
	afx_msg void OnChecker();
	afx_msg void OnVolumeView();
	afx_msg void OnPaint();
	afx_msg void OnColorSel();
	afx_msg void OnSelchangeElements();
	afx_msg void OnChecker2();
	afx_msg void OnBnClickedBoneSequenceAdd();
	afx_msg void OnBnClickedBoneSequenceDelete();
	float FixBoneMX(int id);
	void DrawSwatch();
	void SkeletonOff();
	void AnimationOff();
	void SubBoneON();
	void LayerUPD();

	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBoneData)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeBoneSequenceSpeed();
	afx_msg void OnEnChangeBoneSequenceName();
	afx_msg void OnBnClickedBoneSequenceFileBrowse();
	afx_msg void OnEnChangeBoneSequenceFile();
	afx_msg void OnBnClickedColorized();
	afx_msg void OnBnClickedLodOff();
	afx_msg void OnBnClickedLod();
	afx_msg void OnEnChangeBoneSequenceSmooth();
	afx_msg void OnBnClickedBoneWrench();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBONEDATA_H__11FC17CC_540A_43B8_8221_B805A7BE7F44__INCLUDED_)
