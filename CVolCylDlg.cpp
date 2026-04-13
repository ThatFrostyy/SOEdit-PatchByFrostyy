// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "CVolCylDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolCylDlg dialog


CVolCylDlg::CVolCylDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolCylDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolCylDlg)
	m_CylDiameter = 0.0f;
	m_CylLength = 0.0f;
	m_XYZ = -1;
	//}}AFX_DATA_INIT
    m_XYZval = 0;
}


void CVolCylDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolCylDlg)
	DDX_Text(pDX, IDC_CYL_DIAMETER, m_CylDiameter);
	DDX_Text(pDX, IDC_CYL_LENGTH, m_CylLength);
	DDX_Radio(pDX, IDC_XYZ1, m_XYZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVolCylDlg, CDialog)
	//{{AFX_MSG_MAP(CVolCylDlg)
	ON_BN_CLICKED(IDC_CYL_FIT, OnCylFit)
	ON_BN_CLICKED(IDC_XYZ1, OnXyz1)
	ON_BN_CLICKED(IDC_XYZ2, OnXyz2)
	ON_BN_CLICKED(IDC_XYZ3, OnXyz3)
	ON_EN_CHANGE(IDC_CYL_LENGTH, OnEditChange)
	ON_EN_CHANGE(IDC_CYL_DIAMETER, OnEditChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolCylDlg message handlers

BOOL CVolCylDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_RETURN || pMsg -> wParam == VK_ESCAPE)
			{pMsg -> wParam = NULL;}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

#include "MainFrm.h"
#include "SOEditDoc.h"
#include "CVolProp.h"
#include "CBone.h"

void CVolCylDlg::OnCylFit()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(pMainFrame);
	CSOEditDoc *pDoc = (CSOEditDoc *)pMainFrame->GetActiveDocument();
	ASSERT(pDoc);
	CVolProp *pVolProp = (CVolProp *)pDoc->m_VolProp;
	ASSERT(pVolProp);
	char bonename[_MAX_PATH];
	memset(bonename, '\0', _MAX_PATH);
	CEdit *pEdit = (CEdit *)pVolProp -> GetDlgItem(IDC_VOL_BONE);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	if(strlen(bonename))
	{
		CBone *pBone = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBone(bonename);
		v3_t sizes;
		#ifdef ALTERNATIVE_LANG
			MessageBox("Auto-detection of geometry dimensions is broken.", "ERROR: CVolBoxDlg::OnBoxFit", MB_ICONHAND);
		#else
			MessageBox("Автоопределение размеров геометрии - сломано.", "ERROR: CVolBoxDlg::OnBoxFit", MB_ICONHAND);
		#endif
		if(pBone -> GetBBox(&sizes))
		{
			m_XYZ = m_XYZval;
			switch(this -> m_XYZval)
			{
				case 0:
				{
					this -> m_CylLength = sizes[0];
					if(sizes[1] > sizes[2])
						{m_CylDiameter = sizes[1];}
					else
						{m_CylDiameter = sizes[2];}
				}
				break;
				case 1:
				{
					this -> m_CylLength = sizes[1];
					if(sizes[0] > sizes[2])
						{m_CylDiameter = sizes[0];}
					else
						{m_CylDiameter = sizes[2];}
				}
				break;
				case 2:
				{
					this -> m_CylLength = sizes[2];
					if(sizes[0] > sizes[1])
						{m_CylDiameter = sizes[0];}
					else
						{m_CylDiameter = sizes[1];}
				}
				break;
			}
			UpdateData(false);
		}
		else
		{
			#ifdef ALTERNATIVE_LANG
				MessageBox("The bone does not have a mesh.", "ERROR: CVolCylDlg::OnCylFit" , MB_ICONHAND);
			#else
			MessageBox("Кость не обладает мэшем.", "ERROR: CVolCylDlg::OnCylFit" , MB_ICONHAND);
			#endif
		}
	}
	else
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("There is no link to the bone.", "ERROR: CVolCylDlg::OnBoxFit", MB_ICONHAND);
		#else
		MessageBox("Нет линковки к кости.", "ERROR: CVolCylDlg::OnBoxFit", MB_ICONHAND);
		#endif
	}
}

void CVolCylDlg::OnXyz1()
	{m_XYZval = 0;}

void CVolCylDlg::OnXyz2()
	{m_XYZval = 1;}

void CVolCylDlg::OnXyz3()
	{m_XYZval = 2;}

void CVolCylDlg::OnEditChange()
{
	char bonename[_MAX_PATH];
	memset(bonename, '\0', _MAX_PATH);
	m_CylDiameter = m_CylLength = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_CYL_DIAMETER);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	СommaFixer(bonename);
	ForbiddenSymbolFixer(bonename);
	if(isFloat(bonename))
		{m_CylDiameter = (float)atof(bonename);}
	memset(bonename, '\0', _MAX_PATH);
	pEdit = (CEdit *)GetDlgItem(IDC_CYL_LENGTH);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	СommaFixer(bonename);
	ForbiddenSymbolFixer(bonename);
	if(isFloat(bonename))
		{m_CylLength = (float)atof(bonename);}
}

BOOL CVolCylDlg::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL3_1);
		pWnd -> SetWindowText("Length:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL3_2);
		pWnd -> SetWindowText("Diameter:");
		pWnd = (CWnd *)GetDlgItem(IDC_CYLAXIS);
		pWnd -> SetWindowText("Axis");
	#endif
 	return TRUE;
}