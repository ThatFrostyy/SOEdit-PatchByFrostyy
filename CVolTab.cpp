// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "CVolTab.h"
#include "CVolProp.h"
#include "mainfrm.h"
#include "SOEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVolTab

CVolTab::CVolTab()
   {
    m_DialogID[0] = IDD_VOL_POLY;
    m_DialogID[1] = IDD_VOL_BOX;
    m_DialogID[2] = IDD_VOL_CYL;

    m_Dialog[0] = false;
    m_Dialog[1] = false;
    m_Dialog[2] = false;

    m_BoxLength = 0.0f;
    m_BoxWidth = 0.0f;
    m_BoxHeight = 0.0f;

    m_CylLength = 0.0f;
    m_CylDiameter = 0.0f;
    m_CylRadioSet = false;

    m_VolType = 0;
   }

CVolTab::~CVolTab()
{
	if(m_Dialog[0])
		{m_VolPolyDlg.DestroyWindow();}
	if(m_Dialog[1])
		{m_VolBoxDlg.DestroyWindow();}
	if(m_Dialog[2])
		{m_VolCylDlg.DestroyWindow();}
}


BEGIN_MESSAGE_MAP(CVolTab, CTabCtrl)
	//{{AFX_MSG_MAP(CVolTab)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolTab message handlers

void CVolTab::ActivateTabDialogs()
   {
    int nSel = GetCurSel();

    CRect l_rectClient;
    CRect l_rectWnd;

    GetClientRect(l_rectClient);
    AdjustRect(FALSE,l_rectClient);
    GetWindowRect(l_rectWnd);
    GetParent()->ScreenToClient(l_rectWnd);
    l_rectClient.OffsetRect(l_rectWnd.left,l_rectWnd.top);

    switch(nSel)
       {
        case 0:
             m_VolType = VT_PLY;
             m_VolPolyDlg.m_PolyName = m_PolyName;
             if (false == m_Dialog[nSel])
                {
                 m_VolPolyDlg.Create(m_DialogID[nSel],GetParent());
                 m_Dialog[nSel] = true;
                }
             m_VolPolyDlg.UpdateData(false);
             m_VolPolyDlg.ShowWindow(SW_SHOW);
             m_VolPolyDlg.SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_SHOWWINDOW);
             break;
        case 1:
             m_VolType = VT_BOX;
             m_VolBoxDlg.m_BoxLength = m_BoxLength;
             m_VolBoxDlg.m_BoxWidth = m_BoxWidth;
             m_VolBoxDlg.m_BoxHeight = m_BoxHeight;
             if (false == m_Dialog[nSel])
                {
                 m_VolBoxDlg.Create(m_DialogID[nSel],GetParent());
                 m_Dialog[nSel] = true;
                }
             m_VolBoxDlg.UpdateData(false);
             m_VolBoxDlg.ShowWindow(SW_SHOW);
             m_VolBoxDlg.SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_SHOWWINDOW);
             break;
        case 2:
             m_VolType = VT_CYLINDER;
             m_VolCylDlg.m_CylLength = m_CylLength;
             m_VolCylDlg.m_CylDiameter = m_CylDiameter;
             if (false == m_CylRadioSet)
                {
                 m_VolCylDlg.m_XYZ = 0;
                 m_CylRadioSet = true;
                }
             if (false == m_Dialog[nSel])
                {
                 m_VolCylDlg.Create(m_DialogID[nSel],GetParent());
                 m_Dialog[nSel] = true;
                }
             m_VolCylDlg.UpdateData(false);
             m_VolCylDlg.ShowWindow(SW_SHOW);
             m_VolCylDlg.SetWindowPos(&wndTop, l_rectClient.left, l_rectClient.top, l_rectClient.Width(), l_rectClient.Height(), SWP_SHOWWINDOW);
             break;
       }
   }


void CVolTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
   {
    ActivateTabDialogs();
	
	*pResult = 0;
   }

void CVolTab::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
   {
    int nSel = GetCurSel();

    switch(nSel)
       {
        case 0:
             //m_VolPolyDlg.DestroyWindow();
             m_VolPolyDlg.UpdateData(true);
             m_PolyName = m_VolPolyDlg.m_PolyName;
             m_VolPolyDlg.ShowWindow(SW_HIDE);
             break;
        case 1:
             //m_VolBoxDlg.DestroyWindow();
             m_VolBoxDlg.UpdateData(true);
             m_BoxLength = m_VolBoxDlg.m_BoxLength;
             m_BoxWidth = m_VolBoxDlg.m_BoxWidth;
             m_BoxHeight = m_VolBoxDlg.m_BoxHeight;
             m_VolBoxDlg.ShowWindow(SW_HIDE);
             break;
        case 2:
             //m_VolCylDlg.DestroyWindow();
             m_VolCylDlg.UpdateData(true);
             m_CylLength = m_VolCylDlg.m_CylLength;
             m_CylDiameter = m_VolCylDlg.m_CylDiameter;
             m_VolCylDlg.ShowWindow(SW_HIDE);
             break;
       }

	 *pResult = 0;
   }

void CVolTab::PostNcDestroy() 
{
	CTabCtrl::PostNcDestroy();
    m_PolyName = m_VolPolyDlg.m_PolyName;
    m_BoxLength = m_VolBoxDlg.m_BoxLength;
    m_BoxWidth = m_VolBoxDlg.m_BoxWidth;
    m_BoxHeight = m_VolBoxDlg.m_BoxHeight;
    m_CylLength = m_VolCylDlg.m_CylLength;
    m_CylDiameter = m_VolCylDlg.m_CylDiameter;
}