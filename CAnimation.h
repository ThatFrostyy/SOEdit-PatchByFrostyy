// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "CSequence.h"
#include "CSdl.h"
#include "general.h"

#ifndef _CANIMATION_
#define _CANIMATION_

class CAnimation
   {
    public:
		CAnimation();
		CAnimation(CSdl *sdl);
       ~CAnimation();

       CSequenceList *m_SequenceList;

    private:
       CSdl *m_sdl;
	   CString msg_st;

       void  ParseFile();
       void  AddSequence();
   };

#endif