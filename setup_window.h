#pragma once
// Диалоговое окно setup_window

#include "stdafx.h"
#include "CSdl.h"
#include "general.h"

class setup_window : public CDialog
{
public:
	setup_window(CWnd* pParent = nullptr);   // стандартный конструктор
 	BOOL OnInitDialog();
	virtual ~setup_window();

// Данные диалогового окна
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETUP };
//#endif

	BOOL m_lamp;
	BOOL m_track;
	BOOL m_ram;
	BOOL m_ok;

	CString m_old_author;
	CString m_old_author_ply;
	CString m_old_author_vol;
	CString m_old_date;
	CString m_old_file;
	CString m_author;

	DWORD m_fov;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedViewRamVol();
	afx_msg void OnBnClickedViewTrackVol();
	afx_msg void OnBnClickedViewLampModel();
};
