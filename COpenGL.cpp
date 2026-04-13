// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "COpenGL.h"

COpenGL::COpenGL()
   {
    m_hDC = 0;
    m_hRC = 0;
    m_hWnd = 0;

    m_glnWidth = 0;
    m_glnHeight = 0;
    m_gldAspect = 0.0f;

    m_AmbientLight[0] = 0.1f;
    m_AmbientLight[1] = 0.1f;
    m_AmbientLight[2] = 0.1f;
    m_AmbientLight[3] = 1.0f;

    m_DiffuseLight[0] = 0.5f;
    m_DiffuseLight[1] = 0.5f;
    m_DiffuseLight[2] = 0.5f;
    m_DiffuseLight[3] = 1.0f;

    m_Specular[0] = 1.0f;
    m_Specular[1] = 1.0f;
    m_Specular[2] = 1.0f;
    m_Specular[3] = 1.0f;

    m_LightPos[0] = -50.0f;
    m_LightPos[1] = 50.0f;
    m_LightPos[2] = 100.0f;
    m_LightPos[3] = 1.0f;

    m_SpecRef[0] = 1.0f;
    m_SpecRef[1] = 1.0f;
    m_SpecRef[2] = 1.0f;
    m_SpecRef[3] = 1.0f;

    m_SceneGround = 0;
   }

BOOL COpenGL::Create(HWND hWnd, HDC hDC)
   {
    m_hWnd = hWnd;
    m_hDC = hDC;

    int  nMyPixelFormatID;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR), // struct size 
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

    nMyPixelFormatID = ChoosePixelFormat( m_hDC, &pfd );

    if (nMyPixelFormatID == 0)
       {
        MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "SetPixelFormat FAILED", "Simple Information", MB_OK);
        return(FALSE);
       }

    // catch errors here.
    // If nMyPixelFormat is zero, then there's
    // something wrong... most likely the window's
    // style bits are incorrect (in CreateWindow() )
    // or OpenGl isn't installed on this machine

    SetPixelFormat( m_hDC, nMyPixelFormatID, &pfd );

    m_hRC = wglCreateContext( m_hDC );

//  This next code should be moved out of the OpenGL class and back into the view
    wglMakeCurrent( m_hDC, m_hRC );

    glClearColor( 0.25f, 0.25f, 0.25f, 1.0f );
    glEnable( GL_DEPTH_TEST );

    // Setup the backface polygon culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Turn on the lights
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0,GL_AMBIENT,m_AmbientLight);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,m_DiffuseLight);
    glLightfv(GL_LIGHT0,GL_SPECULAR,m_Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,m_LightPos);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_SpecRef );
    glMateriali(GL_FRONT, GL_SHININESS, 128 );

    glNewList(m_SceneGround = glGenLists(1), GL_COMPILE);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f( 0.50f, 0.50f, 0.50f );    // 50% grey
        glBegin( GL_QUADS );
            glVertex3f( -25.0f, -15.0f, -25.0f);
            glVertex3f( -25.0f, -15.0f,  25.0f);
            glVertex3f(  25.0f, -15.0f,  25.0f);            
            glVertex3f(  25.0f, -15.0f, -25.0f);
        glEnd();
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f( 0.90f, 0.90f, 0.90f );    // 10% grey
        glBegin( GL_QUADS );
            glVertex3f( -25.0f,  10.0f, -25.0f);
            glVertex3f( -25.0f, -15.0f, -25.0f);
            glVertex3f(  25.0f, -15.0f, -25.0f);            
            glVertex3f(  25.0f,  10.0f, -25.0f);
        glEnd();
    glEndList();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // a perspective-view matrix...
    gluPerspective( 45.0, m_gldAspect, 2.0, 4500.0 );
    glViewport( 0, 0, m_glnWidth, m_glnHeight );

    wglMakeCurrent( NULL, NULL );

    return TRUE;
   }

