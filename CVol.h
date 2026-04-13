// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include <stdio.h>

#include "general.h"

#ifndef _CVOL_
#define _CVOL_

class CVol
   {
    public:
       CVol(char *path);
       ~CVol();

       char           m_filepath[_MAX_PATH];
       FILE          *m_fp;

       int            m_numverts;
       vert_t        *m_vertlist;

       int            m_numpolys;
       indx_t        *m_polylist;

       int            m_numsides;
       unsigned char *m_sidelist;
	   int poly_count;

    private:

   };

#endif _CVOL_