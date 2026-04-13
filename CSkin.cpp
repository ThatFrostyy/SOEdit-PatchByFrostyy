// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CSkin.h"

CSkin::CSkin()
   {
    m_verts = 0;
    m_vertlist = NULL;
    m_faces = 0;
    m_facelist = NULL;

    //m_size = 128;
    //m_vertlist = new int[m_size];
   }

CSkin::~CSkin()
   {
    if (m_vertlist)
       {
        delete [] m_vertlist;
       }
    m_vertlist = NULL;

    if (m_facelist)
       {
        delete [] m_facelist;
       }
    m_facelist = NULL;
   }

void CSkin::AddVertex(int vert)
   {
/*
    // do we have room in our array?
    if ((m_verts+1) >= m_size)
       {
        int *tlist;

        m_size;
        tlist = new int[m_size+128];
        for (int v = 0; v < m_size; v++)
           {
            tlist[v] = m_vertlist[v];
           }
        delete [] m_vertlist;
        m_vertlist = tlist;
        m_size += 128;
       }

    m_vertlist[m_verts] = vert;
    m_verts++;
*/
   }

