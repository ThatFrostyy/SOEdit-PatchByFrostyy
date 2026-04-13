// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "CSdl.h"


CSdl::CSdl(char *filename)
{
    int fsize = 0;
	read_comments = false;
	StartComments = "";
    if(!filename)
		{return;}
	if(strlen(filename))
	{
		m_szFile = new char[strlen(filename) + 1];
		strcpy(m_szFile, filename);
	}
    m_buffer = 0;
    m_cursor = 0;
    m_line = 1;
    m_comment = false;
    m_quote = false;
    memset(m_word, 0, 1024);
	HANDLE HFile = NULL;
	FILE_STANDARD_INFO fileInfo;
	DWORD BytesIO = 0;
	HFile = CreateFileA(filename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if(HFile == INVALID_HANDLE_VALUE || HFile == NULL)
	{
		// Throw an exception here
		#ifdef ALTERNATIVE_LANG
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Unable to open the file:\n" + CString(filename)), "ERROR CSdl::CSdl", MB_OK);
		#else
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Невозможно открыть файл:\n" + CString(filename)), "ERROR CSdl::CSdl", MB_OK);
		#endif
	}
	else
	{
		GetFileInformationByHandleEx(HFile, FileStandardInfo, &fileInfo, sizeof(fileInfo));
		fsize = fileInfo.EndOfFile.LowPart;
		m_buffer = new char[fsize + 1];
		memset(m_buffer, 0, fsize + 1);
		ReadFile(HFile, m_buffer, fsize, &BytesIO, NULL);
		CloseHandle(HFile);
		m_cursor = m_buffer;
	}
}

CSdl::CSdl(CString str)
{
	read_comments = false;
	StartComments = "";
	m_buffer = NULL;
    m_cursor = NULL;
    m_line = 1;
    m_comment = false;
    m_quote = false;
    memset(m_word, 0, 1024);
	m_buffer = new char[str.GetLength() + 1];
	memset(m_buffer, 0, str.GetLength() + 1);
	strcpy(m_buffer, str.GetBuffer(1));
	m_cursor = m_buffer;
	m_szFile = NULL;
}

CSdl::~CSdl()
{
	if(m_szFile)
	{
		delete[] m_szFile;
		m_szFile = NULL;
	}
	if(m_buffer)
	{
		delete[] m_buffer;
		m_buffer = NULL;
	}
}

bool CSdl::IsOpen()
{
	if(m_buffer)
		{return true;}
	else
		{return false;}
}

char *CSdl::SkipWhiteSpace(char *buffer)
{
	char *c = buffer;
	while((*c == ' ') || (*c == '\t') || (*c == '\r') || (*c == '\n'))
	{
		if(*c == '\n')
			{m_line++;}
		c++;
	}
	return c;
}

char *CSdl::SkipComment(char *buffer)
{
	char *c = buffer;
	char *tmp_str;
	while(*c != '\n' && *c != '\0')
		{c++;}
	int st_len = (int)(c - buffer) + 2;
	tmp_str = new char[st_len];
	memset(tmp_str, 0, st_len);
	strncpy(tmp_str, buffer, st_len - 1);
	StartComments += tmp_str;
	delete[] tmp_str;
	return SkipWhiteSpace(c);
}

char *CSdl::GetBuffer()
	{return m_buffer;}

char *CSdl::GetNextWord()
{
    char *c;
    memset(m_word, '\0', 1024);
    c = m_word;
    if(m_cursor ==  0)
		{return m_word;}
    m_cursor = SkipWhiteSpace(m_cursor);
    if(*m_cursor == '\0')
		{return m_word;}
	while(*m_cursor)
	{
        /*if(m_comment)
		{
            if(*m_cursor == '\n')
			{
                m_line++;
                m_comment = false;
                m_cursor = SkipWhiteSpace(m_cursor);
			}
            m_cursor++;
            continue;
		}
        else*/
        if(m_quote)
		{
			if(*m_cursor == '"')
			{
                // found the end of a quote
                m_quote = false;  // We've found the end of the quote
                // We don't want the quote as part of the word
                //*c++ = *m_cursor++;
                m_cursor++;  // skip to the next char (whatever it is)
                break;
			}
		}
        else
        if(*m_cursor == ';')
		{
            if(c != m_word)
				{break;}
			m_cursor = SkipComment(m_cursor);
            //m_comment = true;
            //m_cursor++;
            continue;
		}
        else
        if(*m_cursor == '"')
		{
			if(c != m_word)
				{break;}
			// beginning a quote -- we'll break when it's done
            m_quote = true;
            //m_cursor++;
		}
        else
        if((*m_cursor == ' ') || (*m_cursor == '\t') || (*m_cursor == '\r') || (*m_cursor == '\n'))
		{
            // This is white space, stop here!
            break;
		}
        else
        if((*(m_cursor + 1) == '{') || (*(m_cursor+1) == '}'))
		{
            *c++ = *m_cursor;
            m_cursor++;
            break;
		}
        else
        if((*m_cursor == '{') || (*m_cursor == '}'))
		{
            *c++ = *m_cursor;
            m_cursor++;
            break;
		}
		else
		if(*m_cursor == '=')
		{
			m_cursor++;
			break;
		}
        if((*(m_cursor + 1) == '{') || (*(m_cursor + 1) == '}'))
			{break;}
        *c++ = *m_cursor++;
	}
    *c = '\0';  // NULL terminate the word
    return m_word;
}