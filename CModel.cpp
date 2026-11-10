// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "general.h"
#include "CMesh.h"
#include "CBone.h"
#include "CModel.h"
#include "CVolume.h"

extern HWND m_hWnd;

CModel::CModel()
{
    // Need a "skeleton" here
    m_skeleton     = NULL;
    m_sdl          = NULL;
    m_ObstacleList = NULL;
    m_AreaList     = NULL;
    m_VolumeList   = NULL;
    m_Orientation[0] = 0.0f;
    m_Orientation[1] = 0.0f;
    m_Orientation[2] = 0.0f;
}

CModel::CModel(char *filename)
{
    // Need a "skeleton" here
    m_skeleton = NULL;
    m_Orientation[0] = 0.0f;
    m_Orientation[1] = 0.0f;
    m_Orientation[2] = 0.0f;
	if(filename)
		{m_sdl = new CSdl(filename);}
	else
		{m_sdl = NULL;}
    m_ObstacleList = new CObb2List("Obstacle", m_sdl);
    m_VolumeList = new CVolumeList(m_sdl);
    m_AreaList = new CObb2List("Area", m_sdl);
	if(m_sdl)
		{ParseFile();}
}

CModel::~CModel()
{
	if(m_skeleton)
		{delete m_skeleton; m_skeleton = NULL;}
	if(m_sdl)
		{delete m_sdl; m_sdl = NULL;}
	if(m_ObstacleList)
		{delete m_ObstacleList; m_ObstacleList = NULL;}
	if(m_VolumeList)
		{delete m_VolumeList; m_VolumeList = NULL;}
	if(m_AreaList)
		{delete m_AreaList; m_AreaList = NULL;}
}

void CModel::ParseFile()
{
    char *word;
	m_sdl -> read_comments = true;
	word = m_sdl -> GetNextWord();
	m_sdl -> read_comments = false;
    while(strlen(word))
	{
		if(!stricmp(word, "{"))// Beginning our processing block
			{}
		else
		if(!stricmp(word, "}"))// Ending our processing block
			{}
		else
		if(!stricmp("skeleton", word))
			{AddSkeleton();}
		else
		if(!stricmp("obstacle", word))
			{AddObstacle();}
		else
		if(!stricmp("area", word))
			{AddArea();}
		else
		if(!stricmp("volume", word))
			{AddVolume();}
		else
		{
			// Unknown top-level block (the game's extender camera/preview data, for example).
			// Keep it verbatim so it survives a save instead of being dropped or reported as an error.
			const char *buf = m_sdl -> GetBuffer();
			const char *start = m_sdl -> CursorPos();
			while(start > buf && *start != '{')
				{start--;}
			int depth = 1;
			while(depth > 0)
			{
				char *w = m_sdl -> GetNextWord();
				if(!strlen(w)) {break;}
				if(!strcmp(w, "{")) {depth++;}
				else if(!strcmp(w, "}")) {depth--;}
			}
			const char *end = m_sdl -> CursorPos();
			if(*start == '{' && end > start)
			{
				m_ExtraBlocks.Append(start, (int)(end - start));
				m_ExtraBlocks += "\n";
			}
		}
		word = m_sdl -> GetNextWord();
	}
}

void CModel::AddSkeleton(void)
	{m_skeleton = new CSkeleton(m_sdl);}

void CModel::AddObstacle(void)
	{m_ObstacleList -> AddToTail();}

void CModel::AddArea(void)
	{m_AreaList -> AddToTail();}

void CModel::AddVolume(void)
	{m_VolumeList -> AddToTail();}

void CModel::WriteMdl(FILE *fp, int indent)
{
    if(m_skeleton)
	{
        fprintf(fp, "{Skeleton\n");
        indent++;
        m_skeleton -> WriteMdl(fp, indent);
        indent--;
        fprintf(fp, "}\n");
	}
    if(m_ObstacleList)
		{m_ObstacleList -> WriteMdl(fp, indent);}
    if(m_AreaList)
		{m_AreaList -> WriteMdl(fp, indent);}
    if(m_VolumeList)
		{m_VolumeList -> WriteMdl(fp, indent);}
	if(!m_ExtraBlocks.IsEmpty())
		{fputs(m_ExtraBlocks, fp);}
}