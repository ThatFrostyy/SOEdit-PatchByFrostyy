// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "stdio.h"
#include "general.h"

#include "CModel.h"
#include "CSdl.h"

class CSObject
   {
    public:
        CSObject(char *filename);
        ~CSObject();

        void ParseFile();

        int          m_mode;
        CSdl        *m_sdl;

        CModel      *m_Model;

    private:
	   CString msg_st;
   };

