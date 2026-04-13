#include "stdafx.h"
#include "stdio.h"
#include "all_trash.h"

//TODO:
//Add trash

TextPoint All_Trash::find_nul(char* c, unsigned long sh)
{
	TextPoint TP;
	TP.error = false;
	for(; c[sh] != 0; sh++)
	{}
	TP.TP = sh;
	return TP;
}

void All_Trash::CoordWrite(char* in, unsigned long in_start, unsigned long end, char* out, unsigned long out_start)
{
	for(;in_start < end; in_start++, out_start++)
		{out[out_start] = in[in_start];}
}

void All_Trash::Write(char* in, unsigned long in_start, unsigned long end, char* out)
{
	for(short i = 0; in_start < end && i < MAX_PATH; i++, in_start++)
		{out[i] = in[in_start];}
}

TextPoint All_Trash::skip_whitespace(char* c, unsigned long sh)
{
	TextPoint TP;
	TP.error = false;
	for(;c[sh] == ' ' || c[sh] == '\t'; sh++)
	{}
	if(c[sh] == '\0')
		{TP.error = true;}
	TP.TP = sh;
	return TP;
}

TextPoint All_Trash::skip_insignificantly(char* c, unsigned long sh)
{
	TextPoint TP;
	TP.error = false;
	for(;c[sh] == ' ' || c[sh] == '\t' || c[sh] == '\r' || c[sh] == '\n'; sh++)
	{}
	TP.TP = sh;
	if(c[sh] == '\0')
		{TP.error = true; return TP;}
	if(c[sh] == ';')
	{
		TP = find_transfer(c, sh);
		if(TP.error)
			{return TP;}
		TP = skip_insignificantly(c, TP.TP);//рекурсив
		if(TP.error)
			{return TP;}
	}
	return TP;
}

_Pos All_Trash::FindWord(char* c, unsigned long sh)
{
	_Pos pos;
	pos.start = 0;
	for(;c[sh] != ' ' && c[sh] != '\t' && c[sh] != '\r' && c[sh] != '\n' && c[sh] != '\0' && c[sh] != '"' && c[sh] != '{' && c[sh] != '}' && c[sh] != ';'; sh++)
	{}
	pos.end = sh;
	return pos;
}

TextPoint All_Trash::find_symbol(char* c, char s, unsigned long sh)
{
	TextPoint TP;
	TP.error = false;
	for(;c[sh] != s && c[sh] != '\0' && c[sh] != ';'; sh++)
	{}
	if(c[sh] == '\0' || c[sh] == ';')
		{TP.error = true;}
	TP.TP = sh;
	return TP;
}

TextPoint All_Trash::find_transfer(char* c, unsigned long sh)
{
	TextPoint TP;
	TP.error = false;
	for(;c[sh] != '\r' && c[sh] != '\n' && c[sh] != '\0'; sh++)
	{}
	if(c[sh] == '\0')
		{TP.error = true;}
	TP.TP = sh;
	return TP;
}

void All_Trash::Parse_Path(char *path)
{
	char **parts = NULL;
	parts = new char*[150];
	int part_cnt = 0;
	memset(parts, 0, 150 * sizeof(char*));
	int size = 0;
	char *pn, *ps, *pt, *p;
	p = path;
	for(;;)
	{
		pn = ps = pt = NULL;
		pn = strchr(p, '\0');
		ps = strchr(p, '//');
		pt = (ps) ? ps + 1 : pn;
		size = pt - p;
		if(size < 1)
			{break;}
		parts[part_cnt] = new char[size + 1];
		memset(parts[part_cnt], 0, size + 1);
		memcpy(parts[part_cnt], p, size);
		part_cnt++;
		p = pt;
	}
	for(int i = 0; i < part_cnt; i++)
	{
		if(!strcmp(parts[i], "../"))
		{
			delete[] parts[i];
			parts[i] = NULL;
			for(int j = i - 1; j > 0; j--)
			{
				if(parts[j])
				{
					delete[] parts[j];
					parts[j] = NULL;
					break;
				}
			}
		}
	}
	memset(path, 0, strlen(path));
	for(int i = 0; i < part_cnt; i++)
	{
		if(parts[i])
			{strcat(path, parts[i]);}
	}
	for(int i = 0; i < part_cnt; i++)
	{
		if(parts[i])
		{
			delete[] parts[i];
			parts[i] = NULL;
		}
	}
	delete[] parts;
	parts = NULL;
	return;
}