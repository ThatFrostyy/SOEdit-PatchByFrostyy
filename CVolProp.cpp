// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "CVolProp.h"
#include "mainfrm.h"
#include "SOEditDoc.h"
#include "general.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int StrComparator(const void * a, const void * b)
	{return stricmp(*(char**)a, *(char**)b);}

/////////////////////////////////////////////////////////////////////////////
// CVolProp dialog

CVolProp::CVolProp(CWnd* pParent /*=NULL*/)
	: CDialog(CVolProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVolProp)
	m_Name = _T("");
	m_VolBone = _T("");
	//m_PolyhedronName = _T("");
	m_Matrix1_1 = 1.0f;
	m_Matrix1_2 = 0.0f;
	m_Matrix1_3 = 0.0f;
	m_Matrix2_1 = 0.0f;
	m_Matrix2_2 = 1.0f;
	m_Matrix2_3 = 0.0f;
	m_Matrix3_1 = 0.0f;
	m_Matrix3_2 = 0.0f;
	m_Matrix3_3 = 1.0f;
	m_Matrix4_1 = 0.0f;
	m_Matrix4_2 = 0.0f;
	m_Matrix4_3 = 0.0f;
	//}}AFX_DATA_INIT
    m_VolType = 0;
    m_BoxLength = 0.0f;
    m_BoxWidth = 0.0f;
    m_BoxHeight = 0.0f;
    m_CylLength = 0.0f;
    m_CylDiameter = 0.0f;
	nitems = 0;
	sitems = NULL; 
}

void CVolProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolProp)
	DDX_Control(pDX, IDC_VOL_TAB, m_TabCtrl);
	DDX_Text(pDX, IDC_VOLNAME, m_Name);
	DDX_Text(pDX, IDC_VOL_BONE, m_VolBone);
	DDX_Text(pDX, IDC_VOL_M1_1, m_Matrix1_1);
	DDX_Text(pDX, IDC_VOL_M1_2, m_Matrix1_2);
	DDX_Text(pDX, IDC_VOL_M1_3, m_Matrix1_3);
	DDX_Text(pDX, IDC_VOL_M2_1, m_Matrix2_1);
	DDX_Text(pDX, IDC_VOL_M2_2, m_Matrix2_2);
	DDX_Text(pDX, IDC_VOL_M2_3, m_Matrix2_3);
	DDX_Text(pDX, IDC_VOL_M3_1, m_Matrix3_1);
	DDX_Text(pDX, IDC_VOL_M3_2, m_Matrix3_2);
	DDX_Text(pDX, IDC_VOL_M3_3, m_Matrix3_3);
	DDX_Text(pDX, IDC_VOL_M4_1, m_Matrix4_1);
	DDX_Text(pDX, IDC_VOL_M4_2, m_Matrix4_2);
	DDX_Text(pDX, IDC_VOL_M4_3, m_Matrix4_3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVolProp, CDialog)
	//{{AFX_MSG_MAP(CVolProp)
	ON_LBN_SELCHANGE(IDC_VOL_BONES, OnSelchangeVolBones)
	ON_EN_CHANGE(IDC_VOL_M1_1, OnCheckerMatrix11)
	ON_EN_CHANGE(IDC_VOL_M1_2, OnCheckerMatrix12)
	ON_EN_CHANGE(IDC_VOL_M1_3, OnCheckerMatrix13)
	ON_EN_CHANGE(IDC_VOL_M2_1, OnCheckerMatrix21)
	ON_EN_CHANGE(IDC_VOL_M2_2, OnCheckerMatrix22)
	ON_EN_CHANGE(IDC_VOL_M2_3, OnCheckerMatrix23)
	ON_EN_CHANGE(IDC_VOL_M3_1, OnCheckerMatrix31)
	ON_EN_CHANGE(IDC_VOL_M3_2, OnCheckerMatrix32)
	ON_EN_CHANGE(IDC_VOL_M3_3, OnCheckerMatrix33)
	ON_EN_CHANGE(IDC_VOL_M4_1, OnCheckerMatrix41)
	ON_EN_CHANGE(IDC_VOL_M4_2, OnCheckerMatrix42)
	ON_EN_CHANGE(IDC_VOL_M4_3, OnCheckerMatrix43)
	ON_EN_CHANGE(IDC_VOLNAME, OnM_Name)
	ON_BN_CLICKED(IDC_ROUND_VOLUME_BUTTON, OnRoundVolumeClick)
	ON_EN_CHANGE(IDC_VOL_BONE, OnChangeBone)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolProp message handlers

void CVolProp::SkeletonScanning(CBone *basis, bool mod)
{
	CBone *child;
	if(!basis -> sub)
	{
		if(mod)
		{
			sitems[nitems] = new char[strlen(basis -> m_Name) + 1];
			strcpy(sitems[nitems], basis -> m_Name);
		}
		nitems++;
	}
	if((child = basis -> m_ChildFirst) != NULL)
		{SkeletonScanning(child, mod);}
	if(basis -> next)
		{SkeletonScanning(basis -> next, mod);}
}

BOOL CVolProp::OnInitDialog()
{
	CDialog::OnInitDialog();
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
	#ifdef ALTERNATIVE_LANG
		m_TabCtrl.InsertItem(0, "Polyhedron");
		m_TabCtrl.InsertItem(1, "Box");
		m_TabCtrl.InsertItem(2, "Cylinder");
	#else
		m_TabCtrl.InsertItem(0, "Ìíîãîãðàííèê");
		m_TabCtrl.InsertItem(1, "Áîêñ");
		m_TabCtrl.InsertItem(2, "Öèëèíäð");
	#endif
	switch(m_VolType)
	{
		case VT_PLY:      // Activate polyhedron tab
			{m_TabCtrl.SetCurSel(0);}
		break;
		case VT_BOX:      // Activate box tab
			{m_TabCtrl.SetCurSel(1);}
		break;
		case VT_CYLINDER: // Activate cylinder tab
			{m_TabCtrl.SetCurSel(2);}
		break;
	}
	m_TabCtrl.m_BoxLength = m_BoxLength;
	m_TabCtrl.m_BoxWidth = m_BoxWidth;
	m_TabCtrl.m_BoxHeight = m_BoxHeight;
	m_TabCtrl.m_CylLength = m_CylLength;
	m_TabCtrl.m_CylDiameter = m_CylDiameter;
	m_TabCtrl.m_PolyName = m_PolyName;
	m_TabCtrl.ActivateTabDialogs();
	if(pDoc -> m_Model -> m_skeleton)
	{
		if(pDoc -> m_Model -> m_skeleton -> m_bonelist)
		{
			SkeletonScanning(pDoc -> m_Model -> m_skeleton -> m_bonelist, false);
			if(nitems)
			{
				int SetIndx = -1;
				sitems = new char*[nitems];
				memset(sitems, 0, sizeof(char*) * nitems);
				nitems = 0;
				SkeletonScanning(pDoc -> m_Model -> m_skeleton -> m_bonelist, true);
				qsort(sitems, nitems, sizeof(char**), StrComparator);
				for(int i = 0; i < nitems; i++)
				{
					pList -> AddString(sitems[i]);
					if(strcmp(m_VolBone, sitems[i]) == 0)
						{SetIndx = i;}
					delete[] sitems[i];
					sitems[i] = NULL;
				}
				delete[] sitems;
				sitems = NULL;
				pList -> SetCurSel(SetIndx);
			}
		}
	}
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Volume Editor");
		CWnd *pWndl = (CWnd *)GetDlgItem(IDC_STATIC_VL1_1);
		pWndl -> SetWindowText("Volume name:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_VL1_2);
		pWndl -> SetWindowText("Tied to a bone");
		pWndl = (CWnd *)GetDlgItem(IDC_ROUND_VOLUME_BUTTON);
		pWndl -> SetWindowText("Round");
		pWndl = (CWnd *)GetDlgItem(IDCANCEL);
		pWndl -> SetWindowText("Cancel");
		pWndl = (CWnd *)GetDlgItem(IDOK);
		pWndl -> SetWindowText("OK");
	#endif
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CVolProp::OnSelchangeVolBones() 
{
	UpdateData(true);
	CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
	UpdateData(true);
	pList -> GetText(pList -> GetCurSel(), m_VolBone);
	UpdateData(false);
}

BOOL CVolProp::DestroyWindow() 
{
    m_TabCtrl.UpdateData(true);
	m_BoxLength = m_TabCtrl.m_BoxLength;
    m_BoxWidth = m_TabCtrl.m_BoxWidth;
    m_BoxHeight = m_TabCtrl.m_BoxHeight;
    m_CylLength = m_TabCtrl.m_CylLength;
    m_CylDiameter = m_TabCtrl.m_CylDiameter;
    m_PolyName = m_TabCtrl.m_PolyName;
	return CDialog::DestroyWindow();
}

void CVolProp::PostNcDestroy() 
{
    m_BoxLength = m_TabCtrl.m_BoxLength;
    m_BoxWidth = m_TabCtrl.m_BoxWidth;
    m_BoxHeight = m_TabCtrl.m_BoxHeight;
    m_CylLength = m_TabCtrl.m_CylLength;
    m_CylDiameter = m_TabCtrl.m_CylDiameter;
    m_PolyName = m_TabCtrl.m_PolyName;
    m_VolType = m_TabCtrl.m_VolType;
	CDialog::PostNcDestroy();
}

void CVolProp::OnOK()
{
	UpdateData(false);
	CDialog::OnOK();
}

void CVolProp::OnM_Name()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Name = "";
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOLNAME);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	m_Name = text_buff;
}

void CVolProp::OnRoundVolumeClick()
{
	m_Matrix1_1 = FixVolMX(IDC_VOL_M1_1);
	m_Matrix1_2 = FixVolMX(IDC_VOL_M1_2);
	m_Matrix1_3 = FixVolMX(IDC_VOL_M1_3);
	m_Matrix2_1 = FixVolMX(IDC_VOL_M2_1);
	m_Matrix2_2 = FixVolMX(IDC_VOL_M2_2);
	m_Matrix2_3 = FixVolMX(IDC_VOL_M2_3);
	m_Matrix3_1 = FixVolMX(IDC_VOL_M3_1);
	m_Matrix3_2 = FixVolMX(IDC_VOL_M3_2);
	m_Matrix3_3 = FixVolMX(IDC_VOL_M3_3);
	UpdateData(false);
}

void CVolProp::OnCheckerMatrix11()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix1_1 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M1_1);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix1_1 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix12()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix1_2 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M1_2);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix1_2 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix13()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix1_3 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M1_3);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix1_3 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix21()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix2_1 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M2_1);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix2_1 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix22()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix2_2 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M2_2);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix2_2 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix23()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix2_3 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M2_3);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix2_3 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix31()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix3_1 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M3_1);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix3_1 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix32()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix3_2 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M3_2);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix3_2 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix33()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix3_3 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M3_3);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix3_3 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix41()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix4_1 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M4_1);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix4_1 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix42()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix4_2 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M4_2);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix4_2 = (float)atof(text_buff);}
}

void CVolProp::OnCheckerMatrix43()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix4_3 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_M4_3);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix4_3 = (float)atof(text_buff);}
}

float CVolProp::FixVolMX(int id)
{
	CEdit *pEdit;
	memset(text_buff, 0, _MAX_PATH);
	pEdit = (CEdit *)GetDlgItem(id);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	return (float)round(atof(text_buff));
}

void CVolProp::OnChangeBone()
{
	char bonename[_MAX_PATH];
	memset(bonename, '\0', _MAX_PATH);
	CString	Check_Bone;
	int nStartChar = 0, nEndChar = 0;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOL_BONE);
	CListBox *pList = (CListBox *)GetDlgItem(IDC_VOL_BONES);
	ASSERT(pEdit);
	ASSERT(pList);
	pEdit -> GetWindowText(bonename, _MAX_PATH - 1);
	pEdit -> SetFocus();
	pEdit -> GetSel(nStartChar, nEndChar);
	if(!strlen(bonename))
		{return;}
	int n = pList -> GetCount();
	bool b = false;
	for(int i = 0; i < n; i++)
	{
		pList -> GetText(i, Check_Bone);
		if(!strnicmp(Check_Bone.GetBuffer(1), bonename, nStartChar))
		{
			pList -> SetCurSel(i);
			m_VolBone = Check_Bone;
			UpdateData(false);
			pEdit -> SetSel(nStartChar, nEndChar);
			b = true;
			break;
		}
	}
	if(!b)
		{pList -> SetCurSel(-1);}
}
