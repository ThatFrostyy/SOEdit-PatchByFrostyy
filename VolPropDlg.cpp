// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "mainfrm.h"
#include "SOEditDoc.h"
#include "VolPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolPropDlg dialog

CVolPropDlg::CVolPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolPropDlg)
	m_Name = _T("");
	m_Bone = _T("");
	m_VolType = -1;
	//}}AFX_DATA_INIT
	m_Box[0] = 0.0f;
	m_Box[1] = 0.0f;
	m_Box[2] = 0.0f;
	m_Cylinder[0] = 0.0f;
	m_Cylinder[1] = 0.0f;
	m_pVolume = NULL;
}

CVolPropDlg::~CVolPropDlg()
{
    m_VolBoxDlg.DestroyWindow();
    m_VolCylDlg.DestroyWindow();
    m_VolPolyDlg.DestroyWindow();
}

void CVolPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolPropDlg)
	DDX_Text(pDX, IDC_VOLNAME, m_Name);
	DDX_Text(pDX, IDC_VOL_BONE, m_Bone);
	DDX_Radio(pDX, IDC_VOLTYPE1, m_VolType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVolPropDlg, CDialog)
	//{{AFX_MSG_MAP(CVolPropDlg)
	ON_BN_CLICKED(IDC_VOLTYPE1, OnVoltype1)
	ON_BN_CLICKED(IDC_VOLTYPE2, OnVoltype2)
	ON_BN_CLICKED(IDC_VOLTYPE3, OnVoltype3)
	ON_LBN_SELCHANGE(IDC_VOL_BONES, OnSelchangeVolBones)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolPropDlg message handlers

void CVolPropDlg::LoadValues()
{
	m_Name = m_pVolume -> m_Name;
	m_Bone = m_pVolume -> m_Bone;
	if(m_pVolume -> m_Box)
	{
		m_Box[0] = m_pVolume -> m_Box[0];
		m_Box[1] = m_pVolume -> m_Box[1];
		m_Box[2] = m_pVolume -> m_Box[2];
	}
	if(m_pVolume -> m_Cylinder)
	{
		m_Cylinder[0] = m_pVolume -> m_Cylinder[0];
		m_Cylinder[1] = m_pVolume -> m_Cylinder[1];
	}
	if(m_pVolume -> m_Type == VT_PLY)
	{
		char *delim = strrchr(m_pVolume -> m_PolyhedronName, '\\');
		if(!delim)
		{
			delim = strrchr(m_pVolume -> m_PolyhedronName, '/');
		}
		if(delim)
		{
			delim++;
			m_PolyhedronName = delim;
		}
		else
			{m_PolyhedronName = m_pVolume -> m_PolyhedronName;}
	}
	switch(m_pVolume -> m_Type)
	{
		case VT_PLY:      // Activate polyhedron tab
		{
			m_VolType = 2;
			OnVoltype3();
		}
		break;
		case VT_BOX:      // Activate box tab
		{
			m_VolType = 0;
			OnVoltype1();
		}
		break;
		case VT_CYLINDER: // Activate cylinder tab
		{
			m_VolType = 1;
			OnVoltype2();
		}
		break;
	}
	UpdateData(false);
}

void CVolPropDlg::OnVoltype1()
{
	if(m_Created[1])
		{HideCyl();}
	if(m_Created[2])
		{HidePoly();}
	ShowBox();
}

void CVolPropDlg::OnVoltype2()
{
	if(m_Created[0])
		{HideBox();}
	if(m_Created[2])
		{HidePoly();}
	ShowCyl();
}

void CVolPropDlg::OnVoltype3()
{
	if(m_Created[0])
		{HideBox();}
	if(m_Created[1])
		{HideCyl();}
	ShowPoly();
}

void CVolPropDlg::BuildBoneList(CBone *basis)
{
    CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
    pList -> ResetContent();
    AddBoneToList(basis);
}

void CVolPropDlg::AddBoneToList(CBone *basis)
{
    CBone *child;
    CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
    pList -> AddString(basis -> m_Name);
	if(!stricmp(m_Bone.GetBuffer(1), basis -> m_Name))
	{
		int idx = pList -> FindStringExact(1, basis -> m_Name);
		if(idx != LB_ERR)
			{pList -> SetCurSel(idx);}
	}
	if((child = basis -> m_ChildFirst) != NULL)
		{AddBoneToList(child);}
	if(basis -> next)
		{AddBoneToList(basis -> next);}
}

BOOL CVolPropDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	if(pDoc)
	{
		if(pDoc -> m_SelVolume)
			{m_pVolume = pDoc -> m_Model -> m_VolumeList -> FindByName(pDoc -> m_SelVolume);}
	}
	if(m_pVolume)
		{m_VolType = m_pVolume -> m_Type;}
	m_VolCylDlg.Create(IDD_VOL_CYL, this);
	m_VolCylDlg.ShowWindow(SW_HIDE);
	m_VolPolyDlg.Create(IDD_VOL_POLY, this);
	m_VolPolyDlg.ShowWindow(SW_HIDE);
	m_VolBoxDlg.Create(IDD_VOL_BOX, this);
	m_VolBoxDlg.ShowWindow(SW_HIDE);
	switch(m_VolType)
	{
		case VT_PLY:      // Activate polyhedron tab
			{OnVoltype3();}
		break;
		case VT_BOX:      // Activate box tab
			{OnVoltype1();}
		break;
		case VT_CYLINDER: // Activate cylinder tab
			{OnVoltype2();}
		break;
	}
	if(pDoc)
	{
		if(pDoc -> m_Model)
		{
			if(pDoc -> m_Model -> m_skeleton)
			{
				if(pDoc -> m_Model -> m_skeleton -> m_bonelist)
					{BuildBoneList(pDoc -> m_Model -> m_skeleton -> m_bonelist);}
			}
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CVolPropDlg::Activate()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	if(pDoc -> m_SelVolume)
		{m_pVolume = pDoc->m_Model -> m_VolumeList -> FindByName(pDoc -> m_SelVolume);}
	if(m_pVolume)
		{m_VolType = m_pVolume -> m_Type;}
}

void CVolPropDlg::HideBox()
{
	m_VolBoxDlg.UpdateData(true);
	m_Box[0] = m_VolBoxDlg.m_BoxLength;
	m_Box[1] = m_VolBoxDlg.m_BoxWidth;
	m_Box[2] = m_VolBoxDlg.m_BoxHeight;
	m_VolBoxDlg.ShowWindow(SW_HIDE);
	m_Created[0] = false;
}

void CVolPropDlg::HideCyl()
{
	m_VolCylDlg.UpdateData(true);
	m_Cylinder[0] = m_VolCylDlg.m_CylDiameter;
	m_Cylinder[1] = m_VolCylDlg.m_CylLength;
	m_VolCylDlg.ShowWindow(SW_HIDE);
	m_Created[1] = false;
}

void CVolPropDlg::HidePoly()
{
	m_VolPolyDlg.UpdateData(true);
	m_PolyhedronName = m_VolPolyDlg.m_PolyName;
	m_VolPolyDlg.ShowWindow(SW_HIDE);
	m_Created[2] = false;
}

void CVolPropDlg::ShowBox()
{
	CRect l_rectClient;
	CRect l_rectWnd;
	m_Created[0] = true;
	CWnd *pWnd = GetDlgItem(IDC_VOLFRAME);
	pWnd -> GetClientRect(l_rectClient);
	//AdjustRect(FALSE,l_rectClient);
	pWnd -> GetWindowRect(l_rectWnd);
	pWnd -> GetParent() -> ScreenToClient(l_rectWnd);
	l_rectClient.OffsetRect(l_rectWnd.left, l_rectWnd.top);
	m_VolBoxDlg.m_BoxLength = m_Box[0];
	m_VolBoxDlg.m_BoxWidth = m_Box[1];
	m_VolBoxDlg.m_BoxHeight = m_Box[2];
	//m_VolBoxDlg.Create(IDD_VOL_BOX,this);
	m_VolBoxDlg.ShowWindow(SW_SHOW);
	m_VolBoxDlg.UpdateData(false);
	m_VolBoxDlg.SetWindowPos(&wndTop, l_rectClient.left + 2, l_rectClient.top + 2, l_rectClient.Width() - 4, l_rectClient.Height() - 4, SWP_SHOWWINDOW);
}

void CVolPropDlg::ShowCyl()
{
	CRect l_rectClient;
	CRect l_rectWnd;
	m_Created[1] = true;
	CWnd *pWnd = GetDlgItem(IDC_VOLFRAME);
	pWnd -> GetClientRect(l_rectClient);
	//AdjustRect(FALSE,l_rectClient);
	pWnd -> GetWindowRect(l_rectWnd);
	pWnd -> GetParent() -> ScreenToClient(l_rectWnd);
	l_rectClient.OffsetRect(l_rectWnd.left, l_rectWnd.top);
	m_VolCylDlg.m_CylDiameter = m_Cylinder[0];
	m_VolCylDlg.m_CylLength = m_Cylinder[1];
	//m_VolCylDlg.Create(IDD_VOL_CYL,this);
	m_VolCylDlg.ShowWindow(SW_SHOW);
	m_VolCylDlg.UpdateData(false);
	m_VolCylDlg.SetWindowPos(&wndTop, l_rectClient.left + 2, l_rectClient.top + 2, l_rectClient.Width() - 4, l_rectClient.Height() - 4, SWP_SHOWWINDOW);
}

void CVolPropDlg::ShowPoly()
{
	CRect l_rectClient;
	CRect l_rectWnd;
	m_Created[2] = true;
	CWnd *pWnd = GetDlgItem(IDC_VOLFRAME);
	pWnd -> GetClientRect(l_rectClient);
	//AdjustRect(FALSE,l_rectClient);
	pWnd -> GetWindowRect(l_rectWnd);
	pWnd -> GetParent() -> ScreenToClient(l_rectWnd);
	l_rectClient.OffsetRect(l_rectWnd.left, l_rectWnd.top);
	m_VolPolyDlg.m_PolyName = m_PolyhedronName;
	//m_VolPolyDlg.Create(IDD_VOL_POLY,this);
	m_VolPolyDlg.ShowWindow(SW_SHOW);
	m_VolPolyDlg.UpdateData(false);
	m_VolPolyDlg.SetWindowPos(&wndTop, l_rectClient.left + 2, l_rectClient.top + 2, l_rectClient.Width() - 4, l_rectClient.Height() - 4, SWP_SHOWWINDOW);
}

void CVolPropDlg::OnSelchangeVolBones()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
	int nSel = pList -> GetCurSel();
	UpdateData(true);
	pList -> GetText(nSel, m_Bone);
	UpdateData(false);
}


