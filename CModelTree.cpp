// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "soedit.h"
#include "mainfrm.h"
#include "SOEditDoc.h"
#include "CModelTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelTree dialog


CModelTree::CModelTree(CWnd* pParent /*=NULL*/)
	: CDialog(CModelTree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelTree)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	hTreeItemCheckStatusChange = NULL;
}

void CModelTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelTree)
	DDX_Control(pDX, IDC_MODELTREE, m_ModelTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModelTree, CDialog)
	//{{AFX_MSG_MAP(CModelTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MODELTREE, OnSelchangedModeltree)
	//ON_NOTIFY(NM_DBLCLK, IDC_MODELTREE, OnEdit)
		//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_MODELTREE, &CModelTree::OnNMClickModeltree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelTree message handlers

void CModelTree::OnSelchangedModeltree(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrame -> GetActiveDocument();
	CTreeCtrl *pTreeCtrl = (CTreeCtrl *)GetDlgItem(IDC_MODELTREE);
	HTREEITEM   hTreeItem;
	pDoc -> hSelTreeItem = NULL;
	hTreeItem = pTreeCtrl -> GetSelectedItem();
	if(pDoc -> m_SelBone)
		{delete[] pDoc -> m_SelBone;}
	pDoc -> m_SelBone = NULL;
	if(pDoc -> m_SelVolume)
		{delete[] pDoc -> m_SelVolume;}
	pDoc -> m_SelVolume = NULL;
	if(pDoc -> m_SelObstacle)
		{delete[] pDoc -> m_SelObstacle;}
	pDoc -> m_SelObstacle = NULL;
	if(pDoc -> m_SelArea)
		{delete[] pDoc -> m_SelArea;}
	pDoc -> m_SelArea = NULL;
	if(hTreeItem && pTreeCtrl -> GetParentItem(hTreeItem))
	{
		char szTemp[_MAX_PATH];
		strcpy(szTemp, pTreeCtrl -> GetItemText(hTreeItem));
		DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
		switch(itemData)
		{
			case IT_BONE:
			{
				pDoc -> hSelTreeItem = hTreeItem;
				pDoc -> m_SelBone = new char[strlen(szTemp) + 1];
				strcpy(pDoc -> m_SelBone, szTemp);
			}
			break;
			case IT_VOLUME:
			{
				pDoc -> hSelTreeItem = hTreeItem;
				pDoc -> m_SelVolume = new char[strlen(szTemp) + 1];
				strcpy(pDoc -> m_SelVolume, szTemp);
				pFrame -> m_wndToolTab.m_VolPropDlg.m_pVolume = pDoc -> m_Model -> m_VolumeList -> FindByName(pDoc -> m_SelVolume);
				pFrame -> m_wndToolTab.m_VolPropDlg.LoadValues();
				if(pDoc -> m_Model -> m_skeleton)
					{pFrame -> m_wndToolTab.m_VolPropDlg.BuildBoneList(pDoc -> m_Model -> m_skeleton -> m_bonelist);}
			}
			break;
			case IT_OBSTACLE:
			{
				pDoc -> m_SelObstacle = new char[strlen(szTemp) + 1];
				strcpy(pDoc -> m_SelObstacle, szTemp);
			}
			break;
			case IT_AREA:
			{
				pDoc -> m_SelArea = new char[strlen(szTemp) + 1];
				strcpy(pDoc -> m_SelArea, szTemp);
			}
			break;
		}
	}
	pDoc -> UpdateAllViews(NULL);
	*pResult = 0;
}

BOOL CModelTree::PreTranslateMessage(MSG* pMsg)
{
	BOOL b = FALSE;
	CTreeCtrl *pTreeCtrl = (CTreeCtrl *)GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrame -> GetActiveDocument();
	POINT point;
	CMenu menu;
	BOOL CanLoad = FALSE;
	if(pMsg -> message == WM_KEYDOWN)
	{
		if(pMsg -> wParam == VK_RETURN)
		{
			pMsg -> wParam = NULL;
			b = CDialog::PreTranslateMessage(pMsg);
			OnEdit();
			return b;
		}
		else
		if(pMsg -> wParam == VK_ESCAPE)
			{pMsg -> wParam = NULL;}
		else
		if(pMsg -> wParam == VK_SPACE)
		{
			hTreeItem = pTreeCtrl -> GetSelectedItem();
			OnCheckStatusChange(hTreeItem, pTreeCtrl);
		}
		else
		if(pMsg -> wParam == VK_DELETE)
		{
			hTreeItem = pTreeCtrl -> GetSelectedItem();
			if(hTreeItem)
			{
				DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
				switch(itemData)
				{
					case IT_BONE:
						{pDoc -> OnBoneDelete();}
					break;
					case IT_VOLUME:
						{pDoc -> OnVolDelete();}
					break;
				};
			}
		}
	}
	else
	if(pMsg -> message == WM_LBUTTONDBLCLK)
	{
		hTreeItem = pTreeCtrl -> GetSelectedItem();
		if(hTreeItem)
		{
			if(hTreeItem == hTreeItemCheckStatusChange)
			{
				hTreeItemCheckStatusChange = NULL;
				pMsg -> message = WM_LBUTTONDOWN;
			}
			else
			{
				DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
				switch(itemData)
				{
					case IT_BONE:
					case IT_VOLUME:
					{
						pMsg -> message = WM_LBUTTONDOWN;
						b = CDialog::PreTranslateMessage(pMsg);
						OnEdit();
					}
					break;
				};
			}
		}
	}
	else
	if(pMsg -> message == WM_RBUTTONDOWN)
	{
		GetCursorPos(&point);
		pTreeCtrl -> ScreenToClient(&point);
		HTREEITEM item = pTreeCtrl -> HitTest(point);
		pTreeCtrl -> SelectItem(item);
		GetCursorPos(&point);
		hTreeItem = pTreeCtrl -> GetSelectedItem();
		if(hTreeItem)
		{
			DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
			switch(itemData)
			{
				case IT_BONE:
				{
					#ifdef ALTERNATIVE_LANG
						VERIFY(menu.LoadMenu(IDR_BONE_MENU_AL));
					#else
						VERIFY(menu.LoadMenu(IDR_BONE_MENU));
					#endif
					CanLoad = TRUE;
				}
				break;
				case IT_VOLUME:
				{
					#ifdef ALTERNATIVE_LANG
						VERIFY(menu.LoadMenu(IDR_VOLUME_MENU_AL));
					#else
						VERIFY(menu.LoadMenu(IDR_VOLUME_MENU));
					#endif
					CanLoad = TRUE;
				}
				break;
				case IT_OBSTACLE:
				break;
				case IT_AREA:
				break;
				case IT_BONETOP:
				{
					#ifdef ALTERNATIVE_LANG
						VERIFY(menu.LoadMenu(IDR_BONE_TOP_AL));
					#else
						VERIFY(menu.LoadMenu(IDR_BONE_TOP));
					#endif
					CanLoad = TRUE;
				}
				break;
				case IT_VOLUMETOP:
				{
					#ifdef ALTERNATIVE_LANG
						VERIFY(menu.LoadMenu(IDR_VOLUME_TOP_AL));
					#else
						VERIFY(menu.LoadMenu(IDR_VOLUME_TOP));
					#endif
					CanLoad = TRUE;
				}
				break;
				case IT_OBSTACLETOP:
				break;
				case IT_AREATOP:
				break;
			}
			if(CanLoad)
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				ASSERT(pPopup != NULL);
				b = CDialog::PreTranslateMessage(pMsg);
				if(itemData == IT_BONE)
				{
					CBone *pFindBone = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(hTreeItem);
					if(pFindBone && pFindBone -> m_LODView && pFindBone -> sub)
					{
						#ifdef ALTERNATIVE_LANG
							pPopup -> EnableMenuItem(IDM_BONE_INSERT_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COPY_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COPY_BRANCH_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_MATRIX_COPY_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_PARAMETERS_COPY_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_PASTE_AS_NEXT_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_PASTE_AS_INTERNAL_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_MATRIX_PASTE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_PARAMETERS_PASTE_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_EDIT_CUT_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COLLAPSE_BRANCH_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_ADD_VOLUME_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_DELETE_PLY_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_ADD_LOD_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_EXPAND_BRANCH_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_BRANCH_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_COLLISION_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_BRANCH_AND_COLLISION_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_BRANCH_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_COLLISION_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_BRANCH_AND_COLLISION_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_ADD_BONE_TO_ANM_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_DEL_BONE_FROM_ANM_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						#else
							pPopup -> EnableMenuItem(IDM_BONE_INSERT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COPY_BRANCH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_MATRIX_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_PARAMETERS_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_PASTE_AS_NEXT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_PASTE_AS_INTERNAL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_MATRIX_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(BONE_PARAMETERS_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_EDIT_CUT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_COLLAPSE_BRANCH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_ADD_VOLUME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_DELETE_PLY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_ADD_LOD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_EXPAND_BRANCH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_BRANCH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_COLLISION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_SHOW_BRANCH_AND_COLLISION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_BRANCH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_COLLISION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_BONE_HIDE_BRANCH_AND_COLLISION, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_ADD_BONE_TO_ANM, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_DEL_BONE_FROM_ANM, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						#endif
					}
					else
					if(!pDoc -> m_AnimBone)
					{
						#ifdef ALTERNATIVE_LANG
							pPopup -> EnableMenuItem(ID_ADD_BONE_TO_ANM_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_DEL_BONE_FROM_ANM_AL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						#else
							pPopup -> EnableMenuItem(ID_ADD_BONE_TO_ANM, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
							pPopup -> EnableMenuItem(ID_DEL_BONE_FROM_ANM, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
						#endif
					}
				}
				pPopup -> TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
			}
		}
	}
	else
	if(pMsg -> message == WM_LBUTTONDOWN)
	{
		UINT flags = 0;
		GetCursorPos(&point);
		pTreeCtrl -> ScreenToClient(&point);
		HTREEITEM item = pTreeCtrl -> HitTest(point, &flags);
		if(flags & TVHT_ONITEMSTATEICON)
			{pTreeCtrl -> SelectItem(item);}
		GetCursorPos(&point);
		hTreeItem = pTreeCtrl -> GetSelectedItem();
	}
	else
	if(pMsg -> message == WM_MBUTTONDOWN)
	{
		GetCursorPos(&point);
		pTreeCtrl -> ScreenToClient(&point);
		HTREEITEM item = pTreeCtrl -> HitTest(point);
		UINT state;
		if(pTreeCtrl -> GetChildItem(item))
		{
			if((state = pTreeCtrl -> GetItemState(item, TVIS_EXPANDED)) & TVIS_EXPANDED)
				{pTreeCtrl -> Expand(item, TVE_COLLAPSE);}
			else
				{pTreeCtrl -> Expand(item, TVE_EXPAND);}
			pTreeCtrl -> SelectItem(item);
			GetCursorPos(&point);
			hTreeItem = pTreeCtrl -> GetSelectedItem();
		} 
	}
	else
		{b = CDialog::PreTranslateMessage(pMsg);}
	return b;
}

void CModelTree::OnEdit()
{
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrame -> GetActiveDocument();
	CTreeCtrl *pTreeCtrl = (CTreeCtrl *)GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	hTreeItem = pTreeCtrl -> GetSelectedItem();
	pDoc -> hSelTreeItem = hTreeItem;
	if(hTreeItem != NULL)
	{
		DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
		switch(itemData)
		{
			case IT_BONE:
				{pDoc -> OnBoneProp();}
			break;
			case IT_VOLUME:
				{pDoc -> OnVolProp();}
			break;
		}
	}
}

void CModelTree::OnNMClickModeltree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTreeCtrl *pTreeCtrl = (CTreeCtrl *)GetDlgItem(IDC_MODELTREE);
	HTREEITEM hTreeItem;
	POINT point;
	UINT flags = 0;
	hTreeItem = pTreeCtrl -> GetSelectedItem();
	GetCursorPos(&point);
	pTreeCtrl -> ScreenToClient(&point);
	HTREEITEM item = pTreeCtrl -> HitTest(point, &flags);
	if(flags & TVHT_ONITEMSTATEICON)
		{OnCheckStatusChange(hTreeItem, pTreeCtrl);}
	else
		{hTreeItemCheckStatusChange = NULL;}
	*pResult = 0;
}

void CModelTree::OnCheckStatusChange(HTREEITEM hTreeItem, CTreeCtrl *pTreeCtrl)
{
	if(hTreeItem)
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		CSOEditDoc *pDoc = (CSOEditDoc *)pFrame -> GetActiveDocument();
		DWORD itemData = pTreeCtrl -> GetItemData(hTreeItem);
		hTreeItemCheckStatusChange = hTreeItem;
		switch(itemData)
		{
			case IT_BONE:
			{
				CBone *pFindBone = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(hTreeItem);
				if(pFindBone)
				{
					if(!pTreeCtrl -> GetCheck(hTreeItem) != pFindBone -> m_tree_check)
					{
						pFindBone -> m_tree_check = !pFindBone -> m_tree_check;
						pDoc -> UpdateAllViews(NULL);
					}
				}
			}
			break;
			case IT_VOLUME:
			{
				CVolume *pFindVolume = pDoc -> m_Model -> m_VolumeList -> FindVolumeByTreeID(hTreeItem);
				if(pFindVolume)
				{
					if(!pTreeCtrl -> GetCheck(hTreeItem) != pFindVolume -> m_tree_check)
					{
						pFindVolume -> m_tree_check = !pFindVolume -> m_tree_check;
						pDoc -> UpdateAllViews(NULL);
					}
				}
			}
			break;
		};
	}
}

BOOL CModelTree::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_MT_1);
		pWnd -> SetWindowText("Skeleton, Volumes, Obstacles and Areas");
	#endif
 	return TRUE;
}