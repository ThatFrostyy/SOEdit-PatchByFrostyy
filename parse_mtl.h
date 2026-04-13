#include "stdafx.h"
#include "all_trash.h"

#ifndef PARSE_MTL
#define PARSE_MTL

class Parse_Mtl
{
    public:
	char *TextBuffer;
	char TDiffuse[MAX_PATH];
	char TBump[MAX_PATH];
	char TSpecular[MAX_PATH];
	char AlphaChannel;
	All_Trash *AT;
	TextPoint ParseMTL(char *TextBuffer);
	Parse_Mtl();
	~Parse_Mtl();
	private:
	TextPoint ParseParameter(char *TextBuffer, unsigned long sh);
};

#endif


