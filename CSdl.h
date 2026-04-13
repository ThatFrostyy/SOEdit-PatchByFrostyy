// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

#ifndef _CSDL_
#define _CSDL_

class CSdl
{
	public:
	CSdl(char *filename);
	CSdl(CString str);
	~CSdl();

	char  *m_szFile;
	char  *GetNextWord(void);
	bool  IsOpen();
	char  *GetBuffer(void);
	
	bool  read_comments;
	int   m_line;
	CString StartComments;

	private:
	char *m_buffer;
	char *m_cursor;
	bool  m_quote;
	bool  m_comment;
	int   m_block;

	char  m_word[1024];

	char *SkipWhiteSpace(char *);
	char *SkipComment(char *);
};

#endif