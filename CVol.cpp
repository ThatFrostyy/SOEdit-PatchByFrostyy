// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// windows only
#include <conio.h>
#include "CMesh.h"
#include "CVol.h"

extern HWND m_hWnd;
char *vol_siglist[] = {"VERT", "INDX", "SIDE", 0};

CVol::CVol(char *volname)
{
	FixPathDelim(volname);
	FILE *m_fp = NULL;
	m_numverts = 0;
	m_vertlist = NULL;
	m_numpolys = 0;
	m_polylist = NULL;
	m_numsides = 0;
	m_sidelist = NULL;
	poly_count = 0;
	int i, sigindx;
	unsigned char sig[5];
	int vcount;
	if(volname)
		{strcpy(m_filepath, volname);}
	m_fp = fopen(volname, "rb");
	fread(sig, 1, 4, m_fp);
	if(strncmp((char *)sig, "EVLM", 4))
	{
		#ifdef ALTERNATIVE_LANG
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Invalid volume-file:\n" + CString(volname)), "ERROR: CVol::CVol", MB_ICONHAND);
		#else
			MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Недействительный волюм-файл:\n" + CString(volname)), "ERROR: CVol::CVol", MB_ICONHAND);
		#endif
		fclose(m_fp);
		m_fp = NULL;
		return;
	}
	while(fread(sig, 1, 4, m_fp) != NULL)
	{
		sig[4] = '\0';
		for(sigindx = 0; vol_siglist[sigindx]; sigindx++)
		{
			if(!stricmp((char *)sig, vol_siglist[sigindx]))
			{
				switch(sigindx)
				{
					case 0: // VERT
					{
						fread(&vcount, sizeof(int), 1, m_fp);
						m_numverts = vcount;
						m_vertlist = new vert_t[m_numverts];
						for(i = 0; i < vcount; i++)
							{fread(&m_vertlist[i].xyz, sizeof(v3_t), 1, m_fp);}
					}
					break;
					case 1: // INDX
					{
						fread(&vcount, sizeof(int), 1, m_fp);
						m_numpolys = vcount / 3;
						poly_count = vcount / 3;
						m_polylist = new indx_t[m_numpolys];
						for(i = 0; i < vcount / 3; i++)
						{
							short tindx[3];
							fread(tindx, sizeof(short), 3, m_fp);
							m_polylist[i].v[0] = tindx[0];
							m_polylist[i].v[1] = tindx[1];
							m_polylist[i].v[2] = tindx[2];
						}
					}
					break;
					case 2: // SIDE
					{
						fread(&vcount, sizeof(int), 1, m_fp);
						m_numsides = vcount;
						m_sidelist = new unsigned char[m_numsides];
						for(i = 0; i < vcount; i++)
							{fread(&m_sidelist[i], 1, 1, m_fp);}
					}
					break;
					default:
					{	
						#ifdef ALTERNATIVE_LANG
							MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Invalid file section found.", "ERROR: CVol::CVol", MB_ICONHAND);
						#else
							MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Найден недопустимый раздел файла.", "ERROR: CVol::CVol", MB_ICONHAND);
						#endif
						fclose(m_fp);
						return;
					}
					break;
				}
			}
			else
			if(!vol_siglist[sigindx])
			{
				#ifdef ALTERNATIVE_LANG
					MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Invalid file section found.", "ERROR: CVol::CVol", MB_ICONHAND);
				#else
					MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, "Найден недопустимый раздел файла.", "ERROR: CVol::CVol", MB_ICONHAND);
				#endif
				fclose(m_fp);
				return;
			}
		}
	}
	fclose(m_fp);
	return;
}

CVol::~CVol()
{
	if(m_vertlist)
		{delete[] m_vertlist;}
	if(m_polylist)
		{delete[] m_polylist;}
	if(m_sidelist)
		{delete[] m_sidelist;}
}