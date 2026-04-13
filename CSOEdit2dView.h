// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_SOEDIT2DVIEW_H__314E1237_F31C_43FA_912B_87B916D6EEDA__INCLUDED_)
#define AFX_SOEDIT2DVIEW_H__314E1237_F31C_43FA_912B_87B916D6EEDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CCylinder.h"
#include "CObb2.h"
#include "EulerAngles.h"

class CSOEdit2dView : public CView
{
protected: // create from serialization only
	CSOEdit2dView();
	DECLARE_DYNCREATE(CSOEdit2dView)

// Attributes
public:
	CSOEditDoc* GetDocument();

    void  SetView(int viewmode, int indx);

    void  DrawObb2GL(CObb2 *obj);
    void  DrawBoxGL(v3_t box);
    void  DrawTrapGL(v3_t box);
    void  DrawCylGL(CCylinder *cyl);
    void  DrawVolumeGL(CBone *basis);
    void  DrawBoneGL(CBone *basis);
    void  DrawOriginGL(void);
    void  DrawGridGL(void);
	void DrawXYZLinesGL();

    void  FixBoneGeometry(CBone *bone);
    void  FixVolumeGeometry(CVolume *volume);

    void  RenderGL(CDC *pDC);
    BOOL  InitGL();

    void UpdateViewData(CPoint point);

    //CSOEditDoc *m_pDoc;


    int   m_LightMode;
	rmode_t m_DrawMode;
    bool  m_BackFace;
    int   m_BackDir;

    v3_t m_reference;

	bool YBlock, XBlock;

    GLfloat m_AmbientLight[4];
    GLfloat m_DiffuseLight[4];
    GLfloat m_Specular[4];
    GLfloat m_LightPos[4];
    GLfloat m_SpecRef[4];

    GLuint  m_SceneGround;
    camera_t  m_Camera;
    POINT     m_point;

    int  m_texCount;

private:
    GLsizei   m_glnWidth, m_glnHeight;
    GLdouble  m_gldAspect;
	int DocIndx;

    HWND      m_hWnd;
    HDC       m_hDC;
    HGLRC     m_hRC;
	CDC      *m_pDC;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSOEdit2dView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSOEdit2dView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

   int   m_Scale;

//   int   m_mode;

   CPoint m_Point;
   CPoint m_vPoint;

   int   m_xAdj, m_yAdj;

   int   m_x;
   int   m_y;

   int   m_glx;
   int   m_gly;
   int   m_glz;

   int   m_xaxis;
   int   m_yaxis;

   int   m_invx;
   int   m_invy;

   char *m_szCaption;

   int   m_View;

   bool  m_lButton;
   bool  m_mButton;
   bool  m_rButton;

   float m_scales[24];
   float full_mx[16];
 
   void  SetupView();

   void  UpdateSelected(CPoint point);
   EulerAngles Eul_FromHMatrix(HMatrix M, int order);
   bool MR(float f, float b);
   void PreMoveMatrix(matrix34_t * Move_Matrix34, v3_t Move_Translations, float * Move_mx);
   // My round

// Generated message map functions
protected:
	//{{AFX_MSG(CSOEdit2dView)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnViewFront();
	afx_msg void OnViewRight();
	afx_msg void OnViewTop();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewLeft();
	afx_msg void OnViewBottom();
	afx_msg void OnViewRear();
	afx_msg void OnViewCamera();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewBackfaces();
	afx_msg void OnViewShaded();
	afx_msg void OnViewWireframe();
	afx_msg void OnColorize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#ifndef _DEBUG  // debug version in edView.cpp
inline CSOEditDoc* CSOEdit2dView::GetDocument()
   { return (CSOEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSOEDIT2DVIEW_H__314E1237_F31C_43FA_912B_87B916D6EEDA__INCLUDED_)
