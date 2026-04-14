// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "stdio.h"
#include "io.h"

#include "CTexture.h"
#include "mainfrm.h"
#include <iostream>

extern HWND m_hWnd;
using namespace std;

// This constructor assumes that the calling calling "fixed" the texture file name prior to instantiating this class
CTexture::CTexture(char *texfile)
{
	FILE *testfp = NULL;
	m_Filtering = 0;
	m_MipMap = 0;
	m_Blend = NULL;
	m_AlphaChannel = 0;
	m_FrameCnt = 0;
	m_FrameName = NULL;
	m_Frames = NULL;
	m_BitMap = false;
	m_TextureAnimation = 0;
	m_forced_alpha_test = false;
	m_Animation = false;
	m_Loop = false;
	m_AsyncStart = false;
	m_Play = false;
	m_sdl = NULL;
    char ExtTest[MAX_PATH] = {0};
	char CurFile[MAX_PATH] = {0}, *CurFile_ptr;
	CurFile_ptr = &CurFile[0];
	strcpy(ExtTest, texfile);
	m_TexFile = new char[strlen(texfile) + 1];
	memset(m_TexFile, 0, strlen(texfile) + 1);
	strcpy(m_TexFile, texfile);
	strcpy(CurFile_ptr, texfile);
	bool link_type = false, finding = false;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	All_Trash *AT = new All_Trash();
	string st;
	st = texfile;
	size_t len = strlen(st.c_str());
	size_t idx = len - 1;
	char ErrMsg[512] = {0};
	int skip_item = -1;
	for(size_t i = 0; *(st.c_str() + i); i++)
	{
		if(*(st.c_str() + i) == L'.')
			{idx = i;}
		else
		if(*(st.c_str() + i) == L'/' || *(st.c_str() + i) == L'\\')
			{idx = len - 1;}
	}
	if(!stricmp((char*)st.substr(idx + 1).c_str(), "tex"))
	{
		m_forced_alpha_test = true;
		repeat_tex:
		if(m_FrameName)
			{delete[] m_FrameName; m_FrameName = NULL;}
		if(m_sdl)
			{delete m_sdl; m_sdl = NULL;}
		m_sdl = new CSdl(m_TexFile);
		ParseFile();
		link_type = true;
	}
	else
	if(!stricmp((char*)st.substr(idx + 1).c_str(), "mtl"))
	{
		repeat_mtl:
		if(m_FrameName)
			{delete[] m_FrameName; m_FrameName = NULL;}
		if(m_sdl)
			{delete m_sdl; m_sdl = NULL;}
		Parse_Mtl *PM = new Parse_Mtl();
		m_sdl = new CSdl(m_TexFile);
		TextPoint TP = PM -> ParseMTL(m_sdl -> GetBuffer());
		if(TP.error)
		{
			char text_buff[512] = {0};
			#ifdef ALTERNATIVE_LANG
				sprintf(text_buff, "File parsing error %s\nTextPoint = %d", m_TexFile, TP.TP);
			#else
				sprintf(text_buff, "Ошибка парсинга файла %s\nTextPoint = %d", m_TexFile, TP.TP);
			#endif
				if (pWnd->m_ShowTextureErrors)
				{
					MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, text_buff, "ERROR: CTexture::CTexture", MB_ICONHAND);
				}
		}
		link_type = true;
		if(!strlen(PM -> TDiffuse))
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(ErrMsg, "The materials file does not contain data about the Diffuse-texture: %s", CurFile_ptr);
			#else
				sprintf(ErrMsg, "Файл материалов не содержит данных о Diffuse-текстуре: %s", CurFile_ptr);
			#endif
				if (pWnd->m_ShowTextureErrors)
				{
					MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, ErrMsg, "ERROR: CTexture::CTexture", MB_ICONHAND);
				}
			m_FrameName = new char[12];
			strcpy(m_FrameName, "<<checker>>");
			delete PM;
			delete AT;
			return;
		}
		m_FrameName = new char[strlen(PM -> TDiffuse) + 1];
		strcpy(m_FrameName, PM -> TDiffuse);
		m_Filtering = 1; 
		m_AlphaChannel = (PM -> AlphaChannel) ? 1 : 0;
		if(m_AlphaChannel)
		{
			if(m_Blend)
			{
				delete[] m_Blend;
				m_Blend = NULL;
			}
			m_Blend = new char[8];
			memset(m_Blend, 0, 8);
			switch(PM -> AlphaChannel)
			{
				case 1:
					{strcpy(m_Blend, "test\0");}
				break;
				case 2:
					{strcpy(m_Blend, "blend\0");}
				break;
				case 3:
					{strcpy(m_Blend, "add_sub\0");}
				break;
			};
		}
		delete PM;
	}
	else
	{
		m_FrameName = new char[strlen(CurFile_ptr) + 1];
		memset(m_FrameName, 0, strlen(CurFile_ptr) + 1);
		strcpy(m_FrameName, CurFile_ptr);
		finding = true;
	}
	if(link_type && m_FrameName)
	{
		FixPathDelim(CurFile_ptr);
		strlwr(CurFile_ptr);
		FixPathDelim(m_FrameName);
		testfp = NULL;
		char PathTest[MAX_PATH] = {0};
		char *delim = strrchr(CurFile_ptr, '/') + 1, *sptr = "";
		bool tex_test = false, find_mask = false, path_type = false;
		char independent_path[_MAX_PATH] = {0};
		for(int path_search = 6; path_search > ((!path_type) ? 0 : -1) && !tex_test; path_search--)
		{
			sptr = "";
			memset(PathTest, 0, MAX_PATH);
			if((!find_mask && path_type) && path_search < 6)
				{break;}
			if(m_FrameName[0] == '$')
			{
				if(!strlen(pWnd -> m_ResPath[(path_search == 6) ? 0 : path_search]))
					{continue;}
				sptr = m_FrameName;
				sptr += 2;
				sprintf(PathTest, "%s%s", pWnd -> m_ResPath[(path_search == 6) ? 0 : path_search], "resource\\texture\\common\\");
			}
			else
			if(m_FrameName[0] == '/')
			{
				if(!strlen(pWnd -> m_ResPath[(path_search == 6) ? 0 : path_search]))
					{continue;}
				sptr = m_FrameName;
				sprintf(PathTest, "%s%s", pWnd -> m_ResPath[(path_search == 6) ? 0 : path_search], "resource");
			}
			else
			{
				sptr = m_FrameName;
				if(path_search == 6)//для первого раза
				{
					path_type = true;//путь от файлов
					strncpy(PathTest, CurFile_ptr, (int)(delim - CurFile_ptr));//папка по умолчанию
					for(int path_mask = 5; path_mask > -1; path_mask--)
					{
						int ResPath_len = strlen(pWnd -> m_ResPath[path_mask]);
						char tmp_res_path[_MAX_PATH] = {0};
						strcpy(tmp_res_path, pWnd -> m_ResPath[path_mask]);
						FixPathDelim(tmp_res_path);
						strlwr(tmp_res_path);
						if(ResPath_len && strstr(PathTest, tmp_res_path))
						{
							find_mask = true;
							strncpy(independent_path, PathTest + ResPath_len, strlen(PathTest) - ResPath_len);
							skip_item = path_mask;
							break;
						}
					}
				}
				else
				{
					if(skip_item >= 0 && path_search == skip_item)
						{continue;}
					if(!strlen(pWnd -> m_ResPath[path_search]))
						{continue;}
					sprintf(PathTest, "%s%s", pWnd -> m_ResPath[path_search], independent_path); //папка мода
				}
			}
			strcat(PathTest, sptr);
			FixPathDelim(PathTest);
			AT -> Parse_Path(PathTest);
			memset(ExtTest, 0, MAX_PATH);
			strcpy(ExtTest, PathTest);
			if(!(testfp = fopen(ExtTest, "r")))//проверка найденного
			{
				for(int texsigindx = 0; tex_siglist[texsigindx]; texsigindx++)
				{
					strcpy(ExtTest, PathTest);
					strcat(ExtTest, tex_siglist[texsigindx]);
					if(testfp = fopen(ExtTest, "r"))
					{
						if((stricmp(tex_siglist[texsigindx], ".TEX") == 0 || stricmp(tex_siglist[texsigindx], ".MTL") == 0) && stricmp(ExtTest, texfile))
						{
							delete[] m_TexFile;
							m_TexFile = NULL;
							m_TexFile = new char[strlen(ExtTest) + 1];
							memset(m_TexFile, 0, strlen(ExtTest) + 1);
							strcpy(m_TexFile, ExtTest);
							strcpy(CurFile_ptr, ExtTest);
							fclose(testfp);
							testfp = NULL;
							if(tex_siglist[texsigindx][1] == 'T')
								{goto repeat_tex;}
							else
								{goto repeat_mtl;}
						}
						tex_test = true;
						break;
					}
				}
			}
			if(testfp != NULL)
			{
				fclose(testfp);
				testfp = NULL;
				finding = true;
				break;
			}
		}
		if(finding)
		{
			delete[] m_FrameName;
			m_FrameName = NULL;
			m_FrameName = new char[strlen(ExtTest) + 1];
			memset(m_FrameName, 0, strlen(ExtTest) + 1);
			strcpy(m_FrameName, ExtTest);
		}
		else
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(ErrMsg, "Couldn't find texture: %s\nFrom the materials file:\n%s", m_FrameName, CurFile_ptr);
			#else
				sprintf(ErrMsg, "Не удалось найти текстуру: %s\nИз файла материалов:\n%s", m_FrameName, CurFile_ptr);
			#endif
				if (pWnd->m_ShowTextureErrors)
				{
					MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, ErrMsg, "ERROR: CTexture::CTexture", MB_ICONHAND);
				}
			delete[] m_FrameName;
			m_FrameName = NULL;
			m_FrameName = new char[12];
			memset(m_FrameName, 0, 12);
			strcpy(m_FrameName, "<<checker>>");
		}
	}
	delete AT;
}

CTexture::~CTexture()
{
    if(m_Blend)
       {delete[] m_Blend; m_Blend = NULL;}
    if(m_FrameName)
       {delete[] m_FrameName; m_FrameName = NULL;}
    if(m_Frames)
       {delete [] m_Frames; m_Frames = NULL;}
    if(m_TexFile)
       {delete[] m_TexFile; m_TexFile = NULL;}
	if(m_sdl)
		{delete m_sdl; m_sdl = NULL;}
}

void CTexture::ParseFile()
{
	char *word = m_sdl -> GetNextWord();
	if(word == NULL)
		{return;}
	while(strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{}
		else
		if(!stricmp(word, "}"))// Ending our processing block -- drop out here
			{break;}
		else
		if(!stricmp("bitmap", word))
			{m_BitMap = true;}
		else
		if(!stricmp("animation", word))
			{m_Animation = true;}
		else
		if(!stricmp("asyncstart", word))
			{m_AsyncStart = true;}
		else
		if(!stricmp("loop", word))
			{ m_Loop = true;}
		else
		if(!stricmp("play", word))
			{m_Play = true;}
		else
		if(!stricmp("filtering", word))
			{SetFiltering();}
		else
		if(!stricmp("mipmap", word))
			{SetMipMap();}
		else
		if(!stricmp("blend", word))
			{SetBlend();}
		else
		if(!stricmp("textureanimation", word))
			{SetTextureAnimation();}
		else
		if(!stricmp("alphachannel", word))
			{SetAlphaChannel();}
		else
		if(!stricmp("frame", word))
			{SetFrame();}
        else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

void CTexture::SetFiltering()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_Filtering = atoi(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"FILTERING\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"FILTERING\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetFiltering", MB_ICONHAND);
	}
}

void CTexture::SetMipMap()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_MipMap = atoi(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"MIPMAP\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"MIPMAP\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetMipMap", MB_ICONHAND);
	}
}

void CTexture::SetBlend()
{
	char *word;
	word = m_sdl -> GetNextWord();
	if(m_Blend)
	{
		delete[] m_Blend;
		m_Blend = NULL;
	}
	m_Blend = new char[strlen(word)+1];
	strcpy(m_Blend, word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"BLEND\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"BLEND\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetBlend", MB_ICONHAND);
	}
}

void CTexture::SetAlphaChannel()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_AlphaChannel = atoi(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"ALPHACHANNEL\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"ALPHACHANNEL\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetAlphaChannel", MB_ICONHAND);
	}
}

void CTexture::SetTextureAnimation()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_TextureAnimation = atoi(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"TEXTUREANIMATION\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"TEXTUREANIMATION\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetTextureAnimation", MB_ICONHAND);
	}
}

void CTexture::SetFrame()
{
    char *word;
	word = m_sdl -> GetNextWord();
	if(!m_FrameName)
	{
		m_FrameName = new char[strlen(word)];
		strcpy(m_FrameName, word + 1);
	}
	word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"FRAME\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"FRAME\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CTexture::SetFrame", MB_ICONHAND);
	}
}

