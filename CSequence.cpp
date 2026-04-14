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
	m_Events = NULL;
	m_EventCount = 0;
}

CSequence::CSequence(CSdl* sdl)
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
	m_Events = NULL;
	m_EventCount = 0;
	ParseFile();
}

CSequence::CSequence(CSequence* anm)
{
	m_sdl = NULL;
	m_Speed = anm->m_Speed;
	m_Smooth = anm->m_Smooth;
	m_Store = anm->m_Store;
	m_Resume = anm->m_Resume;
	m_Autostart = anm->m_Autostart;
	m_Name = m_FileName = NULL;
	if (anm->m_FileName)
	{
		m_FileName = new char[strlen(anm->m_FileName) + 1];
		strcpy(m_FileName, anm->m_FileName);
	}
	if (anm->m_Name)
	{
		m_Name = new char[strlen(anm->m_Name) + 1];
		strcpy(m_Name, anm->m_Name);
	}
	next = NULL;
	prev = NULL;
	m_AnimBone = anm->m_AnimBone;
	// Deep-copy event list
	m_EventCount = anm->m_EventCount;
	m_Events = NULL;
	if (m_EventCount > 0 && anm->m_Events)
	{
		m_Events = new AnimEvent[m_EventCount];
		for (int i = 0; i < m_EventCount; i++)
		{
			m_Events[i].frame = anm->m_Events[i].frame;
			m_Events[i].name = NULL;
			if (anm->m_Events[i].name)
			{
				m_Events[i].name = new char[strlen(anm->m_Events[i].name) + 1];
				strcpy(m_Events[i].name, anm->m_Events[i].name);
			}
		}
	}
}

CSequence::~CSequence()
{
	if (m_AnimBone)
	{
		delete m_AnimBone; m_AnimBone = NULL;
	}
	if (m_Name)
	{
		delete[] m_Name; m_Name = NULL;
	}
	if (m_FileName)
	{
		delete[] m_FileName; m_FileName = NULL;
	}
	if (m_Events)
	{
		for (int i = 0; i < m_EventCount; i++)
		{
			if (m_Events[i].name) delete[] m_Events[i].name;
		}
		delete[] m_Events;
		m_Events = NULL;
	}
	m_EventCount = 0;
}

void CSequence::ParseFile()
{
	char* word = m_sdl->GetNextWord();
	int block = 0;
	while (strlen(word))
	{
		if (!stricmp(word, "{"))// Beginning our processing block
		{
			block++;
		}
		else
			if (!stricmp(word, "}"))// Ending our processing block -- drop out here
			{
				break;
			}
			else
				if (word[0] == '"')
				{
					SetName(word);
				}
				else
					if (!stricmp("file", word))
					{
						SetFile();
					}
					else
						if (!stricmp("name", word))
						{
							SetName();
						}
						else
							if (!stricmp("resume", word))
							{
								SetResume();
							}
							else
								if (!stricmp("store", word))
								{
									SetStore();
								}
								else
									if (!stricmp("autostart", word))
									{
										SetAutoStart();
									}
									else
										if (!stricmp("smooth", word))
										{
											SetSmooth();
										}
										else
											if (!stricmp("speed", word))
											{
												SetSpeed();
											}
											else
												if (!stricmp("events", word))
												{
													SetEvents();
												}
												else
												{
#ifdef ALTERNATIVE_LANG
													msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl->m_line);
#else
													msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl->m_line);
#endif
													MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CAnimation::ParseFile", MB_ICONHAND);
												}
		word = m_sdl->GetNextWord();
	}
}

void CSequence::SetEvents()
{
	// Read the outer '{' of the events block
	char* word = m_sdl->GetNextWord();
	if (stricmp("{", word))
	{
		return;
	} // malformed - bail out silently

 // Count how many event entries exist first (look-ahead not possible,
 // so we collect into a temporary dynamic array)
	const int MAX_EVENTS = 64;
	AnimEvent tempEvents[MAX_EVENTS];
	int count = 0;

	// Free any previous events
	if (m_Events)
	{
		for (int i = 0; i < m_EventCount; i++)
		{
			if (m_Events[i].name) delete[] m_Events[i].name;
		}
		delete[] m_Events;
		m_Events = NULL;
		m_EventCount = 0;
	}

	word = m_sdl->GetNextWord();
	while (word && strlen(word) && stricmp("}", word) && count < MAX_EVENTS)
	{
		// Each entry is: { frameNum "eventName" }
		if (!stricmp("{", word))
		{
			// read frame number
			word = m_sdl->GetNextWord();
			tempEvents[count].frame = atoi(word);
			tempEvents[count].name = NULL;
			// read event name (may be quoted)
			word = m_sdl->GetNextWord();
			char* ename = word;
			if (ename[0] == '"') { ename++; }
			if (strlen(ename) > 0)
			{
				tempEvents[count].name = new char[strlen(ename) + 1];
				strcpy(tempEvents[count].name, ename);
			}
			count++;
			// read closing '}'
			word = m_sdl->GetNextWord();
			if (stricmp("}", word))
			{
				// unexpected token; continue anyway
				continue;
			}
		}
		word = m_sdl->GetNextWord();
	}
	// word is now '}' (outer close of events block), which is what we want

	if (count > 0)
	{
		m_Events = new AnimEvent[count];
		m_EventCount = count;
		for (int i = 0; i < count; i++)
		{
			m_Events[i] = tempEvents[i];
		}
	}
}
void CSequence::SetName(char* name)
{
	if (m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	char* tname = name;
	if (tname[0] == '"')
	{
		tname++;
	}
	m_Name = new char[strlen(tname) + 1];
	strcpy(m_Name, tname);
}

char* CSequence::GetName()
{
	return m_Name;
}

void CSequence::SetStore()
{
	char* word;
	m_Store = true;
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"STORE\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"STORE\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetStore", MB_OK);
	}
}

void CSequence::SetResume()
{
	char* word;
	m_Resume = true;
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"RESUME\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"RESUME\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetResume", MB_OK);
	}
}

void CSequence::SetAutoStart()
{
	char* word;
	m_Autostart = true;
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"AUTOSTART\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"AUTOSTART\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetAutoStart", MB_OK);
	}
}

void CSequence::SetFile()
{
	char* word = m_sdl->GetNextWord();
	if (m_FileName)
	{
		delete[] m_FileName;
		m_FileName = NULL;
	}
	m_FileName = new char[strlen(word) + 1];
	strcpy(m_FileName, word + 1);
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"FILE\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"FILE\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetFile", MB_OK);
	}
}

void CSequence::SetFile(char* name)
{
	if (m_FileName)
	{
		delete[] m_FileName;
		m_FileName = NULL;
	}
	char* tname = name;
	if (tname[0] == '"')
	{
		tname++;
	}
	m_FileName = new char[strlen(tname) + 1];
	strcpy(m_FileName, tname);
}

void CSequence::SetName()
{
	char* word = m_sdl->GetNextWord();
	if (m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	m_Name = new char[strlen(word) + 1];
	strcpy(m_Name, word);
}

void CSequence::SetSpeed()
{
	char* word;
	word = m_sdl->GetNextWord();
	m_Speed = (float)atof(word);
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"SPEED\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"SPEED\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetSpeed", MB_OK);
	}
}

void CSequence::SetSmooth()
{
	char* word;
	word = m_sdl->GetNextWord();
	m_Smooth = (float)atof(word);
	word = m_sdl->GetNextWord();
	if (stricmp("}", word))
	{
#ifdef ALTERNATIVE_LANG
		msg_st.Format("The block is damaged \"SMOOTH\". Incorrect end of the block.\r\nLine: %d", m_sdl->m_line);
#else
		msg_st.Format("Повреждён блок \"SMOOTH\". Неверный конец блока.\r\nСтрока: %d", m_sdl->m_line);
#endif
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, msg_st, "ERROR: CSequence::SetSmooth", MB_OK);
	}
}

CSequenceList::CSequenceList(CSdl* sdl)
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

CSequenceList::CSequenceList(CSequenceList* anm)
{
	m_sdl = NULL;
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
	CSequence* t_Sequence = anm->GetFirst();
	while (t_Sequence)
	{
		AddToTail(t_Sequence);
		t_Sequence = t_Sequence->next;
	}
}

CSequenceList::~CSequenceList()
{
	ClearList();
}

CSequence* CSequenceList::AddToHead()
{
	CSequence* t_Sequence = new CSequence(m_sdl);
	if (m_head != NULL)
	{
		t_Sequence->next = m_head;
		m_head->prev = t_Sequence;
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

CSequence* CSequenceList::AddToTail()
{
	CSequence* t_Sequence = new CSequence(m_sdl);
	if (m_tail != NULL)
	{
		t_Sequence->prev = m_tail;
		m_tail->next = t_Sequence;
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

CSequence* CSequenceList::AddToTail(CSequence* anm)
{
	CSequence* t_Sequence = new CSequence(anm);
	if (m_tail != NULL)
	{
		t_Sequence->prev = m_tail;
		m_tail->next = t_Sequence;
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

CSequence* CSequenceList::GetFirst()
{
	m_curr = m_head;
	return m_curr;
}

CSequence* CSequenceList::GetNext()
{
	CSequence* t_Sequence = NULL;
	if ((m_curr) && (m_curr->next))
	{
		m_curr = m_curr->next;
		t_Sequence = m_curr;
	}
	return t_Sequence;
}

CSequence* CSequenceList::FindByName(char* szName)
{
	CSequence* pSeq = GetFirst();
	while (pSeq)
	{
		if (pSeq->m_Name && !stricmp(szName, pSeq->m_Name))
		{
			return pSeq;
		}
		pSeq = pSeq->next;
	}
	return NULL;
}

void CSequenceList::DropSequence(char* szName)
{
	CSequence* pSeq = FindByName(szName);
	if (pSeq != NULL)
	{
		if (pSeq->prev)
		{
			pSeq->prev->next = pSeq->next;
		}
		if (pSeq->next)
		{
			pSeq->next->prev = pSeq->prev;
		}
		if (this->m_head == pSeq)
		{
			this->m_head = pSeq->next;
		}
		if (this->m_tail == pSeq)
		{
			this->m_tail = pSeq->prev;
		}
		if (this->m_curr == pSeq)
		{
			this->m_curr = pSeq->prev;
		}
		delete pSeq;
	}
}

CSequence* CSequenceList::GetPrev()
{
	CSequence* t_Sequence = NULL;
	if ((m_curr) && (m_curr->prev))
	{
		m_curr = m_curr->prev;
		t_Sequence = m_curr;
	}
	return t_Sequence;
}

CSequence* CSequenceList::GetCurr()
{
	return m_curr;
}

void CSequenceList::ClearList()
{
	CSequence* t_Sequence;
	while (m_tail)
	{
		t_Sequence = m_tail->prev;
		delete m_tail;
		m_tail = t_Sequence;
	}
	m_curr = NULL;
	m_head = NULL;
	m_tail = NULL;
}

CSequence* CSequenceList::GetLast()
{
	m_curr = m_tail;
	return m_curr;
}

void CSequenceList::WriteMdl(FILE* fp, int indent)
{
	CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
	CSOEditDoc* pDoc = (CSOEditDoc*)pWnd->GetActiveDocument();
	CSequence* pSeq = GetFirst();
	CString tmp_str;
	if (pDoc->Copy_Mode)
	{
		tmp_str.Format("%*s{Animation\n", (4 * indent++), ""); pDoc->Code_block += tmp_str;
	}
	else
	{
		fprintf(fp, "%*s{Animation\n", (4 * indent++), "");
	}
	while (pSeq)
	{
		if (pDoc->Copy_Mode)
		{
			tmp_str.Format("%*s{Sequence \"%s\"", (4 * indent), "", pSeq->GetName()); pDoc->Code_block += tmp_str;
		}
		else
		{
			fprintf(fp, "%*s{Sequence \"%s\"", (4 * indent), "", pSeq->GetName());
		}
		if (pSeq->m_FileName)
		{
			char* tfile = pSeq->m_FileName;
			if (tfile[0] == '"')
			{
				tfile++;
			}
			if (strlen(tfile))
			{
				if (pDoc->Copy_Mode)
				{
					tmp_str.Format(" {File \"%s\"}", tfile); pDoc->Code_block += tmp_str;
				}
				else
				{
					fprintf(fp, " {File \"%s\"}", tfile);
				}
			}
		}
		if ((pSeq->m_Speed != 0.0f) && (pSeq->m_Speed != 1.0f))
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {Speed %g}", pSeq->m_Speed); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {Speed %g}", pSeq->m_Speed);
			}
		}
		if ((pSeq->m_Smooth != 0.0f) && (pSeq->m_Smooth != 1.0f))
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {Smooth %g}", pSeq->m_Smooth); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {Smooth %g}", pSeq->m_Smooth);
			}
		}
		if (pSeq->m_Resume)
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {Resume}"); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {Resume}");
			}
		}
		if (pSeq->m_Autostart)
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {Autostart}"); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {Autostart}");
			}
		}
		if (pSeq->m_Store)
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {Store}"); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {Store}");
			}
		}
		// Write preserved anim events (e.g. {events {1 "open_left"}})
		if (pSeq->m_EventCount > 0 && pSeq->m_Events)
		{
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format(" {events"); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, " {events");
			}
			for (int ei = 0; ei < pSeq->m_EventCount; ei++)
			{
				if (pDoc->Copy_Mode)
				{
					tmp_str.Format(" {%d \"%s\"}", pSeq->m_Events[ei].frame, pSeq->m_Events[ei].name ? pSeq->m_Events[ei].name : ""); pDoc->Code_block += tmp_str;
				}
				else
				{
					fprintf(fp, " {%d \"%s\"}", pSeq->m_Events[ei].frame, pSeq->m_Events[ei].name ? pSeq->m_Events[ei].name : "");
				}
			}
			if (pDoc->Copy_Mode)
			{
				tmp_str.Format("}"); pDoc->Code_block += tmp_str;
			}
			else
			{
				fprintf(fp, "}");
			}
		}
		if (pDoc->Copy_Mode)
		{
			tmp_str.Format("}\n"); pDoc->Code_block += tmp_str;
		}
		else
		{
			fprintf(fp, "}\n");
		}
		pSeq = pSeq->next;
	}
	indent--;
	if (pDoc->Copy_Mode)
	{
		tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc->Code_block += tmp_str;
	}
	else
	{
		fprintf(fp, "%*s}\n", (4 * indent), "");
	}
}