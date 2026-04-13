// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "SOEdit.h"

#include "MainFrm.h"
#include "SOEditDoc.h"
#include "SOEditView.h"

#include "CAnimFrame.h"

#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD lasttime = 0, currtime = 0, lapsed = 0;

// Need to get rid of these globals
char  szFileName[_MAX_PATH] = {0};

char *GetModelPath()
	{return szFileName;}

void FixPathDelim(char *s)
{
	for(unsigned int i = 0; i < strlen(s); i++)
	{
		if(s[i] == '\\')
			{s[i] = '/';}
	}
}
void InvertFixPathDelim(char *s)
{
	for(unsigned int i = 0; i < strlen(s); i++)
	{
		if(s[i] == '/')
			{s[i] = '\\';}
	}
}
void ÑommaFixer(char *s)
{
	for(unsigned int i = 0; i < strlen(s); i++)
	{
		if(s[i] == ',')
			{s[i] = '.';}
	}
}
void ForbiddenSymbolFixer(char *s)
{
	unsigned int i = 0, j = 0;
	bool tchk = false, e = false;
	int len = strlen(s);
	for(;i < len; i++)
	{
		s[j] = s[i];
		if((s[i] >= '0' && s[i] <= '9') || (s[i] == '-' && i == 0) || (s[i] == '.' && i > 0 && !tchk) || ((s[i] == 'e' || s[i] == 'E') && i > 0 && !e))
			{j++;}
		if(s[i] == '.')
			{tchk = true;}
		if(s[i] == 'e' || s[i] == 'E')
			{e = true;}
	}
	s[j] = '\0';
}

int isFloat(char *s)
{
	if(*s == '-' || *s == '+')
	{
		if(!isdigit(*++s))
			{return 0;}
	}
	if(!isdigit(*s))
		{return 0;}
	while(isdigit(*s))
		{s++;}
	if(*s == '.')
	{
		*s++;
		//if(!isdigit(*++s))
		//	{return 0;}
	}
	while(isdigit(*s))
		{s++;}
	if(*s == 'e' || *s == 'E')
	{
		s++;
		if(*s == '+' || *s == '-')
		{
			s++;
			if(!isdigit(*s))
				{return 0;}
		}
		else
		if(!isdigit(*s))
			{return 0;}
	}
	while(isdigit(*s))
		{s++;}
	if(*s == '\0')
		{return 1;}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditApp

BEGIN_MESSAGE_MAP(CSOEditApp, CWinApp)
	//{{AFX_MSG_MAP(CSOEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_ABOUT_AL, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_AL, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_PRINT_SETUP_AL, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSOEditApp construction

CSOEditApp::CSOEditApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	Idle_ON = false;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSOEditApp object
CSOEditApp theApp;
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSOEditApp initialization

BOOL CSOEditApp::InitInstance()
{
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CSingleDocTemplate* pDocTemplate;
	#ifdef ALTERNATIVE_LANG
		pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME_AL,
		RUNTIME_CLASS(CSOEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSOEditView));
	#else
		pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSOEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSOEditView));
	#endif
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line
	if(!ProcessShellCommand(cmdInfo))
		{return FALSE;}
	// The one and only window has been initialized, so show and update it.
	m_pMainWnd -> ShowWindow(SW_SHOW);
	m_pMainWnd -> UpdateWindow();
	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrameWnd -> GetActiveDocument();
	pDoc -> OnClearAll(false);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
 	BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSOEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditApp message handlers

BOOL CSOEditApp::OnIdle(LONG lCount)
{
	currtime = timeGetTime();
	lapsed = currtime - lasttime;
	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pFrameWnd -> GetActiveDocument();
	if(pDoc)
	{
		if(pDoc -> m_Animate && pDoc -> m_AnimBone)
		{
			Idle_ON = true;
			CWnd *pWnd = (CWnd *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_SPEED);
			char szWindowText[_MAX_PATH];
			pWnd -> GetWindowText(szWindowText, _MAX_PATH);
			ÑommaFixer(szWindowText);
			ForbiddenSymbolFixer(szWindowText);
			double t_speed = 1;
			bool inv = false, end = false;
            if(isFloat(szWindowText))
				{t_speed = (float)atof(szWindowText);}
			if(t_speed > 10)
				{t_speed = 10;}
			else
			if(t_speed < -10)
				{t_speed = -10;}
			if(t_speed < 0)
				{inv = true;}
			pDoc -> m_Speed = t_speed;
			// calculate which frame should be visible
			int frmtime = ((int)((float)1000 / max(t_speed, -t_speed)) / 30);
			int frames = lapsed / frmtime;
			if(t_speed >= -0.001f && t_speed <= 0.001f)
				{frames = 0;}
			if(inv)
			{
				pDoc -> m_Frame -= frames;
				if(pDoc -> m_Frame < 0)
					{pDoc -> m_Frame = pDoc -> m_AnimBone -> m_FrameCnt + pDoc -> m_Frame; end = true;}
			}
			else
			{
				pDoc -> m_Frame += frames;
				if(pDoc -> m_Frame >= pDoc -> m_AnimBone -> m_FrameCnt)
					{pDoc -> m_Frame =- pDoc -> m_AnimBone -> m_FrameCnt; end = true;}
			}
			if(!pDoc -> m_AnimBone)
			{
				pDoc -> m_Frame = 0;
				pDoc -> m_Animate = false;
				pDoc -> Anm_and_skeleton_editing_prohibited = false;
				Idle_ON = false;
				CWnd *pWnd = (CWnd *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
				sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
				pWnd -> SetWindowText(szWindowText);
				CSliderCtrl *pSlider = (CSliderCtrl *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
				pSlider -> SetPos(pDoc -> m_Frame);
			}
			if(!pDoc -> ViewWorks[0] && !pDoc -> ViewWorks[1] && !pDoc -> ViewWorks[2] && !pDoc -> ViewWorks[3])
			{
				if(pDoc -> m_AnimBone && pDoc -> m_Frame < pDoc -> m_AnimBone -> m_FrameCnt && pDoc -> m_Frame != pDoc -> old_frm)
				{
					pDoc -> old_frm = pDoc -> m_Frame;
					CWnd *pWnd = (CWnd *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
					sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
					pWnd -> SetWindowText(szWindowText);
					CSliderCtrl *pSlider = (CSliderCtrl *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
					pSlider -> SetPos(pDoc -> m_Frame);
					pDoc -> UpdateAllViews(NULL);
				}
			}
			if(pDoc -> m_AnimBone && end)
			{
				if(!pDoc -> ViewWorks[0] && !pDoc -> ViewWorks[1] && !pDoc -> ViewWorks[2] && !pDoc -> ViewWorks[3])
				{
					if(!pDoc -> m_Loop)
					{
						pDoc -> m_Animate = false;
						pDoc -> Anm_and_skeleton_editing_prohibited = false;
						Idle_ON = false;
					}
					else
					{
						if(inv)
							{pDoc -> m_Frame = pDoc -> m_AnimBone -> m_FrameCnt - 1;}
						else
							{pDoc -> m_Frame = 0;}
					}
				}
				CWnd *pWnd = (CWnd *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_FRAMENO);
				sprintf(szWindowText, "%d", pDoc -> m_Frame + 1);
				pWnd -> SetWindowText(szWindowText);
				CSliderCtrl *pSlider = (CSliderCtrl *)pFrameWnd -> m_wndAnimBox.GetDlgItem(IDC_ANIM_SLIDER);
				pSlider -> SetPos(pDoc -> m_Frame);
			}
			if(t_speed >= -0.001f && t_speed <= 0.001f)
				{lasttime = currtime;}
			else
				{lasttime += frames * frmtime;}
			pDoc -> OnModEditing();
		}
		else
		{
			lasttime = currtime;
			Idle_ON = false;
		}
	}
	else
		{lasttime = currtime;}
	//Sleep(1000);
	//CWinApp::OnIdle(1);
	return Idle_ON;
}

BOOL CAboutDlg::OnInitDialog()
{
 	CDialog::OnInitDialog();
	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	CString StrVer;
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("About SOEdit");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_ABO_1);
		StrVer.Format("SOEdit version 0.50 from Brian (+patch v0.%dm%d from TonaTor)", LOBYTE((pFrameWnd -> ProgVer) >> 8), LOBYTE(pFrameWnd -> ProgVer));
		pWnd -> SetWindowText(StrVer);
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_ABO_2);
		pWnd -> SetWindowText("Men Of War Object Editor\n(Old name its: \"\"Soldiers Object Editor\"\")\nThis is an unofficial utility.\nThe program is still in development.");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_ABO_3);
		pWnd -> SetWindowText("Copyright (C) 2004-2022, MoW Community");
		pWnd = (CWnd *)GetDlgItem(IDC_STATIC_ABO_4);
		pWnd -> SetWindowText("Thanks to Man232 and GhostNT for cooperation");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#else
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_ABO_1);
		StrVer.Format("SOEdit âåðñèè 0.50 îò Brian (+patch v0.%dm%d îò TonaTor)", LOBYTE((pFrameWnd -> ProgVer) >> 8), LOBYTE(pFrameWnd -> ProgVer));
		pWnd -> SetWindowText(StrVer);
	#endif
	HICON hicon = NULL;
	int x = 10, y = 30, bpp = 4, icon_len = 0;
	icon_len = 40 + ((x * y) * bpp);
	BYTE *icon_bmp = new BYTE[icon_len], *bPtr = 0;
	memset(icon_bmp, 255, icon_len);
	BITMAPINFOHEADER BH;
	BH.biSize = 40;
	BH.biWidth = x;
	BH.biHeight = y;
	BH.biPlanes = 1;
	BH.biBitCount = 8 * bpp;
	BH.biCompression = BI_RGB;
	BH.biSizeImage = (x * y) * bpp;
	BH.biXPelsPerMeter = 0;
	BH.biYPelsPerMeter = 0;
	BH.biClrUsed = 0;
	BH.biClrImportant = 0;
	memcpy(icon_bmp, &BH, sizeof(BITMAPINFOHEADER));
	bPtr = icon_bmp + 40;
	DWORD color = 0xffffffff;
	for(int iy = 0; iy < 10; iy++)
	{
		for(int ix = 0; ix < x; ix++)
		{
			memcpy(bPtr, &color, sizeof(DWORD));
			bPtr += bpp;
		}
		if(iy == 0)
			{color = 0xffff0000;}
		if(iy == 5)
			{color = 0xff0000ff;}
		if(iy == 9)
			{color = 0xffffffff;}//êîððåêòèðîâêà
	}
	hicon = CreateIconFromResourceEx(icon_bmp, icon_len, TRUE, 0x00030000, 80, 30, LR_DEFAULTCOLOR);
	CStatic *pMyFlag = (CStatic *)GetDlgItem(IDC_MY_FLAG);
	pMyFlag -> SetIcon(hicon);
	delete icon_bmp;
 	return TRUE;
}