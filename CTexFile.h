// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "gl/gl.h"
#include "gl/glu.h"

#ifndef _CTEXFILE_
#define _CTEXFILE_

#define  FREEIMAGE_LIB

class CTexFile
{
    public:
	CTexFile(char *texname);
	~CTexFile();
	char          *m_TexName; // Texture File Name
	int            DetermineType();
	bool           LoadTGA();
	bool           LoadDDS();
	bool           LoadBits();
	bool           SwapRB();
	unsigned char *GetBits();
	int            GetBPP();
	int            GetWidth();
	int            GetHeight();
	bool           Alpha_Analysis(DWORD *ImageBuffer, DWORD w, DWORD h, bool mod);
	
	//
	bool           m_transparency;

	private:
	unsigned char *m_Bits;
	int            m_Width;
	int            m_Height;
	int            m_BPP;
};
#endif