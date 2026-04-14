// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "CAnimFrame.h"
#include "MainFrm.h"

CAnimSub::CAnimSub()
{
	memset((matrix34_t*)&m_Matrix34, 0, sizeof(matrix34_t));
	m_Bone = 0;
	m_Visible = -1;
	m_next = NULL;
	m_prev = NULL;
	chunk_flags = 0;
}

CAnimSub::~CAnimSub()
{
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
}

CAnimFrame::CAnimFrame()
{
	memset((matrix34_t*)&m_Matrix34, 0, sizeof(matrix34_t));
	m_Bone = 0;
	m_head = NULL;
	m_tail = NULL;
}

CAnimSub* CAnimFrame::AddSub()
{
	if (!m_head)
	{
		m_head = new CAnimSub();
		m_tail = m_head;
	}
	else
	{
		CAnimSub* t_sub = new CAnimSub();
		m_tail->m_next = t_sub;
		t_sub->m_prev = m_tail;
		m_tail = t_sub;
	}
	return m_tail;
}

CAnimSub* CAnimFrame::FindSub(int bone)
{
	if (!this)
	{
		return NULL;
	}
	CAnimSub* t_sub = m_head;
	while (t_sub)
	{
		if (t_sub->m_Bone == bone)
		{
			return t_sub;
		}
		t_sub = t_sub->m_next;
	}
	return NULL;
}

CAnimBone::CAnimBone()
{
	m_BoneCnt = 0;
	m_BoneMap = NULL;
	m_FrameCnt = 0;
	m_Frames = NULL;
	Header_ID = 0;
}

CAnimFrame::~CAnimFrame()
{
	while (m_head)
	{
		CAnimSub* t_sub = m_head->m_next;
		delete m_head;
		m_head = NULL;
		m_head = t_sub;
	}
	m_tail = NULL;
}

CAnimBone::CAnimBone(char* szFile, bool& b)
{
	m_BoneCnt = 0;
	m_BoneMap = NULL;
	m_FrameCnt = 0;
	m_Frames = NULL;
	FILE* fp = fopen(szFile, "rb");
	if (fp)
	{
		b = Process(fp);
	}
	fclose(fp);
}

CAnimBone::~CAnimBone()
{
	if (m_BoneMap)
	{
		for (int i = 0; i < m_BoneCnt; i++)
		{
			delete[] m_BoneMap[i];
			m_BoneMap[i] = NULL;
		}
		delete[] m_BoneMap;
		m_BoneMap = NULL;
	}
	if (m_Frames)
	{
		delete[] m_Frames;
		m_Frames = NULL;
	}
}

char* anm_taglist[] = { "FRMS", "BMAP", "FRMN", "BONE", "MATR", "VISI", "FRM2", "MESH", 0 };

bool CAnimBone::Process(FILE* fp)
{
	char tag[5] = { 0 };
	int bone = 0, tagindx = 0, frame = 0, counter_bytes = 0, * bone_vertex_size = NULL, chunk_index_bone = 0;
	int first_vertex = 0, vertex_size = 0;
	short vertex_count = 0, has_bbox = 0, chunk_flags = 0;
	bool skip = false, invert = false;
	BYTE keyframe_chunks = 0;
	CAnimSub* t_sub = NULL;
	fread(tag, 1, 4, fp);
	if (strnicmp("EANM", tag, 4))
	{
		return false;
	} // This is not a valid ANM file
	fread(&Header_ID, 1, sizeof(int), fp);
	if (Header_ID != 0x00030000 && Header_ID != 0x00040000 && Header_ID != 0x00050000 && Header_ID != 0x00060000 && Header_ID != 0x00060001)
	{
		return false;
	}
	fread(tag, 1, 4, fp);
	if (strnicmp("FRMS", tag, 4))
	{
		return false;
	}
	fread(&m_FrameCnt, 1, sizeof(int), fp);
	fread(tag, 1, 4, fp);
	if (strnicmp("BMAP", tag, 4))
	{
		return false;
	}
	m_Frames = new CAnimFrame[m_FrameCnt];
	fread(&m_BoneCnt, 1, sizeof(int), fp);
	m_BoneMap = new char* [m_BoneCnt];
	for (bone = 0; bone < m_BoneCnt; bone++)
	{
		int namesize;
		fread(&namesize, 1, sizeof(int), fp);
		m_BoneMap[bone] = new char[namesize + 1];
		fread(m_BoneMap[bone], 1, namesize, fp);
		m_BoneMap[bone][namesize] = '\0';
	}
	bone_vertex_size = new int[m_BoneCnt];
	memset(bone_vertex_size, 0, m_BoneCnt * sizeof(int));
	while (fread(tag, 1, 4, fp) != NULL)
	{
		tag[4] = '\0';
		skip = false;
		for (tagindx = 0; anm_taglist[tagindx] && !skip; tagindx++)
		{
			if (!strnicmp((char*)tag, anm_taglist[tagindx], 4))
			{
				switch (tagindx)
				{
				case 2: // FRMN - Frame Number -- this is the first part of a frame
				{ fread(&frame, 1, sizeof(int), fp); skip = true; }
				break;
				case 3: // BONE - Bone index -- the index of the name of the bone affected by this frame
				{
					t_sub = m_Frames[frame].AddSub();
					fread(&t_sub->m_Bone, 1, sizeof(int), fp);
					chunk_index_bone = t_sub->m_Bone;
					skip = true;
				}
				break;
				case 4: // MATR - Matrix34 that describes the motion for this frame
				{
					fread(&t_sub->m_Matrix34, 1, sizeof(matrix34_t), fp);
					skip = true;
					t_sub->chunk_flags |= B_POSITION | B_ORIENTATION;
				}
				break;
				case 5: // VISI - Is the bone visible?
				{
					fread(&t_sub->m_Visible, 1, sizeof(int), fp);
					if (t_sub->m_Visible)
					{
						t_sub->chunk_flags |= B_VISIBLE_ON;
					}
					else
					{
						t_sub->chunk_flags |= B_VISIBLE_OFF;
					}
					skip = true;
				}
				break;
				case 7: //MESH yat`...
				{
					if (true)
					{
#ifdef ALTERNATIVE_LANG
						MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "The animation file contains data about the mesh.\nThis is not supported by the program ;..(", "ERROR: CAnimBone::Process", MB_ICONHAND);
#else
						MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "���� �������� �������� ������ � ����.\n��� �� �������������� ���������� ;..(", "ERROR: CAnimBone::Process", MB_ICONHAND);
#endif
						delete[] bone_vertex_size;
						delete[] m_Frames;
						m_Frames = NULL;
						return false;
					}
					skip = true;
					fread(&counter_bytes, 1, sizeof(int), fp);
					if (bone_vertex_size[chunk_index_bone] == 0)
					{
						fread(&first_vertex, 1, sizeof(int), fp);
						fread(&vertex_count, 1, sizeof(int), fp);
						fread(&has_bbox, 1, sizeof(int), fp);
						//Calculate vertex size
						if (has_bbox)
						{
							vertex_size = int((counter_bytes - 40) / vertex_count);
						}
						else
						{
							vertex_size = int((counter_bytes - 16) / vertex_count);
						}
						counter_bytes -= 8;
					}
					//MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, "!!!", "111", MB_ICONHAND);
					//...
					//...
					//...
					//to be continued
				}
				break;
				case 6: //FRM2
				{
					fread(&frame, 1, sizeof(short), fp);
					fread(&keyframe_chunks, 1, sizeof(char), fp);
					for (int j = 0; j < keyframe_chunks; j++)
					{
						t_sub = m_Frames[frame].AddSub();
						fread(&t_sub->m_Bone, 1, sizeof(char), fp);
						fread(&chunk_flags, 1, sizeof(short), fp);
						float ori[9];
						v3_t pos, quant;
						float w = 0;
						memset(&ori, 0, sizeof(float) * 9);
						memset(&pos, 0, sizeof(v3_t));
						memset(&quant, 0, sizeof(v3_t));
						if (chunk_flags & B_POSITION)
						{
							fread(&pos, 1, sizeof(v3_t), fp);
						}
						if (chunk_flags & B_ORIENTATION)
						{
							fread(&quant, 1, sizeof(v3_t), fp);
							w = (float)sqrt(fabs(1.0 - ((quant[0] * quant[0]) + (quant[1] * quant[1]) + (quant[2] * quant[2]))));
							w = -w;
							if (chunk_flags & B_LEFT_HANDED)
							{
								invert = true;
							}
							float r2 = w * w + quant[0] * quant[0] + quant[1] * quant[1] + quant[2] * quant[2];
							float s = 2 / r2;
							float xs = quant[0] * s;  float ys = quant[1] * s;  float zs = quant[2] * s;
							float xx = quant[0] * xs; float xy = quant[0] * ys; float xz = quant[0] * zs;
							float xw = w * xs; float yy = quant[1] * ys; float yz = quant[1] * zs;
							float yw = w * ys; float zz = quant[2] * zs; float zw = w * zs;
							ori[0] = 1 - (yy + zz);
							ori[1] = (xy - zw);
							ori[2] = (xz + yw);
							ori[3] = (xy + zw);
							ori[4] = 1 - (xx + zz);
							ori[5] = (yz - xw);
							ori[6] = (xz - yw);
							ori[7] = (yz + xw);
							ori[8] = 1 - (xx + yy);
						}
						t_sub->m_Visible = -1;
						if (chunk_flags & B_VISIBLE_ON)
						{
							t_sub->m_Visible = 1;
						}
						if (chunk_flags & B_VISIBLE_OFF)
						{
							t_sub->m_Visible = 0;
						}
						memcpy(&t_sub->m_Matrix34, &ori, sizeof(float) * 9);
						t_sub->m_Matrix34.v[3][0] = pos[0];
						t_sub->m_Matrix34.v[3][1] = pos[1];
						t_sub->m_Matrix34.v[3][2] = pos[2];
						t_sub->chunk_flags = chunk_flags;
						if (chunk_flags & B_MESH)
						{
#ifdef ALTERNATIVE_LANG
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "The animation file contains data about the mesh.\nThis is not supported by the program ;..(", "ERROR: CAnimBone::Process", MB_ICONHAND);
#else
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "���� �������� �������� ������ � ����.\n��� �� �������������� ���������� ;..(", "ERROR: CAnimBone::Process", MB_ICONHAND);
#endif
							delete[] bone_vertex_size;
							delete[] m_Frames;
							m_Frames = NULL;
							return false;
						}
						if (chunk_flags > 63)
						{
#ifdef ALTERNATIVE_LANG
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "Unidentified flag!!!", "ERROR: CAnimBone::Process", MB_ICONHAND);
#else
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "������������ ����!!!", "ERROR: CAnimBone::Process", MB_ICONHAND);
#endif
							delete[] bone_vertex_size;
							delete[] m_Frames;
							m_Frames = NULL;
							return false;
						}
					}
					skip = true;
				}
				break;
				default:// Found an unknown tag -- need to handle this
				{ TRACE("Found an unknown tag [%s] in animation file.", tag); skip = true; }
				break;
				}
			}
		}
	}
	delete[] bone_vertex_size;
	bone_vertex_size = NULL;
	//����� ������� ����� ��� ����������
	int CurFrm = 0, Seeker = 0;
	CAnimSub* new_sub = NULL, * find_sub = NULL, * rep_sub;
	for (int CurBone = 0; CurBone < m_BoneCnt; CurBone++)
	{
		find_sub = NULL;
		if (find_sub = m_Frames[0].FindSub(CurBone))
		{
			if (!(find_sub->chunk_flags & B_ORIENTATION))
			{
				bool orientation_restored = false;
				for (int seek_frame = 1; seek_frame < m_FrameCnt; seek_frame++)
				{
					CAnimSub* next_sub = m_Frames[seek_frame].FindSub(CurBone);
					if (!next_sub)
					{
						continue;
					}
					if (next_sub->chunk_flags & B_ORIENTATION)
					{
						memcpy(&find_sub->m_Matrix34, &next_sub->m_Matrix34, sizeof(float) * 9);
						orientation_restored = true;
						break;
					}
				}
				if (!orientation_restored)
				{
					find_sub->m_Matrix34.v[0][0] = 1;
					find_sub->m_Matrix34.v[1][1] = 1;
					find_sub->m_Matrix34.v[2][2] = 1;
				}
				find_sub->chunk_flags |= B_ORIENTATION;
			}
		}
	}
	//���������� ����������� ���������
	for (int CurBone = 0; CurBone < m_BoneCnt; CurBone++)
	{
		for (CurFrm = 0; CurFrm < m_FrameCnt; CurFrm++)
		{
			new_sub = NULL;
			find_sub = NULL;
			Seeker = CurFrm;
			char subcopy = 0;// 0 - ���������� �� ���������, 1 - ������ ����������� (�� �����������), 2 - ��������� �����������.
			while (true)
			{
				if (Seeker < 0)
				{
					break;
				}
				if (find_sub = m_Frames[Seeker].FindSub(CurBone))
				{
					if (!(find_sub->chunk_flags & B_POSITION) || !(find_sub->chunk_flags & B_ORIENTATION))
					{
						new_sub = find_sub;
						Seeker--;
						subcopy = 2;
						continue;
					}
					if (subcopy)//�� Seeker � CurFrm
					{
						if (subcopy == 1)
						{
							new_sub = m_Frames[CurFrm].AddSub();
							new_sub->m_Bone = CurBone;
							new_sub->m_Visible = find_sub->m_Visible;
						}
						bool full_copy = true;
						if (!(new_sub->chunk_flags & B_POSITION))
						{
							if (find_sub->chunk_flags & B_POSITION)
							{
								new_sub->m_Matrix34.v[3][0] = find_sub->m_Matrix34.v[3][0];
								new_sub->m_Matrix34.v[3][1] = find_sub->m_Matrix34.v[3][1];
								new_sub->m_Matrix34.v[3][2] = find_sub->m_Matrix34.v[3][2];
								new_sub->chunk_flags |= B_POSITION;
							}
							else
							{
								full_copy = false;
							}
						}
						if (!(new_sub->chunk_flags & B_ORIENTATION))
						{
							if (find_sub->chunk_flags & B_ORIENTATION)
							{
								memcpy(&new_sub->m_Matrix34, &find_sub->m_Matrix34, sizeof(float) * 9);
								new_sub->chunk_flags |= B_ORIENTATION;
								if (find_sub->chunk_flags & B_LEFT_HANDED)
								{
									new_sub->chunk_flags |= B_LEFT_HANDED;
								}
							}
							else
							{
								full_copy = false; Seeker--;
							}
						}
						if (!full_copy)
						{
							subcopy = 2;
						}
						else
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					subcopy = 1; Seeker--;
				}
			}
		}
	}
	//������ visible
	for (int CurBone = 0; CurBone < m_BoneCnt; CurBone++)
	{
		for (CurFrm = 0; CurFrm < m_FrameCnt; CurFrm++)
		{
			bool subcopy = false;
			Seeker = CurFrm;
			while (true)
			{
				if (Seeker < 0)
				{
					break;
				}
				if (find_sub = m_Frames[Seeker].FindSub(CurBone))
				{
					if (find_sub->m_Visible == -1)
					{
						rep_sub = find_sub;
						Seeker--;
						subcopy = true;
						continue;
					}
					if (subcopy)
					{
						rep_sub->m_Visible = find_sub->m_Visible;
					}
					break;
				}
				else
				{
					Seeker--;
				}
			}
		}
	}
	if (invert)//������ ��������� ���������� �� ������ ����� �������
	{
		CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
		CSOEditDoc* pDoc = (CSOEditDoc*)pWnd->GetActiveDocument();
		pDoc->fix_mx_ori = true;
	}
	return true;
}

void CAnimBone::ProcessSave(FILE* fp)
{
	int anmver = 0x00040000, BoneIndx = 0, CurFrame = 0, OldFrame = 0;
	bool FrameRecorded = false;
	fwrite("EANM", 4, 1, fp);
	fwrite((int*)&anmver, sizeof(int), 1, fp);
	fwrite("FRMS", 4, 1, fp);
	fwrite((int*)&m_FrameCnt, sizeof(int), 1, fp);
	fwrite("BMAP", 4, 1, fp);
	fwrite((int*)&m_BoneCnt, sizeof(int), 1, fp);
	for (BoneIndx = 0; BoneIndx < m_BoneCnt; BoneIndx++)
	{
		int namesize = strlen(m_BoneMap[BoneIndx]);
		fwrite((int*)&namesize, sizeof(int), 1, fp);//� ������ ��� PLY-������ ��� char??
		fwrite(m_BoneMap[BoneIndx], namesize, 1, fp);
	}
	//������ ���� ����������
	fwrite("FRMN", 4, 1, fp);
	fwrite((int*)&CurFrame, sizeof(int), 1, fp);
	for (BoneIndx = 0; BoneIndx < m_BoneCnt; BoneIndx++)
	{
		CAnimSub* pSub = m_Frames[CurFrame].m_head;
		while (pSub)
		{
			if (BoneIndx == pSub->m_Bone)
			{
				fwrite("BONE", 4, 1, fp);
				fwrite((int*)&BoneIndx, sizeof(int), 1, fp);
				fwrite("MATR", 4, 1, fp);
				fwrite(&pSub->m_Matrix34, sizeof(matrix34_t), 1, fp);
				fwrite("VISI", 4, 1, fp);
				int cv = (pSub->m_Visible) ? 1 : 0;
				fwrite((int*)&cv, 1, sizeof(int), fp);
				break;
			}
			pSub = pSub->m_next;
		}
	}
	for (CurFrame++; CurFrame < m_FrameCnt; CurFrame++, OldFrame++)
	{
		FrameRecorded = false;
		CAnimSub* pSubCur = m_Frames[CurFrame].m_head;
		while (pSubCur)
		{
			CAnimSub* pSubOld = m_Frames[OldFrame].m_head;
			while (pSubOld)
			{
				if (pSubCur->m_Bone == pSubOld->m_Bone)
				{
					bool mtx = false, vis = false;
					if (memcmp(&pSubCur->m_Matrix34, &pSubOld->m_Matrix34, sizeof(matrix34_t)))
					{
						mtx = true;
					}
					int cv = (pSubCur->m_Visible) ? 1 : 0;
					int ov = (pSubOld->m_Visible) ? 1 : 0;
					if (cv != ov)
					{
						vis = true;
					}
					if (mtx || vis)
					{
						if (!FrameRecorded)
						{
							fwrite("FRMN", 4, 1, fp);
							fwrite((int*)&CurFrame, sizeof(int), 1, fp);
							FrameRecorded = true;
						}
						fwrite("BONE", 4, 1, fp);
						fwrite((int*)&pSubCur->m_Bone, sizeof(int), 1, fp);
						if (mtx)
						{
							fwrite("MATR", 4, 1, fp);
							fwrite(&pSubCur->m_Matrix34, sizeof(matrix34_t), 1, fp);
						}
						if (vis)
						{
							fwrite("VISI", 4, 1, fp);
							fwrite((int*)&cv, 1, sizeof(int), fp);
						}
					}
					break;
				}
				pSubOld = pSubOld->m_next;
			}
			pSubCur = pSubCur->m_next;
		}
	}
}

void CAnimBone::BoneAdd(char* Name, matrix34_t* Matrix34, int Vis)
{
	int NBone = 0;//������ ����� �����
	if (!m_BoneMap)//���� ������ ���
	{
		m_BoneMap = new char* [1];
		m_BoneMap[0] = NULL;
		m_BoneCnt++;
	}
	else
	{
		NBone = m_BoneCnt;
		char** NewBoneMap = NULL;
		NewBoneMap = new char* [NBone + 1];
		memset(NewBoneMap, 0, sizeof(char*) * (NBone + 1));
		for (int i = 0; i < m_BoneCnt; i++)
		{
			NewBoneMap[i] = m_BoneMap[i];
		}
		memset(m_BoneMap, 0, sizeof(char*) * m_BoneCnt);
		delete m_BoneMap;
		m_BoneMap = NewBoneMap;
		m_BoneCnt++;
	}
	m_BoneMap[NBone] = new char[strlen(Name) + 1];
	strcpy(m_BoneMap[NBone], Name);
	CAnimSub* t_sub = NULL;
	int n = 0;
	if (!m_FrameCnt)//���� ������ ���
	{
		m_Frames = new CAnimFrame[1];
	}
	do//��������� ����� �� ��� �����
	{
		t_sub = m_Frames[n].AddSub();
		t_sub->m_Bone = NBone;
		memcpy(&t_sub->m_Matrix34, Matrix34, sizeof(matrix34_t));
		t_sub->chunk_flags = B_POSITION | B_ORIENTATION;
		t_sub->m_Visible = (Vis) ? 1 : 0;
		if (t_sub->m_Visible)
		{
			t_sub->chunk_flags |= B_VISIBLE_ON;
		}
		else
		{
			t_sub->chunk_flags |= B_VISIBLE_OFF;
		}
		n++;
	} while (n < m_FrameCnt);
}

void CAnimBone::BoneDelete(int indx)
{
	CAnimSub* pSub = NULL;
	for (int i = 0; i < m_FrameCnt; i++)
	{
		pSub = m_Frames[i].m_head;
		while (pSub)
		{
			if (pSub->m_Bone == indx)
			{
				break;
			}
			pSub = pSub->m_next;
		}
		if (!pSub)
		{
			continue;
		}
		if (pSub->m_next && pSub->m_prev)
		{
			pSub->m_next->m_prev = pSub->m_prev;
			pSub->m_prev->m_next = pSub->m_next;
		}
		else
			if (pSub->m_next && !pSub->m_prev)
			{
				m_Frames[i].m_head = pSub->m_next;
				pSub->m_next->m_prev = NULL;
			}
			else
				if (!pSub->m_next && pSub->m_prev)
				{
					m_Frames[i].m_tail = pSub->m_prev;
					pSub->m_prev->m_next = NULL;
				}
				else
				{
					m_Frames[i].m_tail = NULL;
					m_Frames[i].m_head = NULL;
				}
		delete pSub;
		pSub = NULL;
	}
	int NCnt = m_BoneCnt - 1;
	char** NewBoneMap = NULL;
	NewBoneMap = new char* [NCnt];
	memset(NewBoneMap, 0, sizeof(char*) * NCnt);
	for (int i = 0, j = 0; i < m_BoneCnt; i++)
	{
		if (i == indx)
		{
			continue;
		}
		NewBoneMap[j] = m_BoneMap[i];
		j++;
	}
	delete[] m_BoneMap[indx];
	m_BoneMap[indx] = NULL;
	delete[] m_BoneMap;
	m_BoneMap = NULL;
	m_BoneMap = NewBoneMap;
	for (int i = 0; i < m_FrameCnt; i++)
	{
		pSub = m_Frames[i].m_head;
		while (pSub)
		{
			if (pSub->m_Bone > indx)
			{
				pSub->m_Bone--;
			}
			pSub = pSub->m_next;
		}
	}
	m_BoneCnt--;
}

int CAnimBone::FrameDelete(int indx)
{
	if (m_FrameCnt == 1)
	{
		return 0;
	}
	int NCnt = m_FrameCnt - 1;
	CAnimFrame* New_Frames = new CAnimFrame[NCnt];
	memset(New_Frames, 0, sizeof(CAnimFrame) * NCnt);
	for (int i = 0, j = 0; i < m_FrameCnt; i++)
	{
		if (i == indx)
		{
			continue;
		}
		New_Frames[j] = m_Frames[i];
		memset((CAnimFrame*)&m_Frames[i], 0, sizeof(CAnimFrame));
		j++;
	}
	delete[] m_Frames;
	m_Frames = NULL;
	m_Frames = New_Frames;
	if (m_FrameCnt > 1)
	{
		m_FrameCnt--;
	}
	if (indx >= m_FrameCnt)
	{
		indx = m_FrameCnt - 1;
	}
	return indx;
}

int CAnimBone::OnInsertFrame(int indx)
{
	CAnimSub* pSub = m_Frames[indx].m_head;
	int NCnt = m_FrameCnt + 1;
	CAnimFrame* New_Frames = new CAnimFrame[NCnt];
	memset(New_Frames, 0, sizeof(CAnimFrame) * NCnt);
	for (int i = 0, j = 0; i < NCnt; i++, j++)
	{
		if (i == indx + 1)
		{
			i++;
		}
		if (i < NCnt)
		{
			New_Frames[i] = m_Frames[j];
		}
	}
	int IdxNewFrame = indx + 1;
	CAnimSub* t_sub = NULL;
	while (pSub)
	{
		t_sub = New_Frames[IdxNewFrame].AddSub();
		t_sub->m_Bone = pSub->m_Bone;
		t_sub->chunk_flags = pSub->chunk_flags;
		t_sub->m_Visible = pSub->m_Visible;
		t_sub->m_Matrix34 = pSub->m_Matrix34;
		pSub = pSub->m_next;
	}
	memset(m_Frames, 0, sizeof(CAnimFrame) * m_FrameCnt);
	delete[] m_Frames;
	m_Frames = NULL;
	m_Frames = New_Frames;
	m_FrameCnt++;
	return IdxNewFrame;
}

int CAnimBone::OnAddClone(int indx)//������ �����
{
	int NBone = 0;//������ ����� �����
	if (!m_BoneMap)
	{
		return -1;
	}
	//��������� ���
	bool stop = false;
	char CloneBoneName[_MAX_PATH] = { 0 };
	for (int i = 0; i < 100 && !stop; i++)
	{
		memset(CloneBoneName, 0, _MAX_PATH);
		sprintf(CloneBoneName, "%s_Clone_%d", m_BoneMap[indx], i);
		stop = true;
		for (int i2 = 0; i2 < m_BoneCnt && stop; i2++)
		{
			if (!stricmp(CloneBoneName, m_BoneMap[i2]))
			{
				stop = false;
			}
		}
	}
	//���������� ������ �����
	NBone = m_BoneCnt;
	char** NewBoneMap = NULL;
	NewBoneMap = new char* [NBone + 1];
	memset(NewBoneMap, 0, sizeof(char*) * (NBone + 1));
	for (int i = 0; i < m_BoneCnt; i++)
	{
		NewBoneMap[i] = m_BoneMap[i];
	}
	memset(m_BoneMap, 0, sizeof(char*) * m_BoneCnt);
	delete m_BoneMap;
	m_BoneMap = NewBoneMap;
	m_BoneCnt++;
	m_BoneMap[NBone] = new char[strlen(CloneBoneName) + 1];
	strcpy(m_BoneMap[NBone], CloneBoneName);
	//��������� ����� �� ��� �����
	CAnimSub* t_sub = NULL;
	CAnimSub* pSub = NULL;
	int n = 0;
	do
	{
		t_sub = m_Frames[n].AddSub();
		t_sub->m_Bone = NBone;
		pSub = m_Frames[n].m_head;
		while (pSub)
		{
			if (pSub->m_Bone == indx)
			{
				break;
			}
			pSub = pSub->m_next;
		}
		memcpy(&t_sub->m_Matrix34, &pSub->m_Matrix34, sizeof(matrix34_t));
		t_sub->chunk_flags = B_POSITION | B_ORIENTATION;
		t_sub->m_Visible = (pSub->m_Visible) ? 1 : 0;
		if (t_sub->m_Visible)
		{
			t_sub->chunk_flags |= B_VISIBLE_ON;
		}
		else
		{
			t_sub->chunk_flags |= B_VISIBLE_OFF;
		}
		n++;
	} while (n < m_FrameCnt);
	return NBone;
}

void CAnimBone::ReverseFrames(void)
{
	if (!m_Frames || m_FrameCnt <= 1)
	{
		return;
	}
	// Swap frames from both ends toward the middle.
	// We only transfer the linked-list head/tail pointers (shallow swap),
	// then NULL out the originals so no double-free occurs.
	// This avoids the previous bug where memset() was called on a source
	// slot whose pointers had already been shallow-copied into NewFrames,
	// corrupting the list when i and (m_FrameCnt-1-i) met in the middle.
	int lo = 0, hi = m_FrameCnt - 1;
	while (lo < hi)
	{
		// Swap head/tail pointers between frames[lo] and frames[hi]
		CAnimSub* tmpHead = m_Frames[lo].m_head;
		CAnimSub* tmpTail = m_Frames[lo].m_tail;
		m_Frames[lo].m_head = m_Frames[hi].m_head;
		m_Frames[lo].m_tail = m_Frames[hi].m_tail;
		m_Frames[hi].m_head = tmpHead;
		m_Frames[hi].m_tail = tmpTail;
		lo++;
		hi--;
	}
}