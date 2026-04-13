// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_SOEDITDOC_H__43E63070_57B6_4EE2_BC1A_3C058CA6D208__INCLUDED_)
#define AFX_SOEDITDOC_H__43E63070_57B6_4EE2_BC1A_3C058CA6D208__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CModel.h"
#include "CVolume.h"
#include "CBone.h"
#include "CMaterial.h"
#include "CAnimFrame.h"
#include "CAnimData.h"
#include "CObb2.h"
#include "CVolProp.h"

class CSOEditDoc : public CDocument
{
protected: // create from serialization only
	CSOEditDoc();
	DECLARE_DYNCREATE(CSOEditDoc)

    // Attributes
    public:

    // Operations
    public:
       v3_t       model;
       CAnimBone *m_AnimBone;
       CModel    *m_Model;
       CMtlList   m_MtlList;
       int        m_Frame;
       bool       m_Animate;
       double     m_Speed;
       bool       m_Loop;
       char      *m_SelBone;
       char      *m_SelVolume;
       char      *m_SelObstacle;
       char      *m_SelArea;
       CVolProp  *m_VolProp;
       void       OnLoad(void);
       bool       m_skin;
       char       szAnimPath[_MAX_PATH];
       CAnimData  m_AnimData;
       CVolume   *m_pVolume;
	   char     **NameList;
       bool       m_NewModel;
       v3_t       m_Center;
       float      m_MinY, m_MaxY;
	   UINT       texName[256];
	   UINT       tmptexName[256];
	   int        m_texCount;
	   HTREEITEM  hSelTreeItem, GeneralVolumes, GeneralBones;
	   bool       Lod_writer, fix_mx_ori, VolToBone, TexOff, GeneralVolumesOpened, CuteMode, Colorized_3d;
	   bool       Anm_and_skeleton_editing_prohibited, AnmEditingMod, InheritMatrix, auto_animation;
	   bool       ResetCamView[4];
	   int        ViewWorks[4], old_frm;
	   matrix34_t CopyMatrix;
	   CString    CopyParameters, VolToBone_Name;
	   BYTE       Copy_Mode, Paste_Mode;
	   CString    Code_block;
	   View_mode_3d m_ViewMod;
	   CPly *Lamp_View;
	   CBone *BLamp_View;
	   bool trylamp, add_lod_mode;
	   anm_tasks Animation_Automation;
	   int m_GridSize;
	   bool m_grid;
	   BOOL m_lamp;
	   
		CString m_old_author;
		CString m_old_author_ply;
		CString m_old_author_vol;
		CString m_old_date;
		CString m_old_file;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSOEditDoc)
	public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSOEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnBoneProp();
	afx_msg void OnVolProp();
	afx_msg void OnBoneDelete();
	afx_msg void OnVolDelete();

	void ResetBone(CBone *basis);
	void ANM_Tool(bool mod);
	void CreateNewBoneInAnm(CBone *basis);
	void AnimBoxUPD();
	void ReturnFindingComponent(bool SetFocus = false);
	void OnClearAll(bool question = true);
	void OnModelSave(bool mod);


	protected:
	void ProcessArea(CObb2 *area, HTREEITEM hParent, bool select);
	void ProcessObstacle(CObb2 *obstacle, HTREEITEM hParent, bool select);
	void ProcessVolume(CVolume *volume, HTREEITEM hParent, bool select);
	void ProcessBone(CBone *basis, HTREEITEM hParent, bool select);
	void ProcessSkin(CBone *basis);

	void LoadTextures(void);
	void BonePolyCounter(CBone *basis, int &CouPoly);
	void NameCountAndLinkControl(void);
	void BoneNameCollection(CBone* basis);
	void VolumeNameCollection(CVolume *pVol);
	void VolumePolyCounter(int &CouPoly);
	bool VolumeConnection(CBone *basis, CVolume *volptr);
	void VolumesConnections(void);
	void Code_blockToClipboard();
	void ClipboardToCode_block();
	void ReplaceBoneName(CBone *basis);
	void SaveExpandStatus(HTREEITEM hTreeItem);
	void LoadExpandStatus(CBone *basis);
	void ChildBoneDelete(CBone * curbone, bool ChildrenDelete);
	DWORD Bone_Set_Random_Color();
	void OnModelCopyrightsParser();
	void ExpandCollapseTree(HTREEITEM hTreeItem, bool mod);
	void ShowHideCollisionBoneBranchTree(HTREEITEM hTreeItem, bool on, bool collision, bool branch);
	void AnalysisOfBoneBranchesByVolume(CBone *basis, CString &tmpStr, bool &binding, bool next);
	void DeleteBindingVolumes(CBone *basis, bool next);
	//void ResetBones(void);


// Generated message map functions
protected:
	//{{AFX_MSG(CSOEditDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnAnmLoad();
	afx_msg void OnVolAdd();
	afx_msg void OnBoneAdd();
	afx_msg void OnAnmEdit();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	afx_msg void OnBoneInsert();
	afx_msg void On_Bone_Matrix_Copy();
	afx_msg void On_Bone_Parameters_Copy();
	afx_msg void On_Bone_Matrix_Paste();
	afx_msg void On_Bone_Parameters_Paste();
	afx_msg void On_Volume_Matrix_Copy();
	afx_msg void On_Volume_Matrix_Paste();
	afx_msg void On_Bone_Add_Volume();
	afx_msg void On_Bone_Delete_Mesh();
	afx_msg void On_Bone_Expand();
	afx_msg void On_Bone_Collapse();
	afx_msg void On_Bone_Copy();
	afx_msg void On_Bone_Copy_Branch();
	afx_msg void On_Bone_Paste_As_Next();
	afx_msg void On_Bone_Paste_As_Interal();
	afx_msg void On_Bone_Cut();
	afx_msg void On_Bone_Parent();
	afx_msg void OnBoneSkeletonAnimations();
	afx_msg void OnFileNew();
	afx_msg void OnAnmNew();
	afx_msg void OnBnClickedAnmboxInheritMatrix();
	afx_msg void OnResetCameras();
	afx_msg void OnEnChangeFindingComponent();
	afx_msg void OnBoneAddLod();
	afx_msg void OnBoneHideBranch();
	afx_msg void OnBoneHideCollision();
	afx_msg void OnBoneShowBranch();
	afx_msg void OnBoneShowCollision();
	afx_msg void OnBoneShowBranchAndCollision();
	afx_msg void OnBoneHideBranchAndCollision();
	afx_msg void OnAnmSave();
	afx_msg void OnAnmClose();
	//}}AFX_MSG
public:
	afx_msg void OnModEditing();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnAddBoneToAnm();
	afx_msg void OnDelBoneFromAnm();
	afx_msg void OnBnClickedAnmboxDeleteFrame();
	afx_msg void OnBnClickedAnmboxInsertFrame();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOEDITDOC_H__43E63070_57B6_4EE2_BC1A_3C058CA6D208__INCLUDED_)
