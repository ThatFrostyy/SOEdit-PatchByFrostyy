// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CBone.h"
#include "CVolume.h"
#include "CSdl.h"

#ifndef _CSKELETON_
#define _CSKELETON_

class CSkeleton
   {
    public:
       CSkeleton();
	   void DelAll(CBone *curbone);
       CSkeleton(CSdl *sdl);
	   CSkeleton(CSdl * sdl, bool mod);
       ~CSkeleton(void);


       CBone      *m_bonelist;
	   CBone      *m_ChildLast;
       CAnimation *m_Animation;
       CSdl       *m_sdl;
	   BYTE	       m_clean_on_destroy;

       void   ParseFile(void);
       void   AddExtension();
       void   AddBone();
       void   AddAnimation();
       void   WriteMdl(FILE *fp, int indent);

    private:
	   CString msg_st;

   };

#endif