// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "CCylinder.h"
#include "math.h"

CSection::CSection(int sides, float radius)
{
	m_sides = sides;
	m_xy = NULL;
	float fx = 0.0f, fy = radius;
	float degrees = (360.0f / (float)sides);
	float angle = degrees / (float)(180.0 / 3.1415927);
	m_xy = new v2_t[sides];
	for(int side = 0; side < sides; side++)
	{
		m_xy[side][0] = fx;
		m_xy[side][1] = fy;
		float tx = 0.0f, ty = 0.0f;
		tx = (float)(fx * cos(angle)) - (float)(fy * sin(angle));
		ty = (float)(fy * cos(angle)) + (float)(fx * sin(angle));
		fx = tx;
		fy = ty;
	}
}

CSection::~CSection()
{
	if(m_xy)
		{delete[] m_xy;}
	m_xy = NULL;
}

CCylinder::CCylinder(float radius, float length)
{
	m_section = NULL;
	m_sections = 0;
	m_sides = 12;
	m_verts = NULL;
	m_face = NULL;
	m_faces = 0;
	m_section = new CSection(m_sides, radius);
	m_sections = (((int)length + 5) / 5) + 1;
	float slength = length / ((float)m_sections - 1.0f);
	m_verts = (m_sections * m_sides) + 2;
	m_vert = new v3_t[m_verts];
	m_faces = (m_sections*m_sides * 2);
	m_face = new indx_t[m_faces];
	float sz = length / -2.0f;
	int s, v, i, j, x;
	//создаём вертексы
	for(s = 0; s < m_sections; s++)
	{
		v = (s * m_sides);
		for(i = 0; i < m_sides; i++)
		{
			m_vert[v + i][1] = m_section -> m_xy[i][0];
			m_vert[v + i][0] = m_section -> m_xy[i][1];
			m_vert[v + i][2] = sz;
		}
		sz += slength;
	}
	v = (m_sections * m_sides);
	m_vert[v][0] = 0.0f;
	m_vert[v][1] = 0.0f;
	m_vert[v][2] = length / -2.0f;
	m_vert[v + 1][0] = 0.0f;
	m_vert[v + 1][1] = 0.0f;
	m_vert[v + 1][2] = length / 2.0f;
	//--------------------------------------
	for(s = 0; s < (m_sections - 1); s++)
	{
		v = s * (m_sides * 2);
		for(i = 0; i < (m_sides - 1); i++)
		{
			m_face[v + i].v[0] = (s * m_sides) + i;
			m_face[v + i].v[1] = (s * m_sides) + i + 1;
			m_face[v + i].v[2] = ((s + 1) * m_sides) + i;
			x = 0;
		}
		m_face[v + i].v[2] = (s * m_sides) + i;
		m_face[v + i].v[1] = (s * m_sides);
		m_face[v + i].v[0] = ((s + 1) * m_sides);
		x = 0;
		for(i = m_sides, j = 0; i < ((m_sides * 2) - 1); i++, j++)
		{
			m_face[v + i].v[0] = ((s + 1) * m_sides) + j;
			m_face[v + i].v[1] = (s * m_sides) + j + 1;
			m_face[v + i].v[2] = ((s + 1) * m_sides) + j + 1;
			x = 0;
		}
		m_face[v + i].v[2] = (s * m_sides);
		m_face[v + i].v[1] = ((s + 1) * m_sides) + j;
		m_face[v + i].v[0] = ((s + 1) * m_sides);
		x = 0;
	}
	// Create the bottom cap triangles
	v = (m_sections - 1) * (m_sides * 2);
	for(i = 0; i < (m_sides - 1); i++)
	{
		m_face[v + i].v[2] = i;
		m_face[v + i].v[1] = i + 1;
		m_face[v + i].v[0] = (m_sections * m_sides);
		x = 0;
	}
	m_face[v + i].v[2] = i;
	m_face[v + i].v[1] = 0;
	m_face[v + i].v[0] = (m_sections * m_sides);
	x = 0;
	// Create the top cap triangles (maintain vertex winding order)
	v += m_sides;
	for(i = 0; i < (m_sides - 1); i++)
	{
		m_face[v + i].v[2] = ((m_sections - 1) * m_sides) + i + 1;
		m_face[v + i].v[1] = ((m_sections - 1) * m_sides) + i;
		m_face[v + i].v[0] = (m_sections * m_sides) + 1;
		x = 0;
	}
	m_face[v + i].v[0] = ((m_sections - 1) * m_sides) + i;
	m_face[v + i].v[1] = ((m_sections - 1) * m_sides);
	m_face[v + i].v[2] = (m_sections * m_sides) + 1;
	x = 0;
}

CCylinder::~CCylinder()
{
	if(m_section)
		{delete m_section;}
	m_section = NULL;
	if(m_vert)
	{
		delete[] m_vert;
		m_vert = NULL;
		m_verts = 0;
	}
	if(m_face)
	{
		delete[] m_face;
		m_face = NULL;
		m_faces = 0;
	}
}