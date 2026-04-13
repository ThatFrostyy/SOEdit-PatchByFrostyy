// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "mainfrm.h"
#include "CColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColors dialog


CColors::CColors(CWnd* pParent /*=NULL*/)
	: CDialog(CColors::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColors)
	m_Selected = _T("");
	//}}AFX_DATA_INIT
}


void CColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColors)
	DDX_Control(pDX, IDC_ELEMENTS, m_Elements);
	DDX_Text(pDX, IDC_ELEMENT, m_Selected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColors, CDialog)
	//{{AFX_MSG_MAP(CColors)
	ON_LBN_SELCHANGE(IDC_ELEMENTS, OnSelchangeElements)
	ON_BN_CLICKED(IDC_COLOR_SEL, OnColorSel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColors message handlers

char *Elements[] = {"2D Background", "3D Background", "Grid", "Grid 3D", "Axis",
                    "Mesh (PLY)", "Mesh (PLY) Selected", "Volume", "Volume Selected",
                    "Entity", "Entity Selected", "Obstacle", "Obstacle Selected",
                    "Area", "Area Selected", "Mesh (VOL) Front", "Mesh (VOL) Rear",
					"Mesh (VOL) Right", "Mesh (VOL) Left", "Mesh (VOL) Top", "Mesh (VOL) Bottom", 0};


BOOL CColors::OnInitDialog()
{
	CDialog::OnInitDialog();
	BankColors();
	for(int i = 0; Elements[i]; i++)
		{m_Elements.AddString(Elements[i]);}
	m_Elements.SetCurSel(0);
	m_Selected = Elements[0];
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ELEMENT);
	pEdit->SetWindowText(Elements[0]);
	DrawSwatch();
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Selection of displayed colors");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_CS_1);
		pWnd -> SetWindowText("Parameters:");
		pWnd = (CWnd *)GetDlgItem(IDC_COLOR_SEL);
		pWnd -> SetWindowText("Color selection");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_CS_2);
		pWnd -> SetWindowText("There should have been a preview.");
		pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#endif
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CColors::OnSelchangeElements()
{
	int nSel = m_Elements.GetCurSel();
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ELEMENT);
	pEdit -> SetWindowText(Elements[nSel]);
	DrawSwatch();
}

void CColors::OnColorSel()
{
	int nSel = m_Elements.GetCurSel();
	DWORD rgb = RGB(m_Colors[nSel][0], m_Colors[nSel][1], m_Colors[nSel][2]);
	CColorDialog Dlg(0, 0, this);
	Dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	Dlg.m_cc.rgbResult = rgb;
	if(Dlg.DoModal() == IDOK)
	{ // Microsoft does things backwards -- It's BGR, not RGB...
		m_Colors[nSel][0] = (unsigned char)(Dlg.m_cc.rgbResult & 0xFF);
		m_Colors[nSel][1] = (unsigned char)((Dlg.m_cc.rgbResult >> 8) & 0xFF);
		m_Colors[nSel][2] = (unsigned char)((Dlg.m_cc.rgbResult >> 16) & 0xFF);
		DrawSwatch();
	}
}

void CColors::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    DrawSwatch();
	// Do not call CDialog::OnPaint() for painting messages
}

void CColors::DrawSwatch()
{
	int nSel = m_Elements.GetCurSel();
	CWnd *pWnd = (CWnd *)GetDlgItem(IDC_SWATCH);
	CDC *pDC = pWnd -> GetDC();
	DWORD rgb = RGB(m_Colors[nSel][0], m_Colors[nSel][1], m_Colors[nSel][2]);
	CPen pPen(PS_NULL, 0, rgb);
	CBrush pBrush;
	pBrush.CreateSolidBrush(rgb);
	pDC -> SelectObject(&pBrush);
	pDC -> SelectObject(&pPen);
	CRect rect;
	pWnd -> GetClientRect(rect);
	rect.top += 2;
	rect.left += 2;
	rect.bottom -= 2;
	rect.right -= 2;
	pDC -> Rectangle(rect);
}

void CColors::BankColors()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	for(int i = 0; i < frame_colors; i++)
	{
		for(int c = 0; c < 3; c++)
			{m_Colors[i][c] = pFrame -> m_Colors[i][c];}
	}
}