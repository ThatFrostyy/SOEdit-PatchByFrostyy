// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include "general.h"
#include "CAnimation.h"
#include "CSdl.h"

extern HWND m_hWnd;

CAnimation::CAnimation()
{
	m_sdl = NULL;
	m_SequenceList = NULL;
}

CAnimation::CAnimation(CSdl *sdl)// Setup our sequence list
{
	m_sdl = sdl;
	m_SequenceList = new CSequenceList(m_sdl);
	ParseFile();
}

CAnimation::~CAnimation()
{
	delete m_SequenceList;
	m_SequenceList = NULL;
}

void CAnimation::ParseFile()
{
	char *word = m_sdl -> GetNextWord();
	while(strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{}
		else
		if(!stricmp(word, "}"))// Ending our processing block -- drop out here
			{break;}
		else
		if(!stricmp("sequence", word))
			{AddSequence();}
		else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CAnimation::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

void CAnimation::AddSequence()
	{m_SequenceList -> AddToTail();}
