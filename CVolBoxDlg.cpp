// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "CVolBoxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolBoxDlg dialog


CVolBoxDlg::CVolBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolBoxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolBoxDlg)
	m_BoxHeight = 0.0f;
	m_BoxLength = 0.0f;
	m_BoxWidth = 0.0f;
	//}}AFX_DATA_INIT
}


void CVolBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolBoxDlg)
	DDX_Text(pDX, IDC_BOX_HEIGHT, m_BoxHeight);
	DDX_Text(pDX, IDC_BOX_LENGTH, m_BoxLength);
	DDX_Text(pDX, IDC_BOX_WIDTH, m_BoxWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVolBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CVolBoxDlg)
	ON_BN_CLICKED(IDC_BOX_FIT, OnBoxFit)
	ON_EN_CHANGE(IDC_BOX_HEIGHT, OnChangeBoxHeight)
	ON_EN_CHANGE(IDC_BOX_LENGTH, OnChangeBoxLength)
	ON_EN_CHANGE(IDC_BOX_WIDTH, OnChangeBoxWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolBoxDlg message handlers

BOOL CVolBoxDlg::PreTranslateMessage(MSG* pMsg)
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

void CVolBoxDlg::OnBoxFit()
{
	CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
	ASSERT(pMainFrame);
	CSOEditDoc *pDoc = (CSOEditDoc *)pMainFrame -> GetActiveDocument();
	ASSERT(pDoc);
	CVolProp *pVolProp = (CVolProp *)pDoc -> m_VolProp;
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
			m_BoxLength = sizes[0];
			m_BoxHeight = sizes[1];
			m_BoxWidth = sizes[2];
			UpdateData(false);
		}
		else
		{
			#ifdef ALTERNATIVE_LANG
				MessageBox("The bone does not have a mesh.", "ERROR: CVolBoxDlg::OnBoxFit" , MB_ICONHAND);
			#else
				MessageBox("Кость не обладает мэшем.", "ERROR: CVolBoxDlg::OnBoxFit" , MB_ICONHAND);
			#endif
		}
	}
	else
	{
		#ifdef ALTERNATIVE_LANG
		MessageBox("There is no link to the bone.", "ERROR: CVolBoxDlg::OnBoxFit", MB_ICONHAND);
		#else
		MessageBox("Нет линковки к кости.", "ERROR: CVolBoxDlg::OnBoxFit", MB_ICONHAND);
		#endif
	}
}

void CVolBoxDlg::OnChangeBoxHeight()
{
	UpdateData(true);
	OnEditChange();
}

void CVolBoxDlg::OnChangeBoxLength()
{
	UpdateData(true);
	OnEditChange();
}

void CVolBoxDlg::OnChangeBoxWidth()
{
	UpdateData(true);
	OnEditChange();
}

void CVolBoxDlg::OnEditChange()
{
	char bonename[_MAX_PATH];
	memset(bonename, '\0', _MAX_PATH);
	m_BoxHeight = m_BoxLength = m_BoxWidth = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BOX_HEIGHT);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	СommaFixer(bonename);
	ForbiddenSymbolFixer(bonename);
	if(isFloat(bonename))
		{m_BoxHeight = (float)atof(bonename);}
	memset(bonename, '\0', _MAX_PATH);
	pEdit = (CEdit *)GetDlgItem(IDC_BOX_LENGTH);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	СommaFixer(bonename);
	ForbiddenSymbolFixer(bonename);
	if(isFloat(bonename))
		{m_BoxLength = (float)atof(bonename);}
	memset(bonename, '\0', _MAX_PATH);
	pEdit = (CEdit *)GetDlgItem(IDC_BOX_WIDTH);
	ASSERT(pEdit);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	СommaFixer(bonename);
	ForbiddenSymbolFixer(bonename);
	if(isFloat(bonename))
		{m_BoxWidth = (float)atof(bonename);}
}

BOOL CVolBoxDlg::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL2_1);
		pWnd -> SetWindowText("Length:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL2_2);
		pWnd -> SetWindowText("Width:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL2_3);
		pWnd -> SetWindowText("Height:");
	#endif
 	return TRUE;
}