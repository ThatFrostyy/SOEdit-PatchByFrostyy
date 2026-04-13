// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"
#include "CSdl.h"

#ifndef _CObb2_
#define _CObb2_

class CObb2
   {
    public:
       CObb2(char *type);
       CObb2(char *type, CSdl *sdl);
       ~CObb2();


       CObb2        *next;
       CObb2        *prev;
 
       char         *m_Type;

       void          ParseFile();

       CSdl         *m_sdl;
       char         *m_Name;

       int           m_GType;

       bool          m_Rotate;
       float        *m_Radius;
       float        *m_Center;
       float        *m_Extent;
       float        *m_Axis;
	   char         *m_Tags;

       int           m_Vertices;
       v2_t         *m_Vertex;


       int           m_Level;

       v3_t          m_Rotations;
       v3_t          m_Translations;
       v3_t          m_Scales;

       void InitObject(char *type);

       void SetObb2();
       void SetCircle2();
       void SetPolygon2();

       void SetVertex();
       void SetRadius();
       void SetName(char *name);
       void SetAxis();
	   void SetTags();
       void SetCenter();
       void SetRotate();
       void SetExtent();

       void WriteMdl(FILE *fp, int indent);

    private:
	   CString msg_st;
   };

class CObb2List
   {
    public:
       CObb2 *m_head;
       CObb2 *m_tail;
       CObb2 *m_curr;

       char      *m_Type;

       CObb2List(char *type);
       CObb2List(char *type, CSdl *sdl);
       ~CObb2List();

       void       InitObject(char *type);

       CObb2 *AddToHead();
       CObb2 *AddToTail();
       CObb2 *GetFirst();
       CObb2 *GetNext();
       CObb2 *GetPrev();
       CObb2 *GetCurr();
       CObb2 *GetLast();
       void       ClearList();

       CObb2 *FindByName(char *szName);

       void       WriteMdl(FILE *fp, int indent);
       CSdl      *m_sdl;

    private:
   };

#endif


