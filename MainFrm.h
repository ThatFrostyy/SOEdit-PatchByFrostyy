// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_MAINFRM_H__298C72B9_A7D8_4BE0_ADA0_87FE6669B7D4__INCLUDED_)
#define AFX_MAINFRM_H__298C72B9_A7D8_4BE0_ADA0_87FE6669B7D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CToolTab.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	//}}AFX_VIRTUAL

// Implementation
public:
    CToolTab      m_wndToolTab;
    //CDialogBar    m_wndToolBox;
    CDialogBar    m_wndAnimBox;
    CSplitterWnd  m_wndSplitter;


	void SetDefaultColors();
    void LoadGameDataPath();
    void SaveGameDataPath();

    char          m_ResPath[6][_MAX_PATH];
	char          m_CurMdlPath[_MAX_PATH];

    int           m_mode;
    bool          m_Selected;
    bool          m_ViewVols;
    bool          m_ViewEnts;
    bool          m_ViewEnts3d;
    bool          m_ViewArea;
    bool          m_ViewObst;

	bool          m_Vertex;
	bool          m_CollisionSide;
	bool          m_AllMeshVectors;
	bool          m_ShowTextureErrors;
	bool          m_GEMI_Compatibility; //Совместимость со старой версией импортёра в 3D MAX

    int           m_cBackGround2[3];
    int           m_cBackGround3[3];
    int           m_cGrid[3];
    int           m_cAxis[3];

    int           m_cMesh[3];
    int           m_cVolume[3];
    int           m_cEntity[3];
    int           m_cObstacle[3];
    int           m_cArea[3];

    int           m_cMeshS[3];
    int           m_cVolumeS[3];
    int           m_cEntityS[3];
    int           m_cObstacleS[3];
    int           m_cAreaS[3];

    color_t       m_Colors[frame_colors];
	DWORD ProgVer, RegVer;
	BOOL m_lamp;
	BOOL m_track;
	BOOL m_ram;
	DWORD m_fov;
	char m_author[_MAX_PATH];
	UINT m_FrameHoldTimerId;
	UINT m_FrameHoldBaseInterval;
	DWORD m_FrameHoldStartTick;
	UINT m_FrameHoldCommandId;

	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnModeMove();
	afx_msg void OnModeRotate();
	afx_msg void OnModeScale();
	afx_msg void OnModeSelect();
	afx_msg void OnEditPreferences();
	afx_msg void OnEditProperties();
	afx_msg void OnViewVolumes();
	afx_msg void OnViewEntity();
	afx_msg void OnViewSelected();
	afx_msg void OnViewObstacles();
	afx_msg void OnViewAreas();
	afx_msg void OnViewColors();
	afx_msg void OnCollisionsViewer();
	afx_msg void OnGEMICompatibility();
	afx_msg void OnColorize();
	afx_msg void OnInitMenuPopup(CMenu * pPopupMenu, UINT nIndex, BOOL bSysMenu);
	//}}AFX_MSG
    afx_msg void OnPlay();
    afx_msg void OnStop();
    afx_msg int OnSpeedControl();
    //afx_msg int OnBonesSelChanged();
    //afx_msg int OnBonesRClick();
    afx_msg void OnBack();
    afx_msg void OnStep();
    afx_msg void OnLoop();
    afx_msg void OnLoad();
    afx_msg void OnPause();
	afx_msg void OnGridOff();
	afx_msg void OnGridOn();
	afx_msg void OnGridSmaller();
	afx_msg void OnGridLarger();
	afx_msg void OnAppExitAl();
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnDisplayDisplayallmeshvectors();
	afx_msg void OnDisplayTextureErrors();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__298C72B9_A7D8_4BE0_ADA0_87FE6669B7D4__INCLUDED_)

