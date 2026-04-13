// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "CTexture.h"

#ifndef _CMESH_
#define _CMESH_

class CMesh
{
    public:
	int m_first;
	int m_count;
	int m_fvf;
	DWORD m_flags;
	bool  m_transparency, m_forced_alpha_test;
	CMesh *next;
	CMesh *prev;
	char *m_texname[4];
	CTexture *m_texture[2];
	int m_texnumb[2];
	int m_texcount;

	BYTE subskin_bones[MAX_PATH];
	BYTE subskin_count;
	DWORD specular_rgba_color;

	CMesh(char texname[4][_MAX_PATH], int tex_count, int fvf, DWORD flags);
	~CMesh();
};

class CMeshList
{
    public:
	CMesh *m_head;
	CMesh *m_tail;
	CMesh *m_curr;

	CMeshList();
	~CMeshList();

	CMesh *AddToHead(int first, int count, int fvf, char texname[4][_MAX_PATH], int tex_count, DWORD flags, DWORD specular_rgba_color, BYTE subskin_count, BYTE *subskin_bones);
	CMesh *AddToTail(int first, int count, int fvf, char texname[4][_MAX_PATH], int tex_count, DWORD flags, DWORD specular_rgba_color, BYTE subskin_count, BYTE *subskin_bones);
	CMesh *GetFirst();
	CMesh *GetNext();
	CMesh *GetPrev();
	CMesh *GetCurr();
	void   ClearList();
	CMesh *GetLast();
};

#endif _CMESH_