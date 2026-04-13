// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

// Soldiers .PLY file structure
#include "stdafx.h"
#include "stdio.h"
#include "general.h"

#include "CMesh.h"

#ifndef _CPLY_
#define _CPLY_

class CPly
{
public:
	CPly(char *path);
	~CPly();

	bool WriteFile(char *plyname);

	FILE *m_fp;
	bool loading_successes, Bply;

	//---------BNDS---------
	BOOL BNDS;
	v3_t m_bbox[2];
	//----------------------

	//---------BAS2---------
	BOOL BAS2;
	v3_t m_basis[4];
	//----------------------

	//---------SKIN---------
	int m_bones;
	char **m_bonelist;
	//----------------------
	   
	//---------MESH---------
	CMesh *m_CurrMesh;
	CMeshList *m_meshlist;
	BOOL SKINNED;
	DWORD GlFvf;
	DWORD specular_rgba_color;
	BYTE subskin_bones[MAX_PATH];
	BYTE subskin_count;
	DWORD GlFlags;
	//----------------------
		
	//---------VERT---------
	int m_numverts, num_weights, num_tex_coords, unknown_data_size;
	vert_t *m_vertlist;
	short m_vsize, m_vflags, m_calculated_vsize;
	BOOL has_pos, has_rhw, has_weights, has_normal, has_psize, has_diffuse, has_mesh_bump;
	BOOL has_specular, has_tex_coords, has_matrix_indices, has_mesh_specular, has_w;
	  //----------------------
		
	//---------INDX/IND4---------
	int INDXcount;
	int m_numpolys;
	indx_t *m_polylist;
	BYTE IndexType;// 1 - INDX, 2 - IND4
	int poly_count;
	//----------------------

	//---------MROR---------
	int m_mirror;
	BOOL MROR;
	//----------------------
      
	//---------ADJA---------
	BOOL ADJA;
	int m_numadjas;
	adja_t *m_adjalist;
	//----------------------

	//---------SHDW---------
	BOOL SHDW;
	int m_numshdws;
	unsigned char *m_shdwlist;
	//----------------------
};

/*
// Header
unsigned char signature[4];  // EPLY - .ply file signature
unsigned char signature[4];  // BPLY - .ply file signature

// Bounding box section?
unsigned char bnds[4];       // BNDS - bounding box section tag
// Coordinate 1 of bounding box
float x1, y1, z1;
// Coordinate 2 of bounding box
float x2, y2, z2;

// MESH section -- can be multiple meshes in a single file
unsigned char mesh[4];       // MESH - mesh info section tag
int           version;       // May be Version?  value appears to always be 0x152 (sometimes 0x252)
int           begface;       // Appears to be the beginning face number in the list
int           fcount;        // Appears to be the face (polygon/triangle) count
int           datatype;      // .PLY data type indicator?  (4 means int, unsigned int or float)
unsigned char texlen;        // One byte length of texture name
unsigned char tname[texlen]; // name of texture to use for skin for this mesch

// VERT section
unsigned char vert[4];       // VERT - vertex info section tag
int           pvcount        // Polygon vertex count?
int           blksize;       // Size of info blocks (each unit is this size in bytes) seems to be always 36
                              
// Polygon vertex block info (repeats pvcount times, each block is blksize in size)
float         x,y,z;         // x, y and z 3d coordinates (these all fit within the bounding box)
float         x,y,z;         // vertex normal?  Not unit vectors... (needed for lighting)
int           unknown;       // don't know what this is, yet... (0xFF808080)
float         s,t;           // S and T texture coordinates

// INDX section
unsigned char indx[4];       // INDX - polygon description section
int           count;         // Count of elements in this section
short         v1,v2,v3;      // each element is two bytes long in groups of three
                             // Appears to be three shorts corresponding to three vertices
                             // values here range from zero to pvcount

// ADJA section
unsigned char adja[4];       // ADJA - adjust something? alpha? armor?
int           count;         // Count of elements in this section
short         adj;           // each element is six bytes long don't know meaning yet
                             // values range from 0xffff (negative 1) to fcount - 1

// SHDW section
unsigned char shdw[4];       // SHDW - cause shadow from this face?
int           count;         // Count of elements in this section
unsigned char shadow;        // cast a shadow? (0 appears to be no and 1 appears to be yes)

*/

#endif