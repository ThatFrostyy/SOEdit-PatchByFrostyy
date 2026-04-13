// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"

#ifndef _CSKIN_
#define _CSKIN_

class CSkin
   {
    public:
       CSkin();
       ~CSkin();

       void AddVertex(int vertex);

       int   m_verts;
       int   m_size;
       int  *m_vertlist;
       int   m_faces;
       int  *m_facelist;
   };

#endif