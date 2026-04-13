// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "CSdl.h"
#include "all_trash.h"
#include "parse_mtl.h"
#include "general.h"

#ifndef _CTEXTURE_
#define _CTEXTURE_

class CTexture
   {
    public:

       CTexture(char *);
       ~CTexture();

       int    m_Filtering;
       int    m_MipMap;
       char  *m_Blend;
       int    m_AlphaChannel;
       char  *m_FrameName;
       char **m_Frames;
       int    m_FrameCnt;
       int    m_TextureAnimation;
       bool   m_Animation;
       bool   m_Loop;
       bool   m_AsyncStart;
       bool   m_Play;
	   bool   m_forced_alpha_test;

       bool   m_BitMap;

       char  *m_TexFile;

       void  ParseFile();
       void  SetFiltering();
       void  SetMipMap();
       void  SetBlend();
       void  SetAlphaChannel();
       void  SetFrame();
       void  AddFrame();
       int   GetFrameCnt();
       char *GetFrame(int frame);
       void  SetTextureAnimation();



    private:
       CSdl *m_sdl;
	   CString msg_st;
   };

#endif