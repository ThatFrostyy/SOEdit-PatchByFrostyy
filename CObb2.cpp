// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CObb2.h"
#include "CSdl.h"
#include "general.h"

#include "math.h"

CObb2::CObb2(char *type)
{
	m_sdl = NULL;
	InitObject(type);
}

CObb2::CObb2(char *type, CSdl *sdl)
{
	m_sdl = sdl;
	m_Level = 0;
	InitObject(type);
	ParseFile();
}

void CObb2::InitObject(char *type)
{
	m_Type = new char[strlen(type) + 1];
	strcpy(m_Type, type);
	m_GType = OT_UNK;
	m_Vertices = 0;
	m_Vertex = NULL;
	m_Radius = NULL;
	m_Name = NULL;
	m_Center = NULL;
	m_Extent = NULL;
	m_Axis = NULL;
	m_Tags = NULL;
	next = NULL;
	prev = NULL;
	m_Rotate = false;
	for(int i = 0; i < 3; i++)
	{
		m_Rotations[i] = 0.0f;
		m_Translations[i] = 0.0f;
		m_Scales[i] = 1.0f;
	}
}

CObb2::~CObb2()
{
	if(m_Type)
		{delete[] m_Type;}
	m_Type = NULL;
	if(m_Name)
		{delete[] m_Name;}
	m_Name = NULL;
	if(m_Center)
		{delete[] m_Center;}
	m_Center = NULL;
	if(m_Extent)
		{delete[] m_Extent;}
	m_Extent = NULL;
	if(m_Axis)
		{delete[] m_Axis;}
	m_Axis = NULL;
	if(m_Radius)
		{delete m_Radius;}
	m_Radius = NULL;
	if(m_Vertex)
		{delete[] m_Vertex;}
	m_Vertex = NULL;
	if(m_Tags)
		{delete[] m_Tags;}
	m_Tags = NULL;
}

void CObb2::ParseFile()
{
	char *word = m_sdl->GetNextWord();
	int level = 0;
	while (strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{}
		else
		if(!stricmp(word, "}"))// Ending our processing block -- drop out here
			{break;}
		else
		if(word[0] == '"')
			{SetName(word);}
		else
		if(!stricmp("obb2", word))
			{SetObb2();}
		else
		if(!stricmp("circle2", word))
			{SetCircle2();}
		else
		if(!stricmp("polygon2", word))
			{SetPolygon2();}
		else
		if(!stricmp("center", word))
			{SetCenter();}
		else
		if(!stricmp("radius", word))
			{SetRadius();}
		else
		if(!stricmp("vertex", word))
			{SetVertex();}
		else
		if(!stricmp("extent", word))
			{SetExtent();}
		else
		if(!stricmp("axis", word))
			{SetAxis();}
		else
		if(!stricmp("rotate", word))
			{SetRotate();}
		else
		if(!stricmp("tags", word))
			{SetTags();}
		else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

void CObb2::SetObb2()
{
	m_GType = OT_RECT;
	ParseFile();
}

void CObb2::SetCircle2()
{
	m_GType = OT_CIRCLE;
	ParseFile();
}

void CObb2::SetPolygon2()
{
	m_GType = OT_POLYGON;
	ParseFile();
}

void CObb2::SetVertex()
{
	char *word;
	v2_t *m_temp = NULL;
	if(m_Vertices)
		{m_temp = m_Vertex;}
	m_Vertex = new v2_t[m_Vertices + 1];
	if(m_Vertices)
	{
		for(int i = 0; i < m_Vertices; i++)
		{
			m_Vertex[i][0] = m_temp[i][0];
			m_Vertex[i][1] = m_temp[i][1];
		}
		delete[] m_temp;
	}
	m_Vertex[m_Vertices][0] = 0.0f;
	m_Vertex[m_Vertices][1] = 0.0f;
	word = m_sdl -> GetNextWord();
	m_Vertex[m_Vertices][0] = (float)atof(word);
	word = m_sdl -> GetNextWord();
	m_Vertex[m_Vertices][1] = (float)atof(word);
	m_Vertices++;
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"VERTEX\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"VERTEX\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetVertex", MB_ICONHAND);
	}
}

void CObb2::SetName(char *name)
{
	if(m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	m_Name = new char[strlen(name) + 1];
	strcpy(m_Name, name);
}

void CObb2::SetRadius()
{
	char *word;
	m_Radius = new float;
	word = m_sdl -> GetNextWord();
	*m_Radius = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"RADIUS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"RADIUS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetRadius", MB_ICONHAND);
	}
}

void CObb2::SetCenter()
{
	char *word;
	m_Center = new float[2];
	word = m_sdl -> GetNextWord();
	m_Center[0] = (float)atof(word);
	word = m_sdl -> GetNextWord();
	m_Center[1] = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"CENTER\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"CENTER\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetCenter", MB_ICONHAND);
	}
}

void CObb2::SetExtent()
{
	char *word;
	m_Extent = new float[2];
	word = m_sdl -> GetNextWord();
	m_Extent[0] = (float)atof(word);
	word = m_sdl -> GetNextWord();
	m_Extent[1] = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"EXTENT\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"EXTENT\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetExtent", MB_ICONHAND);
	}
}

void CObb2::SetRotate()
{
	char *word;
	m_Rotate = true;
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"ROTATE\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"ROTATE\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetRotate", MB_ICONHAND);
	}
}

void CObb2::SetAxis()
{
	char *word;
	m_Axis = new float[2];
	memset(m_Axis, 0, sizeof(float) * 2);
	word = m_sdl -> GetNextWord();
	m_Axis[0] = (float)atof(word);
	if((m_Axis[0] > -0.0005f) && (m_Axis[0] < 0.0005f))
		{m_Axis[0] = 0.0f;}
	word = m_sdl -> GetNextWord();
	m_Axis[1] = (float)atof(word);
	if((m_Axis[1] > -0.0005f) && (m_Axis[1] < 0.0005f))
		{m_Axis[1] = 0.0f;}
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"AXIS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"AXIS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CObb2::SetAxis", MB_ICONHAND);
	}
}

void CObb2::SetTags()
{
    char *word = m_sdl -> GetNextWord();
	char *t_Tags = NULL;
	int s = 0, d = 0;
    for(;s < (int)strlen(word); s++)
	{
		if(word[s] != '"')
		{
            word[d] = word[s];
            d++;
		}
	}
	word[d] = '\0';
	if(m_Tags)
	{
		t_Tags = new char[strlen(m_Tags) + strlen(word) + 2];
		sprintf(t_Tags, "%s %s", m_Tags, word);
		delete[] m_Tags;
		m_Tags = NULL;
		m_Tags = t_Tags;
	}
	else
	{
		m_Tags = new char[strlen(word) + 1];
		strcpy(m_Tags, word);
	}
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"TAGS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"TAGS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetTags", MB_ICONHAND);
	}
}

void CObb2::WriteMdl(FILE *fp, int indent)
{
	char *tname = this -> m_Name;
	if(tname[0] == '"')
		{tname++;}
	fprintf(fp, "%*s{%s \"%s\"\n", (4 * indent++), "", m_Type, tname);
	char *ttype = NULL;
	switch(m_GType)
	{
		case OT_RECT:
			{ttype = "Obb2";}
		break;
		case OT_CIRCLE:
			{ttype = "Circle2";}
		break;
		case OT_POLYGON:
			{ttype = "Polygon2";}
		break;
		default:
			{ttype = "unknown";}
		break;
	}
	fprintf(fp, "%*s{%s\n", (4 * indent++), "", ttype);
	if(m_Radius)
		{fprintf(fp, "%*s{Radius %g}\n", (4 * indent), "", *m_Radius);}
	if(m_Vertices)
	{
		double deg2rad = 1.0 / (180.0 / 3.1415927);
		double angle = (double)m_Rotations[2] * deg2rad;
		for(int v = 0; v < m_Vertices; v++)
		{
			m_Vertex[v][0] = (float)((m_Vertex[v][0] * cos(angle)) - (m_Vertex[v][1] * sin(angle)));
			m_Vertex[v][1] = (float)((m_Vertex[v][1] * cos(angle)) + (m_Vertex[v][0] * sin(angle)));
			fprintf(fp, "%*s{Vertex %g %g}\n", (4 * indent), "", m_Vertex[v][0], m_Vertex[v][1]);
		}
	}
	if(m_Center)
		{fprintf(fp, "%*s{Center %g %g}\n", (4 * indent), "", m_Center[0], m_Center[1]);}
	if(m_Extent)
		{fprintf(fp, "%*s{Extent %g %g}\n", (4 * indent), "", m_Extent[0], m_Extent[1]);}
	if(m_Axis)
	{
		double deg2rad = 1.0 / (180.0 / 3.1415927);
		double angle = (double)m_Rotations[2] * deg2rad;
		m_Axis[0] = (float)((m_Axis[0] * cos(angle)) - (m_Axis[1] * sin(angle)));
		m_Axis[1] = (float)((m_Axis[1] * cos(angle)) + (m_Axis[0] * sin(angle)));
		fprintf(fp, "%*s{Axis %g %g}\n", (4 * indent), "", m_Axis[0], m_Axis[1]);
	}
	indent--;
	fprintf(fp, "%*s}\n", (4 * indent), "");
	if(m_Tags)
	{
		char *ttagd = m_Tags;
		if(ttagd[0] == '"')
			{ttagd++;}
		fprintf(fp, "%*s{Tags \"%s\"}\n", (4 * indent), "", ttagd);
	}
	if(m_Rotate)
		{fprintf(fp, "%*s{Rotate}\n", (4 * indent), "");}
	indent--;
	fprintf(fp, "%*s}\n", (4 * indent), "");
}

CObb2List::CObb2List(char *type)
{
	m_sdl = NULL;
	InitObject(type);
}

CObb2List::CObb2List(char *type, CSdl *sdl)
{
	m_sdl = sdl;
	InitObject(type);
}

void CObb2List::InitObject(char *type)
{
	m_Type = new char[strlen(type) + 1];
	strcpy(m_Type, type);
	m_head = NULL;
	m_tail = NULL;
	m_curr = NULL;
}

CObb2List::~CObb2List()
{
	ClearList();
	if(m_Type)
	{
		delete[] m_Type;
		m_Type = NULL;
	}
}

CObb2 *CObb2List::AddToHead()
{
	CObb2 *t_CObb2;
	if(!m_sdl)
		{t_CObb2 = new CObb2(m_Type);}
	else
		{t_CObb2 = new CObb2(m_Type, m_sdl);}
	if(m_head)
	{
		t_CObb2 -> next = m_head;
		m_head -> prev = t_CObb2;
		m_head = t_CObb2;
	}
	else
	{
		m_head = t_CObb2;
		m_tail = t_CObb2;
		m_curr = t_CObb2;
	}
	return t_CObb2;
}

CObb2 *CObb2List::AddToTail()
{
	CObb2 *t_CObb2;
	if(!m_sdl)
		{t_CObb2 = new CObb2(m_Type);}
	else
		{t_CObb2 = new CObb2(m_Type, m_sdl);}
	if(m_tail)
	{
		t_CObb2 -> prev = m_tail;
		m_tail -> next = t_CObb2;
		m_tail = t_CObb2;
	}
	else
	{
		m_head = t_CObb2;
		m_tail = t_CObb2;
		m_curr = t_CObb2;
	}
	return t_CObb2;
}

CObb2 *CObb2List::GetFirst()
{
	m_curr = m_head;
	return m_curr;
}

CObb2 *CObb2List::GetNext()
{
	CObb2 *t_CObb2 = NULL;
	if(m_curr && m_curr -> next)
	{
		m_curr = m_curr -> next;
		t_CObb2 = m_curr;
	}
	return t_CObb2;
}

CObb2 *CObb2List::GetPrev()
{
	CObb2 *t_CObb2 = NULL;
	if(m_curr && m_curr -> prev)
	{
		m_curr = m_curr -> prev;
		t_CObb2 = m_curr;
	}
	return t_CObb2;
}

CObb2 *CObb2List::GetCurr()
	{return m_curr;}

void CObb2List::ClearList()
{
	CObb2 *t_CObb2;
	while(m_tail)
	{
		t_CObb2 = m_tail -> prev;
		delete m_tail;
		m_tail = t_CObb2;
	}
	m_curr = NULL;
	m_head = NULL;
	m_tail = NULL;
}

CObb2 *CObb2List::GetLast()
{
	m_curr = m_tail;
	return m_curr;
}

CObb2 *CObb2List::FindByName(char *szName)
{
	CObb2 *pCObb2 = GetFirst();
	while(pCObb2)
	{
		if(!stricmp(szName, pCObb2 -> m_Name))
			{return pCObb2;}
		pCObb2 = pCObb2 -> next;
	}
	return NULL;
}

void CObb2List::WriteMdl(FILE *fp, int indent)
{
	CObb2 *pCObb2 = GetFirst();
	while(pCObb2)
	{
		pCObb2 -> WriteMdl(fp, indent);
		pCObb2 = pCObb2 -> next;
	}
}
