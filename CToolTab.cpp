// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "mainfrm.h"
#include "CToolTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolTab dialog


//CToolTab::CToolTab(CWnd* pParent /*=NULL*/)
//	: CDialog(CToolTab::IDD, pParent)
CToolTab::CToolTab()
{
	//{{AFX_DATA_INIT(CToolTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_MyTab = NULL;
}

CToolTab::~CToolTab()
{
	m_ModelTree.DestroyWindow();
	m_VolPropDlg.DestroyWindow();
}

void CToolTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolTab)
	DDX_Control(pDX, IDC_TOOLTAB, m_TabCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CToolTab, CDialogBar)
	//{{AFX_MSG_MAP(CToolTab)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_INITDIALOG, OnInitDialog )    // <-- Add this line.
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTab message handlers

LONG CToolTab::OnInitDialog(UINT wParam, LONG lParam)
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	m_MyTab = (CTabCtrl *)GetDlgItem(IDC_TOOLTAB);
	#ifdef ALTERNATIVE_LANG
		m_MyTab -> InsertItem(0, "Model");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_TT_1);
		pWnd -> SetWindowText("Find the bone:");
	#else
		m_MyTab -> InsertItem(0, "Модель");
	#endif
	m_ModelTree.Create(IDD_MODEL_TREE, GetParent());
	m_ModelTree.ShowWindow(SW_HIDE);
	m_VolPropDlg.Create(IDD_VOL_PROPS, GetParent());//забудьте про здравый смысл
	ActivateTabDialogs();
	if(!UpdateData(FALSE))
		{TRACE0("Warning: UpdateData failed during dialog init.\n");}
	return bRet;
}

void CToolTab::ActivateTabDialogs()
{
    int nSel = m_MyTab -> GetCurSel();
    CRect l_rectClient;
    CRect l_rectWnd;
    CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
    m_MyTab -> GetClientRect(l_rectClient);
    m_MyTab -> AdjustRect(FALSE, l_rectClient);
    m_MyTab -> GetWindowRect(l_rectWnd);
    GetParent() -> ScreenToClient(l_rectWnd);
    l_rectClient.OffsetRect(l_rectWnd.left, l_rectWnd.top);
	m_ModelTree.SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_SHOWWINDOW);
}

void CToolTab::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
    if(m_MyTab)
		{ActivateTabDialogs();}
}
