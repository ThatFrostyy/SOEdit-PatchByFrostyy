// PLY_Tweaker.cpp: файл реализации
//
#include "stdafx.h"
#include "SOEdit.h"
#include "PLY_Tweaker.h"

// Диалоговое окно PLY_Tweaker

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PLY_Tweaker::PLY_Tweaker(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PLY_TWEAKER, pParent)
{
	m_RewritePermission = TRUE;
	DataINI();
}

BOOL PLY_Tweaker::OnInitDialog()
{
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Integrated MoW PLY Tweaker");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("Overwrite the file");
	#endif
	CString PrintText;
	CEdit *pEdit;
	CListBox *pList;
	PrintText.Format("X: %g", m_bbox[0][0]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MIN_X);
	pEdit -> SetWindowText(PrintText);
	PrintText.Format("Y: %g", m_bbox[0][1]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MIN_Y);
	pEdit -> SetWindowText(PrintText);
	PrintText.Format("Z: %g", m_bbox[0][2]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MIN_Z);
	pEdit -> SetWindowText(PrintText);
	PrintText.Format("X: %g", m_bbox[1][0]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MAX_X);
	pEdit -> SetWindowText(PrintText);
	PrintText.Format("Y: %g", m_bbox[1][1]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MAX_Y);
	pEdit -> SetWindowText(PrintText);
	PrintText.Format("Z: %g", m_bbox[1][2]);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_BB_MAX_Z);
	pEdit -> SetWindowText(PrintText);
	if(m_meshs >= 1)
	{
		pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
		for(int i = 0; i < m_meshs; i++)
		{
			PrintText.Format("Material: %d", i);
			pList -> AddString(PrintText);
		}
		pList -> SetCurSel(0);
	}
	pList = (CListBox *)GetDlgItem(IDC_PT_SKIN_LIST);
	for(int i = 0; i < m_bones; i++)
		{pList -> AddString(m_bonelist[i]);}
	if(ADJA)
		{CheckDlgButton(IDC_PT_CHECK_ADJA, 1);}
	if(SHDW)
		{CheckDlgButton(IDC_PT_CHECK_SHDW, 1);}
	if(MROR)
		{CheckDlgButton(IDC_PT_CHECK_MROR, 1);}
	ParamUpdate();
	OnLbnSelchangePtMaterialList();
	FVFUPD();
 	CDialog::OnInitDialog();
 	return TRUE;
}

PLY_Tweaker::~PLY_Tweaker()
{
	DataDEL();
}

void PLY_Tweaker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(PLY_Tweaker, CDialog)
	ON_LBN_SELCHANGE(IDC_PT_MATERIAL_LIST, &PLY_Tweaker::OnLbnSelchangePtMaterialList)
	ON_BN_CLICKED(IDC_PT_SET_M_F, &PLY_Tweaker::OnBnClickedPtSetMF)
	ON_BN_CLICKED(IDC_PT_SET_V_F, &PLY_Tweaker::OnBnClickedPtSetVF)
	ON_BN_CLICKED(IDC_PT_S_MV_TWO_SIDED, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_USE_ALPHA, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_LIGHT, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_PLCR, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_SHADOW, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_MIRRORED, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_BLENDTEX, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_SPECULAR, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_MATERIAL, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_TWOTEX, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_IDC_PT_S_MV_USINGVD, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDC_PT_S_MV_LIGHTMAP, &PLY_Tweaker::OnBnClickedMVFlags)
	ON_BN_CLICKED(IDOK, &PLY_Tweaker::OnBnClickedOk)
	ON_EN_CHANGE(IDC_PT_MESH_PATH1, &PLY_Tweaker::OnEnChangePtMeshPath1)
	ON_EN_CHANGE(IDC_PT_MESH_PATH2, &PLY_Tweaker::OnEnChangePtMeshPath2)
END_MESSAGE_MAP()


// Обработчики сообщений PLY_Tweaker

void PLY_Tweaker::DataDEL()
{
	for(int i = 0; i < m_meshs; i++)
		{memset(&m_mesh_base[i], 0, sizeof(Tply_mesh_base));}
	if(m_bones && m_bonelist)
	{
		for(int i = 0; i < m_bones; i++)
		{
			if(m_bonelist[i])
				{delete[] m_bonelist[i];}
		}
		delete[] m_bonelist;
	}
}

void PLY_Tweaker::DataINI()
{
	MROR = SHDW = ADJA = BNDS = BPly_Type = FALSE;
	memset(m_basis, 0, sizeof(v3_t) * 4);
	memset(m_bbox, 0, sizeof(v3_t) * 2);
	m_meshs = 256;
	for(int i = 0; i < m_meshs; i++)
		{memset(&m_mesh_base[i], 0, sizeof(Tply_mesh_base));}
	IndexType = 0;
	GlFlags = GlFvf = 0;
	m_vsize = m_vflags = m_calculated_vsize = 0;
	num_weights = num_tex_coords = m_bones = m_numpolys = m_numverts = m_meshs = 0;
	m_bonelist = NULL;
	has_mesh_specular = has_matrix_indices = has_pos = has_rhw = has_weights = has_normal = has_mesh_bump = FALSE;
	has_w = has_psize = has_diffuse = has_specular = has_tex_coords = has_specular = BNDS = FALSE;
}
//---------------------------------------------------------------------------

void PLY_Tweaker::OnLbnSelchangePtMaterialList()
{
	CListBox *pList;
	pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
	int indx = pList -> GetCurSel();
	if(indx >= 0)
	{
		CheckDlgButton(IDC_PT_SET_M_F, 1);
		CheckDlgButton(IDC_PT_SET_V_F, 0);
		OnBnClickedPtSetMF();
	}
	MtlParamUpdate();
}

void PLY_Tweaker::MtlParamUpdate()
{
	CListBox *pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
	CEdit *pEdit;
	CString PrintText;
	int indx = pList -> GetCurSel();
	if(indx >= 0)
	{
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_F_STR);
		PrintText = m_mesh_base[indx].text_msg_buff;
		pEdit -> SetWindowText(PrintText);
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_FVF);
		PrintText.Format("0x%x", m_mesh_base[indx].fvf);
		pEdit -> SetWindowText(PrintText);
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_FF);
		PrintText.Format("%d", m_mesh_base[indx].first_face);
		pEdit -> SetWindowText(PrintText);
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_FC);
		PrintText.Format("%d", m_mesh_base[indx].face_count);
		pEdit -> SetWindowText(PrintText);
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_F);
		PrintText.Format("0x%x", m_mesh_base[indx].flags);
		pEdit -> SetWindowText(PrintText);
		pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_PATH1);
		m_RewritePermission = FALSE;
		pEdit -> SetWindowText((char*)&m_mesh_base[indx].patch[0]);
		pEdit -> EnableWindow(true);
		if(BPly_Type)
		{
			pEdit = (CEdit *)GetDlgItem(IDC_PT_MESH_PATH2);
			pEdit -> SetWindowText((char*)&m_mesh_base[indx].patch[1]);
			pEdit -> EnableWindow(true);
		}
		m_RewritePermission = TRUE;
	}
}

void PLY_Tweaker::ParamUpdate()
{
	DWORD old_texcount = 0, new_texcount = 0, fvf_wo_texcount = 0;
	CEdit *pEdit;
	CString PrintText;
	GlFvf = GlFlags = 0;
	for(int i = 0; i < m_meshs; i++)
	{
		memset(m_mesh_base[i].text_msg_buff, 0, 2048);
		if(m_mesh_base[i].flags & MESH_FLAG_TWO_SIDED)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_TWO_SIDED\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_USE_ALPHA)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_USE_ALPHA\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_LIGHT)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_LIGHT\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_PLCR)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_PLCR\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_SKINNED)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_SKINNED\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_SHADOW)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_SHADOW\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_MIRRORED)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_MIRRORED\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_BLENDTEX)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_BLENDTEX\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_BUMP)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_BUMP\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_SPECULAR)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_SPECULAR\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_MATERIAL)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_MATERIAL\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_SUBSKIN)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_SUBSKIN\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_TWOTEX)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_TWOTEX\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_USINGVD)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_USINGVD\r\n");}
		if(m_mesh_base[i].flags & MESH_FLAG_LIGHTMAP)
			{strcat(m_mesh_base[i].text_msg_buff, "MESH_FLAG_LIGHTMAP\r\n");}
		if(GlFvf)
		{
			fvf_wo_texcount = (GlFvf | m_mesh_base[i].fvf) & (0xffff ^ D3DFVF_TEXCOUNT_MASK);
			old_texcount = GlFvf & D3DFVF_TEXCOUNT_MASK;
			new_texcount = m_mesh_base[i].fvf & D3DFVF_TEXCOUNT_MASK;
			if(old_texcount > new_texcount)
				{GlFvf = fvf_wo_texcount | old_texcount;}
			else
			{
				GlFvf = fvf_wo_texcount | new_texcount;
				GlFlags = GlFlags | m_mesh_base[i].flags;
			}
		}
		else
			{GlFvf = m_mesh_base[i].fvf; GlFlags = m_mesh_base[i].flags;}
	}
	has_mesh_specular = ((GlFlags & MESH_FLAG_SPECULAR) != 0);
	if(has_rhw || has_w)
		{CheckDlgButton(IDC_PT_VD_RHW, 1);}
	else
		{CheckDlgButton(IDC_PT_VD_RHW, 0);}
	if(has_matrix_indices)
		{CheckDlgButton(IDC_PT_VD_IND, 1);}
	else
		{CheckDlgButton(IDC_PT_VD_IND, 0);}
	if(has_mesh_bump)
		{CheckDlgButton(IDC_PT_VD_BUMP, 1);}
	else
		{CheckDlgButton(IDC_PT_VD_BUMP, 0);}
	if(has_tex_coords)
		{CheckDlgButton(IDC_PT_VD_TEX_COORDS, 1);}
	else
		{CheckDlgButton(IDC_PT_VD_TEX_COORDS, 0);}
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_VS);
	PrintText.Format("Vertex size: %d", m_vsize);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_CVS);
	PrintText.Format("Сalculated vertex size: %d", m_calculated_vsize);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_VC);
	PrintText.Format("Vertex count: %d", m_numverts);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_VF);
	PrintText.Format("Vertex flags: 0x%x", m_vflags);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_FVF);
	PrintText.Format("Flexible Vertex Format: 0x%d", GlFvf);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_NW);
	PrintText.Format("Num weights: %d", num_weights);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_PC);
	PrintText.Format("Polygons count: %d", m_numpolys);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_VD_UV);
	PrintText.Format("Num Tex. Coords (UV): %d", num_tex_coords);
	pEdit -> SetWindowText(PrintText);
	pEdit = (CEdit *)GetDlgItem(IDC_PT_INDEX_TYPE);
	switch(IndexType)
	{
		case 1:
			{pEdit -> SetWindowText("INDX");}
		break;
		case 2:
			{pEdit -> SetWindowText("IND4");}
		break;
	};
}
//---------------------------------------------------------------------------

void PLY_Tweaker::FVFUPD()
{
	if(GlFvf & D3DFVF_RESERVED0)
		{CheckDlgButton(IDC_PT_VD_RESERVED0, 1);}
	if(GlFvf & D3DFVF_POSITION_MASK)
		{CheckDlgButton(IDC_PT_VD_POSITION_MASK, 1);}
	if(GlFvf & D3DFVF_XYZ)
		{CheckDlgButton(IDC_PT_VD_XYZ, 1);}
	if(GlFvf & D3DFVF_XYZRHW)
		{CheckDlgButton(IDC_PT_VD_XYZRHW, 1);}
	if(GlFvf & D3DFVF_XYZB1)
		{CheckDlgButton(IDC_PT_VD_XYZB1, 1);}
	if(GlFvf & D3DFVF_XYZB2)
		{CheckDlgButton(IDC_PT_VD_XYZB2, 1);}
	if(GlFvf & D3DFVF_XYZB3)
		{CheckDlgButton(IDC_PT_VD_XYZB3, 1);}
	if(GlFvf & D3DFVF_XYZB4)
		{CheckDlgButton(IDC_PT_VD_XYZB4, 1);}
	if(GlFvf & D3DFVF_XYZB5)
		{CheckDlgButton(IDC_PT_VD_XYZB5, 1);}
	if(GlFvf & D3DFVF_XYZW)
		{CheckDlgButton(IDC_PT_VD_XYZW, 1);}
	if(GlFvf & D3DFVF_NORMAL)
		{CheckDlgButton(IDC_PT_VD_NORMAL, 1);}
	if(GlFvf & D3DFVF_PSIZE)
		{CheckDlgButton(IDC_PT_VD_PSIZE, 1);}
	if(GlFvf & D3DFVF_DIFFUSE)
		{CheckDlgButton(IDC_PT_VD_DIFFUSE, 1);}
	if(GlFvf & D3DFVF_SPECULAR)
		{CheckDlgButton(IDC_PT_VD_SPECULAR, 1);}
	if(GlFvf & D3DFVF_TEXCOUNT_MASK)
		{CheckDlgButton(IDC_PT_VD_TEXCOUNT_MASK, 1);}
	if(GlFvf & D3DFVF_TEX0)
		{CheckDlgButton(IDC_PT_VD_TEX0, 1);}
	if(GlFvf & D3DFVF_TEX1)
		{CheckDlgButton(IDC_PT_VD_TEX1, 1);}
	if(GlFvf & D3DFVF_TEX2)
		{CheckDlgButton(IDC_PT_VD_TEX2, 1);}
	if(GlFvf & D3DFVF_TEX3)
		{CheckDlgButton(IDC_PT_VD_TEX3, 1);}
	if(GlFvf & D3DFVF_TEX4)
		{CheckDlgButton(IDC_PT_VD_TEX4, 1);}
	if(GlFvf & D3DFVF_TEX5)
		{CheckDlgButton(IDC_PT_VD_TEX5, 1);}
	if(GlFvf & D3DFVF_TEX6)
		{CheckDlgButton(IDC_PT_VD_TEX6, 1);}
	if(GlFvf & D3DFVF_TEX7)
		{CheckDlgButton(IDC_PT_VD_TEX7, 1);}
	if(GlFvf & D3DFVF_TEX8)
		{CheckDlgButton(IDC_PT_VD_TEX8, 1);}
	if(GlFvf & D3DFVF_LASTBETA_UBYTE4)
		{CheckDlgButton(IDC_PT_VD_LASTBETA_UBYTE4, 1);}
	if(GlFvf & D3DFVF_LASTBETA_D3DCOLOR)
		{CheckDlgButton(IDC_PT_VD_LASTBETA_D3DCOLOR, 1);}
	if(GlFvf & D3DFVF_RESERVED2)
		{CheckDlgButton(IDC_PT_VD_RESERVED2, 1);}
}

void PLY_Tweaker::OnBnClickedPtSetMF()
{
	CListBox *pList;
	pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
	int indx = pList -> GetCurSel();
	CheckDlgButton(IDC_PT_SET_V_F, 0);
	FlagsUPD(m_mesh_base[indx].flags);
}

void PLY_Tweaker::OnBnClickedPtSetVF()
{
	CheckDlgButton(IDC_PT_SET_M_F, 0);
	FlagsUPD(m_vflags);
}

void PLY_Tweaker::FlagsUPD(DWORD flags)
{
	if(flags & MESH_FLAG_TWO_SIDED)
		{CheckDlgButton(IDC_PT_S_MV_TWO_SIDED, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_TWO_SIDED, 0);}
	if(flags & MESH_FLAG_USE_ALPHA)
		{CheckDlgButton(IDC_PT_S_MV_USE_ALPHA, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_USE_ALPHA, 0);}
	if(flags & MESH_FLAG_LIGHT)
		{CheckDlgButton(IDC_PT_S_MV_LIGHT, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_LIGHT, 0);}
	if(flags & MESH_FLAG_PLCR)
		{CheckDlgButton(IDC_PT_S_MV_PLCR, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_PLCR, 0);}
	if(flags & MESH_FLAG_SKINNED)
		{CheckDlgButton(IDC_PT_S_MV_SKINNED, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_SKINNED, 0);}
	if(flags & MESH_FLAG_SHADOW)
		{CheckDlgButton(IDC_PT_S_MV_SHADOW, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_SHADOW, 0);}
	if(flags & MESH_FLAG_MIRRORED)
		{CheckDlgButton(IDC_PT_S_MV_MIRRORED, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_MIRRORED, 0);}
	if(flags & MESH_FLAG_BLENDTEX)
		{CheckDlgButton(IDC_PT_S_MV_BLENDTEX, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_BLENDTEX, 0);}
	if(flags & MESH_FLAG_BUMP)
		{CheckDlgButton(IDC_PT_S_MV_BUMP, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_BUMP, 0);}
	if(flags & MESH_FLAG_SPECULAR)
		{CheckDlgButton(IDC_PT_S_MV_SPECULAR, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_SPECULAR, 0);}
	if(flags & MESH_FLAG_MATERIAL)
		{CheckDlgButton(IDC_PT_S_MV_MATERIAL, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_MATERIAL, 0);}
	if(flags & MESH_FLAG_SUBSKIN)
		{CheckDlgButton(IDC_PT_S_MV_SUBSKIN, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_SUBSKIN, 0);}
	if(flags & MESH_FLAG_TWOTEX)
		{CheckDlgButton(IDC_PT_S_MV_TWOTEX, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_TWOTEX, 0);}
	if(flags & MESH_FLAG_USINGVD)
		{CheckDlgButton(IDC_IDC_PT_S_MV_USINGVD, 1);}
	else
		{CheckDlgButton(IDC_IDC_PT_S_MV_USINGVD, 0);}
	if(flags & MESH_FLAG_LIGHTMAP)
		{CheckDlgButton(IDC_PT_S_MV_LIGHTMAP, 1);}
	else
		{CheckDlgButton(IDC_PT_S_MV_LIGHTMAP, 0);}
}

void PLY_Tweaker::OnBnClickedMVFlags()
{
	CListBox *pList;
	DWORD flags = 0;
	pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
	int indx = pList -> GetCurSel();
	if(IsDlgButtonChecked(IDC_PT_S_MV_TWO_SIDED))
		{flags |= MESH_FLAG_TWO_SIDED;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_USE_ALPHA))
		{flags |= MESH_FLAG_USE_ALPHA;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_LIGHT))
		{flags |= MESH_FLAG_LIGHT;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_PLCR))
		{flags |= MESH_FLAG_PLCR;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_SKINNED))
		{flags |= MESH_FLAG_SKINNED;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_SHADOW))
		{flags |= MESH_FLAG_SHADOW;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_MIRRORED))
		{flags |= MESH_FLAG_MIRRORED;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_BLENDTEX))
		{flags |= MESH_FLAG_BLENDTEX;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_BUMP))
		{flags |= MESH_FLAG_BUMP;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_SPECULAR))
		{flags |= MESH_FLAG_SPECULAR;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_MATERIAL))
		{flags |= MESH_FLAG_MATERIAL;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_SUBSKIN))
		{flags |= MESH_FLAG_SUBSKIN;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_TWOTEX))
		{flags |= MESH_FLAG_TWOTEX;}
	if(IsDlgButtonChecked(IDC_IDC_PT_S_MV_USINGVD))
		{flags |= MESH_FLAG_USINGVD;}
	if(IsDlgButtonChecked(IDC_PT_S_MV_LIGHTMAP))
		{flags |= MESH_FLAG_LIGHTMAP;}
	if(IsDlgButtonChecked(IDC_PT_SET_M_F))
		{m_mesh_base[indx].flags = flags;}
	else
	if(IsDlgButtonChecked(IDC_PT_SET_V_F))
		{m_vflags = flags;}
	ParamUpdate();
	MtlParamUpdate();
	// TODO: добавьте свой код обработчика уведомлений
}

void PLY_Tweaker::OnBnClickedOk()
{
	#ifdef ALTERNATIVE_LANG
		if(MessageBox("Do you confirm overwriting the file?", "QUESTION: PLY_Tweaker::OnBnClickedOk", MB_YESNO | MB_ICONQUESTION) == IDYES)
	#else
		if(MessageBox("Вы подтверждаете перезапись файла?", "QUESTION: PLY_Tweaker::OnBnClickedOk", MB_YESNO | MB_ICONQUESTION) == IDYES)
	#endif
		{CDialog::OnOK();}
}

void PLY_Tweaker::OnEnChangePtMeshPath1()
	{OnEnChangePtMeshPaths(IDC_PT_MESH_PATH1);}


void PLY_Tweaker::OnEnChangePtMeshPath2()
	{OnEnChangePtMeshPaths(IDC_PT_MESH_PATH2);}

void PLY_Tweaker::OnEnChangePtMeshPaths(int id)
{
	if(!m_RewritePermission)
		{return;}
	m_RewritePermission = FALSE;
	CListBox *pList = (CListBox *)GetDlgItem(IDC_PT_MATERIAL_LIST);
	int indx = pList -> GetCurSel();
	if(indx < 0)
		{m_RewritePermission = TRUE; return;}
	CEdit *pEdit = (CEdit *)GetDlgItem(id);
	switch(id)
	{
		case IDC_PT_MESH_PATH1:
			{pEdit -> GetWindowText(m_mesh_base[indx].patch[0], _MAX_PATH - 1);}
		break;
		case IDC_PT_MESH_PATH2:
			{pEdit -> GetWindowText(m_mesh_base[indx].patch[1], _MAX_PATH - 1);}
		break;
	};
	m_RewritePermission = TRUE;
}
