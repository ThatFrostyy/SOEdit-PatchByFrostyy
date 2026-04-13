// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "CAnimation.h"
#include "CSkin.h"
#include "CSdl.h"
#include "general.h"
#include "CPly.h"


#ifndef _CBONE_
#define _CBONE_

class CBone
{
    public:
		CBone();
		CBone(CBone *parent);
		CBone(CSdl *sdl, CBone *parent, BYTE mod, int N);
		CBone(CSdl *sdl);
		~CBone();

       CBone      *next;
       CBone      *prev;

       CBone      *m_parent;
       CBone      *m_ChildFirst;
       CBone      *m_ChildLast;
       CBone      *m_curr;

       CSkin      *m_Skin;

       CSdl       *m_sdl;
	   CString msg_st;

       bool        m_Socket;
	   bool        m_Prizmatic;
	   bool        m_Revolute;
	   bool        m_Terminator;
	   bool        m_Speed2;
       bool        m_Ground;
	   bool        m_bone_colorized;
	   bool        m_start_colorized;
       int         m_Visibility;
       CPly       *m_VolumeView;
       char       *m_VolumeViewName;
       char       *m_Name;
	   int         m_Layer;
       matrix34_t *Matrix34;
       matrix33_t *m_Orientation;
       float      *m_Position;
       float       m_Speed;
       char       *m_Parameters;
       CAnimation *m_Animation;
       float       m_Limits[2];
	   bool        m_LimitsType;//1 - NN, 0 - N
	   bool        HasLimits;
	   int		   adds;
	   int		   sub;
	   HTREEITEM   hTreeItem;
	   bool		   m_LODView;
	   bool        m_LOD_OFF;
	   bool        m_NoCastShadows, m_DecalTarget, m_NoGroupMesh, m_NoGetShadows;
	   bool        m_OpenInTree;
	   BYTE        LODView;
	   DWORD       m_bone_color;
	   bool        m_tree_check;
	   bool        m_anm_visi;
       float      *m_Animatrix;

       v3_t        m_Rotations;
       v3_t        m_Translations;
       v3_t        m_Scales;

       void FixGeometry(void);

       bool GetBBox(v3_t *size);

       void ParseFile(void);
	   void SetDecalTarget();
	   void SetGroupMesh();
	   void SetGetShadows();
       void SetName(char *name);
       void SetRevolute();
	   void SetSocket();
	   void SetPrizmatic();
       void SetGround();
	   void SetColor();
       void SetLayer();
       void SetVolumeView();
       void SetVisibility();
	   void SetOFF();
	   void SetParameters();
       void SetOrientation();
       void SetPosition();
       void SetLimits();
       void SetSpeed();
	   void SetSpeed2();
       void AddBone(int mod, int N);
       void SetAnimation();
       void SetMatrix();
	   void SetTerminator();
	   void SetNoCastShadows();
	   void SetTrash();
	   void CommonIni(void);
	   void AddPly(char *FName);

       void WriteMdl(FILE *fp, int indent);

       CBone *FindBone(char *szName);
	   void FindCountBone(int &CouName);

       CBone *AddToHead();
       CBone *AddToTail(int mod, int N);
       CBone *GetFirst();
       CBone *GetNext();
       CBone *GetPrev();
       CBone *GetCurr();
       void   ClearList();
       CBone *GetLast();
	   CBone *FindBoneByTreeID(HTREEITEM ID);
        
    private:

   };

#endif