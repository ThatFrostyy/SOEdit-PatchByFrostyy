// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#if !defined(AFX_SOEDITVIEW_H__60C9EB93_97F9_4FBF_85C0_34D6D905D4D8__INCLUDED_)
#define AFX_SOEDITVIEW_H__60C9EB93_97F9_4FBF_85C0_34D6D905D4D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CBone.h"
#include <vector>

class CSOEditView : public CView
{
protected: // create from serialization only
	CSOEditView();
	DECLARE_DYNCREATE(CSOEditView)

   // Attributes
   public:
       CSOEditDoc* GetDocument();

       CSOEditDoc* m_pDoc;
	   CDC      *m_pDC;

       void LoadTexture(char *texfile, int texnumb, CMaterial *pMaterial);

       BOOL InitGL();
       BOOL ShutdownGL();

       GLfloat m_AmbientLight[4];
       GLfloat m_DiffuseLight[4];
       GLfloat m_Specular[4];
       GLfloat m_LightPos[4];
       GLfloat m_SpecRef[4];
       //GLfloat m_ClearColor[4];

       GLuint  m_SceneGround;
       camera_t  m_Camera;
       POINT     m_point;

	   bool  m_mButton;
	   bool  m_lButton;
	   bool  m_rButton;

       v3_t m_reference;
	   float m_Scale;
	   int m_View;
	   rlmode_t m_DrawMode;
	   View_mode_3d m_ViewMod;
	   std::vector <CString> SourseList;
	   bool m_grid, m_vol_wireframe, m_ViewEnts3dAsWireframe, m_ViewMeshAsWireframe, m_WireframeOverMesh, m_Paint_selected_meshes, m_ViewMeshVector;
	   GLfloat koef_light_diffuse;

    private:
       GLsizei   m_glnWidth, m_glnHeight;
       GLdouble  m_gldAspect;
	   GLdouble Near_View, Far_View;

       HWND      m_hWnd;
       HDC       m_hDC;
       HGLRC     m_hRC;

	   CString OverlayStr[20];

    // Operations
    public:
       void DrawBone(CBone *basis, bool transparency_mod);
	  // void DrawSkin(CBone * pSkin);
	   //void DrawChunk(CBone *basis);
       void DrawScene(void);
	   void RenderCollision();
	   void RenderVisual(bool transparency_mod);
	   void DrawCylGL(CCylinder *cyl);
	   void DrawVolumeLayer(CBone *basis);
	   void DrawBoxGL(v3_t box);
       void DrawTrapGL(v3_t box);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSOEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void anm_normalizer();
	void OnViewShaded();
	void OnViewTextured();
	void OnViewWireframe();
	void OnViewTexturesOff();
	void OnViewWireframeTexturesOff();
	void OnTexturesOff();
	void OnTexturesOn();
	void DrawVolumeGL(CBone *basis);
    void DrawGridGL(void);
	void DrawXYZLinesGL();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSOEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSOEditView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnColorize();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On3DCollisionsViewer();
	afx_msg void On3DVisual();
	afx_msg void On3DHybrid();
	afx_msg void OnView3dEntity();
	afx_msg void OnView3dBackgroundGrid();
	afx_msg void On3dCollisionAsWireframe();
	afx_msg void On3dEntityAsWireframe();
	afx_msg void On3dMeshsAsWireframe();
	afx_msg void OnView3dWireframeOverMesh();
	afx_msg void OnViewingPaintselectedmeshes();
	afx_msg void OnViewingDisplaythevectoroftheselectedmesh();
};

#ifndef _DEBUG  // debug version in SOEditView.cpp
inline CSOEditDoc* CSOEditView::GetDocument()
   { return (CSOEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOEDITVIEW_H__60C9EB93_97F9_4FBF_85C0_34D6D905D4D8__INCLUDED_)
