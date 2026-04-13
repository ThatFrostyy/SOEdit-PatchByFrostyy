// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CMesh.h"

CMesh::CMesh(char texname[4][_MAX_PATH], int tex_count, int fvf, DWORD flags)
{
	m_first = 0;
	m_count = 0;
	m_texname[0] = NULL;
	m_texnumb[0] = -1;
	m_texname[1] = NULL;
	m_texnumb[1] = -1;
	m_texname[2] = NULL;
	m_texname[3] = NULL;
	m_texcount = 0;
	m_texture[0] = m_texture[1] = NULL;
	char msgText[128];
	m_fvf = fvf;
	m_flags = flags;
	m_texcount = tex_count;
	m_transparency = false;
	m_forced_alpha_test = false;
	for(int i = 0; i < m_texcount; i++)
	{
		if(!strlen(texname[i]))
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(msgText, "The name of the material is missing");
			#else
				sprintf(msgText, "Отсутствует имя материала");
			#endif
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, msgText, "ERROR: CMesh::CMesh", MB_ICONHAND);
		}
		else
		{
			m_texname[i] = new char[strlen(texname[i]) + 1];
			strcpy(m_texname[i], texname[i]);
			m_texname[i + 2] = new char[strlen(texname[i + 2]) + 1];
			strcpy(m_texname[i + 2], texname[i + 2]);
			m_texture[i] = NULL;
			m_texture[i] = new CTexture(m_texname[i]);
		}
	}
	m_forced_alpha_test = m_texture[0] -> m_forced_alpha_test;
	next = NULL;
	prev = NULL;
}

CMesh::~CMesh()
{
	for(int i = 0; i < m_texcount; i++)
	{
		if(m_texname[i])
		{
			delete[] m_texname[i];
			m_texname[i] = NULL;
			delete[] m_texname[i + 2];
			m_texname[i + 2] = NULL;
		}
		if(m_texture[i])
		{
			delete m_texture[i];
			m_texture[i] = NULL;
		}
	}
}

CMeshList::CMeshList()
{
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
}

CMeshList::~CMeshList()
	{ClearList();}

CMesh *CMeshList::AddToHead(int first, int count, int fvf, char texname[4][_MAX_PATH], int tex_count, DWORD flags, DWORD specular_rgba_color, BYTE subskin_count, BYTE *subskin_bones)
{
	CMesh *t_mesh = new CMesh(texname, tex_count, fvf, flags);
	t_mesh -> m_first = first;
	t_mesh -> m_count = count;
	t_mesh -> specular_rgba_color = specular_rgba_color;
	t_mesh -> subskin_count = subskin_count;
	memcpy(t_mesh -> subskin_bones, t_mesh -> subskin_bones, subskin_count);
	if(m_head)
	{
		t_mesh -> next = m_head;
		m_head -> prev = t_mesh;
		m_head = t_mesh;
	}
	else
	{
		m_head = t_mesh;
		m_tail = t_mesh;
		m_curr = t_mesh;
	}
	return t_mesh;
}

CMesh *CMeshList::AddToTail(int first, int count, int fvf, char texname[4][_MAX_PATH], int tex_count, DWORD flags, DWORD specular_rgba_color, BYTE subskin_count, BYTE *subskin_bones)
{
	CMesh *t_mesh = new CMesh(texname, tex_count, fvf, flags);
	t_mesh -> m_first = first;
	t_mesh -> m_count = count;
	t_mesh -> specular_rgba_color = specular_rgba_color;
	t_mesh -> subskin_count = subskin_count;
	memcpy(t_mesh -> subskin_bones, t_mesh -> subskin_bones, subskin_count);
	if(m_tail)
	{
		t_mesh -> prev = m_tail;
		m_tail -> next = t_mesh;
		m_tail = t_mesh;
	}
	else
	{
		m_head = t_mesh;
		m_tail = t_mesh;
		m_curr = t_mesh;
	}
	return t_mesh;
}

CMesh *CMeshList::GetFirst()
{
	m_curr = m_head;
	return m_curr;
}

CMesh *CMeshList::GetNext()
{
	CMesh *t_mesh = NULL;
	if(m_curr && m_curr -> next)
	{
		m_curr = m_curr -> next;
		t_mesh = m_curr;
	}
	return t_mesh;
}

CMesh *CMeshList::GetPrev()
{
	CMesh *t_mesh = NULL;
	if(m_curr && m_curr -> prev)
	{
		m_curr = m_curr -> prev;
		t_mesh = m_curr;
	}
	return t_mesh;
}

CMesh *CMeshList::GetCurr()
	{return m_curr;}

void CMeshList::ClearList()
{
	CMesh *t_mesh;
	while(m_tail)
	{
		t_mesh = m_tail -> prev;
		delete m_tail;
		m_tail = t_mesh;
	}
	m_curr = NULL;
	m_head = NULL;
	m_tail = NULL;
}

CMesh *CMeshList::GetLast()
{
	m_curr = m_tail;
	return m_curr;
}
