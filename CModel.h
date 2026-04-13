// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "stdio.h"
#include "general.h"

#include "CSkeleton.h"
#include "CVolume.h"
#include "CObb2.h"
#include "CSdl.h"


#ifndef _CMODEL_
#define _CMODEL_


class CModel
   {
    public:
        CModel();
        CModel(char *filename);
        ~CModel();

        void ParseFile();
  
        int            m_mode;
        CSdl          *m_sdl;

        v3_t           m_Orientation;
  
        CSkeleton     *m_skeleton;
        CVolumeList   *m_VolumeList;
        CObb2List     *m_ObstacleList;
        CObb2List     *m_AreaList;

        void AddSkeleton(void);
        void AddVolume(void);
        void AddObstacle(void);
        void AddArea(void);
        void WriteMdl(FILE *fp, int indent);

    private:
	   CString msg_st;
   };


#endif