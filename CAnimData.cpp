// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CAnimData.h"
#include "mainfrm.h"
#include "SOEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimData dialog

CAnimData::CAnimData(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimData::IDD, pParent)
{
	m_Name = _T("");
	m_FileName = _T("");
	m_FrameCnt = 0;
	m_Frame = 0;
	m_FrameNo = 0;
	m_M1_1 = 0.0f;
	m_M1_2 = 0.0f;
	m_M2_1 = 0.0f;
	m_M1_3 = 0.0f;
	m_M2_2 = 0.0f;
	m_M2_3 = 0.0f;
	m_M3_1 = 0.0f;
	m_M3_2 = 0.0f;
	m_M3_3 = 0.0f;
	m_M4_1 = 0.0f;
	m_M4_2 = 0.0f;
	m_M4_3 = 0.0f;
	m_Visi = FALSE;
	m_RewritePermissionBAnm = m_RewritePermission = TRUE;
	m_pSub = NULL;
	m_ver = 0;
	//m_X = m_Y = m_Z = false;
	//anm_program = 0;
	//start_frm = stop_frm = -1;
	//m_user_axis = 0;
	//anm_law = m_user_law = 0;
	//memset(&m_min_var, 0, sizeof(dv3_t));
	//memset(&m_max_var, 0, sizeof(dv3_t));
	AnmTaskNum = -1;
}

void CAnimData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	BYTE h1 = 0, h2 = 0, h3 = 0, h4 = 0;
	h1 = m_ver;
	h2 = m_ver >> 8;
	h3 = m_ver >> 16;
	h4 = m_ver >> 24;
	CString nil = "0";
	DDX_Control(pDX, IDC_FRAMER, m_Framer);
	DDX_Control(pDX, IDC_MBONES, m_BoneList);
	DDX_Text(pDX, IDC_ANIM_PATH, m_FileName);
	DDX_Text(pDX, IDC_FRAMECNT, m_FrameCnt);
	DDX_Text(pDX, IDC_FRAMENO, m_Frame);
	DDX_Text(pDX, IDC_M1_1, m_M1_1);
	DDX_Text(pDX, IDC_M1_2, m_M1_2);
	DDX_Text(pDX, IDC_M1_3, m_M1_3);
	DDX_Text(pDX, IDC_M2_1, m_M2_1);
	DDX_Text(pDX, IDC_M2_2, m_M2_2);
	DDX_Text(pDX, IDC_M2_3, m_M2_3);
	DDX_Text(pDX, IDC_M3_1, m_M3_1);
	DDX_Text(pDX, IDC_M3_2, m_M3_2);
	DDX_Text(pDX, IDC_M3_3, m_M3_3);
	DDX_Text(pDX, IDC_M4_1, m_M4_1);
	DDX_Text(pDX, IDC_M4_2, m_M4_2);
	DDX_Text(pDX, IDC_M4_3, m_M4_3);
	DDX_Check(pDX, IDC_VISI, m_Visi);
	DDX_Text(pDX, IDC_ANM_VER_1, h1);
	DDX_Text(pDX, IDC_ANM_VER_2, h2);
	DDX_Text(pDX, IDC_ANM_VER_3, h3);
	DDX_Text(pDX, IDC_ANM_VER_4, h4);
	DDX_Text(pDX, IDC_ANM_X_MIN, nil);
	DDX_Text(pDX, IDC_ANM_Y_MIN, nil);
	DDX_Text(pDX, IDC_ANM_Z_MIN, nil);
	DDX_Text(pDX, IDC_ANM_X_MAX, nil);
	DDX_Text(pDX, IDC_ANM_Y_MAX, nil);
	DDX_Text(pDX, IDC_ANM_Z_MAX, nil);
	DDX_Text(pDX, IDC_ANM_TASK, nil);
}

BEGIN_MESSAGE_MAP(CAnimData, CDialog)
	ON_BN_CLICKED(IDC_RESET_BONE, &CAnimData::OnBnClickedResetBone)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY_MATRIX, &CAnimData::OnCopyMatrix)
	ON_COMMAND(ID_PASTE_MATRIX, &CAnimData::OnPasteMatrix)

	//{{AFX_MSG_MAP(CAnimData)
	ON_LBN_SELCHANGE(IDC_MBONES, OnSelchangeMbones)
	ON_NOTIFY(UDN_DELTAPOS, IDC_FRAMER, OnDeltaposFramer)
	ON_EN_CHANGE(IDC_M1_1, OnCheckerMatrix11)
	ON_EN_CHANGE(IDC_M1_2, OnCheckerMatrix12)
	ON_EN_CHANGE(IDC_M1_3, OnCheckerMatrix13)
	ON_EN_CHANGE(IDC_M2_1, OnCheckerMatrix21)
	ON_EN_CHANGE(IDC_M2_2, OnCheckerMatrix22)
	ON_EN_CHANGE(IDC_M2_3, OnCheckerMatrix23)
	ON_EN_CHANGE(IDC_M3_1, OnCheckerMatrix31)
	ON_EN_CHANGE(IDC_M3_2, OnCheckerMatrix32)
	ON_EN_CHANGE(IDC_M3_3, OnCheckerMatrix33)
	ON_EN_CHANGE(IDC_M4_1, OnCheckerMatrix41)
	ON_EN_CHANGE(IDC_M4_2, OnCheckerMatrix42)
	ON_EN_CHANGE(IDC_M4_3, OnCheckerMatrix43)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INSERT_FRAME, &CAnimData::OnInsertFrame)
	ON_BN_CLICKED(IDC_DELETE_FRAME, &CAnimData::OnBnClickedDeleteFrame)
	ON_BN_CLICKED(IDC_VISI, &CAnimData::OnBnClickedVisi)
	ON_BN_CLICKED(IDC_DELETE_ANM_BONE_NAME, &CAnimData::OnBnClickedDeleteAnmBoneName)
	ON_BN_CLICKED(IDC_ANM_AUTO, &CAnimData::OnBnClickedAnmAuto)
	ON_BN_CLICKED(IDC_ANM_AXIS_X, &CAnimData::OnBnClickedAnmAxisX)
	ON_BN_CLICKED(IDC_ANM_AXIS_Y, &CAnimData::OnBnClickedAnmAxisY)
	ON_BN_CLICKED(IDC_ANM_AXIS_Z, &CAnimData::OnBnClickedAnmAxisZ)
	ON_EN_CHANGE(IDC_ANM_X_MIN, &CAnimData::OnEnChangeAnmXMin)
	ON_EN_CHANGE(IDC_ANM_Y_MIN, &CAnimData::OnEnChangeAnmYMin)
	ON_EN_CHANGE(IDC_ANM_Z_MIN, &CAnimData::OnEnChangeAnmZMin)
	ON_EN_CHANGE(IDC_ANM_X_MAX, &CAnimData::OnEnChangeAnmXMax)
	ON_EN_CHANGE(IDC_ANM_Y_MAX, &CAnimData::OnEnChangeAnmYMax)
	ON_EN_CHANGE(IDC_ANM_Z_MAX, &CAnimData::OnEnChangeAnmZMax)
	ON_EN_CHANGE(IDC_ANM_LAW3_RATIO, &CAnimData::OnEnChangeAnmLaw3Ratio)
	ON_BN_CLICKED(IDC_MATRIX_REWRITE, &CAnimData::OnBnClickedMatrixRewrite)
	ON_EN_CHANGE(IDC_AD_ANM_BONE_NAME, &CAnimData::OnEnChangeAdAnmBoneName)
	ON_BN_CLICKED(IDC_ANM_BONE_CLONE, &CAnimData::OnBnClickedAnmBoneClone)
	ON_BN_CLICKED(IDC_ANM_BONE_EXPORT, &CAnimData::OnBnClickedAnmBoneExport)
	ON_BN_CLICKED(IDC_ANM_BONE_IMPORT, &CAnimData::OnBnClickedAnmBoneImport)
	ON_BN_CLICKED(IDC_REVERSE_ANIM, &CAnimData::OnReverseAnimation)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimData message handlers

BOOL CAnimData::OnInitDialog() 
{
	char bonename[_MAX_PATH];
	CDialog::OnInitDialog();
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	m_FrameNo = pDoc -> m_Frame;
    CAnimSub *pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FRAMENO);
	CString st;
	st.Format("%d", m_FrameNo + 1);
	pEdit -> SetWindowText(st);
	m_pSub = pSub;
    while(pSub)
	{
        m_BoneList.AddString(pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]);
        pSub = pSub -> m_next;
	}
    m_BoneList.SetCurSel(0);
	m_BoneList.GetText(0, bonename);
	pEdit = (CEdit *)GetDlgItem(IDC_AD_ANM_BONE_NAME);
	m_RewritePermissionBAnm = FALSE;
	pEdit -> SetWindowText(bonename);
	m_RewritePermissionBAnm = TRUE;
	SelBone = bonename;
    m_Framer.SetRange(0, pDoc -> m_AnimBone -> m_FrameCnt - 1);
    m_Framer.SetPos(pDoc -> m_Frame);
    m_Framer.SetBuddy(GetDlgItem(IDC_FRAMENO));
	OnSelchangeMbones();
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_ACTION);
	pListBox -> AddString("-----------------------------------");
	#ifdef ALTERNATIVE_LANG
		pListBox -> AddString("Moving");
		pListBox -> AddString("Rotation");
		pListBox -> AddString("Show");
		pListBox -> AddString("Hiding");
	#else
		pListBox -> AddString("Перемещение");
		pListBox -> AddString("Вращение");
		pListBox -> AddString("Отображение");
		pListBox -> AddString("Скрытие");
	#endif
	pListBox -> SetCurSel(-1);
	pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
	pListBox -> AddString("-----------------------------------");
	#ifdef ALTERNATIVE_LANG
		pListBox -> AddString("Linear");
		pListBox -> AddString("Boost");
		pListBox -> AddString("Braking");
		pListBox -> AddString("Oma");
		pListBox -> AddString("Test");
	#else
		pListBox -> AddString("Линейный");
		pListBox -> AddString("Ускорение");
		pListBox -> AddString("Торможение");
		pListBox -> AddString("Ома");
		pListBox -> AddString("Тест");
	#endif
	pListBox -> SetCurSel(-1);
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Animation Editor");
		CWnd *pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_1);
		pWndl -> SetWindowText("Frame:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_2);
		pWndl -> SetWindowText("of");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_3);
		pWndl -> SetWindowText("Bone data");
		pWndl = (CWnd *)GetDlgItem(IDC_VISI);
		pWndl -> SetWindowText("Bone visible");
		pWndl = (CWnd *)GetDlgItem(IDC_INSERT_FRAME);
		pWndl -> SetWindowText("Insert a frame");
		pWndl = (CWnd *)GetDlgItem(IDC_DELETE_FRAME);
		pWndl -> SetWindowText("Delete a frame");
		pWndl = (CWnd *)GetDlgItem(IDC_DELETE_ANM_BONE_NAME);
		pWndl -> SetWindowText("Remove the bone");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_4);
		pWndl -> SetWindowText("Version:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_5);
		pWndl -> SetWindowText("Automation");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_6);
		pWndl -> SetWindowText("Task:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_7);
		pWndl -> SetWindowText("In frm with:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_8);
		pWndl -> SetWindowText("by:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_9);
		pWndl -> SetWindowText("Execute:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_10);
		pWndl -> SetWindowText("By axis:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_11);
		pWndl -> SetWindowText("In the range:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_12);
		pWndl -> SetWindowText("Frm:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_13);
		pWndl -> SetWindowText("Bef:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_14);
		pWndl -> SetWindowText("By law:");
		pWndl = (CWnd *)GetDlgItem(IDC_STATIC_AD_15);
		pWndl -> SetWindowText("BC:");
		pWndl = (CWnd *)GetDlgItem(IDC_MATRIX_REWRITE);
		pWndl -> SetWindowText("Overwrite the matrix");
		pWndl = (CWnd *)GetDlgItem(IDC_ANM_BONE_CLONE);
		pWndl -> SetWindowText("Clone");
		pWndl = (CWnd *)GetDlgItem(IDC_ANM_BONE_EXPORT);
		pWndl -> SetWindowText("Export");
		pWndl = (CWnd *)GetDlgItem(IDC_ANM_BONE_IMPORT);
		pWndl -> SetWindowText("Import");
		pWndl = (CWnd *)GetDlgItem(IDOK);
		pWndl -> SetWindowText("OK");
	#endif
	// return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
	return TRUE;
}

void CAnimData::SetMatrixData(int control, float value)
{
    char szValue[_MAX_PATH];
    CEdit *pEdit = (CEdit *)GetDlgItem(control);
    sprintf(szValue, "%g", value);
    pEdit -> SetWindowText(szValue);
}

void CAnimData::OnSelchangeMbones()
{
    char bonename[_MAX_PATH];
    int  bonesel = 0;
    bonesel = m_BoneList.GetCurSel();
    m_BoneList.GetText(bonesel, bonename);
	SelBone = bonename;
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
    CAnimSub *pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_AD_ANM_BONE_NAME);
    while(pSub)
	{
		if(!stricmp(bonename, pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]))
		{
			m_RewritePermission = FALSE;
            SetMatrixData(IDC_M1_1, pSub -> m_Matrix34.v[0][0]);
            SetMatrixData(IDC_M1_2, pSub -> m_Matrix34.v[0][1]);
            SetMatrixData(IDC_M1_3, pSub -> m_Matrix34.v[0][2]);
            SetMatrixData(IDC_M2_1, pSub -> m_Matrix34.v[1][0]);
            SetMatrixData(IDC_M2_2, pSub -> m_Matrix34.v[1][1]);
            SetMatrixData(IDC_M2_3, pSub -> m_Matrix34.v[1][2]);
            SetMatrixData(IDC_M3_1, pSub -> m_Matrix34.v[2][0]);
            SetMatrixData(IDC_M3_2, pSub -> m_Matrix34.v[2][1]);
            SetMatrixData(IDC_M3_3, pSub -> m_Matrix34.v[2][2]);
            SetMatrixData(IDC_M4_1, pSub -> m_Matrix34.v[3][0]);
            SetMatrixData(IDC_M4_2, pSub -> m_Matrix34.v[3][1]);
            SetMatrixData(IDC_M4_3, pSub -> m_Matrix34.v[3][2]);
			if(pSub -> m_Visible)
				{CheckDlgButton(IDC_VISI, 1);}
			else
				{CheckDlgButton(IDC_VISI, 0);}
			m_RewritePermissionBAnm = FALSE;
			pEdit -> SetWindowText(bonename);
			m_RewritePermissionBAnm = TRUE;
			m_RewritePermission = TRUE;
            m_Visi = pSub -> m_Visible;
			m_pSub = pSub;
            break;
		}
		pSub = pSub -> m_next;
	}
}

void CAnimData::OnDeltaposFramer(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	m_FrameNo += pNMUpDown -> iDelta;
	m_FrameNo = max(0, min(m_FrameNo, pDoc -> m_AnimBone -> m_FrameCnt - 1));
    CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FRAMENO);
    char szValue[_MAX_PATH];
    sprintf(szValue, "%d", m_FrameNo + 1);
    pEdit -> SetWindowText(szValue);
	m_RewritePermission = FALSE;
    SetMatrixData(IDC_M1_1, 0.0f);
    SetMatrixData(IDC_M1_2, 0.0f);
    SetMatrixData(IDC_M1_3, 0.0f);
    SetMatrixData(IDC_M2_1, 0.0f);
    SetMatrixData(IDC_M2_2, 0.0f);
    SetMatrixData(IDC_M2_3, 0.0f);
    SetMatrixData(IDC_M3_1, 0.0f);
    SetMatrixData(IDC_M3_2, 0.0f);
    SetMatrixData(IDC_M3_3, 0.0f);
    SetMatrixData(IDC_M4_1, 0.0f);
    SetMatrixData(IDC_M4_2, 0.0f);
    SetMatrixData(IDC_M4_3, 0.0f);
	CheckDlgButton(IDC_VISI, 0);
	m_RewritePermission = TRUE;
	CAnimSub *pSub = NULL;
	pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	m_BoneList.ResetContent();
    while(pSub)
	{
        m_BoneList.AddString(pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]);
        pSub = pSub -> m_next;
	}
	pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
    m_BoneList.SetCurSel(0);
	if(SelBone.GetLength())
	{
		char bonename[_MAX_PATH];
		int  items = m_BoneList.GetCount();
		for(int i = 0; i < items; i++)
		{
			m_BoneList.GetText(i, bonename);
			if(!strcmp(bonename, SelBone))
			{
				m_BoneList.SetCurSel(i);
				break;
			}
		}
		for(int i = 0; pSub; i++)
		{
			if(!strcmp(SelBone, pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]))
				{break;}
			pSub = pSub -> m_next;
		}
	}
    if(pSub)
	{
		m_RewritePermission = FALSE;
        SetMatrixData(IDC_M1_1, pSub -> m_Matrix34.v[0][0]);
        SetMatrixData(IDC_M1_2, pSub -> m_Matrix34.v[0][1]);
        SetMatrixData(IDC_M1_3, pSub -> m_Matrix34.v[0][2]);
        SetMatrixData(IDC_M2_1, pSub -> m_Matrix34.v[1][0]);
        SetMatrixData(IDC_M2_2, pSub -> m_Matrix34.v[1][1]);
        SetMatrixData(IDC_M2_3, pSub -> m_Matrix34.v[1][2]);
        SetMatrixData(IDC_M3_1, pSub -> m_Matrix34.v[2][0]);
        SetMatrixData(IDC_M3_2, pSub -> m_Matrix34.v[2][1]);
        SetMatrixData(IDC_M3_3, pSub -> m_Matrix34.v[2][2]);
        SetMatrixData(IDC_M4_1, pSub -> m_Matrix34.v[3][0]);
        SetMatrixData(IDC_M4_2, pSub -> m_Matrix34.v[3][1]);
        SetMatrixData(IDC_M4_3, pSub -> m_Matrix34.v[3][2]);
		if(pSub -> m_Visible)
			{CheckDlgButton(IDC_VISI, 1);}
		else
			{CheckDlgButton(IDC_VISI, 0);}
		m_RewritePermission = TRUE;
        m_Visi = pSub -> m_Visible;
		m_pSub = pSub;
	}
	*pResult = 0;
}

void CAnimData::OnCheckerMatrix11()
	{OnCheckerMatrix(IDC_M1_1);}

void CAnimData::OnCheckerMatrix12()
	{OnCheckerMatrix(IDC_M1_2);}

void CAnimData::OnCheckerMatrix13()
	{OnCheckerMatrix(IDC_M1_3);}

void CAnimData::OnCheckerMatrix21()
	{OnCheckerMatrix(IDC_M2_1);}

void CAnimData::OnCheckerMatrix22()
	{OnCheckerMatrix(IDC_M2_2);}

void CAnimData::OnCheckerMatrix23()
	{OnCheckerMatrix(IDC_M2_3);}

void CAnimData::OnCheckerMatrix31()
	{OnCheckerMatrix(IDC_M3_1);}

void CAnimData::OnCheckerMatrix32()
	{OnCheckerMatrix(IDC_M3_2);}

void CAnimData::OnCheckerMatrix33()
	{OnCheckerMatrix(IDC_M3_3);}

void CAnimData::OnCheckerMatrix41()
	{OnCheckerMatrix(IDC_M4_1);}

void CAnimData::OnCheckerMatrix42()
	{OnCheckerMatrix(IDC_M4_2);}

void CAnimData::OnCheckerMatrix43()
	{OnCheckerMatrix(IDC_M4_3);}

void CAnimData::OnCheckerMatrix(int control)
{
	if(!m_RewritePermission)
		{return;}
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
    CAnimSub *pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	memset(text_buff, 0, _MAX_PATH);
	CEdit *pEdit = (CEdit *)GetDlgItem(control);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	СommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	while(pSub)
	{
		if(!stricmp(SelBone, pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]))
			{break;}
		pSub = pSub -> m_next;
	}
	if(!pSub)
		{return;}
	switch(control)
	{
		case IDC_M1_1:
		{
			m_M1_1 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M1_1 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[0][0] = m_M1_1;
			}
		}
		break;
		case IDC_M1_2:
		{
			m_M1_2 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M1_2 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[0][1] = m_M1_2;
			}
		}
		break;
		case IDC_M1_3:
		{
			m_M1_3 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M1_3 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[0][2] = m_M1_3;
			}
		}
		break;
		case IDC_M2_1:
		{
			m_M2_1 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M2_1 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[1][0] = m_M2_1;
			}
		}
		break;
		case IDC_M2_2:
		{
			m_M2_2 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M2_2 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[1][1] = m_M2_2;
			}
		}
		break;
		case IDC_M2_3:
		{
			m_M2_3 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M2_3 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[1][2] = m_M2_3;
			}
		}
		break;
		case IDC_M3_1:
		{
			m_M3_1 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M3_1 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[2][0] = m_M3_1;
			}
		}
		break;
		case IDC_M3_2:
		{
			m_M3_2 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M3_2 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[2][1] = m_M3_2;
			}
		}
		break;
		case IDC_M3_3:
		{
			m_M3_3 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M3_3 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[2][2] = m_M3_3;
			}
		}
		break;
		case IDC_M4_1:
		{
			m_M4_1 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M4_1 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[3][0] = m_M4_1;
			}
		}
		break;
		case IDC_M4_2:
		{
			m_M4_2 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M4_2 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[3][1] = m_M4_2;
			}
		}
		break;
		case IDC_M4_3:
		{
			m_M4_3 = 0.0f;
			if(isFloat(text_buff))
			{
				m_M4_3 = (float)atof(text_buff);
				pSub -> m_Matrix34.v[3][2] = m_M4_3;
			}
		}
		break;
	};
}

void CAnimData::OnInsertFrame()
{
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	m_FrameNo = pDoc -> m_AnimBone -> OnInsertFrame(m_FrameNo);
	m_FrameCnt = pDoc -> m_AnimBone -> m_FrameCnt;
	m_Frame = m_FrameNo;
	pDoc -> m_Frame = m_FrameNo;
	OnSelchangeMbones();
	char szValue[_MAX_PATH];
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FRAMENO);
	sprintf(szValue, "%d", m_Frame + 1);
	pEdit -> SetWindowText(szValue);
	pEdit = (CEdit *)GetDlgItem(IDC_FRAMECNT);
	sprintf(szValue, "%d", m_FrameCnt);
	pEdit -> SetWindowText(szValue);
}

void CAnimData::OnBnClickedDeleteFrame()//удалив кадр, нужно править задания
{
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	if(pDoc -> m_AnimBone -> m_FrameCnt == 1)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("Too few frames!", "ERROR: CAnimData::OnBnClickedDeleteFrame", MB_ICONHAND);
		#else
			MessageBox("Мало кадров!", "ERROR: CAnimData::OnBnClickedDeleteFrame", MB_ICONHAND);
		#endif
		return;
	}
	int indx = pDoc -> m_AnimBone -> FrameDelete(m_FrameNo);
	m_Frame = indx;
	m_FrameNo = indx;
	pDoc -> m_Frame = m_Frame;
	m_FrameCnt = pDoc -> m_AnimBone -> m_FrameCnt;
	OnSelchangeMbones();
	char szValue[_MAX_PATH];
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FRAMENO);
	sprintf(szValue, "%d", m_Frame + 1);
	pEdit -> SetWindowText(szValue);
	pEdit = (CEdit *)GetDlgItem(IDC_FRAMECNT);
	sprintf(szValue, "%d", m_FrameCnt);
	pEdit -> SetWindowText(szValue);
}

void CAnimData::OnBnClickedVisi()
{
	if(!m_pSub)
		{return;}
	m_pSub -> m_Visible = IsDlgButtonChecked(IDC_VISI);
}

void CAnimData::OnBnClickedDeleteAnmBoneName()//удалив кость нужно править задания
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	int BDel = m_pSub -> m_Bone;
	if(pDoc -> m_AnimBone -> m_BoneCnt == 1)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("Too few bones!", "ERROR: CAnimData::OnBnClickedDeleteAnmBoneName", MB_ICONHAND);
		#else
			MessageBox("Мало костей!", "ERROR: CAnimData::OnBnClickedDeleteAnmBoneName", MB_ICONHAND);
		#endif
		return;
	}
	#ifdef ALTERNATIVE_LANG
		if(MessageBox("Really remove this bone from the animation?", "QUESTION: CAnimData::OnBnClickedDeleteAnmBoneName", MB_YESNO | MB_ICONQUESTION) != IDYES)
			{return;}
	#else
		if(MessageBox("Точно удалить эту кость из анимации?", "QUESTION: CAnimData::OnBnClickedDeleteAnmBoneName", MB_YESNO | MB_ICONQUESTION) != IDYES)
			{return;}
	#endif
	pDoc -> m_AnimBone -> BoneDelete(m_pSub -> m_Bone);
	int bonesel = 0;
	int count = m_BoneList.GetCount();
    bonesel = m_BoneList.GetCurSel();
	m_BoneList.DeleteString(bonesel);
	if(count - 1 > bonesel)
		{m_BoneList.SetCurSel(bonesel);}
	else
	if(count - 1 == bonesel)
		{m_BoneList.SetCurSel(bonesel - 1);}
	OnSelchangeMbones();
}

void CAnimData::OnBnClickedAnmAuto()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	pDoc -> auto_animation = IsDlgButtonChecked(IDC_ANM_AUTO);
	if(pDoc -> m_AnimBone -> m_FrameCnt <= 1)
	{
		pDoc -> auto_animation = false;
		CheckDlgButton(IDC_ANM_AUTO, 0);
	}
	if(pDoc -> auto_animation)
	{
		AnmTaskNum = 0;
		TaskNumUPD();
		FramerUPD();
		CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_START_ANM_FRAMER);
		pSpinner -> EnableWindow(true);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_IDC_STOP_ANM_FRAMER);
		pSpinner -> EnableWindow(true);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_ACTIONS);
		pSpinner -> EnableWindow(true);
		CButton *pButton = (CButton *)GetDlgItem(IDC_MATRIX_REWRITE);
		pButton -> EnableWindow(true);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_TASKS);
		pSpinner -> EnableWindow(true);
		pDoc -> Animation_Automation.task_on = TRUE;
		ListActionUPD();
		MinMaxUPD();
		ActionUPD();
		LawUPD();
		MatrixRewriteUPD();
	}
	else
	{
		m_RewritePermission = FALSE;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_START_ANM_FRAME);
		pEdit -> SetWindowText("");
		pEdit = (CEdit *)GetDlgItem(IDC_STOP_ANM_FRAME);
		pEdit -> SetWindowText("");
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_X_MIN);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_Y_MIN);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_Z_MIN);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_X_MAX);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_Y_MAX);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_Z_MAX);
		pEdit -> SetWindowText("0");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_LAW3_RATIO);
		pEdit -> SetWindowText("");
		pEdit -> EnableWindow(false);
		pEdit = (CEdit *)GetDlgItem(IDC_ANM_TASK);
		pEdit -> SetWindowText("");
		m_RewritePermission = TRUE;
		CheckDlgButton(IDC_ANM_AXIS_X, 0);
		CheckDlgButton(IDC_ANM_AXIS_Y, 0);
		CheckDlgButton(IDC_ANM_AXIS_Z, 0);
		CheckDlgButton(IDC_MATRIX_REWRITE, 0);
		CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_START_ANM_FRAMER);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_IDC_STOP_ANM_FRAMER);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_X_MINS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Y_MINS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Z_MINS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_X_MAXS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Y_MAXS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Z_MAXS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_TASKS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_ACTIONS);
		pSpinner -> EnableWindow(false);
		pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_LAWS);
		pSpinner -> EnableWindow(false);
		CButton *pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_X);
		pButton -> EnableWindow(false);
		pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_Y);
		pButton -> EnableWindow(false);
		pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_Z);
		pButton -> EnableWindow(false);
		pButton = (CButton *)GetDlgItem(IDC_MATRIX_REWRITE);
		pButton -> EnableWindow(false);
		CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_ACTION);
		pListBox -> SetCurSel(0);
		pListBox -> SetCurSel(-1);
		pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
		pListBox -> SetCurSel(0);
		pListBox -> SetCurSel(-1);
		pDoc -> Animation_Automation.task_on = FALSE;
		//LawUPD();
		CStatic *pStatic = (CStatic *)GetDlgItem(IDC_TEXT_LAW);
		pStatic -> SetWindowText("");
		AnmTaskNum = -1;
	}
}

void CAnimData::OnBnClickedAnmAxisX()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	pDoc -> Animation_Automation.task[AnmTaskNum].x = IsDlgButtonChecked(IDC_ANM_AXIS_X);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ANM_X_MIN);
	CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_ANM_X_MAX);
	CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_X_MINS);
	CSpinButtonCtrl *pSpinner2 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_X_MAXS);
	CSpinButtonCtrl *pSpinner3 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_LAWS);
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
	if(pDoc -> Animation_Automation.task[AnmTaskNum].x)
	{
		pEdit -> EnableWindow(true);
		pEdit2 -> EnableWindow(true);
		pSpinner -> EnableWindow(true);
		pSpinner2 -> EnableWindow(true);
		pSpinner3 -> EnableWindow(true);
		//sbi(m_user_axis, 0);
		//anm_law = m_user_law;
		LawUPD();
	}
	else
	{
		pEdit -> EnableWindow(false);
		pEdit2 -> EnableWindow(false);
		pSpinner -> EnableWindow(false);
		pSpinner2 -> EnableWindow(false);
		//cbi(m_user_axis, 0);
		if(!pDoc -> Animation_Automation.task[AnmTaskNum].y && !pDoc -> Animation_Automation.task[AnmTaskNum].z)
		{
			pListBox -> SetCurSel(0);
			pListBox -> SetCurSel(-1);
			pSpinner3 -> EnableWindow(false);
			//anm_law = 0;
			LawUPD();
		}
	}
}

void CAnimData::OnBnClickedAnmAxisY()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	pDoc -> Animation_Automation.task[AnmTaskNum].y = IsDlgButtonChecked(IDC_ANM_AXIS_Y);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ANM_Y_MIN);
	CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_ANM_Y_MAX);
	CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Y_MINS);
	CSpinButtonCtrl *pSpinner2 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Y_MAXS);
	CSpinButtonCtrl *pSpinner3 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_LAWS);
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
	if(pDoc -> Animation_Automation.task[AnmTaskNum].y)
	{
		pEdit -> EnableWindow(true);
		pEdit2 -> EnableWindow(true);
		pSpinner -> EnableWindow(true);
		pSpinner2 -> EnableWindow(true);
		pSpinner3 -> EnableWindow(true);
		//sbi(m_user_axis, 1);
		//anm_law = m_user_law;
		LawUPD();
	}
	else
	{
		pEdit -> EnableWindow(false);
		pEdit2 -> EnableWindow(false);
		pSpinner -> EnableWindow(false);
		pSpinner2 -> EnableWindow(false);
		//cbi(m_user_axis, 1);
		if(!pDoc -> Animation_Automation.task[AnmTaskNum].x && !pDoc -> Animation_Automation.task[AnmTaskNum].z)
		{
			pListBox -> SetCurSel(-1);
			pSpinner3 -> EnableWindow(false);
			//anm_law = 0;
			LawUPD();
		}
	}
}

void CAnimData::OnBnClickedAnmAxisZ()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	pDoc -> Animation_Automation.task[AnmTaskNum].z = IsDlgButtonChecked(IDC_ANM_AXIS_Z);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ANM_Z_MIN);
	CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_ANM_Z_MAX);
	CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Z_MINS);
	CSpinButtonCtrl *pSpinner2 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_Z_MAXS);
	CSpinButtonCtrl *pSpinner3 = (CSpinButtonCtrl *)GetDlgItem(IDC_ANM_LAWS);
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
	if(pDoc -> Animation_Automation.task[AnmTaskNum].z)
	{
		pEdit -> EnableWindow(true);
		pEdit2 -> EnableWindow(true);
		pSpinner -> EnableWindow(true);
		pSpinner2 -> EnableWindow(true);
		pSpinner3 -> EnableWindow(true);
		//sbi(m_user_axis, 2);
		//anm_law = m_user_law;
		LawUPD();
	}
	else
	{
		pEdit -> EnableWindow(false);
		pEdit2 -> EnableWindow(false);
		pSpinner -> EnableWindow(false);
		pSpinner2 -> EnableWindow(false);
		//cbi(m_user_axis, 2);
		if(!pDoc -> Animation_Automation.task[AnmTaskNum].y && !pDoc -> Animation_Automation.task[AnmTaskNum].y)
		{
			pListBox -> SetCurSel(-1);
			pSpinner3 -> EnableWindow(false);
			//anm_law = 0;
			LawUPD();
		}
	}
}

BOOL CAnimData::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *nmHdr = (NMHDR *)lParam;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)nmHdr;
	if(nmHdr -> code == UDN_DELTAPOS)
	{
		CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
		CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
		switch(wParam)
		{
			case IDC_START_ANM_FRAMER:
			{
				pDoc -> Animation_Automation.task[AnmTaskNum].start_frame -= pNMUpDown -> iDelta;
				if(pDoc -> Animation_Automation.task[AnmTaskNum].start_frame < 1)
					{pDoc -> Animation_Automation.task[AnmTaskNum].start_frame = 1;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].start_frame > pDoc -> m_AnimBone -> m_FrameCnt - 1)
					{pDoc -> Animation_Automation.task[AnmTaskNum].start_frame = pDoc -> m_AnimBone -> m_FrameCnt - 1;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame <= pDoc -> Animation_Automation.task[AnmTaskNum].start_frame)
					{pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame = pDoc -> Animation_Automation.task[AnmTaskNum].start_frame + 1;}
				AutoFramersUPD();
			}
			break;
			case IDC_IDC_STOP_ANM_FRAMER:
			{
				pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame -= pNMUpDown -> iDelta;
				if(pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame < 2)
					{pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame = 2;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame <= pDoc -> Animation_Automation.task[AnmTaskNum].start_frame)
					{pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame = pDoc -> Animation_Automation.task[AnmTaskNum].start_frame + 1;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame > pDoc -> m_AnimBone -> m_FrameCnt)
					{pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame = pDoc -> m_AnimBone -> m_FrameCnt;}
				AutoFramersUPD();
			}
			break;
			case IDC_ANM_ACTIONS:
			{
				CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_ACTION);
				pDoc -> Animation_Automation.task[AnmTaskNum].action += pNMUpDown -> iDelta;
				if(pDoc -> Animation_Automation.task[AnmTaskNum].action < 0)
					{pDoc -> Animation_Automation.task[AnmTaskNum].action = 0;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].action > pListBox -> GetCount() - 1)
					{pDoc -> Animation_Automation.task[AnmTaskNum].action = pListBox -> GetCount() - 1;}
				pListBox -> SetCurSel(pDoc -> Animation_Automation.task[AnmTaskNum].action);
				pListBox -> SetCurSel(-1);
				ActionUPD();
			}
			break;
			case IDC_ANM_X_MINS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].min[0] -= (double)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_X_MIN, pDoc -> Animation_Automation.task[AnmTaskNum].min[0]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_Y_MINS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].min[1] -= (double)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_Y_MIN, pDoc -> Animation_Automation.task[AnmTaskNum].min[1]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_Z_MINS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].min[2] -= (double)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_Z_MIN, pDoc -> Animation_Automation.task[AnmTaskNum].min[2]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_X_MAXS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].max[0] -= (double)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_X_MAX, pDoc -> Animation_Automation.task[AnmTaskNum].max[0]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_Y_MAXS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].max[1] -= (float)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_Y_MAX, pDoc -> Animation_Automation.task[AnmTaskNum].max[1]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_Z_MAXS:
			{
				m_RewritePermission = FALSE;
				pDoc -> Animation_Automation.task[AnmTaskNum].max[2] -= (double)(pNMUpDown -> iDelta * 0.1f);
				AutoVolUPD(IDC_ANM_Z_MAX, pDoc -> Animation_Automation.task[AnmTaskNum].max[2]);
				m_RewritePermission = TRUE;
			}
			break;
			case IDC_ANM_LAWS:
			{
				CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
				pDoc -> Animation_Automation.task[AnmTaskNum].law += pNMUpDown -> iDelta;
				if(pDoc -> Animation_Automation.task[AnmTaskNum].law < 0)
					{pDoc -> Animation_Automation.task[AnmTaskNum].law = 0;}
				if(pDoc -> Animation_Automation.task[AnmTaskNum].law > pListBox -> GetCount() - 1)
					{pDoc -> Animation_Automation.task[AnmTaskNum].law = pListBox -> GetCount() - 1;}
				//m_user_law = anm_law;
				LawUPD();
			}
			break;
			case IDC_ANM_TASKS:
			{
				AnmTaskNum -= pNMUpDown -> iDelta;
				if(AnmTaskNum < 0)
					{AnmTaskNum = 0;}
				else
				if(AnmTaskNum > 2)
					{AnmTaskNum = 2;}
				FramerUPD();
				ListActionUPD();
				MinMaxUPD();
				ActionUPD();
				LawUPD();
			}
			break;
		};
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CAnimData::TaskNumUPD()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ANM_TASK);
	CString st;
	st.Format("%d", AnmTaskNum);
	pEdit -> SetWindowText(st);
}

void CAnimData::MatrixRewriteUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CheckDlgButton(IDC_MATRIX_REWRITE, pDoc -> Animation_Automation.matrix_rewrite);
}

void CAnimData::AutoFramersUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_START_ANM_FRAME);
	CString st;
	st.Format("%d", pDoc -> Animation_Automation.task[AnmTaskNum].start_frame);
	pEdit -> SetWindowText(st);
	pEdit = (CEdit *)GetDlgItem(IDC_STOP_ANM_FRAME);
	st.Format("%d", pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame);
	pEdit -> SetWindowText(st);
}

void CAnimData::ListActionUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_ACTION);
	pListBox -> SetCurSel(pDoc -> Animation_Automation.task[AnmTaskNum].action);
	pListBox -> SetCurSel(-1);
}

void CAnimData::AutoVolUPD(WPARAM id, float vol)
{
	CEdit *pEdit = (CEdit *)GetDlgItem(id);
	CString st;
	st.Format("%g", vol);
	pEdit -> SetWindowText(st);
}

void CAnimData::FramerUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CString st;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_START_ANM_FRAME);
	if(pDoc -> Animation_Automation.task[AnmTaskNum].start_frame < 1)
		{pDoc -> Animation_Automation.task[AnmTaskNum].start_frame = 1;}
	st.Format("%d", pDoc -> Animation_Automation.task[AnmTaskNum].start_frame);
	pEdit -> SetWindowText(st);
	pEdit = (CEdit *)GetDlgItem(IDC_STOP_ANM_FRAME);
	if(pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame < 2)
		{pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame = 2;}
	st.Format("%d", pDoc -> Animation_Automation.task[AnmTaskNum].stop_frame);
	pEdit -> SetWindowText(st);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_TASK);
	st.Format("%d", AnmTaskNum);
	pEdit -> SetWindowText(st);
}

void CAnimData::OnEnChangeAnmXMin()
	{OnCheckerAutoVol(IDC_ANM_X_MIN);}

void CAnimData::OnEnChangeAnmYMin()
	{OnCheckerAutoVol(IDC_ANM_Y_MIN);}

void CAnimData::OnEnChangeAnmZMin()
	{OnCheckerAutoVol(IDC_ANM_Z_MIN);}

void CAnimData::OnEnChangeAnmXMax()
	{OnCheckerAutoVol(IDC_ANM_X_MAX);}

void CAnimData::OnEnChangeAnmYMax()
	{OnCheckerAutoVol(IDC_ANM_Y_MAX);}

void CAnimData::OnEnChangeAnmZMax()
	{OnCheckerAutoVol(IDC_ANM_Z_MAX);}

void CAnimData::OnEnChangeAnmLaw3Ratio()
	{OnCheckerAutoVol(IDC_ANM_LAW3_RATIO);}

void CAnimData::OnCheckerAutoVol(int control)
{
	if(!m_RewritePermission)
		{return;}
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	m_RewritePermission = FALSE;
	memset(text_buff, 0, _MAX_PATH);
	int nStartChar = 0, nEndChar = 0;
	int dilta = 0;
	CEdit *pEdit = (CEdit *)GetDlgItem(control);
	ASSERT(pEdit);
	pEdit -> GetSel(nStartChar, nEndChar);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	СommaFixer(text_buff);
	dilta = strlen(text_buff);
	ForbiddenSymbolFixer(text_buff);
	dilta -= strlen(text_buff);
	switch(control)
	{
		case IDC_ANM_X_MIN:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].min[0] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_Y_MIN:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].min[1] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_Z_MIN:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].min[2] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_X_MAX:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].max[0] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_Y_MAX:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].max[1] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_Z_MAX:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].max[2] = (float)atof(text_buff);}
		}
		break;
		case IDC_ANM_LAW3_RATIO:
		{
			if(isFloat(text_buff))
				{pDoc -> Animation_Automation.task[AnmTaskNum].koef = (float)atof(text_buff);}
		}
		break;
	};
	pEdit -> SetWindowText(text_buff);
	pEdit -> SetSel(nStartChar - dilta, nEndChar - dilta);
	m_RewritePermission = TRUE;
}

void CAnimData::ActionUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	bool mod = false;
	if(pDoc -> Animation_Automation.task[AnmTaskNum].action == 1 || pDoc -> Animation_Automation.task[AnmTaskNum].action == 2)
		{mod = true;}
	CButton *pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_X);
	pButton -> EnableWindow(mod);
	pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_Y);
	pButton -> EnableWindow(mod);
	pButton = (CButton *)GetDlgItem(IDC_ANM_AXIS_Z);
	pButton -> EnableWindow(mod);
	if(!mod)
	{
		CheckDlgButton(IDC_ANM_AXIS_X, 0);
		CheckDlgButton(IDC_ANM_AXIS_Y, 0);
		CheckDlgButton(IDC_ANM_AXIS_Z, 0);
		OnBnClickedAnmAxisX();
		OnBnClickedAnmAxisY();
		OnBnClickedAnmAxisZ();
	}
	else
	{
		if(pDoc -> Animation_Automation.task[AnmTaskNum].x)
		{
			CheckDlgButton(IDC_ANM_AXIS_X, 1);
			OnBnClickedAnmAxisX();
		}
		if(pDoc -> Animation_Automation.task[AnmTaskNum].y)
		{
			CheckDlgButton(IDC_ANM_AXIS_Y, 1);
			OnBnClickedAnmAxisY();
		}
		if(pDoc -> Animation_Automation.task[AnmTaskNum].z)
		{
			CheckDlgButton(IDC_ANM_AXIS_Z, 1);
			OnBnClickedAnmAxisZ();
		}
	}
}

void CAnimData::MinMaxUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CString st;
	CEdit *pEdit;
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].min[0]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_X_MIN);
	pEdit -> SetWindowText(st);
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].min[1]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_Y_MIN);
	pEdit -> SetWindowText(st);
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].min[2]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_Z_MIN);
	pEdit -> SetWindowText(st);
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].max[0]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_X_MAX);
	pEdit -> SetWindowText(st);
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].max[1]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_Y_MAX);
	pEdit -> SetWindowText(st);
	st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].max[2]);
	pEdit = (CEdit *)GetDlgItem(IDC_ANM_Z_MAX);
	pEdit -> SetWindowText(st);
}

void CAnimData::LawUPD()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_ANM_LAW3_RATIO);
	CListBox *pListBox = (CListBox *)GetDlgItem(IDC_ANM_LAW);
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_TEXT_LAW);
	pListBox -> SetCurSel(pDoc -> Animation_Automation.task[AnmTaskNum].law);
	pListBox -> SetCurSel(-1);
	m_RewritePermission = FALSE;
	if(pDoc -> Animation_Automation.task[AnmTaskNum].law == 3)
	{
		CString st;
		st.Format("%g", pDoc -> Animation_Automation.task[AnmTaskNum].koef);
		pEdit -> SetWindowText(st);
		pEdit -> EnableWindow(true);
	}
	else
	{
		pEdit -> SetWindowText("");
		pEdit -> EnableWindow(false);
	}
	m_RewritePermission = TRUE;
	switch(pDoc -> Animation_Automation.task[AnmTaskNum].law)
	{
		case 0:
			{pStatic -> SetWindowText("");}
		break; 
		case 1:
			{pStatic -> SetWindowText("dX=(X1-X2)/(K1-K2-1)");}
		break;
		case 2:
			{pStatic -> SetWindowText("X=A*t*t/2");}
		break;
		case 3:
			{pStatic -> SetWindowText("dX(i)=K3/1000*dX(i-1)");}
		break;
		case 4:
			{pStatic -> SetWindowText("I=U/R");}
		break;
		case 5:
		{
			#ifdef ALTERNATIVE_LANG
				pStatic -> SetWindowText("Don't forget to fix this code");
			#else
				pStatic -> SetWindowText("Не забыть исправить этот код");
			#endif
		}
		break;
		default:
			{pStatic -> SetWindowText("");}
		break; 
	}
}

void CAnimData::OnBnClickedMatrixRewrite()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	pDoc -> Animation_Automation.matrix_rewrite = IsDlgButtonChecked(IDC_MATRIX_REWRITE);
}

void CAnimData::OnEnChangeAdAnmBoneName()
{
	if(!m_RewritePermissionBAnm || !m_pSub)
		{return;}
	m_RewritePermissionBAnm = FALSE;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	CListBox *pList = (CListBox *)GetDlgItem(IDC_MBONES);
	int cur = pList -> GetCurSel();
	memset(text_buff, 0, _MAX_PATH);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_AD_ANM_BONE_NAME);
	ASSERT(pEdit);
	pEdit -> SetFocus();
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	int len = strlen(text_buff);
	int nStartChar = 0, nEndChar = 0;
	pEdit -> GetSel(nStartChar, nEndChar);
	int i = 0, j = 0, m  = 0;
	for(;i < len; i++)
	{
		if((text_buff[i] >= 'a' && text_buff[i] <= 'z') || (text_buff[i] >= 'A' && text_buff[i] <= 'Z') || (text_buff[i] >= '0' && text_buff[i] <= '9') || text_buff[i] == '_')
		{
			text_buff[j] = text_buff[i];
			j++;
		}
		else
			{m++;}
	}
	text_buff[j] = 0;
	len = strlen(text_buff);
	if(len > 30)
		{text_buff[30] = 0;}
	bool stop = false;
	char tmpANMBoneName[_MAX_PATH] = {0}, outstr[31] = {0};
	strcpy(tmpANMBoneName, text_buff);
	strcpy(outstr, text_buff);
	for(int i = 0; i < 100 && !stop; i++)
	{
		stop = true;
		for(int i2 = 0; i2 < pDoc -> m_AnimBone -> m_BoneCnt; i2++)
		{
			if(!stricmp(tmpANMBoneName, pDoc -> m_AnimBone -> m_BoneMap[i2]))
			{
				if(m_pSub -> m_Bone == i2) //Нашли себя. Выходим.
				{
					pEdit -> SetWindowText(outstr);
					pEdit -> SetSel(nStartChar - m, nEndChar - m);
					m_RewritePermissionBAnm = TRUE;
					return;
				}
				else
					{stop = false; break;}
			}
		}
		if(!len && i == 0)
			{stop = false;}
		if(!stop)
		{
			memset(tmpANMBoneName, 0, _MAX_PATH);
			sprintf(tmpANMBoneName, "NewBone%d", i);
			strcpy(text_buff, tmpANMBoneName);
		}
	}
	pList -> DeleteString(cur);
	pList -> InsertString(cur, text_buff);
	pList -> SetCurSel(cur);
	if(!len)
		{pEdit -> SetWindowText("");}
	else
	{
		pEdit -> SetWindowText(outstr);
		pEdit -> SetSel(nStartChar - m, nEndChar - m);
	}
	delete[] pDoc -> m_AnimBone -> m_BoneMap[m_pSub -> m_Bone];
	pDoc -> m_AnimBone -> m_BoneMap[m_pSub -> m_Bone] = NULL;
	pDoc -> m_AnimBone -> m_BoneMap[m_pSub -> m_Bone] = new char[strlen(text_buff) + 1];
	strcpy(pDoc -> m_AnimBone -> m_BoneMap[m_pSub -> m_Bone], text_buff);
	SelBone = text_buff;
	m_RewritePermissionBAnm = TRUE;
}

void CAnimData::OnBnClickedAnmBoneClone()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	char bonename[_MAX_PATH];
    int bonesel = 0;
    bonesel = m_BoneList.GetCurSel();
    m_BoneList.GetText(bonesel, bonename);
	CAnimSub *pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	while(pSub)
	{
		if(!stricmp(bonename, pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]))
			{break;}
		pSub = pSub -> m_next;
	}
	if(!pSub)
		{return;}
	#ifdef ALTERNATIVE_LANG
		if(MessageBox("Really clone a bone?", "QUESTION: CAnimData::OnBnClickedAnmBoneClone", MB_YESNO | MB_ICONQUESTION) != IDYES)
			{return;}
	#else
		if(MessageBox("Точно клонировать кость?", "QUESTION: CAnimData::OnBnClickedAnmBoneClone", MB_YESNO | MB_ICONQUESTION) != IDYES)
			{return;}
	#endif
	int indx = pDoc -> m_AnimBone -> OnAddClone(pSub -> m_Bone);
	if(indx == -1)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("The bone is not cloned!", "ERROR: CAnimData::OnBnClickedAnmBoneClone", MB_ICONHAND);
		#else
			MessageBox("Кость не клонирована!", "ERROR: CAnimData::OnBnClickedAnmBoneClone", MB_ICONHAND);
		#endif
		return;
	}
	m_BoneList.AddString(pDoc -> m_AnimBone -> m_BoneMap[indx]);
}

void CAnimData::OnBnClickedAnmBoneExport()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	char bonename[_MAX_PATH], CurPath[_MAX_PATH] = {0};
    int bonesel = 0, n = 0, indx = 0;
	bool saveit = true;
	CAnimSub *t_sub = NULL;
    bonesel = m_BoneList.GetCurSel();
    m_BoneList.GetText(bonesel, bonename);
	CAnimSub *pSub = pDoc -> m_AnimBone -> m_Frames[m_FrameNo].m_head;
	while(pSub)
	{
		if(!stricmp(bonename, pDoc -> m_AnimBone -> m_BoneMap[pSub -> m_Bone]))
			{indx = pSub -> m_Bone; break;}
		pSub = pSub -> m_next;
	}
	if(!pSub)
		{return;}
	CAnimBone *ExportBone = new CAnimBone();
	ExportBone -> m_BoneCnt = 1;
	ExportBone -> m_BoneMap = new char*[1];
	ExportBone -> m_BoneMap[0] = new char[strlen(pDoc -> m_AnimBone -> m_BoneMap[indx]) + 1];
	strcpy(ExportBone -> m_BoneMap[0], pDoc -> m_AnimBone -> m_BoneMap[indx]);
	ExportBone -> m_FrameCnt = pDoc -> m_AnimBone -> m_FrameCnt;
	ExportBone -> m_Frames = new CAnimFrame[ExportBone -> m_FrameCnt];
	do
	{
		pSub = pDoc -> m_AnimBone -> m_Frames[n].m_head;
		while(pSub)
		{
			if(pSub -> m_Bone == indx)
				{break;}
			pSub = pSub -> m_next;
		}
		t_sub = ExportBone -> m_Frames[n].AddSub();
		t_sub -> m_Bone = 0;
		memcpy(&t_sub -> m_Matrix34, &pSub -> m_Matrix34, sizeof(matrix34_t));
		t_sub -> chunk_flags = B_POSITION | B_ORIENTATION;
		t_sub -> m_Visible = (pSub -> m_Visible) ? 1 : 0;
		if(t_sub -> m_Visible)
			{t_sub -> chunk_flags |= B_VISIBLE_ON;}
		else
			{t_sub -> chunk_flags |= B_VISIBLE_OFF;}
		n++;
	}
	while(n < m_FrameCnt);
	#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "ANM-file (*.anm)|*.anm|";
		static char szTitle[] = "Save the exported animation file as:";
	#else
		static char szFilter[] = "ANM-Файл (*.anm)|*.anm|";
		static char szTitle[] = "Сохранить файл экспортированной анимации как:";
	#endif
	strcpy(CurPath, m_FileName.GetBuffer(1));
	InvertFixPathDelim(CurPath);
	CString OutFile = CString(CurPath) + "_Exported_" + bonename;
	CFileDialog dlg(TRUE, "*.anm", OutFile, OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if(dlg.DoModal() == IDOK)
	{
		FILE *fp;
		if((fp = fopen(dlg.m_ofn.lpstrFile, "r")) != NULL)
		{
			fclose(fp);
			int save = 0;
			#ifdef ALTERNATIVE_LANG
				save = MessageBox(CString("File:\n" + CString(dlg.m_ofn.lpstrFile) + "\nalready exists. Replace?"), "WARNING: CSOEditDoc::OnBnClickedAnmBoneExport", MB_YESNO | MB_ICONEXCLAMATION);
			#else
				save = MessageBox(CString("Файл:\n" + CString(dlg.m_ofn.lpstrFile) + "\nуже существует. Заменить?"), "WARNING: CSOEditDoc::OnBnClickedAnmBoneExport", MB_YESNO | MB_ICONEXCLAMATION);
			#endif
			if(save != IDYES)
				{saveit = false;}
		}
		if(saveit)
		{
			if((fp = fopen(dlg.m_ofn.lpstrFile, "wb")) != NULL)
			{
				ExportBone -> ProcessSave(fp);
				fclose(fp);
			}
		}
	}
	delete ExportBone;
}

void CAnimData::OnBnClickedAnmBoneImport()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	char CurPath[_MAX_PATH] = {0};
	#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "ANM-file (*.anm)|*.anm|";
		static char szTitle[] = "Open an imported animation file";
	#else
		static char szFilter[] = "ANM-файл (*.anm)|*.anm|";
		static char szTitle[] = "Открыть импортируемый файл анимации";
	#endif
	strcpy(CurPath, m_FileName.GetBuffer(1));
	InvertFixPathDelim(CurPath);
	CFileDialog dlg(TRUE, "*.anm", CString(CurPath), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if(dlg.DoModal() != IDOK)
		{return;}
	bool loaded = false;
	CAnimBone *ImportBone;
	ImportBone = new CAnimBone(dlg.m_ofn.lpstrFile, loaded);
	if(!loaded)
		{delete ImportBone; return;}
	if(ImportBone -> Header_ID != 0x00040000)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("Only anm files of version 0.4.0.0 can be imported.!", "ERROR: CAnimData::OnBnClickedAnmBoneImport", MB_ICONHAND);
		#else
			MessageBox("Импортировать можно только anm-файлы 0.4.0.0 версии!", "ERROR: CAnimData::OnBnClickedAnmBoneImport", MB_ICONHAND);
		#endif
		delete ImportBone;
		return;
	}
	//Ищем одинаковые названия костей в импортированном файле и исправляем их.
	for(int i = 0; i < ImportBone -> m_BoneCnt; i++)
	{
		for(int i2 = i + 1; i2 < ImportBone -> m_BoneCnt; i2++)
		{
			if(!stricmp(ImportBone -> m_BoneMap[i], ImportBone -> m_BoneMap[i2]))
			{
				bool stop = false;
				for(int i3 = 0; i3 < 100 && !stop; i3++)
				{
					stop = true;
					sprintf(text_buff, "%s_%d", ImportBone -> m_BoneMap[i2], i3);
					for(int i4 = i2; i4 < ImportBone -> m_BoneCnt; i4++)
					{
						if(!stricmp(text_buff, ImportBone -> m_BoneMap[i4]))
							{stop = false; break;}
					}
				}
				delete[] ImportBone -> m_BoneMap[i2];
				ImportBone -> m_BoneMap[i2] = NULL;
				ImportBone -> m_BoneMap[i2] = new char[strlen(text_buff) + 1];
				strcpy(ImportBone -> m_BoneMap[m_pSub -> m_Bone], text_buff);
			}
		}
	}
	//Ищем совпадающие имена костей в основной анимации и импортированном файле. Переименовываем совпавшие кости в импортированной анимации.
	for(int i = 0; i < ImportBone -> m_BoneCnt; i++)
	{
		for(int i2 = 0; i2 < pDoc -> m_AnimBone -> m_BoneCnt; i2++)
		{
			if(!stricmp(ImportBone -> m_BoneMap[i], pDoc -> m_AnimBone -> m_BoneMap[i2]))
			{
				
				bool stop = false;
				for(int i3 = 0; i3 < 100 && !stop; i3++)
				{
					stop = true;
					sprintf(text_buff, "%s_%d", ImportBone -> m_BoneMap[i], i3);
					for(int i4 = 0; i4 < pDoc -> m_AnimBone -> m_BoneCnt; i4++)
					{
						if(!stricmp(text_buff, pDoc -> m_AnimBone -> m_BoneMap[i4]))
							{stop = false; break;}
					}
				}
				delete[] ImportBone -> m_BoneMap[i];
				ImportBone -> m_BoneMap[i] = NULL;
				ImportBone -> m_BoneMap[i] = new char[strlen(text_buff) + 1];
				strcpy(ImportBone -> m_BoneMap[i], text_buff);
			}
		}
	}
	//Синхронизируем количество кадров.
	if(pDoc -> m_AnimBone -> m_FrameCnt < ImportBone -> m_FrameCnt)
	{
		int delta = ImportBone -> m_FrameCnt - pDoc -> m_AnimBone -> m_FrameCnt;
		for(int i = 0; i < delta; i++)
			{pDoc -> m_AnimBone -> OnInsertFrame(pDoc -> m_AnimBone -> m_FrameCnt - 1);}
	}
	else
	if(pDoc -> m_AnimBone -> m_FrameCnt > ImportBone -> m_FrameCnt)
	{
		int delta = pDoc -> m_AnimBone -> m_FrameCnt - ImportBone -> m_FrameCnt;
		for(int i = 0; i < delta; i++)
			{ImportBone -> OnInsertFrame(ImportBone -> m_FrameCnt - 1);}
	}
	m_FrameNo = 0;
	//Расширяем пространство имён
	char **NewBoneMap = NULL;
	int NBone = pDoc -> m_AnimBone -> m_BoneCnt + ImportBone -> m_BoneCnt;
	NewBoneMap = new char*[NBone];
	memset(NewBoneMap, 0, sizeof(char*) * NBone);
	for(int i = 0; i < pDoc -> m_AnimBone -> m_BoneCnt; i++)
		{NewBoneMap[i] = pDoc -> m_AnimBone -> m_BoneMap[i];}
	memset(pDoc -> m_AnimBone -> m_BoneMap, 0, sizeof(char*) * pDoc -> m_AnimBone -> m_BoneCnt);
	delete pDoc -> m_AnimBone -> m_BoneMap;
	pDoc -> m_AnimBone -> m_BoneMap = NewBoneMap;
	for(int i = pDoc -> m_AnimBone -> m_BoneCnt, i2 = 0; i < NBone; i++, i2++)
	{
		pDoc -> m_AnimBone -> m_BoneMap[i] = new char[strlen(ImportBone -> m_BoneMap[i2]) + 1];
		strcpy(pDoc -> m_AnimBone -> m_BoneMap[i], ImportBone -> m_BoneMap[i2]);
		m_BoneList.AddString(ImportBone -> m_BoneMap[i2]);
	}
	//Переиндексируем имена в фреймах импортированной анимации.
	CAnimSub *pSub = NULL;
	for(int i = 0; i < ImportBone -> m_FrameCnt; i++)
	{
		pSub = ImportBone -> m_Frames[i].m_head;
		while(pSub)
		{
			pSub -> m_Bone += pDoc -> m_AnimBone -> m_BoneCnt;
			pSub = pSub -> m_next;
		}
	}
	pDoc -> m_AnimBone -> m_BoneCnt = NBone;
	//Добавляем новые кости с копированием данных в основную анимацию.
	CAnimSub *t_sub = NULL;
	for(int i = 0; i < pDoc -> m_AnimBone -> m_FrameCnt; i++)
	{
		pSub = ImportBone -> m_Frames[i].m_head;
		while(pSub)
		{
			t_sub = pDoc -> m_AnimBone -> m_Frames[i].AddSub();
			t_sub -> m_Bone = pSub -> m_Bone;
			memcpy(&t_sub -> m_Matrix34, &pSub -> m_Matrix34, sizeof(matrix34_t));
			t_sub -> chunk_flags = pSub -> chunk_flags;
			t_sub -> m_Visible = pSub -> m_Visible;
			pSub = pSub -> m_next;
		}
	}
	//Обновляем текущий интерфейс
	m_FrameCnt = pDoc -> m_AnimBone -> m_FrameCnt;
	m_Frame = m_FrameNo;
	pDoc -> m_Frame = m_FrameNo;
	OnSelchangeMbones();
	char szValue[_MAX_PATH];
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FRAMENO);
	sprintf(szValue, "%d", m_Frame + 1);
	pEdit -> SetWindowText(szValue);
	pEdit = (CEdit *)GetDlgItem(IDC_FRAMECNT);
	sprintf(szValue, "%d", m_FrameCnt);
	pEdit -> SetWindowText(szValue);
	delete ImportBone;
}

///////////////////////////  REVERSE ANIM /////////////////////////// 
void CAnimData::OnReverseAnimation()
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	CSOEditDoc* pDoc = (CSOEditDoc*)pWnd->GetActiveDocument();
	CAnimBone* pBone = pDoc->m_AnimBone;

	if (!pBone || pBone->m_FrameCnt <= 1) return;

	int left = 0;
	int right = pBone->m_FrameCnt - 1;

	while (left < right)
	{
		// We swap the contents of the frames manually to avoid 
		// triggering the destructor (which would delete the sub-bones)
		CAnimFrame& frame1 = pBone->m_Frames[left];
		CAnimFrame& frame2 = pBone->m_Frames[right];

		// Swap the linked list pointers
		CAnimSub* tempHead = frame1.m_head;
		CAnimSub* tempTail = frame1.m_tail;
		frame1.m_head = frame2.m_head;
		frame1.m_tail = frame2.m_tail;
		frame2.m_head = tempHead;
		frame2.m_tail = tempTail;

		// Swap other frame-level properties
		int tempBone = frame1.m_Bone;
		frame1.m_Bone = frame2.m_Bone;
		frame2.m_Bone = tempBone;

		matrix34_t tempMatrix = frame1.m_Matrix34;
		frame1.m_Matrix34 = frame2.m_Matrix34;
		frame2.m_Matrix34 = tempMatrix;

		left++;
		right--;
	}

	// Refresh the UI to show the change
	OnSelchangeMbones();
	MessageBox("Animation Reversed!", "Success", MB_OK);
}

/////////////////////////// RESET BONES /////////////////////////// 
void CAnimData::OnBnClickedResetBone()
{
	// Set the identity matrix in the dialog member variables
	m_M1_1 = 1.0f; m_M1_2 = 0.0f; m_M1_3 = 0.0f;
	m_M2_1 = 0.0f; m_M2_2 = 1.0f; m_M2_3 = 0.0f;
	m_M3_1 = 0.0f; m_M3_2 = 0.0f; m_M3_3 = 1.0f;
	m_M4_1 = 0.0f; m_M4_2 = 0.0f; m_M4_3 = 0.0f;

	// Push values to the screen text boxes.
	// This fires ON_EN_CHANGE for each box, but m_RewritePermission
	// blocks them from saving prematurely while we're still writing.
	m_RewritePermission = FALSE;
	SetMatrixData(IDC_M1_1, m_M1_1); SetMatrixData(IDC_M1_2, m_M1_2); SetMatrixData(IDC_M1_3, m_M1_3);
	SetMatrixData(IDC_M2_1, m_M2_1); SetMatrixData(IDC_M2_2, m_M2_2); SetMatrixData(IDC_M2_3, m_M2_3);
	SetMatrixData(IDC_M3_1, m_M3_1); SetMatrixData(IDC_M3_2, m_M3_2); SetMatrixData(IDC_M3_3, m_M3_3);
	SetMatrixData(IDC_M4_1, m_M4_1); SetMatrixData(IDC_M4_2, m_M4_2); SetMatrixData(IDC_M4_3, m_M4_3);
	m_RewritePermission = TRUE;

	// Now trigger a single save through the normal path,
	// exactly the same way OnPasteMatrix does it.
	OnCheckerMatrix11();
}

/////////////////////////// COPY-PASTE /////////////////////////// 
void CAnimData::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, ID_COPY_MATRIX, "Copy Matrix Values");
	menu.AppendMenu(MF_STRING, ID_PASTE_MATRIX, "Paste Matrix Values");

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CAnimData::OnCopyMatrix()
{
	UpdateData(TRUE); // Get numbers from the screen boxes

	// We treat the clipboard like a list of 12 floats
	float* pClipboard = (float*)&g_MatrixClipboard;

	pClipboard[0] = m_M1_1;  pClipboard[1] = m_M1_2;  pClipboard[2] = m_M1_3;
	pClipboard[3] = m_M2_1;  pClipboard[4] = m_M2_2;  pClipboard[5] = m_M2_3;
	pClipboard[6] = m_M3_1;  pClipboard[7] = m_M3_2;  pClipboard[8] = m_M3_3;
	pClipboard[9] = m_M4_1;  pClipboard[10] = m_M4_2; pClipboard[11] = m_M4_3;

	MessageBox("Matrix Copied!", "Clipboard", MB_OK);
}
void CAnimData::OnPasteMatrix()
{
	// We treat the clipboard like a list of 12 floats
	float* pClipboard = (float*)&g_MatrixClipboard;

	m_M1_1 = pClipboard[0];  m_M1_2 = pClipboard[1];  m_M1_3 = pClipboard[2];
	m_M2_1 = pClipboard[3];  m_M2_2 = pClipboard[4];  m_M2_3 = pClipboard[5];
	m_M3_1 = pClipboard[6];  m_M3_2 = pClipboard[7];  m_M3_3 = pClipboard[8];
	m_M4_1 = pClipboard[9];  m_M4_2 = pClipboard[10]; m_M4_3 = pClipboard[11];

	UpdateData(FALSE);    // Push the new numbers back to the screen boxes
	OnCheckerMatrix11(); // This tells the program "Hey, the data changed, please save it!"
}
