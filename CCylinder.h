// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"

#ifndef _CYLINDER_
#define _CYLINDER_

class CSection
   {
    public:
        v2_t *m_xy;
        int   m_sides;

        CSection(int sides, float diameter);
        ~CSection();
   };

class CCylinder
   {
    public:
        int       m_sections;
        CSection *m_section;
        int       m_sides;

        int       m_verts;
        v3_t     *m_vert;

        int       m_faces;
        indx_t   *m_face;
        v3_t      m_center[2];

        CCylinder(float length, float diameter);
        ~CCylinder();

   };

#endif