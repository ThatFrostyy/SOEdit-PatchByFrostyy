// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CVolume.h"
#include "MainFrm.h"
#include "io.h"

extern HWND m_hWnd;

CVolume::CVolume()
{
    m_sdl = NULL;
    InitObject();
}

CVolume::CVolume(CSdl *sdl)
{
    m_sdl = sdl;
    InitObject();
	ParseFile();
    if(m_Cylinder)
	{
        m_Cyl = new CCylinder(m_Cylinder[0], m_Cylinder[1]);
        int i = 0;
	}
}

void CVolume::InitObject()
{
    m_Name = NULL;
    m_Bone = NULL;
    m_Component = NULL;
	m_Tags = NULL;
    m_PolyhedronName = NULL;
    next = NULL;
    prev = NULL;
    m_Type = VT_UNKNOWN;
    m_Matrix34 = NULL;
    m_Position = NULL;
    m_Cylinder = NULL;
    m_Cyl = NULL;
    m_Box = NULL;
    m_Vol = NULL;
	boneptr = NULL;
	vol_thickness = NULL;
	m_Density = NULL;
	hTreeItem = NULL;
	m_tree_check = true;
    for(int i = 0; i < 3; i++)
	{
        m_Rotations[i] = 0.0f;
        m_Translations[i] = 0.0f;
        m_Scales[i] = 1.0f;
	}
}

CVolume::~CVolume()
{
	if(m_PolyhedronName)
		{delete[] m_PolyhedronName; m_PolyhedronName = NULL;}
	if(m_Bone)
		{delete[] m_Bone; m_Bone = NULL;}
	if(m_Component)
		{delete[] m_Component; m_Component = NULL;}
	if(m_Name)
		{delete[] m_Name; m_Name = NULL;}
	if(m_Matrix34)
		{delete m_Matrix34; m_Matrix34 = NULL;}
	if(m_Position)
		{delete[] m_Position; m_Position = NULL;}
	if(m_Box)
		{delete[] m_Box; m_Box = NULL;}
	if(m_Cylinder)
		{delete[] m_Cylinder; m_Cylinder = NULL;}
	if(m_Cyl)
		{delete m_Cyl; m_Cyl = NULL;}
	if(m_Vol)
		{delete m_Vol; m_Vol = NULL;}
	if(vol_thickness)
		{delete vol_thickness; vol_thickness = NULL;}
	if(m_Tags)
		{delete m_Tags; m_Tags = NULL;}
}

void CVolume::ParseFile()
{
    char *word = m_sdl -> GetNextWord();
    while(strlen(word))
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
        if(!stricmp("box", word))
			{SetBox();}
        else
        if(!stricmp("Sphere", word))
			{SetSphere();}
        else
        if(!stricmp("cylinder", word))
			{SetCylinder();}
        else
        if(!stricmp("polyhedron", word))
			{SetPolyhedron();}
        else
        if(!stricmp("position", word))
			{SetPosition();}
        else
        if(!stricmp("matrix34", word))
			{SetMatrix34();}
        else
        if(!stricmp("bone", word))
			{SetBone();}
        else
        if(!stricmp("component", word))
			{SetComponent();}
		else
        if(!stricmp("tags", word))
			{SetTags();}
		else
        if(!stricmp("density", word))
			{SetDensity();}
		else
        if(!stricmp("orientation", word))
			{SetOrientation();}
		else
        if(!stricmp("thickness", word))
			{SetThickness();}
		else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CAnimation::ParseFile", MB_ICONHAND);
		}
        word = m_sdl -> GetNextWord();
	}
}

void CVolume::SetName(char *name)
{
	int len = strlen(name) + 1;
	if(m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	m_Name = new char[len];
	memset(m_Name, 0, len);
	strcpy(m_Name, name);
}

void CVolume::SetDensity()
{
	char *word = m_sdl -> GetNextWord();
	if(m_Density)
	{
        delete m_Density;
		m_Density = NULL;
	}
	m_Density = new float[1];
	m_Density[0] = (float)atof(word);
	word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"DENSITY\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"DENSITY\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetDensity", MB_ICONHAND);
	}
}

void CVolume::SetBone()
{
    char *word = m_sdl -> GetNextWord();
    if(m_Bone)
	{
        delete[] m_Bone;
        m_Bone = NULL;
	}
	int d = 0, s = 0;
    for(;s < (int)strlen(word); s++)
	{
		if(word[s] != '"')
		{
			word[d] = word[s];
			d++;
		}
	}
    word[d] = '\0';
    m_Bone = new char[strlen(word) + 1];
    strcpy(m_Bone, word);
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"BONE\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"BONE\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetBone", MB_ICONHAND);
	}
}

void CVolume::SetComponent()
{
    char *word = m_sdl -> GetNextWord();
    if(m_Component)
	{
        delete[] m_Component;
        m_Component = NULL;
	}
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
    m_Component = new char[strlen(word) + 1];
    strcpy(m_Component, word);
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"COMPONENT\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"COMPONENT\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetComponent", MB_ICONHAND);
	}
}

void CVolume::SetTags()
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

void CVolume::SetBox()
{
    int j;
    char *word;
	if(m_Box)
	{
        delete m_Box;
		m_Box = NULL;
	}
    m_Box = new v3_t;
    for(j = 0; j < 3; j++)
	{
        word = m_sdl -> GetNextWord();
        m_Box[j] = (float)atof(word);
	}
    m_Type = VT_BOX;
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"BOX\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"BOX\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetBox", MB_ICONHAND);
	}
}

void CVolume::SetSphere()
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_Type = VT_BOX;
	if(m_Box)
	{
        delete m_Box;
		m_Box = NULL;
	}
	m_Box = new v3_t;
	m_Box[0] = m_Box[1] = m_Box[2] = (float)atof(word);//потому что гладиолус
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"SPHERE\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"SPHERE\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetSphere", MB_ICONHAND);
	}
}

void CVolume::SetCylinder()
{
    char *word;
	if(m_Cylinder)
	{
        delete m_Cylinder;
		m_Cylinder = NULL;
	}
    m_Cylinder = new v2_t;
    word = m_sdl -> GetNextWord();
    m_Cylinder[0] = (float)atof(word);
    word = m_sdl -> GetNextWord();
    m_Cylinder[1] = (float)atof(word);
    m_Type = VT_CYLINDER;
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"CYLINDER\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"CYLINDER\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetCylinder", MB_ICONHAND);
	}
}

void CVolume::SetMatrix34()
{
	int i, j;
	char *word;
	if(m_Matrix34)
		{delete m_Matrix34;}
	m_Matrix34 = new matrix34_t;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 3; j++)
		{
			word = m_sdl -> GetNextWord();
			m_Matrix34 -> v[i][j] = (float)atof(word);
		}
	}
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"MATRIX34\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"MATRIX34\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetMatrix34", MB_ICONHAND);
	}
}

void CVolume::SetPosition()
{
    int j;
    char *word;
	if(!m_Matrix34)
	{
		m_Matrix34 = new matrix34_t;
		memset(m_Matrix34, 0, sizeof(matrix34_t));
		m_Matrix34 -> v[0][0] = 1.0f;
		m_Matrix34 -> v[1][1] = 1.0f;
		m_Matrix34 -> v[2][2] = 1.0f;
	}
	for(j = 0; j < 3; j++)
	{
		word = m_sdl -> GetNextWord();
		m_Matrix34-> v[3][j] = (float)atof(word);
	}
    word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"POSITION\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"POSITION\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetPosition", MB_ICONHAND);
	}
}

void CVolume::SetThickness()
{
	char *word = NULL;
	if(vol_thickness)
	{
        delete vol_thickness;
		vol_thickness = NULL;
	}
	vol_thickness = new thickness_t;
	memset(vol_thickness, 0, sizeof(thickness_t));
	word = m_sdl -> GetNextWord();
	vol_thickness -> GT.v[0] = (float)atof(word);
	while(true)
	{
		word = m_sdl -> GetNextWord();
		if(!stricmp("}", word))
			{break;}
		if(!stricmp("{", word))
			{SetThicknessSide();}
		else
		{
			vol_thickness -> GT.l = TRUE;
			vol_thickness -> GT.v[1] = (float)atof(word);
		}
	}
}

void CVolume::SetThicknessSide()
{
	char *word = m_sdl -> GetNextWord();
	thickness_sides_t side = t_none;
	if(!stricmp("top", word))
		{side = t_top;}
	else
	if(!stricmp("front", word))
		{side = t_front;}
	else
	if(!stricmp("left", word))
		{side = t_left;}
	else
	if(!stricmp("right", word))
		{side = t_right;}
	else
	if(!stricmp("bottom", word))
		{side = t_bottom;}
	else
	if(!stricmp("rear", word))
		{side = t_rear;}
	vol_thickness -> sides |= side;
	word = m_sdl -> GetNextWord();
	switch(side)
	{
		case t_top:
			{vol_thickness -> TT.v[0] = (float)atof(word);}
		break;
		case t_front:
			{vol_thickness -> FT.v[0] = (float)atof(word);}
		break;
		case t_left:
			{vol_thickness -> LT.v[0] = (float)atof(word);}
		break;
		case t_right:
			{vol_thickness -> RiT.v[0] = (float)atof(word);}
		break;
		case t_bottom:
			{vol_thickness -> BT.v[0] = (float)atof(word);}
		break;
		case t_rear:
			{vol_thickness -> ReT.v[0] = (float)atof(word);}
		break;
	};
	word = m_sdl -> GetNextWord();
	if(!stricmp("}", word))
		{return;}
	switch(side)
	{
		case t_top:
		{
			vol_thickness -> TT.v[1] = (float)atof(word);
			vol_thickness -> TT.l = TRUE;
		}
		break;
		case t_front:
		{
			vol_thickness -> FT.v[1] = (float)atof(word);
			vol_thickness -> FT.l = TRUE;
		}
		break;
		case t_left:
		{
			vol_thickness -> LT.v[1] = (float)atof(word);
			vol_thickness -> LT.l = TRUE;
		}
		break;
		case t_right:
		{
			vol_thickness -> RiT.v[1] = (float)atof(word);
			vol_thickness -> RiT.l = TRUE;
		}
		break;
		case t_bottom:
		{
			vol_thickness -> BT.v[1] = (float)atof(word);
			vol_thickness -> BT.l = TRUE;
		}
		break;
		case t_rear:
		{
			vol_thickness -> ReT.v[1] = (float)atof(word);
			vol_thickness -> ReT.l = TRUE;
		}
		break;
	};
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"THICKNESS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"THICKNESS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetThicknessSide", MB_ICONHAND);
	}
}

void CVolume::SetOrientation()
{
	int i, j;
	char *word;
	if(!m_Matrix34)
	{
		m_Matrix34 = new matrix34_t;
		memset(m_Matrix34, 0, sizeof(matrix34_t));
	}
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			word = m_sdl -> GetNextWord();
			m_Matrix34 -> v[i][j] = (float)atof(word);
		}
	}
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"ORIENTATION\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"ORIENTATION\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetOrientation", MB_ICONHAND);
	}
}

void CVolume::SetPolyhedron()
{
    char *word = m_sdl -> GetNextWord();
    m_Type = VT_PLY;
	char vol_str[_MAX_PATH] = {0};
    // check for and remove the quotes
    int begin = 0;
    if(word[begin] == '"')
	{
        if(word[strlen(word)] == '"')
			{word[strlen(word)-1] = '\0';}
        begin++;
	}
    FixPathDelim(word);
	if(!m_PolyhedronName)
	{
		m_PolyhedronName = new char[strlen(&word[begin]) + 1];
		strcpy(m_PolyhedronName, &word[begin]);
		strcpy(vol_str, m_PolyhedronName);
		char *c, lpath[_MAX_PATH];
		char *tpath = GetModelPath();
		strcpy(lpath, tpath);
		if(strlen(tpath) && m_PolyhedronName[1] != ':')
		{
			c = strrchr(lpath, '/');
			if(c)
			{
				if(m_PolyhedronName[0] != '/')
					{c++;}
				*c = '\0';
				strcat(lpath, m_PolyhedronName);
				delete[] m_PolyhedronName;
				m_PolyhedronName = NULL;
				m_PolyhedronName = new char[strlen(lpath) + 1];
				strcpy(m_PolyhedronName, lpath);
			}
		}
	}
    word = m_sdl -> GetNextWord();
    if(!stricmp("{", word))
		{ParseFile();}
    else
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"POLYHEDRON\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"POLYHEDRON\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CVolume::SetPolyhedron", MB_ICONHAND);
	}
	if(!m_Vol)
	{
		FILE *FVolTest = NULL;
		FVolTest = fopen(m_PolyhedronName, "rb");
		if(!FVolTest)
		{
			CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
			char PathTest[MAX_PATH] = {0};
			int skip_item = -1;
			char independent_path[_MAX_PATH] = {0};
			bool vol_test = false, find_mask = false;
			strcpy(PathTest, m_PolyhedronName);
			strlwr(PathTest);
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
			if(find_mask)
			{
				for(int path_search = 5; path_search > -1 && !vol_test; path_search--)
				{
					if(skip_item >= 0 && path_search == skip_item)
						{continue;}
					if(!strlen(pWnd -> m_ResPath[path_search]))
						{continue;}
					sprintf(PathTest, "%s%s", pWnd -> m_ResPath[path_search], independent_path);
					if(!(FVolTest = fopen(PathTest, "r")))
						{continue;}
					else
					{
						vol_test = true;
						fclose(FVolTest);
						FVolTest = NULL;
						delete[] m_PolyhedronName;
						m_PolyhedronName = NULL;
						m_PolyhedronName = new char[strlen(PathTest) + 1];
						strcpy(m_PolyhedronName, PathTest);
					}
				}
			}
			if(!find_mask || !vol_test)
			{
				char ErrMsg[512] = {0};
				char *sptr = m_Name;
				if(*sptr == '"')
					{sptr++;}
				#ifdef ALTERNATIVE_LANG
					sprintf(ErrMsg, "VOL-volume file %s not detected:\n%s", sptr, vol_str);
				#else
					sprintf(ErrMsg, "VOL-файл волюма %s не обнаружен:\n%s", sptr, vol_str);
				#endif
				MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, ErrMsg, "ERROR: CVolume::SetPolyhedron", MB_ICONHAND);
				return;
			}
		}
		else
			{fclose(FVolTest);}
		m_Vol = new CVol(m_PolyhedronName);
	}
}

void CVolume::WriteMdl(FILE *fp, int indent)
{
	char buffer[1024];
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	char *tname = this -> m_Name;
	BYTE FT = 3;
	if(tname[0] == '"')
		{tname++;}
	fprintf(fp, "%*s{Volume \"%s\"\n", (4 * indent++), "", tname);
	switch(this -> m_Type)
	{
		case VT_BOX:
			{fprintf(fp, "%*s{Box %g %g %g}\n", (4 * indent), "", m_Box[0], m_Box[1], m_Box[2]);}
		break;
		case VT_CYLINDER:
			{fprintf(fp, "%*s{Cylinder %g %g}\n", (4 * indent), "", m_Cylinder[0], m_Cylinder[1]);}
		break;
		case VT_PLY:
		{
			if(m_PolyhedronName)
			{
				CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
				char *tvolume = NULL, *sptr = 0;
				FixPathDelim(m_PolyhedronName);
				if(!strlen(pWnd -> m_CurMdlPath))
					{tvolume = strrchr(m_PolyhedronName, '/') + 1;}
				else
				{
					char path[_MAX_PATH] = {0};
					strcpy(path, pWnd -> m_CurMdlPath);
					char *ptr = strrchr(path, '/') + 1;
					*ptr = 0;
					strlwr(path);
					strlwr(m_PolyhedronName);
					char independent_path[_MAX_PATH] = {0};
					char PathTest[MAX_PATH] = {0};
					bool vol_test = false, find_mask = false;
					int skip_item = -1;
					FILE *FVolTest = NULL;
					for(int path_mask = 5; path_mask > -1; path_mask--)
					{
						int ResPath_len = strlen(pWnd -> m_ResPath[path_mask]);
						char tmp_res_path[_MAX_PATH] = {0};
						strcpy(tmp_res_path, pWnd -> m_ResPath[path_mask]);
						FixPathDelim(tmp_res_path);
						strlwr(tmp_res_path);
						if(ResPath_len && strstr(path, tmp_res_path))
						{
							find_mask = true;
							strncpy(independent_path, path + ResPath_len, strlen(path) - ResPath_len);
							skip_item = path_mask;
							break;
						}
					}
					if(find_mask)
					{
						sptr = m_PolyhedronName + strlen(path);
						for(int path_search = 5; path_search > -1 && !vol_test; path_search--)
						{
							if(skip_item >= 0 && path_search == skip_item)
								{continue;}
							if(!strlen(pWnd -> m_ResPath[path_search]))
								{continue;}
							sprintf(PathTest, "%s%s%s", pWnd -> m_ResPath[path_search], independent_path, sptr);
							FixPathDelim(PathTest);
							if(!(FVolTest = fopen(PathTest, "r")))
								{continue;}
							else
							{
								vol_test = true;
								fclose(FVolTest);
								FVolTest = NULL;
								sprintf(PathTest, "%s%s", pWnd -> m_ResPath[path_search], independent_path);
								strcpy(path, PathTest);
							}
						}
					}
					FixPathDelim(m_PolyhedronName);
					strlwr(m_PolyhedronName);
					FixPathDelim(path);
					strlwr(path);
					#if Copy_files_on_saving
					sptr = strrchr(m_PolyhedronName, '/') + 1;
					strcat(path, sptr);
					if(!_access(m_PolyhedronName, 0))
						{CopyFile(m_PolyhedronName, path, true);}
					tvolume = sptr;
					#else
					tvolume = strstr(m_PolyhedronName, path);
					if(tvolume)
						{tvolume += strlen(path);}
					else
					{
						tvolume = m_PolyhedronName;
						if(tvolume)
						{
							char msg[512] = {0};
							#ifdef ALTERNATIVE_LANG
								sprintf(msg, "For VOL-file volume \"%s\" the absolute path is used\r\n%s", tname, m_PolyhedronName);
							#else
								sprintf(msg, "Для VOL-файла волюма \"%s\" используется абсолютный путь\r\n%s", tname, m_PolyhedronName);
							#endif
							MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "WARNING: CVolume::WriteMdl", MB_ICONEXCLAMATION);
						}
					}
					#endif
				}
				if(!tvolume || _access(m_PolyhedronName, 0))
				{
					tvolume = NULL;
					char msg[512] = {0};
					#ifdef ALTERNATIVE_LANG
						sprintf(msg, "There is no access to the VOL file of the volume \"%s\".\r\nThe file is blocked or has an incorrect path.\r\nUnlock/fix the file location and re-attach it to the volume\r\n\r\n%s", tname, m_PolyhedronName);
					#else
						sprintf(msg, "Нет доступа к VOL-файлу волюма \"%s\".\r\nФайл заблокирован или имеет неверный путь.\r\nРазблокируйте/исправьте расположение файла и заново прикрепите его к волюму\r\n\r\n%s", tname, m_PolyhedronName);
					#endif
					MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "ERROR: CVolume::WriteMdl", MB_ICONHAND);
				}
				if(tvolume)
					{fprintf(fp, "%*s{Polyhedron \"%s\"}\n", (4 * indent), "", tvolume);}
			}
		}
		break;
	}
	if(m_Bone)
	{
		char *tbone = m_Bone;
		if(tbone[0] == '"')
			{tbone++;}
		CBone *pSelect = pDoc -> m_Model -> m_skeleton -> m_bonelist -> FindBone(tbone);
		if(pSelect)
			{strcpy(m_Bone, pSelect -> m_Name);}
		fprintf(fp, "%*s{Bone \"%s\"}\n", (4 * indent), "", tbone);
	}
	if(m_Component)
	{
		char *tcomponent = m_Component;
		if(tcomponent[0] == '"')
			{tcomponent++;}
		fprintf(fp, "%*s{Component \"%s\"}\n", (4 * indent), "", tcomponent);
	}
	if(m_Tags)
	{
		char *ttagd = m_Tags;
		if(ttagd[0] == '"')
			{ttagd++;}
		fprintf(fp, "%*s{Tags \"%s\"}\n", (4 * indent), "", ttagd);
	}
	if(vol_thickness)
	{
		fprintf(fp, "%*s{Thickness %g", (4 * indent), "", vol_thickness -> GT.v[0]);
		if(vol_thickness -> GT.l)
			{fprintf(fp, " %g", vol_thickness -> GT.v[1]);}
		if(vol_thickness -> sides)
		{
			fprintf(fp, "\n");
			indent++;
			if(vol_thickness -> sides & t_front)
			{
				fprintf(fp, "%*s{Front %g", (4 * indent), "", vol_thickness -> FT.v[0]);
				if(vol_thickness -> FT.l)
					{fprintf(fp, " %g", vol_thickness -> FT.v[1]);}
				fprintf(fp, "}\n");
			}
			if(vol_thickness -> sides & t_rear)
			{
				fprintf(fp, "%*s{Rear %g", (4 * indent), "", vol_thickness -> ReT.v[0]);
				if(vol_thickness -> ReT.l)
					{fprintf(fp, " %g", vol_thickness -> ReT.v[1]);}
				fprintf(fp, "}\n");
			}
			if(vol_thickness -> sides & t_right)
			{
				fprintf(fp, "%*s{Right %g", (4 * indent), "", vol_thickness -> RiT.v[0]);
				if(vol_thickness -> RiT.l)
					{fprintf(fp, " %g", vol_thickness -> RiT.v[1]);}
				fprintf(fp, "}\n");
			}
			if(vol_thickness -> sides & t_left)
			{
				fprintf(fp, "%*s{Left %g", (4 * indent), "", vol_thickness -> LT.v[0]);
				if(vol_thickness -> LT.l)
					{fprintf(fp, " %g", vol_thickness -> LT.v[1]);}
				fprintf(fp, "}\n");
			}
			if(vol_thickness -> sides & t_top)
			{
				fprintf(fp, "%*s{Top %g", (4 * indent), "", vol_thickness -> TT.v[0]);
				if(vol_thickness -> TT.l)
					{fprintf(fp, " %g", vol_thickness -> TT.v[1]);}
				fprintf(fp, "}\n");
			}
			if(vol_thickness -> sides & t_bottom)
			{
				fprintf(fp, "%*s{Bottom %g", (4 * indent), "", vol_thickness -> BT.v[0]);
				if(vol_thickness -> BT.l)
					{fprintf(fp, " %g", vol_thickness -> BT.v[1]);}
				fprintf(fp, "}\n");
			}
			indent--;
			fprintf(fp, "%*s}\n", (4 * indent), "");
		}
		else
			{fprintf(fp, "}\n");}
	}
	if(m_Matrix34)
	{
		if(m_Matrix34 -> v[3][0] == 0 && m_Matrix34 -> v[3][1] == 0 && m_Matrix34 -> v[3][2] == 0)
			{FT--;}
		if(m_Matrix34 -> v[0][0] == 1 && m_Matrix34 -> v[1][1] == 1 && m_Matrix34 -> v[2][2] == 1 \
		 && m_Matrix34 -> v[0][1] == 0 && m_Matrix34 -> v[0][2] == 0 && m_Matrix34 -> v[1][0] == 0 && m_Matrix34 -> v[1][2] == 0 && m_Matrix34 -> v[2][0] == 0 && m_Matrix34 -> v[2][1] == 0)
			{FT -= 2;}
		if(pWnd -> m_GEMI_Compatibility)
			{FT = 3;}
		switch(FT)
		{
			case 1:
				{fprintf(fp, "%*s{Position %g\t%g\t%g}\n", (4 * indent), "", m_Matrix34 -> v[3][0], m_Matrix34 -> v[3][1], m_Matrix34 -> v[3][2]);}
			break;
			case 2:
			{
				fprintf(fp, "%*s{Orientation\n", (4 * indent++), "");
				for(int i = 0; i < 3; i++)
					{fprintf(fp, "%*s%g\t%g\t%g\n", (4 * indent), "", m_Matrix34 -> v[i][0], m_Matrix34 -> v[i][1], m_Matrix34 -> v[i][2]);}
				indent--;
				fprintf(fp, "%*s}\n", (4 * indent), "");
			}
			break;
			case 3:
			{
				fprintf(fp, "%*s{Matrix34\n", (4 * indent++), "");
				for(int i = 0; i < 4; i++)
					{fprintf(fp, "%*s%g\t%g\t%g\n", (4 * indent), "", m_Matrix34 -> v[i][0], m_Matrix34 -> v[i][1], m_Matrix34 -> v[i][2]);}
				indent--;
				fprintf(fp, "%*s}\n", (4 * indent), "");
			}
			break;
		};
	}
	if(m_Density)
		{fprintf(fp, "%*s{Density \"%g\"}\n", (4 * indent), "", m_Density);}
	indent--;
	fprintf(fp, "%*s}\n", (4 * indent), "");
}

CVolumeList::CVolumeList()
{
    m_sdl = NULL;
    m_head = NULL;
    m_tail = NULL;
    m_curr = NULL;
}

CVolumeList::CVolumeList(CSdl *sdl)
{
    m_sdl = sdl;
    m_head = NULL;
    m_tail = NULL;
    m_curr = NULL;
}

CVolumeList::~CVolumeList()
   {ClearList();}

CVolume *CVolumeList::AddToHead()
{
    CVolume *t_volume;
    if(m_sdl == NULL)
		{t_volume = new CVolume();}
    else
		{t_volume = new CVolume(m_sdl);}
    if(m_head != NULL)
	{
        t_volume -> next = m_head;
        m_head -> prev = t_volume;
        m_head = t_volume;
	}
    else
	{
        m_head = t_volume;
        m_tail = t_volume;
        m_curr = t_volume;
	}
    return t_volume;
}

CVolume *CVolumeList::AddToTail()
{
    CVolume *t_volume;
    if(m_sdl == NULL)
		{t_volume = new CVolume();}
    else
		{t_volume = new CVolume(m_sdl);}
    if(m_tail != NULL)
	{
        t_volume -> prev = m_tail;
        m_tail -> next = t_volume;
        m_tail = t_volume;
	}
    else
	{
        m_head = t_volume;
        m_tail = t_volume;
        m_curr = t_volume;
	}
    return t_volume;
}

CVolume *CVolumeList::GetFirst()
{
    m_curr = m_head;
    return m_curr;
}

CVolume *CVolumeList::GetNext()
{
    CVolume *t_volume = NULL;
    if((m_curr) && (m_curr -> next))
	{
        m_curr = m_curr -> next;
        t_volume = m_curr;
	}
    return t_volume;
}

CVolume *CVolumeList::GetPrev()
{
    CVolume *t_volume = NULL;
    if((m_curr) && (m_curr -> prev))
	{
        m_curr = m_curr -> prev;
        t_volume = m_curr;
	}
    return t_volume;
}

CVolume *CVolumeList::GetCurr()
	{return m_curr;}

void CVolumeList::ClearList()
{
    CVolume *t_volume;
    while(m_tail)
	{
        t_volume = m_tail -> prev;
        delete m_tail;
        m_tail = t_volume;
	}
    m_curr = NULL;
    m_head = NULL;
    m_tail = NULL;
}

CVolume *CVolumeList::GetLast()
{
    m_curr = m_tail;
    return m_curr;
}

CVolume *CVolumeList::FindVolumeByTreeID(HTREEITEM ID)
{
    CVolume *pVol = GetFirst();
    while (pVol)
	{
        if(pVol -> hTreeItem == ID)
			{return pVol;}
        pVol = pVol -> next;
	}
    return NULL;
}

CVolume *CVolumeList::FindByBone(char *szBone)
{
    CVolume *pVol = GetFirst();
    while (pVol)
	{
        if(!stricmp(szBone, pVol -> m_Bone))
			{return pVol;}
        pVol = pVol -> next;
	}
    return NULL;
}

int CVolumeList::FindCountBone(void)
{
    CVolume *pVol = GetFirst();
	int CouName = 1;
    while(pVol)
	{
		CouName++;
        pVol = pVol -> next;
	}
    return CouName;
}

CVolume *CVolumeList::FindByName(char *szName)
{
    CVolume *pVol = GetFirst();
    while(pVol)
	{
        if(pVol -> m_Name && !stricmp(szName, pVol -> m_Name))
			{return pVol;}
        pVol = pVol -> next;
	}
    return NULL;
}

void CVolumeList::DropVolume(char *szName)
{
	CVolume *pVol = FindByName(szName);
	if(pVol != NULL)
	{
		if(pVol -> prev)
			{pVol -> prev -> next = pVol -> next;}
		if(pVol -> next)
			{pVol -> next -> prev = pVol -> prev;}
		if(this -> m_head == pVol)
			{this -> m_head = pVol -> next;}
		if(this -> m_tail == pVol)
			{this -> m_tail = pVol -> prev;}
		delete pVol;
	}
}

void CVolumeList::WriteMdl(FILE *fp, int indent)
{
    CVolume *pVol = GetFirst();
    while(pVol)
	{
        pVol -> WriteMdl(fp, indent);
        pVol = pVol -> next;
	}
}