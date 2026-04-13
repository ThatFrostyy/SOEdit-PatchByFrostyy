// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"
#include "CVol.h"
#include "CSdl.h"
#include "CCylinder.h"
#include "SOEditDoc.h"
#include "mainfrm.h"

#ifndef _CVOLUME_
#define _CVOLUME_

class CVolume
   {
    public:
       CVolume();
       CVolume(CSdl *sdl);
       ~CVolume();
       CVolume    *next;
       CVolume    *prev;
       void        ParseFile();
       char       *m_Bone;
       char       *m_Component;
	   char       *m_Tags;
       CSdl       *m_sdl;
       char       *m_Name;
       matrix34_t *m_Matrix34;
       float      *m_Position;
       float      *m_Box;
       float      *m_Cylinder;
	   float      *m_Density;
       CCylinder  *m_Cyl;
       char       *m_PolyhedronName;
       CVol       *m_Vol;
       v3_t        m_Rotations;
       v3_t        m_Translations;
       v3_t        m_Scales;
       int         m_Type; // 3 = polyhedron, 1 = box, 2 = cylinder
	   CBone *boneptr;
	   thickness_t *vol_thickness;
	   HTREEITEM   hTreeItem;
	   bool        m_tree_check;

       void SetName(char *name);
	   void SetDensity();
       void SetBone();
       void SetComponent();
	   void SetTags();
       void SetPolyhedron();
	   void SetThickness();
	   void SetThicknessSide();
       void SetMatrix34();
       void SetOrientation();
	   void SetPosition();
       void SetBox();
	   void SetSphere();
       void SetCylinder();
       void InitObject(void);
       void WriteMdl(FILE *fp, int indent);

    private:
	   CString msg_st;

   };

class CVolumeList
   {
    public:
       CVolume *m_head;
       CVolume *m_tail;
       CVolume *m_curr;

       CVolumeList();
       CVolumeList(CSdl *sdl);
       ~CVolumeList();

       CVolume *AddToHead();
       CVolume *AddToTail();
       CVolume *GetFirst();
       CVolume *GetNext();
       CVolume *GetPrev();
       CVolume *GetCurr();
       void     ClearList();
       CVolume *GetLast();
	   CVolume *FindVolumeByTreeID(HTREEITEM ID);

       void     DropVolume(char *szName);

       CVolume *FindByBone(char *szBone);
	   int FindCountBone(void);
       CVolume *FindByName(char *szName);

       void     WriteMdl(FILE *fp, int indent);
       CSdl    *m_sdl;

    private:
   };


#endif

