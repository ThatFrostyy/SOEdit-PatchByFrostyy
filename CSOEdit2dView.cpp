// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "SOEdit.h"
#include "MainFrm.h"


#include "gl/gl.h"
#include "gl/glu.h"

#include "SOEditDoc.h"
#include "CSOEdit2dView.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CSOEdit2dView

IMPLEMENT_DYNCREATE(CSOEdit2dView, CView)

BEGIN_MESSAGE_MAP(CSOEdit2dView, CView)
	//{{AFX_MSG_MAP(CSOEdit2dView)
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_VIEW_FRONT, OnViewFront)
	ON_COMMAND(ID_VIEW_FRONT_AL, OnViewFront)
	ON_COMMAND(ID_VIEW_RIGHT, OnViewRight)
	ON_COMMAND(ID_VIEW_RIGHT_AL, OnViewRight)
	ON_COMMAND(ID_VIEW_TOP, OnViewTop)
	ON_COMMAND(ID_VIEW_TOP_AL, OnViewTop)
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_LEFT, OnViewLeft)
	ON_COMMAND(ID_VIEW_LEFT_AL, OnViewLeft)
	ON_COMMAND(ID_VIEW_BOTTOM, OnViewBottom)
	ON_COMMAND(ID_VIEW_BOTTOM_AL, OnViewBottom)
	ON_COMMAND(ID_VIEW_REAR, OnViewRear)
	ON_COMMAND(ID_VIEW_REAR_AL, OnViewRear)
	ON_COMMAND(ID_VIEW_CAMERA, OnViewCamera)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_BACKFACES, OnViewBackfaces)
	ON_COMMAND(ID_VIEW_BACKFACES_AL, OnViewBackfaces)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_COMMAND(ID_VIEW_WIREFRAME_AL, OnViewWireframe)
	ON_COMMAND(ID_VIEW_SHADED, OnViewShaded)
	ON_COMMAND(ID_VIEW_SHADED_AL, OnViewShaded)
	ON_COMMAND(ID_VIEW_COLORIZE, OnColorize)
	ON_COMMAND(ID_VIEW_COLORIZE_AL, OnColorize)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSOEdit2dView construction/destruction

BOOL CSOEdit2dView::InitGL()
{
	int  nMyPixelFormatID;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // struct size 
		1,                              // Version number
		PFD_DRAW_TO_WINDOW |    // Flags, draw to a window,
		PFD_SUPPORT_OPENGL |    // use OpenGL
		PFD_DOUBLEBUFFER,       // want double buffering
		PFD_TYPE_RGBA,          // RGBA pixel values
		24,                     // 32-bit color
		0, 0, 0, 0, 0, 0,       // RGB bits & shift sizes. Don't care about them
		0,                      // No alpha buffer info
		0,                      // shift bit ignored
		0,                      // no accumulation buffer
		0, 0, 0, 0,             // accum bits ignored
		16,                     // 16-bit depth buffer
		0,                      // No stencil buffer
		0,                      // No auxiliary buffers
		PFD_MAIN_PLANE,         // main layer
		0,                      // Reserved (must be 0)
		0, 0, 0                 // No layer, visible or damage masks
	};

	nMyPixelFormatID = ChoosePixelFormat(m_hDC, &pfd);

	if(nMyPixelFormatID == 0)
	{
		#ifdef ALTERNATIVE_LANG
			::MessageBoxA(this -> m_hWnd, "Unable to set pixel format", "ERROR: CSOEdit2dView::InitGL", MB_ICONHAND);
		#else
			::MessageBoxA(this -> m_hWnd, "Невозможно установить пиксельный формат", "ERROR: CSOEdit2dView::InitGL", MB_ICONHAND);
		#endif
		return(FALSE);
	}
	// catch errors here.
	// If nMyPixelFormat is zero, then there's
	// something wrong... most likely the window's
	// style bits are incorrect (in CreateWindow() )
	// or OpenGl isn't installed on this machine
	SetPixelFormat(m_hDC, nMyPixelFormatID, &pfd);
	m_hRC = wglCreateContext(m_hDC);
	//  This next code should be moved out of the OpenGL class and back into the view
	wglMakeCurrent(m_hDC, m_hRC);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	if(pFrameWnd)
		{glClearColor(pFrameWnd -> m_Colors[C_BG3D][0] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][1] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][2] / 255.0f, 1.0f);}
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glOrtho(0 - (m_glnWidth / 2), m_glnWidth / 2, m_glnHeight / 2, 0 - (m_glnHeight / 2), -2500, 2500.0);
	glViewport(0, 0, m_glnWidth, m_glnHeight);
	wglMakeCurrent(m_hDC, NULL);
	return TRUE;
}

void CSOEdit2dView::DrawGridGL()
{
    v3_t v1, v2;
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc* pDoc = GetDocument();
    if(pWnd)
		{glColor3f(pWnd -> m_Colors[C_GRID][0]/255.0f, pWnd -> m_Colors[C_GRID][1]/255.0f, pWnd -> m_Colors[C_GRID][2]/255.0f);}
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[m_y] = 1000.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[m_y] = -1000.0f;
    for(int x = -1000; x < 1000; x += pDoc -> m_GridSize)
	{
        v1[m_x] = (float)x;
        v2[m_x] = (float)x;
        glBegin(GL_LINES);
           glVertex3fv(v1);
           glVertex3fv(v2);
        glEnd();
	}
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[m_x] = 1000.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[m_x] = -1000.0f;
    for(int y = -1000; y < 1000; y += pDoc -> m_GridSize)
	{
        v1[m_y] = (float)y;
        v2[m_y] = (float)y;
        glBegin(GL_LINES);
           glVertex3fv(v1);
           glVertex3fv(v2);
        glEnd();
	}
}

void CSOEdit2dView::DrawOriginGL()
{
    v3_t v1, v2;
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    if(pWnd)
		{glColor3f(pWnd -> m_Colors[C_AXIS][0] / 255.0f, pWnd -> m_Colors[C_AXIS][1] / 255.0f, pWnd -> m_Colors[C_AXIS][2] / 255.0f);}
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[m_x] = 0.0f;
    v1[m_y] = 1000.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[m_x] = 0.0f;
    v2[m_y] = -1000.0f;
    glBegin(GL_LINES);
       glVertex3fv(v1);
       glVertex3fv(v2);
    glEnd();
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[m_x] = 1000.0f;
    v1[m_y] = 0.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[m_x] = -1000.0f;
    v2[m_y] = 0.0f;
    glBegin(GL_LINES);
       glVertex3fv(v1);
       glVertex3fv(v2);
    glEnd();
}

CSOEdit2dView::CSOEdit2dView()
{
    m_View = v_right;
    SetupView();
    m_Scale = 4;
    m_scales[0] = 0.07f;
    m_scales[1] = 0.08f;
    m_scales[2] = 0.09f;
    m_scales[3] = 0.1f;
    m_scales[4] = 0.2f;
    m_scales[5] = 0.3f;
    m_scales[6] = 0.4f;
    m_scales[7] = 0.5f;
    m_scales[8] = 0.6f;
    m_scales[9] = 0.7f;
    m_scales[10] = 0.8f;
    m_scales[11] = 0.9f;
    m_scales[12] = 1.0f;
    m_scales[13] = 1.1f;
    m_scales[14] = 1.2f;
    m_scales[13] = 1.3f;
    m_scales[14] = 1.4f;
    m_scales[15] = 1.5f;
    m_scales[16] = 1.6f;
    m_scales[17] = 1.8f;
    m_scales[18] = 1.9f;
    m_scales[19] = 2.0f;
    m_scales[20] = 2.1f;
    m_scales[21] = 2.2f;
    m_scales[22] = 2.3f;
    m_scales[23] = 2.4f;
    m_Point.x = 0;
    m_Point.y = 0;
    m_lButton = false;
    m_mButton = false;
    m_rButton = false;
    m_AmbientLight[0] = 1.0f;
    m_AmbientLight[1] = 1.0f;
    m_AmbientLight[2] = 1.0f;
    m_AmbientLight[3] = 1.0f;
    m_DiffuseLight[0] = 0.5f;
    m_DiffuseLight[1] = 0.5f;
    m_DiffuseLight[2] = 0.5f;
    m_DiffuseLight[3] = 1.0f;
    m_LightPos[0] = -50.0f;
    m_LightPos[1] = 50.0f;
    m_LightPos[2] = 100.0f;
    m_LightPos[3] = 1.0f;
    m_Camera.Position[0] = 0.0f;
    m_Camera.Position[1] = 0.0f;
    m_Camera.Position[2] = -150.0f;
    m_Camera.Orient[0] = 0.0f;
    m_Camera.Orient[1] = 0.0f;
    m_Camera.Orient[2] = 0.0f;
    m_LightMode = l_diffuse;
    m_BackFace = false;
	m_DrawMode = r_wire;
    m_BackDir = GL_CCW;
    m_xAdj = 0;
    m_yAdj = 0;
	DocIndx = 0;
	m_pDC = NULL;
	memset(full_mx, 0, sizeof(float) * 16);
	YBlock = XBlock = false;
}

CSOEdit2dView::~CSOEdit2dView()
{
    if(m_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hRC);
	}
}

void CSOEdit2dView::SetView(int viewmode, int indx)
{
    m_View = viewmode;
	DocIndx = indx;
    SetupView();
    InvalidateRect(false);
}


BOOL CSOEdit2dView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);
}

void CSOEdit2dView::SetupView()
{
    switch(m_View)
	{
        case v_front:
		{
			m_x = 0;
			m_y = 1;
			m_invx = 1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Front view ";
			#else
				m_szCaption = " Вид спереди ";
			#endif
			m_glx = 1;
			m_gly = 2;
			m_glz = 0;
			m_xaxis = 0;
			m_Camera.Position[0] = 0.0f;
			m_Camera.Position[1] = 27.0f;
			m_Camera.Position[2] = -150.0f;
			m_Camera.Orient[0] = 0.0f;
			m_Camera.Orient[1] = 0.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_rear:
		{
			m_x = 0;
			m_y = 1;
			m_invx = -1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Rear view ";
			#else
				m_szCaption = " Вид сзади ";
			#endif
			m_glx = 1;
			m_gly = 2;
			m_glz = 0;
			m_xaxis = 0;
			m_Camera.Position[0] = 0.0f;
			m_Camera.Position[1] = 27.0f;
			m_Camera.Position[2] = 150.0f;
			m_Camera.Orient[0] = 0.0f;
			m_Camera.Orient[1] = 180.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_bottom:
		{
			m_x = 0;
			m_y = 2;
			m_invx = 1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Bottom view ";
			#else
				m_szCaption = " Вид снизу ";
			#endif
			m_glx = 1;
			m_gly = 0;
			m_glz = 2;
			m_xaxis = 2;
			m_Camera.Position[0] = 0.0f;
			m_Camera.Position[1] = -150.0f;
			m_Camera.Position[2] = 0.0f;
			m_Camera.Orient[0] = 90.0f;
			m_Camera.Orient[1] = 0.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_top:
		{
			m_x = 2;
			m_y = 0;
			m_invx = -1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Top view ";
			#else
				m_szCaption = " Вид сверху ";
			#endif
			m_glx = 1;
			m_gly = 0;
			m_glz = 2;
			m_xaxis = 2;
			m_Camera.Position[0] = 0.0f;
			m_Camera.Position[1] = 150.0f;
			m_Camera.Position[2] = 0.0f;
			m_Camera.Orient[0] = -90.0f;
			m_Camera.Orient[1] = 180.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_left:
		{
			m_x = 2;
			m_y = 1;
			m_invx = 1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Left view ";
			#else
				m_szCaption = " Вид слева ";
			#endif
			m_glx = 0;
			m_gly = 2;
			m_glz = 1;
			m_xaxis = 1;
			m_Camera.Position[0] = 150.0f;
			m_Camera.Position[1] = 27.0f;
			m_Camera.Position[2] = 0.0f;
			m_Camera.Orient[0] = 0.0f;
			m_Camera.Orient[1] = 90.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_right:
		{
			m_x = 2;
			m_y = 1;
			m_invx = -1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Right view ";
			#else
				m_szCaption = " Вид справа ";
			#endif
			m_glx = 0;
			m_gly = 2;
			m_glz = 1;
			m_xaxis = 1;
			m_Camera.Position[0] = -150.0f;
			m_Camera.Position[1] = 27.0f;
			m_Camera.Position[2] = 0.0f;
			m_Camera.Orient[0] = 0.0f;
			m_Camera.Orient[1] = -90.0f;
			m_Camera.Orient[2] = 0.0f;
		}
		break;
        case v_camera:
		{
			m_x = 0;
			m_y = 1;
			m_invx = 1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " Camera ";
			#else
				m_szCaption = " Камера ";
			#endif
		}
		break;
        default:
		{
			m_x = 0;
			m_y = 1;
			m_invx = 1;
			m_invy = 1;
			#ifdef ALTERNATIVE_LANG
				m_szCaption = " By default ";
			#else
				m_szCaption = " По умолчанию ";
			#endif
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSOEdit2dView drawing

void CSOEdit2dView::RenderGL(CDC *pDC)
{
	CSOEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	wglMakeCurrent(m_hDC, m_hRC);
	// Setup the backface polygon culling
	if(m_BackFace)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(m_BackDir);
		glCullFace(GL_BACK);
	}
	else
		{glDisable(GL_CULL_FACE);}
	switch(m_DrawMode)
	{
		case r_wire:
		{
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK, GL_LINE);
			glDisable(GL_LIGHTING);
		}
		break;
		case r_flat:
		case r_smooth:
		case r_texture:
		{
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
			/*glEnable(GL_LIGHTING);
			glLightfv(GL_LIGHT0, GL_AMBIENT, m_AmbientLight);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, m_DiffuseLight);
			glLightfv(GL_LIGHT0, GL_POSITION, m_LightPos);
			glEnable(GL_LIGHT0);*/
		}
		break;
	}
	glDisable(GL_DEPTH_TEST);
	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	if(pFrameWnd)
		{glClearColor(pFrameWnd -> m_Colors[C_BG2D][0] / 255.0f, pFrameWnd -> m_Colors[C_BG2D][1] / 255.0f, pFrameWnd -> m_Colors[C_BG2D][2] / 255.0f, 1.0f);}
    // Clear the color and depth buffers.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    m_reference[0] = 0.0f;
    m_reference[1] = 0.0f;
    m_reference[2] = 0.0f;

	int xfactor = (int)((float)(m_glnWidth / 2) * m_scales[m_Scale]), yfactor = (int)((float)(m_glnHeight / 2) * m_scales[m_Scale]);
	glOrtho((0 - xfactor) + m_xAdj, xfactor + m_xAdj, yfactor + m_yAdj, (0 - yfactor) + m_yAdj, -5000, 5000.0);
    glViewport(0, 0, m_glnWidth, m_glnHeight);

	// set up a projection matrix to fill the client window
    // a perspective-view matrix...
    glRotatef(m_Camera.Orient[0], 1.0f, 0.0f, 0.0f);
    glRotatef(m_Camera.Orient[1], 0.0f, 1.0f, 0.0f);
    glRotatef(m_Camera.Orient[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(m_Camera.Position[0], m_Camera.Position[1], m_Camera.Position[2]);

    // Define the modelview transformation.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glPushMatrix();
	if(pDoc -> m_grid)
		{DrawGridGL();}
	DrawOriginGL();
	glEnable(GL_DEPTH_TEST);
	glRotatef(-90.f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.f, 1.0f, 0.0f, 0.0f);
	// a perspective-view matrix...
	if(pDoc -> m_Model && !pDoc -> fix_mx_ori)
	{
		CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
		if((pDoc -> m_Model -> m_ObstacleList) && (pWnd -> m_ViewObst) && !pFrameWnd -> m_CollisionSide)
		{
			bool selected;
			CObb2 *pObj = pDoc -> m_Model -> m_ObstacleList -> GetFirst();
			while(pObj)
			{
				selected = false;
				if(pDoc -> m_SelObstacle)
				{
					if(!stricmp(pObj -> m_Name, pDoc -> m_SelObstacle))
						{selected = true;}
				}
				if(selected)
					{glColor3f(pWnd -> m_Colors[C_OBSTACLES][0] / 255.0f, pWnd -> m_Colors[C_OBSTACLES][1] / 255.0f, pWnd -> m_Colors[C_OBSTACLES][2] / 255.0f);}
				else
					{glColor3f(pWnd -> m_Colors[C_OBSTACLE][0] / 255.0f, pWnd -> m_Colors[C_OBSTACLE][1] / 255.0f, pWnd -> m_Colors[C_OBSTACLE][2] / 255.0f);}
				DrawObb2GL(pObj);
				pObj = pObj -> next;
			}
		}
		if((pDoc -> m_Model -> m_AreaList) && (pWnd -> m_ViewArea) && !pFrameWnd -> m_CollisionSide)
		{
			bool selected;
			CObb2 *pObj = pDoc -> m_Model -> m_AreaList -> GetFirst();
			while(pObj)
			{
				selected = false;
				if(pDoc -> m_SelArea)
				{
					if(!stricmp(pObj -> m_Name, pDoc -> m_SelArea))
						{selected = true;}
				}
				if(selected)
					{glColor3f(pWnd -> m_Colors[C_AREAS][0] / 255.0f, pWnd -> m_Colors[C_AREAS][1] / 255.0f, pWnd -> m_Colors[C_AREAS][2] / 255.0f);}
				else
					{glColor3f(pWnd -> m_Colors[C_AREA][0] / 255.0f, pWnd -> m_Colors[C_AREA][1] / 255.0f, pWnd -> m_Colors[C_AREA][2] / 255.0f);}
				DrawObb2GL(pObj);
				pObj = pObj -> next;
			}
		}
		if(pDoc -> m_Model -> m_skeleton)
		{
			if(pDoc -> m_Model -> m_skeleton -> m_bonelist)
			{
				DrawBoneGL(pDoc -> m_Model -> m_skeleton -> m_bonelist);
				CBone *Nlist = pDoc -> m_Model -> m_skeleton -> m_bonelist -> next;
				while(Nlist)
				{
					DrawBoneGL(Nlist);
					Nlist = Nlist -> next;
				}
			}
		}
	}
	glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Swap the rendering buffers...
    glFinish();
    SwapBuffers(m_hDC);
    wglMakeCurrent(NULL, NULL);
}

void CSOEdit2dView::OnDraw(CDC* pDC)
{
    CSOEditDoc* pDoc = GetDocument();
	if(pDoc -> ResetCamView[DocIndx])
	{
		m_xAdj = 0;
		m_yAdj = 0;
		pDoc -> ResetCamView[DocIndx] = false;
		RenderGL(pDC);
	}
	if(!pDoc -> ViewWorks[DocIndx])
	{
		pDoc -> ViewWorks[DocIndx] = 1;
		RenderGL(pDC);
		if(pDC)
		{
			pDC -> SetBkColor(0xCFCFCF);
			pDC -> SetBkMode(OPAQUE);
			pDC -> SetTextColor(0x3F3F3F);
			pDC -> TextOut(0, 0, m_szCaption);
		}
		pDoc -> ViewWorks[DocIndx] = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSOEdit2dView printing

BOOL CSOEdit2dView::OnPreparePrinting(CPrintInfo* pInfo)
   {
    // default preparation
    return DoPreparePrinting(pInfo);
   }

void CSOEdit2dView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
   {
    // TODO: add extra initialization before printing
   }

void CSOEdit2dView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
   {
    // TODO: add cleanup after printing
   }

/////////////////////////////////////////////////////////////////////////////
// CSOEdit2dView diagnostics

#ifdef _DEBUG
void CSOEdit2dView::AssertValid() const
	{CView::AssertValid();}

void CSOEdit2dView::Dump(CDumpContext& dc) const
	{CView::Dump(dc);}

CSOEditDoc* CSOEdit2dView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSOEditDoc)));
	return (CSOEditDoc*)m_pDocument;
}
#endif //_DEBUG

void CSOEdit2dView::UpdateSelected(CPoint point)
{
	CSOEditDoc* pDoc = GetDocument();
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	float deltax = 0, deltay = 0;
	if(m_View == v_left || m_View == v_right)
		{deltax = (float)m_Point.x - point.x;}
	else
		{deltax = (float)point.x - m_Point.x;}
	deltay = (float)m_Point.y - point.y;
	if(pDoc)
	{
		ASSERT_VALID(pDoc);
		if(pDoc -> m_SelBone)
		{
			CBone *pSelect = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(pDoc -> hSelTreeItem);
			if(!pSelect)
				{return;}
			if(!pSelect-> sub)
			{
				switch(pWnd -> m_mode)
				{
					case m_move:
					{
						if(!XBlock)
							{pSelect -> m_Translations[m_glx] -= (deltax * m_scales[m_Scale]) * (float)m_invx;}
						if(!YBlock)
							{pSelect -> m_Translations[m_gly] += (deltay * m_scales[m_Scale]) * (float)m_invy;}
					}
					break;
					case m_scale:
					if(pSelect -> m_VolumeView)
					{
						if(!XBlock)
							{pSelect -> m_Scales[m_glx] += ((deltax * m_scales[m_Scale]) * (float)m_invx) * 0.025f;}
						if(!YBlock)
							{pSelect -> m_Scales[m_gly] += ((deltay * m_scales[m_Scale]) * (float)m_invy) * 0.025f;}
					}
					break;
					case m_rotate:
					{
						if(!XBlock)
							{pSelect -> m_Rotations[m_xaxis] -= (deltax * m_scales[m_Scale]) * (float)m_invx;}
					}
					break;
					case m_select:
					break;
				}
			}
		}
		if(pDoc -> m_SelVolume)
		{
			CVolume *pSelect = pDoc -> m_Model -> m_VolumeList -> FindVolumeByTreeID(pDoc -> hSelTreeItem);
			if(pSelect == NULL)
				{return;}
			switch(pWnd -> m_mode)
			{
				case m_move:
				{
					if(!XBlock)
						{pSelect -> m_Translations[m_glx] -= (deltax*m_scales[m_Scale]) * (float)m_invx;}
					if(!YBlock)
						{pSelect -> m_Translations[m_gly] += (deltay*m_scales[m_Scale]) * (float)m_invy;}
				}
				break;
				case m_scale:
				{
					if(!XBlock)
						{pSelect -> m_Scales[m_glx] += ((deltax*m_scales[m_Scale]) * (float)m_invx) * 0.025f;}
					if(!YBlock)
						{pSelect -> m_Scales[m_gly] += ((deltay*m_scales[m_Scale]) * (float)m_invy) * 0.025f;}
				}
				break;
				case m_rotate:
				{
					if(!XBlock)
						{pSelect -> m_Rotations[m_xaxis] -= (deltax * m_scales[m_Scale]) * (float)m_invx;}
				}
				break;
				case m_select:
				break;
			}
		}
		if(pDoc -> m_SelObstacle)
		{
			CObb2 *pSelect = pDoc -> m_Model -> m_ObstacleList -> FindByName(pDoc -> m_SelObstacle);
			if(pSelect == NULL)
				{return;}
			switch(pWnd -> m_mode)
			{
				case m_move:
				{
					if(!XBlock)
						{pSelect -> m_Translations[m_glx] -= (deltax * m_scales[m_Scale]) * (float)m_invx;}
					if(!YBlock)
						{pSelect -> m_Translations[m_gly] += (deltay * m_scales[m_Scale]) * (float)m_invy;}
				}
				break;
				case m_scale:
				{
					if(!XBlock)
						{pSelect -> m_Scales[m_glx] += ((deltax * m_scales[m_Scale]) * (float)m_invx) * 0.025f;}
					if(!YBlock)
						{pSelect -> m_Scales[m_gly] += ((deltay * m_scales[m_Scale]) * (float)m_invy) * 0.025f;}
				}
				break;
				case m_rotate:
				{
					if(!XBlock)
						{pSelect -> m_Rotations[m_xaxis] -= (deltax * m_scales[m_Scale]) * (float)m_invx;}
				}
				break;
				case m_select:
				break;
			}
		}
		if(pDoc -> m_SelArea)
		{
			CObb2 *pSelect = pDoc -> m_Model -> m_AreaList -> FindByName(pDoc -> m_SelArea);
			if(pSelect == NULL)
				{return;}
			switch(pWnd -> m_mode)
			{
				case m_move:
				{
					if(!XBlock)
						{pSelect -> m_Translations[m_glx] += (deltax * m_scales[m_Scale]) * (float)m_invx;}
					if(!YBlock)
						{pSelect -> m_Translations[m_gly] += (deltay * m_scales[m_Scale]) * (float)m_invy;}
				}
				break;
				case m_scale:
				{
					if(!XBlock)
						{pSelect -> m_Scales[m_glx] += ((deltax * m_scales[m_Scale]) * (float)m_invx) * 0.025f;}
					if(!YBlock)
						{pSelect -> m_Scales[m_gly] += ((deltay * m_scales[m_Scale]) * (float)m_invy) * 0.025f;}
				}
				break;
				case m_rotate:
				{
					if(!XBlock)
						{pSelect -> m_Rotations[m_xaxis] -= (deltax*m_scales[m_Scale]) * (float)m_invx;}
				}
				break;
				case m_select:
				break;
			}
		}
		pDoc -> UpdateAllViews(NULL, 0, NULL);
	}
	m_Point.x = point.x;
	m_Point.y = point.y;
}

void CSOEdit2dView::UpdateViewData(CPoint point)
{
    int deltax, deltay;
    deltax = point.x - m_vPoint.x;
    deltay = point.y - m_vPoint.y;
    m_xAdj -= deltax;
    m_yAdj -= deltay;
    m_vPoint.x = point.x;
    m_vPoint.y = point.y;
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSOEdit2dView message handlers

void CSOEdit2dView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    m_lButton = true;
    m_Point.x = point.x;
    m_Point.y = point.y;
	CView::OnLButtonDown(nFlags, point);
}

void CSOEdit2dView::OnMButtonDown(UINT nFlags, CPoint point) 
{
    m_mButton = true;
    m_vPoint.x = point.x;
    m_vPoint.y = point.y;
	CView::OnMButtonDown(nFlags, point);
}

void CSOEdit2dView::OnRButtonDown(UINT nFlags, CPoint point) 
{
    m_rButton = true;
    CMenu menu;
    #ifdef ALTERNATIVE_LANG
		VERIFY(menu.LoadMenu(IDR_VIEW_MENU_AL));
	#else
		VERIFY(menu.LoadMenu(IDR_VIEW_MENU));
	#endif
    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup != NULL);
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc* pDoc = GetDocument();
    #ifdef ALTERNATIVE_LANG
		if(m_BackFace)
			{pPopup -> CheckMenuItem(ID_VIEW_BACKFACES_AL, MF_UNCHECKED);}
		else
			{pPopup -> CheckMenuItem(ID_VIEW_BACKFACES_AL, MF_CHECKED);}
		if(pWnd -> m_Selected)
			{pPopup -> CheckMenuItem(ID_VIEW_SELECTED_AL, MF_CHECKED);}
		else
			{pPopup -> CheckMenuItem(ID_VIEW_SELECTED_AL, MF_UNCHECKED);}
		pPopup -> CheckMenuItem(ID_VIEW_SHADED_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_WIREFRAME_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_COLORIZE_AL, MF_UNCHECKED);
		if(pDoc -> Colorized_3d)
			{pPopup -> CheckMenuItem(ID_VIEW_COLORIZE_AL, MF_CHECKED);}
		switch(m_DrawMode)
		{
			case r_texture:
				{pPopup -> CheckMenuItem(ID_VIEW_SHADED_AL, MF_CHECKED);}
			break;
			case r_wire:
				{pPopup -> CheckMenuItem(ID_VIEW_WIREFRAME_AL, MF_CHECKED);}
			break;
		};
		pPopup -> CheckMenuItem(ID_VIEW_LEFT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_TOP_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_RIGHT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_BOTTOM_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_FRONT_AL, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_REAR_AL, MF_UNCHECKED);
		switch(m_View)
		{
			case v_front:
				{pPopup -> CheckMenuItem(ID_VIEW_FRONT_AL, MF_CHECKED);}
			break;
			case v_right:
				{pPopup -> CheckMenuItem(ID_VIEW_RIGHT_AL, MF_CHECKED);}
			break;
			case v_left:
				{pPopup -> CheckMenuItem(ID_VIEW_LEFT_AL, MF_CHECKED);}
			break;
			case v_top:
				{pPopup -> CheckMenuItem(ID_VIEW_TOP_AL, MF_CHECKED);}
			break;
			case v_rear:
				{pPopup -> CheckMenuItem(ID_VIEW_REAR_AL, MF_CHECKED);}
			break;
			case v_bottom:
				{pPopup -> CheckMenuItem(ID_VIEW_BOTTOM_AL, MF_CHECKED);}
			break;
		};
	#else
		if(m_BackFace)
			{pPopup -> CheckMenuItem(ID_VIEW_BACKFACES, MF_UNCHECKED);}
		else
			{pPopup -> CheckMenuItem(ID_VIEW_BACKFACES, MF_CHECKED);}
		if(pWnd -> m_Selected)
			{pPopup -> CheckMenuItem(ID_VIEW_SELECTED, MF_CHECKED);}
		else
			{pPopup -> CheckMenuItem(ID_VIEW_SELECTED, MF_UNCHECKED);}
		pPopup -> CheckMenuItem(ID_VIEW_SHADED, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_WIREFRAME, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_COLORIZE, MF_UNCHECKED);
		if(pDoc -> Colorized_3d)
			{pPopup -> CheckMenuItem(ID_VIEW_COLORIZE, MF_CHECKED);}
		switch(m_DrawMode)
		{
			case r_texture:
				{pPopup -> CheckMenuItem(ID_VIEW_SHADED, MF_CHECKED);}
			break;
			case r_wire:
				{pPopup -> CheckMenuItem(ID_VIEW_WIREFRAME, MF_CHECKED);}
			break;
		};
		pPopup -> CheckMenuItem(ID_VIEW_LEFT, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_TOP, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_RIGHT, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_BOTTOM, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_FRONT, MF_UNCHECKED);
		pPopup -> CheckMenuItem(ID_VIEW_REAR, MF_UNCHECKED);
		switch(m_View)
		{
			case v_front:
				{pPopup -> CheckMenuItem(ID_VIEW_FRONT, MF_CHECKED);}
			break;
			case v_right:
				{pPopup -> CheckMenuItem(ID_VIEW_RIGHT, MF_CHECKED);}
			break;
			case v_left:
				{pPopup -> CheckMenuItem(ID_VIEW_LEFT, MF_CHECKED);}
			break;
			case v_top:
				{pPopup -> CheckMenuItem(ID_VIEW_TOP, MF_CHECKED);}
			break;
			case v_rear:
				{pPopup -> CheckMenuItem(ID_VIEW_REAR, MF_CHECKED);}
			break;
			case v_bottom:
				{pPopup -> CheckMenuItem(ID_VIEW_BOTTOM, MF_CHECKED);}
			break;
		};

	#endif

    GetCursorPos(&point);
    pPopup -> TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	CView::OnRButtonDown(nFlags, point);
}

void CSOEdit2dView::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if(m_lButton)
	{
		m_lButton = false;
		UpdateSelected(point);
	}
	CView::OnLButtonUp(nFlags, point);
}

void CSOEdit2dView::OnMButtonUp(UINT nFlags, CPoint point) 
{
    m_mButton = false;
	CView::OnMButtonUp(nFlags, point);
}

void CSOEdit2dView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_rButton = false;
	CView::OnRButtonUp(nFlags, point);
}

void CSOEdit2dView::OnViewRight() 
{
    m_View = v_right;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewLeft() 
{
    m_View = v_left;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewFront() 
{
    m_View = v_front;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewRear() 
{
    m_View = v_rear;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}
 
void CSOEdit2dView::OnViewTop() 
{
    m_View = v_top;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewBottom() 
{
    m_View = v_bottom;
    SetupView();
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewCamera() 
{
    m_View = v_camera;
    SetupView();
    OnUpdate(NULL, 0, NULL);
}

BOOL CSOEdit2dView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
    if((zDelta > 0) && (m_Scale > 0))
		{m_Scale--;}
    else
    if((zDelta < 0) && (m_Scale < 23))
		{m_Scale++;}
    switch(m_View)
	{
		case v_front:
			{m_Camera.Position[2] += (zDelta / 10.f);}
		break; // Right
        case v_rear: // actual front
			{m_Camera.Position[2] -= (zDelta / 10.f);}
		break; // Right
        case v_bottom:
			{m_Camera.Position[1] += (zDelta / 10.f);}
		break; // Top
        case v_top:
			{m_Camera.Position[1] -= (zDelta / 10.f);}
		break; // Top
        case v_right: // actual right
			{m_Camera.Position[0] += (zDelta / 10.f);}
		break; // Front
        case v_left: // actual left
			{m_Camera.Position[0] -= (zDelta / 10.f);}
		break; // Front
	}
    InvalidateRect(NULL, false);
    OnUpdate(NULL, 0, NULL);
    return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSOEdit2dView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!(nFlags & MK_LBUTTON))
		{m_lButton = false;}
	if(!(nFlags & MK_MBUTTON))
		{m_mButton = false;}
	if(!(nFlags & MK_RBUTTON))
		{m_rButton = false;}
	if(nFlags & MK_CONTROL)
		{YBlock = true;}
	else
		{YBlock = false;}
	if(nFlags & MK_SHIFT)
		{XBlock = true;}
	else
		{XBlock = false;}
	if(m_lButton)
		{UpdateSelected(point);}
	if(m_mButton)
		{UpdateViewData(point);}
	 CView::OnMouseMove(nFlags, point);
}

void CSOEdit2dView::FixVolumeGeometry(CVolume *pVol)
   {
    glPushMatrix();
    glLoadIdentity();
    if(pVol -> m_Matrix34)
	{
        // Add the editing transformations to the existing Matrix43
        int i, j;
		float m[16] = {0};
        for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 3; j++)
               {m[i*4+j] = pVol -> m_Matrix34 -> v[i][j];}
            m[i*4+j] = 0.0f;
		}
        m[15] = 1.0f;
        glLoadMatrixf(m);
        //glTranslatef(pVol->m_Translations[0], pVol->m_Translations[1], pVol->m_Translations[2]);
        glRotatef(pVol -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
        glRotatef(pVol -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
        glRotatef(pVol -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
        glScalef(pVol -> m_Scales[0], pVol -> m_Scales[1], pVol -> m_Scales[2]);
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
		for(i = 0; i < 4; i++)
		{
            for(j = 0; j < 3; j++)
               {pVol -> m_Matrix34 -> v[i][j] = m[i * 4 + j];}
		}
	}
    else
	{
		if(((pVol -> m_Rotations[0] != 0.0f) || (pVol -> m_Rotations[1] != 0.0f) || (pVol -> m_Rotations[2] != 0.0f))
		|| ((pVol -> m_Scales[0] != 1.0f) || (pVol -> m_Scales[1] != 1.0f) || (pVol -> m_Scales[2] != 1.0f)))
		{
            int i, j;
			float m[16] = {0};
			//glTranslatef(pVol->m_Translations[0], pVol->m_Translations[1], pVol->m_Translations[2]);//
			glRotatef(pVol -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
			glRotatef(pVol -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
			glRotatef(pVol -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
			glScalef(pVol -> m_Scales[0], pVol -> m_Scales[1], pVol -> m_Scales[2]);
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			pVol -> m_Matrix34 = new matrix34_t;
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 3; j++)
					{pVol -> m_Matrix34 -> v[i][j] = m[i * 4 + j];}
			}
		}
        else
		{
			pVol -> m_Matrix34 = new matrix34_t;
			pVol -> m_Matrix34 -> v[0][0] = 1;
			pVol -> m_Matrix34 -> v[0][1] = 0;
			pVol -> m_Matrix34 -> v[0][2] = 0;
			pVol -> m_Matrix34 -> v[1][0] = 0;
			pVol -> m_Matrix34 -> v[1][1] = 1;
			pVol -> m_Matrix34 -> v[1][2] = 0;
			pVol -> m_Matrix34 -> v[2][0] = 0;
			pVol -> m_Matrix34 -> v[2][1] = 0;
			pVol -> m_Matrix34 -> v[2][2] = 1;
			pVol -> m_Matrix34 -> v[3][0] = pVol -> m_Translations[0];
			pVol -> m_Matrix34 -> v[3][1] = pVol -> m_Translations[1];
			pVol -> m_Matrix34 -> v[3][2] = pVol -> m_Translations[2];
		}
	}
	glPopMatrix();
	for(int i = 0; i < 3; i++)
	{
		pVol -> m_Rotations[i] = 0.0f;
		pVol -> m_Scales[i] = 1.0f;
	}
}

void CSOEdit2dView::DrawVolumeGL(CBone *basis)
{
	CSOEditDoc* pDoc = GetDocument();
	bool selected;
	CVolume *pSelect = NULL;
	float VSaturation = 0.0f;
	CVolume *pVol = pDoc -> m_Model -> m_VolumeList -> GetFirst();
	while(pVol)
	{
		if(pVol -> boneptr != basis || !pVol -> m_tree_check)
			{pVol = pVol -> next; continue;}
		FixVolumeGeometry(pVol);
		glPushMatrix();
		if(pVol -> m_Position)
			{glTranslatef(pVol -> m_Position[0], pVol -> m_Position[1], pVol -> m_Position[2]);}
		selected = false;
		if(pDoc -> m_SelVolume)
		{
			if(pVol -> hTreeItem == pDoc -> hSelTreeItem)
			{
				selected = true;
				glGetFloatv(GL_MODELVIEW_MATRIX, full_mx);
				PreMoveMatrix(pVol -> m_Matrix34, pVol -> m_Translations, full_mx);
				pVol -> m_Translations[0] = pVol -> m_Translations[1] = pVol -> m_Translations[2] = 0.0f;
			}
		}
		if(pVol -> m_Matrix34)
		{
			int i, j;
			float m[16];
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 3; j++)
					{m[i * 4 + j] = pVol -> m_Matrix34 -> v[i][j];}
				m[i * 4 + j] = 0.0f;
			}
			m[15] = 1.0f;
			glMultMatrixf((float *)&m);
		}
		CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
		if(selected)
		{
			VSaturation = 384.0f;
			glColor3f(pWnd -> m_Colors[C_VOLUMES][0] / VSaturation, pWnd -> m_Colors[C_VOLUMES][1] / VSaturation, pWnd -> m_Colors[C_VOLUMES][2] / VSaturation);
		}
		else
		{
			VSaturation = 255.0f;
			glColor3f(pWnd -> m_Colors[C_VOLUME][0] / VSaturation, pWnd -> m_Colors[C_VOLUME][1] / VSaturation, pWnd -> m_Colors[C_VOLUME][2] / VSaturation);
		}
		// glTranslatef(pVol->m_Translations[0], pVol->m_Translations[1], pVol->m_Translations[2]);//
		glRotatef(pVol -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
		glRotatef(pVol -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
		glRotatef(pVol -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
		glScalef(pVol -> m_Scales[0], pVol -> m_Scales[1], pVol -> m_Scales[2]);
		if((selected && (pWnd -> m_Selected  && pDoc -> m_SelVolume)) || (!(pWnd -> m_Selected && pDoc -> m_SelVolume)))
		{
			switch(pVol -> m_Type)
			{
				case VT_BOX:
				{
					DrawBoxGL(pVol -> m_Box);
					break;
				}
				case VT_CYLINDER:
				{
					DrawCylGL(pVol -> m_Cyl);
					break;
				}
				case VT_PLY:
				{
					for(int f = 0; pVol -> m_Vol && f < pVol -> m_Vol -> m_numpolys; f++)
					{
						if(selected || pWnd -> m_CollisionSide)
						{
							switch(pVol -> m_Vol -> m_sidelist[f])
							{
								case 1://front
									{glColor3f(pWnd -> m_Colors[VOLUME_FRONT][0] / VSaturation, pWnd -> m_Colors[VOLUME_FRONT][1] / VSaturation, pWnd -> m_Colors[VOLUME_FRONT][2] / VSaturation);}
								break;
								case 2://top
									{glColor3f(pWnd -> m_Colors[VOLUME_TOP][0] / VSaturation, pWnd -> m_Colors[VOLUME_TOP][1] / VSaturation, pWnd -> m_Colors[VOLUME_TOP][2] / VSaturation);}
								break;
								case 3://left
									{glColor3f(pWnd -> m_Colors[VOLUME_LEFT][0] / VSaturation, pWnd -> m_Colors[VOLUME_LEFT][1] / VSaturation, pWnd -> m_Colors[VOLUME_LEFT][2] / VSaturation);}
								break;
								case 4://right
									{glColor3f(pWnd -> m_Colors[VOLUME_RIGHT][0] / VSaturation, pWnd -> m_Colors[VOLUME_RIGHT][1] / VSaturation, pWnd -> m_Colors[VOLUME_RIGHT][2] / VSaturation);}
								break;
								case 5://rear
									{glColor3f(pWnd -> m_Colors[VOLUME_REAR][0] / VSaturation, pWnd -> m_Colors[VOLUME_REAR][1] / VSaturation, pWnd -> m_Colors[VOLUME_REAR][2] / VSaturation);}
								break;
								case 6://bottom
									{glColor3f(pWnd -> m_Colors[VOLUME_BOTTOM][0] / VSaturation, pWnd -> m_Colors[VOLUME_BOTTOM][1] / VSaturation, pWnd -> m_Colors[VOLUME_BOTTOM][2] / VSaturation);}
								break;
							};
						}
						glBegin(GL_POLYGON);
						for(int v = 2; v >= 0; v--)
							{glVertex3fv(pVol -> m_Vol -> m_vertlist[pVol -> m_Vol -> m_polylist[f].v[v]].xyz);}
						glEnd();
					}
				}
				break;
			}
		}
		glPopMatrix();
		pVol = pVol -> next;
	}
}

void CSOEdit2dView::DrawBoxGL(v3_t box)
{
    v3_t vert[8];

    // left rear top
    vert[0][0] = box[0] / -2.0f;
    vert[0][1] = box[1] / -2.0f;
    vert[0][2] = box[2] / 2.0f;

    // right rear top
    vert[1][0] = box[0] / -2.0f;
    vert[1][1] = box[1] / 2.0f;
    vert[1][2] = box[2] / 2.0f;

    // right front top
    vert[2][0] = box[0] / 2.0f;
    vert[2][1] = box[1] / 2.0f;
    vert[2][2] = box[2] / 2.0f;

    // left front top
    vert[3][0] = box[0] / 2.0f;
    vert[3][1] = box[1] / -2.0f;
    vert[3][2] = box[2] / 2.0f;

    // left rear bottom
    vert[4][0] = box[0] / -2.0f;
    vert[4][1] = box[1] / -2.0f;
    vert[4][2] = box[2] / -2.0f;

    // right rear bottom
    vert[5][0] = box[0] / -2.0f;
    vert[5][1] = box[1] / 2.0f;
    vert[5][2] = box[2] / -2.0f;

    // right front bottom
    vert[6][0] = box[0] / 2.0f;
    vert[6][1] = box[1] / 2.0f;
    vert[6][2] = box[2] / -2.0f;

    // left front bottom
    vert[7][0] = box[0] / 2.0f;
    vert[7][1] = box[1] / -2.0f;
    vert[7][2] = box[2] / -2.0f;

    indx_t indx[12];
    indx[0].v[0] = 3;
    indx[0].v[1] = 0;
    indx[0].v[2] = 1;
    indx[1].v[0] = 1;
    indx[1].v[1] = 2;
    indx[1].v[2] = 3;
    indx[2].v[0] = 5;
    indx[2].v[1] = 2;
    indx[2].v[2] = 1;
    indx[3].v[0] = 6;
    indx[3].v[1] = 2;
    indx[3].v[2] = 5;
    indx[4].v[0] = 2;
    indx[4].v[1] = 6;
    indx[4].v[2] = 3;
    indx[5].v[0] = 6;
    indx[5].v[1] = 7;
    indx[5].v[2] = 3;
    indx[6].v[0] = 3;
    indx[6].v[1] = 7;
    indx[6].v[2] = 0;
    indx[7].v[0] = 4;
    indx[7].v[1] = 0;
    indx[7].v[2] = 7;
    indx[8].v[0] = 0;
    indx[8].v[1] = 4;
    indx[8].v[2] = 1;
    indx[9].v[0] = 4;
    indx[9].v[1] = 5;
    indx[9].v[2] = 1;
    indx[10].v[0] = 6;
    indx[10].v[1] = 5;
    indx[10].v[2] = 4;
    indx[11].v[0] = 7;
    indx[11].v[1] = 6;
    indx[11].v[2] = 4;
	for(int f = 0; f < 12; f++)
	{
        glBegin(GL_POLYGON);
        for(int v = 2; v >= 0; v--)
           {glVertex3fv(vert[indx[f].v[v]]);}
        glEnd();
	}
}

void CSOEdit2dView::DrawTrapGL(v3_t box)
{
    v3_t vert[6];

    // left rear top
    vert[0][0] = box[0] / -2.0f;
    vert[0][1] = box[1] / -2.0f;
    vert[0][2] = box[2] / 2.0f;

    // right rear top
    vert[1][0] = box[0] / -2.0f;
    vert[1][1] = box[1] / 2.0f;
    vert[1][2] = box[2] / 2.0f;

    // left rear bottom
    vert[2][0] = box[0] / -2.0f;
    vert[2][1] = box[1] / -2.0f;
    vert[2][2] = box[2] / -2.0f;

    // right rear bottom
    vert[3][0] = box[0] / -2.0f;
    vert[3][1] = box[1] / 2.0f;
    vert[3][2] = box[2] / -2.0f;

    // right front bottom
    vert[4][0] = box[0] / 2.0f;
    vert[4][1] = box[1] / 2.0f;
    vert[4][2] = box[2] / -2.0f;

    // left front bottom
    vert[5][0] = box[0] / 2.0f;
    vert[5][1] = box[1] / -2.0f;
    vert[5][2] = box[2] / -2.0f;

    indx_t indx[8];
    indx[0].v[0] = 1;
    indx[0].v[1] = 4;
    indx[0].v[2] = 3;
    indx[1].v[0] = 5;
    indx[1].v[1] = 0;
    indx[1].v[2] = 2;
    indx[2].v[0] = 0;
    indx[2].v[1] = 1;
    indx[2].v[2] = 3;
    indx[3].v[0] = 3;
    indx[3].v[1] = 2;
    indx[3].v[2] = 0;
    indx[4].v[0] = 1;
    indx[4].v[1] = 0;
    indx[4].v[2] = 5;
    indx[5].v[0] = 5;
    indx[5].v[1] = 4;
    indx[5].v[2] = 1;
    indx[6].v[0] = 2;
    indx[6].v[1] = 3;
    indx[6].v[2] = 4;
    indx[7].v[0] = 4;
    indx[7].v[1] = 5;
    indx[7].v[2] = 2;
	for(int f = 0; f < 8; f++)
	{
        glBegin(GL_POLYGON);
        for(int v = 2; v >= 0; v--)
           {glVertex3fv(vert[indx[f].v[v]]);}
        glEnd();
	}
}

void CSOEdit2dView::DrawDiamondGL(v3_t box)
{
	v3_t vert[6];

	// top
	vert[0][0] = 0.0f;
	vert[0][1] = 0.0f;
	vert[0][2] = box[2] / 2.0f;

	// bottom
	vert[1][0] = 0.0f;
	vert[1][1] = 0.0f;
	vert[1][2] = box[2] / -2.0f;

	// left
	vert[2][0] = box[0] / -2.0f;
	vert[2][1] = 0.0f;
	vert[2][2] = 0.0f;

	// right
	vert[3][0] = box[0] / 2.0f;
	vert[3][1] = 0.0f;
	vert[3][2] = 0.0f;

	// rear
	vert[4][0] = 0.0f;
	vert[4][1] = box[1] / -2.0f;
	vert[4][2] = 0.0f;

	// front
	vert[5][0] = 0.0f;
	vert[5][1] = box[1] / 2.0f;
	vert[5][2] = 0.0f;

	indx_t indx[8];
	indx[0].v[0] = 0; indx[0].v[1] = 3; indx[0].v[2] = 5;
	indx[1].v[0] = 0; indx[1].v[1] = 5; indx[1].v[2] = 2;
	indx[2].v[0] = 0; indx[2].v[1] = 2; indx[2].v[2] = 4;
	indx[3].v[0] = 0; indx[3].v[1] = 4; indx[3].v[2] = 3;
	indx[4].v[0] = 1; indx[4].v[1] = 5; indx[4].v[2] = 3;
	indx[5].v[0] = 1; indx[5].v[1] = 2; indx[5].v[2] = 5;
	indx[6].v[0] = 1; indx[6].v[1] = 4; indx[6].v[2] = 2;
	indx[7].v[0] = 1; indx[7].v[1] = 3; indx[7].v[2] = 4;

	for (int f = 0; f < 8; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 2; v >= 0; v--)
		{
			glVertex3fv(vert[indx[f].v[v]]);
		}
		glEnd();
	}
}

void CSOEdit2dView::DrawCylGL(CCylinder *cyl)
{
	for(int f = 0; f < cyl -> m_faces; f++)
	{
        glBegin(GL_POLYGON);
        for(int v = 2; v >= 0; v--)
           {glVertex3fv(cyl -> m_vert[cyl -> m_face[f].v[v]]);}
        glEnd();
	}
}

void CSOEdit2dView::DrawObb2GL(CObb2 *obj)
{
	glPushMatrix();
	if(obj -> m_Center)
	{
		obj -> m_Center[0] += obj -> m_Translations[0];
		obj -> m_Center[1] += obj -> m_Translations[1];
		glTranslatef(obj -> m_Center[0], obj -> m_Center[1], 0.0f);
	}
	switch(obj -> m_GType)
	{
		case OT_RECT:
		{
			v3_t  square[4];
			obj -> m_Extent[0] *= obj -> m_Scales[0];
			obj -> m_Extent[1] *= obj -> m_Scales[1];
			if(!obj -> m_Rotate)
			{
				square[0][0] = 0.0f;
				square[0][1] = 0.0f;
				square[0][2] = 0.0f;
				square[1][0] = 0.0f;
				square[1][1] = obj -> m_Extent[1];
				square[1][2] = 0.0f;
				square[2][0] = obj -> m_Extent[0];
				square[2][1] = obj -> m_Extent[1];
				square[2][2] = 0.0f;
				square[3][0] = obj -> m_Extent[0];
				square[3][1] = 0.0f;
				square[3][2] = 0.0f;
			}
			else
			{
				square[0][0] = 0.0f - obj -> m_Extent[0];
				square[0][1] = 0.0f - obj -> m_Extent[1];
				square[0][2] = 0.0f;
				square[1][0] = 0.0f - obj -> m_Extent[0];
				square[1][1] = obj -> m_Extent[1];
				square[1][2] = 0.0f;
				square[2][0] = obj -> m_Extent[0];
				square[2][1] = obj -> m_Extent[1];
				square[2][2] = 0.0f;
				square[3][0] = obj -> m_Extent[0];
				square[3][1] = 0.0f - obj -> m_Extent[1];
				square[3][2] = 0.0f;
			}
			float angle = 0.0f;
			angle = (float)atan2(obj -> m_Axis[1], obj -> m_Axis[0]);
			angle = (float)(angle * (180.0 / 3.1415927));
			glRotatef(angle, 0.0f, 0.0f, 1.0f);
			glRotatef(obj -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
			glBegin(GL_POLYGON);
			glVertex3fv(square[0]);
			glVertex3fv(square[1]);
			glVertex3fv(square[2]);
			glVertex3fv(square[3]);
			glEnd();
		}
		break;
		case OT_CIRCLE:
		{
			int sides = 12;
			v2_t *m_xy = NULL;
			*obj -> m_Radius *= obj -> m_Scales[0];
			float fx = 0.0f, fy = *obj -> m_Radius;
			float degrees = (360.0f / (float)sides);
			float angle = degrees / (float)(180.0 / 3.1415927);
			m_xy = new v2_t[sides];
			for(int side = 0; side < sides; side++)
			{
				m_xy[side][0] = fx;
				m_xy[side][1] = fy;
				float tx = 0.0f, ty = 0.0f;
				tx = (float)(fx * cos(angle)) - (float)(fy * sin(angle));
				ty = (float)(fy * cos(angle)) + (float)(fx * sin(angle));
				fx = tx;
				fy = ty;
			}
			for(int side = 0; side < sides; side++)
			{
				glBegin(GL_POLYGON);
				glVertex3f(m_xy[side][0], m_xy[side][1], 0.0f);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glVertex3f(m_xy[(side + 1) % sides][0], m_xy[(side + 1) % sides][1], 0.0f);
				glEnd();
			}
			delete[] m_xy;
			m_xy = NULL;
		}
		break;
		case OT_POLYGON:
		{
			glRotatef(obj -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
			glBegin(GL_POLYGON);
			for (int v = 0; v < obj -> m_Vertices; v++)
			{
				obj -> m_Vertex[v][0] *= obj -> m_Scales[0];
				obj -> m_Vertex[v][1] *= obj -> m_Scales[1];
				obj -> m_Vertex[v][0] += obj -> m_Translations[0];
				obj -> m_Vertex[v][1] += obj -> m_Translations[1];
				glVertex3f(obj -> m_Vertex[v][0], obj -> m_Vertex[v][1], 0.0f);
			}
			glEnd();
		}
		break;
	}
	obj -> m_Scales[0] = 1.0f;
	obj -> m_Scales[1] = 1.0f;
	obj -> m_Scales[2] = 1.0f;
	obj -> m_Translations[0] = 0.0f;
	obj -> m_Translations[1] = 0.0f;
	obj -> m_Translations[2] = 0.0f;
	glPopMatrix();
}

void CSOEdit2dView::FixBoneGeometry(CBone *bone)
{
    glPushMatrix();
    glLoadIdentity();
    if(bone -> Matrix34)
	{
        // Add the editing transformations to the existing Matrix43
        int i, j;
		float m[16] = {0};
        for(i = 0; i < 4; i++)
		{
            for(j = 0; j < 3; j++)
               {m[i*4+j] = bone -> Matrix34 -> v[i][j];}
            m[i*4+j] = 0.0f;
		}
        m[15] = 1.0f;
        glLoadMatrixf(m);
        glRotatef(bone -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
        glRotatef(bone -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
        glRotatef(bone -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
        glScalef(bone -> m_Scales[0], bone -> m_Scales[1], bone -> m_Scales[2]);
        glGetFloatv(GL_MODELVIEW_MATRIX, m);
        for(i = 0; i < 4; i++)
		{
            for(j = 0; j < 3; j++)
				{bone -> Matrix34 -> v[i][j] = m[i*4+j];}
		}
	}
    else
	{
		if((((bone -> m_Rotations[0] != 0.0f) || (bone -> m_Rotations[1] != 0.0f) || (bone -> m_Rotations[2] != 0.0f))
		|| ((bone -> m_Scales[0] != 1.0f) || (bone -> m_Scales[1] != 1.0f) || (bone -> m_Scales[2] != 1.0f)))
		&& ((bone -> m_Translations[0] != 0.0f) || (bone -> m_Translations[1] != 0.0f) || (bone -> m_Translations[2] != 0.0f)))
		{
            int i, j;
			float m[16] = {0};
            glRotatef(bone -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
            glRotatef(bone -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
            glRotatef(bone -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
            glScalef(bone -> m_Scales[0], bone -> m_Scales[1], bone -> m_Scales[2]);
            glGetFloatv(GL_MODELVIEW_MATRIX, m);
            bone -> Matrix34 = new matrix34_t;
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 3; j++)
					{bone -> Matrix34 -> v[i][j] = m[i * 4 + j];}
			}
		}
        else
        if(((bone -> m_Rotations[0] != 0.0f) || (bone -> m_Rotations[1] != 0.0f) || (bone -> m_Rotations[2] != 0.0f))
		|| ((bone -> m_Scales[0] != 1.0f) || (bone -> m_Scales[1] != 1.0f) || (bone -> m_Scales[2] != 1.0f)))
		{
            int i, j;
            float m[16] = {0};
            glRotatef(bone -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
            glRotatef(bone -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
            glRotatef(bone -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
            glScalef(bone -> m_Scales[0], bone -> m_Scales[1], bone -> m_Scales[2]);
            glGetFloatv(GL_MODELVIEW_MATRIX, m);
            bone -> Matrix34 = new matrix34_t;
            for(i = 0; i < 3; i++)
			{
				for(j = 0; j < 3; j++)
					{bone -> Matrix34 -> v[i][j] = m[i*4+j];}
			}
			bone -> Matrix34 -> v[3][0] = 0;
			bone -> Matrix34 -> v[3][1] = 0;
			bone -> Matrix34 -> v[3][2] = 0;
		}
        else
		{
			bone -> Matrix34 = new matrix34_t;
			bone -> Matrix34 -> v[0][0] = 1;
			bone -> Matrix34 -> v[0][1] = 0;
			bone -> Matrix34 -> v[0][2] = 0;
			bone -> Matrix34 -> v[1][0] = 0;
			bone -> Matrix34 -> v[1][1] = 1;
			bone -> Matrix34 -> v[1][2] = 0;
			bone -> Matrix34 -> v[2][0] = 0;
			bone -> Matrix34 -> v[2][1] = 0;
			bone -> Matrix34 -> v[2][2] = 1;
			bone -> Matrix34 -> v[3][0] = bone -> m_Translations[0];
			bone -> Matrix34 -> v[3][1] = bone -> m_Translations[1];
			bone -> Matrix34 -> v[3][2] = bone -> m_Translations[2];
		}
	}
    glPopMatrix();
	/*
	IF object.matrix34 IS NOT NULL THEN
	   USE existing object.matrix34
	ELSE // object.matrix34 is NULL 
	IF object.orientation IS NOT NULL THEN
	   IF object.translates IS NULL THEN // analogous to position
		  CREATE object.matrix34 and DROP object.orientation
	   ELSE
		  USE existing object.orientation
	   ENDIF
	ELSE // object.matrix34 and object.orientation are NULL
	IF object.position IS NOT NULL THEN
	   IF object.rotations OR object.scales THEN
		  CREATE object.matrix34 AND DROP object.position
	   ELSE
		  USE object.position
	   ENDIF
	ELSE // object.matrix34 and object.orientation and object.position are NULL // highly unlikely but possible
	   IF object.position AND (object.scales OR object.rotations) THEN
		  CREATE object.matrix34
	   ELSE
	   IF object.position AND object.scales IS NULL AND object.rotations IS NULL THEN
		  CREATE object.position
	   ELSE
	   IF object.scales OR object.rotations THEN
		  CREATE object.orientation
	w   ENDIF
	ENDIF
	*/
}

void CSOEdit2dView::DrawBoneGL(CBone *basis)
{
	CBone *child, *pSelect = NULL;
	int f, v;
	bool selected = false, impact = false;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc* pDoc = GetDocument();
	glGetFloatv(GL_MODELVIEW_MATRIX, full_mx);//берём старую матрицу
    glPushMatrix();
	if(pDoc -> m_Frame < 0)
		{pDoc -> m_Frame = 0;}
    if(((basis -> m_Rotations[0] != 0.0f) || (basis -> m_Rotations[1] != 0.0f) || (basis -> m_Rotations[2] != 0.0f))
    || ((basis -> m_Scales[0] != 1.0f) || (basis -> m_Scales[1] != 1.0f) || (basis -> m_Scales[2] != 1.0f))
	|| ((basis -> m_Translations[0] != 0.0f) || (basis -> m_Translations[1] != 0.0f) || (basis -> m_Translations[2] != 0.0f)))
		{impact = true;}
	if(pDoc -> m_SelBone)
	{
		pSelect = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(pDoc -> hSelTreeItem);
		if(pSelect == basis)
			{selected = true;}
	}
	if(impact && selected)
	{
		if((!pDoc -> m_AnimBone || !pDoc -> m_Frame) && !pDoc -> Anm_and_skeleton_editing_prohibited)
		{
			FixBoneGeometry(basis);
			PreMoveMatrix(basis -> Matrix34, basis -> m_Translations, full_mx);
		}
	}
	if(basis -> Matrix34)//Нужна на случай отсутствия кости в анимации. И без анимации тоже нужна. (Больше не прикасаться к этому блоку!!!)
	{
		int i, j;
		float m[16] = {0};
		for(i = 0; i < 4; i++)
		{
			for(j = 0; j < 3; j++)
				{m[i * 4 + j] = basis -> Matrix34 -> v[i][j];}
			m[i * 4 + j] = 0.0f;
		}
		m[15] = 1.0f;
		glMultMatrixf((float *)&m);
	}



	/*if(impact && selected)
	{
		float full_mx2[16];
		HMatrix HM = {0};
		glGetFloatv(GL_MODELVIEW_MATRIX, full_mx2);
		HM[0][0] = full_mx2[0]; HM[1][0] = full_mx2[1]; HM[2][0] = full_mx2[2]; HM[3][0] = full_mx2[3];
		HM[0][1] = full_mx2[4]; HM[1][1] = full_mx2[5]; HM[2][1] = full_mx2[6]; HM[3][1] = full_mx2[7];
		HM[0][2] = full_mx2[8]; HM[1][2] = full_mx2[9]; HM[2][2] = full_mx2[10]; HM[3][2] = full_mx2[11];
		HM[0][3] = full_mx2[12]; HM[1][3] = full_mx2[13]; HM[2][3] = full_mx2[14]; HM[3][3] = full_mx2[15];

		//EulOrdXYZs Y можно интерпритировать как ось Z. Диапазон 90 -90. По дефолту смещён не семетрично.
		//
		//


		EulerAngles ea = Eul_FromHMatrix(HM, EulOrdXYXs);

		char mess[1000] = {0};
		sprintf(mess, "X = %f | Y = %f | Z = %f | W = %f", (ea.x * (180.0f / 3.14f)), (ea.y * (180.0f / 3.14f)), (ea.z * (180.0f / 3.14f)), ea.w);
		pDoc -> SetTitle(mess);
	}/**/



	if(pDoc -> m_AnimBone)
	{
		int iBone = 0;
		for(;iBone < pDoc -> m_AnimBone -> m_BoneCnt; iBone++)
		{
			if(!stricmp(pDoc -> m_AnimBone -> m_BoneMap[iBone], basis -> m_Name))
				{break;}
		}
		CAnimSub *t_sub;
		if(iBone == pDoc -> m_AnimBone -> m_BoneCnt && selected && impact && pDoc -> m_Frame > 0)
			{pDoc -> CreateNewBoneInAnm(basis);}//нет кости в кадре - создаём
		if(iBone < pDoc -> m_AnimBone -> m_BoneCnt)
		{
			if(pDoc -> m_Frame >= pDoc -> m_AnimBone -> m_FrameCnt)
				{pDoc -> m_Frame = pDoc -> m_AnimBone -> m_FrameCnt - 1;}
			if(t_sub = pDoc -> m_AnimBone -> m_Frames[pDoc -> m_Frame].FindSub(iBone))
			{
				if(!pDoc -> Anm_and_skeleton_editing_prohibited && pDoc -> m_Frame)
					{PreMoveMatrix(&t_sub -> m_Matrix34, basis -> m_Translations, full_mx);}//смещаем по старой матрице
				int i, j;
				float m[16];
				if(!pDoc -> m_Frame)
					{memcpy(&t_sub -> m_Matrix34, basis -> Matrix34, sizeof(matrix34_t));}//синхронизируем матрицу первого кадра с матрицей кости
				for(i = 0; i < 4; i++)//грузим матрицу во временный буфер
				{
					for(j = 0; j < 3; j++)
					   {m[i * 4 + j] = t_sub -> m_Matrix34.v[i][j];}
					m[i * 4 + j] = 0.0f;
				}
				m[15] = 1.0f;
				if(!pDoc -> Anm_and_skeleton_editing_prohibited && pDoc -> m_Frame)//если можно редактировать
				{
					glPushMatrix();//создаём изоляционные условия
					glLoadIdentity();//загружаем чистый лист
					glLoadMatrixf(m);//загружаем нашу матрицу в чистую матрицу
					//издеваемся над матрицей
					glRotatef(basis -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
					glRotatef(basis -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
					glRotatef(basis -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
					glScalef(basis -> m_Scales[0], basis -> m_Scales[1], basis -> m_Scales[2]);
					glGetFloatv(GL_MODELVIEW_MATRIX, m);//грузим полученную матрицу во временный буфер
					for(i = 0; i < 4; i++)
					{
						for(j = 0; j < 3; j++)
							{t_sub -> m_Matrix34.v[i][j] = m[i * 4 + j];}//перегружаем буфер в кадр
					}
					if(pDoc -> InheritMatrix && selected && impact)
					{
						CAnimSub *t2_sub = NULL;
						for(int i = pDoc -> m_Frame + 1; i < pDoc -> m_AnimBone -> m_FrameCnt; i++)
						{
							t2_sub = pDoc -> m_AnimBone -> m_Frames[i].m_head;
							while(t2_sub)
							{
								if(t2_sub -> m_Bone == t_sub -> m_Bone)
									{break;}
								t2_sub = t2_sub -> m_next;
							}
							if(!t2_sub)
								{continue;}
							memcpy(&t2_sub -> m_Matrix34, &t_sub -> m_Matrix34, sizeof(matrix34_t));
						}
					}
					glPopMatrix();//снимаем изоляционные условия
				}
				glPopMatrix();//возвращаемся к состоянию перед загрузкой матрицы кости
				glPushMatrix();//создаём новый уровень матрицы
				glMultMatrixf((float *)&m);//в новом уровне домножаем старое состояние на матрицу из буфера
			}
		}
	}
	for(int i = 0; i < 3; i++)
	{
        basis -> m_Rotations[i] = 0.0f;
        basis -> m_Translations[i] = 0.0f;
        basis -> m_Scales[i] = 1.0f;
	}
	if((!pWnd -> m_CollisionSide || pDoc -> m_ViewMod == vm3_hybrid || pDoc -> m_ViewMod == vm3_visual) && basis -> m_tree_check)
	{
		if(basis -> m_VolumeView)
		{
			if(pDoc -> Colorized_3d)
			{
				if(selected)
					{glColor3f(pWnd -> m_Colors[C_MESHS][0] / 255.0f, pWnd -> m_Colors[C_MESHS][1] / 255.0f, pWnd -> m_Colors[C_MESHS][2] / 255.0f);}
				else
					{glColor3f(GetRValue(basis -> m_bone_color) / 255.0f, GetGValue(basis -> m_bone_color) / 255.0f, GetBValue(basis -> m_bone_color) / 255.0f);}
			}
			else
			{
				if(selected)
					{glColor3f(pWnd -> m_Colors[C_MESHS][0] / 255.0f, pWnd -> m_Colors[C_MESHS][1] / 255.0f, pWnd -> m_Colors[C_MESHS][2] / 255.0f);}
				else
					{glColor3f(pWnd -> m_Colors[C_MESH][0] / 255.0f, pWnd -> m_Colors[C_MESH][1] / 255.0f, pWnd -> m_Colors[C_MESH][2] / 255.0f);}
			}
			if((selected && (pWnd -> m_Selected && pDoc -> m_SelBone)) || (!(pWnd -> m_Selected && pDoc -> m_SelBone)))
			{
				CMesh *pMesh = basis -> m_VolumeView -> m_meshlist -> GetFirst();
				int tindx = 0;
				while(pMesh)
				{
					for(f = pMesh -> m_first; f < pMesh -> m_count + pMesh -> m_first; f++)
					{
						glBegin(GL_POLYGON);
						for(v = 2; v >= 0; v--)
						{
							if((basis -> m_VolumeView -> m_vertlist) && (basis -> m_VolumeView -> m_polylist))
							{
								tindx = basis -> m_VolumeView -> m_polylist[f].v[v];
								glVertex3fv(basis -> m_VolumeView -> m_vertlist[tindx].xyz);
							}
						}
						glEnd();
					}
					pMesh = pMesh -> next;
				}
			}
			if(pWnd -> m_AllMeshVectors || selected)
				{DrawXYZLinesGL();}
		}
		else
		{
			if(selected)
				{glColor3f(pWnd -> m_Colors[C_ENTITYS][0] / 255.0f, pWnd -> m_Colors[C_ENTITYS][1] / 255.0f, pWnd -> m_Colors[C_ENTITYS][2] / 255.0f);}
			else
				{glColor3f(pWnd -> m_Colors[C_ENTITY][0] / 255.0f, pWnd -> m_Colors[C_ENTITY][1] / 255.0f, pWnd -> m_Colors[C_ENTITY][2] / 255.0f);}
			if(((selected && (pWnd -> m_Selected && pDoc -> m_SelBone)) || (!(pWnd -> m_Selected && pDoc -> m_SelBone))) && (pWnd -> m_ViewEnts))
			{
				v3_t box;
				box[0] = 6.0f;
				box[1] = 6.0f;
				box[2] = 6.0f;
				DrawDiamondGL(box);
			}
		}
	}
	if((pDoc -> m_Model -> m_VolumeList) && (pWnd -> m_ViewVols) && basis)
		{DrawVolumeGL(basis);}
	if((child = basis -> m_ChildFirst) != NULL)
	{
		while(child)
		{
			DrawBoneGL(child);
			child = child -> next;
		}
	}
	glPopMatrix();
}

void CSOEdit2dView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	m_pDC = GetDC();
    m_hDC = m_pDC -> m_hDC;
    InitGL();
    OnDraw(m_pDC);
}

void CSOEdit2dView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
	{OnDraw(m_pDC);}

void CSOEdit2dView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
    RECT rect;
    GetClientRect(&rect);
    m_glnWidth = rect.right;
    m_glnHeight = rect.bottom;
    m_gldAspect = (GLdouble)m_glnWidth/(GLdouble)m_glnHeight;
    OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewBackfaces()
{
	if(m_BackFace)
		{m_BackFace = false;}
	else
		{m_BackFace = true;}
	OnUpdate(NULL, 0, NULL);
}

/* Convert matrix to Euler angles (in radians). */
EulerAngles CSOEdit2dView::Eul_FromHMatrix(HMatrix M, int order)
{
	EulerAngles ea;
	int i, j, k, h, n, s, f;
	EulGetOrd(order, i, j, k, h, n, s, f);
	if(s == EulRepYes)
	{
		float sy = sqrtf(M[i][j] * M[i][j] + M[i][k] * M[i][k]);
		if(sy > 16 * FLT_EPSILON)
		{
			ea.x = atan2f(M[i][j], M[i][k]);
			ea.y = atan2f(sy, M[i][i]);
			ea.z = atan2f(M[j][i], -M[k][i]);
		}
		else
		{
			ea.x = atan2f(-M[j][k], M[j][j]);
			ea.y = atan2f(sy, M[i][i]);
			ea.z = 0;
		}
	}
	else
	{
		float cy = sqrtf(M[i][i] * M[i][i] + M[j][i] * M[j][i]);
		if(cy > 16 * FLT_EPSILON)
		{
			ea.x = atan2f(M[k][j], M[k][k]);
			ea.y = atan2f(-M[k][i], cy);
			ea.z = atan2f(M[j][i], M[i][i]);
		}
		else
		{
			ea.x = atan2f(-M[j][k], M[j][j]);
			ea.y = atan2f(-M[k][i], cy);
			ea.z = 0;
		}
	}
	if(n == EulParOdd)
		{ea.x = -ea.x; ea.y = -ea.y; ea.z = -ea.z;}
	if(f == EulFrmR)
		{float t = ea.x; ea.x = ea.z; ea.z = t;}
	ea.w = (float)order;
	return (ea);
}

bool CSOEdit2dView::MR(float f, float b)
{
	if(b < 0)
	{
		if(f <= (b + 0.1) && f >= (b - 0.1))
			{return true;}
		else
			{return false;}
	}
	else
	{
		if(f >= (b - 0.1) && f <= (b + 0.1))
			{return true;}
		else
			{return false;}
	}
	return false;
}

void CSOEdit2dView::PreMoveMatrix(matrix34_t *Move_Matrix34, v3_t Move_Translations, float *Move_mx)
{
	float P0 = 0, P1 = 0, P2 = 0;
	float T1 = 0.0, T2 = 0.0, T3 = 0.0, T4 = 0.0, T5 = 0.0, T6 = 0.0, T7 = 0.0, T8 = 0.0, T9 = 0.0;
	int MoveType = 0;
	//CSOEditDoc* pDoc = GetDocument();
	T1 = round(Move_mx[0]);
	T2 = round(Move_mx[1]);
	T3 = round(Move_mx[2]);
	T4 = round(Move_mx[4]);
	T5 = round(Move_mx[5]);
	T6 = round(Move_mx[6]);
	T7 = round(Move_mx[8]);
	T8 = round(Move_mx[9]);
	T9 = round(Move_mx[10]);
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 1;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 2;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 3;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 4;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 5;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 6;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 7;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 8;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 9;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 10;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 11;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 12;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 13;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 0) && MR(T5, 1) && MR(T6, 0) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 14;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 15;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 0) && MR(T5, -1) && MR(T6, 0) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 16;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 17;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 18;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 19;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 20;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 21;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 22;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, 1))
		{MoveType = 23;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 0) && MR(T9, -1))
		{MoveType = 24;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 25;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 26;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 27;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 28;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 29;}
	else
	if(MR(T1, 0) && MR(T2, 1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 30;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 31;}
	else
	if(MR(T1, 0) && MR(T2, -1) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, -1) && MR(T8, 0) && MR(T9, 0))
		{MoveType = 32;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 33;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 34;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 35;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, 1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 36;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 37;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 38;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, 1) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 39;}
	else
	if(MR(T1, 0) && MR(T2, 0) && MR(T3, -1) && MR(T4, -1) && MR(T5, 0) && MR(T6, 0) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 40;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 41;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 42;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 43;}
	else
	if(MR(T1, 1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 44;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 45;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 0) && MR(T8, 1) && MR(T9, 0))
		{MoveType = 46;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, 1) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 47;}
	else
	if(MR(T1, -1) && MR(T2, 0) && MR(T3, 0) && MR(T4, 0) && MR(T5, 0) && MR(T6, -1) && MR(T7, 0) && MR(T8, -1) && MR(T9, 0))
		{MoveType = 48;}
	//char mess[1000] = {0};
	//sprintf(mess, "T1 = %f | T2 = %f | T3 = %f T4 = %f T5 = %f T6 = %f T7 = %f T8 = %f T9 = %f (%s) [%d]", T1, T2, T3, T4, T5, T6, T7, T8, T9, pDoc->m_SelBone, MoveType);
	//pDoc->SetTitle(mess);
	switch(MoveType)
	{
		case 39:
			{P0 = Move_Translations[0]; P1 = Move_Translations[1]; P2 = Move_Translations[2];}
		break;
		case 37:
			{P0 = Move_Translations[0]; P1 = Move_Translations[1]; P2 = -Move_Translations[2];}
		break;
		case 35:
			{P0 = Move_Translations[0]; P1 = -Move_Translations[1]; P2 = Move_Translations[2];}
		break;
		case 33:
			{P0 = Move_Translations[0]; P1 = -Move_Translations[1]; P2 = -Move_Translations[2];}
		break;
		case 40:
			{P0 = -Move_Translations[0]; P1 = Move_Translations[1]; P2 = Move_Translations[2];}
		break;
		case 38:
			{P0 = -Move_Translations[0]; P1 = Move_Translations[1]; P2 = -Move_Translations[2];}
		break;
		case 36:
			{P0 = -Move_Translations[0]; P1 = -Move_Translations[1]; P2 = Move_Translations[2];}
		break;
		case 34:
			{P0 = -Move_Translations[0]; P1 = -Move_Translations[1]; P2 = -Move_Translations[2];}
		break;
		case 23:
			{P0 = Move_Translations[2]; P1 = Move_Translations[1]; P2 = Move_Translations[0];}
		break;
		case 21:
			{P0 = -Move_Translations[2]; P1 = Move_Translations[1]; P2 = Move_Translations[0];}
		break;
		case 19:
			{P0 = Move_Translations[2]; P1 = -Move_Translations[1]; P2 = Move_Translations[0];}
		break;
		case 17:
			{P0 = -Move_Translations[2]; P1 = -Move_Translations[1]; P2 = Move_Translations[0];}
		break;
		case 24:
			{P0 = Move_Translations[2]; P1 = Move_Translations[1]; P2 = -Move_Translations[0];}
		break;
		case 22:
			{P0 = -Move_Translations[2]; P1 = Move_Translations[1]; P2 = -Move_Translations[0];}
		break;
		case 20:
			{P0 = Move_Translations[2]; P1 = -Move_Translations[1]; P2 = -Move_Translations[0];}
		break;
		case 18:
			{P0 = -Move_Translations[2]; P1 = -Move_Translations[1]; P2 = -Move_Translations[0];}
		break;
		case 47:
			{P0 = Move_Translations[1]; P1 = Move_Translations[0]; P2 = Move_Translations[2];}
		break;
		case 45:
			{P0 = Move_Translations[1]; P1 = Move_Translations[0]; P2 = -Move_Translations[2];}
		break;
		case 43:
			{P0 = -Move_Translations[1]; P1 = Move_Translations[0]; P2 = Move_Translations[2];}
		break;
		case 41:
			{P0 = -Move_Translations[1]; P1 = Move_Translations[0]; P2 = -Move_Translations[2];}
		break;
		case 48:
			{P0 = Move_Translations[1]; P1 = -Move_Translations[0]; P2 = Move_Translations[2];}
		break;
		case 46:
			{P0 = Move_Translations[1]; P1 = -Move_Translations[0]; P2 = -Move_Translations[2];}
		break;
		case 44:
			{P0 = -Move_Translations[1]; P1 = -Move_Translations[0]; P2 = Move_Translations[2];}
		break;
		case 42:
			{P0 = -Move_Translations[1]; P1 = -Move_Translations[0]; P2 = -Move_Translations[2];}
		break;
		case 7:
			{P0 = Move_Translations[1]; P1 = Move_Translations[2]; P2 = Move_Translations[0];}
		break;
		case 5:
			{P0 = Move_Translations[1]; P1 = -Move_Translations[2]; P2 = Move_Translations[0];}
		break;
		case 3:
			{P0 = -Move_Translations[1]; P1 = Move_Translations[2]; P2 = Move_Translations[0];}
		break;
		case 1:
			{P0 = -Move_Translations[1]; P1 = -Move_Translations[2]; P2 = Move_Translations[0];}
		break;
		case 8:
			{P0 = Move_Translations[1]; P1 = Move_Translations[2]; P2 = -Move_Translations[0];}
		break;
		case 6:
			{P0 = Move_Translations[1]; P1 = -Move_Translations[2]; P2 = -Move_Translations[0];}
		break;
		case 4:
			{P0 = -Move_Translations[1]; P1 = Move_Translations[2]; P2 = -Move_Translations[0];}
		break;
		case 2:
			{P0 = -Move_Translations[1]; P1 = -Move_Translations[2]; P2 = -Move_Translations[0];}
		break;
		case 31:
			{P0 = Move_Translations[2]; P1 = Move_Translations[0]; P2 = Move_Translations[1];}
		break;
		case 29:
			{P0 = -Move_Translations[2]; P1 = Move_Translations[0]; P2 = Move_Translations[1];}
		break;
		case 27:
			{P0 = Move_Translations[2]; P1 = Move_Translations[0]; P2 = -Move_Translations[1];}
		break;
		case 25:
			{P0 = -Move_Translations[2]; P1 = Move_Translations[0]; P2 = -Move_Translations[1];}
		break;
		case 32:
			{P0 = Move_Translations[2]; P1 = -Move_Translations[0]; P2 = Move_Translations[1];}
		break;
		case 30:
			{P0 = -Move_Translations[2]; P1 = -Move_Translations[0]; P2 = Move_Translations[1];}
		break;
		case 28:
			{P0 = Move_Translations[2]; P1 = -Move_Translations[0]; P2 = -Move_Translations[1];}
		break;
		case 26:
			{P0 = -Move_Translations[2]; P1 = -Move_Translations[0]; P2 = -Move_Translations[1];}
		break;
		case 15:
			{P0 = Move_Translations[0]; P1 = Move_Translations[2]; P2 = Move_Translations[1];}
		break;
		case 13:
			{P0 = Move_Translations[0]; P1 = -Move_Translations[2]; P2 = Move_Translations[1];}
		break;
		case 11:
			{P0 = Move_Translations[0]; P1 = Move_Translations[2]; P2 = -Move_Translations[1];}
		break;
		case 9:
			{P0 = Move_Translations[0]; P1 = -Move_Translations[2]; P2 = -Move_Translations[1];}
		break;
		case 16:
			{P0 = -Move_Translations[0]; P1 = Move_Translations[2]; P2 = Move_Translations[1];}
		break;
		case 14:
			{P0 = -Move_Translations[0]; P1 = -Move_Translations[2]; P2 = Move_Translations[1];}
		break;
		case 12:
			{P0 = -Move_Translations[0]; P1 = Move_Translations[2]; P2 = -Move_Translations[1];}
		break;
		case 10:
			{P0 = -Move_Translations[0]; P1 = -Move_Translations[2]; P2 = -Move_Translations[1];}
		break;
	};
	Move_Matrix34 -> v[3][0] += P0;
	Move_Matrix34 -> v[3][1] += P1;
	Move_Matrix34 -> v[3][2] += P2;
}

void CSOEdit2dView::OnViewShaded()
{
	m_DrawMode = r_texture;
	OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnViewWireframe()
{
	m_DrawMode = r_wire;
	OnUpdate(NULL, 0, NULL);
}

void CSOEdit2dView::OnColorize()
{
	CSOEditDoc* pDoc = GetDocument();
	pDoc -> Colorized_3d = !pDoc -> Colorized_3d;
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEdit2dView::DrawXYZLinesGL()
{
	v3_t vert[4];
	vert[0][0] = 0.0f;
    vert[0][1] = 0.0f;
    vert[0][2] = 0.0f;

	vert[1][0] = 50.0f;
    vert[1][1] = 0.0f;
    vert[1][2] = 0.0f;

	vert[2][0] = 0.0f;
    vert[2][1] = 50.0f;
    vert[2][2] = 0.0f;

	vert[3][0] = 0.0f;
    vert[3][1] = 0.0f;
    vert[3][2] = 50.0f;

	glNormal3f(0.0f, 1.0f, 0.0f);
	glColor3f(2.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3fv(vert[0]);
		glVertex3fv(vert[1]);
	glEnd();
	glColor3f(0.0f, 2.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3fv(vert[0]);
		glVertex3fv(vert[2]);
	glEnd();
	glColor3f(0.0f, 0.0f, 2.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
		glVertex3fv(vert[0]);
		glVertex3fv(vert[3]);
	glEnd();
}

