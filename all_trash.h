#include "stdafx.h"

#ifndef ALL_TRASHH
#define ALL_TRASHH

struct _Pos
{
	unsigned long start;
	unsigned long end;
};

struct TextPoint
{
	unsigned long TP;
	bool error;
};

class All_Trash
{
    public:
	TextPoint find_nul(char* c, unsigned long sh);
	void CoordWrite(char* in, unsigned long in_start, unsigned long end, char* out, unsigned long out_start);
	void Write(char* in, unsigned long in_start, unsigned long end, char* out);
	TextPoint skip_whitespace(char* c, unsigned long sh);
	TextPoint skip_insignificantly(char* c, unsigned long sh);
	_Pos FindWord(char* c, unsigned long sh);
	TextPoint find_symbol(char* c, char s, unsigned long sh);
	TextPoint find_transfer(char* c, unsigned long sh);
	void Parse_Path(char *path);
};

#endif