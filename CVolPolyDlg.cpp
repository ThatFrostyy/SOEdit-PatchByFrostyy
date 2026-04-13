// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "CVolPolyDlg.h"
#include "mainfrm.h"
#include "SOEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolPolyDlg dialog


CVolPolyDlg::CVolPolyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVolPolyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolPolyDlg)
	m_PolyName = _T("");
	//}}AFX_DATA_INIT
}


void CVolPolyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolPolyDlg)
	DDX_Text(pDX, IDC_VOL_POLY_NAME, m_PolyName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVolPolyDlg, CDialog)
	//{{AFX_MSG_MAP(CVolPolyDlg)
	ON_BN_CLICKED(IDC_VOL_CHOOSE, OnVolChoose)
	ON_EN_CHANGE(IDC_VOL_POLY_NAME, OnM_Name)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolPolyDlg message handlers

BOOL CVolPolyDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_RETURN || pMsg -> wParam == VK_ESCAPE)
			{pMsg -> wParam = NULL;}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CVolPolyDlg::OnVolChoose() 
   {
    char modelpath[_MAX_PATH], *ptr;
    strcpy(modelpath, GetModelPath());
    ptr = strrchr(modelpath, '/');
    if(!ptr)
		{ptr = strrchr(modelpath, '\\');}
    if(ptr)
		{*ptr = NULL;}
	#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "VOL-file (*.vol)|*.vol|";
		static char szTitle[] = "Opening a volume file";
	#else
		static char szFilter[] = "VOL-файл (*.vol)|*.vol|";
		static char szTitle[] = "Открытие волюм-файла";
	#endif
    CFileDialog dlg(TRUE, "*.vol", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFilter, AfxGetMainWnd());
    dlg.m_ofn.lpstrTitle = szTitle;
    dlg.m_ofn.lpstrInitialDir = modelpath;
    if(dlg.DoModal() == IDOK)
	{
		strcpy(modelpath, dlg.m_ofn.lpstrFile);
        m_PolyName = modelpath;
        CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_POLY_NAME);
        pEdit -> SetWindowText(modelpath);
	}
}

void CVolPolyDlg::OnM_Name()
{
	char text_buff[_MAX_PATH];
	memset(text_buff, 0, _MAX_PATH);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_POLY_NAME);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	m_PolyName = text_buff;
}

BOOL CVolPolyDlg::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_VL4_1);
		pWnd -> SetWindowText("Volume-file (VOL):");
		pWnd = (CWnd *)GetDlgItem(IDC_VOL_CHOOSE);
		pWnd -> SetWindowText("Choose");
	#endif
 	return TRUE;
}