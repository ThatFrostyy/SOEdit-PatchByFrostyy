// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "SOEdit.h"

#include "MainFrm.h"
#include "SOEditDoc.h"
#include "CBoneData.h"
#include "CAnimData.h"
#include "CVolProp.h"
#include "VolPropDlg.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char szFileName[_MAX_PATH];
extern HWND m_hWnd;

/////////////////////////////////////////////////////////////////////////////
// CSOEditDoc

IMPLEMENT_DYNCREATE(CSOEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CSOEditDoc, CDocument)
	//{{AFX_MSG_MAP(CSOEditDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_AL, OnFileOpen)
	ON_COMMAND(ID_ANM_LOAD, OnAnmLoad)
	ON_COMMAND(ID_ANM_LOAD_AL, OnAnmLoad)
	ON_COMMAND(IDM_VOL_ADD, OnVolAdd)
	ON_COMMAND(IDM_VOL_ADD_AL, OnVolAdd)
	ON_COMMAND(IDM_BONE_ADD, OnBoneAdd)
	ON_COMMAND(IDM_BONE_ADD_AL, OnBoneAdd)
	ON_COMMAND(IDM_BONE_PROP, OnBoneProp)
	ON_COMMAND(IDM_BONE_PROP_AL, OnBoneProp)
	ON_COMMAND(ID_ANM_EDIT, OnAnmEdit)
	ON_COMMAND(ID_ANM_EDIT_AL, OnAnmEdit)
	ON_COMMAND(IDM_VOL_PROP, OnVolProp)
	ON_COMMAND(IDM_VOL_PROP_AL, OnVolProp)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_AS_AL, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AL, OnFileSave)
	ON_COMMAND(IDM_BONE_INSERT, OnBoneInsert)
	ON_COMMAND(IDM_BONE_INSERT_AL, OnBoneInsert)
	ON_COMMAND(IDM_BONE_DELETE, OnBoneDelete)
	ON_COMMAND(IDM_BONE_DELETE_AL, OnBoneDelete)
	ON_COMMAND(IDM_VOL_DELETE, OnVolDelete)
	ON_COMMAND(IDM_VOL_DELETE_AL, OnVolDelete)
	ON_COMMAND(BONE_MATRIX_COPY, On_Bone_Matrix_Copy)
	ON_COMMAND(BONE_MATRIX_COPY_AL, On_Bone_Matrix_Copy)
	ON_COMMAND(BONE_PARAMETERS_COPY, On_Bone_Parameters_Copy)
	ON_COMMAND(BONE_PARAMETERS_COPY_AL, On_Bone_Parameters_Copy)
	ON_COMMAND(BONE_MATRIX_PASTE, On_Bone_Matrix_Paste)
	ON_COMMAND(BONE_MATRIX_PASTE_AL, On_Bone_Matrix_Paste)
	ON_COMMAND(BONE_PARAMETERS_PASTE, On_Bone_Parameters_Paste)
	ON_COMMAND(BONE_PARAMETERS_PASTE_AL, On_Bone_Parameters_Paste)
	ON_COMMAND(VOLUME_MATRIX_COPY, On_Volume_Matrix_Copy)
	ON_COMMAND(VOLUME_MATRIX_COPY_AL, On_Volume_Matrix_Copy)
	ON_COMMAND(VOLUME_MATRIX_PASTE, On_Volume_Matrix_Paste)
	ON_COMMAND(VOLUME_MATRIX_PASTE_AL, On_Volume_Matrix_Paste)
	ON_COMMAND(ID_BONE_ADD_VOLUME, On_Bone_Add_Volume)
	ON_COMMAND(ID_BONE_ADD_VOLUME_AL, On_Bone_Add_Volume)
	ON_COMMAND(ID_BONE_DELETE_PLY, On_Bone_Delete_Mesh)
	ON_COMMAND(ID_BONE_DELETE_PLY_AL, On_Bone_Delete_Mesh)
	ON_COMMAND(ID_BONE_MERGE_PARENT, &CSOEditDoc::OnBoneMergeToParent)
	ON_COMMAND(ID_BONE_MERGE_PARENT_AL, &CSOEditDoc::OnBoneMergeToParent)
	ON_COMMAND(ID_BONE_EXPAND_ALL, On_Bone_Expand)
	ON_COMMAND(ID_BONE_EXPAND_ALL_AL, On_Bone_Expand)
	ON_COMMAND(ID_BONE_EXPAND_BRANCH, On_Bone_Expand)
	ON_COMMAND(ID_BONE_EXPAND_BRANCH_AL, On_Bone_Expand)
	ON_COMMAND(ID_BONE_COLLAPSE_BRANCH, On_Bone_Collapse)
	ON_COMMAND(ID_BONE_COLLAPSE_BRANCH_AL, On_Bone_Collapse)
	ON_COMMAND(ID_BONE_COPY, On_Bone_Copy)
	ON_COMMAND(ID_BONE_COPY_AL, On_Bone_Copy)
	ON_COMMAND(ID_BONE_COPY_BRANCH, On_Bone_Copy_Branch)
	ON_COMMAND(ID_BONE_COPY_BRANCH_AL, On_Bone_Copy_Branch)
	ON_COMMAND(ID_BONE_PASTE_AS_NEXT, On_Bone_Paste_As_Next)
	ON_COMMAND(ID_BONE_PASTE_AS_NEXT_AL, On_Bone_Paste_As_Next)
	ON_COMMAND(ID_BONE_PASTE_AS_INTERNAL, On_Bone_Paste_As_Interal)
	ON_COMMAND(ID_BONE_PASTE_AS_INTERNAL_AL, On_Bone_Paste_As_Interal)
	ON_COMMAND(ID_BONE_CUT, On_Bone_Cut)
	ON_COMMAND(IDM_BONE_PARENT, On_Bone_Parent)
	ON_COMMAND(IDM_BONE_PARENT_AL, On_Bone_Parent)
	ON_COMMAND(ID_EDIT_COPY, On_Bone_Copy_Branch)
	ON_COMMAND(ID_EDIT_COPY_AL, On_Bone_Copy_Branch)
	ON_COMMAND(ID_EDIT_PASTE, On_Bone_Paste_As_Next)
	ON_COMMAND(ID_EDIT_PASTE_AL, On_Bone_Paste_As_Next)
	ON_COMMAND(ID_EDIT_CUT, On_Bone_Cut)
	ON_COMMAND(ID_EDIT_CUT_AL, On_Bone_Cut)
	ON_COMMAND(ID_BONE_SKELETON_ANIMATIONS, &CSOEditDoc::OnBoneSkeletonAnimations)
	ON_COMMAND(ID_BONE_SKELETON_ANIMATIONS_AL, &CSOEditDoc::OnBoneSkeletonAnimations)
	ON_COMMAND(ID_FILE_NEW, &CSOEditDoc::OnFileNew)
	ON_COMMAND(ID_FILE_NEW_AL, &CSOEditDoc::OnFileNew)
	ON_COMMAND(ID_PLY_MERGE, &CSOEditDoc::OnPlyMergeFiles)
	ON_COMMAND(ID_PLY_MERGE_AL, &CSOEditDoc::OnPlyMergeFiles)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ANM_NEW, &CSOEditDoc::OnAnmNew)
	ON_COMMAND(ID_ANM_NEW_AL, &CSOEditDoc::OnAnmNew)
	ON_BN_CLICKED(IDC_ANMBOX_INHERIT_MATRIX, &CSOEditDoc::OnBnClickedAnmboxInheritMatrix)
	ON_COMMAND(ID_RESET_CAMERAS, &CSOEditDoc::OnResetCameras)
	ON_COMMAND(ID_RESET_CAMERAS_AL, &CSOEditDoc::OnResetCameras)
	ON_EN_CHANGE(IDC_FINDING_COMPONENT, &CSOEditDoc::OnEnChangeFindingComponent)
	ON_COMMAND(ID_BONE_ADD_LOD, &CSOEditDoc::OnBoneAddLod)
	ON_COMMAND(ID_BONE_ADD_LOD_AL, &CSOEditDoc::OnBoneAddLod)
	ON_COMMAND(ID_BONE_HIDE_BRANCH, &CSOEditDoc::OnBoneHideBranch)
	ON_COMMAND(ID_BONE_HIDE_BRANCH_AL, &CSOEditDoc::OnBoneHideBranch)
	ON_COMMAND(ID_BONE_HIDE_COLLISION, &CSOEditDoc::OnBoneHideCollision)
	ON_COMMAND(ID_BONE_HIDE_COLLISION_AL, &CSOEditDoc::OnBoneHideCollision)
	ON_COMMAND(ID_BONE_SHOW_BRANCH, &CSOEditDoc::OnBoneShowBranch)
	ON_COMMAND(ID_BONE_SHOW_BRANCH_AL, &CSOEditDoc::OnBoneShowBranch)
	ON_COMMAND(ID_BONE_SHOW_COLLISION, &CSOEditDoc::OnBoneShowCollision)
	ON_COMMAND(ID_BONE_SHOW_COLLISION_AL, &CSOEditDoc::OnBoneShowCollision)
	ON_COMMAND(ID_BONE_SHOW_BRANCH_AND_COLLISION, &CSOEditDoc::OnBoneShowBranchAndCollision)
	ON_COMMAND(ID_BONE_SHOW_BRANCH_AND_COLLISION_AL, &CSOEditDoc::OnBoneShowBranchAndCollision)
	ON_COMMAND(ID_BONE_HIDE_BRANCH_AND_COLLISION, &CSOEditDoc::OnBoneHideBranchAndCollision)
	ON_COMMAND(ID_BONE_HIDE_BRANCH_AND_COLLISION_AL, &CSOEditDoc::OnBoneHideBranchAndCollision)
	ON_COMMAND(ID_ANM_SAVE, &CSOEditDoc::OnAnmSave)
	ON_COMMAND(ID_ANM_SAVE_AL, &CSOEditDoc::OnAnmSave)
	ON_COMMAND(ID_ANM_CLOSE, &CSOEditDoc::OnAnmClose)
	ON_COMMAND(ID_ANM_CLOSE_AL, &CSOEditDoc::OnAnmClose)
	ON_COMMAND(ID_ADD_BONE_TO_ANM, &CSOEditDoc::OnAddBoneToAnm)
	ON_COMMAND(ID_DEL_BONE_FROM_ANM, &CSOEditDoc::OnDelBoneFromAnm)
	ON_COMMAND(ID_ADD_BONE_TO_ANM_AL, &CSOEditDoc::OnAddBoneToAnm)
	ON_COMMAND(ID_DEL_BONE_FROM_ANM_AL, &CSOEditDoc::OnDelBoneFromAnm)
	ON_BN_CLICKED(IDC_ANMBOX_DELETE_FRAME, &CSOEditDoc::OnBnClickedAnmboxDeleteFrame)
	ON_BN_CLICKED(IDC_ANMBOX_INSERT_FRAME, &CSOEditDoc::OnBnClickedAnmboxInsertFrame)
	ON_BN_CLICKED(IDC_REVERSE_ANIM, &CSOEditDoc::OnReverseAnimation)
	ON_BN_CLICKED(IDC_ANM_LERP_APPLY, &CSOEditDoc::OnBnClickedAnmLerpApply)
	ON_BN_CLICKED(IDC_ANM_AUTO, &CSOEditDoc::OnBnClickedAnmboxAuto)
	ON_BN_CLICKED(IDC_ANMBOX_DROP_FIRST_FRAME, &CSOEditDoc::OnBnClickedAnmboxDropFirstFrame)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSOEditDoc construction/destruction

CSOEditDoc::CSOEditDoc()
{
	m_Model = NULL;
	m_AnimBone = NULL;
	m_SelBone = m_SelVolume = m_SelObstacle = m_SelArea = NULL;
	m_SelObstacle = m_SelVolume = m_SelArea = m_SelBone = NULL;
	m_VolProp = NULL;
	m_pVolume = NULL;
	BLamp_View = NULL;

	model[0] = -90.0f;
	model[1] = 0.0f;
	model[2] = -90.0f;

	m_Frame = 0;
	m_Animate = false;
	m_Speed = 1.0f;
	m_Loop = false;

	m_VolProp = NULL;

	m_Center[0] = 0.0f;
	m_Center[1] = 0.0f;
	m_Center[2] = -150.0f;
	memset(texName, 0, sizeof(unsigned int) * 256);
	memset(tmptexName, 0, sizeof(unsigned int) * 256);
	m_texCount = 0;
	NameList = NULL;
	hSelTreeItem = NULL;
	memset(ViewWorks, 0, sizeof(int) * 4);
	old_frm = 0;
	auto_animation = m_skin = Colorized_3d = CuteMode = GeneralVolumesOpened = false;
	m_DropFirstFrameOnSave = false;
	m_NewModel = TexOff = VolToBone = Lod_writer = fix_mx_ori = false;
	InheritMatrix = AnmEditingMod = Anm_and_skeleton_editing_prohibited = false;
	VolToBone_Name = _T("");
	CopyParameters = _T("");
	Code_block = _T("");
	Paste_Mode = Copy_Mode = 0;
	memset((void*)&CopyMatrix, 0, sizeof(matrix34_t));
	CopyMatrix.v[0][0] = 1;
	CopyMatrix.v[1][1] = 1;
	CopyMatrix.v[2][2] = 1;
	GeneralVolumes = GeneralBones = NULL;
	memset(szFileName, 0, _MAX_PATH);
	m_ViewMod = vm3_visual;
	ResetCamView[0] = ResetCamView[1] = ResetCamView[2] = ResetCamView[3] = false;
	trylamp = false;
	add_lod_mode = false;
	memset((void*)&Animation_Automation, 0, sizeof(anm_tasks));
	m_GridSize = 5;
	m_grid = true;
	m_lamp = true;
	m_old_author = m_old_author_ply = m_old_author_vol = m_old_date = m_old_file = "";
}

CSOEditDoc::~CSOEditDoc()
{
	if (m_Model)
	{
		delete m_Model;
		m_Model = NULL;
	}
	if (m_AnimBone)
	{
		delete m_AnimBone;
		m_AnimBone = NULL;
	}
	if (m_SelBone)
	{
		delete[] m_SelBone;
		m_SelBone = NULL;
	}
	if (m_SelVolume)
	{
		delete[] m_SelVolume;
		m_SelVolume = NULL;
	}
	if (m_SelObstacle)
	{
		delete[] m_SelObstacle;
		m_SelObstacle = NULL;
	}
	if (m_SelArea)
	{
		delete[] m_SelArea;
		m_SelArea = NULL;
	}
	if (m_VolProp)
	{
		delete m_VolProp;
		m_VolProp = NULL;
	}
	if (BLamp_View)
	{
		delete BLamp_View;
		BLamp_View = NULL;
	}
}

void CSOEditDoc::OnFileNew()
{
	OnClearAll();
}

void CSOEditDoc::OnClearAll(bool question)
{
	if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist && question)
	{
#ifdef ALTERNATIVE_LANG
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "You confirm clearing the scene?", "QUESTION: CSOEditDoc::OnClearAll", MB_YESNO | MB_ICONQUESTION) != IDYES)
#else
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Âû ïîäòâåðæäàåòå î÷èñòêó ñöåíû?", "QUESTION: CSOEditDoc::OnClearAll", MB_YESNO | MB_ICONQUESTION) != IDYES)
#endif
		{
			return;
		}
	}
	if (m_Model)
	{
		delete m_Model;
		m_Model = NULL;
	}
	if (m_AnimBone)
	{
		delete m_AnimBone;
		m_AnimBone = NULL;
	}
	if (m_SelBone)
	{
		delete[] m_SelBone;
		m_SelBone = NULL;
	}
	if (m_SelVolume)
	{
		delete[] m_SelVolume;
		m_SelVolume = NULL;
	}
	if (m_SelObstacle)
	{
		delete[] m_SelObstacle;
		m_SelObstacle = NULL;
	}
	if (m_SelArea)
	{
		delete[] m_SelArea;
		m_SelArea = NULL;
	}
	if (m_VolProp)
	{
		delete m_VolProp;
		m_VolProp = NULL;
	}
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	pTreeCtrl->DeleteAllItems();
#ifdef ALTERNATIVE_LANG
	hTreeItem = pTreeCtrl->InsertItem("Skeleton", TVI_ROOT, NULL);
#else
	hTreeItem = pTreeCtrl->InsertItem("Ñêåëåò", TVI_ROOT, NULL);
#endif
	GeneralBones = hTreeItem;
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_BONETOP);
#ifdef ALTERNATIVE_LANG
	hTreeItem = pTreeCtrl->InsertItem("Volumes", TVI_ROOT, NULL);
#else
	hTreeItem = pTreeCtrl->InsertItem("Âîëþìû", TVI_ROOT, NULL);
#endif
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_VOLUMETOP);
	GeneralVolumes = hTreeItem;
#ifdef ALTERNATIVE_LANG
	hTreeItem = pTreeCtrl->InsertItem("Obstacles", TVI_ROOT, NULL);
#else
	hTreeItem = pTreeCtrl->InsertItem("Ïðåïÿòñòâèÿ", TVI_ROOT, NULL);
#endif
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_OBSTACLETOP);
#ifdef ALTERNATIVE_LANG
	hTreeItem = pTreeCtrl->InsertItem("Areas", TVI_ROOT, NULL);
#else
	hTreeItem = pTreeCtrl->InsertItem("Îáëàñòè", TVI_ROOT, NULL);
#endif
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_AREATOP);
	m_Model = new CModel(NULL);
	NameList = NULL;
	hSelTreeItem = NULL;
	memset(szFileName, 0, _MAX_PATH);
	memset(pFrameWnd->m_CurMdlPath, 0, _MAX_PATH);
	CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
	pWnd->SetWindowText("0");
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
	pWnd->SetWindowText("0");
	CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
	pSpinner->SetRange(1, 1);
	pSpinner->SetPos(1);
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);
	pWnd->SetWindowText("1.0");
	CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
	pSlider->SetPos(0);
	pSlider->SetRange(0, 0, true);
	pWnd = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_NAME);
	pWnd->SetWindowText("");
	ANM_Tool(false);
	Anm_and_skeleton_editing_prohibited = false;
	CMenu* pMenu = pFrameWnd->GetMenu();
	CMenu* pPopup = pMenu->GetSubMenu(3);
	ASSERT(pPopup != NULL);
#ifdef ALTERNATIVE_LANG
	pPopup->EnableMenuItem(ID_ANM_SAVE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#else
	pPopup->EnableMenuItem(ID_ANM_SAVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#endif
#ifdef ALTERNATIVE_LANG
	SetTitle("Untitled");
#else
	SetTitle("Áåç íàçâàíèÿ");
#endif
	if (!BLamp_View && !trylamp)
	{
		trylamp = true;
		char* c = NULL, LampFile[MAX_PATH];
		GetModuleFileNameA(NULL, LampFile, MAX_PATH);
		c = strrchr(LampFile, '\\');
		if (c)
		{
			c++;
			*c = '\0';
			strcat(LampFile, "lamp.ply");
			FILE* FPlyTest = NULL;
			FPlyTest = fopen(LampFile, "rb");
			if (FPlyTest)
			{
				fclose(FPlyTest);
				Lamp_View = new CPly(LampFile);
				if (!Lamp_View->loading_successes)
				{
					delete Lamp_View;
					Lamp_View = NULL;
				}
				else
				{
					BLamp_View = new CBone();
					BLamp_View->m_VolumeView = Lamp_View;
					BLamp_View->m_bone_color = 0xFF10E1F8;
				}
			}
		}
	}
	m_old_author = m_old_author_ply = m_old_author_vol = m_old_date = m_old_file = "";
	CEdit* pEdit = (CEdit*)pFrameWnd->m_wndToolTab.GetDlgItem(IDC_FINDING_COMPONENT);
	pEdit->SetWindowText("");
	UpdateAllViews(NULL, 0, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditDoc serialization

void CSOEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())// TODO: add storing code here
	{
	}
	else
	{
		m_Model = new CModel((char*)((LPCTSTR)GetPathName()));
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditDoc diagnostics

#ifdef _DEBUG
void CSOEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSOEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSOEditDoc commands

void RemoveQuotes(char* s)
{
	if (!s)
	{
		return;
	}
	int i = strlen(s);
	if (s[0] == '"')
	{
		strncpy(s, &s[1], i - 1);
		s[i - 1] = '\0';
	}
}

void CSOEditDoc::ProcessBone(CBone* basis, HTREEITEM hParent, bool select)
{
	CBone* child;
	CMaterial* pMaterial;
	HTREEITEM   hTreeItem;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	RemoveQuotes(basis->m_Name);
	hTreeItem = pTreeCtrl->InsertItem(basis->m_Name, hParent, NULL);
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	basis->hTreeItem = hTreeItem;
	pTreeCtrl->SetItemData(hTreeItem, IT_BONE);
	if (select)
	{
		pTreeCtrl->Select(hTreeItem, TVGN_CARET);
	}
	if (basis->m_VolumeView)
	{
		if (!basis->m_bone_colorized && !basis->m_start_colorized)
		{
			basis->m_bone_color = Bone_Set_Random_Color();
			basis->m_start_colorized = true;
		}
		if (basis->m_VolumeView->m_bbox[0][1] > m_MaxY)
		{
			m_MaxY = basis->m_VolumeView->m_bbox[0][1];
		}
		if (basis->m_VolumeView->m_bbox[1][1] > m_MaxY)
		{
			m_MaxY = basis->m_VolumeView->m_bbox[1][1];
		}
		if (basis->m_VolumeView->m_bbox[0][1] < m_MinY)
		{
			m_MinY = basis->m_VolumeView->m_bbox[0][1];
		}
		if (basis->m_VolumeView->m_bbox[1][1] < m_MinY)
		{
			m_MinY = basis->m_VolumeView->m_bbox[1][1];
		}
		if (basis->m_VolumeView->m_meshlist)
		{
			CMesh* pMesh = basis->m_VolumeView->m_meshlist->GetFirst();
			while (NULL != pMesh)
			{
				for (int i = 0; i < pMesh->m_texcount; i++)
				{
					if (pMesh->m_texture[i]->m_FrameName)
					{
						if ((pMaterial = m_MtlList.FindMaterial(pMesh->m_texture[i]->m_FrameName)) == NULL)
						{
							m_MtlList.AddToTail(pMesh->m_texture[i]->m_FrameName, -1);
						}
					}
					else
					{
						char msgText[512];
#ifdef ALTERNATIVE_LANG
						sprintf(msgText, "Bone: %s\nThe mesh has no material.\nPLY: %s", basis->m_Name, basis->m_VolumeViewName);
#else
						sprintf(msgText, "Êîñòü: %s\nÌýø íå èìååò ìàòåðèàëà.\nPLY: %s", basis->m_Name, basis->m_VolumeViewName);
#endif
						MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, msgText, "ERROR: CSOEditDoc::Processbone", MB_ICONHAND);
					}
				}
				pMesh = pMesh->next;
			}
		}
	}
	if (basis->next)
	{
		ProcessBone(basis->next, hParent, select);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		ProcessBone(child, hTreeItem, select);
	}
}

void CSOEditDoc::ProcessSkin(CBone* basis)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CBone* child;
	CBone* skin;
	RemoveQuotes(basis->m_Name);
	if (!basis->m_VolumeView)
	{
		skin = m_Model->m_skeleton->m_bonelist->FindBone("skin");
		if (skin)
		{
			if (skin->m_VolumeView && skin->m_VolumeView->SKINNED && false)//îòêëþ÷èë ñåïàðàöèþ ìýøåé
			{
				if (skin->m_VolumeView->m_bones)
				{
					m_skin = true;
				}
				for (int bone = 0; bone < skin->m_VolumeView->m_bones; bone++)
				{
					if (!stricmp(skin->m_VolumeView->m_bonelist[bone], basis->m_Name))
					{
						int vcount = 0;
						for (int v = 0; v < skin->m_VolumeView->m_numverts; v++)
						{
							if (((skin->m_VolumeView->m_vertlist[v].bones[0] - 1) == bone))//  ||
								// ((skin->m_VolumeView->m_vertlist[v].bones[1]-1) == bone))
							{
								vcount++;
							}
						}
						if (vcount)
						{
							if (basis->m_Skin)
							{
								if (basis->m_Skin->m_vertlist)
								{
									delete[] basis->m_Skin->m_vertlist;
									basis->m_Skin->m_vertlist = NULL;
								}
								delete basis->m_Skin;
								basis->m_Skin = NULL;
							}
							basis->m_Skin = new CSkin();
							basis->m_Skin->m_verts = vcount;
							basis->m_Skin->m_vertlist = new int[vcount];
							//char  errmsg[1024];
							//sprintf(errmsg, "Found %d vertices for bone %s in skin", vcount, basis->m_Name);
							//MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, errmsg, "CSOEditDoc::ProcessSkin", MB_ICONHAND);
						}
						else
						{
							continue;
						}
						vcount = 0;
						for (int v = 0; v < skin->m_VolumeView->m_numverts; v++)
						{
							if (((skin->m_VolumeView->m_vertlist[v].bones[0] - 1) == bone))//  ||
								//  ((skin->m_VolumeView->m_vertlist[v].bones[1]-1) == bone))
							{
								basis->m_Skin->m_vertlist[vcount] = v;
								vcount++;
							}
						}
						int fcount = 0;
						for (int f = 0; f < skin->m_VolumeView->m_numpolys; f++)
						{
							for (int v = 0; v < basis->m_Skin->m_verts; v++)
							{
								if ((basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[0])
									|| (basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[1])
									|| (basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[2]))
								{
									fcount++;
									break;
								}
							}
						}
						if (fcount)
						{
							if (basis->m_Skin->m_facelist)
							{
								delete[] basis->m_Skin->m_facelist;
								basis->m_Skin->m_facelist = NULL;
							}
							basis->m_Skin->m_faces = fcount;
							basis->m_Skin->m_facelist = new int[fcount];
							//char  errmsg[1024];
							//sprintf(errmsg, "Found %d faces for bone %s in skin", fcount, basis->m_Name);
							//MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, errmsg, "CSOEditDoc::ProcessSkin", MB_ICONHAND);
						}
						else
						{
							continue;
						}
						fcount = 0;
						for (int f = 0; f < skin->m_VolumeView->m_numpolys; f++)
						{
							for (int v = 0; v < basis->m_Skin->m_verts; v++)
							{
								if ((basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[0])
									|| (basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[1])
									|| (basis->m_Skin->m_vertlist[v] == skin->m_VolumeView->m_polylist[f].v[2]))
								{
									basis->m_Skin->m_facelist[fcount] = f;
									fcount++;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (basis->next)
	{
		ProcessSkin(basis->next);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		ProcessSkin(child);
	}
}

void CSOEditDoc::ResetBone(CBone* basis)
{
	CBone* child;
	if (basis->m_Animatrix)
	{
		delete[] basis->m_Animatrix;
		basis->m_Animatrix = NULL;
	}
	if (basis->next)
	{
		ProcessSkin(basis->next);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		ProcessSkin(child);
	}
}

/*void CSOEditDoc::ResetBones()
	{}*/

void CSOEditDoc::ProcessVolume(CVolume* volume, HTREEITEM hParent, bool select)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	RemoveQuotes(volume->m_Name);
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	hTreeItem = pTreeCtrl->InsertItem(volume->m_Name, hParent, NULL);
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	volume->hTreeItem = hTreeItem;
	pTreeCtrl->SetItemData(hTreeItem, IT_VOLUME);
	if (select)
	{
		pTreeCtrl->Select(hTreeItem, TVGN_CARET);
	}
}

void CSOEditDoc::ProcessObstacle(CObb2* obstacle, HTREEITEM hParent, bool select)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	RemoveQuotes(obstacle->m_Name);
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	hTreeItem = pTreeCtrl->InsertItem(obstacle->m_Name, hParent, NULL);
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_OBSTACLE);
	if (select)
	{
		pTreeCtrl->Select(hTreeItem, TVGN_CARET);
	}
}

void CSOEditDoc::ProcessArea(CObb2* area, HTREEITEM hParent, bool select)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	RemoveQuotes(area->m_Name);
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM   hTreeItem;
	hTreeItem = pTreeCtrl->InsertItem(area->m_Name, hParent, NULL);
	pTreeCtrl->SetCheck(hTreeItem, TRUE);
	pTreeCtrl->SetItemData(hTreeItem, IT_AREA);
	if (select)
	{
		pTreeCtrl->Select(hTreeItem, TVGN_CARET);
	}
}

void CSOEditDoc::LoadTextures()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	if (m_Model->m_skeleton)
	{
		if (m_Model->m_skeleton->m_bonelist)
		{
#ifdef ALTERNATIVE_LANG
			hTreeItem = pTreeCtrl->InsertItem("Skeleton", TVI_ROOT, NULL);
#else
			hTreeItem = pTreeCtrl->InsertItem("Ñêåëåò", TVI_ROOT, NULL);
#endif
			pTreeCtrl->SetCheck(hTreeItem, TRUE);
			GeneralBones = hTreeItem;
			pTreeCtrl->SetItemData(hTreeItem, IT_BONETOP);
			m_MinY = 99999.0f;
			m_MaxY = -99999.0f;
			m_Center[1] = m_MaxY - ((m_MaxY - m_MinY) * 0.5f);
			ProcessBone(m_Model->m_skeleton->m_bonelist, hTreeItem, false);
			//ProcessSkin(m_Model -> m_skeleton -> m_bonelist);
		}
	}
	if (m_Model->m_VolumeList)
	{
#ifdef ALTERNATIVE_LANG
		hTreeItem = pTreeCtrl->InsertItem("Volumes", TVI_ROOT, NULL);
#else
		hTreeItem = pTreeCtrl->InsertItem("Âîëþìû", TVI_ROOT, NULL);
#endif
		pTreeCtrl->SetCheck(hTreeItem, TRUE);
		GeneralVolumes = hTreeItem;
		pTreeCtrl->SetItemData(hTreeItem, IT_VOLUMETOP);
		CVolume* volume = m_Model->m_VolumeList->m_head;
		while (volume)
		{
			ProcessVolume(volume, hTreeItem, false);
			volume = volume->next;
		}
	}
	if (m_Model->m_ObstacleList)
	{
#ifdef ALTERNATIVE_LANG
		hTreeItem = pTreeCtrl->InsertItem("Obstacles", TVI_ROOT, NULL);
#else
		hTreeItem = pTreeCtrl->InsertItem("Ïðåïÿòñòâèÿ", TVI_ROOT, NULL);
#endif
		pTreeCtrl->SetCheck(hTreeItem, TRUE);
		pTreeCtrl->SetItemData(hTreeItem, IT_OBSTACLETOP);
		CObb2* obstacle = m_Model->m_ObstacleList->m_head;
		while (obstacle)
		{
			ProcessObstacle(obstacle, hTreeItem, false);
			obstacle = obstacle->next;
		}
	}
	if (m_Model->m_AreaList)
	{
#ifdef ALTERNATIVE_LANG
		hTreeItem = pTreeCtrl->InsertItem("Areas", TVI_ROOT, NULL);
#else
		hTreeItem = pTreeCtrl->InsertItem("Îáëàñòè", TVI_ROOT, NULL);
#endif
		pTreeCtrl->SetCheck(hTreeItem, TRUE);
		pTreeCtrl->SetItemData(hTreeItem, IT_AREATOP);
		CObb2* area = m_Model->m_AreaList->m_head;
		while (area)
		{
			ProcessArea(area, hTreeItem, false);
			area = area->next;
		}
	}
}

void CSOEditDoc::OnFileOpen()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
#ifdef ALTERNATIVE_LANG
	static char szFilter[] = "MDL-file (*.mdl)|*.mdl|";
	static char szTitle[] = "Opening the model file";
#else
	static char szFilter[] = "MDL-Ôàéë (*.mdl)|*.mdl|";
	static char szTitle[] = "Îòêðûòèå ôàéëà ìîäåëè";
#endif
	bool p = false;
	char CurFolder[_MAX_PATH] = { 0 };
	if (strlen(pFrameWnd->m_CurMdlPath))
	{
		p = true;
		strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
		char* ptr = strrchr(CurFolder, '/') + 1;
		*ptr = 0;
		InvertFixPathDelim(CurFolder);
	}
	CFileDialog dlg(TRUE, "*.mdl", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if (dlg.DoModal() == IDOK)
	{
		strcpy(szFileName, dlg.m_ofn.lpstrFile);
		FixPathDelim(szFileName);
		if (TexOff)
		{
			memcpy(texName, tmptexName, sizeof(unsigned int) * 256);
		}
		TexOff = false;
		Colorized_3d = false;
		for (int i = 0; i < m_texCount; i++)
		{
			glDeleteTextures(1, &texName[i]);
		}
		m_texCount = 0;
		memset(texName, 0, sizeof(unsigned int) * 256);
		memset(tmptexName, 0, sizeof(unsigned int) * 256);
		if (m_Model)
		{
			delete m_Model;
			m_Model = NULL;
		}
		if (m_AnimBone)
		{
			delete m_AnimBone;
			m_AnimBone = NULL;
		}
		if (m_SelBone)
		{
			delete[] m_SelBone;
			m_SelBone = NULL;
		}
		if (m_SelVolume)
		{
			delete[] m_SelVolume;
			m_SelVolume = NULL;
		}
		if (m_SelObstacle)
		{
			delete[] m_SelObstacle;
			m_SelObstacle = NULL;
		}
		if (m_SelArea)
		{
			delete[] m_SelArea;
			m_SelArea = NULL;
		}
		if (m_VolProp)
		{
			delete m_VolProp;
			m_VolProp = NULL;
		}
		CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
		pWnd->SetWindowText("0");
		pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
		pWnd->SetWindowText("0");
		CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
		pSpinner->SetRange(1, 1);
		pSpinner->SetPos(1);
		pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);
		pWnd->SetWindowText("1.0");
		CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
		pSlider->SetPos(0);
		pSlider->SetRange(0, 0, true);
		pWnd = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_NAME);
		pWnd->SetWindowText("");
		ANM_Tool(false);
		Anm_and_skeleton_editing_prohibited = false;
		CMenu* pMenu = pFrameWnd->GetMenu();
		CMenu* pPopup = pMenu->GetSubMenu(3);
		ASSERT(pPopup != NULL);
#ifdef ALTERNATIVE_LANG
		pPopup->EnableMenuItem(ID_ANM_SAVE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopup->EnableMenuItem(ID_ANM_CLOSE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#else
		pPopup->EnableMenuItem(ID_ANM_SAVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pPopup->EnableMenuItem(ID_ANM_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#endif
		m_skin = false;
		fix_mx_ori = false;
		m_pVolume = NULL;
		m_MtlList.ClearList();
		hSelTreeItem = NULL;
		memset(pFrameWnd->m_CurMdlPath, 0, _MAX_PATH);
		strcpy(pFrameWnd->m_CurMdlPath, szFileName);
		m_Model = new CModel(szFileName);
		NameList = NULL;
		int BonePoly = 0, VolumePoly = 0;
		VolumePolyCounter(VolumePoly);
		if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
		{
			BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
			NameCountAndLinkControl();
			VolumesConnections();
			OnModelCopyrightsParser();
		}
		CEdit* pEdit = (CEdit*)pFrameWnd->m_wndToolTab.GetDlgItem(IDC_FINDING_COMPONENT);
		pEdit->SetWindowText("");
		CString TitleBuff;
		TitleBuff.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
		SetTitle(TitleBuff);
		m_NewModel = true;
		CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
		pTreeCtrl->DeleteAllItems();
		LoadTextures();
		CRect rect;
		pFrameWnd->GetClientRect(&rect);
		pFrameWnd->InvalidateRect(&rect, false);
	}
}

void CSOEditDoc::OnLoad()
{
	OnAnmLoad();
}

void CSOEditDoc::OnAnmLoad()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	if (!strlen(pFrameWnd->m_CurMdlPath))
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "The model is not loaded!\nIf the model is made from scratch, then save and load it.", "ERROR: CSOEditDoc::OnAnmLoad", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Ìîäåëü íå çàãðóæåíà!\nÅñëè ìîäåëü äåëàåòñÿ ñ íóëÿ, òî ñîõðàíè è çàãðóçè å¸.", "ERROR: CSOEditDoc::OnAnmLoad", MB_ICONHAND);
#endif
		m_Animate = false;
		return;
	}
#ifdef ALTERNATIVE_LANG
	static char szFilter[] = "ANM-file (*.anm)|*.anm|";
	static char szTitle[] = "Opening an animation file";
#else
	static char szFilter[] = "ANM-ôàéë (*.anm)|*.anm|";
	static char szTitle[] = "Îòêðûòèå ôàéëà àíèìàöèè";
#endif
	bool p = false;
	char CurFolder[_MAX_PATH] = { 0 };
	if (strlen(pFrameWnd->m_CurMdlPath))
	{
		p = true;
		strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
		char* ptr = strrchr(CurFolder, '/') + 1;
		*ptr = 0;
		InvertFixPathDelim(CurFolder);
	}
	CFileDialog dlg(TRUE, "*.anm", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if (dlg.DoModal() == IDOK)
	{
		m_Animate = false;
		m_Frame = 0;
		old_frm = 0;
		strcpy(szAnimPath, dlg.m_ofn.lpstrFile);
		FixPathDelim(szAnimPath);
		if (m_AnimBone)
		{
			delete m_AnimBone;
			m_AnimBone = NULL;
		}
		bool b = false;
		m_AnimBone = new CAnimBone(szAnimPath, b);
		if (!b)
		{
			delete m_AnimBone;
			m_AnimBone = NULL;
			return;
		}
		ANM_Tool(true);
		CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
		char szWindowText[_MAX_PATH];
		CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
		sprintf(szWindowText, "%d", m_Frame + 1);
		pWnd->SetWindowText(szWindowText);
		pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
		sprintf(szWindowText, "%d", m_AnimBone->m_FrameCnt);
		pWnd->SetWindowText(szWindowText);
		CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
		pSpinner->SetRange(1, m_AnimBone->m_FrameCnt);
		pSpinner->SetPos(1);
		pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);
		pWnd->SetWindowText("1.0");
		CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
		pSlider->SetPos(m_Frame);
		pSlider->SetRange(0, m_AnimBone->m_FrameCnt - 1, true);
		pWnd = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_NAME);
		pWnd->SetWindowText(szAnimPath);
		CMenu* pMenu = pFrameWnd->GetMenu();
		CMenu* pPopup = pMenu->GetSubMenu(3);
		ASSERT(pPopup != NULL);
#ifdef ALTERNATIVE_LANG
		pPopup->EnableMenuItem(ID_ANM_SAVE_AL, MF_BYCOMMAND | MF_ENABLED);
		pPopup->EnableMenuItem(ID_ANM_CLOSE_AL, MF_BYCOMMAND | MF_ENABLED);
#else
		pPopup->EnableMenuItem(ID_ANM_SAVE, MF_BYCOMMAND | MF_ENABLED);
		pPopup->EnableMenuItem(ID_ANM_CLOSE, MF_BYCOMMAND | MF_ENABLED);
#endif
		UpdateAllViews(NULL, 0, NULL);
	}
}

void CSOEditDoc::OnVolAdd()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	if (m_Model->m_VolumeList)
	{
		if (m_Model->m_VolumeList->m_sdl)
		{
			m_Model->m_VolumeList->m_sdl = NULL;
		}
	}
	else
	{
		m_Model->m_VolumeList = new CVolumeList();
	}
	m_Model->m_VolumeList->AddToTail();
	CVolume* pVolume = m_Model->m_VolumeList->GetLast();
	CVolume* pFindVolume = NULL;
	char tmpVolName[_MAX_PATH] = { 0 };
	for (int i = 0; i < 100; i++)
	{
		memset(tmpVolName, 0, _MAX_PATH);
		sprintf(tmpVolName, "volume%d", i);
		pFindVolume = m_Model->m_VolumeList->FindByName(tmpVolName);
		if (!pFindVolume)
		{
			break;
		}
	}
	pVolume->m_Name = new char[strlen(tmpVolName) + 1];
	strcpy(pVolume->m_Name, tmpVolName);
	char szVolPath[_MAX_PATH];
	int yesno = IDNO;
#ifdef ALTERNATIVE_LANG
	if (IDYES == (yesno = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Add a volume from a file (VOL)?", "QUESTION: CSOEditDoc::OnVolAdd", MB_YESNO | MB_ICONQUESTION)))
#else
	if (IDYES == (yesno = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Äîáàâèòü âîëþì èç ôàéëà (VOL)?", "QUESTION: CSOEditDoc::OnVolAdd", MB_YESNO | MB_ICONQUESTION)))
#endif
	{
#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "VOL-file (*.vol)|*.vol||";
		static char szTitle[] = "Opening a volume-file";
#else
		static char szFilter[] = "VOL-ôàéë (*.vol)|*.vol||";
		static char szTitle[] = "Îòêðûòèå âîëþì-ôàéëà";
#endif
		bool p = false;
		char CurFolder[_MAX_PATH] = { 0 };
		if (strlen(pFrameWnd->m_CurMdlPath))
		{
			p = true;
			strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
			char* ptr = strrchr(CurFolder, '/') + 1;
			*ptr = 0;
			InvertFixPathDelim(CurFolder);
		}
		CFileDialog dlg(TRUE, "*.vol", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
		dlg.m_ofn.lpstrTitle = szTitle;
		if (dlg.DoModal() == IDOK)
		{
			strcpy(szVolPath, dlg.m_ofn.lpstrFile);
			FixPathDelim(szVolPath);
			pVolume->m_PolyhedronName = new char[strlen(szVolPath) + 1];
			strcpy(pVolume->m_PolyhedronName, szVolPath);
			char tstr[_MAX_PATH];
			strcpy(tstr, strrchr(pVolume->m_PolyhedronName, '/'));
			char* pdot = strrchr(tstr, '.');
			if (pdot)
			{
				*pdot = '\0';
			}
			delete[] pVolume->m_Name;
			pFindVolume = NULL;
			pFindVolume = m_Model->m_VolumeList->FindByName(tstr + 1);
			if (pFindVolume)
			{
				pFindVolume = NULL;
				for (int i = 0; i < 100; i++)
				{
					memset(tmpVolName, 0, _MAX_PATH);
					sprintf(tmpVolName, "%s%d", tstr + 1, i);
					pFindVolume = m_Model->m_VolumeList->FindByName(tmpVolName);
					if (!pFindVolume)
					{
						break;
					}
				}
			}
			else
			{
				memset(tmpVolName, 0, _MAX_PATH);
				strcpy(tmpVolName, tstr + 1);
			}
			int len = strlen(tmpVolName) + 2;
			pVolume->m_Name = new char[len];
			memset(pVolume->m_Name, 0, len);
			strcpy(pVolume->m_Name, tmpVolName);
			pVolume->m_Type = VT_PLY;
			pVolume->m_Vol = NULL;
			ProcessVolume(pVolume, hTreeItem, true);
		}
		else
		{
			HTREEITEM hNewItem = pTreeCtrl->InsertItem(pVolume->m_Name, hTreeItem, NULL);
			pTreeCtrl->SetCheck(hNewItem, TRUE);
			pTreeCtrl->SetItemData(hNewItem, IT_VOLUME);
			pTreeCtrl->Select(hNewItem, TVGN_CARET);
			hSelTreeItem = hNewItem;
			pVolume->hTreeItem = hNewItem;
		}
	}
	else
	{
		HTREEITEM hNewItem = pTreeCtrl->InsertItem(pVolume->m_Name, hTreeItem, NULL);
		pTreeCtrl->SetCheck(hNewItem, TRUE);
		pTreeCtrl->SetItemData(hNewItem, IT_VOLUME);
		pTreeCtrl->Select(hNewItem, TVGN_CARET);
		hSelTreeItem = hNewItem;
		pVolume->hTreeItem = hNewItem;
	}
	if (VolToBone)
	{
		pVolume->m_Bone = new char[VolToBone_Name.GetLength() + 1];
		memset(pVolume->m_Bone, 0, VolToBone_Name.GetLength() + 1);
		strcpy(pVolume->m_Bone, VolToBone_Name.GetBuffer(1));
	}
	OnVolProp();
}

void CSOEditDoc::OnBoneInsert()
{
	OnBoneAdd();
}

void CSOEditDoc::OnBoneAdd()
{
	if (!m_Model)
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "The model is not loaded.", "ERROR: CSOEditDoc::OnBoneAdd", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Ìîäåëü íå çàãðóæåíà.", "ERROR: CSOEditDoc::OnBoneAdd", MB_ICONHAND);
#endif
		m_Animate = false;
		return;
	}
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	CBone* pBone = NULL;
	char szPlyPath[_MAX_PATH];
	CBone* pFindBone = NULL;
	char tmpBoneName[_MAX_PATH] = { 0 };
	bool basis = false;
	int len = 0;
	pBone = new CBone();
	if (m_Model->m_skeleton == NULL)
	{
		m_Model->m_skeleton = new CSkeleton();
		m_Model->m_skeleton->m_bonelist = pBone;
		if (pBone->m_Name)
		{
			delete[] pBone->m_Name;
			pBone->m_Name = NULL;
		}
		len = strlen("basis") + 1;
		pBone->m_Name = new char[len];
		strcpy(pBone->m_Name, "basis");
		basis = true;
	}
	else
	{
		for (int i = 0; i < 100; i++)
		{
			memset(tmpBoneName, 0, _MAX_PATH);
			sprintf(tmpBoneName, "bone%d", i);
			pFindBone = m_Model->m_skeleton->m_bonelist->FindBone(tmpBoneName);
			if (!pFindBone)
			{
				break;
			}
		}
		if (pBone->m_Name)
		{
			delete[] pBone->m_Name;
			pBone->m_Name = NULL;
		}
		len = strlen(tmpBoneName) + 1;
		pBone->m_Name = new char[len];
		strcpy(pBone->m_Name, tmpBoneName);
	}
	DWORD data = pTreeCtrl->GetItemData(hTreeItem);
	// If this is the top most bone -- There is no parent
	if (data != IT_BONETOP)
	{
		CBone* pParentBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
		pBone->m_parent = pParentBone;
		if (pParentBone)
		{
			CBone* tBone = pParentBone->GetLast();
			if (tBone)
			{
				tBone->next = pBone;
				pBone->prev = tBone;
			}
			else
			{
				pParentBone->m_ChildFirst = pBone;
			}
			pParentBone->m_ChildLast = pBone;
		}
	}
	else
		if (!basis)
		{
			pFindBone = m_Model->m_skeleton->m_bonelist;
			while (pFindBone->next)
			{
				pFindBone = pFindBone->next;
			}
			pFindBone->next = pBone;
			pBone->prev = pFindBone;
		}
	int yesno = IDYES;
#ifdef ALTERNATIVE_LANG
	if ((yesno = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Add a mesh from a file (PLY)?", "QUESTION: CSOEditDoc::OnBoneAdd", MB_YESNO | MB_ICONQUESTION)) == IDYES)
#else
	if ((yesno = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Äîáàâèòü ìýø èç ôàéëà (PLY)?", "QUESTION: CSOEditDoc::OnBoneAdd", MB_YESNO | MB_ICONQUESTION)) == IDYES)
#endif
	{
#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "PLY-file (*.ply)|*.ply||";
		static char szTitle[] = "Opening a mesh-file";
#else
		static char szFilter[] = "PLY-ôàéë (*.ply)|*.ply||";
		static char szTitle[] = "Îòêðûòèå ìýø-ôàéëà";
#endif
		bool p = false;
		char CurFolder[_MAX_PATH] = { 0 };
		if (strlen(pFrameWnd->m_CurMdlPath))
		{
			p = true;
			strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
			char* ptr = strrchr(CurFolder, '/') + 1;
			*ptr = 0;
			InvertFixPathDelim(CurFolder);
		}
		CFileDialog dlg(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
		dlg.m_ofn.lpstrTitle = szTitle;
		if (dlg.DoModal() == IDOK)
		{
			strcpy(szPlyPath, dlg.m_ofn.lpstrFile);
			FixPathDelim(szPlyPath);
			pBone->m_VolumeViewName = new char[strlen(szPlyPath) + 1];
			strcpy(pBone->m_VolumeViewName, szPlyPath);
			char tstr[_MAX_PATH];
			strcpy(tstr, strrchr(pBone->m_VolumeViewName, '/'));
			char* pdot = strrchr(tstr, '.');
			if (pdot)
			{
				*pdot = '\0';
			}
			delete[] pBone->m_Name;
			pFindBone = NULL;
			pFindBone = m_Model->m_skeleton->m_bonelist->FindBone(tstr + 1);
			if (pFindBone)
			{
				pFindBone = NULL;
				for (int i = 0; i < 100; i++)
				{
					memset(tmpBoneName, 0, _MAX_PATH);
					sprintf(tmpBoneName, "%s%d", tstr + 1, i);
					pFindBone = m_Model->m_skeleton->m_bonelist->FindBone(tmpBoneName);
					if (!pFindBone)
					{
						break;
					}
				}
			}
			else
			{
				memset(tmpBoneName, 0, _MAX_PATH);
				strcpy(tmpBoneName, tstr + 1);
			}
			len = strlen(tmpBoneName) + 1;
			pBone->m_Name = new char[len];
			memset(pBone->m_Name, 0, len);
			strcpy(pBone->m_Name, tmpBoneName);
			pBone->m_VolumeView = new CPly(pBone->m_VolumeViewName);
			if (!pBone->m_VolumeView->loading_successes)
			{
				delete pBone->m_VolumeView;
				pBone->m_VolumeView = NULL;
			}
			ProcessBone(pBone, hTreeItem, true);
			int BonePoly = 0, VolumePoly = 0;
			VolumePolyCounter(VolumePoly);
			if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
			{
				BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
			}
			CString TitleBuff;
			TitleBuff.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
			SetTitle(TitleBuff);
		}
		else
		{
			HTREEITEM hNewItem = pTreeCtrl->InsertItem(pBone->m_Name, hTreeItem, NULL);
			pTreeCtrl->SetCheck(hNewItem, TRUE);
			pTreeCtrl->SetItemData(hNewItem, IT_BONE);
			pTreeCtrl->Select(hNewItem, TVGN_CARET);
			hSelTreeItem = hNewItem;
			pBone->hTreeItem = hNewItem;
		}
	}
	else
	{
		HTREEITEM hNewItem = pTreeCtrl->InsertItem(pBone->m_Name, hTreeItem, NULL);
		pTreeCtrl->SetCheck(hNewItem, TRUE);
		pTreeCtrl->SetItemData(hNewItem, IT_BONE);
		pTreeCtrl->Select(hNewItem, TVGN_CARET);
		hSelTreeItem = hNewItem;
		pBone->hTreeItem = hNewItem;
	}
	OnBoneProp();
}

void CSOEditDoc::OnBoneDelete()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	char szBoneName[_MAX_PATH] = { 0 };
	bool SubNumRewrite = false;
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();

	if (!hTreeItem || !m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist)
		return;

	strcpy(szBoneName, pTreeCtrl->GetItemText(hTreeItem));
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);

	if (pBone)
	{
#ifdef ALTERNATIVE_LANG
		if (CuteMode || (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Really remove this bone?", "QUESTION", MB_YESNO | MB_ICONQUESTION) == IDYES))
#else
		if (CuteMode || (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Точно удалить эту кость?", "QUESTION", MB_YESNO | MB_ICONQUESTION) == IDYES))
#endif
		{
			// 1. ANALYSIS & NOTIFICATION UI
			if (m_Model->m_skeleton->m_bonelist && !CuteMode)
			{
				CString tmpStr;
				bool binding = false;
#ifdef ALTERNATIVE_LANG
				tmpStr = "This bone / branch of bones has linked volumes:\r\n";
#else
				tmpStr = "У этой кости / ветви костей есть волюмная привязка:\r\n";
#endif
				AnalysisOfBoneBranchesByVolume(pBone, tmpStr, binding, false);
#ifdef ALTERNATIVE_LANG
				tmpStr += "\r\nDelete linked volumes?";
#else
				tmpStr += "\r\nУдалить привязанные волюмы?";
#endif
				if (binding && MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, tmpStr, "QUESTION", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					DeleteBindingVolumes(pBone, false);
				}
			}

			// 2. DATA CAPTURE (CRITICAL FIX: Capture BEFORE deletion)
			SubNumRewrite = (pBone->sub > 0);
			CBone* pParent = pBone->m_parent;
			CBone* Bnext = pBone->next;
			CBone* Bprev = pBone->prev;
			HTREEITEM hFallbackParent = pTreeCtrl->GetParentItem(hTreeItem); // Capture UI parent now

			// Disable Tree Redraw to prevent re-entrant selection events
			pTreeCtrl->SetRedraw(FALSE);

			// 3. RECURSIVE DELETE CHILDREN
			if (pBone->m_ChildFirst)
			{
				ChildBoneDelete(pBone, false);
				pBone->m_ChildFirst = NULL;
				pBone->m_ChildLast = NULL;
			}

			// 4. LINKED LIST MAINTENANCE
			if (pParent && pParent->m_ChildFirst == pBone)
			{
				pParent->m_ChildFirst = Bnext;
			}
			if (pParent && pParent->m_ChildLast == pBone)
			{
				pParent->m_ChildLast = Bprev;
			}

			HTREEITEM HSelBone = NULL;
			if (!Bnext)
			{
				if (Bprev)
				{
					HSelBone = Bprev->hTreeItem;
				}
				else if (pParent)
				{
					HSelBone = pParent->hTreeItem;
				}
			}
			else
			{
				HSelBone = Bnext->hTreeItem;
			}

			// 5. ACTUAL MEMORY DELETION
			if (!pParent && pBone == m_Model->m_skeleton->m_bonelist)
			{
				if (Bnext)
				{
					Bnext->prev = NULL;
					m_Model->m_skeleton->m_bonelist = Bnext;
				}
				delete pBone;
				pBone = NULL;
			}
			else
			{
				if (Bnext) { Bnext->prev = Bprev; }
				if (Bprev) { Bprev->next = Bnext; }
				delete pBone;
				pBone = NULL;
			}

			// Cleanup skeleton if empty
			if (!pParent && !Bnext && !Bprev)
			{
				m_Model->m_skeleton->m_bonelist = NULL;
				delete m_Model->m_skeleton;
				m_Model->m_skeleton = NULL;
			}

			// 6. UI UPDATE
			pTreeCtrl->DeleteItem(hTreeItem);
			pTreeCtrl->SetRedraw(TRUE); // Turn redraw back on

			if (HSelBone)
			{
				pTreeCtrl->SelectItem(HSelBone);
			}
			else if (hFallbackParent)
			{
				pTreeCtrl->Select(hFallbackParent, TVGN_CARET);
			}

			// 7. SUB-MESH/LOD RENAMING LOGIC
			if (pParent && SubNumRewrite)
			{
				CBone* child = pParent->m_ChildFirst;
				int NewSubNum = 1;
				CString NewName;
				while (child && child->m_LODView && child->sub)
				{
					child->sub = NewSubNum;
#ifdef ALTERNATIVE_LANG
					NewName.Format("%s (%s-%d)", pParent->m_Name, (child->m_LODView) ? "Lod" : "Sub. mesh", NewSubNum);
#else
					NewName.Format("%s (%s-%d)", pParent->m_Name, (child->m_LODView) ? "Лод" : "Суб. мэш", NewSubNum);
#endif
					if (child->m_Name) { delete[] child->m_Name; }
					child->m_Name = new char[NewName.GetLength() + 1];
					strcpy(child->m_Name, NewName);
					pTreeCtrl->SetItemText(child->hTreeItem, NewName);
					child = child->next;
					NewSubNum++;
				}
			}

			// 8. FINAL REFRESH
			int BonePoly = 0, VolumePoly = 0;
			VolumePolyCounter(VolumePoly);
			if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
			{
				BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
			}

			char TitleBuff[512];
			sprintf(TitleBuff, "%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
			SetTitle(TitleBuff);
		}
	}
	VolumesConnections();
	pTreeCtrl->SetFocus();
	pTreeCtrl->Invalidate();
}

void CSOEditDoc::AnalysisOfBoneBranchesByVolume(CBone* basis, CString& tmpStr, bool& binding, bool next)
{
	CBone* child = NULL;
	CVolume* pVol = m_Model->m_VolumeList->GetFirst();
	while (pVol)
	{
		if (pVol->boneptr == basis)
		{
#ifdef ALTERNATIVE_LANG
			tmpStr += "Volume \"" + CString(pVol->m_Name) + "\" linked to bone \"" + CString(basis->m_Name) + "\"\r\n";
#else
			tmpStr += "Âîëþì \"" + CString(pVol->m_Name) + "\" ïðèâÿçàí ê êîñòè \"" + CString(basis->m_Name) + "\"\r\n";
#endif
			binding = true;
			//break;
		}
		pVol = pVol->next;
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		AnalysisOfBoneBranchesByVolume(child, tmpStr, binding, true);
	}
	if (basis->next && next)
	{
		AnalysisOfBoneBranchesByVolume(basis->next, tmpStr, binding, true);
	}
}

void CSOEditDoc::DeleteBindingVolumes(CBone* basis, bool next)
{
	HTREEITEM hTreeItem = NULL;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	CBone* child = NULL;
	CVolume* pVol = m_Model->m_VolumeList->GetFirst();
	char szVolumeName[_MAX_PATH];
	while (pVol)
	{
		if (pVol->boneptr == basis)
		{
			hTreeItem = pVol->hTreeItem;
			strcpy(szVolumeName, pTreeCtrl->GetItemText(hTreeItem));
			m_Model->m_VolumeList->DropVolume(szVolumeName);
			pTreeCtrl->DeleteItem(hTreeItem);
			pVol = m_Model->m_VolumeList->GetFirst();
			//break;
		}
		else
		{
			pVol = pVol->next;
		}
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		DeleteBindingVolumes(child, true);
	}
	if (basis->next && next)
	{
		DeleteBindingVolumes(basis->next, true);
	}
}

void CSOEditDoc::ChildBoneDelete(CBone* curbone, bool ChildrenDelete)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (!curbone)
	{
		return;
	}
	CBone* child = NULL;
	if ((child = curbone->m_ChildFirst) != NULL)
	{
		ChildBoneDelete(child, true);
	}
	if (curbone->next && ChildrenDelete)
	{
		ChildBoneDelete(curbone->next, true);
	}
	if (curbone && ChildrenDelete)
	{
		pTreeCtrl->DeleteItem(curbone->hTreeItem);
		delete curbone;
		curbone = NULL;
	}
}

void CSOEditDoc::OnBoneProp()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	char szBoneName[_MAX_PATH];
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	strcpy(szBoneName, pTreeCtrl->GetItemText(hTreeItem));
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	CSequenceList* m_SequenceList = NULL;
	if (pBone)
	{
		CBoneData bonedata;
		if (pBone->sub)
		{
			bonedata.WindowMode = CBoneData::TSubBone;
		}
		else
		{
			if (pBone->m_LODView)
			{
				CBone* child = pBone->m_ChildFirst;
				if (child)
				{
					if (child->sub && child->m_LODView)
					{
						bonedata.has_sub = true;
					}
				}
			}
		}
		HTREEITEM hParentItem = pTreeCtrl->GetParentItem(hTreeItem);
		if (IT_BONETOP != pTreeCtrl->GetItemData(hParentItem))
		{
			bonedata.m_ParentBone = pTreeCtrl->GetItemText(hParentItem);
		}
		bonedata.m_BoneName = pBone->m_Name;
		bonedata.m_Ground = pBone->m_Ground;
		bonedata.m_Visible = pBone->m_Visibility;
		bonedata.bone_color = pBone->m_bone_color;
		bonedata.bone_colorized = pBone->m_bone_colorized;
		if (!pBone->Matrix34)
		{
			pBone->Matrix34 = new matrix34_t;
			memset(pBone->Matrix34, 0, sizeof(matrix34_t));
			pBone->Matrix34->v[0][0] = 1.0f;
			pBone->Matrix34->v[1][1] = 1.0f;
			pBone->Matrix34->v[2][2] = 1.0f;
		}
		if (pBone->Matrix34)
		{
			bonedata.m_Matrix11 = pBone->Matrix34->v[0][0];
			bonedata.m_Matrix12 = pBone->Matrix34->v[0][1];
			bonedata.m_Matrix13 = pBone->Matrix34->v[0][2];
			bonedata.m_Matrix21 = pBone->Matrix34->v[1][0];
			bonedata.m_Matrix22 = pBone->Matrix34->v[1][1];
			bonedata.m_Matrix23 = pBone->Matrix34->v[1][2];
			bonedata.m_Matrix31 = pBone->Matrix34->v[2][0];
			bonedata.m_Matrix32 = pBone->Matrix34->v[2][1];
			bonedata.m_Matrix33 = pBone->Matrix34->v[2][2];
			bonedata.m_Matrix41 = pBone->Matrix34->v[3][0];
			bonedata.m_Matrix42 = pBone->Matrix34->v[3][1];
			bonedata.m_Matrix43 = pBone->Matrix34->v[3][2];
		}
		if (pBone->m_Parameters)
		{
			RemoveQuotes(pBone->m_Parameters);
			bonedata.m_Parameters = pBone->m_Parameters;
		}
		if (pBone->m_VolumeViewName)
		{
			bonedata.m_VolumeView = pBone->m_VolumeViewName;
		}
		bonedata.m_Terminator = pBone->m_Terminator;
		if (pBone->m_Revolute)
		{
			bonedata.RotateType = CBoneData::TRotateType::TRevolute;
		}
		else
			if (pBone->m_Prizmatic)
			{
				bonedata.RotateType = CBoneData::TRotateType::TPrizmatic;
			}
			else
				if (pBone->m_Socket)
				{
					bonedata.RotateType = CBoneData::TRotateType::TSocket;
				}
				else
				{
					bonedata.RotateType = CBoneData::TRotateType::TNone;
				}
		bonedata.HasLimits = pBone->HasLimits;
		if (pBone->m_LimitsType)
		{
			bonedata.m_LimitsLo = pBone->m_Limits[0];
			bonedata.m_LimitsHi = pBone->m_Limits[1];
		}
		else
		{
			bonedata.m_LimitsLo = 0.0f;
			bonedata.m_LimitsHi = 0.0f;
			bonedata.m_LimitsCommon = pBone->m_Limits[0];
		}
		bonedata.m_Speed = pBone->m_Speed;
		bonedata.m_Speed2 = pBone->m_Speed2;
		if (pBone->m_Animation && pBone->m_Animation->m_SequenceList)
		{
			m_SequenceList = new CSequenceList(pBone->m_Animation->m_SequenceList);
			bonedata.m_SequenceList = m_SequenceList;
		}
		bonedata.m_Ground = pBone->m_Ground;
		bonedata.m_NoCastShadows = pBone->m_NoCastShadows;
		bonedata.m_DecalTarget = pBone->m_DecalTarget;
		bonedata.m_NoGroupMesh = pBone->m_NoGroupMesh;
		bonedata.m_NoGetShadows = pBone->m_NoGetShadows;
		bonedata.m_LODView = pBone->m_LODView;
		bonedata.m_LOD_OFF = pBone->m_LOD_OFF;
		bonedata.m_Layer = pBone->m_Layer;

		//îòêðûòü îêíî
		int result = bonedata.DoModal();
		if (result == IDOK)
		{
			if (pBone->m_VolumeViewName)
			{
				delete[] pBone->m_VolumeViewName;
				pBone->m_VolumeViewName = NULL;
			}
			if (pBone->m_VolumeView)
			{
				delete pBone->m_VolumeView;
				pBone->m_VolumeView = NULL;
			}
		}
		if (!bonedata.m_VolumeView.IsEmpty() && (result == IDOK || bonedata.forced_ply_upd))
		{
			pBone->m_VolumeViewName = new char[bonedata.m_VolumeView.GetLength() + 1];
			strcpy(pBone->m_VolumeViewName, bonedata.m_VolumeView.GetBuffer(1));
			FILE* FPlyTest = NULL;
			FPlyTest = fopen(pBone->m_VolumeViewName, "rb");
			if (FPlyTest)
			{
				fclose(FPlyTest);
				pBone->m_VolumeView = new CPly(pBone->m_VolumeViewName);
				if (!pBone->m_VolumeView->loading_successes)
				{
					delete pBone->m_VolumeView;
					pBone->m_VolumeView = NULL;
				}
			}
			if (pBone->m_VolumeView && pBone->m_VolumeView->m_meshlist)
			{
				CMaterial* pMaterial;
				CMesh* pMesh = pBone->m_VolumeView->m_meshlist->GetFirst();
				while (pMesh)
				{
					for (int i = 0; i < pMesh->m_texcount; i++)
					{
						if (pMesh->m_texture[i]->m_FrameName)
						{
							if ((pMaterial = m_MtlList.FindMaterial(pMesh->m_texture[i]->m_FrameName)) == NULL)
							{
								m_MtlList.AddToTail(pMesh->m_texture[i]->m_FrameName, -1);
							}
						}
						else
						{
							CString msgText;
#ifdef ALTERNATIVE_LANG
							msgText.Format("Bone: %s\nThe mesh has no material.\nPLY: %s", pBone->m_Name, pBone->m_VolumeViewName);
#else
							msgText.Format("Êîñòü: %s\nÌýø íå èìååò ìàòåðèàëà.\nPLY: %s", pBone->m_Name, pBone->m_VolumeViewName);
#endif
							MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, msgText, "ERROR: CSOEditDoc::OnBoneProp", MB_ICONHAND);
						}
					}
					pMesh = pMesh->next;
				}
			}
			int BonePoly = 0, VolumePoly = 0;
			VolumePolyCounter(VolumePoly);
			if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
			{
				BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
			}
			CString msgText;
			msgText.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
			SetTitle(msgText);
		}
		if (result != IDOK)
		{
			delete bonedata.m_SequenceList;
			if (bonedata.forced_ply_upd)
			{
				UpdateAllViews(NULL, 0, NULL);
			}
			pTreeCtrl->SetFocus();
			return;
		}
		pBone->m_Ground = bonedata.m_Ground;
		pBone->m_NoCastShadows = bonedata.m_NoCastShadows;
		pBone->m_DecalTarget = bonedata.m_DecalTarget;
		pBone->m_NoGroupMesh = bonedata.m_NoGroupMesh;
		pBone->m_NoGetShadows = bonedata.m_NoGetShadows;
		pBone->m_LODView = bonedata.m_LODView;
		pBone->m_LOD_OFF = bonedata.m_LOD_OFF;
		pBone->m_Layer = bonedata.m_Layer;
		if (pBone->m_Animation && pBone->m_Animation->m_SequenceList)
		{
			delete pBone->m_Animation->m_SequenceList;
			pBone->m_Animation->m_SequenceList = NULL;
		}
		if (bonedata.m_SequenceList && !bonedata.m_SequenceList->m_head)
		{
			delete bonedata.m_SequenceList;
			bonedata.m_SequenceList = NULL;
		}
		if (bonedata.m_SequenceList)
		{
			if (!pBone->m_Animation)
			{
				pBone->m_Animation = new CAnimation();
			}
			pBone->m_Animation->m_SequenceList = bonedata.m_SequenceList;
		}
		else
		{
			if (pBone->m_Animation)
			{
				delete pBone->m_Animation;
				pBone->m_Animation = NULL;
			}
		}
		pBone->m_bone_colorized = bonedata.bone_colorized;
		pBone->m_bone_color = bonedata.bone_color;
		if (pBone->sub)
		{
			return;
		}
		delete[] pBone->m_Name;
		pBone->m_Name = NULL;
		int len = bonedata.m_BoneName.GetLength() + 1;
		pBone->m_Name = new char[len];
		memset(pBone->m_Name, 0, len);
		strcpy(pBone->m_Name, bonedata.m_BoneName.GetBuffer(1));
		pTreeCtrl->SetItemText(hTreeItem, pBone->m_Name);
		pBone->m_Speed = bonedata.m_Speed;
		pBone->m_LimitsType = true;
		pBone->m_Terminator = bonedata.m_Terminator;
		pBone->m_Speed2 = bonedata.m_Speed2;
		if (pBone->m_Speed2)//àíòè êðàø
		{
			if ((pBone->m_Speed > 0 && pBone->m_Speed < 0.573) || (pBone->m_Speed < 0 && pBone->m_Speed > -0.573))
			{
				pBone->m_Speed = 0;
			}
		}
		pBone->HasLimits = bonedata.HasLimits;
		pBone->m_Revolute = false;
		pBone->m_Prizmatic = false;
		pBone->m_Socket = false;
		switch (bonedata.RotateType)
		{
		case CBoneData::TRotateType::TRevolute:
		{ pBone->m_Revolute = true; }
		break;
		case CBoneData::TRotateType::TPrizmatic:
		{ pBone->m_Prizmatic = true; pBone->m_LimitsType = true; }
		break;
		case CBoneData::TRotateType::TSocket:
		{ pBone->m_Socket = true; pBone->m_LimitsType = false; }
		break;
		};
		if (pBone->m_Revolute || pBone->m_Prizmatic)
		{
			pBone->m_Limits[0] = bonedata.m_LimitsLo;
			pBone->m_Limits[1] = bonedata.m_LimitsHi;
		}
		else
			if (pBone->m_Socket)
			{
				pBone->m_Limits[0] = bonedata.m_LimitsCommon;
				pBone->m_Limits[1] = 0.0f;
			}
			else
			{
				pBone->m_Limits[0] = 0.0f;
				pBone->m_Limits[1] = 0.0f;
			}
		if (m_SelBone)
		{
			delete[] m_SelBone;
		}
		m_SelBone = NULL;
		len = strlen(pBone->m_Name) + 1;
		m_SelBone = new char[len];
		memset(m_SelBone, 0, len);
		strcpy(m_SelBone, pBone->m_Name);
		if ((bonedata.m_Matrix11 != 0.0f) || (bonedata.m_Matrix12 != 0.0f) || (bonedata.m_Matrix13 != 0.0f) \
			|| (bonedata.m_Matrix21 != 0.0f) || (bonedata.m_Matrix22 != 0.0f) || (bonedata.m_Matrix23 != 0.0f) \
			|| (bonedata.m_Matrix31 != 0.0f) || (bonedata.m_Matrix32 != 0.0f) || (bonedata.m_Matrix33 != 0.0f) \
			|| (bonedata.m_Matrix41 != 0.0f) || (bonedata.m_Matrix42 != 0.0f) || (bonedata.m_Matrix43 != 0.0f))
		{
			if (!pBone->Matrix34)
			{
				pBone->Matrix34 = new matrix34_t;
			}
			pBone->Matrix34->v[0][0] = bonedata.m_Matrix11;
			pBone->Matrix34->v[0][1] = bonedata.m_Matrix12;
			pBone->Matrix34->v[0][2] = bonedata.m_Matrix13;
			pBone->Matrix34->v[1][0] = bonedata.m_Matrix21;
			pBone->Matrix34->v[1][1] = bonedata.m_Matrix22;
			pBone->Matrix34->v[1][2] = bonedata.m_Matrix23;
			pBone->Matrix34->v[2][0] = bonedata.m_Matrix31;
			pBone->Matrix34->v[2][1] = bonedata.m_Matrix32;
			pBone->Matrix34->v[2][2] = bonedata.m_Matrix33;
			pBone->Matrix34->v[3][0] = bonedata.m_Matrix41;
			pBone->Matrix34->v[3][1] = bonedata.m_Matrix42;
			pBone->Matrix34->v[3][2] = bonedata.m_Matrix43;
		}
		else
		{
			if (pBone->Matrix34)
			{
				delete pBone->Matrix34; pBone->Matrix34 = NULL;
			}
		}
		pBone->m_Visibility = bonedata.m_Visible;
		if (bonedata.m_Ground == 1)
		{
			pBone->m_Ground = true;
		}
		else
		{
			pBone->m_Ground = false;
		}
		if (pBone->m_Parameters)
		{
			delete[] pBone->m_Parameters; pBone->m_Parameters = NULL;
		}
		if (!bonedata.m_Parameters.IsEmpty())
		{
			pBone->m_Parameters = new char[bonedata.m_Parameters.GetLength() + 1];
			strcpy(pBone->m_Parameters, bonedata.m_Parameters.GetBuffer(1));
		}
		VolumesConnections();
		CRect rect;
		pFrameWnd->GetClientRect(&rect);
		pFrameWnd->InvalidateRect(&rect, false);
	}
	else
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Bone: " + CString(szBoneName) + " not found.\nThe program memory is damaged!\nRestart!"), "ERROR: CSOEditDoc::OnBoneProp", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Êîñòü: " + CString(szBoneName) + " íå íàéäåíà.\nÏàìÿòü ïðîãðàììû ïîâðåæäåíà!\nÏåðåçàïóñòè!"), "ERROR: CSOEditDoc::OnBoneProp", MB_ICONHAND);
#endif
	}
	pTreeCtrl->SetFocus();
}

void CSOEditDoc::OnAnmEdit()
{
	if (!m_AnimBone)
	{
#ifdef ALTERNATIVE_LANG
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Animation is not loaded.\nUpload?", "QUESTION: CSOEditDoc::OnAnmEdit", MB_YESNO | MB_ICONQUESTION) == IDYES)
#else
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Àíèìàöèÿ íå çàãðóæåíà.\nÇàãðóçèòü?", "QUESTION: CSOEditDoc::OnAnmEdit", MB_YESNO | MB_ICONQUESTION) == IDYES)
#endif
		{
			OnAnmLoad();
		}
	}
	if (m_AnimBone)
	{
		CAnimData* pAnimData = new CAnimData;
		pAnimData->m_FileName = szAnimPath;
		pAnimData->m_Frame = m_Frame;
		pAnimData->m_FrameCnt = m_AnimBone->m_FrameCnt;
		pAnimData->m_ver = m_AnimBone->Header_ID;
		CAnimSub* pSub = m_AnimBone->m_Frames[m_Frame].m_head;
		if (pSub)
		{
			pAnimData->m_M1_1 = pSub->m_Matrix34.v[0][0];
			pAnimData->m_M1_2 = pSub->m_Matrix34.v[0][1];
			pAnimData->m_M1_3 = pSub->m_Matrix34.v[0][2];
			pAnimData->m_M2_1 = pSub->m_Matrix34.v[1][0];
			pAnimData->m_M2_2 = pSub->m_Matrix34.v[1][1];
			pAnimData->m_M2_3 = pSub->m_Matrix34.v[1][2];
			pAnimData->m_M3_1 = pSub->m_Matrix34.v[2][0];
			pAnimData->m_M3_2 = pSub->m_Matrix34.v[2][1];
			pAnimData->m_M3_3 = pSub->m_Matrix34.v[2][2];
			pAnimData->m_M4_1 = pSub->m_Matrix34.v[3][0];
			pAnimData->m_M4_2 = pSub->m_Matrix34.v[3][1];
			pAnimData->m_M4_3 = pSub->m_Matrix34.v[3][2];
			pAnimData->m_Visi = pSub->m_Visible;
		}
		pAnimData->m_FrameNo = m_Frame;
		pAnimData->DoModal();
		delete pAnimData;
		AnimBoxUPD();
		//------------------------
		//åñëè çàäàíèÿ íåò, òî è ïåðåìåííàÿ îáíóëÿåòñÿ
		//------------------------
	}
}

void CSOEditDoc::OnVolProp()
{
	char tpath[_MAX_PATH] = { 0 }, * delim = NULL, * delim2 = NULL;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	char szVolName[_MAX_PATH] = { 0 };
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	strcpy(szVolName, pTreeCtrl->GetItemText(hTreeItem));
	//MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, szVolName, "!!!", MB_OK);
	m_pVolume = m_Model->m_VolumeList->FindVolumeByTreeID(hSelTreeItem);
	if (m_pVolume)
	{
		if (m_VolProp)
		{
			delete m_VolProp;
			m_VolProp = NULL;
		}
		m_VolProp = new CVolProp;
		if (m_pVolume->m_Type == VT_BOX)
		{
			m_VolProp->m_BoxLength = m_pVolume->m_Box[0];
			m_VolProp->m_BoxWidth = m_pVolume->m_Box[1];
			m_VolProp->m_BoxHeight = m_pVolume->m_Box[2];
		}
		if (m_pVolume->m_Type == VT_CYLINDER)
		{
			m_VolProp->m_CylDiameter = m_pVolume->m_Cylinder[0];
			m_VolProp->m_CylLength = m_pVolume->m_Cylinder[1];
		}
		if (m_pVolume->m_Type == VT_PLY)
		{
			m_VolProp->m_PolyName = m_pVolume->m_PolyhedronName;
		}
		m_VolProp->m_VolType = m_pVolume->m_Type;
		m_VolProp->m_Name = m_pVolume->m_Name;
		m_VolProp->m_VolBone = m_pVolume->m_Bone;
		if (m_pVolume->m_Matrix34)
		{
			m_VolProp->m_Matrix1_1 = m_pVolume->m_Matrix34->v[0][0];
			m_VolProp->m_Matrix1_2 = m_pVolume->m_Matrix34->v[0][1];
			m_VolProp->m_Matrix1_3 = m_pVolume->m_Matrix34->v[0][2];
			m_VolProp->m_Matrix2_1 = m_pVolume->m_Matrix34->v[1][0];
			m_VolProp->m_Matrix2_2 = m_pVolume->m_Matrix34->v[1][1];
			m_VolProp->m_Matrix2_3 = m_pVolume->m_Matrix34->v[1][2];
			m_VolProp->m_Matrix3_1 = m_pVolume->m_Matrix34->v[2][0];
			m_VolProp->m_Matrix3_2 = m_pVolume->m_Matrix34->v[2][1];
			m_VolProp->m_Matrix3_3 = m_pVolume->m_Matrix34->v[2][2];
			m_VolProp->m_Matrix4_1 = m_pVolume->m_Matrix34->v[3][0];
			m_VolProp->m_Matrix4_2 = m_pVolume->m_Matrix34->v[3][1];
			m_VolProp->m_Matrix4_3 = m_pVolume->m_Matrix34->v[3][2];
		}
		if (m_VolProp->DoModal() != IDOK)
		{
			VolumesConnections();
			pTreeCtrl->SetFocus();
			return;
		}
		// Fixup the name of the collision volume
		delete[] m_pVolume->m_Name;
		m_pVolume->m_Name = NULL;
		int len = m_VolProp->m_Name.GetLength() + 1;
		m_pVolume->m_Name = new char[len];
		memset(m_pVolume->m_Name, 0, len);
		strcpy(m_pVolume->m_Name, m_VolProp->m_Name.GetBuffer(1));
		pTreeCtrl->SetItemText(hTreeItem, m_pVolume->m_Name);
		delete[] m_SelVolume;
		m_SelVolume = NULL;
		len = strlen(m_pVolume->m_Name) + 1;
		m_SelVolume = new char[len];
		memset(m_SelVolume, 0, len);
		strcpy(m_SelVolume, m_pVolume->m_Name);
		delete[] m_pVolume->m_Bone;
		m_pVolume->m_Bone = NULL;
		m_pVolume->m_Bone = new char[m_VolProp->m_VolBone.GetLength() + 1];
		memset(m_pVolume->m_Bone, 0, m_VolProp->m_VolBone.GetLength() + 1);
		strcpy(m_pVolume->m_Bone, m_VolProp->m_VolBone.GetBuffer(1));
		switch (m_pVolume->m_Type)
		{
		case VT_BOX:
			if (m_pVolume->m_Box)
			{
				delete[] m_pVolume->m_Box; m_pVolume->m_Box = NULL;
			}
			break;
		case VT_CYLINDER:
			if (m_pVolume->m_Cyl)
			{
				delete m_pVolume->m_Cyl; m_pVolume->m_Cyl = NULL;
			}
			if (m_pVolume->m_Cylinder)
			{
				delete[] m_pVolume->m_Cylinder; m_pVolume->m_Cylinder = NULL;
			}
			break;
		case VT_PLY:
			if (m_pVolume->m_Vol)
			{
				delete m_pVolume->m_Vol; m_pVolume->m_Vol = NULL;
			}
			if (m_pVolume->m_PolyhedronName)
			{
				delete[] m_pVolume->m_PolyhedronName; m_pVolume->m_PolyhedronName = NULL;
			}
			break;
		}
		m_pVolume->m_Type = m_VolProp->m_VolType;
		// Setup the new collision volume type
		switch (m_pVolume->m_Type)
		{
		case VT_BOX:
			m_pVolume->m_Box = new float[3];
			m_pVolume->m_Box[0] = m_VolProp->m_BoxLength;
			m_pVolume->m_Box[1] = m_VolProp->m_BoxWidth;
			m_pVolume->m_Box[2] = m_VolProp->m_BoxHeight;
			break;
		case VT_CYLINDER:
			m_pVolume->m_Cylinder = new float[2];
			m_pVolume->m_Cylinder[0] = m_VolProp->m_CylDiameter;
			m_pVolume->m_Cylinder[1] = m_VolProp->m_CylLength;
			m_pVolume->m_Cyl = new CCylinder(m_pVolume->m_Cylinder[0], m_pVolume->m_Cylinder[1]);
			break;
		case VT_PLY:
			// setup the VOL file name
			memset((char*)&tpath, 0, sizeof(tpath));
			strcpy(tpath, GetModelPath());
			if (!tpath[0])
			{
				strcat(tpath, m_VolProp->m_PolyName.GetBuffer(1));
			}
			else
			{
				strcpy(tpath, m_VolProp->m_PolyName.GetBuffer(1));
			}
			// store the file name in m_PolyhedronName
			delete[] m_pVolume->m_PolyhedronName;
			m_pVolume->m_PolyhedronName = NULL;
			m_pVolume->m_PolyhedronName = new char[strlen(tpath) + 1];
			strcpy(m_pVolume->m_PolyhedronName, tpath);
			delete m_pVolume->m_Vol;
			m_pVolume->m_Vol = NULL;
			FILE* FVolTest = NULL;
			FVolTest = fopen(m_pVolume->m_PolyhedronName, "rb");
			if (FVolTest)
			{
				fclose(FVolTest);
				m_pVolume->m_Vol = new CVol(m_pVolume->m_PolyhedronName);
				int BonePoly = 0, VolumePoly = 0;
				VolumePolyCounter(VolumePoly);
				if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
				{
					BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
				}
				CString TitleBuff;
				TitleBuff.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
				SetTitle(TitleBuff);
			}
			// create the collision volume
			break;
		}
		if ((m_VolProp->m_Matrix1_1 != 0.0f) || (m_VolProp->m_Matrix1_2 != 0.0f) || (m_VolProp->m_Matrix1_3 != 0.0f) \
			|| (m_VolProp->m_Matrix2_1 != 0.0f) || (m_VolProp->m_Matrix2_2 != 0.0f) || (m_VolProp->m_Matrix2_3 != 0.0f) \
			|| (m_VolProp->m_Matrix3_1 != 0.0f) || (m_VolProp->m_Matrix3_2 != 0.0f) || (m_VolProp->m_Matrix3_3 != 0.0f) \
			|| (m_VolProp->m_Matrix4_1 != 0.0f) || (m_VolProp->m_Matrix4_2 != 0.0f) || (m_VolProp->m_Matrix4_3 != 0.0f))
		{
			if (m_pVolume->m_Matrix34 == NULL)
			{
				m_pVolume->m_Matrix34 = new matrix34_t;
			}
			m_pVolume->m_Matrix34->v[0][0] = m_VolProp->m_Matrix1_1;
			m_pVolume->m_Matrix34->v[0][1] = m_VolProp->m_Matrix1_2;
			m_pVolume->m_Matrix34->v[0][2] = m_VolProp->m_Matrix1_3;
			m_pVolume->m_Matrix34->v[1][0] = m_VolProp->m_Matrix2_1;
			m_pVolume->m_Matrix34->v[1][1] = m_VolProp->m_Matrix2_2;
			m_pVolume->m_Matrix34->v[1][2] = m_VolProp->m_Matrix2_3;
			m_pVolume->m_Matrix34->v[2][0] = m_VolProp->m_Matrix3_1;
			m_pVolume->m_Matrix34->v[2][1] = m_VolProp->m_Matrix3_2;
			m_pVolume->m_Matrix34->v[2][2] = m_VolProp->m_Matrix3_3;
			m_pVolume->m_Matrix34->v[3][0] = m_VolProp->m_Matrix4_1;
			m_pVolume->m_Matrix34->v[3][1] = m_VolProp->m_Matrix4_2;
			m_pVolume->m_Matrix34->v[3][2] = m_VolProp->m_Matrix4_3;
		}
		else
		{
			if (m_pVolume->m_Matrix34 != NULL)
			{
				delete m_pVolume->m_Matrix34;
				m_pVolume->m_Matrix34 = NULL;
			}
		}
		UpdateAllViews(NULL, 0, NULL);
	}
	CRect rect;
	pFrameWnd->GetClientRect(&rect);
	pFrameWnd->InvalidateRect(&rect, false);
	VolumesConnections();
	pTreeCtrl->SetFocus();
}

void CSOEditDoc::OnModelSave(bool mod)
{
	if (m_Model)
	{
		CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
#ifdef ALTERNATIVE_LANG
		static char szFilter[] = "MDL-file (*.mdl)|*.mdl|";
		static char szTitle[] = "Save the model file as:";
#else
		static char szFilter[] = "MDL-Ôàéë (*.mdl)|*.mdl|";
		static char szTitle[] = "Ñîõðàíèòü ôàéë ìîäåëè êàê:";
#endif
		FILE* fp;
		bool p = false;
		char CurFolder[_MAX_PATH] = { 0 };
		if (strlen(pFrameWnd->m_CurMdlPath))
		{
			p = true;
			strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
			char* ptr = strrchr(CurFolder, '/') + 1;
			*ptr = 0;
			InvertFixPathDelim(CurFolder);
		}
		CFileDialog dlg(TRUE, "*.mdl", ((p) ? CurFolder : NULL), OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON, szFilter);
		dlg.m_ofn.lpstrTitle = szTitle;
		if (!strlen(pFrameWnd->m_CurMdlPath))
		{
			mod = true;
		}
		if (!mod)
		{
			if ((fp = fopen(pFrameWnd->m_CurMdlPath, "r")) != NULL)
			{
				fclose(fp);
			}
			else
			{
				mod = true;
			}
		}
		if (mod ? dlg.DoModal() == IDOK : true)
		{
			EnableWindow(pFrameWnd->m_hWnd, false);
			char path[_MAX_PATH] = { 0 };
			if (mod)
			{
				strcpy(szFileName, dlg.m_ofn.lpstrFile);
				FixPathDelim(szFileName);
				strcpy(pFrameWnd->m_CurMdlPath, szFileName);
			}
			else
			{
				strcpy(szFileName, pFrameWnd->m_CurMdlPath);
			}
			strcpy(path, pFrameWnd->m_CurMdlPath);
			if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
			{
				NameCountAndLinkControl();
			}
			bool saveit = true;
			if ((fp = fopen(szFileName, "r")) != NULL)
			{
				fclose(fp);
				int save = 0;
				if (mod)
				{
#ifdef ALTERNATIVE_LANG
					save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("File:\n" + CString(szFileName) + "\nalready exists. Replace?"), "WARNING: CSOEditDoc::OnModelSave", MB_YESNO | MB_ICONEXCLAMATION);
#else
					save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Ôàéë:\n" + CString(szFileName) + "\nóæå ñóùåñòâóåò. Çàìåíèòü?"), "WARNING: CSOEditDoc::OnModelSave", MB_YESNO | MB_ICONEXCLAMATION);
#endif
				}
				else
				{
#ifdef ALTERNATIVE_LANG
					save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Do you confirm overwriting the file?"), "QUESTION: CSOEditDoc::OnModelSave", MB_YESNO | MB_ICONQUESTION);
#else
					save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Âû ïîäòâåðæäàåòå ïåðåçàïèñü ôàéëà?"), "QUESTION: CSOEditDoc::OnModelSave", MB_YESNO | MB_ICONQUESTION);
#endif
				}
				if (save != IDYES)
				{
					saveit = false;
				}
			}
			if (saveit)
			{
				static char* day_of_the_week[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", 0 };
				static char* month_of_the_year[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", 0 };
				time_t current_time;
				struct tm local_time;
				time(&current_time);
				localtime_s(&local_time, &current_time);
				fp = fopen(szFileName, "w");
				fprintf(fp, ";MDL File generated by SOEdit Version Next (v0.%dm%d)\n", LOBYTE((pFrameWnd->ProgVer) >> 8), LOBYTE(pFrameWnd->ProgVer));
				if (m_old_author.GetLength() > 0)
				{
					fprintf(fp, ";Model created by: %s\n", m_old_author.GetBuffer(1));
				}
				if (m_old_date.GetLength() > 0)
				{
					fprintf(fp, ";Date: %s\n", m_old_date.GetBuffer(1));
				}
				if (m_old_file.GetLength() > 0)
				{
					fprintf(fp, ";File: %s\n", m_old_file.GetBuffer(1));
				}
				if (strlen(pFrameWnd->m_author))
				{
					fprintf(fp, ";Model create/rebuild by: %s\n", pFrameWnd->m_author);
				}
				else
				{
#ifdef ALTERNATIVE_LANG
					MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Mistake. Your nickname is not specified!\r\nGo to the program properties, write down your nickname and resave the file.", "ERROR: CSOEditDoc::OnFileSaveAs", MB_ICONHAND);
#else
					MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Îøèáêà. Òâîé íèêíåéì íå óêàçàí!\r\nÇàéäè â ñâîéñòâà ïðîãðàììû, çàïèøè ñâîé íèê è ïåðåñîõðàíè ôàéë.", "ERROR: CSOEditDoc::OnFileSaveAs", MB_ICONHAND);
#endif
					fprintf(fp, ";Model create/rebuild by: [Error: nickname is missing.]\n");
				}
				if (m_old_author_ply.GetLength() > 0)
				{
					fprintf(fp, ";PLY Geometry by: %s\n", m_old_author_ply.GetBuffer(1));
				}
				if (m_old_author_vol.GetLength() > 0)
				{
					fprintf(fp, ";VOL Geometry by: %s\n", m_old_author_vol.GetBuffer(1));
				}
				fprintf(fp, ";Last build date: %s %d %s %d [%s%d:%s%d:%s%d]\n", day_of_the_week[local_time.tm_wday], local_time.tm_mday, month_of_the_year[local_time.tm_mon], local_time.tm_year + 1900, (local_time.tm_hour < 10) ? "0" : "", local_time.tm_hour, (local_time.tm_min < 10) ? "0" : "", local_time.tm_min, (local_time.tm_sec < 10) ? "0" : "", local_time.tm_sec);
				m_Model->WriteMdl(fp, 0);
				fclose(fp);
			}
			if (strlen(path))
			{
				strcpy(pFrameWnd->m_CurMdlPath, path);
			}
			EnableWindow(pFrameWnd->m_hWnd, true);
		}
	}
}

void CSOEditDoc::OnFileSaveAs()
{
	OnModelSave(true);
}

void CSOEditDoc::OnFileSave()
{
	OnModelSave(false);
}

void CSOEditDoc::OnPlyMergeFiles()
{
#ifdef ALTERNATIVE_LANG
	static char szFilter[] = "PLY-file (*.ply)|*.ply||";
#else
	static char szFilter[] = "PLY-ôàéë (*.ply)|*.ply||";
#endif

	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	bool p = false;
	char CurFolder[_MAX_PATH] = { 0 };
	if (pFrameWnd && strlen(pFrameWnd->m_CurMdlPath))
	{
		p = true;
		strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
		char* ptr = strrchr(CurFolder, '/');
		if (ptr)
		{
			*(ptr + 1) = 0;
		}
		InvertFixPathDelim(CurFolder);
	}

#ifdef ALTERNATIVE_LANG
	CFileDialog dlgBase(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlgBase.m_ofn.lpstrTitle = "Select base mesh (PLY)";
#else
	CFileDialog dlgBase(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlgBase.m_ofn.lpstrTitle = "Âûáåðè áàçîâûé ìýø (PLY)";
#endif
	if (dlgBase.DoModal() != IDOK)
	{
		return;
	}

#ifdef ALTERNATIVE_LANG
	CFileDialog dlgAppend(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlgAppend.m_ofn.lpstrTitle = "Select mesh to append (PLY)";
#else
	CFileDialog dlgAppend(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlgAppend.m_ofn.lpstrTitle = "Âûáåðè äîáàâëÿåìûé ìýø (PLY)";
#endif
	if (dlgAppend.DoModal() != IDOK)
	{
		return;
	}

	char outPath[_MAX_PATH] = { 0 };
	strncpy(outPath, dlgBase.m_ofn.lpstrFile, _MAX_PATH - 1);
	outPath[_MAX_PATH - 1] = '\0';
	char* dot = strrchr(outPath, '.');
	if (dot)
	{
		*dot = 0;
	}
	if (strlen(outPath) + 12 < _MAX_PATH)
	{
		strcat(outPath, "_merged.ply");
	}

#ifdef ALTERNATIVE_LANG
	CFileDialog dlgOut(FALSE, "*.ply", outPath, OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NONETWORKBUTTON, szFilter);
	dlgOut.m_ofn.lpstrTitle = "Save merged mesh as";
#else
	CFileDialog dlgOut(FALSE, "*.ply", outPath, OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NONETWORKBUTTON, szFilter);
	dlgOut.m_ofn.lpstrTitle = "Ñîõðàíèòü îáúåäèíåííûé ìýø êàê";
#endif
	if (dlgOut.DoModal() != IDOK)
	{
		return;
	}

	char basePath[_MAX_PATH] = { 0 }, appendPath[_MAX_PATH] = { 0 }, mergedPath[_MAX_PATH] = { 0 };
	strcpy(basePath, dlgBase.m_ofn.lpstrFile);
	strcpy(appendPath, dlgAppend.m_ofn.lpstrFile);
	strcpy(mergedPath, dlgOut.m_ofn.lpstrFile);
	FixPathDelim(basePath);
	FixPathDelim(appendPath);
	FixPathDelim(mergedPath);

	CPly basePly(basePath);
	CPly appendPly(appendPath);
	if (!basePly.loading_successes || !appendPly.loading_successes)
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to load one of selected PLY files.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü çàãðóçèòü îäèí èç âûáðàííûõ PLY-ôàéëîâ.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#endif
		return;
	}

	if (!basePly.MergeKeepSeparateTextures(&appendPly))
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Merge failed. Make sure both PLY files use compatible vertex format.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Îáúåäèíåíèå íå óäàëîñü. Ïðîâåðü ñîâìåñòèìîñòü ôîðìàòà âåðøèí îáîèõ PLY.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#endif
		return;
	}

	if (!basePly.WriteFile(mergedPath))
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to save merged PLY file.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü ñîõðàíèòü îáúåäèíåííûé PLY-ôàéë.", "ERROR: CSOEditDoc::OnPlyMergeFiles", MB_ICONHAND);
#endif
		return;
	}

#ifdef ALTERNATIVE_LANG
	MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Merged file saved:\n" + CString(mergedPath)), "Done", MB_OK | MB_ICONINFORMATION);
#else
	MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Îáúåäèíåííûé ôàéë ñîõðàíåí:\n" + CString(mergedPath)), "Ãîòîâî", MB_OK | MB_ICONINFORMATION);
#endif
}

void CSOEditDoc::OnVolDelete()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	char szVolumeName[_MAX_PATH];
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	HTREEITEM HSelVol = NULL;
	strcpy(szVolumeName, pTreeCtrl->GetItemText(hTreeItem));
	CVolume* pVolume = m_Model->m_VolumeList->FindByName(szVolumeName);
	if (pVolume)
	{
#ifdef ALTERNATIVE_LANG
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Really delete this volume?", "QUESTION: CSOEditDoc::OnVolDelete", MB_YESNO | MB_ICONQUESTION) == IDYES)
#else
		if (MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Òî÷íî óäàëèòü ýòîò âîëþì?", "QUESTION: CSOEditDoc::OnVolDelete", MB_YESNO | MB_ICONQUESTION) == IDYES)
#endif
		{
			if (pVolume->next)
			{
				HSelVol = pVolume->next->hTreeItem;
			}
			else
				if (pVolume->prev)
				{
					HSelVol = pVolume->prev->hTreeItem;
				}
			m_Model->m_VolumeList->DropVolume(szVolumeName);
			if (HSelVol)
			{
				pTreeCtrl->SelectItem(HSelVol);
			}
			else
			{
				pTreeCtrl->Select(pTreeCtrl->GetParentItem(hTreeItem), TVGN_CARET);
			}
			pTreeCtrl->DeleteItem(hTreeItem);
			int BonePoly = 0, VolumePoly = 0;
			VolumePolyCounter(VolumePoly);
			if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
			{
				BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
			}
			CString TitleBuff;
			TitleBuff.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
			SetTitle(TitleBuff);
		}
	}
	VolumesConnections();
	pTreeCtrl->SetFocus();
}

void CSOEditDoc::BonePolyCounter(CBone* basis, int& CouPoly)
{
	CBone* child = NULL;
	if (basis->m_VolumeView)
	{
		CouPoly += basis->m_VolumeView->poly_count;
	}
	if (basis->next)
	{
		BonePolyCounter(basis->next, CouPoly);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		BonePolyCounter(child, CouPoly);
	}
}

void CSOEditDoc::NameCountAndLinkControl(void)
{
	int cou = 0, i = 0;
	bool errors = false, fatal_vol_errors = false;
	CString mesg, tmp_str;
#ifdef ALTERNATIVE_LANG
	mesg = "Errors found in the model:\n";
#else
	mesg = "Â ìîäåëå íàéäåíû îøèáêè:\n";
#endif
	m_Model->m_skeleton->m_bonelist->FindCountBone(cou);
	NameList = new char* [cou + 1];
	memset(NameList, 0, sizeof(char*) * (cou + 1));
	BoneNameCollection(m_Model->m_skeleton->m_bonelist);
	for (;NameList[i]; i++)
	{
		cou = NameList[i][strlen(NameList[i]) + 1];
		if (cou > 0)
		{
			errors = true;
#ifdef ALTERNATIVE_LANG
			tmp_str.Format("\n%d repeat(s) of the name of the bone \"%s\"", cou, NameList[i]);
#else
			tmp_str.Format("\n%d ïîâòîð (à, îâ) èìåíè êîñòè \"%s\"", cou, NameList[i]);
#endif
			mesg += tmp_str;
		}
	}
	for (i = 0; NameList[i]; i++)
	{
		delete[] NameList[i];
		NameList[i] = NULL;
	}
	delete[] NameList;
	NameList = NULL;
	if (m_Model->m_VolumeList)
	{
		cou = m_Model->m_VolumeList->FindCountBone();
		NameList = new char* [cou + 1];
		memset(NameList, 0, sizeof(char*) * (cou + 1));
		CVolume* pVol = m_Model->m_VolumeList->GetFirst();
		VolumeNameCollection(pVol);
		for (i = 0; NameList[i]; i++)
		{
			cou = NameList[i][strlen(NameList[i]) + 1];
			if (cou > 0)
			{
				errors = true;
#ifdef ALTERNATIVE_LANG
				tmp_str.Format("\n%d repeat(s) of volume's name \"%s\"", cou, NameList[i]);
#else
				tmp_str.Format("\n%d ïîâòîð (à, îâ) èìåíè âîëþìà \"%s\"", cou, NameList[i]);
#endif
				mesg += tmp_str;
			}
		}
		for (i = 0; NameList[i]; i++)
		{
			delete[] NameList[i];
			NameList[i] = NULL;
		}
		delete[] NameList;
		NameList = NULL;
		pVol = m_Model->m_VolumeList->GetFirst();
		while (pVol)
		{
			if (pVol->m_Bone)
			{
				CBone* pFB = m_Model->m_skeleton->m_bonelist->FindBone(pVol->m_Bone);
				if (!pFB)
				{
#ifdef ALTERNATIVE_LANG
					tmp_str.Format("\nVolume \"%s\" tied to a nonexistent bone \"%s\"", pVol->m_Name, pVol->m_Bone);
#else
					tmp_str.Format("\nÂîëþì \"%s\" ïðèâÿçàí ê íåñóùåñòâóþùåé êîñòè \"%s\"", pVol->m_Name, pVol->m_Bone);
#endif
					mesg += tmp_str;
					fatal_vol_errors = true;
				}
			}
			else
			{
#ifdef ALTERNATIVE_LANG
				tmp_str.Format("\nVolume \"%s\" not tied to a bone.", pVol->m_Name);
#else
				tmp_str.Format("\nÂîëþì \"%s\" íå ïðèâÿçàí ê êîñòè.", pVol->m_Name);
#endif
				mesg += tmp_str;
				fatal_vol_errors = true;
			}
			pVol = pVol->next;
		}
	}
	if (errors || fatal_vol_errors)
	{
#ifdef ALTERNATIVE_LANG
		tmp_str.Format("\n\nThe model is assembled incorrectly!!!");
#else
		tmp_str.Format("\n\nÌîäåëü ñîáðàíà íåâåðíî!!!");
#endif
		mesg += tmp_str;
		if (fatal_vol_errors)
		{
#ifdef ALTERNATIVE_LANG
			tmp_str.Format("\nThis model will drop the game!!!");
#else
			tmp_str.Format("\nÝòà ìîäåëü óðîíèò èãðó!!!");
#endif
			mesg += tmp_str;
		}
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, CString(mesg), "WARNING: CSOEditDoc::NameCountAndLinkControl", MB_ICONEXCLAMATION);
	}
}

void CSOEditDoc::BoneNameCollection(CBone* basis)
{
	bool find = false;
	int i = 0;
	if (!basis->sub)
	{
		RemoveQuotes(basis->m_Name);
		for (;NameList[i]; i++)
		{
			if (!stricmp(NameList[i], basis->m_Name))
			{
				if (NameList[i][strlen(NameList[i]) + 1] < 255)
				{
					NameList[i][strlen(NameList[i]) + 1]++;
				}
				find = true;
				break;
			}
		}
		if (!find)
		{
			NameList[i] = new char[strlen(basis->m_Name) + 2];
			memset(NameList[i], 0, strlen(basis->m_Name) + 2);
			strcpy(NameList[i], basis->m_Name);
		}
	}
	CBone* child = NULL;
	if ((child = basis->m_ChildFirst) != NULL)
	{
		BoneNameCollection(child);
	}
	if (basis->next)
	{
		BoneNameCollection(basis->next);
	}
}

void CSOEditDoc::VolumeNameCollection(CVolume* pVol)
{
	bool find = false;
	int i = 0;
	if (!pVol)
	{
		return;
	}
	RemoveQuotes(pVol->m_Name);
	for (;NameList[i]; i++)
	{
		if (!stricmp(NameList[i], pVol->m_Name))
		{
			NameList[i][strlen(NameList[i]) + 1]++;
			find = true;
			break;
		}
	}
	if (!find)
	{
		NameList[i] = new char[strlen(pVol->m_Name) + 2];
		memset(NameList[i], 0, strlen(pVol->m_Name) + 2);
		strcpy(NameList[i], pVol->m_Name);
	}
	pVol = pVol->next;
	if (pVol)
	{
		VolumeNameCollection(pVol);
	}
}

void CSOEditDoc::VolumePolyCounter(int& CouPoly)
{
	if (!m_Model || !m_Model->m_VolumeList)
	{
		return;
	}
	CVolume* pVol = m_Model->m_VolumeList->GetFirst();
	while (pVol)
	{
		if (pVol->m_Vol)
		{
			CouPoly += pVol->m_Vol->poly_count;
		}
		pVol = pVol->next;
	}
}

void CSOEditDoc::VolumesConnections(void) //Ýòî êîñòûëü. Íå ñìîã ïî÷èíèòü êîíôèã áèáëèîòåê äëÿ CBone.h
{
	if (!m_Model || !m_Model->m_VolumeList || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist)
	{
		return;
	}
	CVolume* pVol = m_Model->m_VolumeList->GetFirst();
	while (pVol)
	{
		VolumeConnection(m_Model->m_skeleton->m_bonelist, pVol);
		pVol = pVol->next;
	}
}

//Âñ¸ äîëæíî ðàáîòàòü íå "õîðîøî", à òàê êàê â èãðå. Ëèíêîâêà â èãðå - íå î÷åâèäíà...
bool CSOEditDoc::VolumeConnection(CBone* basis, CVolume* volptr)
{
	CBone* child = NULL;
	volptr->boneptr = NULL;
	if (volptr->m_Bone)
	{
		if (!stricmp(volptr->m_Bone, basis->m_Name))
		{
			volptr->boneptr = basis;
			return true;
		}
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		if (VolumeConnection(child, volptr))
		{
			return true;
		}
	}
	if (basis->next)
	{
		if (VolumeConnection(basis->next, volptr))
		{
			return true;
		}
	}
	return false;
}

void CSOEditDoc::On_Bone_Matrix_Copy()
{
	CBone* CopyBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (CopyBone)
	{
		if (CopyBone->Matrix34)
		{
			memcpy((void*)&CopyMatrix, CopyBone->Matrix34, sizeof(matrix34_t));
		}
	}
}

void CSOEditDoc::On_Bone_Parameters_Copy()
{
	CBone* CopyBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (CopyBone)
	{
		if (CopyBone->m_Parameters)
		{
			CopyParameters = CopyBone->m_Parameters;
		}
	}
}

void CSOEditDoc::On_Bone_Matrix_Paste()
{
	CBone* PasteBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (PasteBone)
	{
		if (!PasteBone->Matrix34)
		{
			PasteBone->Matrix34 = new matrix34_t;
		}
		memcpy(PasteBone->Matrix34, (void*)&CopyMatrix, sizeof(matrix34_t));
		UpdateAllViews(NULL, 0, NULL);
	}
}

void CSOEditDoc::On_Bone_Parameters_Paste()
{
	CBone* PasteBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (PasteBone)
	{
		if (PasteBone->m_Parameters)
		{
			delete[] PasteBone->m_Parameters; PasteBone->m_Parameters = NULL;
		}
		PasteBone->m_Parameters = new char[CopyParameters.GetLength() + 1];
		strcpy(PasteBone->m_Parameters, CopyParameters.GetBuffer(1));
	}
}

void CSOEditDoc::On_Volume_Matrix_Copy()
{
	CVolume* CopyVolume = m_Model->m_VolumeList->FindVolumeByTreeID(hSelTreeItem);
	if (CopyVolume)
	{
		if (CopyVolume->m_Matrix34)
		{
			memcpy((void*)&CopyMatrix, CopyVolume->m_Matrix34, sizeof(matrix34_t));
		}
	}
}

void CSOEditDoc::On_Volume_Matrix_Paste()
{
	CVolume* CopyVolume = m_Model->m_VolumeList->FindVolumeByTreeID(hSelTreeItem);
	if (CopyVolume)
	{
		if (!CopyVolume->m_Matrix34)
		{
			CopyVolume->m_Matrix34 = new matrix34_t;
		}
		memcpy(CopyVolume->m_Matrix34, (void*)&CopyMatrix, sizeof(matrix34_t));
		UpdateAllViews(NULL, 0, NULL);
	}
}

void CSOEditDoc::On_Bone_Add_Volume()
{
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (pBone)
	{
		VolToBone_Name = pBone->m_Name;
		VolToBone = true;
		if (pTreeCtrl->SelectItem(GeneralVolumes))
		{
			OnVolAdd();
		}
	}
	VolToBone = false;
	VolToBone_Name = _T("");
}

void CSOEditDoc::On_Bone_Delete_Mesh()
{
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (pBone)
	{
		if (pBone->m_VolumeViewName)
		{
			delete[] pBone->m_VolumeViewName;
			pBone->m_VolumeViewName = NULL;
		}
		if (pBone->m_VolumeView)
		{
			delete pBone->m_VolumeView;
			pBone->m_VolumeView = NULL;
		}
		UpdateAllViews(NULL, 0, NULL);
	}
}

void CSOEditDoc::OnBoneMergeToParent()
{
	CBone* childBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (!childBone || !childBone->m_parent)
	{
		return;
	}
	CBone* parentBone = childBone->m_parent;
	if (!childBone->m_VolumeViewName || !strlen(childBone->m_VolumeViewName))
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Selected child bone has no mesh (PLY) to merge.", "Bone merge", MB_OK | MB_ICONINFORMATION);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Ó âûáðàííîé äî÷åðíåé êîñòè íåò PLY-ìýøà äëÿ îáúåäèíåíèÿ.", "Bone merge", MB_OK | MB_ICONINFORMATION);
#endif
		return;
	}
	if (childBone->m_ChildFirst)
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Merge requires a leaf child bone (without nested children).", "Bone merge", MB_OK | MB_ICONINFORMATION);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Îáúåäèíåíèå äîñòóïíî òîëüêî äëÿ ëèñòîâîé äî÷åðíåé êîñòè (áåç âëîæåííûõ êîñòåé).", "Bone merge", MB_OK | MB_ICONINFORMATION);
#endif
		return;
	}

	char childPath[_MAX_PATH] = { 0 };
	strncpy(childPath, childBone->m_VolumeViewName, _MAX_PATH - 1);
	childPath[_MAX_PATH - 1] = '\0';
	FixPathDelim(childPath);
	CPly childPly(childPath);
	if (!childPly.loading_successes)
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to load child PLY file.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü çàãðóçèòü PLY-ôàéë äî÷åðíåé êîñòè.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#endif
		return;
	}

	if (childBone->Matrix34)
	{
		childPly.ApplyTransform(childBone->Matrix34);
	}
	childPly.RebindBoneName(childBone->m_Name, parentBone->m_Name);

	CPly* basePly = NULL;
	if (parentBone->m_VolumeViewName && strlen(parentBone->m_VolumeViewName))
	{
		char parentPath[_MAX_PATH] = { 0 };
		strncpy(parentPath, parentBone->m_VolumeViewName, _MAX_PATH - 1);
		parentPath[_MAX_PATH - 1] = '\0';
		FixPathDelim(parentPath);
		basePly = new CPly(parentPath);
		if (!basePly->loading_successes)
		{
			delete basePly;
			basePly = NULL;
#ifdef ALTERNATIVE_LANG
			MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to load parent PLY file.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#else
			MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü çàãðóçèòü PLY-ôàéë ðîäèòåëüñêîé êîñòè.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#endif
			return;
		}
	}
	else
	{
		basePly = new CPly(childPath);
		if (!basePly->loading_successes)
		{
			delete basePly;
			basePly = NULL;
			return;
		}
		if (childBone->Matrix34)
		{
			basePly->ApplyTransform(childBone->Matrix34);
		}
		basePly->RebindBoneName(childBone->m_Name, parentBone->m_Name);
	}

	bool ok = true;
	if (parentBone->m_VolumeViewName && strlen(parentBone->m_VolumeViewName))
	{
		ok = basePly->MergeKeepSeparateTextures(&childPly);
	}
	if (!ok)
	{
		delete basePly;
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to merge meshes. Ensure both PLY files have compatible vertex format.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü îáúåäèíèòü ìýøè. Ïðîâåðü ñîâìåñòèìîñòü ôîðìàòîâ âåðøèí.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#endif
		return;
	}

	char drive[_MAX_DRIVE] = { 0 }, dir[_MAX_DIR] = { 0 }, fname[_MAX_FNAME] = { 0 }, ext[_MAX_EXT] = { 0 };
	const char* outSource = (parentBone->m_VolumeViewName && strlen(parentBone->m_VolumeViewName)) ? parentBone->m_VolumeViewName : childBone->m_VolumeViewName;
	_splitpath(outSource, drive, dir, fname, ext);
	char mergedName[_MAX_FNAME] = { 0 };
	_snprintf(mergedName, _MAX_FNAME - 1, "%s_merged_%s", fname, childBone->m_Name ? childBone->m_Name : "child");
	mergedName[_MAX_FNAME - 1] = '\0';
	ForbiddenSymbolFixer(mergedName);
	char mergedPath[_MAX_PATH] = { 0 };
	_makepath(mergedPath, drive, dir, mergedName, ".ply");
	FixPathDelim(mergedPath);

	if (!basePly->WriteFile(mergedPath))
	{
		delete basePly;
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Failed to save merged PLY file.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "Íå óäàëîñü ñîõðàíèòü îáúåäèíåííûé PLY-ôàéë.", "ERROR: OnBoneMergeToParent", MB_ICONHAND);
#endif
		return;
	}
	delete basePly;

	if (parentBone->m_VolumeViewName)
	{
		delete[] parentBone->m_VolumeViewName;
		parentBone->m_VolumeViewName = NULL;
	}
	parentBone->m_VolumeViewName = new char[strlen(mergedPath) + 1];
	strcpy(parentBone->m_VolumeViewName, mergedPath);
	parentBone->AddPly(parentBone->m_VolumeViewName);

	hSelTreeItem = childBone->hTreeItem;
	OnBoneDelete();

	UpdateAllViews(NULL, 0, NULL);
#ifdef ALTERNATIVE_LANG
	MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Merged PLY saved:\n" + CString(mergedPath)), "Done", MB_OK | MB_ICONINFORMATION);
#else
	MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Îáúåäèíåííûé PLY ñîõðàíåí:\n" + CString(mergedPath)), "Ãîòîâî", MB_OK | MB_ICONINFORMATION);
#endif
}

void CSOEditDoc::On_Bone_Expand()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	ExpandCollapseTree(hTreeItem, true);
}

void CSOEditDoc::On_Bone_Collapse()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	ExpandCollapseTree(hTreeItem, false);
}

void CSOEditDoc::ExpandCollapseTree(HTREEITEM hTreeItem, bool mod)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (!pTreeCtrl->ItemHasChildren(hTreeItem))
	{
		return;
	}
	HTREEITEM hNextItem = pTreeCtrl->GetChildItem(hTreeItem);
	while (hNextItem != NULL)
	{
		ExpandCollapseTree(hNextItem, mod);
		hNextItem = pTreeCtrl->GetNextItem(hNextItem, TVGN_NEXT);
	}
	pTreeCtrl->Expand(hTreeItem, (mod) ? TVE_EXPAND : TVE_COLLAPSE);
}

void CSOEditDoc::On_Bone_Copy()//åñëè áûëà âûäåëåíà ñàá-êîñòü, òî íóæíî êîïèðîâàòü ïàðåíò
{
	Copy_Mode = 1;
	Code_blockToClipboard();
	Copy_Mode = 0;
}

void CSOEditDoc::On_Bone_Copy_Branch()
{
	Copy_Mode = 2;
	Code_blockToClipboard();
	Copy_Mode = 0;
}

void CSOEditDoc::On_Bone_Paste_As_Next()
{
	Paste_Mode = 1;
	ClipboardToCode_block();
	Paste_Mode = 0;
}

void CSOEditDoc::On_Bone_Paste_As_Interal()
{
	Paste_Mode = 2;
	ClipboardToCode_block();
	Paste_Mode = 0;
}

void CSOEditDoc::On_Bone_Cut()
{
	Copy_Mode = 2;
	Code_blockToClipboard();
	CuteMode = true;
	OnBoneDelete();
	CuteMode = false;
	Copy_Mode = 0;
}

void CSOEditDoc::ClipboardToCode_block()
{
	if (!m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist || !hSelTreeItem)
	{
		return;
	}
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	CBone* tBone = NULL;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (!pBone)
	{
		return;
	}
	Code_block = _T("");
	if (OpenClipboard(AfxGetApp()->m_pMainWnd->m_hWnd))//îòêðûâàåì áóôåð îáìåíà
	{
		HANDLE hData = GetClipboardData(CF_TEXT);//èçâëåêàåì òåêñò èç áóôåðà îáìåíà
		char* chBuffer = (char*)GlobalLock(hData);//áëîêèðóåì ïàìÿòü
		Code_block = chBuffer;
		GlobalUnlock(hData);//ðàçáëîêèðóåì ïàìÿòü
		CloseClipboard();//çàêðûâàåì áóôåð îáìåíà
	}
	if (Code_block.GetLength())
	{
		CSdl* cb_sdl = new CSdl(Code_block);
		CSkeleton* cb_skeleton = new CSkeleton(cb_sdl, false);
		delete cb_sdl;
		cb_sdl = NULL;
		tBone = cb_skeleton->m_bonelist;
		if (!tBone)
		{
			cb_skeleton->m_clean_on_destroy = true;
			delete cb_skeleton;
			cb_skeleton = NULL;
			return;
		}
		ReplaceBoneName(cb_skeleton->m_bonelist);
		if (Paste_Mode == 1)
		{
			hSelTreeItem = NULL;
			while (tBone)
			{
				tBone->m_parent = pBone->m_parent;
				tBone = tBone->next;
			}
			if (pBone->next)
			{
				pBone->next->prev = cb_skeleton->m_ChildLast;
				cb_skeleton->m_ChildLast->next = pBone->next;
			}
			pBone->next = cb_skeleton->m_bonelist;
			cb_skeleton->m_bonelist->prev = pBone;
			CBone* Parent = NULL;
			Parent = pBone->m_parent;
			if (Parent)
			{
				tBone = Parent->m_ChildFirst;
				while (tBone)
				{
					Parent->m_ChildLast = tBone;
					tBone = tBone->next;
				}
			}
			SaveExpandStatus(GeneralBones);
			UINT state;
			if ((state = pTreeCtrl->GetItemState(GeneralVolumes, TVIS_EXPANDED)) & TVIS_EXPANDED)
			{
				GeneralVolumesOpened = true;
			}
			CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
			pTreeCtrl->DeleteAllItems();
			LoadTextures();
			LoadExpandStatus(m_Model->m_skeleton->m_bonelist);
			if (GeneralVolumesOpened)
			{
				pTreeCtrl->Expand(GeneralVolumes, TVE_EXPAND);
			}
			if (cb_skeleton->m_ChildLast)
			{
				pTreeCtrl->SelectItem(cb_skeleton->m_ChildLast->hTreeItem);
			}
			else
			{
				pTreeCtrl->SelectItem(pBone->hTreeItem);
			}
			pTreeCtrl->SetFocus();
		}
		else
			if (Paste_Mode == 2)
			{
				ProcessBone(cb_skeleton->m_bonelist, hSelTreeItem, false);
				while (tBone)
				{
					tBone->m_parent = pBone;
					tBone = tBone->next;
				}
				if (!pBone->m_ChildFirst)
				{
					pBone->m_ChildFirst = cb_skeleton->m_bonelist;
					pBone->m_ChildLast = cb_skeleton->m_ChildLast;
				}
				else
				{
					pBone->m_ChildLast->next = cb_skeleton->m_bonelist;
					cb_skeleton->m_bonelist->prev = pBone->m_ChildLast;
					pBone->m_ChildLast = cb_skeleton->m_ChildLast;
				}
				if (cb_skeleton->m_bonelist)
				{
					pTreeCtrl->SelectItem(cb_skeleton->m_bonelist->hTreeItem);
				}
				pTreeCtrl->SetFocus();
			}
		delete cb_skeleton;
		cb_skeleton = NULL;
		CRect rect;
		pFrameWnd->GetClientRect(&rect);
		pFrameWnd->InvalidateRect(&rect, false);
		int BonePoly = 0, VolumePoly = 0;
		VolumePolyCounter(VolumePoly);
		if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
		{
			BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
		}
		CString TitleBuff;
		TitleBuff.Format("%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
		SetTitle(TitleBuff);
		VolumesConnections();
	}
}

void CSOEditDoc::ReplaceBoneName(CBone* basis)
{
	CBone* child;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	CBone* pFindBone = NULL;
	char tmpBoneName[_MAX_PATH] = { 0 };
	RemoveQuotes(basis->m_Name);
	pFindBone = m_Model->m_skeleton->m_bonelist->FindBone(basis->m_Name);
	if (pFindBone)
	{
		for (int i = 0; i < 100; i++)
		{
			memset(tmpBoneName, 0, _MAX_PATH);
			sprintf(tmpBoneName, "%s_%d", basis->m_Name, i);
			pFindBone = m_Model->m_skeleton->m_bonelist->FindBone(tmpBoneName);
			if (!pFindBone)
			{
				break;
			}
		}
		if (basis->m_Name)
		{
			delete[] basis->m_Name;
			basis->m_Name = NULL;
		}
		int len = strlen(tmpBoneName) + 1;
		basis->m_Name = new char[len];
		strcpy(basis->m_Name, tmpBoneName);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		ReplaceBoneName(child);
	}
	if (basis->next)
	{
		ReplaceBoneName(basis->next);
	}
}

void CSOEditDoc::SaveExpandStatus(HTREEITEM hTreeItem)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (!pTreeCtrl->ItemHasChildren(hTreeItem))
	{
		return;
	}
	UINT state;
	if ((state = pTreeCtrl->GetItemState(hTreeItem, TVIS_EXPANDED)) & TVIS_EXPANDED)
	{
		CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
		if (pBone)
		{
			pBone->m_OpenInTree = true;
		}
	}
	HTREEITEM hNextItem = pTreeCtrl->GetChildItem(hTreeItem);
	while (hNextItem != NULL)
	{
		SaveExpandStatus(hNextItem);
		hNextItem = pTreeCtrl->GetNextItem(hNextItem, TVGN_NEXT);
	}
}

void CSOEditDoc::LoadExpandStatus(CBone* basis)
{
	CBone* child;
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (basis->m_OpenInTree)
	{
		pTreeCtrl->Expand(basis->hTreeItem, TVE_EXPAND);
		basis->m_OpenInTree = false;
	}
	if (!basis->m_tree_check)
	{
		pTreeCtrl->SetCheck(basis->hTreeItem, FALSE);
	}
	if ((child = basis->m_ChildFirst) != NULL)
	{
		LoadExpandStatus(child);
	}
	if (basis->next)
	{
		LoadExpandStatus(basis->next);
	}
}

void CSOEditDoc::Code_blockToClipboard()
{
	if (!m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist || !hSelTreeItem)
	{
		return;
	}
	Code_block = _T("");
	CBone* CopyBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	if (CopyBone)
	{
		if (CopyBone->sub)
		{
			CopyBone = CopyBone->m_parent;
		}
	}
	if (CopyBone)
	{
		CopyBone->WriteMdl(NULL, 0);
	}
	if (OpenClipboard(AfxGetApp()->m_pMainWnd->m_hWnd))//îòêðûâàåì áóôåð îáìåíà
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //î÷èùàåì áóôåð
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, Code_block.GetLength() + 1);//âûäåëÿåì ïàìÿòü
		chBuffer = (char*)GlobalLock(hgBuffer); //áëîêèðóåì ïàìÿòü
		strcpy(chBuffer, LPCSTR(Code_block));
		GlobalUnlock(hgBuffer);//ðàçáëîêèðóåì ïàìÿòü
		SetClipboardData(CF_TEXT, hgBuffer);//ïîìåùàåì òåêñò â áóôåð îáìåíà
		CloseClipboard(); //çàêðûâàåì áóôåð îáìåíà
	}
}

void CSOEditDoc::On_Bone_Parent()
{
	CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hSelTreeItem);
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	if (pBone && pBone->m_parent)
	{
		hSelTreeItem = pBone->m_parent->hTreeItem;
		pTreeCtrl->SelectItem(hSelTreeItem);
	}
}

DWORD CSOEditDoc::Bone_Set_Random_Color()
{
	BYTE R = 0, G = 0, B = 0;
	DWORD result = 0x00ffffff;
	R = rand() % 255;
	G = rand() % 255;
	B = rand() % 255;
	result = RGB(R, G, B);
	return result;
}

void CSOEditDoc::OnBoneSkeletonAnimations()
{
	CBoneData bonedata;
	CSequenceList* m_SequenceList = NULL;
	if (!m_Model || !m_Model->m_skeleton)
	{
		return;
	}
	if (m_Model->m_skeleton->m_Animation && m_Model->m_skeleton->m_Animation->m_SequenceList)
	{
		m_SequenceList = new CSequenceList(m_Model->m_skeleton->m_Animation->m_SequenceList);
		bonedata.m_SequenceList = m_SequenceList;
	}
	bonedata.WindowMode = CBoneData::TAnimation;
	if (bonedata.DoModal() != IDOK)
	{
		delete bonedata.m_SequenceList;
		return;
	}
	if (bonedata.m_SequenceList && !bonedata.m_SequenceList->m_head)
	{
		delete bonedata.m_SequenceList;
		bonedata.m_SequenceList = NULL;
	}
	if (m_Model->m_skeleton->m_Animation && m_Model->m_skeleton->m_Animation->m_SequenceList)
	{
		delete m_Model->m_skeleton->m_Animation->m_SequenceList;
		m_Model->m_skeleton->m_Animation->m_SequenceList = NULL;
	}
	if (bonedata.m_SequenceList)
	{
		if (!m_Model->m_skeleton->m_Animation)
		{
			m_Model->m_skeleton->m_Animation = new CAnimation();
		}
		m_Model->m_skeleton->m_Animation->m_SequenceList = bonedata.m_SequenceList;
	}
	else
	{
		if (m_Model->m_skeleton->m_Animation)
		{
			delete m_Model->m_skeleton->m_Animation;
			m_Model->m_skeleton->m_Animation = NULL;
		}
	}
}

void CSOEditDoc::ANM_Tool(bool mod)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CButton* pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_PAUSE);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_PLAY);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_BACK);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_STEP);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_STOP);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INSERT_FRAME);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANMBOX_DELETE_FRAME);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_LOOP);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INHERIT_MATRIX);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_REVERSE_ANIM);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANM_LERP_APPLY);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANM_AUTO);
	pButton->EnableWindow(mod);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANMBOX_INHERIT_MATRIX);
	pFrameWnd->m_wndAnimBox.CheckDlgButton(IDC_ANMBOX_INHERIT_MATRIX, 0);
	pButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANMBOX_DROP_FIRST_FRAME);
	if (pButton)
		pButton->EnableWindow(mod);
	CStatic* pStatic = (CStatic*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANM_EDITING_MODE);
#ifdef ALTERNATIVE_LANG
	pStatic->SetWindowText("Mode: Skeleton Editing");
#else
	pStatic->SetWindowText("Ðåæèì: Ðåäàêòèðîâàíèå ñêåëåòà");
#endif
	CEdit* pEdit = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);;
	pEdit->EnableWindow(mod);
	CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
	pSpinner->EnableWindow(mod);
}

void CSOEditDoc::OnModEditing()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CStatic* pStatic = (CStatic*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANM_EDITING_MODE);
	if (m_Frame > 0)
	{
		if (!AnmEditingMod)
		{
#ifdef ALTERNATIVE_LANG
			pStatic->SetWindowText("Mode: Animation Editing");
#else
			pStatic->SetWindowText("Ðåæèì: Ðåäàêòèðîâàíèå àíèìàöèè");
#endif
		}
		AnmEditingMod = true;
	}
	else
	{
		if (AnmEditingMod)
		{
#ifdef ALTERNATIVE_LANG
			pStatic->SetWindowText("Mode: Skeleton Editing");
#else
			pStatic->SetWindowText("Ðåæèì: Ðåäàêòèðîâàíèå ñêåëåòà");
#endif
		}
		AnmEditingMod = false;
	}
}

void CSOEditDoc::CreateNewBoneInAnm(CBone* basis)
{
	if (!m_AnimBone)
	{
		return;
	}
	matrix34_t matrix_with_scale = { 0 };
	if (basis->Matrix34)
	{
		memcpy(&matrix_with_scale, basis->Matrix34, sizeof(matrix34_t));
	}
	else
	{
		matrix_with_scale.v[0][0] = 1.0f;
		matrix_with_scale.v[1][1] = 1.0f;
		matrix_with_scale.v[2][2] = 1.0f;
	}

	// Bake bone scale into the orientation part of the matrix.
	// Each COLUMN of the 3x3 (v[0..2][col]) is the basis vector for axis 'col'.
	// Multiplying column 'col' by m_Scales[col] scales that axis correctly.
	for (int col = 0; col < 3; col++)
	{
		for (int row = 0; row < 3; row++)
		{
			matrix_with_scale.v[row][col] *= basis->m_Scales[col];
		}
	}

	m_AnimBone->BoneAdd(basis->m_Name, &matrix_with_scale, basis->m_Visibility);
	AnimBoxUPD();
}

void CSOEditDoc::AnimBoxUPD()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	char szWindowText[_MAX_PATH];
	CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
	sprintf(szWindowText, "%d", m_Frame + 1);
	pWnd->SetWindowText(szWindowText);
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
	sprintf(szWindowText, "%d", m_AnimBone->m_FrameCnt);
	pWnd->SetWindowText(szWindowText);
	CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
	pSpinner->SetRange(1, m_AnimBone->m_FrameCnt);
	pSpinner->SetPos(m_Frame);
	CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
	pSlider->SetPos(m_Frame);
	pSlider->SetRange(0, m_AnimBone->m_FrameCnt - 1, true);
	pFrameWnd->m_wndAnimBox.CheckDlgButton(IDC_ANM_AUTO, auto_animation ? 1 : 0);
	if (m_AnimBone->m_FrameCnt > 0)
	{
		CWnd* pStartEdit = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_LERP_START_FRAME);
		CWnd* pEndEdit = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_LERP_END_FRAME);
		if (pStartEdit && pEndEdit)
		{
			char buff[32] = { 0 };
			pStartEdit->GetWindowText(buff, 31);
			int startFrame = atoi(buff);
			pEndEdit->GetWindowText(buff, 31);
			int endFrame = atoi(buff);
			if (startFrame < 1)
			{
				startFrame = 1;
			}
			if (endFrame <= startFrame)
			{
				endFrame = min(m_AnimBone->m_FrameCnt, startFrame + 1);
			}
			if (endFrame > m_AnimBone->m_FrameCnt)
			{
				endFrame = m_AnimBone->m_FrameCnt;
			}
			if (startFrame >= endFrame)
			{
				startFrame = max(1, endFrame - 1);
			}
			char szValue[32];
			sprintf(szValue, "%d", startFrame);
			pStartEdit->SetWindowText(szValue);
			sprintf(szValue, "%d", endFrame);
			pEndEdit->SetWindowText(szValue);
		}
	}
}

void CSOEditDoc::OnBnClickedAnmboxInheritMatrix()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	InheritMatrix = pFrameWnd->m_wndAnimBox.IsDlgButtonChecked(IDC_ANMBOX_INHERIT_MATRIX);
}

void CSOEditDoc::OnResetCameras()
{
	ResetCamView[0] = ResetCamView[1] = ResetCamView[2] = ResetCamView[3] = true;
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnEnChangeFindingComponent()
{
	ReturnFindingComponent();
}

void CSOEditDoc::ReturnFindingComponent(bool SetFocus)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CWnd* pWnd = (CWnd*)pFrameWnd->m_wndToolTab.GetDlgItem(IDC_FINDING_COMPONENT);
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	char text_buff[_MAX_PATH];
	if (!pWnd)
	{
		TRACE0("\nError: CSOEditDoc::ReturnFindingComponent\n");
		return;
	}
	pWnd->GetWindowText(text_buff, _MAX_PATH - 1);
	if (!strlen(text_buff))
	{
		return;
	}
	if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
	{
		CBone* pBone = m_Model->m_skeleton->m_bonelist->FindBone(text_buff);
		if (pBone && pBone->hTreeItem)
		{
			pTreeCtrl->SelectItem(pBone->hTreeItem);
			if (SetFocus)
			{
				pTreeCtrl->SetFocus();
			}
		}
	}
}

void CSOEditDoc::OnBoneAddLod()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	CBone* pFindBone = NULL;
	DWORD data = 0;
	if (!hTreeItem || !m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist)
	{
		return;
	}
	data = pTreeCtrl->GetItemData(hTreeItem);
	if (data == IT_BONETOP)
	{
		return;
	}
	pFindBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
	if (!pFindBone)
	{
		return;
	}
	if (pFindBone->sub)
	{
		return;
	}
	CString LodName;
#ifdef ALTERNATIVE_LANG
	static char szFilter[] = "PLY-file (*.ply)|*.ply|";
	static char szTitle[] = "Opening a mesh-file";
#else
	static char szFilter[] = "PLY-ôàéë (*.ply)|*.ply|";
	static char szTitle[] = "Îòêðûòèå ìýø-ôàéëà";
#endif
	bool p = false;
	char CurFolder[_MAX_PATH] = { 0 };
	if (strlen(pFrameWnd->m_CurMdlPath))
	{
		p = true;
		strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
		char* ptr = strrchr(CurFolder, '/') + 1;
		*ptr = 0;
		InvertFixPathDelim(CurFolder);
	}
	CFileDialog dlg(TRUE, "*.ply", ((p) ? CurFolder : NULL), OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if (dlg.DoModal() == IDOK)
	{
		LodName = dlg.m_ofn.lpstrFile;
	}
	else
	{
		return;
	}
	if (!pFindBone->m_LODView)
	{
		pFindBone->m_LODView = true;
	}
	pFindBone->m_Layer = -1;
	int NewSubNum = 1;
	CBone* child = pFindBone->m_ChildFirst, * NewChildLodAfter = NULL;
	while (child && child->m_LODView && child->sub)
	{
		NewSubNum++;
		NewChildLodAfter = child;
		child = child->next;
	}
	char* c = NULL;
	c = strrchr(LodName.GetBuffer(1), '\\') + 1;
	CBone* tBone = new CBone();
	tBone->m_parent = pFindBone;
	tBone->sub = NewSubNum;
	tBone->m_LODView = true;
	tBone->SetName(c);
	hSelTreeItem = NULL;
	child = pFindBone->m_ChildFirst;
	if (!child)
	{
		pFindBone->m_ChildFirst = tBone;
		pFindBone->m_ChildLast = tBone;
		pFindBone->m_curr = tBone;
	}
	else
	{
		if (!NewChildLodAfter)
		{
			pFindBone->m_ChildFirst->prev = tBone;
			tBone->next = pFindBone->m_ChildFirst;
			pFindBone->m_ChildFirst = tBone;
		}
		else
		{
			if (NewChildLodAfter->next)
			{
				tBone->next = NewChildLodAfter->next;
				NewChildLodAfter->next->prev = tBone;
			}
			NewChildLodAfter->next = tBone;
			tBone->prev = NewChildLodAfter;
		}
	}
	SaveExpandStatus(GeneralBones);
	UINT state;
	if ((state = pTreeCtrl->GetItemState(GeneralVolumes, TVIS_EXPANDED)) & TVIS_EXPANDED)
	{
		GeneralVolumesOpened = true;
	}
	pTreeCtrl->DeleteAllItems();
	LoadTextures();
	LoadExpandStatus(m_Model->m_skeleton->m_bonelist);
	if (GeneralVolumesOpened)
	{
		pTreeCtrl->Expand(GeneralVolumes, TVE_EXPAND);
	}
	pTreeCtrl->SelectItem(tBone->hTreeItem);
	hSelTreeItem = tBone->hTreeItem;
	pTreeCtrl->SetFocus();
	CRect rect;
	pFrameWnd->GetClientRect(&rect);
	pFrameWnd->InvalidateRect(&rect, false);
	int BonePoly = 0, VolumePoly = 0;
	VolumePolyCounter(VolumePoly);
	if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
	{
		BonePolyCounter(m_Model->m_skeleton->m_bonelist, BonePoly);
	}
	char TitleBuff[512];
	sprintf(TitleBuff, "%s [%d PLY Poly] [%d VOL Poly]", pFrameWnd->m_CurMdlPath, BonePoly, VolumePoly);
	SetTitle(TitleBuff);
	VolumesConnections();
	OnBoneProp();
}

void CSOEditDoc::OnModelCopyrightsParser()
{
	m_old_author = m_old_author_ply = m_old_author_vol = m_old_date = m_old_file = "";
	char* c = NULL, * tc = NULL;
	//Copyrights type 
	static char* CR_MDL_List[] =
	{
		"Exported by:", "Exported by", "Model created by:", "Model created by", "Model by:", "Model by", "Made by:", "Made by", "Created by:", "Created by", "Assembled by:", "Assembled by",
		"By:", "by", "Model recreated:", "Model recreated", "Model create/rebuild by:", "Model create/rebuild by", "Modified by:", "Modified by", 0
	};
	static char* CR_PLY_List[] = { "PLY Geometry by:", "PLY Geometry by", 0 };
	static char* CR_VOL_List[] = { "VOL Geometry by:", "VOL Geometry by", 0 };
	static char* CR_Date_List[] = { "Date:", "Date", "Last build date:", "Last build date", 0 };
	static char* CR_File_List[] = { "File:", "File", 0 };
	bool st_mdl_author = false, st_ply_author = false, st_vol_author = false, st_date_author = false, st_file_author = false;
	if (m_Model->m_sdl->StartComments.GetLength() < 1)
	{
		return;
	}
	c = m_Model->m_sdl->StartComments.GetBuffer(1);
	while (true)
	{
		if (*c == ';')
		{
			c++;
		}
		while ((*c == ' ') || (*c == '\t') || (*c == '\r') || (*c == '\n'))
		{
			c++;
		}
		if (*c == '\0')
		{
			return;
		}
		tc = c;
		while (*c != '\r' && *c != '\n' && *c != '\0')
		{
			c++;
		}
		int st_len = (int)(c - tc) + 1;
		char* tmp_str = NULL;
		tmp_str = new char[st_len];
		memset(tmp_str, 0, st_len);
		strncpy(tmp_str, tc, st_len - 1);
		if (!st_mdl_author)
		{
			for (int i = 0; CR_MDL_List[i]; i++)
			{
				int len = strlen(CR_MDL_List[i]);
				if (!strnicmp(tmp_str, CR_MDL_List[i], len))
				{
					tc += len;
					while ((*tc == ' ') || (*tc == '\t') || (*tc == '\r') || (*tc == '\n'))
					{
						tc++; len++;
					}
					if (len < strlen(tmp_str))
					{
						m_old_author = (tmp_str + len);
						st_mdl_author = true;
					}
					break;
				}
			}
		}
		if (!st_ply_author)
		{
			for (int i = 0; CR_PLY_List[i]; i++)
			{
				int len = strlen(CR_PLY_List[i]);
				if (!strnicmp(tmp_str, CR_PLY_List[i], len))
				{
					tc += len;
					while ((*tc == ' ') || (*tc == '\t') || (*tc == '\r') || (*tc == '\n'))
					{
						tc++; len++;
					}
					if (len < strlen(tmp_str))
					{
						m_old_author_ply = (tmp_str + len);
						st_ply_author = true;
					}
					break;
				}
			}
		}
		if (!st_vol_author)
		{
			for (int i = 0; CR_VOL_List[i]; i++)
			{
				int len = strlen(CR_VOL_List[i]);
				if (!strnicmp(tmp_str, CR_VOL_List[i], len))
				{
					tc += len;
					while ((*tc == ' ') || (*tc == '\t') || (*tc == '\r') || (*tc == '\n'))
					{
						tc++; len++;
					}
					if (len < strlen(tmp_str))
					{
						m_old_author_vol = (tmp_str + len);
						st_vol_author = true;
					}
					break;
				}
			}
		}
		if (!st_date_author)
		{
			for (int i = 0; CR_Date_List[i]; i++)
			{
				int len = strlen(CR_Date_List[i]);
				if (!strnicmp(tmp_str, CR_Date_List[i], len))
				{
					tc += len;
					while ((*tc == ' ') || (*tc == '\t') || (*tc == '\r') || (*tc == '\n'))
					{
						tc++; len++;
					}
					if (len < strlen(tmp_str))
					{
						m_old_date = (tmp_str + len);
						st_date_author = true;
					}
					break;
				}
			}
		}
		if (!st_file_author)
		{
			for (int i = 0; CR_File_List[i]; i++)
			{
				int len = strlen(CR_File_List[i]);
				if (!strnicmp(tmp_str, CR_File_List[i], len))
				{
					tc += len;
					while ((*tc == ' ') || (*tc == '\t') || (*tc == '\r') || (*tc == '\n'))
					{
						tc++; len++;
					}
					if (len < strlen(tmp_str))
					{
						m_old_file = (tmp_str + len);
						st_file_author = true;
					}
					break;
				}
			}
		}
		delete[] tmp_str;
		while (*c != ';' && *c != '\0')
		{
			c++;
		}
	}
}

void CSOEditDoc::OnBoneHideBranch()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), false, false, true);
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBoneHideCollision()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), false, true, false);
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBoneHideBranchAndCollision()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), false, true, true);
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBoneShowBranch()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), true, false, true);
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBoneShowCollision()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), true, true, false);
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBoneShowBranchAndCollision()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	ShowHideCollisionBoneBranchTree(pTreeCtrl->GetSelectedItem(), true, true, true);
	UpdateAllViews(NULL, 0, NULL);
}

//Àõ, êàêàÿ íåâåçóõà-à-à. Àáñîëþòíî íåòó ðèôìû...
void CSOEditDoc::ShowHideCollisionBoneBranchTree(HTREEITEM hTreeItem, bool on, bool collision, bool branch)
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hNextItem = pTreeCtrl->GetChildItem(hTreeItem);
	while (hNextItem != NULL)
	{
		ShowHideCollisionBoneBranchTree(hNextItem, on, collision, branch);
		hNextItem = pTreeCtrl->GetNextItem(hNextItem, TVGN_NEXT);
	}
	if (branch)
	{
		pTreeCtrl->SetCheck(hTreeItem, on);
	}
	CBone* pBone = NULL;
	if (m_Model && m_Model->m_skeleton && m_Model->m_skeleton->m_bonelist)
	{
		pBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
		if (pBone)
		{
			if (branch)
			{
				pBone->m_tree_check = on;
			}
			if (collision && m_Model->m_VolumeList)
			{
				CVolume* pVol = m_Model->m_VolumeList->GetFirst();
				while (pVol)
				{
					if (pVol->boneptr == pBone)
					{
						pVol->m_tree_check = on;
						if (pVol->hTreeItem)
						{
							pTreeCtrl->SetCheck(pVol->hTreeItem, on);
						}
					}
					pVol = pVol->next;
				}
			}
		}
	}
}

void CSOEditDoc::OnAnmSave()
{
	FILE* fp;
	bool saveit = true, mod = false;
#ifdef ALTERNATIVE_LANG
	static char szFilter[] = "ANM-file (*.anm)|*.anm|";
	static char szTitle[] = "Save the animation file as:";
#else
	static char szFilter[] = "ANM-Ôàéë (*.anm)|*.anm|";
	static char szTitle[] = "Ñîõðàíèòü ôàéë àíèìàöèè êàê:";
#endif
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	bool p = false;
	char CurFolder[_MAX_PATH] = { 0 };
	if (strlen(pFrameWnd->m_CurMdlPath))
	{
		p = true;
		strcpy(CurFolder, pFrameWnd->m_CurMdlPath);
		char* ptr = strrchr(CurFolder, '/') + 1;
		*ptr = 0;
		InvertFixPathDelim(CurFolder);
	}
	CFileDialog dlg(TRUE, "*.anm", ((p) ? CurFolder : NULL), OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON, szFilter);
	dlg.m_ofn.lpstrTitle = szTitle;
	if (!strlen(szAnimPath))
	{
		mod = true;
	}
	if (!mod)
	{
		if ((fp = fopen(szAnimPath, "r")) != NULL)
		{
			fclose(fp);
		}
		else
		{
			mod = true;
		}
	}
	if (mod ? dlg.DoModal() == IDOK : true)
	{
		if (mod)
		{
			strcpy(szAnimPath, dlg.m_ofn.lpstrFile);
			FixPathDelim(szAnimPath);
		}
		if ((fp = fopen(szAnimPath, "r")) != NULL)
		{
			fclose(fp);
			int save = 0;
			if (mod)
			{
#ifdef ALTERNATIVE_LANG
				save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("File:\n" + CString(szAnimPath) + "\nalready exists. Replace?"), "WARNING: CSOEditDoc::OnAnmSave", MB_YESNO | MB_ICONEXCLAMATION);
#else
				save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Ôàéë:\n" + CString(szAnimPath) + "\nóæå ñóùåñòâóåò. Çàìåíèòü?"), "WARNING: CSOEditDoc::OnAnmSave", MB_YESNO | MB_ICONEXCLAMATION);
#endif
			}
			else
			{
#ifdef ALTERNATIVE_LANG
				save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Do you confirm overwriting the animation file?"), "QUESTION: CSOEditDoc::OnAnmSave", MB_YESNO | MB_ICONQUESTION);
#else
				save = MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Âû ïîäòâåðæäàåòå ïåðåçàïèñü ôàéëà àíèìàöèè?"), "QUESTION: CSOEditDoc::OnAnmSave", MB_YESNO | MB_ICONQUESTION);
#endif
			}
			if (save != IDYES)
			{
				saveit = false;
			}
		}
		if (saveit)
		{
			if (m_DropFirstFrameOnSave && m_AnimBone && m_AnimBone->m_FrameCnt >= 2)
			{
				m_AnimBone->FrameDelete(0);
				// Keep the current-frame cursor inside valid range.
				if (m_Frame > 0)
					m_Frame--;
				// Reflect the new frame count in the UI.
				AnimBoxUPD();
			}

			if ((fp = fopen(szAnimPath, "wb")) != NULL)
			{
				m_AnimBone->ProcessSave(fp);
				fclose(fp);
			}
		}
	}
}

void CSOEditDoc::OnAnmNew()
{
	// TODO:     
	if (!m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist)
	{
#ifdef ALTERNATIVE_LANG
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "The model skeleton is not loaded!", "ERROR: CSOEditDoc::OnAnmNew", MB_ICONHAND);
#else
		MessageBox(AfxGetApp()->m_pMainWnd->m_hWnd, "    !", "ERROR: CSOEditDoc::OnAnmNew", MB_ICONHAND);
#endif
		return;
	}

	m_Animate = false;
	m_Frame = 0;
	old_frm = 0;
	memset(szAnimPath, 0, _MAX_PATH);

	if (m_AnimBone)
	{
		delete m_AnimBone;
		m_AnimBone = NULL;
	}

	m_AnimBone = new CAnimBone();
	m_AnimBone->Header_ID = 0x00060001;
	m_AnimBone->m_FrameCnt = 1;
	m_AnimBone->m_Frames = new CAnimFrame[m_AnimBone->m_FrameCnt];

	ANM_Tool(true);
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
	pWnd->SetWindowText("1");
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
	pWnd->SetWindowText("1");
	CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
	pSpinner->SetRange(1, 1);
	pSpinner->SetPos(1);
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);
	pWnd->SetWindowText("1.0");
	CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
	pSlider->SetPos(0);
	pSlider->SetRange(0, 0, true);
	pWnd = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_NAME);
	pWnd->SetWindowText("new_animation.anm");

	CMenu* pMenu = pFrameWnd->GetMenu();
	CMenu* pPopup = pMenu->GetSubMenu(3);
	ASSERT(pPopup != NULL);
#ifdef ALTERNATIVE_LANG
	pPopup->EnableMenuItem(ID_ANM_SAVE_AL, MF_BYCOMMAND | MF_ENABLED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE_AL, MF_BYCOMMAND | MF_ENABLED);
#else
	pPopup->EnableMenuItem(ID_ANM_SAVE, MF_BYCOMMAND | MF_ENABLED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE, MF_BYCOMMAND | MF_ENABLED);
#endif
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnAnmClose()
{
	if (m_AnimBone)
	{
		delete m_AnimBone;
		m_AnimBone = NULL;
	}
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CWnd* pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
	pWnd->SetWindowText("0");
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_FRAMES);
	pWnd->SetWindowText("0");
	CSpinButtonCtrl* pSpinner = (CSpinButtonCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED_CONTROL);
	pSpinner->SetRange(1, 1);
	pSpinner->SetPos(1);
	pWnd = (CWnd*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_SPEED);
	pWnd->SetWindowText("1.0");
	CSliderCtrl* pSlider = (CSliderCtrl*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
	pSlider->SetPos(0);
	pSlider->SetRange(0, 0, true);
	pWnd = (CEdit*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANIM_NAME);
	pWnd->SetWindowText("");
	ANM_Tool(false);
	Anm_and_skeleton_editing_prohibited = false;
	memset(szAnimPath, 0, _MAX_PATH);
	CMenu* pMenu = pFrameWnd->GetMenu();
	CMenu* pPopup = pMenu->GetSubMenu(3);
	ASSERT(pPopup != NULL);
#ifdef ALTERNATIVE_LANG
	pPopup->EnableMenuItem(ID_ANM_SAVE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#else
	pPopup->EnableMenuItem(ID_ANM_SAVE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	pPopup->EnableMenuItem(ID_ANM_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
#endif
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnAddBoneToAnm()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	CBone* pFindBone = NULL;
	bool Finded = false;
	if (!hTreeItem || !m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist || !m_AnimBone)
	{
		return;
	}
	pFindBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
	for (int i = 0; i < m_AnimBone->m_BoneCnt; i++)
	{
		if (!stricmp(pFindBone->m_Name, m_AnimBone->m_BoneMap[i]))
		{
			Finded = true;
		}
	}
	if (!Finded)
	{
		CreateNewBoneInAnm(pFindBone);
	}
}

void CSOEditDoc::OnDelBoneFromAnm()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl->GetSelectedItem();
	CBone* pFindBone = NULL;
	int Finded = 0;
	if (!hTreeItem || !m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist || !m_AnimBone)
	{
		return;
	}
	pFindBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
	for (int i = 0; i < m_AnimBone->m_BoneCnt; i++)
	{
		if (!stricmp(pFindBone->m_Name, m_AnimBone->m_BoneMap[i]))
		{
			Finded = i;
		}
	}
	if (Finded)//åñëè êîñòü îäíà - òî óäàëÿòü íåëüçÿ
	{
		m_AnimBone->BoneDelete(Finded);
	}
}

void CSOEditDoc::OnBnClickedAnmboxDeleteFrame()
{
	if (!m_AnimBone)
	{
		return;
	}
	m_AnimBone->FrameDelete(m_Frame);
	AnimBoxUPD();
}

void CSOEditDoc::OnBnClickedAnmboxInsertFrame()
{
	if (!m_AnimBone)
	{
		return;
	}
	m_Frame = m_AnimBone->OnInsertFrame(m_Frame);
	AnimBoxUPD();
}

void CSOEditDoc::OnBnClickedAnmboxAuto()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	if (!pFrameWnd)
	{
		return;
	}
	CButton* pAutoButton = (CButton*)pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_ANM_AUTO);
	if (!pAutoButton)
	{
		return;
	}
	auto_animation = pAutoButton->GetCheck() == BST_CHECKED;
}

void CSOEditDoc::OnReverseAnimation()
{
	if (!m_AnimBone || m_AnimBone->m_FrameCnt <= 1)
	{
		return;
	}
	m_AnimBone->ReverseFrames();
	m_Frame = 0;
	AnimBoxUPD();
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBnClickedAnmLerpApply()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	if (!pFrameWnd || !m_AnimBone)
	{
		return;
	}
	char frameBuff[32] = { 0 };
	int startFrame = 0, endFrame = 0;
	CWnd* pStartEdit = pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_LERP_START_FRAME);
	CWnd* pEndEdit = pFrameWnd->m_wndAnimBox.GetDlgItem(IDC_LERP_END_FRAME);
	if (!pStartEdit || !pEndEdit)
	{
		return;
	}
	pStartEdit->GetWindowText(frameBuff, 31);
	startFrame = atoi(frameBuff);
	memset(frameBuff, 0, sizeof(frameBuff));
	pEndEdit->GetWindowText(frameBuff, 31);
	endFrame = atoi(frameBuff);
	if (startFrame < 1 || endFrame < 1 || startFrame >= endFrame || endFrame > m_AnimBone->m_FrameCnt)
	{
		return;
	}

	CTreeCtrl* pTreeCtrl = (CTreeCtrl*)pFrameWnd->m_wndToolTab.m_ModelTree.GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem = pTreeCtrl ? pTreeCtrl->GetSelectedItem() : NULL;
	if (!hTreeItem || !m_Model || !m_Model->m_skeleton || !m_Model->m_skeleton->m_bonelist)
	{
		return;
	}
	CBone* pFindBone = m_Model->m_skeleton->m_bonelist->FindBoneByTreeID(hTreeItem);
	if (!pFindBone)
	{
		return;
	}

	int boneIndex = -1;
	for (int i = 0; i < m_AnimBone->m_BoneCnt; i++)
	{
		if (!stricmp(pFindBone->m_Name, m_AnimBone->m_BoneMap[i]))
		{
			boneIndex = i;
			break;
		}
	}
	if (boneIndex < 0)
	{
		return;
	}

	CAnimSub* startSub = m_AnimBone->m_Frames[startFrame - 1].FindSub(boneIndex);
	CAnimSub* endSub = m_AnimBone->m_Frames[endFrame - 1].FindSub(boneIndex);
	if (!startSub || !endSub)
	{
		return;
	}
	const int stepCount = endFrame - startFrame;
	for (int frame = startFrame + 1; frame < endFrame; frame++)
	{
		const float t = (float)(frame - startFrame) / (float)stepCount;
		CAnimSub* midSub = m_AnimBone->m_Frames[frame - 1].FindSub(boneIndex);
		if (!midSub)
		{
			continue;
		}
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 3; col++)
			{
				const float a = startSub->m_Matrix34.v[row][col];
				const float b = endSub->m_Matrix34.v[row][col];
				midSub->m_Matrix34.v[row][col] = a + ((b - a) * t);
			}
		}
		midSub->m_Visible = (t < 0.5f) ? startSub->m_Visible : endSub->m_Visible;
	}
	UpdateAllViews(NULL, 0, NULL);
}

void CSOEditDoc::OnBnClickedAnmboxDropFirstFrame()
{
	CMainFrame* pFrameWnd = (CMainFrame*)AfxGetMainWnd();
	m_DropFirstFrameOnSave =
		pFrameWnd->m_wndAnimBox.IsDlgButtonChecked(IDC_ANMBOX_DROP_FIRST_FRAME) != 0;
}
