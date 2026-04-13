// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#ifndef _COPENGL_
#define _COPENGL_

class COpenGL
   {
    public:
       COpenGL();
       ~COpenGL();

       BOOL Create(HWND hWnd, HDC hDC);
       int Destroy();

       GLfloat m_AmbientLight[4];
       GLfloat m_DiffuseLight[4];
       GLfloat m_Specular[4];
       GLfloat m_LightPos[4];
       GLfloat m_SpecRef[4];

       GLuint  m_SceneGround;

    private:
       GLsizei   m_glnWidth, m_glnHeight;
       GLdouble  m_gldAspect;

       HWND      m_hWnd;
       HDC       m_hDC;
       HGLRC     m_hRC;
   };

#endif