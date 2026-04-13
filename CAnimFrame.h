// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"

#ifndef _CANIMFRAME_
#define _CANIMFRAME_

class CAnimSub
   {
    public:
       CAnimSub();
       ~CAnimSub();

       CAnimSub   *m_next;
       CAnimSub   *m_prev;

       int         m_Bone;
       matrix34_t  m_Matrix34;
       int         m_Visible;
	   short	   chunk_flags;
   };

class CAnimFrame
   {
    public:
       CAnimFrame();
       ~CAnimFrame();

       int         m_Bone;
       matrix34_t  m_Matrix34;

       CAnimSub   *m_head;
       CAnimSub   *m_tail;

       CAnimSub   *AddSub(void);
       CAnimSub   *FindSub(int bone);

   };

class CAnimBone
   {
    public:
		CAnimBone();
		CAnimBone(char *szFile, bool &b);
       ~CAnimBone();

       bool Process(FILE *fp);
	   void ProcessSave(FILE * fp);

	   void BoneAdd(char *Name, matrix34_t *Matrix34, int Vis);
	   void BoneDelete(int indx);
	   int FrameDelete(int indx);
	   int OnInsertFrame(int indx);
	   int OnAddClone(int indx);


	   unsigned int Header_ID;

       int            m_BoneCnt;
       char         **m_BoneMap;

       int            m_FrameCnt;
       CAnimFrame    *m_Frames;
    
   };

#endif