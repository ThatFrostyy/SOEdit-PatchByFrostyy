// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_SOEDIT_H__322AE4A2_BBE9_4379_9F9E_F02F32F47E70__INCLUDED_)
#define AFX_SOEDIT_H__322AE4A2_BBE9_4379_9F9E_F02F32F47E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSOEditApp:
// See SOEdit.cpp for the implementation of this class
//

class CSOEditApp : public CWinApp
{
public:
	CSOEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSOEditApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL
	bool Idle_ON;
// Implementation
	//{{AFX_MSG(CSOEditApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOEDIT_H__322AE4A2_BBE9_4379_9F9E_F02F32F47E70__INCLUDED_)
