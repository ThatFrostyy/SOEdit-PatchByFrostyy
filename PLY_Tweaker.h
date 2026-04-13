#pragma once
// Диалоговое окно setup_window

#include "stdafx.h"
#include "CSdl.h"
#include "general.h"

struct Tply_mesh_base
{
	DWORD fvf;
	DWORD first_face;
	DWORD face_count;
	DWORD flags;
	char patch[2][MAX_PATH];
	BYTE patch_len[2];
	BYTE subskin_bones[MAX_PATH];
	BYTE subskin_count;
	DWORD specular_rgba_color;
	char text_msg_buff[2048];
};

class PLY_Tweaker : public CDialog
{
public:
	PLY_Tweaker(CWnd* pParent = nullptr);   // стандартный конструктор
 	BOOL OnInitDialog();
	virtual ~PLY_Tweaker();
	
// Данные диалогового окна
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLY_TWEAKER };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

	void DataDEL();
	void DataINI();
	void ParamUpdate();
	void FVFUPD();
	void FlagsUPD(DWORD flags);
	void MtlParamUpdate();
	void OnEnChangePtMeshPaths(int id);
	
protected:
	BOOL m_RewritePermission;

	//---------MESH---------
	DWORD GlFlags;
	DWORD GlFvf;
	//----------------------

public:
	BOOL BPly_Type;
	
	//---------BAS2---------
	v3_t m_basis[4];
	//----------------------

	//---------BNDS---------
	BOOL BNDS;
	v3_t m_bbox[2];
	//----------------------

	//---------MESH---------
	int m_meshs;
	Tply_mesh_base m_mesh_base[256];
	//----------------------

	//---------SKIN---------
	int m_bones;
	char **m_bonelist;
	//----------------------

	//---------MROR---------
	BOOL MROR;
	//----------------------

	//---------INDX/IND4---------
	int m_numpolys;
	BYTE IndexType;// 1 - INDX, 2 - IND4
	//----------------------

	//---------ADJA---------
	BOOL ADJA;
	//----------------------

	//---------SHDW---------
	BOOL SHDW;
	//----------------------

	//---------VERT---------
	int m_numverts;
	short m_vsize, m_calculated_vsize, m_vflags;
	BOOL has_pos, has_rhw, has_weights, has_normal, has_psize, has_diffuse, has_mesh_bump;
	BOOL has_specular, has_tex_coords, has_matrix_indices, has_mesh_specular, has_w;
	int num_weights, num_tex_coords;
	//----------------------

protected:

	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnLbnSelchangePtMaterialList();
	afx_msg void OnBnClickedPtSetMF();
	afx_msg void OnBnClickedPtSetVF();
	afx_msg void OnBnClickedMVFlags();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangePtMeshPath1();
	afx_msg void OnEnChangePtMeshPath2();
};
