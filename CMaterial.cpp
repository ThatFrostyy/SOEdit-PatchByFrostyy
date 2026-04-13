// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CMaterial.h"
#include "CTexFile.h"

CMaterial::CMaterial(char *szName, int iTexNumb)
{
	m_szName = NULL;
	m_iTexNumb = 0;
	m_transparency = false;

	if(szName)
	{
		if(strlen(szName))
		{
			m_szName = new char[strlen(szName) + 1];
			strcpy(m_szName, szName);
		}
	}
	else
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "The name of the material is missing", "ERROR: CMaterial::CMaterial", MB_OK);
		#else
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Отсутствует имя материала", "ERROR: CMaterial::CMaterial", MB_OK);
		#endif
	}
	m_iTexNumb = iTexNumb;
	m_next = NULL;
	m_prev = NULL;
}

CMaterial::~CMaterial()
{
	if(m_szName)
	{
		delete[] m_szName;
		m_szName = NULL;
	}
	m_iTexNumb = 0;
}

CMtlList::CMtlList()
{
	m_curr = NULL;
	m_head = NULL;
	m_tail = NULL;
}

CMtlList::~CMtlList()
	{ClearList();}

CMaterial *CMtlList::FindMaterial(char *szName)
{
	CMaterial *t_mtl = m_head;
	if(!szName)
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "The name of the material is missing", "ERROR: CMtlList::FindMaterial", MB_OK);
		#else
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Отсутствует имя материала", "ERROR: CMtlList::FindMaterial", MB_OK);
		#endif
	}
	while(t_mtl && szName)
	{
		if(t_mtl -> m_szName && !stricmp(szName, t_mtl -> m_szName))
		{
			m_curr = t_mtl;
			return t_mtl;
		}
		t_mtl = t_mtl -> m_next;
	}
	return NULL;
}

CMaterial *CMtlList::GoToHead()
{
	m_curr = m_head;
	return m_curr;
}

CMaterial *CMtlList::GoToTail()
{
	m_curr = m_tail;
	return m_tail;
}

CMaterial *CMtlList::AddToHead(char *szName, int iTexNumb)
{
	CMaterial *t_mtl = new CMaterial(szName, iTexNumb);
	if(!m_head)
	{
		m_head = t_mtl;
		m_curr = t_mtl;
		m_tail = t_mtl;
	}
	else
	{
		m_head -> m_prev = t_mtl;
		m_head = t_mtl;
	}
	return t_mtl;
}

CMaterial *CMtlList::AddToTail(char *szName, int iTexNumb)
{
	CMaterial *t_mtl = new CMaterial(szName, iTexNumb);
	if(!m_tail)
	{
		m_head = t_mtl;
		m_curr = t_mtl;
		m_tail = t_mtl;
	}
	else
	{
		m_tail -> m_next = t_mtl;
		m_tail = t_mtl;
	}
	return t_mtl;
}

CMaterial *CMtlList::GetNext()
{
	if(!m_curr)
		{return m_curr;}
	if(m_curr -> m_next)
	{
		m_curr = m_curr -> m_next;
		return m_curr;
	}
	else
		{return NULL;}
}

CMaterial *CMtlList::GetPrev()
{
	if(!m_curr)
		{return m_curr;}
	if(m_curr -> m_prev)
	{
		m_curr = m_curr -> m_prev;
		return m_curr;
	}
	else
		{return NULL;}
}

bool CMtlList::DropItem(CMaterial *pItem)
{
	if(!pItem)
		{return false;}
	if(pItem -> m_prev)
	{
		pItem -> m_prev -> m_next = pItem -> m_next;
		if(m_curr == pItem)
			{m_curr = pItem -> m_prev;}
	}
	else
		{m_head = pItem -> m_next;}
	if(pItem -> m_next)
	{
		pItem -> m_next -> m_prev = pItem -> m_prev;
		if(m_curr == pItem)
			{m_curr = pItem -> m_next;}
	}
	else
		{m_tail = pItem -> m_prev;}
	delete pItem;
	return true;
}

void CMtlList::ClearList()
{
	CMaterial *t_mtl = m_head;
	while(t_mtl)
	{
		m_head = t_mtl -> m_next;
		delete t_mtl;
		t_mtl = m_head;
	}
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
}

CMaterial *CMtlList::InsertBefore(char *szName, int iTexNumb)
{
	CMaterial *t_mtl = new CMaterial(szName, iTexNumb);
	if(!m_curr)
	{
		m_curr = t_mtl;
		m_head = t_mtl;
		m_tail = t_mtl;
	}
	else
	{
		if(m_curr -> m_prev)
			{m_curr -> m_prev -> m_next = t_mtl;}
		else
			{m_head = t_mtl;}
		t_mtl -> m_next = m_curr;
		m_curr -> m_prev = t_mtl;
	}
	return t_mtl;
}

CMaterial *CMtlList::InsertAfter(char *szName, int iTexNumb)
{
	CMaterial *t_mtl = new CMaterial(szName, iTexNumb);
	if(!m_curr)
	{
		m_curr = t_mtl;
		m_head = t_mtl;
		m_tail = t_mtl;
	}
	else
	{
		if(m_curr -> m_next)
			{m_curr -> m_next -> m_prev = t_mtl;}
		else
			{m_tail = t_mtl;}
		t_mtl -> m_prev = m_curr;
		m_curr -> m_next = t_mtl;
	}
	return t_mtl;
}
