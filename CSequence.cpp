// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"

extern HWND m_hWnd;

#include "CSequence.h"
#include "MainFrm.h"
#include "CSdl.h"

CSequence::CSequence()
{
    m_sdl = NULL;
    m_Speed = 1.0f;
	m_Smooth = 0.0f;
	m_Store = false;
	m_Resume = false;
    m_Autostart = false;
    m_FileName = NULL;
    m_Name = NULL;
    next = NULL;
    prev = NULL;
	m_AnimBone = NULL;
	m_EventsBlock = "";
	m_UnknownBlocks = "";
}

CSequence::CSequence(CSdl *sdl)
{
    m_sdl = sdl;
    m_Speed = 1.0f;
	m_Smooth = 0.0f;
	m_Store = false;
	m_Resume = false;
    m_Autostart = false;
    m_FileName = NULL;
    m_Name = NULL;
    next = NULL;
    prev = NULL;
	m_AnimBone = NULL;
	m_EventsBlock = "";
	m_UnknownBlocks = "";
    ParseFile();
}

CSequence::CSequence(CSequence *anm)
{
    m_sdl = NULL;
    m_Speed = anm -> m_Speed;
    m_Smooth = anm -> m_Smooth;
	m_Store = anm -> m_Store;
	m_Resume = anm -> m_Resume;
	m_Autostart = anm -> m_Autostart;
	m_Name = m_FileName = NULL;
	if(anm -> m_FileName)
	{
		m_FileName = new char[strlen(anm -> m_FileName) + 1];
		strcpy(m_FileName, anm -> m_FileName);
	}
	if(anm -> m_Name)
	{
		m_Name = new char[strlen(anm -> m_Name) + 1];
		strcpy(m_Name, anm -> m_Name);
	}
	next = NULL;
	prev = NULL;
	m_AnimBone = anm -> m_AnimBone;
	m_EventsBlock = anm -> m_EventsBlock;
	m_UnknownBlocks = anm -> m_UnknownBlocks;
}

CSequence::~CSequence()
{
	if(m_AnimBone)
		{delete m_AnimBone; m_AnimBone = NULL;}
	if(m_Name)
		{delete[] m_Name; m_Name = NULL;}
	if(m_FileName)
		{delete[] m_FileName; m_FileName = NULL;}
}

void CSequence::ParseFile()
{
	char *word = m_sdl -> GetNextWord();
	int block = 0;
	while(strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{block++;}
		else
		if(!stricmp(word, "}"))// Ending our processing block -- drop out here
			{break;}
		else
		if(word[0] == '"')
			{SetName(word);}
		else
		if(!stricmp("file", word))
			{SetFile();}
		else
		if(!stricmp("name", word))
			{SetName();}
		else
		if(!stricmp("resume", word))
			{SetResume();}
		else
		if(!stricmp("store", word))
			{SetStore();}
		else
		if(!stricmp("autostart", word))
			{SetAutoStart();}
		else
		if(!stricmp("smooth", word))
			{SetSmooth();}
		else
		if(!stricmp("speed", word))
			{SetSpeed();}
		else
		if(!stricmp("events", word))
			{SetEvents();}
		else
			{SetUnknownBlock(word);}
		word = m_sdl -> GetNextWord();
	}
}

void CSequence::SetEvents()
{
	char *word = m_sdl -> GetNextWord();
	if(stricmp("{", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"EVENTS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("  \"EVENTS\".   .\r\n: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetEvents", MB_ICONHAND);
		return;
	}

	int block = 1;
	m_EventsBlock = "{Events";
	while(block > 0)
	{
		word = m_sdl -> GetNextWord();
		if(!strlen(word))
			{break;}
		if(!stricmp("{", word))
		{
			block++;
			m_EventsBlock += " {";
		}
		else
		if(!stricmp("}", word))
		{
			block--;
			m_EventsBlock += "}";
		}
		else
		{
			m_EventsBlock += " ";
			m_EventsBlock += word;
		}
	}
}

void CSequence::SetUnknownBlock(char *word)
{
	CString block;
	block.Format("{%s", word);
	int depth = 1;
	char *next_word = m_sdl -> GetNextWord();
	while(strlen(next_word))
	{
		if(!stricmp("{", next_word))
		{
			depth++;
			block += " {";
		}
		else
		if(!stricmp("}", next_word))
		{
			depth--;
			block += "}";
			if(depth <= 0)
				{break;}
		}
		else
		{
			block += " ";
			block += next_word;
		}
		next_word = m_sdl -> GetNextWord();
	}
	if(!m_UnknownBlocks.IsEmpty())
		{m_UnknownBlocks += " ";}
	m_UnknownBlocks += block;
}

void CSequence::SetName(char *name)
{
	if(m_Name)
	{
        delete[] m_Name;
        m_Name = NULL;
	}
	char *tname = name;
	if(tname[0] == '"')
		{tname++;}
    m_Name = new char[strlen(tname) + 1];
    strcpy(m_Name, tname);
}

char *CSequence::GetName()
	{return m_Name;}

void CSequence::SetStore()
{
	char *word;
	m_Store = true;
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"STORE\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
    m_Autostart = true;
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetStore", MB_OK);
	}
}

void CSequence::SetResume()
{
	char *word;
	m_Resume = true;
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"RESUME\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Ïîâðåæä¸í áëîê \"RESUME\". Íåâåðíûé êîíåö áëîêà.\r\nÑòðîêà: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetResume", MB_OK);
	}
}

void CSequence::SetAutoStart()
{
    char *word;
    m_Resume = true;
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"AUTOSTART\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Ïîâðåæä¸í áëîê \"AUTOSTART\". Íåâåðíûé êîíåö áëîêà.\r\nÑòðîêà: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetAutoStart", MB_OK);
	}
}

void CSequence::SetFile()
{
    char *word = m_sdl -> GetNextWord();
    if(m_FileName)
	{
		delete[] m_FileName;
		m_FileName = NULL;
	}
	m_FileName = new char[strlen(word) + 1];
	strcpy(m_FileName, word + 1);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"FILE\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Ïîâðåæä¸í áëîê \"FILE\". Íåâåðíûé êîíåö áëîêà.\r\nÑòðîêà: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetFile", MB_OK);
	}
}

void CSequence::SetFile(char *name)
{
	if(m_FileName)
	{
        delete[] m_FileName;
		m_FileName = NULL;
	}
	char *tname = name;
	if(tname[0] == '"')
		{tname++;}
	m_FileName = new char[strlen(tname) + 1];
    strcpy(m_FileName, tname);
}

void CSequence::SetName()
{
    char *word = m_sdl -> GetNextWord();
    if(m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	m_Name = new char[strlen(word)+1];
	strcpy(m_Name, word);
}

void CSequence::SetSpeed()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_Speed = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"SPEED\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Ïîâðåæä¸í áëîê \"SPEED\". Íåâåðíûé êîíåö áëîêà.\r\nÑòðîêà: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetSpeed", MB_OK);
	}
}

void CSequence::SetSmooth()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_Smooth = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"SMOOTH\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Ïîâðåæä¸í áëîê \"SMOOTH\". Íåâåðíûé êîíåö áëîêà.\r\nÑòðîêà: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CSequence::SetSmooth", MB_OK);
	}
}

CSequenceList::CSequenceList(CSdl *sdl)
{
    m_sdl = sdl;
    m_head = NULL;
    m_tail = NULL;
    m_curr = NULL;
}
CSequenceList::CSequenceList()
{
	m_sdl = NULL;
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
}

CSequenceList::CSequenceList(CSequenceList *anm)
{
	m_sdl = NULL;
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
	CSequence *t_Sequence = anm -> GetFirst();
	while(t_Sequence)
	{
		AddToTail(t_Sequence);
		t_Sequence = t_Sequence -> next;
	}
}

CSequenceList::~CSequenceList()
	{ClearList();}

CSequence *CSequenceList::AddToHead()
{
    CSequence *t_Sequence = new CSequence(m_sdl);
    if(m_head != NULL)
	{
        t_Sequence -> next = m_head;
        m_head -> prev = t_Sequence;
        m_head = t_Sequence;
	}
    else
	{
        m_head = t_Sequence;
        m_tail = t_Sequence;
        m_curr = t_Sequence;
	}
    return t_Sequence;
}

CSequence *CSequenceList::AddToTail()
{
    CSequence *t_Sequence = new CSequence(m_sdl);
    if(m_tail != NULL)
	{
        t_Sequence -> prev = m_tail;
        m_tail -> next = t_Sequence;
        m_tail = t_Sequence;
	}
    else
	{
        m_head = t_Sequence;
        m_tail = t_Sequence;
        m_curr = t_Sequence;
	}
    return t_Sequence;
}

CSequence *CSequenceList::AddToTail(CSequence *anm)
{
    CSequence *t_Sequence = new CSequence(anm);
    if(m_tail != NULL)
	{
        t_Sequence -> prev = m_tail;
        m_tail -> next = t_Sequence;
        m_tail = t_Sequence;
	}
    else
	{
        m_head = t_Sequence;
        m_tail = t_Sequence;
        m_curr = t_Sequence;
	}
    return t_Sequence;
}

CSequence *CSequenceList::GetFirst()
{
    m_curr = m_head;
    return m_curr;
}

CSequence *CSequenceList::GetNext()
{
    CSequence *t_Sequence = NULL;
    if((m_curr) && (m_curr -> next))
	{
        m_curr = m_curr -> next;
        t_Sequence = m_curr;
	}
    return t_Sequence;
}

CSequence *CSequenceList::FindByName(char *szName)
{
	CSequence *pSeq = GetFirst();
    while(pSeq)
	{
        if(pSeq -> m_Name && !stricmp(szName, pSeq -> m_Name))
			{return pSeq;}
		pSeq = pSeq -> next;
	}
    return NULL;
}

void CSequenceList::DropSequence(char *szName)
{
	CSequence *pSeq = FindByName(szName);
	if(pSeq != NULL)
	{
		if(pSeq -> prev)
			{pSeq -> prev -> next = pSeq -> next;}
		if(pSeq -> next)
			{pSeq -> next -> prev = pSeq -> prev;}
		if(this -> m_head == pSeq)
			{this -> m_head = pSeq -> next;}
		if(this -> m_tail == pSeq)
			{this -> m_tail = pSeq -> prev;}
		if(this -> m_curr == pSeq)
			{this -> m_curr = pSeq -> prev;}
		delete pSeq;
	}
}

CSequence *CSequenceList::GetPrev()
{
    CSequence *t_Sequence = NULL;
    if((m_curr) && (m_curr -> prev))
	{
        m_curr = m_curr -> prev;
        t_Sequence = m_curr;
	}
    return t_Sequence;
}

CSequence *CSequenceList::GetCurr()
	{return m_curr;}

void CSequenceList::ClearList()
{
    CSequence *t_Sequence;
    while(m_tail)
	{
        t_Sequence = m_tail -> prev;
        delete m_tail;
        m_tail = t_Sequence;
	}
    m_curr = NULL;
    m_head = NULL;
    m_tail = NULL;
}

CSequence *CSequenceList::GetLast()
{
    m_curr = m_tail;
    return m_curr;
}

void CSequenceList::WriteMdl(FILE *fp, int indent)
{
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd->GetActiveDocument();
	CSequence *pSeq = GetFirst();
	CString tmp_str;
	if(pDoc -> Copy_Mode)
		{tmp_str.Format("%*s{Animation\n", (4 * indent++), ""); pDoc -> Code_block += tmp_str;}
	else
		{fprintf(fp, "%*s{Animation\n", (4 * indent++), "");}
	while(pSeq)
	{
		if(pDoc -> Copy_Mode)
			{tmp_str.Format("%*s{Sequence \"%s\"", (4 * indent), "", pSeq -> GetName()); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, "%*s{Sequence \"%s\"", (4 * indent), "", pSeq -> GetName());}
		if(pSeq -> m_FileName)
		{
			char *tfile = pSeq -> m_FileName;
			if(tfile[0] == '"')
				{tfile++;}
			if(strlen(tfile))
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {File \"%s\"}", tfile); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {File \"%s\"}", tfile);}
			}
		}
		if((pSeq -> m_Speed != 0.0f) && (pSeq -> m_Speed != 1.0f))
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" {Speed %.6g}", pSeq -> m_Speed); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " {Speed %.6g}", pSeq -> m_Speed);}
		}
		if((pSeq -> m_Smooth != 0.0f) && (pSeq -> m_Smooth != 1.0f))
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" {Smooth %.6g}", pSeq -> m_Smooth); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " {Smooth %.6g}", pSeq -> m_Smooth);}
		}
		if(pSeq -> m_Resume)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" {Resume}"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " {Resume}");}
		}
		if(pSeq -> m_Autostart)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" {Autostart}"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " {Autostart}");}
		}
		if(pSeq -> m_Store)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" {Store}"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " {Store}");}
		}
		if(!pSeq -> m_EventsBlock.IsEmpty())
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" %s", pSeq -> m_EventsBlock); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " %s", (LPCSTR)pSeq -> m_EventsBlock);}
		}
		if(!pSeq -> m_UnknownBlocks.IsEmpty())
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" %s", pSeq -> m_UnknownBlocks); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " %s", (LPCSTR)pSeq -> m_UnknownBlocks);}
		}
		if(pDoc -> Copy_Mode)
			{tmp_str.Format("}\n"); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, "}\n");}
		pSeq = pSeq -> next;
	}
	indent--;
	if(pDoc -> Copy_Mode)
		{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
	else
		{fprintf(fp, "%*s}\n", (4 * indent), "");}
}