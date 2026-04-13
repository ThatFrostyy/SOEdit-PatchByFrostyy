// setup_window.cpp: файл реализации
//

#include "stdafx.h"
#include "SOEdit.h"
#include "setup_window.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

setup_window::setup_window(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_SETUP, pParent)
{
	m_lamp = m_track = m_ram = m_ok = false;
	m_old_author = m_old_author_ply = m_old_author_vol = m_old_date = m_old_file = m_author = "";
	m_fov = 45;
}

setup_window::~setup_window()
	{}

void setup_window::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_OLD_AUTHOR, m_old_author);
	DDX_Text(pDX, IDC_OLD_PLY_FILE, m_old_author_ply);
	DDX_Text(pDX, IDC_OLD_VOL_FILE, m_old_author_vol);
	DDX_Text(pDX, IDC_OLD_TIME, m_old_date);
	DDX_Text(pDX, IDC_OLD_FILE, m_old_file);
	DDX_Text(pDX, IDC_YOUR_NAME, m_author);
	DDX_Check(pDX, IDC_VIEW_LAMP_MODEL, m_lamp);
	DDX_Check(pDX, IDC_VIEW_TRACK_VOL, m_track);
	DDX_Check(pDX, IDC_VIEW_RAM_VOL, m_ram);
	DDX_Text(pDX, IDC_FOV_EDIT, m_fov);

	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(setup_window, CDialog)
	ON_BN_CLICKED(IDC_VIEW_RAM_VOL, &setup_window::OnBnClickedViewRamVol)
	ON_BN_CLICKED(IDC_VIEW_TRACK_VOL, &setup_window::OnBnClickedViewTrackVol)
	ON_BN_CLICKED(IDC_VIEW_LAMP_MODEL, &setup_window::OnBnClickedViewLampModel)
END_MESSAGE_MAP()
// Обработчики сообщений setup_window

void setup_window::OnBnClickedViewRamVol()
	{m_ram = IsDlgButtonChecked(IDC_VIEW_RAM_VOL);}


void setup_window::OnBnClickedViewTrackVol()
	{m_track = IsDlgButtonChecked(IDC_VIEW_TRACK_VOL);}


void setup_window::OnBnClickedViewLampModel()
	{m_lamp = IsDlgButtonChecked(IDC_VIEW_LAMP_MODEL);}

BOOL setup_window::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_FOV_EDIT);
	CString st;
	NMHDR *nmHdr = (NMHDR *)lParam;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)nmHdr;
	if(nmHdr -> code == UDN_DELTAPOS)
	{
		if(wParam == IDC_FOV_EDITOR)
		{
			m_fov -= pNMUpDown -> iDelta;
			if(m_fov < 20)
				{m_fov = 20;}
			if(m_fov > 120)
				{m_fov = 120;}
			st.Format("%d", m_fov);
			pEdit -> SetWindowText(st);
			//Смотри строку ниже.
			//Смотри строку выше.
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL setup_window::OnInitDialog()
{
 	CDialog::OnInitDialog();
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Trash collector");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_1);
		pWnd -> SetWindowText("Original author:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_2);
		pWnd -> SetWindowText("PLY-files:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_3);
		pWnd -> SetWindowText("VOL-files:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_4);
		pWnd -> SetWindowText("You:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_5);
		pWnd -> SetWindowText("Date:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_6);
		pWnd -> SetWindowText("File:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_7);
		pWnd -> SetWindowText("*Your advertisement could be here* (no)");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_8);
		pWnd -> SetWindowText("Copyrights:");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_SW_9);
		pWnd -> SetWindowText("Other:");
		pWnd = (CWnd *)GetDlgItem(IDC_VIEW_RAM_VOL);
		pWnd -> SetWindowText("Display RAM-volume");
		pWnd = (CWnd *)GetDlgItem(IDC_VIEW_TRACK_VOL);
		pWnd -> SetWindowText("Display track volumes");
		pWnd = (CWnd *)GetDlgItem(IDC_VIEW_LAMP_MODEL);
		pWnd -> SetWindowText("Display the light bulb model");
		pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#endif
 	return TRUE;
}