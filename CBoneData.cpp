// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "SOEdit.h"
#include "CBoneData.h"
#include "general.h"
#include "CPly.h"
#include "PLY_Tweaker.h"

static bool AppendSequenceEvent(CSequence* seq, int frame, const char* name)
{
	if (!seq || !name || !strlen(name))
		return false;
	AnimEvent* events = new AnimEvent[seq->m_EventCount + 1];
	for (int i = 0; i < seq->m_EventCount; i++)
	{
		events[i] = seq->m_Events[i];
	}
	events[seq->m_EventCount].frame = frame;
	events[seq->m_EventCount].name = new char[strlen(name) + 1];
	strcpy(events[seq->m_EventCount].name, name);
	delete[] seq->m_Events;
	seq->m_Events = events;
	seq->m_EventCount++;
	return true;
}

static bool IsIntegerText(const char* text)
{
	if (!text || !strlen(text))
		return false;
	int i = 0;
	if (text[0] == '-' || text[0] == '+')
		i = 1;
	if (!text[i])
		return false;
	for (; text[i]; i++)
	{
		if (text[i] < '0' || text[i] > '9')
			return false;
	}
	return true;
}

static void RemoveSequenceEvent(CSequence* seq, int index)
{
	if (!seq || !seq->m_Events || index < 0 || index >= seq->m_EventCount)
		return;
	delete[] seq->m_Events[index].name;
	if (seq->m_EventCount == 1)
	{
		delete[] seq->m_Events;
		seq->m_Events = NULL;
		seq->m_EventCount = 0;
		return;
	}
	AnimEvent* events = new AnimEvent[seq->m_EventCount - 1];
	int dst = 0;
	for (int i = 0; i < seq->m_EventCount; i++)
	{
		if (i == index)
			continue;
		events[dst++] = seq->m_Events[i];
	}
	delete[] seq->m_Events;
	seq->m_Events = events;
	seq->m_EventCount--;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoneData dialog

CBoneData::CBoneData(CWnd* pParent /*=NULL*/)
	: CDialog(CBoneData::IDD, pParent)
{
	m_BoneName = _T("");
	m_Parameters = _T("");
	m_VolumeView = _T("");
	m_ParentBone = _T("");
	m_LimitsLo = m_Matrix12 = m_LimitsHi = m_LimitsCommon = 0.0f;
	m_Matrix33 = m_Matrix22 = m_Matrix11 = 1.0f;
	m_Matrix32 = m_Matrix31 = m_Matrix23 = m_Matrix21 = m_Matrix13 = 0.0f;
	m_PositionX = m_Matrix43 = m_Matrix42 = m_Matrix41 = 0.0f;
	m_Orientation12 = m_Orientation11 = m_PositionZ = m_PositionY = 0.0f;
	m_Orientation33 = m_Orientation22 = m_Orientation21 = m_Orientation13 = 0.0f;
	m_Speed = m_Orientation32 = m_Orientation31 = m_Orientation23 = 0.0f;
	RotateType = TNone;
	WindowMode = TFull;
	has_sub = m_Ground = m_LODView = m_LOD_OFF = m_Speed2 = HasLimits = m_Terminator = FALSE;
	m_NoCastShadows = m_DecalTarget = m_NoGroupMesh = m_NoGetShadows = FALSE;
	bone_color = 0;
	forced_ply_upd = SeqSelMode = bone_colorized = false;
	m_SequenceList = NULL;
	m_Layer = SequenceIndex = -1;
	pSeq = NULL;
	m_Visible = 0;
	m_RewritePermission = TRUE;
}

void CBoneData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if(!m_VolumeView.GetLength())
	{
		m_LODView = m_LOD_OFF = FALSE;
		m_Ground = m_NoCastShadows = m_DecalTarget = m_NoGroupMesh = m_NoGetShadows = FALSE;
		m_Layer = -1;
	}
	DDX_Text(pDX, IDC_BONENAME, m_BoneName);
	DDX_Check(pDX, IDC_GROUND, m_Ground);
	DDX_Text(pDX, IDC_LIMITS_COMMON, m_LimitsCommon);
	DDX_Text(pDX, IDC_LIMITS_HI, m_LimitsHi);
	DDX_Text(pDX, IDC_LIMITS_LO, m_LimitsLo);
	DDX_Text(pDX, IDC_MATRIX11, m_Matrix11);
	DDX_Text(pDX, IDC_MATRIX12, m_Matrix12);
	DDX_Text(pDX, IDC_MATRIX13, m_Matrix13);
	DDX_Text(pDX, IDC_MATRIX21, m_Matrix21);
	DDX_Text(pDX, IDC_MATRIX22, m_Matrix22);
	DDX_Text(pDX, IDC_MATRIX23, m_Matrix23);
	DDX_Text(pDX, IDC_MATRIX31, m_Matrix31);
	DDX_Text(pDX, IDC_MATRIX32, m_Matrix32);
	DDX_Text(pDX, IDC_MATRIX33, m_Matrix33);
	DDX_Text(pDX, IDC_MATRIX41, m_Matrix41);
	DDX_Text(pDX, IDC_MATRIX42, m_Matrix42);
	DDX_Text(pDX, IDC_MATRIX43, m_Matrix43);
	DDX_Text(pDX, IDC_BONE_SPEED, m_Speed);
	DDX_Text(pDX, IDC_PARAMETERS, m_Parameters);
	DDX_Check(pDX, IDC_LIMITS, HasLimits);
	DDX_Check(pDX, IDC_TERMINATOR, m_Terminator);
	DDX_Text(pDX, IDC_VOLUME_VIEW, m_VolumeView);
	DDX_Text(pDX, IDC_PARENT_BONE, m_ParentBone);
	DDX_Check(pDX, IDC_VISIBLE, m_Visible);
	DDX_Check(pDX, IDC_COLORIZED, bone_colorized);
	DDX_Check(pDX, IDC_B_SPEED2, m_Speed2);
	if(m_LODView)
		{DDX_Text(pDX, IDC_LOD_LAYER, m_Layer);}
	else
		{LayerUPD();}
	DDX_Check(pDX, IDC_LOD_OFF, m_LOD_OFF);
	DDX_Check(pDX, IDC_LOD, m_LODView);
	DDX_Check(pDX, IDC_NO_CAST_SHADOWS, m_NoCastShadows);
	DDX_Check(pDX, IDC_DECAL_TARGET, m_DecalTarget);
	DDX_Check(pDX, IDC_NO_GROUP_MESH, m_NoGroupMesh);
	DDX_Check(pDX, IDC_NO_GET_SHADOWS, m_NoGetShadows);
	CEdit *pEditCom = (CEdit *)GetDlgItem(IDC_LIMITS_COMMON);
	CEdit *pEditHi = (CEdit *)GetDlgItem(IDC_LIMITS_HI);
	CEdit *pEditLo = (CEdit *)GetDlgItem(IDC_LIMITS_LO);
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pEditCom -> EnableWindow(false);
	pEditHi -> EnableWindow(false);
	pEditLo -> EnableWindow(false);
	pButtonLim -> EnableWindow(false);
	BOOL RType = FALSE;
	DDX_Check(pDX, IDC_RADIO1, RType);
	DDX_Check(pDX, IDC_RADIO2, RType); 
	DDX_Check(pDX, IDC_RADIO3, RType);
	DDX_Check(pDX, IDC_RADIO4, RType);
	RType = TRUE;
	switch(RotateType)
	{
		case TRevolute:
		{
			DDX_Check(pDX, IDC_RADIO1, RType);
			if(HasLimits)
			{
				pEditHi -> EnableWindow(true);
				pEditLo -> EnableWindow(true);
			}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TPrizmatic:
		{
			DDX_Check(pDX, IDC_RADIO2, RType);
			if(HasLimits)
			{
				pEditHi -> EnableWindow(true);
				pEditLo -> EnableWindow(true);
			}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TSocket:
		{
			DDX_Check(pDX, IDC_RADIO3, RType);
			if(HasLimits)
				{pEditCom -> EnableWindow(true);}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TNone:
		{
			DDX_Check(pDX, IDC_RADIO4, RType);
			m_LimitsCommon = m_LimitsHi = m_LimitsLo = 0.0f;
		}
		break;
	};
	if(m_SequenceList)
	{
		CSequence *pSeq = m_SequenceList -> GetFirst();
		CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
		pList -> ResetContent();
		while(pSeq)
		{
			pList -> AddString(pSeq -> m_Name);
			pSeq = pSeq -> next;
		}
	}
}
BEGIN_MESSAGE_MAP(CBoneData, CDialog)
	//{{AFX_MSG_MAP(CBoneData)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	//ON_WM_CREATE()
	ON_BN_CLICKED(IDC_RADIO1, OnRevolute)
	ON_BN_CLICKED(IDC_RADIO2, OnPrizmatic)
	ON_BN_CLICKED(IDC_RADIO3, OnSocket)
	ON_BN_CLICKED(IDC_RADIO4, OnNone)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_BONENAME, OnCheckerBoneName)
	ON_EN_CHANGE(IDC_PARENT_BONE, OnCheckerParent_Bone)
	ON_EN_CHANGE(IDC_MATRIX11, OnCheckerMatrix11)
	ON_EN_CHANGE(IDC_MATRIX12, OnCheckerMatrix12)
	ON_EN_CHANGE(IDC_MATRIX13, OnCheckerMatrix13)
	ON_EN_CHANGE(IDC_MATRIX21, OnCheckerMatrix21)
	ON_EN_CHANGE(IDC_MATRIX22, OnCheckerMatrix22)
	ON_EN_CHANGE(IDC_MATRIX23, OnCheckerMatrix23)
	ON_EN_CHANGE(IDC_MATRIX31, OnCheckerMatrix31)
	ON_EN_CHANGE(IDC_MATRIX32, OnCheckerMatrix32)
	ON_EN_CHANGE(IDC_MATRIX33, OnCheckerMatrix33)
	ON_EN_CHANGE(IDC_MATRIX41, OnCheckerMatrix41)
	ON_EN_CHANGE(IDC_MATRIX42, OnCheckerMatrix42)
	ON_EN_CHANGE(IDC_MATRIX43, OnCheckerMatrix43)
	ON_EN_CHANGE(IDC_BONE_SPEED, OnBoneSpeed)
	ON_EN_CHANGE(IDC_LIMITS_COMMON, OnCheckerLimits_Common)
	ON_EN_CHANGE(IDC_LIMITS_HI, OnCheckerLimits_Hi)
	ON_EN_CHANGE(IDC_LIMITS_LO, OnCheckerLimits_Lo)
	ON_EN_CHANGE(IDC_PARAMETERS, OnCheckerParameters)
	ON_EN_CHANGE(IDC_VOLUME_VIEW, OnVolumeView)
	ON_BN_CLICKED(IDC_GROUND, OnChecker)
	ON_BN_CLICKED(IDC_TERMINATOR, OnChecker)
	ON_BN_CLICKED(IDC_LIMITS, OnChecker)
	ON_BN_CLICKED(IDC_VISIBLE, OnChecker)
	ON_BN_CLICKED(IDC_B_SPEED2, OnChecker)
	ON_BN_CLICKED(IDC_NO_CAST_SHADOWS, OnChecker)
	ON_BN_CLICKED(IDC_NO_GET_SHADOWS, OnChecker)
	ON_BN_CLICKED(IDC_NO_GROUP_MESH, OnChecker)
	ON_BN_CLICKED(IDC_DECAL_TARGET, OnChecker)
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_AUTOSTART, OnChecker2)
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_STORE, OnChecker2)
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_RESUME, OnChecker2)
	ON_BN_CLICKED(IDC_ROUND_VOLUME_BUTTON, OnRoundBoneClick)
	ON_BN_CLICKED(IDC_BONE_COLOR_SEL, OnColorSel)
	ON_LBN_SELCHANGE(IDC_BONE_SEQUENCE, OnSelchangeElements)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_ADD, &CBoneData::OnBnClickedBoneSequenceAdd)
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_DELETE, &CBoneData::OnBnClickedBoneSequenceDelete)
	ON_BN_CLICKED(IDC_BUTTON5, &CBoneData::OnBnClickedAddSequenceEvent)
	ON_BN_CLICKED(IDC_BUTTON6, &CBoneData::OnBnClickedRemoveSequenceEvent)
	ON_LBN_SELCHANGE(IDC_LIST2, &CBoneData::OnSelchangeSequenceEvents)
	ON_EN_CHANGE(IDC_BONE_SEQUENCE_SPEED, &CBoneData::OnEnChangeBoneSequenceSpeed)
	ON_EN_CHANGE(IDC_BONE_SEQUENCE_NAME, &CBoneData::OnEnChangeBoneSequenceName)
	ON_BN_CLICKED(IDC_BONE_SEQUENCE_FILE_BROWSE, &CBoneData::OnBnClickedBoneSequenceFileBrowse)
	ON_EN_CHANGE(IDC_BONE_SEQUENCE_FILE, &CBoneData::OnEnChangeBoneSequenceFile)
	ON_BN_CLICKED(IDC_COLORIZED, &CBoneData::OnBnClickedColorized)
	ON_BN_CLICKED(IDC_LOD_OFF, &CBoneData::OnBnClickedLodOff)
	ON_BN_CLICKED(IDC_LOD, &CBoneData::OnBnClickedLod)
	ON_EN_CHANGE(IDC_BONE_SEQUENCE_SMOOTH, &CBoneData::OnEnChangeBoneSequenceSmooth)
	ON_BN_CLICKED(IDC_BONE_WRENCH, &CBoneData::OnBnClickedBoneWrench)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoneData message handlers

void CBoneData::OnRevolute()
{
	RotateType = TRevolute;
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pButtonLim -> EnableWindow(true);
	OnChecker();
}

void CBoneData::OnPrizmatic()
{
	RotateType = TPrizmatic;
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pButtonLim -> EnableWindow(true);
	OnChecker();
}

void CBoneData::OnSocket()
{
	RotateType = TSocket;
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pButtonLim -> EnableWindow(true);
	OnChecker();
}

void CBoneData::OnNone()
{
	RotateType = TNone;
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pButtonLim -> EnableWindow(false);
	OnChecker();
}

void CBoneData::OnBrowse()
{
	#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "PLY-file (*.ply)|*.ply|";
		static char szTitle[] = "Opening a mesh file";
	#else
		static char szFilter[] = "PLY-ôàéë (*.ply)|*.ply|";
		static char szTitle[] = "Îòêğûòèå ìıø-ôàéëà";
	#endif
	CFileDialog dlg(TRUE, "*.ply", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if(dlg.DoModal() == IDOK)
	{
		m_VolumeView = dlg.m_ofn.lpstrFile;
		UpdateData(false);
	}
}

void CBoneData::OnVolumeView()
{
	memset(text_buff, 0, _MAX_PATH);
	m_VolumeView = "";
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOLUME_VIEW);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	m_VolumeView = text_buff;
}

void CBoneData::OnCheckerBoneName()
{
	memset(text_buff, 0, _MAX_PATH);
	m_BoneName = "";
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONENAME);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	m_BoneName = text_buff;
}

void CBoneData::OnCheckerParent_Bone()
{
	memset(text_buff, 0, _MAX_PATH);
	m_ParentBone = "";
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_PARENT_BONE);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	m_ParentBone = text_buff;
}

void CBoneData::OnCheckerMatrix11()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix11 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX11);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix11 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix12()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix12 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX12);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix12 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix13()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix13 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX13);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix13 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix21()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix21 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX21);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix21 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix22()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix22 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX22);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix22 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix23()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix23 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX23);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix23 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix31()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix31 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX31);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix31 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix32()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix32 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX32);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix32 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix33()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix33 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX33);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix33 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix41()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix41 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX41);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix41 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix42()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix42 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX42);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix42 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerMatrix43()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Matrix43 = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_MATRIX43);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Matrix43 = (float)atof(text_buff);}
}

void CBoneData::OnCheckerLimits_Common()
{
	memset(text_buff, 0, _MAX_PATH);
	m_LimitsCommon = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_COMMON);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_LimitsCommon = (float)atof(text_buff);}
}

void CBoneData::OnCheckerLimits_Hi()
{
	memset(text_buff, 0, _MAX_PATH);
	m_LimitsHi = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_HI);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_LimitsHi = (float)atof(text_buff);}
}

void CBoneData::OnCheckerLimits_Lo()
{
	memset(text_buff, 0, _MAX_PATH);
	m_LimitsLo = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_LO);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_LimitsLo = (float)atof(text_buff);}
}

void CBoneData::OnCheckerParameters()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Parameters = "";
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_PARAMETERS);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	m_Parameters = text_buff;
}

void CBoneData::OnChecker()
{
	m_Ground = IsDlgButtonChecked(IDC_GROUND);
	m_Terminator = IsDlgButtonChecked(IDC_TERMINATOR);
	HasLimits = IsDlgButtonChecked(IDC_LIMITS);
	m_Visible = IsDlgButtonChecked(IDC_VISIBLE);
	m_Speed2 = IsDlgButtonChecked(IDC_B_SPEED2);
	m_NoCastShadows = IsDlgButtonChecked(IDC_NO_CAST_SHADOWS);
	m_DecalTarget = IsDlgButtonChecked(IDC_DECAL_TARGET);
	m_NoGroupMesh = IsDlgButtonChecked(IDC_NO_GROUP_MESH);
	m_NoGetShadows = IsDlgButtonChecked(IDC_NO_GET_SHADOWS);
	CEdit *pEditCom = (CEdit *)GetDlgItem(IDC_LIMITS_COMMON);
	CEdit *pEditHi = (CEdit *)GetDlgItem(IDC_LIMITS_HI);
	CEdit *pEditLo = (CEdit *)GetDlgItem(IDC_LIMITS_LO);
	CButton *pButtonLim = (CButton *)GetDlgItem(IDC_LIMITS);
	pEditCom -> EnableWindow(false);
	pEditHi -> EnableWindow(false);
	pEditLo -> EnableWindow(false);
	pButtonLim -> EnableWindow(false);
	switch(RotateType)
	{
		case TRevolute:
		{
			if(HasLimits)
			{
				pEditHi -> EnableWindow(true);
				pEditLo -> EnableWindow(true);
			}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TPrizmatic:
		{
			if(HasLimits)
			{
				pEditHi -> EnableWindow(true);
				pEditLo -> EnableWindow(true);
			}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TSocket:
		{
			if(HasLimits)
				{pEditCom -> EnableWindow(true);}
			pButtonLim -> EnableWindow(true);
		}
		break;
		case TNone:
		{
			if(HasLimits)
				{CheckDlgButton(IDC_LIMITS, 0);}
		}
		break;
	};
}

void CBoneData::OnBoneSpeed()
{
	memset(text_buff, 0, _MAX_PATH);
	m_Speed = 0.0f;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SPEED);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	ÑommaFixer(text_buff);
	ForbiddenSymbolFixer(text_buff);
	if(isFloat(text_buff))
		{m_Speed = (float)atof(text_buff);}
}

void CBoneData::OnRoundBoneClick()
{
	m_Matrix11 = FixBoneMX(IDC_MATRIX11);
	m_Matrix12 = FixBoneMX(IDC_MATRIX12);
	m_Matrix13 = FixBoneMX(IDC_MATRIX13);
	m_Matrix21 = FixBoneMX(IDC_MATRIX21);
	m_Matrix22 = FixBoneMX(IDC_MATRIX22);
	m_Matrix23 = FixBoneMX(IDC_MATRIX23);
	m_Matrix31 = FixBoneMX(IDC_MATRIX31);
	m_Matrix32 = FixBoneMX(IDC_MATRIX32);
	m_Matrix33 = FixBoneMX(IDC_MATRIX33);
	UpdateData(false);
}

float CBoneData::FixBoneMX(int id)
{
	CEdit *pEdit;
	memset(text_buff, 0, _MAX_PATH);
	pEdit = (CEdit *)GetDlgItem(id);
	ASSERT(pEdit);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	return (float)round(atof(text_buff));
}

void CBoneData::OnPaint()
{
	CPaintDC dc(this);
	DrawSwatch();
}

void CBoneData::DrawSwatch()
{
	CWnd *pWnd = (CWnd *)GetDlgItem(IDC_BONE_SWATCH);
	CDC *pDC = pWnd -> GetDC();
	bone_color = bone_color & 0x00ffffff;
	DWORD rgb = bone_color;
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

void CBoneData::OnColorSel()
{
	DWORD rgb = bone_color;
	CColorDialog Dlg(0, 0, this);
	Dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	Dlg.m_cc.rgbResult = rgb;
	if(Dlg.DoModal() == IDOK)
	{
		bone_color = RGB(Dlg.m_cc.rgbResult & 0xFF, (Dlg.m_cc.rgbResult >> 8) & 0xFF, (Dlg.m_cc.rgbResult >> 16) & 0xFF);
		DrawSwatch();
	}
}

void CBoneData::OnSelchangeElements()
{
	if(m_SequenceList)
	{
		pSeq = m_SequenceList -> GetFirst();
		CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
		CEdit *pEdit = NULL;
		char szWindowText[_MAX_PATH] = { 0 };
		SequenceIndex = pList -> GetCurSel();
		if(SequenceIndex < 0)
			{return;}
		for(int i = 0; pSeq; i++)
		{
			if(SequenceIndex == i)
			{
				SeqSelMode = true;
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
				ASSERT(pEdit);
				pEdit -> SetWindowText(pSeq -> m_Name);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SPEED);
				ASSERT(pEdit);
				sprintf(szWindowText, "%g", pSeq -> m_Speed);
				pEdit -> SetWindowText(szWindowText);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SMOOTH);
				ASSERT(pEdit);
				sprintf(szWindowText, "%g", pSeq -> m_Smooth);
				pEdit -> SetWindowText(szWindowText);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_FILE);
				ASSERT(pEdit);
				pEdit -> SetWindowText(pSeq -> m_FileName);
				if(pSeq -> m_Autostart)
					{CheckDlgButton(IDC_BONE_SEQUENCE_AUTOSTART, 1);}
				else
					{CheckDlgButton(IDC_BONE_SEQUENCE_AUTOSTART, 0);}
				if(pSeq -> m_Store)
					{CheckDlgButton(IDC_BONE_SEQUENCE_STORE, 1);}
				else
					{CheckDlgButton(IDC_BONE_SEQUENCE_STORE, 0);}
				if(pSeq -> m_Resume)
					{CheckDlgButton(IDC_BONE_SEQUENCE_RESUME, 1);}
				else
					{CheckDlgButton(IDC_BONE_SEQUENCE_RESUME, 0);}
				CButton *pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_DELETE);
				pButton -> EnableWindow(true);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SPEED);
				pEdit -> EnableWindow(true);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SMOOTH);
				pEdit -> EnableWindow(true);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_FILE);
				pEdit -> EnableWindow(true);
				pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
				pEdit -> EnableWindow(true);
				pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_AUTOSTART);
				pButton -> EnableWindow(true);
				pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_STORE);
				pButton -> EnableWindow(true);
				pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_RESUME);
				pButton -> EnableWindow(true);
				pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_FILE_BROWSE);
				pButton -> EnableWindow(true);
				pButton = (CButton*)GetDlgItem(IDC_BUTTON5);
				pButton->EnableWindow(true);
				pButton = (CButton*)GetDlgItem(IDC_BUTTON6);
				pButton->EnableWindow(true);
				pEdit = (CEdit*)GetDlgItem(IDC_EDIT4);
				pEdit->EnableWindow(true);
				pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
				pEdit->EnableWindow(true);
				CListBox* pEventList = (CListBox*)GetDlgItem(IDC_LIST2);
				pEventList->EnableWindow(true);
				RefreshSequenceEventList();
				SeqSelMode = false;
				break;
			}
			pSeq = pSeq -> next;
		}
	}
}

void CBoneData::RefreshSequenceEventList()
{
	CListBox* pEventList = (CListBox*)GetDlgItem(IDC_LIST2);
	if (!pEventList)
		return;
	pEventList->ResetContent();
	if (!pSeq || !pSeq->m_Events || pSeq->m_EventCount <= 0)
		return;
	char row[256] = { 0 };
	for (int i = 0; i < pSeq->m_EventCount; i++)
	{
		sprintf(row, "%d | %s", pSeq->m_Events[i].frame, pSeq->m_Events[i].name ? pSeq->m_Events[i].name : "");
		pEventList->AddString(row);
	}
}

void CBoneData::OnChecker2()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
	if(!pSeq || !m_SequenceList || pList -> GetCurSel() < 0)
		{return;}
	pSeq -> m_Autostart = IsDlgButtonChecked(IDC_BONE_SEQUENCE_AUTOSTART);
	pSeq -> m_Store = IsDlgButtonChecked(IDC_BONE_SEQUENCE_STORE);
	pSeq -> m_Resume = IsDlgButtonChecked(IDC_BONE_SEQUENCE_RESUME);
}

void CBoneData::OnBnClickedBoneSequenceAdd()
{
	CEdit *pNameEdit;
	CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
	pNameEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
	ASSERT(pNameEdit);
	CSequence *NewSequence = new CSequence();
	if(!m_SequenceList)
		{m_SequenceList = new CSequenceList();}
	NewSequence -> SetName("NewAnm");
	m_SequenceList -> AddToTail(NewSequence);
	pList -> AddString("NewAnm");
	pList -> SetCurSel(pList -> GetCount() - 1);
	OnSelchangeElements();
	delete NewSequence;
}

void CBoneData::OnBnClickedBoneSequenceDelete()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
	int sel = pList -> GetCurSel();
	int count = pList -> GetCount();
	if(m_SequenceList && sel >= 0)
	{
		memset(text_buff, 0, _MAX_PATH);
		pList -> GetText(sel, text_buff);
		m_SequenceList -> DropSequence(text_buff);
		pList -> DeleteString(sel);
		if(count - 1 > sel)
			{pList -> SetCurSel(sel);}
		else
		if(count == 1)
		{
			pSeq = NULL;
			CheckDlgButton(IDC_BONE_SEQUENCE_AUTOSTART, 0);
			CheckDlgButton(IDC_BONE_SEQUENCE_STORE, 0);
			CheckDlgButton(IDC_BONE_SEQUENCE_RESUME, 0);
			pList -> SetCurSel(-1);
			CButton *pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_DELETE);
			pButton -> EnableWindow(false);
			CEdit *pEdit;
			pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SPEED);
			pEdit -> SetWindowText("");
			pEdit -> EnableWindow(false);
			pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SMOOTH);
			pEdit -> SetWindowText("");
			pEdit -> EnableWindow(false);
			pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_FILE);
			pEdit -> SetWindowText("");
			pEdit -> EnableWindow(false);
			pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
			pEdit -> SetWindowText("");
			pEdit -> EnableWindow(false);
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT4);
			pEdit->SetWindowText("");
			pEdit->EnableWindow(false);
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
			pEdit->SetWindowText("");
			pEdit->EnableWindow(false);
			pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_AUTOSTART);
			pButton -> EnableWindow(false);
			pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_STORE);
			pButton -> EnableWindow(false);
			pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_RESUME);
			pButton -> EnableWindow(false);
			pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_FILE_BROWSE);
			pButton -> EnableWindow(false);
			pButton = (CButton*)GetDlgItem(IDC_BUTTON5);
			pButton->EnableWindow(false);
			pButton = (CButton*)GetDlgItem(IDC_BUTTON6);
			pButton->EnableWindow(false);
			CListBox* pEventList = (CListBox*)GetDlgItem(IDC_LIST2);
			pEventList->ResetContent();
			pEventList->EnableWindow(false);
		}
		else
		if(count - 1 == sel)
			{pList -> SetCurSel(sel - 1);}
		OnSelchangeElements();
	}
}

void CBoneData::OnBnClickedAddSequenceEvent()
{
	if (!pSeq)
		return;
	char frameText[_MAX_PATH] = { 0 };
	char eventName[_MAX_PATH] = { 0 };
	CEdit* pFrame = (CEdit*)GetDlgItem(IDC_EDIT4);
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT5);
	pFrame->GetWindowText(frameText, _MAX_PATH - 1);
	pName->GetWindowText(eventName, _MAX_PATH - 1);
	ForbiddenSymbolFixer(frameText);
	ForbiddenSymbolFixer(eventName);
	if (!IsIntegerText(frameText) || !strlen(eventName))
		return;
	if (!AppendSequenceEvent(pSeq, atoi(frameText), eventName))
		return;
	RefreshSequenceEventList();
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST2);
	pList->SetCurSel(pList->GetCount() - 1);
}

void CBoneData::OnBnClickedRemoveSequenceEvent()
{
	if (!pSeq)
		return;
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST2);
	int cur = pList->GetCurSel();
	if (cur < 0)
		return;
	RemoveSequenceEvent(pSeq, cur);
	RefreshSequenceEventList();
	if (pList->GetCount() > 0)
		pList->SetCurSel(min(cur, pList->GetCount() - 1));
	OnSelchangeSequenceEvents();
}

void CBoneData::OnSelchangeSequenceEvents()
{
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST2);
	int cur = pList->GetCurSel();
	if (!pSeq || cur < 0 || !pSeq->m_Events || cur >= pSeq->m_EventCount)
		return;
	char frameText[64] = { 0 };
	sprintf(frameText, "%d", pSeq->m_Events[cur].frame);
	CEdit* pFrame = (CEdit*)GetDlgItem(IDC_EDIT4);
	CEdit* pName = (CEdit*)GetDlgItem(IDC_EDIT5);
	pFrame->SetWindowText(frameText);
	pName->SetWindowText(pSeq->m_Events[cur].name ? pSeq->m_Events[cur].name : "");
}

void CBoneData::SkeletonOff()
{
	CButton *pButton = (CButton *)GetDlgItem(IDC_BROWSE);
	pButton -> EnableWindow(false);
	//pButton = (CButton *)GetDlgItem(IDC_BONE_WRENCH);
	//pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_ROUND_BONE_BUTTON);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_BONE_COLOR_SEL);
	pButton -> EnableWindow(false);
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONENAME);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_PARENT_BONE);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_VOLUME_VIEW);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX11);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX12);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX13);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX21);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX22);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX23);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX31);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX32);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX33);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX41);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX42);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_MATRIX43);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_BONE_SPEED);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_STATIC);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_HI);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_LO);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_LIMITS_COMMON);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_PARAMETERS);
	pEdit -> EnableWindow(false);
	pEdit = (CEdit *)GetDlgItem(IDC_LOD_LAYER);
	pEdit -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_LOD_OFF);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_LOD);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_NO_CAST_SHADOWS);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_NO_GET_SHADOWS);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_DECAL_TARGET);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_NO_GROUP_MESH);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_TERMINATOR);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO1);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_GROUND);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO3);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_VISIBLE);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO2);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_LIMITS);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_RADIO4);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_B_SPEED2);
	pButton -> EnableWindow(false);
	pButton = (CButton *)GetDlgItem(IDC_COLORIZED);
	pButton -> EnableWindow(false);
	CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_LOD_LAYERS);
	pSpinner -> EnableWindow(false);
}

void CBoneData::AnimationOff()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
	pEdit -> EnableWindow(false);
	CButton *pButton = (CButton *)GetDlgItem(IDC_BONE_SEQUENCE_ADD);
	pButton -> EnableWindow(false);
}

void CBoneData::SubBoneON()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOLUME_VIEW);
	pEdit -> EnableWindow(true);
	pEdit = (CEdit *)GetDlgItem(IDC_LOD_LAYER);
	pEdit -> EnableWindow(true);
	CButton *pButton = (CButton *)GetDlgItem(IDC_BROWSE);
	pButton -> EnableWindow(true);
	//pButton = (CButton *)GetDlgItem(IDC_BONE_WRENCH);
	//pButton -> EnableWindow(true);
	pButton = (CButton *)GetDlgItem(IDC_BONE_COLOR_SEL);
	pButton -> EnableWindow(true);
	CSpinButtonCtrl *pSpinner = (CSpinButtonCtrl *)GetDlgItem(IDC_LOD_LAYERS);
	pSpinner -> EnableWindow(true);
}

void CBoneData::OnEnChangeBoneSequenceSpeed()
{
	if(!m_RewritePermission)
		{return;}
	if(pSeq)
	{
		m_RewritePermission = FALSE;
		int nStartChar = 0, nEndChar = 0;
		int dilta = 0;
		memset(text_buff, 0, _MAX_PATH);
		pSeq -> m_Speed = 0.0f;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SPEED);
		ASSERT(pEdit);
		pEdit -> GetSel(nStartChar, nEndChar);
		pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
		ÑommaFixer(text_buff);
		dilta = strlen(text_buff);
		ForbiddenSymbolFixer(text_buff);
		dilta -= strlen(text_buff);
		if(isFloat(text_buff))
			{pSeq -> m_Speed = (float)atof(text_buff);}
		pEdit -> SetWindowText(text_buff);
		pEdit -> SetSel(nStartChar - dilta, nEndChar - dilta);
		m_RewritePermission = TRUE;
	}
}

void CBoneData::OnEnChangeBoneSequenceName()
{
	if(SeqSelMode)
		{return;}
	SeqSelMode = true;
	CListBox *pList = (CListBox *)GetDlgItem(IDC_BONE_SEQUENCE);
	int cur = pList -> GetCurSel();
	if(pSeq)
	{	
		memset(text_buff, 0, _MAX_PATH);
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_NAME);
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
		if(!len)
		{
			pList -> DeleteString(cur);
			pList -> InsertString(cur, "NewAnm");
			pSeq -> SetName("NewAnm");
			pList -> SetCurSel(cur);
			pEdit -> SetWindowText("");
			SeqSelMode = false;
			return;
		}
		else
		if(len > 30)
			{text_buff[30] = 0;}
		pList -> DeleteString(cur);
		pList -> InsertString(cur, text_buff);
		pList -> SetCurSel(cur);
		pSeq -> SetName(text_buff);
		pEdit -> SetWindowText(text_buff);
		pEdit -> SetSel(nStartChar - m, nEndChar - m);
	}
	SeqSelMode = false;
}

void CBoneData::OnBnClickedBoneSequenceFileBrowse()
{
	#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "ANM-file (*.anm)|*.anm|";
		static char szTitle[] = "Opening an animation file";
	#else
		static char szFilter[] = "ANM-ôàéë (*.anm)|*.anm|";
		static char szTitle[] = "Îòêğûòèå ôàéëà àíèìàöèè";
	#endif
	CFileDialog dlg(TRUE, "*.anm", NULL, OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if(dlg.DoModal() == IDOK)
	{
		if(pSeq)
		{
			CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_FILE);
			char *SPtr = strrchr(dlg.m_ofn.lpstrFile, '\\');
			if(!SPtr)
				{SPtr = dlg.m_ofn.lpstrFile;}
			else
				{SPtr++;}
			pSeq -> SetFile(SPtr);
			pEdit -> SetWindowText(SPtr);
		}
	}
}

void CBoneData::OnEnChangeBoneSequenceFile()
{
	if(SeqSelMode)
		{return;}
	SeqSelMode = true;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_FILE);
	ASSERT(pEdit);
	memset(text_buff, 0, _MAX_PATH);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	if(pSeq)
		{pSeq -> SetFile(text_buff);}
	SeqSelMode = false;
}

void CBoneData::OnBnClickedColorized()
	{bone_colorized = IsDlgButtonChecked(IDC_COLORIZED);}

void CBoneData::OnOK()
{
	if(!m_VolumeView.GetLength() && m_LODView)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("The lod does not contain a mesh. Fix it!", "ERROR: CBoneData::OnOK()", MB_ICONHAND);
		#else
			MessageBox("Ëîä íå ñîäåğæèò ìıø. Èñïğàâü!", "ERROR: CBoneData::OnOK()", MB_ICONHAND);
		#endif
	}
	else
	if(!m_VolumeView.GetLength() && !m_LODView && WindowMode == CBoneData::TSubBone)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox("The sub-mesh does not contain a mesh. Fix it!", "ERROR: CBoneData::OnOK()", MB_ICONHAND);
		#else
			MessageBox("Ñóá. ìıø íå ñîäåğæèò ìıø. Èñïğàâü!", "ERROR: CBoneData::OnOK()", MB_ICONHAND);
		#endif
	}
	else
	{
		UpdateData(false);
		CDialog::OnOK();
	}
}

void CBoneData::OnBnClickedLodOff()
	{m_LOD_OFF = IsDlgButtonChecked(IDC_LOD_OFF);}

void CBoneData::OnBnClickedLod()
{
	CButton *pButtonOFF = (CButton *)GetDlgItem(IDC_LOD_OFF);
	CButton *pButton = NULL;
	m_LODView = IsDlgButtonChecked(IDC_LOD);
	if(!m_LODView)
	{
		m_LOD_OFF = FALSE;
		m_Layer = -1;
		LayerUPD();
		CheckDlgButton(IDC_LOD_OFF, 0);
		pButtonOFF -> EnableWindow(false);
	}
	else
	{
		m_Layer = -1;
		LayerUPD();
		pButtonOFF -> EnableWindow(true);
	}
}

void CBoneData::OnEnChangeBoneSequenceSmooth()
{
	if(!m_RewritePermission)
		{return;}
	if(pSeq)
	{
		m_RewritePermission = FALSE;
		int nStartChar = 0, nEndChar = 0;
		int dilta = 0;
		memset(text_buff, 0, _MAX_PATH);
		pSeq -> m_Smooth = 0.0f;
		CEdit *pEdit = (CEdit *)GetDlgItem(IDC_BONE_SEQUENCE_SMOOTH);
		ASSERT(pEdit);
		pEdit -> GetSel(nStartChar, nEndChar);
		pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
		ÑommaFixer(text_buff);
		dilta = strlen(text_buff);
		ForbiddenSymbolFixer(text_buff);
		dilta -= strlen(text_buff);
		if(isFloat(text_buff))
			{pSeq -> m_Smooth = (float)atof(text_buff);}
		pEdit -> SetWindowText(text_buff);
		pEdit -> SetSel(nStartChar - dilta, nEndChar - dilta);
		m_RewritePermission = TRUE;
	}
}

BOOL CBoneData::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR *nmHdr = (NMHDR *)lParam;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)nmHdr;
	if(nmHdr -> code == UDN_DELTAPOS)
	{
		switch(wParam)
		{
			case IDC_LOD_LAYERS:
			{
				m_Layer -= pNMUpDown -> iDelta;
				if(m_Layer < -1)
					{m_Layer = -1;}
				else
				if(m_Layer > 0 && !m_LODView)
					{m_Layer = 0;}
				else
				if(m_Layer > 10 && m_LODView)
					{m_Layer = 10;}
				LayerUPD();
			}
			break;
		};
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CBoneData::LayerUPD()
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_LOD_LAYER);
	ASSERT(pEdit);
	CString st;
	if(m_LODView)
		{st.Format("%d", m_Layer);}
	else
	{
		switch(m_Layer)
		{
			case -1:
				{st = "";}
			break;
			case 0:
				{st = "WaterScissor";}
			break;
		};
	}
	pEdit -> SetWindowText(st);
}

BOOL CBoneData::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Bone Editor");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_1);
		pWnd -> SetWindowText("Bone:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_2);
		pWnd -> SetWindowText("File:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_3);
		pWnd -> SetWindowText("Parent:");
		pWnd = (CWnd *)GetDlgItem(IDC_ROUND_BONE_BUTTON);
		pWnd -> SetWindowText("Round");
		pWnd = (CWnd *)GetDlgItem(IDC_LOD);
		pWnd -> SetWindowText("Lod");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_4);
		pWnd -> SetWindowText("Layer:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_5);
		pWnd -> SetWindowText("The mash is painted");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_6);
		pWnd -> SetWindowText("Speed");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_7);
		pWnd -> SetWindowText("Limit-1");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_8);
		pWnd -> SetWindowText("Limits-2");
		pWnd = (CWnd *)GetDlgItem(IDC_BONE_COLOR_SEL);
		pWnd -> SetWindowText("Choose");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_9);
		pWnd -> SetWindowText("Parameters:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_10);
		pWnd -> SetWindowText("Bone Animations");
		pWnd = (CWnd *)GetDlgItem(IDC_BONE_SEQUENCE_ADD);
		pWnd -> SetWindowText("Add");
		pWnd = (CWnd *)GetDlgItem(IDC_BONE_SEQUENCE_DELETE);
		pWnd -> SetWindowText("Remove");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_11);
		pWnd -> SetWindowText("Parameters:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_12);
		pWnd -> SetWindowText("Speed");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_13);
		pWnd -> SetWindowText("Current name");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_14);
		pWnd -> SetWindowText("Smoothing");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_15);
		pWnd -> SetWindowText("File");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_BE_16);
		pWnd -> SetWindowText("Animation Events");
		pWnd = (CWnd *)GetDlgItem(IDC_BUTTON5);
		pWnd -> SetWindowText("Add");
		pWnd = (CWnd *)GetDlgItem(IDC_BUTTON6);
		pWnd -> SetWindowText("Remove");
		pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#endif
	CButton *pButtonWrench = (CButton *)GetDlgItem(IDC_BONE_WRENCH);
	CSOEditApp *pApp = (CSOEditApp *)AfxGetApp();
	HICON hIcon = ::LoadIcon(pApp -> m_hInstance, "IDI_WRENCH");
	pButtonWrench -> SetIcon(hIcon);
	CButton *pButtonLOD = (CButton *)GetDlgItem(IDC_LOD);
	CButton *pButtonOFF = (CButton *)GetDlgItem(IDC_LOD_OFF);
	switch(WindowMode)
	{
		case CBoneData::TSubBone:
		{
			SkeletonOff();
			CButton *pButton = (CButton *)GetDlgItem(IDC_NO_CAST_SHADOWS);
			pButton -> EnableWindow(true);
			pButton = (CButton *)GetDlgItem(IDC_NO_GET_SHADOWS);
			pButton -> EnableWindow(true);
			pButton = (CButton *)GetDlgItem(IDC_NO_GROUP_MESH);
			pButton -> EnableWindow(true);
			pButton = (CButton *)GetDlgItem(IDC_DECAL_TARGET);
			pButton -> EnableWindow(true);
			pButton = (CButton *)GetDlgItem(IDC_GROUND);
			pButton -> EnableWindow(true);
			AnimationOff();
			SubBoneON();
			pButtonOFF -> EnableWindow(false);
			pButtonLOD -> EnableWindow(false);
		}
		break;
		case CBoneData::TAnimation:
			{SkeletonOff();}
		break;
		case CBoneData::TFull:
		{
			if(!m_LODView)
				{pButtonOFF -> EnableWindow(false);}
		}
		break;
	}
	if(has_sub)
		{pButtonLOD -> EnableWindow(false);}
 	return TRUE;
}

void CBoneData::OnBnClickedBoneWrench()
{
	//TODO:
	//DO TO...
	//return;
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_VOLUME_VIEW);
	pEdit -> GetWindowText(text_buff, _MAX_PATH - 1);
	if(strlen(text_buff))
	{
		CPly *tPly = new CPly(text_buff);
		if(tPly -> loading_successes)
		{
			PLY_Tweaker pDlg;
			int m_meshs = 0;
			pDlg.BPly_Type = tPly -> Bply;
			memcpy(pDlg.m_basis, tPly -> m_basis, sizeof(v3_t) * 4);
			pDlg.BNDS = tPly -> BNDS;
			memcpy(pDlg.m_bbox, tPly -> m_bbox, sizeof(v3_t) * 2);
			CMesh *pMesh;
			pMesh = tPly -> m_meshlist -> GetFirst();
			while(pMesh)
			{
				pDlg.m_mesh_base[m_meshs].fvf = pMesh -> m_fvf;
				pDlg.m_mesh_base[m_meshs].first_face = pMesh -> m_first;
				pDlg.m_mesh_base[m_meshs].face_count = pMesh -> m_count;
				pDlg.m_mesh_base[m_meshs].flags = pMesh -> m_flags;
				if(pMesh -> m_texname[2])
					{strcpy(pDlg.m_mesh_base[m_meshs].patch[0], pMesh -> m_texname[2]);}
				if(pMesh -> m_texname[3])
					{strcpy(pDlg.m_mesh_base[m_meshs].patch[1], pMesh -> m_texname[3]);}
				pMesh = pMesh -> next;
				m_meshs++;
			}
			pDlg.m_meshs = m_meshs;
			pDlg.m_bones = tPly -> m_bones;
			if(tPly -> m_bones)
			{
				int len = 0;
				pDlg.m_bonelist = new char*[tPly -> m_bones];
				memset(pDlg.m_bonelist, 0, sizeof(char*) * tPly -> m_bones);
				for(int i = 0; i < tPly -> m_bones; i++)
				{
					len = strlen(tPly -> m_bonelist[i]);
					pDlg.m_bonelist[i] = new char[len + 1];
					memset(pDlg.m_bonelist[i], 0, len + 1);
					strcpy(pDlg.m_bonelist[i], tPly -> m_bonelist[i]);
				}
			}
			pDlg.MROR = tPly -> MROR;
			pDlg.m_numpolys = tPly -> m_numpolys;
			pDlg.IndexType = tPly -> IndexType;
			pDlg.ADJA = tPly -> ADJA;
			pDlg.SHDW = tPly -> SHDW;
			pDlg.m_numverts = tPly -> m_numverts;
			pDlg.m_vsize = tPly -> m_vsize;
			pDlg.m_calculated_vsize = tPly -> m_calculated_vsize;
			pDlg.m_vflags = tPly -> m_vflags;
			pDlg.has_pos = tPly -> has_pos;
			pDlg.has_rhw = tPly -> has_rhw;
			pDlg.has_weights = tPly -> has_weights;
			pDlg.has_normal = tPly -> has_normal;
			pDlg.has_psize = tPly -> has_psize;
			pDlg.has_diffuse = tPly -> has_diffuse;
			pDlg.has_mesh_bump = tPly -> has_mesh_bump;
			pDlg.has_specular = tPly -> has_specular;
			pDlg.has_tex_coords = tPly -> has_tex_coords;
			pDlg.has_matrix_indices = tPly -> has_matrix_indices;
			pDlg.has_mesh_specular = tPly -> has_mesh_specular;
			pDlg.has_w = tPly -> has_w;
			pDlg.num_weights = tPly -> num_weights;
			pDlg.num_tex_coords = tPly -> num_tex_coords;
			if(pDlg.DoModal() == IDOK)
			{
				BYTE plen = 0;
				m_meshs = 0;
				pMesh = tPly -> m_meshlist -> GetFirst();
				while(pMesh)
				{
					pMesh -> m_flags = pDlg.m_mesh_base[m_meshs].flags;
					if(pMesh -> m_texname[2])
						{delete[] pMesh -> m_texname[2]; pMesh -> m_texname[2] = NULL;}
					if(pMesh -> m_texname[3])
						{delete[] pMesh -> m_texname[3]; pMesh -> m_texname[3] = NULL;}
					plen = strlen(pDlg.m_mesh_base[m_meshs].patch[0]);
					if(plen)
					{
						pMesh -> m_texname[2] = new char[plen + 1];
						strcpy(pMesh -> m_texname[2], pDlg.m_mesh_base[m_meshs].patch[0]);
					}
					plen = strlen(pDlg.m_mesh_base[m_meshs].patch[1]);
					if(plen)
					{
						pMesh -> m_texname[3] = new char[plen + 1];
						strcpy(pMesh -> m_texname[3], pDlg.m_mesh_base[m_meshs].patch[1]);
					}
					pMesh = pMesh -> next;
					m_meshs++;
				}
				tPly -> m_vflags = pDlg.m_vflags;
				if(!tPly -> WriteFile(text_buff))
				{
					#ifdef ALTERNATIVE_LANG
						MessageBox("TOverwriting the file failed. Sadness!", "ERROR: CBoneData::OnBnClickedBoneWrench()", MB_ICONHAND);
					#else
						MessageBox("Ïåğåçàïèñü ôàéëà íå óäàëàñü. Ïå÷àëüêà!", "ERROR: CBoneData::OnBnClickedBoneWrench()", MB_ICONHAND);
					#endif
				}
				forced_ply_upd = true;
			}
		}
		delete tPly;
		tPly = NULL;
	}
}
