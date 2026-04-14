// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "SOEdit.h"

#include "MainFrm.h"

#include "SOEditDoc.h"
#include "SOEditView.h"
#include "CTexFile.h"
#include "CAnimFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CSOEditView

IMPLEMENT_DYNCREATE(CSOEditView, CView)

BEGIN_MESSAGE_MAP(CSOEditView, CView)
	//{{AFX_MSG_MAP(CSOEditView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONUP()

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()

	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_3D_SHADED, OnViewShaded)
	ON_COMMAND(ID_3D_SHADED_AL, OnViewShaded)
	ON_COMMAND(ID_3D_TEXTURED, OnViewTextured)
	ON_COMMAND(ID_3D_TEXTURED_AL, OnViewTextured)
	ON_COMMAND(ID_3D_WIREFRAME, OnViewWireframe)
	ON_COMMAND(ID_3D_WIREFRAME_AL, OnViewWireframe)
	ON_COMMAND(ID_3D_TEXTURES_OFF, OnViewTexturesOff)
	ON_COMMAND(ID_3D_TEXTURES_OFF_AL, OnViewTexturesOff)
	ON_COMMAND(ID_3D_WIREFRAME_TEXTURES_OFF, OnViewWireframeTexturesOff)
	ON_COMMAND(ID_3D_WIREFRAME_TEXTURES_OFF_AL, OnViewWireframeTexturesOff)
	ON_COMMAND(ID_3D_COLORIZE, OnColorize)
	ON_COMMAND(ID_3D_COLORIZE_AL, OnColorize)
	ON_COMMAND(ID_3D_COLLISIONSVIEWER, &CSOEditView::On3DCollisionsViewer)
	ON_COMMAND(ID_3D_COLLISIONSVIEWER_AL, &CSOEditView::On3DCollisionsViewer)
	ON_COMMAND(ID_3D_VISUAL, &CSOEditView::On3DVisual)
	ON_COMMAND(ID_3D_VISUAL_AL, &CSOEditView::On3DVisual)
	ON_COMMAND(ID_3D_HYBRID, &CSOEditView::On3DHybrid)
	ON_COMMAND(ID_3D_HYBRID_AL, &CSOEditView::On3DHybrid)
	ON_COMMAND(ID_VIEW3D_ENTITY, &CSOEditView::OnView3dEntity)
	ON_COMMAND(ID_VIEW3D_ENTITY_AL, &CSOEditView::OnView3dEntity)
	ON_COMMAND(ID_VIEW3D_BACKGROUND_GRID, &CSOEditView::OnView3dBackgroundGrid)
	ON_COMMAND(ID_VIEW3D_BACKGROUND_GRID_AL, &CSOEditView::OnView3dBackgroundGrid)
	ON_COMMAND(ID_3D_COLLISION_AS_WIREFRAME, &CSOEditView::On3dCollisionAsWireframe)
	ON_COMMAND(ID_3D_COLLISION_AS_WIREFRAME_AL, &CSOEditView::On3dCollisionAsWireframe)
	ON_COMMAND(ID_3D_ENTITY_AS_WIREFRAME, &CSOEditView::On3dEntityAsWireframe)
	ON_COMMAND(ID_3D_ENTITY_AS_WIREFRAME_AL, &CSOEditView::On3dEntityAsWireframe)
	ON_COMMAND(ID_3D_MESHS_AS_WIREFRAME, &CSOEditView::On3dMeshsAsWireframe)
	ON_COMMAND(ID_3D_MESHS_AS_WIREFRAME_AL, &CSOEditView::On3dMeshsAsWireframe)
	ON_COMMAND(ID_VIEW3D_WIREFRAME_OVER_MESH, &CSOEditView::OnView3dWireframeOverMesh)
	ON_COMMAND(ID_VIEW3D_WIREFRAME_OVER_MESH_AL, &CSOEditView::OnView3dWireframeOverMesh)
	ON_COMMAND(ID_VIEWING_PAINTSELECTEDMESHES, &CSOEditView::OnViewingPaintselectedmeshes)
	ON_COMMAND(ID_VIEWING_PAINTSELECTEDMESHES_AL, &CSOEditView::OnViewingPaintselectedmeshes)
	ON_COMMAND(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH, &CSOEditView::OnViewingDisplaythevectoroftheselectedmesh)
	ON_COMMAND(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH_AL, &CSOEditView::OnViewingDisplaythevectoroftheselectedmesh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSOEditView construction/destruction

CSOEditView::CSOEditView()
{
    m_pDoc = NULL;
	memset((void*)&m_point, 0, sizeof(POINT));
    m_hDC = 0;
    m_hRC = 0;
	m_rButton = m_mButton = m_lButton = false;
    m_glnWidth = 0;
    m_glnHeight = 0;
    m_gldAspect = 0.0f;
	m_Scale = 0.05f;
	
	//îñëàáèë ñ 0.25
    m_AmbientLight[0] = 0.225f;
    m_AmbientLight[1] = 0.225f;
    m_AmbientLight[2] = 0.225f;
    m_AmbientLight[3] = 1.0f;

	//óñèëèë íà 0.15
	m_DiffuseLight[0] = 0.75f;
	m_DiffuseLight[1] = 0.75f;
    m_DiffuseLight[2] = 0.75f;
    m_DiffuseLight[3] = 1.0f;

    m_Specular[0] = 0.50f;
    m_Specular[1] = 0.50f;
    m_Specular[2] = 0.50f; 
    m_Specular[3] = 1.50f;

    m_LightPos[0] = 0.0f;
    m_LightPos[1] = 0.0f;
    m_LightPos[2] = 200.0f;
    m_LightPos[3] = 0.0f;//

    /*m_ClearColor[0] = 0.5f;
    m_ClearColor[1] = 0.5f;
    m_ClearColor[2] = 0.5f;
    m_ClearColor[3] = 1.0f;*/

	//È êàê ýòî íàñòðàèâàòü?
    m_SpecRef[0] = 1.0f;
    m_SpecRef[1] = 1.0f;
    m_SpecRef[2] = 1.0f;
    m_SpecRef[3] = 1.0f;

    m_SceneGround = 0;

    m_Camera.Position[0] = 0.0f;
    m_Camera.Position[1] = -10.0f;
    m_Camera.Position[2] = -150.0f;
    m_Camera.Orient[0] = 0.0f;
    m_Camera.Orient[1] = 0.0f;
    m_Camera.Orient[2] = 0.0f;

	m_DrawMode = rl_ambient;
	m_ViewMod = vm3_visual;

	for(int i = 0; i < 20; i++)
		{OverlayStr[i] = "";}
	m_pDC = NULL;
	m_grid = true;
	m_ViewMeshVector = m_Paint_selected_meshes = m_WireframeOverMesh = m_ViewMeshAsWireframe = m_ViewEnts3dAsWireframe = m_vol_wireframe = false;
	Near_View = 2.0;//äâà ïîïóãàÿ
	Far_View = 10000.0;//10 òûñ. ïîïóãàåâ
	koef_light_diffuse = 1.25f;

	m_FlyMode = false;
	m_WASDTimer = 0;
	memset(m_keys, 0, sizeof(m_keys));
}

CSOEditView::~CSOEditView()
{
    if(m_hRC)
	{
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(m_hRC);
	}
}

BOOL CSOEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
    cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditView drawing

void CSOEditView::OnDraw(CDC* pDC)
{
	CSOEditDoc* pDoc = GetDocument();
	int type = 0; // 1- bone, 2 - volume
	ASSERT_VALID(pDoc);
	if(!m_pDoc -> ViewWorks[3])
	{
		OverlayStr[0] = "";
		m_pDoc -> ViewWorks[3] = 1;
		DrawScene();
		m_pDoc -> ViewWorks[3] = 0;

		if(m_pDoc && m_pDoc -> m_Model && m_pDoc -> m_Model -> m_skeleton && m_pDoc -> m_Model -> m_skeleton -> m_bonelist)
		{
			CBone *pSelect = m_pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(m_pDoc -> hSelTreeItem);
			if(pSelect)
			{
				type = 1;
				OverlayStr[0] = pSelect -> m_Name;
				if(pSelect -> m_VolumeView)
					{OverlayStr[0] += " [M]";}
				else
					{OverlayStr[0] += " [NM]";}
				if(!pSelect -> m_tree_check)
					{OverlayStr[0] += " [H]";}
				if(pSelect -> m_Animation && pSelect -> m_Animation -> m_SequenceList)
					{OverlayStr[0] += " [A]";}
			}
		}
		if(m_pDoc && m_pDoc -> m_Model && m_pDoc -> m_Model -> m_skeleton && m_pDoc -> m_Model -> m_skeleton -> m_bonelist)
		{
			CVolume *pSelect = m_pDoc -> m_Model -> m_VolumeList -> FindVolumeByTreeID(m_pDoc -> hSelTreeItem);
			if(pSelect)
			{
				type = 2;
				OverlayStr[0] = pSelect -> m_Name;
				if(pSelect -> m_Type == VT_PLY)
					{OverlayStr[0] += " [M]";}
				else
					{OverlayStr[0] += " [NM]";}
				if(!pSelect -> m_tree_check)
					{OverlayStr[0] += " [H]";}
			}
		}
		if(pDC)
		{
			pDC -> SetBkMode(OPAQUE);
			pDC -> SetBkColor(0x00);
			pDC -> SetTextColor(0x3F3F3F);
			if(type)
			{
				if(type == 1)
					{pDC -> SetTextColor(0x00ffff);}
				if(type == 2)
					{pDC -> SetTextColor(0xffff00);}
				pDC -> TextOut(2, 0, OverlayStr[0]);
			}
			for(int i = 1; i < 20; i++)
				{pDC -> TextOut(2, 17 * i, OverlayStr[i]);}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditView printing

BOOL CSOEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSOEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSOEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSOEditView diagnostics

#ifdef _DEBUG
void CSOEditView::AssertValid() const
	{CView::AssertValid();}

void CSOEditView::Dump(CDumpContext& dc) const
	{CView::Dump(dc);}

CSOEditDoc* CSOEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSOEditDoc)));
	return (CSOEditDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL CSOEditView::InitGL()
{
    int nMyPixelFormatID;
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
			::MessageBoxA(this -> m_hWnd, "Unable to set pixel format", "ERROR: CSOEditView::InitGL", MB_ICONHAND);
		#else
			::MessageBoxA(this -> m_hWnd, "Íåâîçìîæíî óñòàíîâèòü ïèêñåëüíûé ôîðìàò", "ERROR: CSOEditView::InitGL", MB_ICONHAND);
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
    CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
    if(pFrameWnd)
		{glClearColor(pFrameWnd -> m_Colors[C_BG3D][0] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][1] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][2] / 255.0f, 1.0f);}
    glEnable(GL_DEPTH_TEST);
    // Setup the backface polygon culling
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);
    //glCullFace(GL_BACK);
    // Turn on the lights
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT, m_AmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, m_DiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, m_Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, m_LightPos);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_SpecRef);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    glNewList(m_SceneGround = glGenLists(1), GL_COMPILE);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f(0.50f, 0.50f, 0.50f);    // 50% grey
        glBegin( GL_QUADS );
            glVertex3f(-25.0f, -15.0f, -25.0f);
            glVertex3f(-25.0f, -15.0f,  25.0f);
            glVertex3f( 25.0f, -15.0f,  25.0f);            
            glVertex3f( 25.0f, -15.0f, -25.0f);
        glEnd();
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f( 0.90f, 0.90f, 0.90f);    // 10% grey
        glBegin( GL_QUADS);
            glVertex3f(-25.0f,  10.0f, -25.0f);
            glVertex3f(-25.0f, -15.0f, -25.0f);
            glVertex3f( 25.0f, -15.0f, -25.0f);            
            glVertex3f( 25.0f,  10.0f, -25.0f);
        glEnd();
    glEndList();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // a perspective-view matrix...
    gluPerspective(45, m_gldAspect, Near_View, Far_View);
    glViewport(0, 0, m_glnWidth, m_glnHeight);
    wglMakeCurrent(NULL, NULL);
    return TRUE;
}

void CSOEditView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_pDC = GetDC();
	m_hDC = m_pDC -> m_hDC;
	InitGL();
}

void CSOEditView::DrawBone(CBone *basis, bool transparency_mod)
{
    CBone *child, *pSelect = NULL;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    int f, v;
	bool selected = false, anm_visi = true;
	if(m_pDoc -> m_SelBone)
	{
		pSelect = m_pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBoneByTreeID(m_pDoc -> hSelTreeItem);
		if(pSelect == basis)
			{selected = true;}
	}
    glPushMatrix();
	m_pDoc -> m_skin = false;
	if(!m_pDoc -> m_skin)
	{
		if(basis -> Matrix34)
		{
			int i, j;
			float m[16];
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 3; j++)
					{m[i * 4 + j] = basis -> Matrix34 -> v[i][j];}
				m[i * 4 + j] = 0.0f;
			}
			m[15] = 1.0f;
			glMultMatrixf((float *)&m);
		}
	}
	if(basis -> m_Skin)//Ýýýý?
	{
		CBone *pSkin = m_pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBone("skin");
		if(pSkin)
		{
			if(pSkin -> Matrix34)
			{
				int i, j;
				float m[16];
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 3; j++)
						{m[i * 4 + j] = pSkin -> Matrix34 -> v[i][j];}
					m[i * 4 + j] = 0.0f;
				}
				m[15] = 1.0f;
				glMultMatrixf((float *)&m);
			}
		}
	}
	if(m_pDoc -> m_AnimBone && basis != m_pDoc -> BLamp_View)
	{
		int iBone;
		for(iBone = 0; iBone < m_pDoc -> m_AnimBone -> m_BoneCnt; iBone++)
		{
			if(!stricmp(m_pDoc -> m_AnimBone -> m_BoneMap[iBone], basis -> m_Name))
				{break;}
		}
		CAnimSub *t_sub;
		if(iBone < m_pDoc -> m_AnimBone -> m_BoneCnt)
		{
			if(m_pDoc -> m_Frame >= m_pDoc -> m_AnimBone -> m_FrameCnt)
			{
				m_pDoc -> m_Frame = m_pDoc -> m_AnimBone -> m_FrameCnt - 1;
				if(m_pDoc -> m_Frame < 0)
					{m_pDoc -> m_Frame = 0;}
			}
			if((t_sub = m_pDoc -> m_AnimBone -> m_Frames[m_pDoc -> m_Frame].FindSub(iBone)) != NULL)
			{
				int i, j;
				basis -> m_anm_visi = t_sub -> m_Visible;
				if(!basis -> m_Animatrix)
					{basis -> m_Animatrix = new float[16];}
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 3; j++)
						{basis -> m_Animatrix[i * 4 + j] = t_sub -> m_Matrix34.v[i][j];}
					basis -> m_Animatrix[i * 4 + j] = 0.0f;
				}
				basis -> m_Animatrix[15] = 1.0f;
				glPopMatrix();
				glPushMatrix();
				glMultMatrixf(basis -> m_Animatrix);
				delete[] basis -> m_Animatrix;
				basis -> m_Animatrix = NULL;
			}
		}
	}
    glRotatef(basis -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
    glRotatef(basis -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
    glRotatef(basis -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
    glScalef(basis -> m_Scales[0], basis -> m_Scales[1], basis -> m_Scales[2]);
	if(m_pDoc -> m_AnimBone)
	{
		if(basis -> sub)
			{anm_visi = basis -> m_parent -> m_anm_visi;}
		else
			{anm_visi = basis -> m_anm_visi;}
	}
    if((basis -> m_VolumeView) && (!m_pDoc -> m_skin) && ((selected && (pWnd -> m_Selected && m_pDoc -> m_SelBone)) || (!(pWnd -> m_Selected && m_pDoc -> m_SelBone))) && basis -> m_tree_check)
	{
        long CR = 0, CG = 0, CB = 0;
		if(m_pDoc -> Colorized_3d || basis == m_pDoc -> BLamp_View || (m_Paint_selected_meshes && selected))
		{
			CR = GetRValue(basis -> m_bone_color);
			CG = GetGValue(basis -> m_bone_color);
			CB = GetBValue(basis -> m_bone_color);
		}
		else
			{CR = CG = CB = 255;}
        //glAlphaFunc(GL_GREATER, 50.0f / 255.0f); //åñëè blend - òî 0, åñëè test - òî 128
		int rep = 1;
		if(m_WireframeOverMesh && !m_ViewMeshAsWireframe && m_DrawMode != rl_wire && m_DrawMode != rl_ambient_tex_off)
			{rep = 2;}
		for(int iDraw = 0; iDraw < rep; iDraw++)
		{
			if(iDraw)
			{
				glPolygonMode(GL_FRONT, GL_LINE);
				glPolygonMode(GL_BACK, GL_LINE);
				glDisable(GL_LIGHTING);
				glDisable(GL_TEXTURE_2D);
				CR = GetRValue(basis -> m_bone_color);
				CG = GetGValue(basis -> m_bone_color);
				CB = GetBValue(basis -> m_bone_color);
			}
			else
				{glEnable(GL_TEXTURE_2D);}
			CMesh *pMesh = basis -> m_VolumeView -> m_meshlist -> GetFirst();
			while(pMesh)
			{
				bool alpha_blend = false;
				if(!iDraw)
				{
					if(pMesh -> m_texnumb[0] == -1 && pMesh -> m_texture[0])
					{
						CMaterial *pMaterial = m_pDoc -> m_MtlList.FindMaterial(pMesh -> m_texture[0] -> m_FrameName);
						if(pMaterial)
						{
							pMesh -> m_texnumb[0] = pMaterial -> m_iTexNumb;
							pMesh -> m_transparency = pMaterial -> m_transparency;
						}
					}
					if(pMesh -> m_texnumb[1] == -1 && pMesh -> m_texture[1])
					{
						CMaterial *pMaterial = m_pDoc -> m_MtlList.FindMaterial(pMesh -> m_texture[1] -> m_FrameName);
						if(pMaterial)
							{pMesh -> m_texnumb[1] = pMaterial -> m_iTexNumb;}
					}
					if(pMesh -> m_forced_alpha_test)
						{glEnable(GL_ALPHA_TEST);}
					else
						{glDisable(GL_ALPHA_TEST);}
					if(pMesh -> m_texture[0] -> m_Blend)//ïåðåäåëàòü ýòè õðåíè
					{
						glEnable(GL_ALPHA_TEST);
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						alpha_blend = true;
						if(!stricmp(pMesh -> m_texture[0] -> m_Blend, "test"))
							{glAlphaFunc(GL_GREATER, 128.0f / 255.0f);}
						else
						if(!stricmp(pMesh -> m_texture[0] -> m_Blend, "blend") || !stricmp(pMesh -> m_texture[0] -> m_Blend, "add_sub"))
							{glAlphaFunc(GL_GREATER, 0.0f);}
					}
					glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh -> m_texnumb[0]]);
				}
				if(pMesh -> m_fvf == 0x1158 || pMesh -> m_flags & MESH_FLAG_TWO_SIDED || m_ViewMeshAsWireframe || iDraw || m_DrawMode == rl_wire || m_DrawMode == rl_ambient_tex_off)
					{glDisable(GL_CULL_FACE);}
				//ïåðâàÿ î÷åðåäü ïîëèãîíà
				long RS = 0, GS = 0, BS = 0, AS = 0;
				DWORD DC = 0;
				if(m_ViewMeshAsWireframe)
				{
					glPolygonMode(GL_FRONT, GL_LINE);
					glPolygonMode(GL_BACK, GL_LINE);
				}
				for(f = pMesh -> m_first; f < pMesh -> m_count + pMesh -> m_first && ((!iDraw) ? anm_visi : true); f++)
				{
					if(transparency_mod == (alpha_blend && pMesh -> m_transparency))//êàêàÿ î÷åðåäü
					{
						glBegin(GL_POLYGON);
						
						for(v = 2; v >= 0; v--)
						{ 
							v2_t uv;
							uv[0] = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].uv[0][0];
							uv[1] = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].uv[0][1];
							glTexCoord2fv(uv);
							glNormal3fv(basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].vn);
							DC = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].diffuse;
							RS = GetRValue(DC);
							GS = GetGValue(DC);
							BS = GetBValue(DC);
							AS = GetAValue(DC);
							
							RS = (RS + CB) - 255;
							GS = (GS + CG) - 255;
							BS = (BS + CR) - 255;
							RS = max(0, RS);
							GS = max(0, GS);
							BS = max(0, BS);


							//RS = (CR - RS) * (long)(AS) / 255 + RS;
							//GS = (CG - GS) * (long)(AS) / 255 + GS;
							//BS = (CB - BS) * (long)(AS) / 255 + BS;

							glColor4f(((float)BS / 255.0f) * koef_light_diffuse, ((float)GS / 255.0f) * koef_light_diffuse, ((float)RS / 255.0f) * koef_light_diffuse, (float)AS / 255.0f);
							/*if(AS == 255)
								{glDepthMask(GL_TRUE);}
							else
								{glDepthMask(GL_FALSE);}*/
							glVertex3fv(basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].xyz);
						}
						glEnd();
					}
				}
				if(m_ViewMeshAsWireframe)
				{
					switch(m_DrawMode)
					{
						case rl_ambient:
						case rl_tex_off:
						case rl_diffuse:
						{
							glPolygonMode(GL_FRONT, GL_FILL);
							glPolygonMode(GL_BACK, GL_FILL);
						}
					};
				}
				/*glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh -> m_texnumb[1]]);
				if(basis -> m_VolumeView -> Bply)
				{
					glColor3f(1.0f, 1.0f, 1.0f);
					for(f = pMesh -> m_first; f < pMesh -> m_count + pMesh -> m_first; f++)
					{
						glBegin(GL_POLYGON);
						for(v = 2; v >= 0; v--)
						{ 
							v2_t uv;
							uv[0] = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].uv[0][0];
							uv[1] = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].uv[0][1];
							glTexCoord2fv(uv);
							if(basis -> m_VolumeView -> m_bones == 0)//çà÷åì çäåñü ýòî???
								{glNormal3fv(basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].vn);}
							/*DC = basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].diffuse;
							RS = GetRValue(DC);
							GS = GetGValue(DC);
							BS = GetBValue(DC);
							AS = GetAValue(DC);
							RS = (CR - RS) * (long)(AS) / 255 + RS;
							GS = (CG - GS) * (long)(AS) / 255 + GS;
							BS = (CB - BS) * (long)(AS) / 255 + BS;
							glColor4f((float)RS / 255.0f, (float)GS / 255.0f, (float)BS / 255.0f, (float)AS / 255.0f);*/
							/*glVertex3fv(basis -> m_VolumeView -> m_vertlist[basis -> m_VolumeView -> m_polylist[f].v[v]].xyz);
						}
						glEnd();
					}
				}*/
				if(!iDraw)
				{
					if(pMesh -> m_texture[0] -> m_Blend)
						{glDisable(GL_BLEND);}
					glBindTexture(GL_TEXTURE_2D, 0);///
				}
				if(pMesh -> m_fvf == 0x1158 || pMesh -> m_flags & MESH_FLAG_TWO_SIDED || m_ViewMeshAsWireframe || iDraw || m_DrawMode == rl_wire || m_DrawMode == rl_ambient_tex_off)
					{glEnable(GL_CULL_FACE);}
				pMesh = pMesh -> next;
			}
			if(iDraw)
			{
				glPolygonMode(GL_FRONT, GL_FILL);
				glPolygonMode(GL_BACK, GL_FILL);
				if(m_DrawMode == rl_ambient || m_DrawMode == rl_tex_off)
					{glEnable(GL_LIGHTING);}
			}
		}
		glDisable(GL_ALPHA_TEST);
		if(pWnd -> m_AllMeshVectors || (m_ViewMeshVector && selected))
			{DrawXYZLinesGL();}
	}
	else
	if((!basis -> m_VolumeView) && (!m_pDoc -> m_skin) && basis -> m_tree_check)
	{
		if(((selected && (pWnd -> m_Selected && m_pDoc -> m_SelBone)) || (!(pWnd -> m_Selected && m_pDoc -> m_SelBone))) && (pWnd -> m_ViewEnts3d))
		{
			if(selected)
				{glColor3f((pWnd -> m_Colors[C_ENTITYS][0] / 255.0f) * 3, (pWnd -> m_Colors[C_ENTITYS][1] / 255.0f) * 3, (pWnd -> m_Colors[C_ENTITYS][2] / 255.0f) * 3);}
			else
				{glColor3f((pWnd -> m_Colors[C_ENTITY][0] / 255.0f) * 3, (pWnd -> m_Colors[C_ENTITY][1] / 255.0f) * 3, (pWnd -> m_Colors[C_ENTITY][2] / 255.0f) * 3);}
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.0f); 
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_CULL_FACE);
			v3_t box;
			box[0] = 8.0f;
			box[1] = 5.0f;
			box[2] = 2.0f;
			DrawTrapGL(box);
			glEnable(GL_CULL_FACE);
			glDisable(GL_ALPHA_TEST);
		}
	}
    if(basis -> m_Skin)
	{
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glEnable(GL_TEXTURE_2D);
        CBone *pSkin = m_pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBone("skin");
        if(pSkin)
		{
            CMesh *pMesh = pSkin -> m_VolumeView -> m_meshlist -> GetFirst();
            if(pMesh -> m_texnumb[0] == -1)
			{
                CMaterial *pMaterial = m_pDoc -> m_MtlList.FindMaterial(pMesh -> m_texture[0] -> m_FrameName);
                if(pMaterial)
					{pMesh -> m_texnumb[0] = pMaterial -> m_iTexNumb;}
			}
            if(pMesh -> m_texture[0] -> m_Blend)
			{
				if(!stricmp("alpha", pMesh -> m_texture[0] -> m_Blend))
				{
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				}
			}
            glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh -> m_texnumb[0]]);
            int face;
            for(f = 0; f < basis -> m_Skin -> m_faces; f++)
			{
                face = basis -> m_Skin -> m_facelist[f];
                glBegin(GL_POLYGON);
                for(v = 2; v >= 0; v--)
				{ 
                    glTexCoord2fv(pSkin -> m_VolumeView -> m_vertlist[pSkin -> m_VolumeView -> m_polylist[face].v[v]].uv[0]);
                    glVertex3fv(pSkin -> m_VolumeView -> m_vertlist[pSkin -> m_VolumeView -> m_polylist[face].v[v]].xyz);
				}
                glEnd();
                glBegin(GL_POLYGON);
				for(v = 0; v < 2; v++)
				{ 
                    glTexCoord2fv(pSkin -> m_VolumeView -> m_vertlist[pSkin -> m_VolumeView -> m_polylist[face].v[v]].uv[0]);
                    glVertex3fv(pSkin -> m_VolumeView -> m_vertlist[pSkin -> m_VolumeView -> m_polylist[face].v[v]].xyz);
				}
				glEnd();
			}
			if(pMesh -> m_texture[0] -> m_Blend)
			{
				if(!stricmp("alpha", pMesh -> m_texture[0] -> m_Blend))
					{glDisable(GL_BLEND);}
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			pMesh = pMesh -> next;
		}
		glDisable(GL_ALPHA_TEST);
	}
	if((child = basis -> m_ChildFirst) != NULL)
	{
		while(child)
		{
			if(basis -> m_Skin)
			{
				glPopMatrix();
				glPushMatrix();
			}
			DrawBone(child, transparency_mod);
			child = child -> next;
		}
	}
    glPopMatrix();
}

/*void CSOEditView::DrawChunk(CBone *basis)
{
    CBone      *child;
    int         f, v;

    glPushMatrix();

    if (basis->Matrix34)
       {
        int        i, j;
        float      m[16];

        for (i = 0; i < 4; i++)
           {
            for (j = 0; j < 3; j++)
               {
                m[i*4+j] = basis->Matrix34->v[i][j];
               }
            m[i*4+j] = 0.0f;
           }
        m[15] = 1.0f;

        glMultMatrixf((float *)&m);
       }

    if (basis->m_Skin)
       {
        CBone *pSkin = m_pDoc->m_Model->m_skeleton->m_bonelist->FindBone("skin");
        if (pSkin)
           {
            glPopMatrix();
            glPushMatrix();

            if (pSkin->Matrix34)
               {
                int        i, j;
                float      m[16];
 
                for (i = 0; i < 4; i++)
                   {
                    for (j = 0; j < 3; j++)
                       {
                        m[i*4+j] = pSkin->Matrix34->v[i][j];
                       }
                    m[i*4+j] = 0.0f;
                   }
                m[15] = 1.0f;

                glMultMatrixf((float *)&m);
               }
           }
       }

    if (m_pDoc->m_AnimBone)
       {
        int iBone;
   
        for (iBone = 0; iBone < m_pDoc->m_AnimBone->m_BoneCnt; iBone++)
           {
            if (!stricmp(m_pDoc->m_AnimBone->m_BoneMap[iBone], basis->m_Name))
               {
                break;
               }
           }

        CAnimSub *t_sub;
        if (iBone < m_pDoc->m_AnimBone->m_BoneCnt)
           {
            if (NULL != (t_sub = m_pDoc->m_AnimBone->m_Frames[m_pDoc->m_Frame].FindSub(iBone)))
               {
                int        i, j;
                if (NULL == basis->m_Animatrix)
                   {
                    basis->m_Animatrix = new float[16];
                   }

                for (i = 0; i < 4; i++)
                   {
                    for (j = 0; j < 3; j++)
                       {
                        basis->m_Animatrix[i*4+j] = t_sub->m_Matrix34.v[i][j];
                       }
                    basis->m_Animatrix[i*4+j] = 0.0f;
                   }
                basis->m_Animatrix[15] = 1.0f;

                //if (NULL == basis->m_Skin)
                   //{
                    glPopMatrix();
                    glPushMatrix();
                   //}
                glMultMatrixf(basis->m_Animatrix);
               }
           }
       }

    //glTranslatef(basis->m_Translations[0], basis->m_Translations[1], basis->m_Translations[2]);//
    glRotatef(basis->m_Rotations[0], 1.0f, 0.0f, 0.0f);
    glRotatef(basis->m_Rotations[1], 0.0f, 1.0f, 0.0f);
    glRotatef(basis->m_Rotations[2], 0.0f, 0.0f, 1.0f);
    glScalef(basis->m_Scales[0], basis->m_Scales[1], basis->m_Scales[2]);

    if ((basis->m_VolumeView) && (false == m_pDoc->m_skin))
       {
        CMesh *pMesh = basis->m_VolumeView->m_meshlist->GetFirst();

        glColor3f( 1.0f, 1.0f, 1.0f );

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);

        glEnable(GL_TEXTURE_2D);
        while (NULL != pMesh)
           {
            if (-1 == pMesh->m_texnumb[0])
               {
                CMaterial *pMaterial = m_pDoc->m_MtlList.FindMaterial(pMesh->m_texture[0]->m_FrameName);
                if (NULL != pMaterial)
                   {
                    pMesh->m_texnumb[0] = pMaterial->m_iTexNumb;
                   }
               }
            if (pMesh->m_texture[0]->m_Blend)
               {
                if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
                   {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                   }
               }
            glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh->m_texnumb[0]]);
            for (f = pMesh->m_first; f < pMesh->m_count+pMesh->m_first; f++)
               {
                glBegin( GL_POLYGON );
                for (v = 2; v >= 0; v--)
                   { 
                    v2_t uv;
					uv[0] = basis->m_VolumeView->m_vertlist[basis->m_VolumeView->m_polylist[f].v[v]].uv[0];
					uv[1] = basis->m_VolumeView->m_vertlist[basis->m_VolumeView->m_polylist[f].v[v]].uv[1];

                    glTexCoord2fv(uv);
                    if (0 == basis->m_VolumeView->m_bones)
                       {
                        glNormal3fv(basis->m_VolumeView->m_vertlist[basis->m_VolumeView->m_polylist[f].v[v]].vn);
                       }
                    glVertex3fv(basis->m_VolumeView->m_vertlist[basis->m_VolumeView->m_polylist[f].v[v]].xyz);
                   }
                glEnd();
               }
            if (pMesh->m_texture[0]->m_Blend)
               {
                if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
                   {
                    glDisable(GL_BLEND);
                   }
               }
            glBindTexture(GL_TEXTURE_2D, 0);
            pMesh = pMesh->next;
           }

        glDisable(GL_ALPHA_TEST);
       }

    if (basis->m_Skin)
       {
        glColor3f( 1.0f, 1.0f, 1.0f );

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);

        glEnable(GL_TEXTURE_2D);

        CBone *pSkin = m_pDoc->m_Model->m_skeleton->m_bonelist->FindBone("skin");
        if (pSkin)
           {
            CMesh *pMesh = pSkin->m_VolumeView->m_meshlist->GetFirst();
            if (-1 == pMesh->m_texnumb[0])
               {
                CMaterial *pMaterial = m_pDoc->m_MtlList.FindMaterial(pMesh->m_texture[0]->m_FrameName);
                if (NULL != pMaterial)
                   {
                    pMesh->m_texnumb[0] = pMaterial->m_iTexNumb;
                   }
               }
            if (pMesh->m_texture[0]->m_Blend)
               {
                if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
                   {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                   }
               }
            glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh->m_texnumb[0]]);

            int face;
            for (f = 0; f < basis->m_Skin->m_faces; f++)
               {
                face = basis->m_Skin->m_facelist[f];
                glBegin( GL_POLYGON );
                for (v = 2; v >= 0; v--)
                   { 
                    glTexCoord2fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].uv);
                    glVertex3fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].xyz);
                   }
                glEnd();
               }

            if (pMesh->m_texture[0]->m_Blend)
               {
                if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
                   {
                    glDisable(GL_BLEND);
                   }
               }
            glBindTexture(GL_TEXTURE_2D, 0);
            pMesh = pMesh->next;
           }

        glDisable(GL_ALPHA_TEST);
       }

    if (NULL != (child = basis->m_ChildFirst))
       {
        while (child)
           {
            DrawBone(child);
            child = child->next;
           }
       }

    glPopMatrix();
}*/

/*void CSOEditView::DrawSkin(CBone *pSkin)
{
    int         f, v;

    glPushMatrix();
    glColor3f( 1.0f, 1.0f, 1.0f );

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glEnable(GL_TEXTURE_2D);

    CMesh *pMesh = pSkin->m_VolumeView->m_meshlist->GetFirst();
    if (-1 == pMesh->m_texnumb[0])
       {
        CMaterial *pMaterial = m_pDoc->m_MtlList.FindMaterial(pMesh->m_texture[0]->m_FrameName);
        if (NULL != pMaterial)
           {
            pMesh->m_texnumb[0] = pMaterial->m_iTexNumb;
           }
       }
    if (pMesh->m_texture[0]->m_Blend)
       {
        if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
           {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
           }
       }
    glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[pMesh->m_texnumb[0]]);

    if (m_pDoc->m_AnimBone)
       {
        for (int b = 0; b < m_pDoc->m_AnimBone->m_BoneCnt; b++)
           {
            CBone *pBone = m_pDoc->m_Model->m_skeleton->m_bonelist->FindBone(m_pDoc->m_AnimBone->m_BoneMap[b]);

            if (NULL == pBone)
               {
                TRACE("ERROR: Animation refers to a bone [%s] that does not exist!", m_pDoc->m_AnimBone->m_BoneMap[b]);
                continue;
               }

            if (NULL == pBone->m_Skin)
               {
                continue;
               }

            CAnimSub *pAnimSub = m_pDoc->m_AnimBone->m_Frames[m_pDoc->m_Frame].FindSub(b);
            if (pAnimSub)
               {
                int        i, j;
                if (NULL == pBone->m_Animatrix)
                   {
                    pBone->m_Animatrix = new float[16];
                   }

                for (i = 0; i < 4; i++)
                   {
                    for (j = 0; j < 3; j++)
                       {
                        pBone->m_Animatrix[i*4+j] = pSkin->Matrix34->v[i][j];
                       }
                    pBone->m_Animatrix[i*4+j] = 0.0f;
                   }
                pBone->m_Animatrix[15] = 1.0f;
               }
            glMultMatrixf((float *)&pBone->m_Animatrix);

            int face;
            for (f = 0; f < pBone->m_Skin->m_faces; f++)
               {
                face = pBone->m_Skin->m_facelist[f];
                glBegin( GL_POLYGON );
                for (v = 2; v >= 0; v--)
                   { 
                    glTexCoord2fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].uv[0]);
                    glVertex3fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].xyz);
                   }
                glEnd();
               } 
           }
       }

/*
    for (int b = 0; b < pSkin->m_VolumeView->m_bones; b++)
       {
        glPushMatrix();

        CBone *pBone = m_pDoc->m_Model->m_skeleton->m_bonelist->FindBone(pSkin->m_VolumeView->m_bonelist[b]);

        if (NULL == pBone)
           {
            TRACE("Bone %s from model not found.", pSkin->m_VolumeView->m_bonelist[b]);
            continue;
           }

        if (pBone->m_Skin)
           {
            if (m_pDoc->m_AnimBone)
               {
                int iBone;
   
                for (iBone = 0; iBone < m_pDoc->m_AnimBone->m_BoneCnt; iBone++)
                   {
                    if (!stricmp(m_pDoc->m_AnimBone->m_BoneMap[iBone], pBone->m_Name))
                       {
                        break;
                       }
                   }

                CAnimSub *t_sub;
                if (iBone < m_pDoc->m_AnimBone->m_BoneCnt)
                   { 
                    if (NULL != (t_sub = m_pDoc->m_AnimBone->m_Frames[m_pDoc->m_Frame].FindSub(iBone)))
                       {
                        int        i, j;

                        for (i = 0; i < 4; i++)
                           {
                            for (j = 0; j < 3; j++)
                               {
                                pBone->m_Animatrix[i*4+j] = t_sub->m_Matrix34.v[i][j];
                               }
                            pBone->m_Animatrix[i*4+j] = 0.0f;
                           }
                        pBone->m_Animatrix[15] = 1.0f;
 
                        glPopMatrix();
                        glPushMatrix();
                       }
                    glMultMatrixf(pBone->m_Animatrix);
                   }
               }

            int face;
            for (f = 0; f < pBone->m_Skin->m_faces; f++)
               {
                face = pBone->m_Skin->m_facelist[f];
                glBegin( GL_POLYGON );
                for (v = 2; v >= 0; v--)
                   { 
                    glTexCoord2fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].uv2);
                    glVertex3fv(pSkin->m_VolumeView->m_vertlist[pSkin->m_VolumeView->m_polylist[face].v[v]].xyz);
                   }
                glEnd();
               } 
           }
        glPopMatrix();
       }
*//*
    if (pMesh->m_texture[0]->m_Blend)
       {
        if (!stricmp("alpha", pMesh->m_texture[0]->m_Blend))
           {
            glDisable(GL_BLEND);
           }
       }
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

    glPopMatrix();
}*/
   
void CSOEditView::DrawScene()
{
    if(!m_pDoc)
		{m_pDoc = GetDocument();}
    wglMakeCurrent(m_hDC, m_hRC);
	if(m_pDoc -> m_NewModel)
		{m_DrawMode = rl_ambient;}
    // Be sure all the textures are loaded so we can render the object
    CMaterial *pMaterial = m_pDoc -> m_MtlList.GoToHead();
	while(pMaterial)
	{
		if(pMaterial -> m_iTexNumb == -1)
		{
			pMaterial -> m_iTexNumb = m_pDoc -> m_texCount;
			LoadTexture(pMaterial -> m_szName, m_pDoc -> m_texCount, pMaterial);
		}
		pMaterial = m_pDoc -> m_MtlList.GetNext();
	}
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	CMainFrame *pFrameWnd = (CMainFrame *)AfxGetMainWnd();
	if(pFrameWnd)
		{glClearColor(pFrameWnd -> m_Colors[C_BG3D][0] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][1] / 255.0f, pFrameWnd -> m_Colors[C_BG3D][2] / 255.0f, 1.0f);}
	
	// Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // Define the modelview transformation.
	koef_light_diffuse = 1.0f;
	switch(m_DrawMode)
	{
		case rl_ambient:
			{koef_light_diffuse = 1.25f;}
		//break íå ñòàâèòü!
		case rl_tex_off:
		{
			glEnable(GL_LIGHTING);
			glLightfv(GL_LIGHT0, GL_AMBIENT, m_AmbientLight);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, m_DiffuseLight);
			glLightfv(GL_LIGHT0, GL_SPECULAR, m_Specular);
			glLightfv(GL_LIGHT0, GL_POSITION, m_LightPos);
			glEnable(GL_LIGHT0);
		}
		break;
		case rl_diffuse:
		{
			glDisable(GL_LIGHTING);
			koef_light_diffuse = 1.25f;
		}
		break;
		case rl_wire:
		case rl_ambient_tex_off:
		{
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK, GL_LINE);
			glDisable(GL_LIGHTING);
		}
		break;
	};
	//glEnable(GL_NORMALIZE);
	if(pFrameWnd)
		{gluPerspective(pFrameWnd -> m_fov, m_gldAspect, Near_View, Far_View);}
	else
		{gluPerspective(45, m_gldAspect, Near_View, Far_View);}
	glViewport(0, 0, m_glnWidth, m_glnHeight);
	if(m_pDoc -> ResetCamView[3])
	{
		m_Camera.Position[0] = 0.0f;
		m_Camera.Position[1] = -10.0f;
		m_Camera.Position[2] = -150.0f;
		m_Camera.Orient[0] = 0.0f;
		m_Camera.Orient[1] = 0.0f;
		m_Camera.Orient[2] = 0.0f;
		m_pDoc -> ResetCamView[3] = false;
	}
	//float ZoomFactor = (m_scales[m_Scale] * 5);
	float ZoomFactor = (m_Scale * 5);
	// a perspective-view matrix...
	glRotatef(m_Camera.Orient[0], 1.0f, 0.0f, 0.0f);
	glRotatef(m_Camera.Orient[1], 0.0f, 1.0f, 0.0f);
	glRotatef(m_Camera.Orient[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(m_Camera.Position[0], m_Camera.Position[1], m_Camera.Position[2] * ZoomFactor * 5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	if(m_grid)
		{DrawGridGL();}
    glRotatef(m_pDoc -> model[0], 1.0f, 0.0f, 0.0f);
    glRotatef(m_pDoc -> model[1], 0.0f, 1.0f, 0.0f);
    glRotatef(m_pDoc -> model[2], 0.0f, 0.0f, 1.0f);
    m_reference[0] = 0.0f;
    m_reference[1] = 0.0f;
    m_reference[2] = 0.0f;
    glPushMatrix();
	if(m_pDoc -> fix_mx_ori)
	{
		anm_normalizer();//Äà-äà... Âîò òàê âîò ïðîñòî èñïðàâëÿåì ìàòðèöó ïðÿìî â ïðîöåññå ðåíäåðà...
		m_pDoc -> fix_mx_ori = false;
		//TODO:
		//ïåðåäåëàòü ýòîò òðýø
		//...
		//À õîòÿ... è òàê ñîéä¸ò...
		//...
		//Ïîèñêàòü ìåòîä ðåøåíèÿ ïðîáëåìû
	}
	if(m_pDoc -> BLamp_View && pFrameWnd -> m_lamp)
	{
		m_pDoc -> BLamp_View -> Matrix34 -> v[3][0] = m_LightPos[0];
		m_pDoc -> BLamp_View -> Matrix34 -> v[3][1] = m_LightPos[1];
		m_pDoc -> BLamp_View -> Matrix34 -> v[3][2] = m_LightPos[2] - 2.0f;
		DrawBone(m_pDoc -> BLamp_View, false);
	}
	if(m_pDoc -> m_Model)// a perspective-view matrix...
	{
		glScalef(1.0f, -1.0f, 1.0f);
		if(m_pDoc -> m_Model -> m_skeleton)
		{
			if(m_pDoc -> m_Model -> m_skeleton -> m_bonelist)
			{
				if(m_vol_wireframe)
				{
					if(m_ViewMod == vm3_collision || m_ViewMod == vm3_hybrid)
						{RenderCollision();}
					if(m_ViewMod == vm3_visual || m_ViewMod == vm3_hybrid)
						{RenderVisual(false); RenderVisual(true);}
				}
				else
				{
					if(m_ViewMod == vm3_visual || m_ViewMod == vm3_hybrid)
						{RenderVisual(false); }
					if(m_ViewMod != vm3_collision)
						{RenderVisual(true);}
					if(m_ViewMod == vm3_collision || m_ViewMod == vm3_hybrid)
						{RenderCollision();}
				}
				/*if(m_pDoc->m_skin)
				{
					CBone *pSkin = m_pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBone("skin");
					if(pSkin)
						{DrawSkin(pSkin);}
				}
				else
					{DrawBone(m_pDoc->m_Model->m_skeleton->m_bonelist);}
				*/
			}
		}
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// set up a projection matrix to fill the client window
	// Swap the rendering buffers...
	glFinish();
	SwapBuffers(m_hDC);
	wglMakeCurrent(NULL, NULL);
	m_pDoc -> m_NewModel = false;
}

void CSOEditView::RenderCollision()
{
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f); 
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);
	if(m_vol_wireframe)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	CBone *Nlist = m_pDoc -> m_Model -> m_skeleton -> m_bonelist;
	while(Nlist)
	{
		DrawVolumeLayer(Nlist);
		Nlist = Nlist -> next;
	}
	glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	if(m_vol_wireframe && m_DrawMode != rl_wire && m_DrawMode != rl_ambient_tex_off)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
}

void CSOEditView::RenderVisual(bool transparency_mod)
{
	CBone *Nlist = m_pDoc -> m_Model -> m_skeleton -> m_bonelist;
	while(Nlist && !transparency_mod)
	{
		DrawBone(Nlist, transparency_mod);
		Nlist = Nlist -> next;
	}
	Nlist = m_pDoc -> m_Model -> m_skeleton -> m_bonelist;
	while(Nlist && transparency_mod)
	{
		DrawBone(Nlist, transparency_mod);
		Nlist = Nlist -> next;
	}
}

void CSOEditView::OnMouseMove(UINT nFlags, CPoint point)
{
	//nFlags & MK_LBUTTON nFlags & MK_RBUTTON nFlags & MK_MBUTTON

	if (m_FlyMode)
	{
		int dx = point.x - (m_FlyAnchor.x - 0); // anchor is in screen coords
		// convert anchor to client for comparison
		CPoint clientAnchor = m_FlyAnchor;
		ScreenToClient(&clientAnchor);
		dx = point.x - clientAnchor.x;
		int dy = point.y - clientAnchor.y;
		if (dx != 0 || dy != 0)
		{
			const float sensitivity = 0.25f;
			m_Camera.Orient[0] += dy * sensitivity;   // pitch
			m_Camera.Orient[1] += dx * sensitivity;   // yaw
			// Clamp pitch
			if (m_Camera.Orient[0] > 89.0f) m_Camera.Orient[0] = 89.0f;
			if (m_Camera.Orient[0] < -89.0f) m_Camera.Orient[0] = -89.0f;
			// Wrap yaw
			if (m_Camera.Orient[1] > 180.0f) m_Camera.Orient[1] -= 360.0f;
			if (m_Camera.Orient[1] < -180.0f) m_Camera.Orient[1] += 360.0f;
			// Re-centre cursor
			SetCursorPos(m_FlyAnchor.x, m_FlyAnchor.y);
			RECT rc; GetClientRect(&rc);
			InvalidateRect(&rc, false);
		}
		return;  // skip old mouse-drag logic while in fly mode
	}

	OverlayStr[12] = "";
	OverlayStr[13] = "";
	POINT delta;
	bool k_ShButton = false, k_CoButton = false;
	if(!(nFlags & MK_LBUTTON))
		{m_lButton = false;}
	if(!(nFlags & MK_MBUTTON))
		{m_mButton = false;}
	if(!(nFlags & MK_RBUTTON))
		{m_rButton = false;}
	if(nFlags & MK_CONTROL)
		{k_CoButton = true;}
	if(nFlags & MK_SHIFT)
		{k_ShButton = true;}
	if(!m_lButton && !m_mButton && !m_rButton && !k_CoButton && !k_ShButton)
	{
		CView::OnMouseMove(nFlags, point);
		return;
	}
	m_pDoc = GetDocument();
	delta.x = point.x;
	delta.y = point.y;
	if(!m_lButton && !m_rButton && m_mButton && (nFlags & MK_CONTROL))
	{//zoom in and out
		float Angle = m_Camera.Orient[1];
		float Angle2 = m_Camera.Orient[0];
		//----------------------------------------
		if(Angle < 0)
			{Angle += 360;}
		else
		if(Angle > 360)
			{Angle -= 360;}
		//----------------------------------------
		if(Angle2 < 0)
			{Angle2 += 360;}
		else
		if(Angle2 > 360)
			{Angle2 -= 360;}
		//----------------------------------------
		float radians = (float)((double)(2 * 3.1416 * Angle) / 360);
		float cosA = cos(radians);
		float sinA = sin(radians);
		float outputX = 0, outputY = 0;
		outputX -= (1.0f * cosA);
		outputY -= (1.0f * sinA);
		float radians2 = (float)((double)(2 * 3.1416 * Angle2) / 360);
		float sinA2 = sin(radians2);
		if((delta.y - m_point.y) < 0)
		{
			m_Camera.Position[0] += outputY;
			m_Camera.Position[1] += (2.0f * sinA2);
			m_Camera.Position[2] -= outputX;
		}
		else
		if((delta.y - m_point.y) > 0)
		{
			m_Camera.Position[0] -= outputY;
			m_Camera.Position[1] -= (2.0f * sinA2);
			m_Camera.Position[2] += outputX;
		}
		m_point.y = delta.y;
		m_point.x = delta.x;
	}
	else
	if(!m_lButton && !m_rButton && m_mButton)
	{
		// move camera
		float inputZ = (delta.y - m_point.y) * 0.5f;
		float outputX = 0, outputY = 0;
		float Angle = m_Camera.Orient[1];
		float Angle2 = m_Camera.Orient[0];
		float inputX = (delta.x - m_point.x) * 0.5f;
		//----------------------------------------
		if(Angle < 0)
			{Angle += 360;}
		else
		if(Angle > 360)
			{Angle -= 360;}
		//----------------------------------------
		if(Angle2 < 0)
			{Angle2 += 360;}
		else
		if(Angle2 > 360)
			{Angle2 -= 360;}
		//----------------------------------------
		float radians = (float)((double)(2 * 3.1416 * Angle) / 360);
		float radians2 = (float)((double)(2 * 3.1416 * Angle2) / 360);
		float cosA = cos(radians);
		float sinA = sin(radians);
		float cosA2 = cos(radians2);
		float sinA2 = sin(radians2);
		float KoefInputZ = inputZ * sinA2;
		outputX -= (inputX * cosA);
		outputY -= (inputX * sinA);
		outputX += (KoefInputZ * sinA);
		outputY -= (KoefInputZ * cosA);
		/*OverlayStr[12].Format("outputX = %g", outputX);
		OverlayStr[13].Format("outputY = %g", outputY);*/
		m_Camera.Position[0] -= outputX;
		m_Camera.Position[2] -= outputY;
		m_Camera.Position[1] -= (inputZ * cosA2);//Z
		m_point.y = delta.y;
		m_point.x = delta.x;
	}
	else
	if(!m_mButton && !m_rButton && m_lButton)
	{
		if(nFlags & MK_CONTROL && !(nFlags & MK_SHIFT))//rotate camera
		{
			m_Camera.Orient[0] += delta.y - m_point.y;
			m_Camera.Orient[2] -= delta.x - m_point.x;
		}
		else
		if(nFlags & MK_SHIFT)//rotate light
		{
			float Angle = m_Camera.Orient[1];
			float outputX = 0, outputY = 0;
			float inputY = (delta.y - m_point.y) * 0.5f;
			float inputX = (delta.x - m_point.x) * 0.5f;
			//----------------------------------------
			if(Angle < 0)
				{Angle += 360;}
			else
			if(Angle > 360)
				{Angle -= 360;}
			//----------------------------------------
			float radians = (float)((double)(2 * 3.1416 * Angle) / 360);
			float cosA = cos(radians);
			float sinA = sin(radians);
			outputX += (inputY * cosA);
			outputY -= (inputY * sinA);
			outputY += (inputX * cosA);
			outputX += (inputX * sinA);
			if(nFlags & MK_CONTROL)
				{m_LightPos[2] -= inputY;}
			else
			{
				m_LightPos[0] += outputX;
				m_LightPos[1] += outputY;
			}
		}
		else// rotate model
		{
			m_Camera.Orient[0] += delta.y - m_point.y;
			m_Camera.Orient[1] += delta.x - m_point.x;
			
			/*float inputZ = (delta.y - m_point.y) * 1.0f;
			float outputX = 0, outputY = 0;
			float Angle = m_Camera.Orient[1];
			float Angle2 = m_Camera.Orient[0];
			float inputX = (delta.x - m_point.x) * 1.0f;
			//----------------------------------------
			if(Angle < 0)
				{Angle += 360;}
			else
			if(Angle > 360)
				{Angle -= 360;}
			//----------------------------------------
			if(Angle2 < 0)
				{Angle2 += 360;}
			else
			if(Angle2 > 360)
				{Angle2 -= 360;}
			//----------------------------------------
			float radians = (2 * 3.1416 * Angle) / 360;
			float radians2 = (2 * 3.1416 * Angle2) / 360;
			float cosA = cos(radians);
			float sinA = sin(radians);
			float cosA2 = cos(radians2);
			float sinA2 = sin(radians2);
			float KoefInputZ = inputZ * sinA2;
			outputX -= ((inputX * cosA) * cosA2);
			outputY -= ((inputX * sinA) * cosA2);
			/*outputX += (KoefInputZ * sinA);
			outputY -= (KoefInputZ * cosA);*/
			/*m_Camera.Position[0] -= outputX;
			m_Camera.Position[2] -= outputY;
			//m_Camera.Position[1] -= (inputZ * cosA2);//Z
			*/

		}
		if(m_Camera.Orient[0] > 180)
			{m_Camera.Orient[0] -= 360;}
		if(m_Camera.Orient[0] < -180)
			{m_Camera.Orient[0] += 360;}
		if(m_Camera.Orient[1] > 180)
			{m_Camera.Orient[1] -= 360;}
		if(m_Camera.Orient[1] < -180)
			{m_Camera.Orient[1] += 360;}
		m_point.y = delta.y;
		m_point.x = delta.x;
	}
	//îòëàäêà
	float Angle = m_Camera.Orient[1], Angle2 = m_Camera.Orient[0];
	if(Angle < 0)
		{Angle += 360;}
	else
	if(Angle > 360)
		{Angle -= 360;}
	if(Angle2 < 0)
		{Angle2 += 360;}
	else
	if(Angle2 > 360)
		{Angle2 -= 360;}
	float radians = (float)((double)(2 * 3.1416 * Angle) / 360);
	float radians2 = (float)((double)(2 * 3.1416 * Angle2) / 360);
	float cosA = cos(radians);
	float sinA = sin(radians);
	float cosA2 = cos(radians2);
	float sinA2 = sin(radians2);

	/*OverlayStr[1].Format("Angle = %g", Angle);
	OverlayStr[2].Format("Radians = %g", radians);
	OverlayStr[3].Format("cosA = %g", cosA);
	OverlayStr[4].Format("sinA = %g", sinA);

	OverlayStr[5].Format("Angle2 = %g", Angle2);
	OverlayStr[6].Format("Radians2 = %g", radians2);
	OverlayStr[7].Format("cosA2 = %g", cosA2);
	OverlayStr[8].Format("sinA2 = %g", sinA2);

	OverlayStr[9].Format("m_Camera.Orient[0] = %g", m_Camera.Orient[0]);
	OverlayStr[10].Format("m_Camera.Orient[1] = %g", m_Camera.Orient[1]);
	OverlayStr[11].Format("m_Camera.Orient[2] = %g", m_Camera.Orient[2]);

	OverlayStr[14].Format("m_Camera.Position = %g", m_Camera.Position[0]);
	OverlayStr[15].Format("m_Camera.Position = %g", m_Camera.Position[1]);
	OverlayStr[16].Format("m_Camera.Position = %g", m_Camera.Position[2]);*/

	RECT rect;
	GetClientRect(&rect);
	InvalidateRect(&rect, false);
	CView::OnMouseMove(nFlags, point);
}

void CSOEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_point.x = point.x;
	m_point.y = point.y;
	m_lButton = true;
	CView::OnLButtonDown(nFlags, point);
}

void CSOEditView::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_point.x = point.x;
	m_point.y = point.y;
	m_mButton = true;
	CView::OnMButtonDown(nFlags, point);
}

void CSOEditView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	SetFocus();
	m_point.x = point.x;
	m_point.y = point.y;
	m_rButton = true;
	m_RButtonDownTime = GetTickCount();

	// Don't enter fly mode yet — wait to see if this is a hold or a click.
	// A WM_TIMER (ID=2) fires after 200ms; if RMB is still held, activate fly mode.
	SetTimer(2, 200, NULL);
}

void CSOEditView::LoadTexture(char *texfile, int texnumb, CMaterial *pMaterial)
{
    //MessageBox(texfile, "LoadTexture: Loading texture file", MB_OK);
    CTexFile *tex = new CTexFile(texfile);
    if(!tex -> GetBits())
	{
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		if (!pWnd || pWnd->m_ShowTextureErrors)
		{
#ifdef ALTERNATIVE_LANG
			MessageBox(CString("Failed to load texture file:\\n" + CString(texfile)), "ERROR: CSOEditView::LoadTexture", MB_ICONHAND);
#else
			MessageBox(CString("Failed to load texture file:\\n" + CString(texfile)), "ERROR: CSOEditView::LoadTexture", MB_ICONHAND);
#endif
		}
	}
	pMaterial -> m_transparency = tex -> m_transparency;
	if(m_pDoc -> TexOff)
		{memcpy(m_pDoc -> texName, m_pDoc -> tmptexName, sizeof(unsigned int) * 256);}
    glGenTextures(1, &m_pDoc -> texName[texnumb]);
    glBindTexture(GL_TEXTURE_2D, m_pDoc -> texName[texnumb]);
	if(m_pDoc -> TexOff)
	{
		memcpy(m_pDoc -> tmptexName, m_pDoc -> texName, sizeof(unsigned int) * 256);
		memset(m_pDoc -> texName, 0, sizeof(unsigned int) * 256);
	}
    m_pDoc -> m_texCount++;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Define the 2D texture image.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	// Force 4-byte alignment
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    if(tex -> GetBPP() == 24)
		{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex -> GetWidth(), tex -> GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex -> GetBits());}
    else
    if(tex -> GetBPP() == 32)
		{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex -> GetWidth(), tex -> GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex -> GetBits());}
    delete tex;
}

void CSOEditView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
    RECT rect;
    GetClientRect(&rect);
    m_glnWidth = rect.right;
    m_glnHeight = rect.bottom;
    m_gldAspect = (GLdouble)m_glnWidth / (GLdouble)m_glnHeight;
    InvalidateRect(&rect, false);
    DrawScene();
}

void CSOEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
	{OnDraw(m_pDC);}

BOOL CSOEditView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	float CamSpeed = 3.0f;
	float Angle = m_Camera.Orient[1];
	float Angle2 = m_Camera.Orient[0];
	if(nFlags & MK_CONTROL)
		{CamSpeed *= 10;}
	else
	if(nFlags & MK_SHIFT)
		{CamSpeed /= 3;}
	//----------------------------------------
	if(Angle < 0)
		{Angle += 360;}
	else
	if(Angle > 360)
		{Angle -= 360;}
	//----------------------------------------
	if(Angle2 < 0)
		{Angle2 += 360;}
	else
	if(Angle2 > 360)
		{Angle2 -= 360;}
	//----------------------------------------
	float radians = (float)((double)(2 * 3.1416 * Angle) / 360);
	float radians2 = (float)((double)(2 * 3.1416 * Angle2) / 360);
	float cosA = cos(radians);
	float sinA = sin(radians);
	float cosA2 = cos(radians2);
	float sinA2 = sin(radians2);
	float outputX = 0, outputY = 0;
	outputX -= ((CamSpeed * cosA) * cosA2);
	outputY -= ((CamSpeed * sinA) * cosA2);
	//zoom in and out
	if(zDelta > 0)
	{
		m_Camera.Position[0] += outputY;
		m_Camera.Position[1] += (CamSpeed * sinA2);
		m_Camera.Position[2] -= outputX;
	}
	else
	if(zDelta < 0)
	{
		m_Camera.Position[0] -= outputY;
		m_Camera.Position[1] -= (CamSpeed * sinA2);
		m_Camera.Position[2] += outputX;
	}
	InvalidateRect(NULL, false);
	OnUpdate(NULL, 0, NULL);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CSOEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar < 256) m_keys[nChar] = true;
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSOEditView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar < 256) m_keys[nChar] = false;
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CSOEditView::OnKillFocus(CWnd* pNewWnd)
{
	// If we lose focus while in fly mode, clean up so we're not stuck
	if (m_FlyMode)
	{
		m_FlyMode = false;
		m_rButton = false;
		ShowCursor(TRUE);
		ReleaseCapture();
		if (m_WASDTimer) { KillTimer(m_WASDTimer); m_WASDTimer = 0; }
	}
	memset(m_keys, 0, sizeof(m_keys));
	CView::OnKillFocus(pNewWnd);
}

void CSOEditView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 2) // Hold-detection: RMB held long enough → enter fly mode
	{
		KillTimer(2);
		if (m_rButton) // still held
		{
			RECT rc;
			GetClientRect(&rc);
			CPoint centre(rc.right / 2, rc.bottom / 2);
			ClientToScreen(&centre);
			m_FlyAnchor = centre;
			SetCursorPos(centre.x, centre.y);
			ShowCursor(FALSE);
			m_FlyMode = true;
			if (!m_WASDTimer)
				m_WASDTimer = SetTimer(1, 16, NULL);
		}
	}
	if (nIDEvent == 1 && m_FlyMode)
	{
		// Build the forward/right/up vectors from current yaw & pitch
		// (reuse the exact same trig pattern already used by OnMouseWheel)
		const float speed = 3.0f;
		float Angle = m_Camera.Orient[1];   // yaw
		float Angle2 = m_Camera.Orient[0];   // pitch

		if (Angle < 0)   Angle += 360.0f;
		else if (Angle > 360) Angle -= 360.0f;
		if (Angle2 < 0)  Angle2 += 360.0f;
		else if (Angle2 > 360) Angle2 -= 360.0f;

		float rad = (float)((double)(2 * 3.1416 * Angle) / 360);
		float rad2 = (float)((double)(2 * 3.1416 * Angle2) / 360);
		float cosA = cos(rad), sinA = sin(rad);
		float cosA2 = cos(rad2), sinA2 = sin(rad2);

		// Forward vector (match OnMouseWheel sign convention exactly so WASD
		// movement remains camera-relative in the same direction as wheel zoom).
		float outputX = -((speed * cosA) * cosA2);
		float outputY = -((speed * sinA) * cosA2);
		float fwdX = outputY;            // position[0] component
		float fwdY = speed * sinA2;      // position[1] (up/down lean)
		float fwdZ = -outputX;           // position[2] component

		// Right vector (perpendicular in XZ plane)
		float rightX = cosA * speed;
		float rightZ = sinA * speed;

		bool moved = false;

		if (m_keys['W'] || m_keys[VK_UP])
		{
			m_Camera.Position[0] += fwdX;
			m_Camera.Position[1] += fwdY;
			m_Camera.Position[2] += fwdZ;
			moved = true;
		}
		if (m_keys['S'] || m_keys[VK_DOWN])
		{
			m_Camera.Position[0] -= fwdX;
			m_Camera.Position[1] -= fwdY;
			m_Camera.Position[2] -= fwdZ;
			moved = true;
		}
		if (m_keys['A'] || m_keys[VK_LEFT])
		{
			m_Camera.Position[0] -= rightX;
			m_Camera.Position[2] -= rightZ;
			moved = true;
		}
		if (m_keys['D'] || m_keys[VK_RIGHT])
		{
			m_Camera.Position[0] += rightX;
			m_Camera.Position[2] += rightZ;
			moved = true;
		}
		// E / Q = move straight up / down (world-space)
		if (m_keys['E'])
		{
			m_Camera.Position[1] += speed;
			moved = true;
		}
		if (m_keys['Q'])
		{
			m_Camera.Position[1] -= speed;
			moved = true;
		}

		if (moved)
		{
			RECT rc; GetClientRect(&rc);
			InvalidateRect(&rc, false);
		}
	}
	CView::OnTimer(nIDEvent);
}

void CSOEditView::anm_normalizer()
{
	glPushMatrix();
	glLoadIdentity();
	CAnimSub *subFRM = NULL;
	if(m_pDoc -> m_AnimBone)
	{
		for(int CurBone = 0; CurBone < m_pDoc -> m_AnimBone -> m_BoneCnt; CurBone++)
		{
			for(int CurFrm = 0; CurFrm < m_pDoc -> m_AnimBone -> m_FrameCnt; CurFrm++)
			{
				if(subFRM = m_pDoc -> m_AnimBone -> m_Frames[CurFrm].FindSub(CurBone))
				{
					if(subFRM -> chunk_flags & B_LEFT_HANDED)
					{
						glLoadIdentity();
						float m[16] = {0};
						int i, j;
						for(i = 0; i < 4; i++)
						{
							for(j = 0; j < 3; j++)
								{m[i * 4 + j] = subFRM -> m_Matrix34.v[i][j];}
							m[i * 4 + j] = 0.0f;
						}
						m[15] = 1.0f;
						glMultMatrixf((float *)&m);
						glScalef(1.0f, 1.0f, -1.0f);
						glGetFloatv(GL_MODELVIEW_MATRIX, m);
						for(int i = 0; i < 4; i++)
						{
							for(int j = 0; j < 3; j++)
								{subFRM -> m_Matrix34.v[i][j] = m[i * 4 + j];}
						}
						subFRM -> chunk_flags &= ~(1 << B_LEFT_HANDED);
					}
				}
			}
		}
	}
	glPopMatrix();
}  

void CSOEditView::OnViewShaded()
{
	if(m_pDoc -> TexOff)
		{OnTexturesOn();}
	m_DrawMode = rl_ambient;
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::OnViewTextured()
{
	if(m_pDoc -> TexOff)
		{OnTexturesOn();}
	m_DrawMode = rl_diffuse;
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::OnViewWireframe()
{
	if(m_pDoc -> TexOff)
		{OnTexturesOn();}
	m_DrawMode = rl_wire;
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::OnViewTexturesOff()
{
	m_DrawMode = rl_tex_off;
	OnTexturesOff();
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::OnViewWireframeTexturesOff()
{
	m_DrawMode = rl_ambient_tex_off;
	OnTexturesOff();
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::OnTexturesOff()
{
	if(m_pDoc -> TexOff)
		{return;}
	m_pDoc -> Colorized_3d = true;
	memcpy(m_pDoc -> tmptexName, m_pDoc -> texName, sizeof(unsigned int) * 256);
	memset(m_pDoc -> texName, 0, sizeof(unsigned int) * 256);
	m_pDoc -> TexOff = true;
}

void CSOEditView::OnTexturesOn()
{
	m_pDoc -> Colorized_3d = false;
	memcpy(m_pDoc -> texName, m_pDoc -> tmptexName, sizeof(unsigned int) * 256);
	memset(m_pDoc -> tmptexName, 0, sizeof(unsigned int) * 256);
	m_pDoc -> TexOff = false;
}

void CSOEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_lButton = false;
	CView::OnLButtonUp(nFlags, point);
}

void CSOEditView::OnMButtonUp(UINT nFlags, CPoint point)
{
	m_mButton = false;
	CView::OnLButtonUp(nFlags, point);
}

void CSOEditView::OnRButtonUp(UINT nFlags, CPoint point)
{
	KillTimer(2); // cancel the hold-detection timer regardless

	if (m_FlyMode)
	{
		// Was in fly mode — exit cleanly
		m_FlyMode = false;
		m_rButton = false;
		ShowCursor(TRUE);
		ReleaseCapture();
		if (m_WASDTimer) { KillTimer(m_WASDTimer); m_WASDTimer = 0; }
		memset(m_keys, 0, sizeof(m_keys));
	}
	else
	{
		// Short click — show context menu
		m_rButton = false;
		ReleaseCapture();
		CMenu menu;
#ifdef ALTERNATIVE_LANG
		VERIFY(menu.LoadMenu(IDR_3D_VIEW_MENU_AL));
#else
		VERIFY(menu.LoadMenu(IDR_3D_VIEW_MENU));
#endif
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
#ifdef ALTERNATIVE_LANG
		pPopup->CheckMenuItem(ID_3D_SHADED_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_TEXTURED_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_WIREFRAME_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_TEXTURES_OFF_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_WIREFRAME_TEXTURES_OFF_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_COLLISIONSVIEWER_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_VISUAL_AL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_HYBRID_AL, MF_UNCHECKED);
		switch (m_DrawMode)
		{
		case rl_ambient:
		{ pPopup->CheckMenuItem(ID_3D_SHADED_AL, MF_CHECKED); }
		break;
		case rl_diffuse:
		{ pPopup->CheckMenuItem(ID_3D_TEXTURED_AL, MF_CHECKED); }
		break;
		case rl_wire:
		{ pPopup->CheckMenuItem(ID_3D_WIREFRAME_AL, MF_CHECKED); }
		break;
		case rl_tex_off:
		{ pPopup->CheckMenuItem(ID_3D_TEXTURES_OFF_AL, MF_CHECKED); }
		break;
		case rl_ambient_tex_off:
		{ pPopup->CheckMenuItem(ID_3D_WIREFRAME_TEXTURES_OFF_AL, MF_CHECKED); }
		break;
		};
		switch (m_ViewMod)
		{
		case vm3_visual:
		{ pPopup->CheckMenuItem(ID_3D_VISUAL_AL, MF_CHECKED); }
		break;
		case vm3_hybrid:
		{ pPopup->CheckMenuItem(ID_3D_HYBRID_AL, MF_CHECKED); }
		break;
		case vm3_collision:
		{ pPopup->CheckMenuItem(ID_3D_COLLISIONSVIEWER_AL, MF_CHECKED); }
		break;
		};
		if (m_vol_wireframe)
		{
			pPopup->CheckMenuItem(ID_3D_COLLISION_AS_WIREFRAME_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_COLLISION_AS_WIREFRAME_AL, MF_UNCHECKED);
		}
		if (m_ViewEnts3dAsWireframe)
		{
			pPopup->CheckMenuItem(ID_3D_ENTITY_AS_WIREFRAME_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_ENTITY_AS_WIREFRAME_AL, MF_UNCHECKED);
		}
		if (m_pDoc->Colorized_3d)
		{
			pPopup->CheckMenuItem(ID_3D_COLORIZE_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_COLORIZE_AL, MF_UNCHECKED);
		}
		if (pWnd->m_ViewEnts3d)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_ENTITY_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_ENTITY_AL, MF_UNCHECKED);
		}
		if (pWnd->m_Selected)
		{
			pPopup->CheckMenuItem(ID_VIEW_SELECTED_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW_SELECTED_AL, MF_UNCHECKED);
		}
		if (m_grid)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_BACKGROUND_GRID_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_BACKGROUND_GRID_AL, MF_UNCHECKED);
		}
		if (m_ViewMeshAsWireframe)
		{
			pPopup->CheckMenuItem(ID_3D_MESHS_AS_WIREFRAME_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_MESHS_AS_WIREFRAME_AL, MF_UNCHECKED);
		}
		if (m_WireframeOverMesh)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_WIREFRAME_OVER_MESH_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_WIREFRAME_OVER_MESH_AL, MF_UNCHECKED);
		}
		if (m_Paint_selected_meshes)
		{
			pPopup->CheckMenuItem(ID_VIEWING_PAINTSELECTEDMESHES_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEWING_PAINTSELECTEDMESHES_AL, MF_UNCHECKED);
		}
		if (m_ViewMeshVector)
		{
			pPopup->CheckMenuItem(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH_AL, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH_AL, MF_UNCHECKED);
		}
#else
		pPopup->CheckMenuItem(ID_3D_SHADED, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_TEXTURED, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_WIREFRAME, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_TEXTURES_OFF, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_WIREFRAME_TEXTURES_OFF, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_COLLISIONSVIEWER, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_VISUAL, MF_UNCHECKED);
		pPopup->CheckMenuItem(ID_3D_HYBRID, MF_UNCHECKED);
		switch (m_DrawMode)
		{
		case rl_ambient:
		{ pPopup->CheckMenuItem(ID_3D_SHADED, MF_CHECKED); }
		break;
		case rl_diffuse:
		{ pPopup->CheckMenuItem(ID_3D_TEXTURED, MF_CHECKED); }
		break;
		case rl_wire:
		{ pPopup->CheckMenuItem(ID_3D_WIREFRAME, MF_CHECKED); }
		break;
		case rl_tex_off:
		{ pPopup->CheckMenuItem(ID_3D_TEXTURES_OFF, MF_CHECKED); }
		break;
		case rl_ambient_tex_off:
		{ pPopup->CheckMenuItem(ID_3D_WIREFRAME_TEXTURES_OFF, MF_CHECKED); }
		break;
		};
		switch (m_ViewMod)
		{
		case vm3_visual:
		{ pPopup->CheckMenuItem(ID_3D_VISUAL, MF_CHECKED); }
		break;
		case vm3_hybrid:
		{ pPopup->CheckMenuItem(ID_3D_HYBRID, MF_CHECKED); }
		break;
		case vm3_collision:
		{ pPopup->CheckMenuItem(ID_3D_COLLISIONSVIEWER, MF_CHECKED); }
		break;
		};
		if (m_vol_wireframe)
		{
			pPopup->CheckMenuItem(ID_3D_COLLISION_AS_WIREFRAME, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_COLLISION_AS_WIREFRAME, MF_UNCHECKED);
		}
		if (m_ViewEnts3dAsWireframe)
		{
			pPopup->CheckMenuItem(ID_3D_ENTITY_AS_WIREFRAME, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_ENTITY_AS_WIREFRAME, MF_UNCHECKED);
		}
		if (m_pDoc->Colorized_3d)
		{
			pPopup->CheckMenuItem(ID_3D_COLORIZE, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_COLORIZE, MF_UNCHECKED);
		}
		if (pWnd->m_ViewEnts3d)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_ENTITY, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_ENTITY, MF_UNCHECKED);
		}
		if (pWnd->m_Selected)
		{
			pPopup->CheckMenuItem(ID_VIEW_SELECTED, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW_SELECTED, MF_UNCHECKED);
		}
		if (m_grid)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_BACKGROUND_GRID, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_BACKGROUND_GRID, MF_UNCHECKED);
		}
		if (m_ViewMeshAsWireframe)
		{
			pPopup->CheckMenuItem(ID_3D_MESHS_AS_WIREFRAME, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_3D_MESHS_AS_WIREFRAME, MF_UNCHECKED);
		}
		if (m_WireframeOverMesh)
		{
			pPopup->CheckMenuItem(ID_VIEW3D_WIREFRAME_OVER_MESH, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEW3D_WIREFRAME_OVER_MESH, MF_UNCHECKED);
		}
		if (m_Paint_selected_meshes)
		{
			pPopup->CheckMenuItem(ID_VIEWING_PAINTSELECTEDMESHES, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEWING_PAINTSELECTEDMESHES, MF_UNCHECKED);
		}
		if (m_ViewMeshVector)
		{
			pPopup->CheckMenuItem(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH, MF_CHECKED);
		}
		else
		{
			pPopup->CheckMenuItem(ID_VIEWING_DISPLAYTHEVECTOROFTHESELECTEDMESH, MF_UNCHECKED);
		}
#endif
		GetCursorPos(&point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
	CView::OnRButtonUp(nFlags, point);
}

void CSOEditView::OnColorize()
{
	m_pDoc -> Colorized_3d = !m_pDoc -> Colorized_3d;
	m_pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::DrawVolumeGL(CBone *basis)
{
	CSOEditDoc* pDoc = GetDocument();
	bool selected;
	CVolume *pSelect = NULL;
	CVolume *pVol = pDoc -> m_Model -> m_VolumeList -> GetFirst();
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	float VSaturation = 0.0f;
	while(pVol)
	{
		if(pVol -> boneptr != basis || !pVol -> m_tree_check)
			{pVol = pVol -> next; continue;}
		selected = false;
		if(pDoc -> m_SelVolume)
		{
			if(pVol -> hTreeItem == pDoc -> hSelTreeItem)
				{selected = true;}
		}
		if(!stricmp(pVol -> m_Name, "ram") && !pWnd -> m_ram && !selected)
			{pVol = pVol -> next; continue;}

		if((!stricmp(pVol -> m_Name, "trackleft") || !stricmp(pVol -> m_Name, "trackright") || !stricmp(pVol -> m_Name, "trackl") || !stricmp(pVol -> m_Name, "trackr")) && !pWnd -> m_track && !selected)
			{pVol = pVol -> next; continue;}
		glPushMatrix();
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
		if(selected && m_ViewMod == vm3_collision)
			{glDisable(GL_BLEND);}
		if(selected)
		{
			if(m_ViewMod == vm3_collision)
				{VSaturation = 120.0f;}
			else
				{VSaturation = 180.0f;}
			glColor3f(pWnd -> m_Colors[C_VOLUMES][0] / VSaturation, pWnd -> m_Colors[C_VOLUMES][1] / VSaturation, pWnd -> m_Colors[C_VOLUMES][2] / VSaturation);
		}
		else
		{
			VSaturation = 384.0f;
			glColor3f(pWnd -> m_Colors[C_VOLUME][0] / VSaturation, pWnd -> m_Colors[C_VOLUME][1] / VSaturation, pWnd -> m_Colors[C_VOLUME][2] / VSaturation);
		}
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
									{glColor3f(pWnd -> m_Colors[VOLUME_FRONT][0] / VSaturation, pWnd -> m_Colors[VOLUME_FRONT][1] / VSaturation, pWnd -> m_Colors[VOLUME_FRONT][2] / VSaturation);}//, alpha_intensity / 255.0f
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
						v3_t vn = {0};
						glBegin(GL_POLYGON);
						for(int v = 2; v >= 0; v--)
						{
							glVertex3fv(pVol -> m_Vol -> m_vertlist[pVol -> m_Vol -> m_polylist[f].v[v]].xyz);
							glNormal3fv(vn);
						}
						glEnd();
					}
				}
				break;
			}
		}
		if(selected && m_ViewMod == vm3_collision)
			{glEnable(GL_BLEND);}
		glPopMatrix();
		pVol = pVol -> next;
	}
}

void CSOEditView::DrawBoxGL(v3_t box)
{
    v3_t vert[8];
	v3_t vn = {0};

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
		{
			glVertex3fv(vert[indx[f].v[v]]);
			glNormal3fv(vn);
		}
        glEnd();
	}
}

void CSOEditView::DrawCylGL(CCylinder *cyl)
{
	v3_t vn = {0};
	for(int f = 0; f < cyl -> m_faces; f++)
	{
		glBegin(GL_POLYGON);
		for(int v = 2; v >= 0; v--)
		{
			glVertex3fv(cyl -> m_vert[cyl -> m_face[f].v[v]]);
			glNormal3fv(vn);
		}
		glEnd();
	}
}

void CSOEditView::DrawVolumeLayer(CBone *basis)
{
    CBone *child;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
    glPushMatrix();
	if(!m_pDoc -> m_skin)
	{
		if(basis -> Matrix34)
		{
			int i, j;
			float m[16];
			for(i = 0; i < 4; i++)
			{
				for(j = 0; j < 3; j++)
					{m[i * 4 + j] = basis -> Matrix34 -> v[i][j];}
				m[i * 4 + j] = 0.0f;
			}
			m[15] = 1.0f;
			glMultMatrixf((float *)&m);
		}
	}
	if(m_pDoc -> m_AnimBone)
	{
		int iBone;
		for(iBone = 0; iBone < m_pDoc -> m_AnimBone -> m_BoneCnt; iBone++)
		{
			if(!stricmp(m_pDoc -> m_AnimBone -> m_BoneMap[iBone], basis -> m_Name))
				{break;}
		}
		CAnimSub *t_sub;
		if(iBone < m_pDoc -> m_AnimBone -> m_BoneCnt)
		{
			if(m_pDoc -> m_Frame >= m_pDoc -> m_AnimBone -> m_FrameCnt)
			{
				m_pDoc -> m_Frame = m_pDoc -> m_AnimBone -> m_FrameCnt - 1;
				if(m_pDoc -> m_Frame < 0)
					{m_pDoc -> m_Frame = 0;}
			}
			if((t_sub = m_pDoc -> m_AnimBone -> m_Frames[m_pDoc -> m_Frame].FindSub(iBone)) != NULL)
			{
				int i, j;
				if(!basis -> m_Animatrix)
					{basis -> m_Animatrix = new float[16];}
				for(i = 0; i < 4; i++)
				{
					for(j = 0; j < 3; j++)
						{basis -> m_Animatrix[i * 4 + j] = t_sub -> m_Matrix34.v[i][j];}
					basis -> m_Animatrix[i * 4 + j] = 0.0f;
				}
				basis -> m_Animatrix[15] = 1.0f;
				glPopMatrix();
				glPushMatrix();
				glMultMatrixf(basis -> m_Animatrix);
				delete[] basis -> m_Animatrix;
				basis -> m_Animatrix = NULL;
			}
		}
	}
    glRotatef(basis -> m_Rotations[0], 1.0f, 0.0f, 0.0f);
    glRotatef(basis -> m_Rotations[1], 0.0f, 1.0f, 0.0f);
    glRotatef(basis -> m_Rotations[2], 0.0f, 0.0f, 1.0f);
    glScalef(basis -> m_Scales[0], basis -> m_Scales[1], basis -> m_Scales[2]);
    glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_DST_ALPHA);
	if((m_pDoc-> m_Model -> m_VolumeList) && (pWnd -> m_ViewVols) && basis)
		{DrawVolumeGL(basis);}
	glDisable(GL_BLEND);
	if((child = basis -> m_ChildFirst) != NULL)
	{
		while(child)
		{
			DrawVolumeLayer(child);
			child = child -> next;
		}
	}
    glPopMatrix();
}

void CSOEditView::On3DCollisionsViewer()
{
	CSOEditDoc* pDoc = GetDocument();
	pDoc -> m_ViewMod = m_ViewMod = vm3_collision;
	pDoc -> UpdateAllViews(NULL);
}

void CSOEditView::On3DVisual()
{
	CSOEditDoc* pDoc = GetDocument();
	pDoc -> m_ViewMod = m_ViewMod = vm3_visual;
	pDoc -> UpdateAllViews(NULL);
}

void CSOEditView::On3DHybrid()
{
	CSOEditDoc* pDoc = GetDocument();
	pDoc -> m_ViewMod = m_ViewMod = vm3_hybrid;
	pDoc -> UpdateAllViews(NULL);
}

void CSOEditView::DrawTrapGL(v3_t box)
{
    v3_t vert[6];
	v3_t vn = {0};
	if(m_ViewEnts3dAsWireframe)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}

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
		{
			glVertex3fv(vert[indx[f].v[v]]);
			glNormal3fv(vn);
		}
        glEnd();
	}
	if(m_DrawMode != rl_wire && m_DrawMode != rl_ambient_tex_off)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
}

void CSOEditView::OnView3dEntity()
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc* pDoc = GetDocument();
	pWnd -> m_ViewEnts3d = !pWnd -> m_ViewEnts3d;
	pDoc -> UpdateAllViews(NULL, 0, NULL);
}

void CSOEditView::DrawXYZLinesGL()
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


void CSOEditView::DrawGridGL()
{
    v3_t v1, v2;
    CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc* pDoc = GetDocument();
    if(pWnd)
		{glColor3f(pWnd -> m_Colors[C_3D_GRID][0]/255.0f, pWnd -> m_Colors[C_3D_GRID][1]/255.0f, pWnd -> m_Colors[C_3D_GRID][2]/255.0f);}
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[0] = 1000.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[0] = -1000.0f;
    for(int x = -1000; x < 1000; x += pDoc -> m_GridSize)
	{
        v1[2] = (float)x;
        v2[2] = (float)x;
		glNormal3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
           glVertex3fv(v1);
           glVertex3fv(v2);
        glEnd();
	}
    v1[0] = 0.0f;
    v1[1] = 0.0f;
    v1[2] = 0.0f;
    v1[2] = 1000.0f;
    v2[0] = 0.0f;
    v2[1] = 0.0f;
    v2[2] = 0.0f;
    v2[2] = -1000.0f;
    for(int y = -1000; y < 1000; y += pDoc -> m_GridSize)
	{
        v1[0] = (float)y;
        v2[0] = (float)y;
		glNormal3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);
           glVertex3fv(v1);
           glVertex3fv(v2);
        glEnd();
	}
}

void CSOEditView::OnView3dBackgroundGrid()
{
	m_grid = !m_grid;
	InvalidateRect(NULL, false);
}

void CSOEditView::On3dCollisionAsWireframe()
{
	m_vol_wireframe = !m_vol_wireframe;
	InvalidateRect(NULL, false);
}

void CSOEditView::On3dEntityAsWireframe()
{
	m_ViewEnts3dAsWireframe = !m_ViewEnts3dAsWireframe;
	InvalidateRect(NULL, false);
}

void CSOEditView::On3dMeshsAsWireframe()
{
	m_ViewMeshAsWireframe = !m_ViewMeshAsWireframe;
	InvalidateRect(NULL, false);
}

void CSOEditView::OnView3dWireframeOverMesh()
{
	m_WireframeOverMesh = !m_WireframeOverMesh;
	InvalidateRect(NULL, false);
}


void CSOEditView::OnViewingPaintselectedmeshes()
{
	m_Paint_selected_meshes = !m_Paint_selected_meshes;
	InvalidateRect(NULL, false);
}

void CSOEditView::OnViewingDisplaythevectoroftheselectedmesh()
{
	m_ViewMeshVector = !m_ViewMeshVector;
	InvalidateRect(NULL, false);
}
