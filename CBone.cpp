// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CBone.h"
#include "CAnimation.h"
#include "CSdl.h"
#include "CPly.h"
#include "MainFrm.h"
#include "io.h"
#include <string.h>

extern HWND m_hWnd;

void CBone::CommonIni(void)
{
	m_VolumeViewName = m_Parameters = m_Name = NULL;
    m_VolumeView = NULL;
    m_Animation = NULL;
    m_Skin = NULL;
    m_Animatrix = m_Position = NULL;
    m_Limits[0] = 0.0f;
    m_Limits[1] = 0.0f;
    m_Speed = 0.0f;
    Matrix34 = NULL;
	Matrix34 = new matrix34_t;
	memset(Matrix34, 0, sizeof(matrix34_t));
	Matrix34 -> v[0][0] = 1.0f;
	Matrix34 -> v[1][1] = 1.0f;
	Matrix34 -> v[2][2] = 1.0f;
    prev = next = m_curr = m_ChildLast = m_ChildFirst = NULL;
    m_Orientation = NULL;
    m_Visibility = 2;
	sub = adds = 0;
	m_Layer = -1;
	hTreeItem = NULL;
	LODView = 0;
	m_tree_check = true;
	m_anm_visi = true;
	HasLimits = m_LimitsType = m_Terminator = false;
	m_Ground = m_Revolute = m_Prizmatic = m_Socket = m_Speed2 = false;
	m_OpenInTree = m_LOD_OFF = m_NoCastShadows = m_LODView = false;
	m_DecalTarget = m_NoGroupMesh = m_NoGetShadows = false;
	m_bone_color = 0xffffffff;
	m_bone_colorized = false;
	m_start_colorized = false;
    for(int i = 0; i < 3; i++)
	{
        m_Rotations[i] = 0.0f;
        m_Translations[i] = 0.0f;
        m_Scales[i] = 1.0f;
	}
}

CBone::CBone()
{
	CommonIni();
	m_parent = NULL;
	m_sdl = NULL;
}

CBone::CBone(CBone *parent)
{
	CommonIni();
	m_parent = parent;
    m_sdl = NULL;
}

CBone::CBone(CSdl *sdl, CBone *parent, BYTE mod, int N)
{
	CommonIni();
	if(mod == 2)
		{m_LODView = true;}
	sub = N;
	m_parent = parent;
    m_sdl = sdl;
    ParseFile();
}

CBone::CBone(CSdl *sdl)
{
	CommonIni();
	m_parent = NULL;
	m_sdl = sdl;
	ParseFile();
}

CBone::~CBone()
{
    if(m_Animation)
	{
        delete m_Animation;
        m_Animation = NULL;
	}
    if(m_Name)
	{
        delete[] m_Name;
        m_Name = NULL;
	}
    if(m_Parameters)
	{
        delete[] m_Parameters;
        m_Parameters = NULL;
	}
    if(m_VolumeViewName)
	{
        delete[] m_VolumeViewName;
        m_VolumeViewName = NULL;
	}
    if(m_VolumeView)
	{
        delete m_VolumeView;
        m_VolumeView = NULL;
	}
    if(m_Position)
	{
        delete [] m_Position;
        m_Position = NULL;
	}
    if(Matrix34)
	{
        delete Matrix34;
        Matrix34 = NULL;
	}
    if(m_Orientation)
	{
        delete m_Orientation;
        m_Orientation = NULL;
	}
}

bool CBone::GetBBox(v3_t *size)
{
    if(m_VolumeView)
	{
        (*size)[0] = m_VolumeView -> m_bbox[1][0] - m_VolumeView -> m_bbox[0][0];
        (*size)[1] = m_VolumeView -> m_bbox[1][1] - m_VolumeView -> m_bbox[0][1];
        (*size)[2] = m_VolumeView -> m_bbox[1][2] - m_VolumeView -> m_bbox[0][2];
        return true;
	}
    else
		{return false;}
}

void CBone::ParseFile()
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
        if(!stricmp("position", word))
			{SetPosition();}
        else
        if(!stricmp("orientation", word))
			{SetOrientation();}
        else
        if(!stricmp("visibility", word))
			{SetVisibility();}
        else
        if(!stricmp("revolute", word))
			{SetRevolute();}
		else
        if(!stricmp("prizmatic", word))
			{SetPrizmatic();}
		else
        if(!stricmp("socket", word))
			{SetSocket();}
        else
        if (!stricmp("limits", word))
			{SetLimits();}
        else
        if(!stricmp("volumeview", word))
			{SetVolumeView();}
        else
        if(!stricmp("animation", word))
			{SetAnimation();}
        else
        if(!stricmp("speed", word))
			{SetSpeed();}
        else
        if(!stricmp("speed2", word))
			{SetSpeed2();}
        else
        if(!stricmp("terminator", word))
			{SetTerminator();}
        else
        if(!stricmp("NoCastShadows", word))
			{SetNoCastShadows();}
		else
        if(!stricmp("off", word))
			{SetOFF();}
        else
        if(!stricmp("parameters", word))
			{SetParameters();}
        else
        if(!stricmp("ground", word))
			{SetGround();}
        else
        if(!stricmp("layer", word))
			{SetLayer();}
        else
        if(!stricmp("matrix34", word))
			{SetMatrix();}
        else
        if(!stricmp("bone", word))
			{AddBone(1, 0);}
        else
		if(!stricmp("##bone", word))
			{AddBone(0, 0);}
        else
		if(!stricmp("LODView", word))
		{
			LODView = 1;
			ParseFile();
			LODView = 0;
			if(m_LODView && !sub)
			{
				//анализ цепи на офф
				CBone *child = NULL;
				if((child = m_ChildFirst) != NULL)
				{
					while(child && child -> m_LODView && child -> sub)
					{
						if(child -> m_LOD_OFF)
						{
							m_LOD_OFF = true;
							child -> m_LOD_OFF = false;
						}
						child = child -> next;
					}
				}
			}
		}
		else
        if(!stricmp("Color", word))
			{SetColor();}
        else
        if(!stricmp("DecalTarget", word))
			{SetDecalTarget();}
        else
        if(!stricmp("NoGroupMesh", word))
			{SetGroupMesh();}
        else
        if(!stricmp("NoGetShadows", word))
			{SetGetShadows();}
        else
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("Unidentified command: %s\r\nLine: %d", word, m_sdl -> m_line);
			#else
				msg_st.Format("Неопознанная команда: %s\r\nСтрока: %d", word, m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::ParseFile", MB_ICONHAND);
		}
		word = m_sdl -> GetNextWord();
	}
}

void CBone::SetName(char *name)
{
	char num[20] = {0};
	int len = 0;
	if(m_Name)
	{
		delete[] m_Name;
		m_Name = NULL;
	}
	if(sub)
	{
		if(m_LODView)
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(num, " (Lod-%d)", sub);
			#else
				sprintf(num, " (Лод-%d)", sub);
			#endif
		}
		else
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(num, " (Sub. mesh-%d)", sub);
			#else
				sprintf(num, " (Суб. мэш-%d)", sub);
			#endif
		}
		char ply_str[_MAX_PATH] = {0};
		strcpy(ply_str, name + ((name[0] == '"') ? 1 : 0));
		FixPathDelim(ply_str);
		len = strlen(ply_str) + 1;
		m_VolumeViewName = new char[len];
		memset(m_VolumeViewName, 0, len);
		strcpy(m_VolumeViewName, ply_str);
		AddPly(ply_str);
		len = strlen(m_parent -> m_Name) + strlen(num) + 1;
		m_Name = new char[len];
		memset(m_Name, 0, len);
		strcpy(m_Name, m_parent -> m_Name);
		strcat(m_Name, num);
	}
	else
	{
		len = strlen(name) + 2;//Так надо!
		m_Name = new char[len];
		memset(m_Name, 0, len);
		strcpy(m_Name, name);
	}
}

void CBone::SetRevolute()
	{m_Revolute = true;}

void CBone::SetSocket()
	{m_Socket = true;}

void CBone::SetPrizmatic()
	{m_Prizmatic = true;}

void CBone::SetGround()
{
    m_Ground = true;
    char *word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"GROUND\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"GROUND\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetGround", MB_ICONHAND);
	}
}

void CBone::SetColor()
{
	m_bone_colorized = true;
    char *word = m_sdl -> GetNextWord();
	char *c;
	if(word[0] == '0' && word[1] == 'x')
		{m_bone_color = strtoul(word, &c, 16);}
	else
		{m_bone_color = strtoul(word, &c, 10);}
	m_bone_color = RGBA(GetBValue(m_bone_color), GetGValue(m_bone_color), GetRValue(m_bone_color), GetAValue(m_bone_color));
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"COLOR\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"COLOR\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetColor", MB_ICONHAND);
	}
}

void CBone::SetLayer()
{
	char *word = m_sdl -> GetNextWord();
	if(m_LODView)
		{m_Layer = (int)atoi(word);}
	else
	if(!stricmp("waterscissor", word))
		{m_Layer = 0;}
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"LAYER\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"LAYER\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetLayer", MB_ICONHAND);
	}
}

void CBone::SetTrash()
{
    char *word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"TRASH\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"TRASH\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetTrash", MB_ICONHAND);
	}
}

void CBone::SetTerminator()
{
	char *word = m_sdl -> GetNextWord();
	m_Terminator = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"TERMINATOR\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"TERMINATOR\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetTerminator", MB_ICONHAND);
	}
}

void CBone::SetNoCastShadows()
{
	char *word = m_sdl -> GetNextWord();
	m_NoCastShadows = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"NOCASTSHADOWS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"NOCASTSHADOWS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetNoCastShadows", MB_ICONHAND);
	}
}

void CBone::SetOFF()
{
	char *word = m_sdl -> GetNextWord();
	m_LOD_OFF = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"OFF\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"OFF\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetOFF", MB_ICONHAND);
	}
}
  
void CBone::SetDecalTarget()
{
	char *word = m_sdl -> GetNextWord();
	m_DecalTarget = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"DECALTARGET\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"DECALTARGET\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetDecalTarget", MB_ICONHAND);
	}
}

void CBone::SetGroupMesh()
{
	char *word = m_sdl -> GetNextWord();
	m_NoGroupMesh = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"GROUPMESH\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"GROUPMESH\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetGroupMesh", MB_ICONHAND);
	}
}

void CBone::SetGetShadows()
{
	char *word = m_sdl -> GetNextWord();
	m_NoGetShadows = true;
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"GETSHADOWS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"GETSHADOWS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetGetShadows", MB_ICONHAND);
	}
}

void CBone::SetParameters()
{
    char *word = m_sdl -> GetNextWord();
    if(m_Parameters)
	{
		delete[] m_Parameters;
		m_Parameters = NULL;
	}
    m_Parameters = new char[strlen(word) + 1];
    strcpy(m_Parameters, word);
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"PARAMETERS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"PARAMETERS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetParameters", MB_ICONHAND);
	}
}

void CBone::SetVolumeView()
{
	char *word;
    int begin = 0;
	char ply_str[_MAX_PATH] = {0};
	if(m_VolumeViewName)
	{
		adds++;
		AddBone(((LODView == 1) ? 2 : 1), adds);
		return;
	}
	if(LODView == 1)
		{m_LODView = true;}
	word = m_sdl -> GetNextWord();
    if(word[begin] == '"')// check for and remove the quotes
	{
        if(word[strlen(word)] == '"')
			{word[strlen(word) - 1] = '\0';}
        begin++;
	}
    FixPathDelim(word);
	if(!m_VolumeViewName)
	{
		m_VolumeViewName = new char[strlen(&word[begin]) + 1];
		strcpy(m_VolumeViewName, &word[begin]);
		strcpy(ply_str, m_VolumeViewName);
	}
    word = m_sdl -> GetNextWord();
    if(!stricmp("{", word))
		{ParseFile();}
    else
	{
		if(stricmp("}", word))
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("The block is damaged \"VOLUMEVIEW\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
			#else
				msg_st.Format("Повреждён блок \"VOLUMEVIEW\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetVolumeView", MB_ICONHAND);
		}
	}
	AddPly(ply_str);
}

void CBone::SetMatrix()
{
	int i, j;
	char *word;
	if(Matrix34)
		{delete Matrix34;}
	Matrix34 = new matrix34_t;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 3; j++)
		{
			word = m_sdl -> GetNextWord();
			Matrix34 -> v[i][j] = (float)atof(word);
		}
	}
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"MATRIX\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"MATRIX\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetMatrix", MB_ICONHAND);
	}
}

void CBone::SetOrientation()
{
	int i, j;
	char *word;
	if(!Matrix34)
	{
		Matrix34 = new matrix34_t;
		memset(Matrix34, 0, sizeof(matrix34_t));
	}
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			word = m_sdl -> GetNextWord();
			Matrix34 -> v[i][j] = (float)atof(word);
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
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetOrientation", MB_ICONHAND);
	}
}

void CBone::SetSpeed()//1/20 метра, радианы
{
	char *word;
	word = m_sdl -> GetNextWord();
	m_Speed = (float)atof(word);
	word = m_sdl->GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"SPEED\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"SPEED\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetSpeed", MB_ICONHAND);
	}
}

void CBone::SetSpeed2()//метров в секунду, градусы
{
	char *word;
	m_Speed2 = true;
	word = m_sdl -> GetNextWord();
	m_Speed = (float)atof(word);
	word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"SPEED2\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"SPEED2\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetSpeed2", MB_ICONHAND);
	}
}

void CBone::SetVisibility()
{
    char *word;
    word = m_sdl -> GetNextWord();
    m_Visibility = atoi(word);
    word = m_sdl -> GetNextWord();
    if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"VISIBILITY\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"VISIBILITY\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetVisibility", MB_ICONHAND);
	}
}

void CBone::SetLimits()
{
    char *word;
	HasLimits = true;
    word = m_sdl -> GetNextWord();
	if(!stricmp("}", word))
	{
		m_Limits[0] = 0.0f;
		m_Limits[1] = 0.0f;
		m_LimitsType = true;
		return;
	}
    m_Limits[0] = (float)atof(word);
    word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		m_Limits[1] = (float)atof(word);
		word = m_sdl->GetNextWord();
		m_LimitsType = true;
		if(stricmp("}", word))
		{
			#ifdef ALTERNATIVE_LANG
				msg_st.Format("The block is damaged \"LIMITS\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
			#else
				msg_st.Format("Повреждён блок \"LIMITS\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetLimits", MB_ICONHAND);
		}
	}
	else
		{m_Limits[1] = 0.0f;}
}

void CBone::SetPosition()
{
    int j;
    char *word;
	if(!Matrix34)
	{
		Matrix34 = new matrix34_t;
		memset(Matrix34, 0, sizeof(matrix34_t));
		Matrix34 -> v[0][0] = 1.0f;
		Matrix34 -> v[1][1] = 1.0f;
		Matrix34 -> v[2][2] = 1.0f;
	}
	for(j = 0; j < 3; j++)
	{
		word = m_sdl -> GetNextWord();
		Matrix34 -> v[3][j] = (float)atof(word);
	}
    word = m_sdl -> GetNextWord();
	if(stricmp("}", word))
	{
		#ifdef ALTERNATIVE_LANG
			msg_st.Format("The block is damaged \"POSITION\". Incorrect end of the block.\r\nLine: %d", m_sdl -> m_line);
		#else
			msg_st.Format("Повреждён блок \"POSITION\". Неверный конец блока.\r\nСтрока: %d", m_sdl -> m_line);
		#endif
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg_st, "ERROR: CBone::SetPosition", MB_ICONHAND);
	}
}

void CBone::SetAnimation()
	{m_Animation = new CAnimation(m_sdl);}

void CBone::AddBone(int mod, int N)
	{AddToTail(mod, N);}

CBone *CBone::AddToHead()
{
    CBone *t_bone = new CBone(m_sdl, this, true, 0);
    if(m_ChildFirst != NULL)
	{
        t_bone -> next = m_ChildFirst;
        m_ChildFirst -> prev = t_bone;
        m_ChildFirst = t_bone;
	}
    else
	{
        m_ChildFirst = t_bone;
        m_ChildLast = t_bone;
        m_curr = t_bone;
	}
    return t_bone;
}

CBone *CBone::AddToTail(int mod, int N)
{
    CBone *t_bone = new CBone(m_sdl, this, mod, N);
	if(!mod)
	{
		delete t_bone;
		return NULL;
	}
    if(m_ChildLast != NULL)
	{
        t_bone -> prev = m_ChildLast;
        m_ChildLast -> next = t_bone;
        m_ChildLast = t_bone;
	}
    else
	{
        m_ChildFirst = t_bone;
        m_ChildLast = t_bone;
        m_curr = t_bone;
	}
    return t_bone;
}

CBone *CBone::GetFirst()
	{m_curr = m_ChildFirst; return m_curr;}

CBone *CBone::GetNext()
{
    CBone *t_bone = NULL;
    if((m_curr) && (m_curr -> next))
	{
        m_curr = m_curr -> next;
        t_bone = m_curr;
	}
    return t_bone;
}

CBone *CBone::GetPrev()
{
    CBone *t_bone = NULL;
    if((m_curr) && (m_curr -> prev))
	{
        m_curr = m_curr -> prev;
        t_bone = m_curr;
	}
    return t_bone;
}

CBone *CBone::GetCurr()
	{return m_curr;}

void CBone::ClearList()//Оно вообще кому-либо надо?
{
    CBone *t_bone;
    while(m_ChildLast)
	{
        t_bone = m_ChildLast -> prev;
        delete m_ChildLast;
        m_ChildLast = t_bone;
	}
    m_curr = NULL;
    m_ChildFirst = NULL;
    m_ChildLast = NULL;
}

CBone *CBone::GetLast()
{
    m_curr = m_ChildLast;
    return m_curr;
}

CBone *CBone::FindBoneByTreeID(HTREEITEM ID)
{
	CBone *child = NULL, *fbone = NULL;
	if(m_Name && hTreeItem == ID)
		{return this;}
	if(m_ChildFirst)
	{
		fbone = m_ChildFirst -> FindBoneByTreeID(ID);
		if(fbone)
			{return fbone;}
	}
	if(next)
	{
		fbone = next -> FindBoneByTreeID(ID);
		if(fbone)
			{return fbone;}
	}
    return NULL;
}

CBone *CBone::FindBone(char *szName)
{
    CBone *child = NULL;
	CBone *fbone = NULL;
    if(this && m_Name && !stricmp(szName, m_Name))
		{return this;}
	if(this && (child = this -> m_ChildFirst) != NULL)
		{fbone = child -> FindBone(szName);}
	if(fbone)
		{return fbone;}
	if(this && this -> next)
		{fbone = next -> FindBone(szName);}
	if(fbone)
		{return fbone;}
    return NULL;
}

void CBone::FindCountBone(int &CouName)
{
    CBone *child = NULL;
    if(this && !this -> sub)
		{CouName++;}
	if(this && (child = this -> m_ChildFirst) != NULL)
		{child -> FindCountBone(CouName);}
	if(this && this -> next)
		{next -> FindCountBone(CouName);}
    return;
}

void CBone::FixGeometry()
{
    if(Matrix34) // We have to stay with this
	{
        int i, j;
        float Xmatrix[4][4], Vmatrix[4][3];
        for(i = 0; i < 4; i++)
		{
            for(j = 0; j < 4; j++)
				{Xmatrix[i][j] = 0.0f;}
		}
        Xmatrix[0][0] = 1.0f;
        Xmatrix[1][1] = 1.0f;
        Xmatrix[2][2] = 1.0f;
        Xmatrix[3][3] = 1.0f;
        for(i = 0; i < 4; i++)
		{
            for(j = 0; j < 3; j++)
				{Vmatrix[i][j] = Matrix34 -> v[i][j];}
		}
	}
}

void CBone::WriteMdl(FILE *fp, int indent)
{
	CBone *child = NULL;
	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	CSOEditDoc *pDoc = (CSOEditDoc *)pWnd -> GetActiveDocument();
	BYTE FT = 3;
	CString tmp_str;
	if(!sub)
	{
		if(pDoc -> Copy_Mode)
			{tmp_str.Format("%*s{Bone", (4 * indent++), ""); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, "%*s{Bone", (4 * indent++), "");}
		if(m_Revolute)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" revolute"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " revolute");}
		}
		else
		if(m_Prizmatic)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" prizmatic"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " prizmatic");}
		}
		else
		if(m_Socket)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format(" socket"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, " socket");}
		}
		if(pDoc -> Copy_Mode)
			{tmp_str.Format(" \"%s\"\n", m_Name); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, " \"%s\"\n", m_Name);}
		if(m_Parameters)
		{
			char *tparms = m_Parameters;
			if(tparms[0] == '"')
				{tparms++;}
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{Parameters \"%s\"}\n", (4 * indent), "", tparms); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "%*s{Parameters \"%s\"}\n", (4 * indent), "", tparms);}
		}
		if(m_Animation)
		{
			if(m_Animation -> m_SequenceList)
				{m_Animation -> m_SequenceList -> WriteMdl(fp, indent);}
		}
		if(m_Revolute || m_Socket || m_Prizmatic)
		{
			if(HasLimits)
			{
				if(m_LimitsType)
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{Limits %g %g}\n", (4 * indent), "", m_Limits[0], m_Limits[1]); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{Limits %g %g}\n", (4 * indent), "", m_Limits[0], m_Limits[1]);}
				}
				else
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{Limits %g}\n", (4 * indent), "", m_Limits[0]); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{Limits %g}\n", (4 * indent), "", m_Limits[0]);}
				}
			}
			if((m_Speed != 0.0f) && (m_Speed != 1.0f))
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format("%*s{%s %g}\n", (4 * indent), "", (m_Speed2) ? "Speed2" : "Speed", m_Speed); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, "%*s{%s %g}\n", (4 * indent), "", (m_Speed2) ? "Speed2" : "Speed", m_Speed);}
			}
		}
		if(m_Terminator)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{Terminator}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "%*s{Terminator}\n", (4 * indent), "");}
		}
		if(m_bone_colorized)
		{
			DWORD t_bone_color = RGBA(GetBValue(m_bone_color), GetGValue(m_bone_color), GetRValue(m_bone_color), GetAValue(m_bone_color));
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{Color 0x%x}\n", (4 * indent), "", t_bone_color); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "%*s{Color 0x%x}\n", (4 * indent), "", t_bone_color);}
		}
		if(Matrix34)
		{
			if(Matrix34 -> v[3][0] == 0 && Matrix34 -> v[3][1] == 0 && Matrix34 -> v[3][2] == 0)
				{FT--;}
			// Only skip writing orientation if BOTH the raw matrix is identity
			// AND the scale is {1,1,1}. If scale differs, we must write the matrix.
			bool scaleIsIdentity = (m_Scales[0] == 1.0f && m_Scales[1] == 1.0f && m_Scales[2] == 1.0f);
			if (scaleIsIdentity
				&& Matrix34->v[0][0] == 1 && Matrix34->v[1][1] == 1 && Matrix34->v[2][2] == 1
				&& Matrix34->v[0][1] == 0 && Matrix34->v[0][2] == 0 && Matrix34->v[1][0] == 0 && Matrix34->v[1][2] == 0 && Matrix34->v[2][0] == 0 && Matrix34->v[2][1] == 0)
			{
				FT -= 2;
			}
			if(pWnd -> m_GEMI_Compatibility)
				{FT = 3;}
			switch(FT)
			{
				case 1:
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{Position %g\t%g\t%g}\n", (4 * indent), "", Matrix34 -> v[3][0], Matrix34 -> v[3][1], Matrix34 -> v[3][2]); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{Position %g\t%g\t%g}\n", (4 * indent), "", Matrix34 -> v[3][0], Matrix34 -> v[3][1], Matrix34 -> v[3][2]);}
				}
				break;
				case 2:
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{Orientation\n", (4 * indent++), ""); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{Orientation\n", (4 * indent++), "");}
					for (int i = 0; i < 3; i++)
					{
						// Bake m_Scales into the orientation rows at save time.
						// Column j of row i is scaled by m_Scales[j].
						float r0 = Matrix34->v[i][0] * m_Scales[0];
						float r1 = Matrix34->v[i][1] * m_Scales[1];
						float r2 = Matrix34->v[i][2] * m_Scales[2];
						if (pDoc->Copy_Mode)
						{
							tmp_str.Format("%*s%g\t%g\t%g\n", (4 * indent), "", r0, r1, r2); pDoc->Code_block += tmp_str;
						}
						else
						{
							fprintf(fp, "%*s%g\t%g\t%g\n", (4 * indent), "", r0, r1, r2);
						}
					}
					indent--;
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s}\n", (4 * indent), "");}
				}
				break;
				case 3:
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{Matrix34\n", (4 * indent++), ""); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{Matrix34\n", (4 * indent++), "");}
					for (int i = 0; i < 4; i++)
					{
						float r0, r1, r2;
						if (i < 3)
						{
							// Orientation rows: bake m_Scales[j] into column j.
							r0 = Matrix34->v[i][0] * m_Scales[0];
							r1 = Matrix34->v[i][1] * m_Scales[1];
							r2 = Matrix34->v[i][2] * m_Scales[2];
						}
						else
						{
							// Position row (row 3): never scale the translation.
							r0 = Matrix34->v[i][0];
							r1 = Matrix34->v[i][1];
							r2 = Matrix34->v[i][2];
						}
						if (pDoc->Copy_Mode)
						{
							tmp_str.Format("%*s%g\t%g\t%g\n", (4 * indent), "", r0, r1, r2); pDoc->Code_block += tmp_str;
						}
						else
						{
							fprintf(fp, "%*s%g\t%g\t%g\n", (4 * indent), "", r0, r1, r2);
						}
					}
					indent--;
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s}\n", (4 * indent), "");}
				}
				break;
			};
		}
		// Scale has now been baked into the file. Bake it into the in-memory
		// Matrix34 as well, and reset m_Scales to {1,1,1}, so that rendering
		// and future saves remain consistent.
		if (Matrix34 && !(m_Scales[0] == 1.0f && m_Scales[1] == 1.0f && m_Scales[2] == 1.0f))
		{
			for (int row = 0; row < 3; row++)
			{
				Matrix34->v[row][0] *= m_Scales[0];
				Matrix34->v[row][1] *= m_Scales[1];
				Matrix34->v[row][2] *= m_Scales[2];
			}
			m_Scales[0] = 1.0f;
			m_Scales[1] = 1.0f;
			m_Scales[2] = 1.0f;
		}
		if(m_Visibility != 2)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{Visibility %d}\n", (4 * indent), "", m_Visibility); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "%*s{Visibility %d}\n", (4 * indent), "", m_Visibility);}
		}
	}
	if(/*m_VolumeView && */m_VolumeViewName)
	{
		char *tvolume = NULL, *sptr = 0;
		FixPathDelim(m_VolumeViewName);
		if(!strlen(pWnd -> m_CurMdlPath))
			{tvolume = strrchr(m_VolumeViewName, '/') + 1;}//если работаем вне модели, то все файлы должны быть в папке текущей mdl
		else
		{
			char path[_MAX_PATH] = {0};
			strcpy(path, pWnd -> m_CurMdlPath);
			char *ptr = strrchr(path, '/') + 1;
			*ptr = 0;
			//path D:/Games/В тылу врага 2 - Лис Пустыни (normal)/resource/entity/-vehicle/tank_top/JagdMaus/
			//от него ищем маску
			strlwr(path);
			strlwr(m_VolumeViewName);
			char independent_path[_MAX_PATH] = {0};
			char PathTest[MAX_PATH] = {0};
			bool ply_test = false, find_mask = false;
			int skip_item = -1;
			FILE *FPlyTest = NULL;
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
			if(find_mask)//если модель из игры или мода, то можем опираться на её путь
			{
				sptr = m_VolumeViewName + strlen(path);
				for(int path_search = 5; path_search > -1 && !ply_test; path_search--)
				{
					if(skip_item >= 0 && path_search == skip_item)
						{continue;}
					if(!strlen(pWnd -> m_ResPath[path_search]))
						{continue;}
					sprintf(PathTest, "%s%s%s", pWnd -> m_ResPath[path_search], independent_path, sptr);
					FixPathDelim(PathTest);
					if(!(FPlyTest = fopen(PathTest, "r")))
						{continue;}
					else
					{
						ply_test = true;
						fclose(FPlyTest);
						FPlyTest = NULL;
						sprintf(PathTest, "%s%s", pWnd -> m_ResPath[path_search], independent_path);
						strcpy(path, PathTest);
					}
				}
			}
			FixPathDelim(m_VolumeViewName);
			strlwr(m_VolumeViewName);
			FixPathDelim(path);
			strlwr(path);
			#if Copy_files_on_saving
			sptr = strrchr(m_VolumeViewName, '/') + 1;
			strcat(path, sptr);
			if(!_access(m_VolumeViewName, 0))
				{CopyFile(m_VolumeViewName, path, true);}
			tvolume = sptr;
			#else
			tvolume = strstr(m_VolumeViewName, path);
			if(tvolume)
				{tvolume += strlen(path);}//тут и есть наше выражение
			else
			{
				tvolume = m_VolumeViewName;//если файл левый,
				if(tvolume)//то селявя
				{
					char msg[500] = {0};
					#ifdef ALTERNATIVE_LANG
						sprintf(msg, "For a PLY-file %s \"%s\" the absolute path is used\r\n%s", ((sub) ? (m_LODView) ? "lod" : "sub. mesh" : "bones"), ((sub) ? m_parent -> m_Name : m_Name), m_VolumeViewName);
					#else
						sprintf(msg, "Для PLY-файла %s \"%s\" используется абсолютный путь\r\n%s", ((sub) ? (m_LODView) ? "лода" : "суб. мэша" : "кости"), ((sub) ? m_parent -> m_Name : m_Name), m_VolumeViewName);
					#endif
					MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "WARNING: CBone::WriteMdl", MB_ICONEXCLAMATION | MB_OK);
				}
			}
			#endif
		}
		if(!tvolume || _access(m_VolumeViewName, 0))
		{
			tvolume = NULL;
			char msg[512] = {0};
			#ifdef ALTERNATIVE_LANG
				sprintf(msg, "No access to the PLY-file %s \"%s\".\r\nThe file is blocked or has an incorrect path.\r\nUnlock/fix the location of the file and reattach it to the bone\r\n\r\n%s", ((sub) ? "sub-mesh" : "bones"), ((sub) ? m_parent -> m_Name : m_Name), m_VolumeViewName);
			#else
				sprintf(msg, "Нет доступа к PLY-файлу %s \"%s\".\r\nФайл заблокирован или имеет неверный путь.\r\nРазблокируйте/исправьте расположение файла и заново прикрепите его к кости\r\n\r\n%s", ((sub) ? "суб-мэша" : "кости"), ((sub) ? m_parent -> m_Name : m_Name), m_VolumeViewName);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "ERROR: CBone::WriteMdl", MB_ICONHAND | MB_OK);
		}
		if(tvolume)
		{
			if(m_LODView && !pDoc -> Lod_writer)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format("%*s{LODView\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
				else
				fprintf(fp, "%*s{LODView\n", (4 * indent), "");
				pDoc -> Lod_writer = true;
			}
			if(m_LODView)
				{indent++;}
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{VolumeView \"%s\"", (4 * indent), "", tvolume); pDoc -> Code_block += tmp_str;}
			else
			fprintf(fp, "%*s{VolumeView \"%s\"", (4 * indent), "", tvolume);
			if(m_NoCastShadows)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {NoCastShadows}"); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {NoCastShadows}");}
			}
			if(m_DecalTarget)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {DecalTarget}"); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {DecalTarget}");}
			}
			if(m_NoGroupMesh)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {NoGroupMesh}"); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {NoGroupMesh}");}
			}
			if(m_NoGetShadows)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {NoGetShadows}"); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {NoGetShadows}");}
			}
			if(m_Ground)
			{
				if(pDoc -> Copy_Mode)
					{tmp_str.Format(" {Ground}"); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, " {Ground}");}
			}
			if(m_Layer > -1)
			{
				if(m_LODView)
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format(" {Layer %d}", m_Layer); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, " {Layer %d}", m_Layer);}
				}
				else
				{
					CString st = "";
					switch(m_Layer)
					{
						case 0:
							{st = "WaterScissor";}
						break;
					};
					if(pDoc -> Copy_Mode)
						{tmp_str.Format(" {Layer %s}", st); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, " {Layer %s}", st);}
				}
			}
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("}\n"); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "}\n");}
			if(m_LODView && pDoc -> Lod_writer && !sub && m_ChildFirst && !m_ChildFirst -> sub)
			{
				if(m_LOD_OFF)
				{
					if(pDoc -> Copy_Mode)
						{tmp_str.Format("%*s{OFF}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
					else
						{fprintf(fp, "%*s{OFF}\n", (4 * indent), "");}
				}
				indent--;
				if(pDoc -> Copy_Mode)
					{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
				else
					{fprintf(fp, "%*s}\n", (4 * indent), "");}
				pDoc -> Lod_writer = false;
			}
			else
			{
				if(m_LODView)
					{indent--;}
			}
		}
	}
	// Write out any child bones here
	if(((child = m_ChildFirst) != NULL) && pDoc -> Copy_Mode != 1)
	{
		while(child)
		{
			child -> WriteMdl(fp, indent);
			child = child -> next;
		}
	}
	if((pDoc -> Lod_writer && !next) || (pDoc -> Lod_writer && next && (!next -> m_LODView || !next -> sub)))
	{
		indent++;
		if(m_LOD_OFF || m_parent -> m_LOD_OFF)
		{
			if(pDoc -> Copy_Mode)
				{tmp_str.Format("%*s{OFF}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
			else
				{fprintf(fp, "%*s{OFF}\n", (4 * indent), "");}
		}
		indent--;
		pDoc -> Lod_writer = false;
		if(pDoc -> Copy_Mode)
			{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, "%*s}\n", (4 * indent), "");}
	}
	indent--;
	if(!sub)
	{
		if(pDoc -> Copy_Mode)
			{tmp_str.Format("%*s}\n", (4 * indent), ""); pDoc -> Code_block += tmp_str;}
		else
			{fprintf(fp, "%*s}\n", (4 * indent), "");}
	}
}

void CBone::AddPly(char *FName)
{
	if(m_VolumeView)
		{return;}
    char *c, lpath[_MAX_PATH];
    char *tpath = GetModelPath();
    strcpy(lpath, tpath);
    if(strlen(tpath) && m_VolumeViewName[1] != ':')
	{
		c = strrchr(lpath, '/');
		if(c)
		{
			if(m_VolumeViewName[0] != '/')
               {c++;}
            *c = '\0';
            strcat(lpath, m_VolumeViewName);
            delete[] m_VolumeViewName;
            m_VolumeViewName = NULL;
            m_VolumeViewName = new char[strlen(lpath)+1];
            strcpy(m_VolumeViewName, lpath);
		}
	}
	FILE *FPlyTest = NULL;
	FPlyTest = fopen(m_VolumeViewName, "rb");
	if(!FPlyTest)
	{
		CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
		char PathTest[MAX_PATH] = {0};
		int skip_item = -1;
		char independent_path[_MAX_PATH] = {0};
		bool ply_test = false, find_mask = false;
		strcpy(PathTest, m_VolumeViewName);
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
			for(int path_search = 5; path_search > -1 && !ply_test; path_search--)
			{
				if(skip_item >= 0 && path_search == skip_item)
					{continue;}
				if(!strlen(pWnd -> m_ResPath[path_search]))
					{continue;}
				sprintf(PathTest, "%s%s", pWnd -> m_ResPath[path_search], independent_path);
				if(!(FPlyTest = fopen(PathTest, "r")))
					{continue;}
				else
				{
					ply_test = true;
					fclose(FPlyTest);
					FPlyTest = NULL;
					delete[] m_VolumeViewName;
					m_VolumeViewName = NULL;
					m_VolumeViewName = new char[strlen(PathTest) + 1];
					strcpy(m_VolumeViewName, PathTest);
				}
			}
		}
		if(!find_mask || !ply_test)
		{
			char ErrMsg[512] = {0};
			char num[10] = {0};
			char *sptr = ((sub) ? m_parent -> m_Name : m_Name);
			if(*sptr == '"')
				{sptr++;}
			if(sub)
			{
				#ifdef ALTERNATIVE_LANG
					sprintf(num, "(Lod-%d)", sub);
				#else
					sprintf(num, "(Лод-%d)", sub);
				#endif
			}
			#ifdef ALTERNATIVE_LANG
				sprintf(ErrMsg, "PLY-file %s%s \"%s\" not detected:\n%s", ((sub) ? (m_LODView) ? "lod " : "sub. mesh " : "bones "), num, sptr, FName);
			#else
				sprintf(ErrMsg, "PLY-файл %s%s \"%s\" не обнаружен:\n%s", ((sub) ? (m_LODView) ? "лода " : "суб. мэша " : "кости "), num, sptr, FName);
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, ErrMsg, "ERROR: CBone::AddPly", MB_ICONHAND);
			return;
		}
	}
	else
		{fclose(FPlyTest);}
    m_VolumeView = new CPly(m_VolumeViewName);
	if(!m_VolumeView -> loading_successes)
	{
		delete m_VolumeView;
		m_VolumeView = NULL;
	}
}