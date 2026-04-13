#include "stdafx.h"
#include "general.h"

#include "stdafx.h"
#include "general.h"

class CMatrixStack
   {
    public:
        CMatrixStack();
        ~CMatrixStack();

        CMatrixStack *m_next;
        Load(matrix44_t *matrix);

        matrix44_t    m_matrix;
   };

class CMatrix
   {
    public:
       CMatrix();
       ~CMatrix();

       void Push(void);
       void Pop(void);

       void LoadIdentity(void);

       void Load(matrix44_t *matrix);
       void Load(CMatrix *matrix);
       void Load(matrix34_t *matrix);

       void MultMatrix(CMatrix *matrix);

       void Scale(v3_t vector);
       void Rotate(v3_t vector);
       void Translate(v3_t vector);

       matrix44_t    m_matrix;
       CMatrixStack *m_stack;

    private:

   };