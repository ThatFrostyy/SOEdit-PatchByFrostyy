#include "stdafx.h"

#include "CMatrix.h"

CMatrixStack::CMatrixStack()
   {
    m_next = NULL;

    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = 0.0f;
           }
       }
   }

CMatrixStack::~CMatrixStack()
   {
   }

CMatrixStack::Load(matrix44_t *matrix)
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = matrix->v[i][j];
           }
       }
   }

CMatrix::CMatrix()
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = 0.0f;
           }
       }
    
    m_stack = NULL;
   }

CMatrix::~CMatrix()
   {
   }

void CMatrix::LoadIdentity(void)
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[0][0] = 0.0f;
           }
       }
    m_matrix.v[0][0] = 1.0f;
    m_matrix.v[1][1] = 1.0f;
    m_matrix.v[2][2] = 1.0f;
    m_matrix.v[3][3] = 1.0f;
   }

// Multiply the current matrix by another CMatrix
void CMatrix::MultMatrix(CMatrix *matrix)
   {
    matrix44_t *result;
    int        i = 0, j = 0, k = 0;

    result = new matrix44_t;
    for (i = 0; i < 4; i++)
       {
        for (j = 0; j < 4; j++)
           {
            result->v[i][j] = 0.0f;
           }
       }
    
    for (i = 0; i < 4; i++)
       {
        for (j = 0; j < 4; j++)
           {
            for (k = 0; k < 4; k++)
               {
                result->v[i][j] += m_matrix.v[i][k] * matrix->m_matrix.v[k][j];
               }
           }
       }

    for (i = 0; i < 4; i++)
       {
        for (j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = result->v[i][j];
           }
       }
    delete result;
   }

void CMatrix::Load(CMatrix *matrix)
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = matrix->m_matrix.v[i][j];
           }
       }
   }

void CMatrix::Load(matrix34_t *matrix)
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 3; j++)
           {
            m_matrix.v[i][j] = matrix->v[i][j];
           }
       }
    m_matrix.v[0][3] = 0.0f;
    m_matrix.v[1][3] = 0.0f;
    m_matrix.v[2][3] = 0.0f;
    m_matrix.v[3][3] = 1.0f;
   }

void CMatrix::Load(matrix44_t *matrix)
   {
    for (int i = 0; i < 4; i++)
       {
        for (int j = 0; j < 4; j++)
           {
            m_matrix.v[i][j] = matrix->v[i][j];
           }
       }
   }

void CMatrix::Push()
   {
    CMatrixStack *pMatrix = new CMatrixStack();
    pMatrix->Load(&m_matrix);

    if (NULL == m_stack)
       {
        m_stack = pMatrix;
       }
    else
       {
        pMatrix->m_next = m_stack;
        m_stack = pMatrix;
       }
   }

void CMatrix::Pop()
   {
    if (NULL != m_stack)
       {
        Load(&m_stack->m_matrix);
        CMatrixStack *pMatrix = m_stack->m_next;
        delete m_stack;
        m_stack = pMatrix;
       }
   }

void CMatrix::Scale(v3_t vector)
   {
   }

void CMatrix::Rotate(v3_t vector)
   {
   }

void CMatrix::Translate(v3_t vector)
   {
    m_matrix.v[3][0] += vector[0];
    m_matrix.v[3][1] += vector[1];
    m_matrix.v[3][2] += vector[2];
   }
