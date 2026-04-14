// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "CSdl.h"
#include "CAnimFrame.h"

#ifndef _CSEQUENCE_
#define _CSEQUENCE_

class CSequence
   {
    public:
	   CSequence();
       CSequence(CSdl *sdl);
	   CSequence(CSequence *anm);
       ~CSequence();

       void   ParseFile();
       char  *GetName();
	   void   SetStore();
       void   SetName(char *);
       void   SetName();
       void   SetFile();
	   void   SetFile(char *name);
       void   SetSpeed();
	   void   SetSmooth();
	   void   SetEvents();
	   void   SetUnknownBlock(char *word);
       void   SetResume();
       void   SetAutoStart();

       CSequence *next;
       CSequence *prev;

       CAnimBone *m_AnimBone;

       CSdl  *m_sdl;
       char  *m_Name;
       char  *m_FileName;
       float  m_Speed;
       float  m_Smooth;
       bool   m_Store;
	   bool   m_Resume;
       bool   m_Autostart;
	   CString m_EventsBlock;
	   CString m_UnknownBlocks;

    private:
	   CString msg_st;
   };


class CSequenceList
   {
    public:
       CSequence *m_head;
       CSequence *m_tail;
       CSequence *m_curr;

	   CSequenceList();
       CSequenceList(CSdl *sdl);
	   CSequenceList(CSequenceList *anm);
       ~CSequenceList();

       CSequence *AddToHead();
       CSequence *AddToTail();
	   CSequence *AddToTail(CSequence *anm);
       CSequence *GetFirst();
       CSequence *GetNext();
	   CSequence *FindByName(char *szName);
	   void DropSequence(char *szName);
       CSequence *GetPrev();
       CSequence *GetCurr();
       void       ClearList();
       CSequence *GetLast();

       void       WriteMdl(FILE *fp, int indent);

       CSdl      *m_sdl;
   };


#endif
