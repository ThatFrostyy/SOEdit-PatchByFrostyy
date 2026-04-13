// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CTexFile.h"

class CMaterial
{
    public:
	CMaterial(char *szName, int iTexNumb);
	~CMaterial();

	char *m_szName;
	int   m_iTexNumb;
	bool  m_transparency;

	CMaterial *m_next;
	CMaterial *m_prev;
	CTexFile  *m_TexFile;
};

class CMtlList
{
    public:
       CMtlList();
       ~CMtlList();

       CMaterial *FindMaterial(char *szName);

       CMaterial *GoToHead();
       CMaterial *GoToTail();
       CMaterial *AddToHead(char *szName, int iTexNumb);
       CMaterial *AddToTail(char *szName, int iTexNumb);
       CMaterial *GetNext();
       CMaterial *GetPrev();

       bool       DropItem(CMaterial *pItem);

       void       ClearList();

       CMaterial *InsertBefore(char *szName, int iTexNumb);
       CMaterial *InsertAfter(char *szName, int iTexNumb);

    private:
       CMaterial *m_curr;
       CMaterial *m_head;
       CMaterial *m_tail;
};