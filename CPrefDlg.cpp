// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "mainfrm.h"
#include "CPrefDlg.h"
#include "CDlgGetPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrefDlg dialog


CPrefDlg::CPrefDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrefDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrefDlg)
	m_ResPath[0] = _T("");
	m_ResPath[1] = _T("");
	m_ResPath[2] = _T("");
	m_ResPath[3] = _T("");
	m_ResPath[4] = _T("");
	m_ResPath[5] = _T("");
	//}}AFX_DATA_INIT
}


void CPrefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrefDlg)
	DDX_Text(pDX, IDC_RESPATH, m_ResPath[0]);
	DDX_Text(pDX, IDC_RESPATH2, m_ResPath[1]);
	DDX_Text(pDX, IDC_RESPATH3, m_ResPath[2]);
	DDX_Text(pDX, IDC_RESPATH4, m_ResPath[3]);
	DDX_Text(pDX, IDC_RESPATH5, m_ResPath[4]);
	DDX_Text(pDX, IDC_RESPATH6, m_ResPath[5]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrefDlg, CDialog)
	//{{AFX_MSG_MAP(CPrefDlg)
	ON_BN_CLICKED(IDC_GDBROWSE, OnGdbrowse)
	ON_BN_CLICKED(IDC_GDBROWSE2, OnGdbrowse2)
	ON_BN_CLICKED(IDC_GDBROWSE3, OnGdbrowse3)
	ON_BN_CLICKED(IDC_GDBROWSE4, OnGdbrowse4)
	ON_BN_CLICKED(IDC_GDBROWSE5, OnGdbrowse5)
	ON_BN_CLICKED(IDC_GDBROWSE6, OnGdbrowse6)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrefDlg message handlers

void CPrefDlg::OnGdbrowse()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose the game catalog");
	#else
		PathDlg.SetTitle("Выбери каталог игры");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[0] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH);
		pEdit -> SetWindowText(m_ResPath[0].GetBuffer(1));
	}
}

void CPrefDlg::OnGdbrowse2()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose a mod catalog");
	#else
		PathDlg.SetTitle("Выбери каталог мода");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[1] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH2);
		pEdit -> SetWindowText(m_ResPath[1].GetBuffer(1));
	}
}

void CPrefDlg::OnGdbrowse3()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose a mod catalog");
	#else
		PathDlg.SetTitle("Выбери каталог мода");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[2] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH3);
		pEdit -> SetWindowText(m_ResPath[2].GetBuffer(1));
	}
}

void CPrefDlg::OnGdbrowse4()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose a mod catalog");
	#else
		PathDlg.SetTitle("Выбери каталог мода");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[3] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH4);
		pEdit -> SetWindowText(m_ResPath[3].GetBuffer(1));
	}
}

void CPrefDlg::OnGdbrowse5()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose a mod catalog");
	#else
		PathDlg.SetTitle("Выбери каталог мода");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[4] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH5);
		pEdit -> SetWindowText(m_ResPath[4].GetBuffer(1));
	}
}

void CPrefDlg::OnGdbrowse6()
{
	CDlgGetPath PathDlg;
	#ifdef ALTERNATIVE_LANG
		PathDlg.SetTitle("Choose a mod catalog");
	#else
		PathDlg.SetTitle("Выбери каталог мода");
	#endif
	if(PathDlg.DoModal() == IDOK)
	{
		m_ResPath[5] = PathDlg.GetPath();
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_RESPATH6);
		pEdit -> SetWindowText(m_ResPath[5].GetBuffer(1));
	}
}

BOOL CPrefDlg::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Preferences");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_PREF_1);
		pWnd -> SetWindowText("The path to the game");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_PREF_2);
		pWnd -> SetWindowText("Paths to Mods");
		pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#endif
 	return TRUE;
}