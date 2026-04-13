// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"
#include "CSObject.h"
#include "CSdl.h"

extern HWND m_hWnd;
CSObject::CSObject(char *filename)
{
	m_sdl = new CSdl(filename);
	ParseFile();
}

CSObject::~CSObject()
	{}

void CSObject::ParseFile()
{
	char *word = m_sdl -> GetNextWord();
	while (strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{}
		else
		if(!stricmp(word, "}"))// Ending our processing block
			{}
		else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSObject::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

