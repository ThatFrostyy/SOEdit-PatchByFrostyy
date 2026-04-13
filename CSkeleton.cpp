// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CSkeleton.h"
#include "CSdl.h"

extern HWND m_hWnd;

CSkeleton::CSkeleton()
{
    m_sdl = NULL;
    m_bonelist = NULL;
	m_ChildLast = NULL;
    m_Animation = NULL;
	m_clean_on_destroy = true;
}

CSkeleton::CSkeleton(CSdl *sdl)
{
	m_sdl = sdl;
	m_bonelist = NULL;
	m_ChildLast = NULL;
	m_Animation = NULL;
	m_clean_on_destroy = true;
	ParseFile();
}

CSkeleton::CSkeleton(CSdl *sdl, bool mod)
{
	m_sdl = sdl;
	m_bonelist = NULL;
	m_ChildLast = NULL;
	m_Animation = NULL;
	m_clean_on_destroy = mod;
	ParseFile();
}

CSkeleton::~CSkeleton()
{
	if(m_clean_on_destroy)
		{DelAll(m_bonelist);}
	m_ChildLast = NULL;
	delete m_Animation;
	m_Animation = NULL;
}

void CSkeleton::DelAll(CBone *curbone)
{
	if(!curbone)
		{return;}
	CBone *child = NULL;
	if((child = curbone -> m_ChildFirst) != NULL)
		{DelAll(child);}
	if(curbone -> next)
		{DelAll(curbone -> next);}
	if(curbone)
	{
		delete curbone;
		curbone = NULL;
	}
}

void CSkeleton::ParseFile()
{
	char *word = m_sdl -> GetNextWord();
    while(strlen(word))
	{
        if(!stricmp(word, "{"))// Beginning our processing block
			{}
        else
        if(!stricmp(word, "}"))// Ending our processing block
			{break;}
        else
        if(!stricmp("bone", word))
			{AddBone();}
        else
        if(!stricmp("animation", word))
			{AddAnimation();}
        else
        if(!stricmp("extension", word))
			{AddExtension();}
		else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSkeleton::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

void CSkeleton::AddExtension()// ???
{
    char *word = m_sdl -> GetNextWord();
    word = m_sdl -> GetNextWord();
}

void CSkeleton::AddBone()
{
	CBone *t_bone = new CBone(m_sdl, NULL, 1, 0);
	if(!m_bonelist)
		{m_bonelist = t_bone;}
	else
	{
		t_bone -> prev = m_ChildLast;
		m_ChildLast -> next = t_bone;
	}
	m_ChildLast = t_bone;
}

void CSkeleton::AddAnimation()
	{m_Animation = new CAnimation(m_sdl);}


void CSkeleton::WriteMdl(FILE *fp, int indent)
{
	if(m_Animation)
	{
		if(m_Animation -> m_SequenceList)
			{m_Animation -> m_SequenceList -> WriteMdl(fp, indent);}
	}
	CBone *child = NULL;
	if((child = m_bonelist) != NULL)
	{
		while(child)
		{
			child -> WriteMdl(fp, indent);
			child = child -> next;
		}
	}
}