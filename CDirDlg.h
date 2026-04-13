#if !defined(AFX_CDIRDLG_H__326C5324_6EE6_4543_A204_738242E642DD__INCLUDED_)
#define AFX_CDIRDLG_H__326C5324_6EE6_4543_A204_738242E642DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDirDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirDlg dialog


class CDirDlg : public CFileDialog
{
	DECLARE_DYNAMIC(CDirDlg)

public:
	CDirDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

    BOOL m_bDlgJustCameUp;

protected:
	//{{AFX_MSG(CDirDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDIRDLG_H__326C5324_6EE6_4543_A204_738242E642DD__INCLUDED_)
