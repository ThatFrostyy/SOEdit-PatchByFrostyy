// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_CMODELTREE_H__87C7375C_4D2C_4101_808D_4E126541D26F__INCLUDED_)
#define AFX_CMODELTREE_H__87C7375C_4D2C_4101_808D_4E126541D26F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CModelTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelTree dialog

class CModelTree : public CDialog
{
// Construction
public:
	CModelTree(CWnd* pParent = NULL);   // standard constructor
 	BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	//{{AFX_DATA(CModelTree)
	enum { IDD = IDD_MODEL_TREE };
	CTreeCtrl	m_ModelTree;
	//}}AFX_DATA


// Overrides
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelTree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	HTREEITEM hTreeItemCheckStatusChange;

	// Generated message map functions
	//{{AFX_MSG(CModelTree)
	afx_msg void OnSelchangedModeltree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnNMClickModeltree(NMHDR *pNMHDR, LRESULT *pResult);
	void OnCheckStatusChange(HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMODELTREE_H__87C7375C_4D2C_4101_808D_4E126541D26F__INCLUDED_)
