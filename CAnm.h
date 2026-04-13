#include "stdafx.h"
#include "general.h"

class CAnmBone
   {
    public:
        matrix34_t  m_Matrix;
        bool        m_Visi;
   };

class CAnmFrame
   {
    public:
       CAnmBone *m_Bones;
   };

class CAnm
   {
    
   };
