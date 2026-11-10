// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "ModernUI.h"
#include "SOEdit.h"
#include "SOEditDoc.h"
#include "SOEDitView.h"
#include "CSOEDit2dView.h"
#include "CPrefDlg.h"
#include "setup_window.h"
#include "CDlgGetPath.h"
#include "Winerror.h"
#include "CColors.h"
#include "Mmsystem.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

inline double round_1000(double x)
{
	int n = x * 1000;
	return (double)n / 1000;
}

extern char *Elements[];
static int TimelineKeyProvider(int frame);

HWND m_hWnd;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_MODE_MOVE, OnModeMove)
	ON_COMMAND(ID_MODE_MOVE_AL, OnModeMove)
	ON_COMMAND(ID_MODE_ROTATE, OnModeRotate)
	ON_COMMAND(ID_MODE_ROTATE_AL, OnModeRotate)
	ON_COMMAND(ID_MODE_SCALE, OnModeScale)
	ON_COMMAND(ID_MODE_SCALE_AL, OnModeScale)
	ON_COMMAND(ID_MODE_SELECT, OnModeSelect)
	ON_COMMAND(ID_MODE_SELECT_AL, OnModeSelect)
	ON_COMMAND(ID_EDIT_PREFERENCES, OnEditPreferences)
	ON_COMMAND(ID_EDIT_PREFERENCES_AL, OnEditPreferences)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_COMMAND(ID_EDIT_PROPERTIES_AL, OnEditProperties)
	ON_COMMAND(ID_VIEW_VOLUMES, OnViewVolumes)
	ON_COMMAND(ID_VIEW_VOLUMES_AL, OnViewVolumes)
	ON_COMMAND(ID_VIEW_ENTITY, OnViewEntity)
	ON_COMMAND(ID_VIEW_ENTITY_AL, OnViewEntity)
	ON_COMMAND(ID_VIEW_SELECTED, OnViewSelected)
	ON_COMMAND(ID_VIEW_SELECTED_AL, OnViewSelected)
	ON_COMMAND(ID_VIEW_OBSTACLES, OnViewObstacles)
	ON_COMMAND(ID_VIEW_OBSTACLES_AL, OnViewObstacles)
	ON_COMMAND(ID_VIEW_AREAS, OnViewAreas)
	ON_COMMAND(ID_VIEW_AREAS_AL, OnViewAreas)
	ON_COMMAND(ID_VIEW_COLORS, OnViewColors)
	ON_COMMAND(ID_VIEW_COLORS_AL, OnViewColors)
	ON_COMMAND(ID_VIEW_COLLISIONSVIEWER, OnCollisionsViewer)
	ON_COMMAND(ID_VIEW_COLLISIONSVIEWER_AL, OnCollisionsViewer)
	ON_COMMAND(OLD_GEMI_COMPATIBILITY, OnGEMICompatibility)
	ON_COMMAND(OLD_GEMI_COMPATIBILITY_AL, OnGEMICompatibility)
	ON_COMMAND(ID_VIEW_COLORIZE, OnColorize)
	ON_COMMAND(ID_VIEW_COLORIZE_AL, OnColorize)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP_FINDER_AL, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
    ON_COMMAND(IDC_PLAY, CMainFrame::OnPlay)
    ON_COMMAND(IDC_STOP, CMainFrame::OnStop)
    ON_COMMAND(IDC_BACK, CMainFrame::OnBack)
    ON_COMMAND(IDC_STEP, CMainFrame::OnStep)
    ON_COMMAND(IDC_LOOP, CMainFrame::OnLoop)
    ON_COMMAND(IDC_LOAD, CMainFrame::OnLoad)
    ON_COMMAND(IDC_PAUSE, CMainFrame::OnPause)
	ON_COMMAND(ID_GRID_OFF, OnGridOff)
	ON_COMMAND(ID_GRID_OFF_AL, OnGridOff)
	ON_COMMAND(ID_GRID_ON, OnGridOn)
	ON_COMMAND(ID_GRID_ON_AL, OnGridOn)
	ON_COMMAND(ID_GRID_SMALLER, OnGridSmaller)
	ON_COMMAND(ID_GRID_SMALLER_AL, OnGridSmaller)
	ON_COMMAND(ID_GRID_LARGER, OnGridLarger)
	ON_COMMAND(ID_GRID_LARGER_AL, OnGridLarger)
	ON_COMMAND(ID_APP_EXIT_AL, &CMainFrame::OnAppExitAl)
	ON_COMMAND(ID_HELP_SHORTCUTS, &CMainFrame::OnHelpShortcuts)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_DISPLAY_DISPLAYALLMESHVECTORS_AL, &CMainFrame::OnDisplayDisplayallmeshvectors)
	ON_COMMAND(ID_DISPLAY_DISPLAYALLMESHVECTORS, &CMainFrame::OnDisplayDisplayallmeshvectors)
	ON_COMMAND(ID_DISPLAY_TEXTURE_ERRORS_AL, &CMainFrame::OnDisplayTextureErrors)
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	memset(m_ResPath[0], 0, _MAX_PATH);
	memset(m_ResPath[1], 0, _MAX_PATH);
	memset(m_ResPath[2], 0, _MAX_PATH);
	memset(m_ResPath[3], 0, _MAX_PATH);
	memset(m_ResPath[4], 0, _MAX_PATH);
	memset(m_ResPath[5], 0, _MAX_PATH);
	memset(m_CurMdlPath, 0, _MAX_PATH);
    m_Selected = false;
    m_mode = m_select;
    m_ViewVols = true;
    m_ViewEnts = true;
	m_ViewEnts3d = false;
    m_ViewArea = true;
    m_ViewObst = true;
	m_ram = m_track = m_lamp = true;
	m_fov = 45;
	m_GEMI_Compatibility = false;
	RegVer = ProgVer = 0;
	m_AllMeshVectors = m_CollisionSide = m_Vertex = false;
	m_ShowTextureErrors = true;
	memset(m_author, 0, _MAX_PATH);
	m_FrameHoldTimerId = 0;
	m_FrameHoldBaseInterval = 180;
	m_FrameHoldStartTick = 0;
	m_FrameHoldCommandId = 0;
	wchar_t szVersionFile[MAX_PATH];
	GetModuleFileNameW(NULL, szVersionFile, MAX_PATH);
	DWORD verHandle = 0;
	UINT size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD verSize = GetFileVersionInfoSizeW(szVersionFile, &verHandle);
	if(verSize)
	{
		LPSTR verData = new char[verSize];
		if(GetFileVersionInfoW(szVersionFile, verHandle, verSize, verData))
		{
			if(VerQueryValue(verData,"\\",(VOID FAR* FAR*)&lpBuffer,&size))
			{
				if(size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if(verInfo -> dwSignature == 0xfeef04bd)
					{
						ProgVer = LOBYTE((verInfo -> dwFileVersionMS >> 16) & 0xffff) << 24;
						ProgVer |= LOBYTE((verInfo -> dwFileVersionMS >> 0) & 0xffff) << 16;
						ProgVer |= LOBYTE((verInfo -> dwFileVersionLS >> 16) & 0xffff) << 8;
						ProgVer |= LOBYTE((verInfo -> dwFileVersionLS >> 0) & 0xffff);
					}
				}
			}
		}
		delete[] verData;
	}
    SetDefaultColors();
    LoadGameDataPath();
}

void CMainFrame::SetDefaultColors()
{
	// Modern dark viewport palette, in element_t order.
	static const unsigned char pal[frame_colors][3] =
	{
		{ 27,  29,  36},  // C_BG2D
		{ 36,  40,  52},  // C_BG3D
		{ 58,  62,  76},  // C_GRID
		{ 74,  80, 100},  // C_3D_GRID
		{170, 176, 194},  // C_AXIS
		{222, 226, 238},  // C_MESH
		{255, 110, 110},  // C_MESHS
		{255, 206,  86},  // C_VOLUME
		{ 94, 142, 255},  // C_VOLUMES
		{ 80, 220, 140},  // C_ENTITY
		{255, 100, 220},  // C_ENTITYS
		{ 90, 130, 235},  // C_OBSTACLE
		{235, 205,  70},  // C_OBSTACLES
		{ 60, 190, 120},  // C_AREA
		{190, 110, 235},  // C_AREAS
		{255,  90,  90},  // VOLUME_FRONT
		{255, 160,  60},  // VOLUME_REAR
		{ 70, 170,  90},  // VOLUME_RIGHT
		{140, 150, 255},  // VOLUME_LEFT
		{ 60, 230, 230},  // VOLUME_TOP
		{240,  80, 220}   // VOLUME_BOTTOM
	};
	for(int i = 0; i < frame_colors; i++)
	{
		m_Colors[i][0] = pal[i][0];
		m_Colors[i][1] = pal[i][1];
		m_Colors[i][2] = pal[i][2];
	}
}

CMainFrame::~CMainFrame()
	{}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		{return -1;}
		if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME_AL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//m_wndToolBar.LoadBitmapA("IDB_DOCK");
	if(!m_wndStatusBar.Create(this)
	|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndToolBar);
		ModernUI::ModernizeToolbar(m_wndToolBar.GetToolBarCtrl().m_hWnd, IDR_MAINFRAME_AL);
    m_wndAnimBox.Create(this, IDD_ANIMBOX, CBRS_BOTTOM | CBRS_FLYBY | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC, IDD_ANIMBOX);
    CMenu *pMenu = GetMenu();
    CMenu* pPopup = pMenu -> GetSubMenu(5);
    ASSERT(pPopup != NULL);
	
		pPopup -> CheckMenuItem(ID_MODE_SELECT_AL, MF_CHECKED);
		pPopup -> CheckMenuItem(ID_MODE_MOVE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_ROTATE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_SCALE_AL, MF_UNCHECKED);

    m_wndToolTab.Create(this, IDD_TOOLTAB, CBRS_LEFT | CBRS_FLYBY | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC, IDD_TOOLBOX);
    //m_wndToolBox.Create(this, IDD_TOOLBOX, CBRS_RIGHT | CBRS_FLYBY | CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC, IDD_TOOLBOX);
    CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_SPEED);
    CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
    //pSpinner->SetBuddy(pWnd);
    pSpinner -> SetRange(1, 20);
    pSpinner -> SetPos(10);
    pWnd -> SetWindowText("1.0");
	CWnd* pLerpStart = (CWnd*)m_wndAnimBox.GetDlgItem(IDC_LERP_START_FRAME);
	CWnd* pLerpEnd = (CWnd*)m_wndAnimBox.GetDlgItem(IDC_LERP_END_FRAME);
	if (pLerpStart)
	{
		pLerpStart->SetWindowText("1");
	}
	if (pLerpEnd)
	{
		pLerpEnd->SetWindowText("2");
	}
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_ANM_EDITING_MODE);
		pWnd -> SetWindowText("Mode: Skeleton Editing");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INHERIT_MATRIX);
		pWnd -> SetWindowText("Copy the matrix to the following frames");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_LOOP);
		pWnd -> SetWindowText("Repeat");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INSERT_FRAME);
		pWnd -> SetWindowText("Insert frame");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_ANMBOX_DELETE_FRAME);
		pWnd -> SetWindowText("Delete frame");
		pWnd = (CWnd*)m_wndAnimBox.GetDlgItem(IDC_REVERSE_ANIM);
		pWnd->SetWindowText("Reverse");
		pWnd = (CWnd*)m_wndAnimBox.GetDlgItem(IDC_ANM_LERP_APPLY);
		pWnd->SetWindowText("Fill");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_STATIC_AB_1);
		pWnd -> SetWindowText("Frame:");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_STATIC_AB_3);
		pWnd -> SetWindowText("of");
		pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_STATIC_AB_2);
		pWnd -> SetWindowText("Animation file:");
    CSOEditApp *pApp = (CSOEditApp *)AfxGetApp();
    CButton *pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_PAUSE);
    HICON hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_PAUSE");
    pButton -> SetIcon(hIcon);
    pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_PLAY);
    hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_PLAY");
    pButton -> SetIcon(hIcon);
    pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_BACK);
    hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_BACK");
    pButton -> SetIcon(hIcon);
    pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_STEP);
    hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_STEP");
    pButton -> SetIcon(hIcon);
    pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_STOP);
    hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_STOP");
    pButton -> SetIcon(hIcon);
    CSliderCtrl *pSlider = (CSliderCtrl *)m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
    pSlider -> SetRange(0, 0, true);
    pSlider -> SetPos(0);
	ModernUI::SetTimelineKeyFn(TimelineKeyProvider);
    m_hWnd = this -> m_hWnd;
	return 0;
}

void CMainFrame::OnClose()
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	if(pDoc -> m_Model && pDoc -> m_Model -> m_skeleton && pDoc -> m_Model -> m_skeleton -> m_bonelist)
	{
			if(MessageBox("Are you confirming the exit?", "QUESTION: CMainFrame::OnClose", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{return CFrameWnd::OnClose();}
	}
	else
		{return CFrameWnd::OnClose();}
	return;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cy = 752;
	cs.cx = 1426;
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnPlay()
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(!pDoc -> m_Model)
			{return;}
		if(!pDoc -> m_Model -> m_skeleton)
			{return;}
		if(!pDoc -> m_Model -> m_skeleton -> m_bonelist)
			{return;}
		pDoc -> Anm_and_skeleton_editing_prohibited = true;
		pDoc -> ResetBone(pDoc -> m_Model -> m_skeleton -> m_bonelist);
		if(pDoc -> m_AnimBone)
			{pDoc -> m_Animate = true;}
	}
	return;
}

void CMainFrame::OnStop()
{
    CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    if(pDoc)
	{
        if(pDoc -> m_AnimBone)
		{
            pDoc -> m_Animate = false;
            pDoc -> m_Frame = 0;
            char szWindowText[32];
            CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
            sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
            pWnd -> SetWindowText(szWindowText);
            CSliderCtrl *pSlider = (CSliderCtrl *)m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
            pSlider -> SetPos(pDoc -> m_Frame);
			pDoc -> Anm_and_skeleton_editing_prohibited = false;
			pDoc -> OnModEditing();
			pDoc -> UpdateAllViews(NULL);
		}
	}
    return;
}

void CMainFrame::OnPause()
{
    CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    if(pDoc)
	{
        if(pDoc->m_AnimBone)
		{
			pDoc -> m_Animate = false;
			pDoc -> Anm_and_skeleton_editing_prohibited = false;
		}
	}
    return;
}

int CMainFrame::OnSpeedControl()
{
    CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    if(pDoc)
	{
        if(pDoc -> m_AnimBone)
		{
            CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
            int tspeed = pSpinner -> GetPos() & 0xFFFF;
            pDoc -> m_Speed = (float)tspeed * 0.1f;
			pDoc -> m_Speed = round_1000(pDoc -> m_Speed);
            char szWindowText[_MAX_PATH];
            sprintf(szWindowText, "%0.1f", pDoc -> m_Speed);
            CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_SPEED);
            pWnd -> SetWindowText(szWindowText);
		}
	}
    return 0;
}

void CMainFrame::OnBack()
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(pDoc -> m_AnimBone && !pDoc -> m_Animate)
		{
			if(pDoc -> m_Frame > 0)
			{
				pDoc -> m_Frame--;
				char szWindowText[32];
				CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
				sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
				pWnd -> SetWindowText(szWindowText);
				CSliderCtrl *pSlider = (CSliderCtrl *)m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
				pSlider -> SetPos(pDoc -> m_Frame);
				pDoc -> OnModEditing();
				pDoc -> UpdateAllViews(NULL);
			}
		}
	}
	return;
}

void CMainFrame::OnStep()
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(pDoc -> m_AnimBone && !pDoc -> m_Animate)
		{
			if((pDoc -> m_Frame < (pDoc -> m_AnimBone -> m_FrameCnt - 1)) || (pDoc -> m_Loop))
			{
				pDoc -> m_Frame++;
				if(pDoc -> m_Loop)
					{pDoc -> m_Frame = pDoc -> m_Frame % pDoc -> m_AnimBone -> m_FrameCnt;}
				char szWindowText[32];
				CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
				sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
				pWnd -> SetWindowText(szWindowText);
				CSliderCtrl *pSlider = (CSliderCtrl *)m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
				pSlider -> SetPos(pDoc -> m_Frame);
				pDoc -> OnModEditing();
				pDoc -> UpdateAllViews(NULL);
			}
		}
	}
	return;
}

void CMainFrame::OnLoop()
{
    CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	CButton *pButton = (CButton *)m_wndAnimBox.GetDlgItem(IDC_LOOP);
	if(pDoc)
	{
		if(pDoc -> m_Loop)
		{
			pDoc -> m_Loop = false;
				pButton -> SetWindowText("Repeat");
		}
		else
		{
			pDoc -> m_Loop = true;
				pButton -> SetWindowText("No repeats");
		}
	}
	return;
}

void CMainFrame::OnLoad()
{
    CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    if(pDoc)
		{pDoc -> OnLoad();}
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	switch(wParam)
	{
		case IDC_ANIM_SLIDER:
		{
			NMHDR *nmHdr = (NMHDR *)lParam;
			if(nmHdr -> code == 0xFFFFFFF0 || nmHdr -> code == 0xFFFFFFF4)
			{
				CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
				CSliderCtrl *pSlider = (CSliderCtrl *)m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
				CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
				char szWindowText[32];
				if(pDoc && !pDoc -> m_Animate)
				{
					pDoc -> m_Frame = pSlider -> GetPos();
					//pSlider -> SetPos(pDoc -> m_Frame);
					sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
					pWnd -> SetWindowText(szWindowText);
					pDoc -> OnModEditing();
					pDoc -> UpdateAllViews(NULL);
				}
			}
		}
		break;
		case IDC_SPEED_CONTROL:
		{
			NMHDR *nmHdr = (NMHDR *)lParam;
			if(nmHdr -> code == UDN_DELTAPOS)
			{
				CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
				if(pDoc)
				{
					NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)nmHdr;
					pDoc -> m_Speed += pNMUpDown -> iDelta * 0.1;
					pDoc -> m_Speed = max(-10, min(pDoc -> m_Speed, 10));
					pDoc -> m_Speed = round_1000(pDoc -> m_Speed);
					char szWindowText[_MAX_PATH];
					sprintf(szWindowText, "%0.1f", pDoc -> m_Speed);
					CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_SPEED);
					pWnd -> SetWindowText(szWindowText);
				}
			}
		}
		break;
	}
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	bool bSuccess;
	if(m_wndSplitter.CreateStatic(this, 2, 2, WS_CHILD | WS_VISIBLE, AFX_IDW_PANE_FIRST))
		{bSuccess = true;}
	else
		{bSuccess = false;}
	m_wndSplitter.SetRowInfo(0, 120, 10);
	m_wndSplitter.SetRowInfo(1, 120, 10);
	m_wndSplitter.SetColumnInfo(0, 100, 10);
	m_wndSplitter.SetColumnInfo(1, 100, 10);
	RECT rect;
	GetClientRect(&rect);
	rect.right -= (230 + 8);
	rect.bottom -= (125 + 8);
	CSize vSize;
	vSize.cx = rect.right / 2;
	vSize.cy = rect.bottom / 2;
	int square = 250; 
	CSize vSize2[4];
	vSize2[0].cx = square;
	vSize2[0].cy = square;
	vSize2[1].cx = vSize.cx - square;
	vSize2[1].cy = square;
	vSize2[2].cx = square;
	vSize2[2].cy = vSize.cy - square;
	vSize2[3].cx = vSize.cx - square;
	vSize2[3].cy = vSize.cy - square;
	//RECT toolbox;
	//m_wndToolBox.GetWindowRect(&toolbox);
	//RECT animbar;
	//m_wndAnimBox.GetWindowRect(&animbar);
	//rect.right -= (toolbox.right - toolbox.left);
	//rect.bottom -= (animbar.bottom - animbar.top);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CSOEdit2dView), vSize2[0], pContext);
	m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSOEdit2dView), vSize2[1], pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CSOEdit2dView), vSize2[2], pContext);
	m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CSOEditView), vSize2[3], pContext);
	CSOEdit2dView *p2dView = (CSOEdit2dView *)m_wndSplitter.GetPane(0, 0);
	p2dView -> SetView(v_front, 0);
	p2dView = (CSOEdit2dView *)m_wndSplitter.GetPane(0, 1);
	p2dView -> SetView(v_left, 1);
	p2dView = (CSOEdit2dView *)m_wndSplitter.GetPane(1, 0);
	p2dView -> SetView(v_top, 2);
	
	return bSuccess;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnModeSelect()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(5);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_MODE_SELECT_AL, MF_CHECKED);
		pPopup -> CheckMenuItem(ID_MODE_MOVE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_ROTATE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_SCALE_AL, MF_UNCHECKED);
	m_mode = m_select;
}

void CMainFrame::OnModeMove()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(5);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_MODE_SELECT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_MOVE_AL, MF_CHECKED);
		pPopup -> CheckMenuItem(ID_MODE_ROTATE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_SCALE_AL, MF_UNCHECKED);
	m_mode = m_move;
}

void CMainFrame::OnModeScale()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(5);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_MODE_SELECT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_MOVE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_ROTATE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_SCALE_AL, MF_CHECKED);
	m_mode = m_scale;
}

void CMainFrame::OnModeRotate()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(5);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_MODE_SELECT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_MOVE_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_MODE_ROTATE_AL, MF_CHECKED);
		pPopup -> CheckMenuItem(ID_MODE_SCALE_AL, MF_UNCHECKED);
	m_mode = m_rotate;
}

void CMainFrame::OnEditPreferences() 
{
    CPrefDlg pDlg;
    pDlg.m_ResPath[0] = m_ResPath[0];
	pDlg.m_ResPath[1] = m_ResPath[1];
	pDlg.m_ResPath[2] = m_ResPath[2];
	pDlg.m_ResPath[3] = m_ResPath[3];
	pDlg.m_ResPath[4] = m_ResPath[4];
	pDlg.m_ResPath[5] = m_ResPath[5];
	if(pDlg.DoModal() == IDOK)
	{
		for(int i = 0; i < 6; i++)
		{
			if(!strlen(pDlg.m_ResPath[i]))
				{continue;}
			InvertFixPathDelim(pDlg.m_ResPath[i].GetBuffer(1));
			if(pDlg.m_ResPath[i][strlen(pDlg.m_ResPath[i]) - 1] != '\\')
				{pDlg.m_ResPath[i] += "\\";}
		}
		strcpy(m_ResPath[0], pDlg.m_ResPath[0].GetBuffer(0));
		strcpy(m_ResPath[1], pDlg.m_ResPath[1].GetBuffer(0));
		strcpy(m_ResPath[2], pDlg.m_ResPath[2].GetBuffer(0));
		strcpy(m_ResPath[3], pDlg.m_ResPath[3].GetBuffer(0));
		strcpy(m_ResPath[4], pDlg.m_ResPath[4].GetBuffer(0));
		strcpy(m_ResPath[5], pDlg.m_ResPath[5].GetBuffer(0));
	}
}

void CMainFrame::OnEditProperties()
{
	CSOEditDoc* pDoc = (CSOEditDoc *)GetActiveDocument();
	setup_window pDlg;
	pDlg.m_lamp = m_lamp;
	pDlg.m_ram = m_ram;
	pDlg.m_track = m_track;
	pDlg.m_author = m_author;
	pDlg.m_old_author = pDoc -> m_old_author;
	pDlg.m_old_author_ply = pDoc -> m_old_author_ply;
	pDlg.m_old_author_vol = pDoc -> m_old_author_vol;
	pDlg.m_old_date = pDoc -> m_old_date;
	pDlg.m_old_file = pDoc -> m_old_file;
	pDlg.m_fov = m_fov;
	if(pDlg.DoModal() == IDOK)
	{
		m_lamp = pDlg.m_lamp;
		m_ram = pDlg.m_ram;
		m_track = pDlg.m_track;
		strcpy(m_author, pDlg.m_author);
		m_fov = pDlg.m_fov;
		pDoc -> UpdateAllViews(NULL, 0, NULL);
	}
}

void CMainFrame::LoadGameDataPath()
{
	HKEY hk;
	LONG err;
	err = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\SOEdit_VNext\\SOEdit", NULL, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hk);
	if(err == ERROR_SUCCESS)
	{
		TCHAR Reget[_MAX_PATH] = {0};
		DWORD RegetPath = _MAX_PATH;
		DWORD Type = REG_DWORD;
		BOOL old_ver = false;
		LONG res;
		if(ERROR_SUCCESS == (res = RegQueryValueEx(hk, TEXT("Version"), NULL, &Type, (PBYTE)&RegVer, &RegetPath)))
		{
			if(ProgVer > RegVer)
			{
				old_ver = true;
			}
		}
		else
		{
				MessageBox("It is impossible to determine the number of the old version of the program.\r\nSome parameters may be missing.", "WARNING: CMainFrame::LoadGameDataPath", MB_ICONEXCLAMATION);
		}
		if(old_ver)
		{
				MessageBox(CString("Settings from the old version of the program were detected.\r\nSome registry parameters may be missing and not readable." + CString((ColorUPD) ? "\nThe color settings will be reset to default." : "")), "INFORMATION: CMainFrame::LoadGameDataPath", MB_ICONINFORMATION);
		}
		Type = 0;
		RegetPath = _MAX_PATH;
		res = RegQueryValueEx(hk, TEXT("GameDataPath"), NULL, &Type, (LPBYTE)&m_ResPath[0], &RegetPath); RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("ModsDataPath1"), NULL, &Type, (LPBYTE)&m_ResPath[1], &RegetPath); RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("ModsDataPath2"), NULL, &Type, (LPBYTE)&m_ResPath[2], &RegetPath); RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("ModsDataPath3"), NULL, &Type, (LPBYTE)&m_ResPath[3], &RegetPath); RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("ModsDataPath4"), NULL, &Type, (LPBYTE)&m_ResPath[4], &RegetPath); RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("ModsDataPath5"), NULL, &Type, (LPBYTE)&m_ResPath[5], &RegetPath); RegetPath = _MAX_PATH;
		if(res != ERROR_SUCCESS)
		{
				MessageBox("Error reading the path to the game or mods.", "ERROR: CMainFrame::LoadGameDataPath", MB_ICONHAND);
		}
		Type = REG_DWORD;
		DWORD color = 0x00000000;
		RegetPath = _MAX_PATH;
		for(int c = 0; c < frame_colors && ((ColorUPD) ? !old_ver : true); c++)
		{
			if(ERROR_SUCCESS == (res = RegQueryValueEx(hk, TEXT(Elements[c]), NULL, &Type, (PBYTE)&color, &RegetPath)))
			{
				m_Colors[c][0] = (unsigned char)((color >> 16) & 0xFF);
				m_Colors[c][1] = (unsigned char)((color >> 8) & 0xFF);
				m_Colors[c][2] = (unsigned char)(color & 0xFF);
			}
			else
			{
				char msg[256] = {0};
					sprintf(msg, "Parameter reading error \"%s\"!", Elements[c]);
				MessageBox(msg, "ERROR: CMainFrame::LoadGameDataPath", MB_ICONHAND);
			}
		}
		res = 0;
		res = RegQueryValueEx(hk, TEXT("3D_Fov"), NULL, &Type, (PBYTE)&m_fov, &RegetPath);
		if(m_fov < 20)
			{m_fov = 20;}
		if(m_fov > 120)
			{m_fov = 120;}
		Type = 0;
		RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("Model_author"), NULL, &Type, (LPBYTE)m_author, &RegetPath);
		Type = REG_BINARY;
		RegetPath = _MAX_PATH;
		res |= RegQueryValueEx(hk, TEXT("View_Lamp"), NULL, &Type, (PBYTE)&m_lamp, &RegetPath);
		res |= RegQueryValueEx(hk, TEXT("View_Tracks_Vol"), NULL, &Type, (PBYTE)&m_track, &RegetPath);
		res |= RegQueryValueEx(hk, TEXT("View_Ram_Vol"), NULL, &Type, (PBYTE)&m_ram, &RegetPath);
		if(res != ERROR_SUCCESS)
		{
				MessageBox("Error reading data for the settings window.", "ERROR: CMainFrame::LoadGameDataPath", MB_ICONHAND);
		}
		RegCloseKey(hk);
	}
}

void CMainFrame::SaveGameDataPath()
{
	HKEY hk;
	LONG err;
	DWORD dwType, dwSize;
	err = RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\SOEdit_VNext\\SOEdit", 0, "", REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, NULL);
	if(err == ERROR_SUCCESS)
	{
		err = RegSetValueEx(hk, TEXT("GameDataPath"), 0, REG_SZ, (PBYTE)m_ResPath[0], strlen(m_ResPath[0]));
		err |= RegSetValueEx(hk, TEXT("ModsDataPath1"), 0, REG_SZ, (PBYTE)m_ResPath[1], strlen(m_ResPath[1]));
		err |= RegSetValueEx(hk, TEXT("ModsDataPath2"), 0, REG_SZ, (PBYTE)m_ResPath[2], strlen(m_ResPath[2]));
		err |= RegSetValueEx(hk, TEXT("ModsDataPath3"), 0, REG_SZ, (PBYTE)m_ResPath[3], strlen(m_ResPath[3]));
		err |= RegSetValueEx(hk, TEXT("ModsDataPath4"), 0, REG_SZ, (PBYTE)m_ResPath[4], strlen(m_ResPath[4]));
		err |= RegSetValueEx(hk, TEXT("ModsDataPath5"), 0, REG_SZ, (PBYTE)m_ResPath[5], strlen(m_ResPath[5]));
		if(err != ERROR_SUCCESS)
		{
				MessageBox("Writing error Game/Mods Data Path", "ERROR: CMainFrame::LoadGameDataPath", MB_ICONHAND);
		}
		RegSetValueEx(hk, TEXT("Model_author"), 0, REG_SZ, (PBYTE)m_author, strlen(m_author));
		dwType = REG_DWORD;
		dwSize = sizeof(DWORD);
		if(ProgVer > RegVer)
			{RegSetValueEx(hk, TEXT("Version"), 0, dwType, (PBYTE)&ProgVer, dwSize);}
		DWORD color = 0x00000000;
		for(int c = 0; c < frame_colors; c++)
		{
			color = (m_Colors[c][0] << 16) + (m_Colors[c][1] << 8) + m_Colors[c][2];
			RegSetValueEx(hk, TEXT(Elements[c]), 0, dwType, (PBYTE)&color, dwSize);
		}
		RegSetValueEx(hk, TEXT("3D_Fov"), 0, dwType, (PBYTE)&m_fov, dwSize);
		dwType = REG_BINARY;
		dwSize = sizeof(BOOL);
		RegSetValueEx(hk, TEXT("View_Lamp"), 0, dwType, (PBYTE)&m_lamp, dwSize);
		RegSetValueEx(hk, TEXT("View_Tracks_Vol"), 0, dwType, (PBYTE)&m_track, dwSize);
		RegSetValueEx(hk, TEXT("View_Ram_Vol"), 0, dwType, (PBYTE)&m_ram, dwSize);
		RegCloseKey(hk);
		//Таракан. Медленно ползёт по своим делам...
	}
}

BOOL CMainFrame::DestroyWindow()
{
	if (m_FrameHoldTimerId)
	{
		KillTimer(0xA11F);
		m_FrameHoldTimerId = 0;
		m_FrameHoldCommandId = 0;
	}
	SaveGameDataPath();
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewVolumes()
{
	CSOEditDoc* pDoc = (CSOEditDoc *)GetActiveDocument();
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	if(m_ViewVols)
	{
			pPopup -> CheckMenuItem(ID_VIEW_VOLUMES_AL, MF_UNCHECKED);
		m_ViewVols = false;
	}
	else
	{	
			pPopup -> CheckMenuItem(ID_VIEW_VOLUMES_AL, MF_CHECKED);
		m_ViewVols = true;
	}
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnViewEntity()
{
	CSOEditDoc* pDoc = (CSOEditDoc *)GetActiveDocument();
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	if(m_ViewEnts)
	{
			pPopup -> CheckMenuItem(ID_VIEW_ENTITY_AL, MF_UNCHECKED);
		m_ViewEnts = false;
	}
	else
	{
			pPopup -> CheckMenuItem(ID_VIEW_ENTITY_AL, MF_CHECKED);
		m_ViewEnts = true;
	}
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnViewSelected()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	if(m_Selected)
	{
		pPopup -> CheckMenuItem(ID_VIEW_SELECTED, MF_UNCHECKED);
		m_Selected = false;
	}
	else
	{
		pPopup -> CheckMenuItem(ID_VIEW_SELECTED, MF_CHECKED);
		m_Selected = true;
	}
	GetActiveDocument() -> UpdateAllViews(NULL);
}

void CMainFrame::OnViewObstacles()
{
	CSOEditDoc* pDoc = (CSOEditDoc *)GetActiveDocument();
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	if(m_ViewObst)
	{
			pPopup -> CheckMenuItem(ID_VIEW_OBSTACLES_AL, MF_UNCHECKED);
		m_ViewObst = false;
	}
	else
	{
			pPopup -> CheckMenuItem(ID_VIEW_OBSTACLES_AL, MF_CHECKED);
		m_ViewObst = true;
	}
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnViewAreas()
{
	CSOEditDoc* pDoc = (CSOEditDoc *)GetActiveDocument();
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	if(m_ViewArea)
	{
			pPopup -> CheckMenuItem(ID_VIEW_AREAS_AL, MF_UNCHECKED);
		m_ViewArea = false;
	}
	else
	{
			pPopup -> CheckMenuItem(ID_VIEW_AREAS_AL, MF_CHECKED);
		m_ViewArea = true;
	}
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnViewColors()
{
	CColors Dlg;
	if(Dlg.DoModal() == IDOK)
	{
		for(int i = 0; i < frame_colors; i++)
		{
			for(int c = 0; c < 3; c++)
				{m_Colors[i][c] = Dlg.m_Colors[i][c];}
		}
	}
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnCollisionsViewer()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	m_CollisionSide = !m_CollisionSide;
		pPopup -> CheckMenuItem(ID_VIEW_COLLISIONSVIEWER_AL, (m_CollisionSide) ? MF_CHECKED : MF_UNCHECKED);
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnDisplayDisplayallmeshvectors()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
	m_AllMeshVectors = !m_AllMeshVectors;
		pPopup -> CheckMenuItem(ID_DISPLAY_DISPLAYALLMESHVECTORS_AL, (m_AllMeshVectors) ? MF_CHECKED : MF_UNCHECKED);
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnDisplayTextureErrors()
{
	CMenu* pMenu = GetMenu();
	CMenu* pPopup = pMenu->GetSubMenu(6);
	ASSERT(pPopup != NULL);
	m_ShowTextureErrors = !m_ShowTextureErrors;
	pPopup->CheckMenuItem(ID_DISPLAY_TEXTURE_ERRORS_AL, (m_ShowTextureErrors) ? MF_CHECKED : MF_UNCHECKED);
}

void CMainFrame::OnGEMICompatibility()
{
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(0);
	ASSERT(pPopup != NULL);
	m_GEMI_Compatibility = !m_GEMI_Compatibility;
		pPopup -> CheckMenuItem(OLD_GEMI_COMPATIBILITY_AL, (m_GEMI_Compatibility) ? MF_CHECKED : MF_UNCHECKED);
}

void CMainFrame::OnColorize()
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	pDoc -> Colorized_3d = !pDoc -> Colorized_3d;
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
		pPopupMenu -> CheckMenuItem(ID_VIEW_COLORIZE_AL, (pDoc -> Colorized_3d) ? MF_CHECKED : MF_UNCHECKED);
		pPopupMenu->CheckMenuItem(ID_DISPLAY_DISPLAYALLMESHVECTORS_AL, (m_AllMeshVectors) ? MF_CHECKED : MF_UNCHECKED);
		pPopupMenu->CheckMenuItem(ID_DISPLAY_TEXTURE_ERRORS_AL, (m_ShowTextureErrors) ? MF_CHECKED : MF_UNCHECKED);
}

// Timeline keyframe test: a frame is a "key" for the selected bone when its transform differs
// from the previous frame (frame 1 is always a key when the bone is animated at all).
static bool SameMatrix(const matrix34_t &a, const matrix34_t &b)
{
	return memcmp(&a, &b, sizeof(matrix34_t)) == 0;
}

static int TimelineKeyProvider(int frame)
{
	CSOEditDoc *pDoc = (CSOEditDoc *)(AfxGetMainWnd() ? ((CMainFrame *)AfxGetMainWnd())->GetActiveDocument() : NULL);
	if (!pDoc || !pDoc->m_AnimBone || !pDoc->m_SelBone || !pDoc->m_AnimBone->m_Frames) {return 0;}
	CAnimBone *anm = pDoc->m_AnimBone;
	if (frame < 0 || frame >= anm->m_FrameCnt) {return 0;}
	int bone = -1;
	for (int i = 0; i < anm->m_BoneCnt; i++)
	{
		if (anm->m_BoneMap[i] && _stricmp(anm->m_BoneMap[i], pDoc->m_SelBone) == 0) {bone = i; break;}
	}
	if (bone < 0) {return 0;}
	CAnimSub *cur = anm->m_Frames[frame].FindSub(bone);
	if (frame == 0) {return cur != NULL;}
	CAnimSub *prev = anm->m_Frames[frame - 1].FindSub(bone);
	if (!cur || !prev) {return (cur != NULL) != (prev != NULL);}
	return !SameMatrix(cur->m_Matrix34, prev->m_Matrix34);
}

// True while the keyboard focus is in a text box, so single-key shortcuts must not steal typing.
static bool IsTypingFocus()
{
	HWND f = ::GetFocus();
	if (!f) {return false;}
	char cls[32] = {0};
	::GetClassNameA(f, cls, 39);
	// Single-key shortcuts only run while the viewports/timeline have focus. Text boxes, trees, lists,
	// combo boxes and buttons keep their own keys (arrows, space, letters).
	return !(_strnicmp(cls, "Afx", 3) == 0 || _stricmp(cls, "msctls_trackbar32") == 0);
}

// Jump the animation to a frame (clamped), updating the slider, the frame box and the views.
static void GoToFrame(CMainFrame *pFrame, int frame)
{
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrame->GetActiveDocument();
	if (!pDoc || !pDoc->m_AnimBone || pDoc->m_Animate) {return;}
	int last = pDoc->m_AnimBone->m_FrameCnt - 1;
	if (last < 0) {return;}
	pDoc->m_Frame = max(0, min(frame, last));
	char text[32];
	sprintf(text, "%d", pDoc->m_Frame + 1);
	CWnd *pWnd = pFrame->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
	if (pWnd) {pWnd->SetWindowText(text);}
	CSliderCtrl *pSlider = (CSliderCtrl *)pFrame->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
	if (pSlider) {pSlider->SetPos(pDoc->m_Frame);}
	pDoc->OnModEditing();
	pDoc->UpdateAllViews(NULL);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	bool up_down = false, action = false;
	if (pMsg->message == WM_TIMER && pMsg->wParam == 0xA11F && m_FrameHoldCommandId)
	{
		if (GetActiveDocument())
		{
			SendMessage(WM_COMMAND, m_FrameHoldCommandId, 0);
		}
		DWORD holdMs = GetTickCount() - m_FrameHoldStartTick;
		UINT nextInterval = m_FrameHoldBaseInterval;
		if (holdMs > 1200)
		{
			nextInterval = 35;
		}
		else if (holdMs > 700)
		{
			nextInterval = 60;
		}
		else if (holdMs > 300)
		{
			nextInterval = 100;
		}
		if (nextInterval != m_FrameHoldBaseInterval)
		{
			m_FrameHoldBaseInterval = nextInterval;
			SetTimer(0xA11F, m_FrameHoldBaseInterval, NULL);
		}
		return TRUE;
	}
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		HWND hInsert = m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INSERT_FRAME) ? m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INSERT_FRAME)->GetSafeHwnd() : NULL;
		HWND hDelete = m_wndAnimBox.GetDlgItem(IDC_ANMBOX_DELETE_FRAME) ? m_wndAnimBox.GetDlgItem(IDC_ANMBOX_DELETE_FRAME)->GetSafeHwnd() : NULL;
		if (pMsg->hwnd == hInsert || pMsg->hwnd == hDelete)
		{
			m_FrameHoldCommandId = (pMsg->hwnd == hInsert) ? IDC_ANMBOX_INSERT_FRAME : IDC_ANMBOX_DELETE_FRAME;
			m_FrameHoldStartTick = GetTickCount();
			m_FrameHoldBaseInterval = 180;
			if (m_FrameHoldTimerId)
			{
				KillTimer(0xA11F);
			}
			m_FrameHoldTimerId = SetTimer(0xA11F, m_FrameHoldBaseInterval, NULL);
		}
	}
	if (pMsg->message == WM_LBUTTONUP)
	{
		if (m_FrameHoldTimerId)
		{
			KillTimer(0xA11F);
			m_FrameHoldTimerId = 0;
			m_FrameHoldCommandId = 0;
		}
	}
	if(pMsg -> message == WM_KEYDOWN || pMsg -> message == WM_MOUSEWHEEL)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			const bool hasModifier = ((GetKeyState(VK_CONTROL) & 0x8000) != 0) || ((GetKeyState(VK_MENU) & 0x8000) != 0);
			CView* pActiveView = GetActiveView();
			if (!hasModifier && !IsTypingFocus())
			{
				// Blender-style shortcuts
				switch (pMsg->wParam)
				{
				case 'G':
					OnModeMove();
					return TRUE;
				case 'R':
					OnModeRotate();
					return TRUE;
				case 'S':
					OnModeScale();
					return TRUE;
				case 'Q':
					OnModeSelect();
					return TRUE;
				case VK_SPACE:
				{
					CSOEditDoc *pPlayDoc = (CSOEditDoc *)GetActiveDocument();
					if (pPlayDoc && pPlayDoc->m_AnimBone)
					{
						if (pPlayDoc->m_Animate) {OnPause();} else {OnPlay();}
					}
					return TRUE;
				}
				case VK_LEFT:
					OnBack();
					return TRUE;
				case VK_RIGHT:
					OnStep();
					return TRUE;
				case VK_HOME:
					GoToFrame(this, 0);
					return TRUE;
				case VK_END:
					GoToFrame(this, 1000000);
					return TRUE;
				case VK_F2:
					SendMessage(WM_COMMAND, ID_HELP_SHORTCUTS, 0);
					return TRUE;
				}
			}
		}
		CEdit *pEdit = (CEdit *)m_wndAnimBox.GetDlgItem(IDC_SPEED);
		CEdit *pEdit2 = (CEdit *)m_wndToolTab.GetDlgItem(IDC_FINDING_COMPONENT);
		if (pMsg->message == WM_KEYDOWN && !IsTypingFocus() && !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000))
		{
			if (pMsg->wParam == 'I')
			{
				if (GetActiveDocument())
				{
					SendMessage(WM_COMMAND, IDC_ANMBOX_INSERT_FRAME, 0);
				}
				return TRUE;
			}
			if (pMsg->wParam == VK_DELETE)
			{
				if (GetActiveDocument())
				{
					SendMessage(WM_COMMAND, IDC_ANMBOX_DELETE_FRAME, 0);
				}
				return TRUE;
			}
		}
		if(pMsg -> hwnd == pEdit -> GetSafeHwnd())
		{
			if(pMsg -> wParam == VK_DOWN || pMsg -> wParam == VK_UP)
			{
				action = true;
				up_down = pMsg -> wParam == VK_UP;
			}
			else
			{
				short wheelDelta = GET_WHEEL_DELTA_WPARAM(pMsg -> wParam);
				if(wheelDelta > 0)
				{
					action = true;
					up_down = true;
				}
				else
				if(wheelDelta < 0)
				{
					action = true;
					up_down = false;
				}
			}
			if(action)
			{
				CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
				pDoc -> m_Speed += (up_down) ? 0.1 : -0.1;
				pDoc -> m_Speed = max(-10, min(pDoc -> m_Speed, 10));
				pDoc -> m_Speed = round(pDoc -> m_Speed);
				char szWindowText[_MAX_PATH];
				sprintf(szWindowText, "%0.1f", pDoc -> m_Speed);
				CWnd *pWnd = (CWnd *)m_wndAnimBox.GetDlgItem(IDC_SPEED);
				pWnd -> SetWindowText(szWindowText);
				pMsg -> wParam = NULL;
			}
		}
		else
		if(pMsg -> hwnd == pEdit2 -> GetSafeHwnd())
		{
			if(pMsg -> wParam == VK_RETURN)
			{
				CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
				if(pDoc)
					{pDoc -> ReturnFindingComponent(true);}
			}
		}
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnGridOff() 
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    pDoc -> m_grid = false;
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_GRID_ON_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_GRID_OFF_AL, MF_CHECKED);
    pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnGridOn() 
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    pDoc -> m_grid = true;
	CMenu *pMenu = GetMenu();
	CMenu* pPopup = pMenu -> GetSubMenu(6);
	ASSERT(pPopup != NULL);
		pPopup -> CheckMenuItem(ID_GRID_OFF_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_GRID_ON_AL, MF_CHECKED);
    pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnGridSmaller() 
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
    if(pDoc -> m_GridSize > 5)
		{pDoc -> m_GridSize -= 5;}
    pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnGridLarger() 
{
	CSOEditDoc *pDoc = (CSOEditDoc *)GetActiveDocument();
	if(pDoc -> m_GridSize < 50)
		{pDoc -> m_GridSize += 5;}
    pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CMainFrame::OnAppExitAl()
	{OnClose();}

void CMainFrame::GetMessageString(UINT nID, CString & rMessage) const
{
		if(nID == AFX_IDS_IDLEMESSAGE)
			{rMessage = _T("G Move    R Rotate    S Scale    Q Select    Space Play/Pause    Left/Right Frame    I Insert frame    F2 All shortcuts");}
		else
			{CFrameWnd::GetMessageString(nID, rMessage);}
}

void CMainFrame::OnHelpShortcuts()
{
	MessageBoxA("Viewport\n  Q  Select      G  Move      R  Rotate      S  Scale\n  Mouse wheel  Zoom      Middle drag  Pan\n\nAnimation\n  Space  Play / Pause\n  Left / Right  Previous / next frame\n  Home / End  First / last frame\n  I  Insert frame      Delete  Delete frame\n\nGeneral\n  Ctrl+N / O / S  New / Open / Save\n  F2  This list", "Keyboard shortcuts", MB_ICONINFORMATION);
}
