// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "afxwin.h"
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

// windows only
#include <conio.h>
#include "CMesh.h"
#include "CPly.h"
#include "MainFrm.h"

extern HWND m_hWnd;


struct Tply_mesh_base
{
	DWORD fvf;
	DWORD first_face;
	DWORD face_count;
	DWORD flags;
};

CPly::CPly(char* plyname)
{
	int tex_count = 1;
	int i = 0, sigindx = 0, i2 = 0;
	char texname[4][_MAX_PATH] = { 0 }, textemp[_MAX_PATH] = { 0 }, * path = NULL;
	char text_msg_buff[2048] = { 0 };
	bool skip = false, CopyUV = false;
	BYTE  sig[5] = { 0 };
	Tply_mesh_base ply_mesh;
	memset(&ply_mesh, 0, sizeof(Tply_mesh_base));
	DWORD old_texcount = 0, new_texcount = 0, fvf_wo_texcount = 0;
	BOOL forced_bamp = FALSE;
	FILE* m_fp = NULL;
	loading_successes = false;
	m_bonelist = NULL;
	m_vertlist = NULL;
	m_meshlist = NULL;
	m_polylist = NULL;
	m_adjalist = NULL;
	m_shdwlist = NULL;
	memset(m_bbox, 0, sizeof(v3_t) * 2);
	memset(m_basis, 0, sizeof(v3_t) * 4);
	num_weights = 0, num_tex_coords = poly_count = m_bones = m_numverts = m_numpolys = m_numadjas = m_numshdws = m_mirror = unknown_data_size = 0;
	subskin_count = IndexType = 0;
	specular_rgba_color = GlFvf = GlFlags = 0;
	Bply = false;
	memset(subskin_bones, 0, MAX_PATH);
	has_pos = FALSE, has_rhw = FALSE, has_weights = FALSE, has_matrix_indices = FALSE, has_normal = FALSE, has_psize = FALSE, has_w = FALSE;
	has_diffuse = FALSE, has_specular = FALSE, has_tex_coords = FALSE, has_mesh_specular = FALSE, has_mesh_bump = FALSE;
	BAS2 = SHDW = ADJA = MROR = BNDS = SKINNED = FALSE;
	FixPathDelim(plyname);
	m_fp = fopen(plyname, "rb");
	if (!m_fp)
	{
		return;
	}
	m_meshlist = new CMeshList();
	fread(sig, 1, 4, m_fp);
	if (strncmp((char*)sig, "BPLY", 4) && strncmp((char*)sig, "EPLY", 4))
	{
#ifdef ALTERNATIVE_LANG
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Invalid mesh file:\n" + CString(plyname)), "ERROR: CPly::CPly", MB_ICONHAND);
#else
		MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Íåäåéñòâèòåëüíûé ìýø-ôàéë:\n" + CString(plyname)), "ERROR: CPly::CPly", MB_ICONHAND);
#endif
		fclose(m_fp);
		m_fp = NULL;
		return;
	}
	if (sig[0] == 'B')
	{
		tex_count = 2;
		Bply = true;
	}
	while (fread(sig, 1, 4, m_fp) != NULL)
	{
		sig[4] = '\0';
		skip = false;
		for (sigindx = 0; ply_siglist[sigindx] && !skip; sigindx++)
		{
			if (!stricmp((char*)sig, ply_siglist[sigindx]))
			{
				switch (sigindx)
				{
				case 0: //BNDS
				{
					BNDS = TRUE;
					skip = true;
					fread(m_bbox, sizeof(v3_t), 2, m_fp);
				}
				break;
				case 9: //SUBM
				case 1: //MESH
				{
					skip = true;
					if (sigindx != 9)
					{
						fread(&ply_mesh.fvf, sizeof(int), 1, m_fp);
						fread(&ply_mesh.first_face, sizeof(int), 1, m_fp);
						fread(&ply_mesh.face_count, sizeof(int), 1, m_fp);
						fread(&ply_mesh.flags, sizeof(int), 1, m_fp);
					}
					else
					{
						DWORD unknown_const[14], unknown_4byte, SUBM_Flags;
						fread(unknown_const, 14, 1, m_fp);
						ply_mesh.fvf = 0x00000152;
						GlFvf = ply_mesh.fvf;
						fread(&ply_mesh.first_face, sizeof(DWORD), 1, m_fp);
						fread(&ply_mesh.face_count, sizeof(DWORD), 1, m_fp);
						ply_mesh.flags = 0x00001004;
						GlFlags = ply_mesh.flags;
						fread(&unknown_4byte, sizeof(DWORD), 1, m_fp);
						fread(&unknown_4byte, sizeof(DWORD), 1, m_fp);
						fread(&SUBM_Flags, sizeof(DWORD), 1, m_fp);
						tex_count = 1;
					}
					/*
					Åñëè â ëþáîì MESH âñòðåòèëñÿ ôëàã BUMP, òî âî âñåõ ñëåäóþùèõ MESH ïðåäïîëàãàåòñÿ ôëàã BUMP
					Àáñîëþòíî âñå âåðòåêñû â ýòîì ñëó÷àå áóäóò ñ BUMP.
					*/

					memset(text_msg_buff, 0, 2048);
					//printf("------------ Mesh Flags -----------");
					if (ply_mesh.flags & MESH_FLAG_TWO_SIDED)
					{
						strcat(text_msg_buff, "MESH_FLAG_TWO_SIDED\n");
					}
					if (ply_mesh.flags & MESH_FLAG_USE_ALPHA)
					{
						strcat(text_msg_buff, "MESH_FLAG_USE_ALPHA\n");
					}
					if (ply_mesh.flags & MESH_FLAG_LIGHT)
					{
						strcat(text_msg_buff, "MESH_FLAG_LIGHT\n");
					}
					if (ply_mesh.flags & MESH_FLAG_PLCR)
					{
						strcat(text_msg_buff, "MESH_FLAG_PLCR\n");
					}
					if (ply_mesh.flags & MESH_FLAG_SKINNED)
					{
						strcat(text_msg_buff, "MESH_FLAG_SKINNED\n");
					}
					if (ply_mesh.flags & MESH_FLAG_SHADOW)
					{
						strcat(text_msg_buff, "MESH_FLAG_SHADOW\n");
					}
					if (ply_mesh.flags & MESH_FLAG_MIRRORED)
					{
						strcat(text_msg_buff, "MESH_FLAG_MIRRORED\n");
					}
					if (ply_mesh.flags & MESH_FLAG_BLENDTEX)
					{
						strcat(text_msg_buff, "MESH_FLAG_BLENDTEX\n");
					}
					if (ply_mesh.flags & MESH_FLAG_BUMP)
					{
						strcat(text_msg_buff, "MESH_FLAG_BUMP\n");
					}
					if (ply_mesh.flags & MESH_FLAG_SPECULAR)
					{
						strcat(text_msg_buff, "MESH_FLAG_SPECULAR\n");
					}
					if (ply_mesh.flags & MESH_FLAG_MATERIAL)
					{
						strcat(text_msg_buff, "MESH_FLAG_MATERIAL\n");
					}
					if (ply_mesh.flags & MESH_FLAG_SUBSKIN)
					{
						strcat(text_msg_buff, "MESH_FLAG_SUBSKIN\n");
					}
					if (ply_mesh.flags & MESH_FLAG_TWOTEX)
					{
						strcat(text_msg_buff, "MESH_FLAG_TWOTEX\n");
					}
					if (ply_mesh.flags & MESH_FLAG_USINGVD)
					{
						strcat(text_msg_buff, "MESH_FLAG_USINGVD\n");
					}
					if (ply_mesh.flags & MESH_FLAG_LIGHTMAP)
					{
						strcat(text_msg_buff, "MESH_FLAG_LIGHTMAP\n");
					}
					//printf("-----------------------------------");
					printf("%s", text_msg_buff);
					///MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, text_msg_buff, "PLY", MB_OK); 

					//ñïàñèáî GhostNT
					if (GlFvf)
					{
						fvf_wo_texcount = (GlFvf | ply_mesh.fvf) & (0xffff ^ D3DFVF_TEXCOUNT_MASK);
						old_texcount = GlFvf & D3DFVF_TEXCOUNT_MASK;
						new_texcount = ply_mesh.fvf & D3DFVF_TEXCOUNT_MASK;
						if (old_texcount > new_texcount)
						{
							GlFvf = fvf_wo_texcount | old_texcount;
						}
						else
						{
							GlFvf = fvf_wo_texcount | new_texcount;
							GlFlags = GlFlags | ply_mesh.flags;
						}
					}
					else
					{
						GlFvf = ply_mesh.fvf; GlFlags = ply_mesh.flags;
					}
					if (ply_mesh.flags & MESH_FLAG_SPECULAR)
					{
						fread(&specular_rgba_color, sizeof(int), 1, m_fp);
					}
					if (ply_mesh.flags & MESH_FLAG_BUMP)
					{
						forced_bamp = TRUE;
					}
					if (ply_mesh.flags & MESH_FLAG_SKINNED)
					{
						SKINNED = TRUE;
					}
					memset(texname[0], 0, _MAX_PATH);
					memset(texname[1], 0, _MAX_PATH);
					memset(texname[2], 0, _MAX_PATH);
					memset(texname[3], 0, _MAX_PATH);
					for (i = 0; i < tex_count; i++)
					{
						BYTE texlong = 0;
						fread(&texlong, sizeof(BYTE), 1, m_fp);
						if (texlong)
						{
							fread(textemp, 1, texlong, m_fp);
						}
						else
						{
							fread(textemp, 1, 1, m_fp); break;
						}
						textemp[texlong] = '\0';
						if (!i)
						{
							strcpy(texname[2], textemp);
						}
						else
						{
							strcpy(texname[3], textemp);
						}
						FixPathDelim(textemp);
						CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd();
						bool tex_test = false, repeat_for_mdl = false;
						do
						{
							int skip_item = -1;
							char independent_path[_MAX_PATH] = { 0 };
							bool find_mask = false, path_type = false;// true - àáñîëþòíûé ïóòü, false - ëîêàëüíûé ïóòü
							for (int path_search = 6; path_search > ((!path_type) ? 0 : -1) && !tex_test; path_search--)
							{
								if ((!find_mask && path_type) && path_search < 6)
								{
									break;
								}
								tex_test = false;
								if (textemp[0] == '/')
								{
									// Cut off the path before "entity" and append this to it
									if (textemp[1] == 'e' || textemp[1] == 't' || textemp[1] == 'E' || textemp[1] == 'T') //entity or texture
									{
										strcpy(texname[i], pWnd->m_ResPath[(path_search == 6) ? 0 : path_search]);
										strcat(texname[i], "resource");
										strcat(texname[i], textemp);
										path_type = false;
									}
								}
								else
								{
									if (path_search == 6)
									{
										path_type = true;
										if (repeat_for_mdl)
										{
											repeat_for_mdl = false;
											path = strrchr((char*)pWnd->m_CurMdlPath, '/');
											path++;
											strncpy((char*)texname[i], (char*)pWnd->m_CurMdlPath, (path - pWnd->m_CurMdlPath));
											texname[i][path - pWnd->m_CurMdlPath] = '\0';
											//òåêóùèé ïóòü mdl`à
										}
										else
										{
											// Cut off the path before the ply name and append this to it
											if (strlen(pWnd->m_CurMdlPath))
											{
												repeat_for_mdl = true;
											}
											path = strrchr((char*)plyname, '/');
											path++;
											strncpy((char*)texname[i], (char*)plyname, (path - plyname));
											texname[i][path - plyname] = '\0';
											//ïóòü ply-ôàéëà
										}
									}
									else
									{
										if (skip_item >= 0 && path_search == skip_item)
										{
											continue;
										}
										if (!strlen(pWnd->m_ResPath[path_search]))
										{
											continue;
										}
										memset(texname[i], 0, _MAX_PATH);
										sprintf(texname[i], "%s%s", pWnd->m_ResPath[path_search], independent_path);
									}
									//D:\Games\Men of War Assault Squad 2\mods\LTP\resource\entity\test\2\ba27m_2\
										//áåð¸ì ïóòü "ïî óìîë÷àíèþ" (äëÿ path_search = 6)
									if (path_search == 6)
									{
										for (int path_mask = 5; path_mask > -1; path_mask--)
										{
											int ResPath_len = strlen(pWnd->m_ResPath[path_mask]);
											//D:\Games\Men of War Assault Squad 2\mods\LTP\
												//ïðîâåðÿåì ñïèñîê
											char tmp_res_path[_MAX_PATH] = { 0 };
											strcpy(tmp_res_path, pWnd->m_ResPath[path_mask]);
											FixPathDelim(tmp_res_path);
											strlwr(tmp_res_path);
											strlwr(texname[i]);
											if (ResPath_len && strstr(texname[i], tmp_res_path))
											{
												find_mask = true;
												strncpy(independent_path, texname[i] + ResPath_len, strlen(texname[i]) - ResPath_len);
												skip_item = path_mask;
												//resource\entity\test\2\ba27m_2\
													//ïîëó÷àåì êîðåíü ïóòè
												break;
											}
										}
									}
									strcat((char*)texname[i], (char*)textemp);// ÷àñòü ïóòè (åñëè åñòü â ply), èìÿ è ðàñøèðåíèå ôàéëà ìàòåðèàëà
								}
								FixPathDelim(texname[i]);
								All_Trash* AT = new All_Trash();
								AT->Parse_Path(texname[i]);
								delete AT;
								//òóò ïðîâåðêà ïóòè
								//ïðîâåðÿåì íà tex`îâîñòü èëè ïîëíîå èìÿ
								FILE* testfp = NULL;
								char ExtTest[MAX_PATH] = { 0 };
								strcpy(ExtTest, texname[i]);
								if (!(testfp = fopen(ExtTest, "r")))
								{
									strcat(ExtTest, ".tex");
									if (testfp = fopen(ExtTest, "r"))
									{
										strcat(texname[i], ".tex");
										tex_test = true;
										repeat_for_mdl = false;
									}
								}
								else
								{
									tex_test = true;
									repeat_for_mdl = false;
								}
								if (testfp)
								{
									fclose(testfp); testfp = NULL;
								}
								//èùåì ðàñøèðåíèÿ
								if (!tex_test)
								{
									for (int texsigindx = 0; tex_siglist[texsigindx]; texsigindx++)
									{
										strcpy(ExtTest, texname[i]);
										strcat(ExtTest, tex_siglist[texsigindx]);
										if (testfp = fopen(ExtTest, "r"))
										{
											strcat(texname[i], tex_siglist[texsigindx]);
											tex_test = true;
											fclose(testfp);
											testfp = NULL;
											repeat_for_mdl = false;
											break;
										}
									}
								}
							}
						} while (repeat_for_mdl);
						if (!tex_test)
						{
#ifdef ALTERNATIVE_LANG
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, CString("The material file was not found:\n" + CString(textemp)), "ERROR: CPly::CPly", MB_ICONHAND);
#else
							MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, CString("Ôàéë ìàòåðèàëà íå íàéäåí:\n" + CString(textemp)), "ERROR: CPly::CPly", MB_ICONHAND);
#endif
							memset(texname[i], 0, _MAX_PATH);
							strcpy(texname[i], "<<checker>>");
						}
					}
					if (ply_mesh.flags & MESH_FLAG_SUBSKIN)
					{
						fread(&subskin_count, sizeof(BYTE), 1, m_fp);
						for (i = 0; i < subskin_count; i++)
						{
							fread(&subskin_bones[i], sizeof(BYTE), 1, m_fp);
						}
					}
					if (strlen(texname[0]))
					{
						m_CurrMesh = m_meshlist->AddToTail(ply_mesh.first_face, ply_mesh.face_count, ply_mesh.fvf, texname, tex_count, ply_mesh.flags, specular_rgba_color, subskin_count, subskin_bones);
					}
				}
				break;
				case 2: //VERT
				{
					skip = true;
					fread(&m_numverts, sizeof(int), 1, m_fp);
					fread(&m_vsize, sizeof(short), 1, m_fp);
					fread(&m_vflags, sizeof(short), 1, m_fp);
					unknown_data_size = m_vsize;
					if (GlFvf & D3DFVF_POSITION_MASK)
					{
						has_pos = TRUE;
						unknown_data_size -= 12;
						if ((GlFvf & D3DFVF_XYZB5) >= D3DFVF_XYZB1)
						{
							num_weights = ((GlFvf & D3DFVF_XYZB5) - D3DFVF_XYZB1) >> 1;
							has_weights = TRUE;
							unknown_data_size -= 4 * (num_weights + 1);
							if (GlFvf & D3DFVF_LASTBETA_UBYTE4)
							{
								has_matrix_indices = TRUE;
							}
							else
								if (GlFvf & D3DFVF_LASTBETA_D3DCOLOR)
								{
									has_matrix_indices = TRUE;
								}
								else
								{
									num_weights += 1;
								}
						}
						else
							if ((GlFvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZW)
							{
								has_w = TRUE;
								unknown_data_size -= 4;
							}
							else
								if ((GlFvf & D3DFVF_POSITION_MASK) == D3DFVF_XYZRHW)
								{
									has_rhw = TRUE;
									unknown_data_size -= 4;
								}
					}
					if (GlFvf & D3DFVF_NORMAL)
					{
						has_normal = TRUE;
						unknown_data_size -= 12;
					}
					if (GlFvf & D3DFVF_PSIZE)
					{
						has_psize = TRUE;
						unknown_data_size -= 4;
					}
					if (GlFvf & D3DFVF_DIFFUSE)
					{
						has_diffuse = TRUE;
						unknown_data_size -= 4;
					}
					if (GlFvf & D3DFVF_SPECULAR)
					{
						has_specular = TRUE;
						unknown_data_size -= 4;
					}
					if (GlFvf & D3DFVF_TEXCOUNT_MASK)
					{
						num_tex_coords = (GlFvf & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;
						if (GlFlags & MESH_FLAG_TWOTEX)
						{
							num_tex_coords *= 2;
						}
						if (Bply && m_vsize == 36)//äè÷ü
						{
							num_tex_coords -= 1; CopyUV = true;
						}
						unknown_data_size -= 8 * num_tex_coords;
						has_tex_coords = TRUE;
					}
					has_mesh_specular = ((GlFvf & MESH_FLAG_SPECULAR) != 0);
					has_mesh_bump = ((GlFvf & MESH_FLAG_BUMP) != 0) | forced_bamp;
					if (has_mesh_bump)
					{
						if (unknown_data_size < 16)
						{
							has_mesh_bump = FALSE;
						}
						else
						{
							unknown_data_size -= 16;
						}
					}
					m_calculated_vsize = 0;
					m_calculated_vsize = (has_pos) ? sizeof(v3_t) : 0;
					m_calculated_vsize += (has_rhw) ? 4 : 0;
					m_calculated_vsize += (has_weights && num_weights > 0) ? (num_weights * 4) : 0;
					m_calculated_vsize += (has_matrix_indices) ? 4 : 0;
					m_calculated_vsize += (has_normal) ? sizeof(v3_t) : 0;
					m_calculated_vsize += (has_psize) ? 4 : 0;
					m_calculated_vsize += (has_diffuse) ? sizeof(int) : 0;
					m_calculated_vsize += (has_specular) ? 4 : 0;
					m_calculated_vsize += (has_tex_coords && num_tex_coords > 0) ? (num_tex_coords * sizeof(v2_t)) : 0;
					m_calculated_vsize += (has_mesh_bump) ? 16 : 0;
					memset(text_msg_buff, 0, 2048);
					sprintf(text_msg_buff, "Vertex size: %d\nÑalculated vsize: %d\nVertex count: %d\nVertex flags: %X\n------------ D3D9 FVF -----------\nFVF flags: %X\nHas position: %s\nHas RHW: %s\nHas weights: %s\nNum weights: %d\n"
						"Has matrix indices: %s\nHas normal: %s\nHas psize: %s\nHas diffuse: %s\nHas specular: %s\nHas tex coords: %s\nNum tex coords: %d\nHas bump: %s", m_vsize, m_calculated_vsize, m_numverts, m_vflags, ply_mesh.fvf, (has_pos) ? "TRUE" : "FALSE",
						(has_rhw) ? "TRUE" : "FALSE", (has_weights) ? "TRUE" : "FALSE", num_weights, (has_matrix_indices) ? "TRUE" : "FALSE", (has_normal) ? "TRUE" : "FALSE", (has_psize) ? "TRUE" : "FALSE",
						(has_diffuse) ? "TRUE" : "FALSE", (has_specular) ? "TRUE" : "FALSE", (has_tex_coords) ? "TRUE" : "FALSE", num_tex_coords, (has_mesh_bump) ? "TRUE" : "FALSE");
					///MessageBox(AfxGetApp() -> m_pMainWnd -> m_hWnd, text_msg_buff, "PLY", MB_OK);

					m_vertlist = new vert_t[m_numverts];
					memset(m_vertlist, 0, sizeof(vert_t) * m_numverts);
					for (i = 0; i < m_numverts; i++)
					{
						if (has_pos)
						{
							fread(&m_vertlist[i].xyz, sizeof(v3_t), 1, m_fp);
							if (has_rhw || has_w)
							{
								fread(&m_vertlist[i].rhw, 4, 1, m_fp);
							}
							if (num_weights > 0)
							{
								m_vertlist[i].WeightsData = new DWORD[num_weights];
								for (i2 = 0; i2 < num_weights; i2++)
								{
									fread(&m_vertlist[i].WeightsData[i2], 4, 1, m_fp);
								}
							}
							if (has_matrix_indices)
							{
								fread(&m_vertlist[i].bones[0], 1, 1, m_fp);
								fread(&m_vertlist[i].bones[1], 1, 1, m_fp);
								fread(&m_vertlist[i].bones[2], 1, 1, m_fp);
								fread(&m_vertlist[i].bones[3], 1, 1, m_fp);
							}
						}
						if (has_normal)
						{
							fread(&m_vertlist[i].vn, sizeof(v3_t), 1, m_fp);
						}
						if (has_psize)
						{
							fread(&m_vertlist[i].psize, 4, 1, m_fp);
						}
						if (has_diffuse)
						{
							fread(&m_vertlist[i].real_diffuse, 4, 1, m_fp);
							m_vertlist[i].diffuse = m_vertlist[i].real_diffuse;
							if (GetAValue(m_vertlist[i].diffuse) == 0)
							{
								m_vertlist[i].diffuse |= 0xFF000000;
							}
						}
						else
						{
							m_vertlist[i].diffuse = 0xFFFFFFFF;
						}
						//m_vertlist[i].diffuse = 0x80FFFFFF;
						if (has_specular)
						{
							fread(&m_vertlist[i].specular, 4, 1, m_fp);
						}
						if (num_tex_coords > 0)
						{
							fread(&m_vertlist[i].uv[0], sizeof(v2_t), 1, m_fp);
							if (num_tex_coords == 2)
							{
								fread(&m_vertlist[i].uv[1], sizeof(v2_t), 1, m_fp);
							}
							if (Bply && CopyUV)
							{
								memcpy(m_vertlist[i].uv[1], m_vertlist[i].uv[0], sizeof(v2_t));
							}
							float AddU = 0, AddV = 0;
							for (i2 = 0; i2 < (num_tex_coords - 2); i2++)
							{
								fread(&AddU, 4, 1, m_fp);
								fread(&AddV, 4, 1, m_fp);
							}
						}
						if (has_mesh_bump)
						{
							fread(&m_vertlist[i].bump12, sizeof(v3_t), 1, m_fp);
							fread(&m_vertlist[i].bump4, sizeof(float), 1, m_fp);
							//memcpy(&m_vertlist[i].vn, &m_vertlist[i].bump12, sizeof(v3_t));
						}
						if (unknown_data_size > 0)
						{
							fread(&m_vertlist[i].other_buff, unknown_data_size, 1, m_fp);
						}
					}
				}
				break;
				case 3: //INDX
				{
					skip = true;
					IndexType = 1;
					fread(&INDXcount, sizeof(int), 1, m_fp);
					m_numpolys = INDXcount / 3;
					poly_count = INDXcount / 3;
					m_polylist = new indx_t[m_numpolys];
					memset(m_polylist, 0, sizeof(indx_t) * m_numpolys);
					for (i = 0; i < m_numpolys; i++)
					{
						WORD tshort[3];
						fread(tshort, sizeof(WORD), 3, m_fp);
						/*if(SKINNED)
						{
							m_polylist[i].v[0] = tshort[2];//
							m_polylist[i].v[1] = tshort[1];// Ýòîò êîñòûëü ÷èñòî äëÿ ýòîé ïðîãðàììû
							m_polylist[i].v[2] = tshort[0];//
						}
						else*/
						//{
						m_polylist[i].v[0] = tshort[0];
						m_polylist[i].v[1] = tshort[1];
						m_polylist[i].v[2] = tshort[2];
						//}
						//fread(&m_polylist[i], sizeof(indx_t), 1, m_fp);
					}
				}
				break;
				case 7: //IND4
				{
					skip = true;
					IndexType = 2;
					fread(&INDXcount, sizeof(int), 1, m_fp);
					poly_count = INDXcount / 3;
					m_numpolys = INDXcount / 3;
					m_polylist = new indx_t[m_numpolys];
					memset(m_polylist, 0, sizeof(indx_t) * m_numpolys);
					for (i = 0; i < INDXcount / 3; i++)
					{
						fread(&m_polylist[i], sizeof(indx_t), 1, m_fp);
					}
				}
				break;
				case 4: //ADJA
				{
					ADJA = TRUE;
					skip = true;
					fread(&m_numadjas, sizeof(int), 1, m_fp);
					m_adjalist = new adja_t[m_numadjas];
					for (i = 0; i < m_numadjas; i++)
					{
						fread(&m_adjalist[i], sizeof(adja_t), 1, m_fp);
					}
				}
				break;
				case 5: //SHDW
				{
					SHDW = TRUE;
					skip = true;
					fread(&m_numshdws, sizeof(int), 1, m_fp);
					m_shdwlist = new BYTE[m_numshdws];
					for (i = 0; i < m_numshdws; i++)
					{
						fread(&m_shdwlist[i], 1, 1, m_fp);
					}
				}
				break;
				case 6: //SKIN
				{
					skip = true;
					BYTE csize = 0;
					fread(&m_bones, sizeof(int), 1, m_fp);
					if (m_bones)
					{
						m_bonelist = new char* [m_bones];
						for (i = 0; i < m_bones; i++)
						{
							fread(&csize, 1, sizeof(char), m_fp);
							m_bonelist[i] = new char[csize + 1];
							fread(m_bonelist[i], 1, csize, m_fp);
							m_bonelist[i][csize] = '\0';
						}
					}
				}
				break;
				case 8: //MROR
				{
					MROR = TRUE;
					skip = true;
					m_mirror = 1;
				}
				break;
				case 10: //BAS2
				{
					BAS2 = TRUE;
					skip = true;
					fread(m_basis, sizeof(v3_t) * 4, 1, m_fp);
				}
				break;
				default: //Other
				{
#ifdef ALTERNATIVE_LANG
					MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "A valid but unreadable section of the file was found.", "ERROR: CPly::CPly", MB_ICONHAND);
#else
					MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "Íàéäåí äîïóñòèìûé, íî íå÷èòàåìûé ðàçäåë ôàéëà.", "ERROR: CPly::CPly", MB_ICONHAND);
#endif
					fclose(m_fp);
					return;
				}
				break;
				}
			}
		}
		if (!ply_siglist[sigindx - 1])
		{
			CStringA st;
			long failure_address = ftell(m_fp);
#ifdef ALTERNATIVE_LANG
			st.Format("File: %s\r\ndamaged or has an unknown format.\r\nStudy the problem in the hexadecimal editor in the address area: 0x%x", plyname, failure_address);
#else
			st.Format("Ôàéë: %s\r\nïîâðåæä¸í èëè èìååò íåèçâåñòíûé ôîðìàò.\r\nÈçó÷àé ïðîáëåìó â øåñòíàäöàòåðè÷íîì ðåäàêòîðå â îáëàñòè àäðåñà: 0x%x", plyname, failure_address);
#endif
			MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, st, "ERROR: CPly::CPly", MB_ICONHAND);
			fclose(m_fp);
			return;
		}
	}
	fclose(m_fp);
	loading_successes = true;
	return;
}

CPly::~CPly()
{
	if (m_bones)
	{
		for (int i = 0; i < m_bones; i++)
		{
			delete[] m_bonelist[i];
		}
		delete[] m_bonelist;
		m_bonelist = NULL;
		m_bones = 0;
	}
	if (m_vertlist)
	{
		for (int i = 0; i < m_numverts; i++)
		{
			if (m_vertlist[i].WeightsData)
			{
				delete[] m_vertlist[i].WeightsData; m_vertlist[i].WeightsData = NULL;
			}
		}
		delete[] m_vertlist; m_vertlist = NULL;
	}
	if (m_meshlist)
	{
		delete m_meshlist; m_meshlist = NULL;
	}
	if (m_polylist)
	{
		delete[] m_polylist; m_polylist = NULL;
	}
	if (m_adjalist)
	{
		delete[] m_adjalist; m_adjalist = NULL;
	}
	if (m_shdwlist)
	{
		delete[] m_shdwlist; m_shdwlist = NULL;
	}
	SKINNED = FALSE;
}

static char* DupStringOrNull(const char* src)
{
	if (!src)
	{
		return NULL;
	}
	size_t len = strlen(src) + 1;
	char* dup = new char[len];
	memset(dup, 0, len);
	strcpy(dup, src);
	return dup;
}

static void CopyVertexData(vert_t& dst, const vert_t& src, int weights_count)
{
	memcpy(&dst, &src, sizeof(vert_t));
	dst.WeightsData = NULL;
	if (weights_count > 0 && src.WeightsData)
	{
		dst.WeightsData = new DWORD[weights_count];
		memset(dst.WeightsData, 0, sizeof(DWORD) * weights_count);
		memcpy(dst.WeightsData, src.WeightsData, sizeof(DWORD) * weights_count);
	}
}

bool CPly::MergeKeepSeparateTextures(const CPly* append_ply)
{
	if (!append_ply || !append_ply->loading_successes || !append_ply->m_meshlist)
	{
		return false;
	}

	if (!m_meshlist)
	{
		m_meshlist = new CMeshList();
	}

	if (m_numverts > 0)
	{
		if (m_vsize != append_ply->m_vsize || m_vflags != append_ply->m_vflags || num_weights != append_ply->num_weights || num_tex_coords != append_ply->num_tex_coords || unknown_data_size != append_ply->unknown_data_size)
		{
			return false;
		}
	}
	else
	{
		m_vsize = append_ply->m_vsize;
		m_vflags = append_ply->m_vflags;
		m_calculated_vsize = append_ply->m_calculated_vsize;
		num_weights = append_ply->num_weights;
		num_tex_coords = append_ply->num_tex_coords;
		unknown_data_size = append_ply->unknown_data_size;
		has_pos = append_ply->has_pos;
		has_rhw = append_ply->has_rhw;
		has_weights = append_ply->has_weights;
		has_normal = append_ply->has_normal;
		has_psize = append_ply->has_psize;
		has_diffuse = append_ply->has_diffuse;
		has_mesh_bump = append_ply->has_mesh_bump;
		has_specular = append_ply->has_specular;
		has_tex_coords = append_ply->has_tex_coords;
		has_matrix_indices = append_ply->has_matrix_indices;
		has_mesh_specular = append_ply->has_mesh_specular;
		has_w = append_ply->has_w;
	}

	int oldVertCount = m_numverts;
	int oldPolyCount = m_numpolys;
	int oldAdjaCount = m_numadjas;
	int oldShdwCount = m_numshdws;

	int* bone_remap = NULL;
	if (append_ply->m_bones > 0 && append_ply->m_bonelist)
	{
		bone_remap = new int[append_ply->m_bones];
		memset(bone_remap, 0, sizeof(int) * append_ply->m_bones);

		int merged_bone_count = m_bones;
		for (int i = 0; i < append_ply->m_bones; i++)
		{
			bool found = false;
			for (int i2 = 0; i2 < m_bones && !found; i2++)
			{
				if (m_bonelist[i2] && !stricmp(m_bonelist[i2], append_ply->m_bonelist[i]))
				{
					found = true;
				}
			}
			if (!found)
			{
				merged_bone_count++;
			}
		}

		if (merged_bone_count > 255)
		{
			delete[] bone_remap;
#ifdef ALTERNATIVE_LANG
			MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "Merge aborted: combined unique bone count exceeds the format limit of 255.", "ERROR: CPly::MergeKeepSeparateTextures", MB_ICONHAND);
#else
			MessageBoxA(AfxGetApp()->m_pMainWnd->m_hWnd, "Ñëèÿíèå ïðåðâàíî: îáùåå êîëè÷åñòâî óíèêàëüíûõ êîñòåé ïðåâûøàåò ëèìèò ôîðìàòà (255).", "ERROR: CPly::MergeKeepSeparateTextures", MB_ICONHAND);
#endif
			return false;
		}

		char** merged_bones = new char* [merged_bone_count];
		memset(merged_bones, 0, sizeof(char*) * merged_bone_count);
		for (int i = 0; i < m_bones; i++)
		{
			merged_bones[i] = DupStringOrNull(m_bonelist[i]);
		}

		int write_bone = m_bones;
		for (int i = 0; i < append_ply->m_bones; i++)
		{
			int remap = -1;
			for (int i2 = 0; i2 < write_bone; i2++)
			{
				if (merged_bones[i2] && !stricmp(merged_bones[i2], append_ply->m_bonelist[i]))
				{
					remap = i2;
					break;
				}
			}
			if (remap == -1)
			{
				merged_bones[write_bone] = DupStringOrNull(append_ply->m_bonelist[i]);
				remap = write_bone;
				write_bone++;
			}
			bone_remap[i] = remap;
		}

		if (m_bonelist)
		{
			for (int i = 0; i < m_bones; i++)
			{
				delete[] m_bonelist[i];
			}
			delete[] m_bonelist;
		}
		m_bonelist = merged_bones;
		m_bones = merged_bone_count;
	}

	int new_vert_count = m_numverts + append_ply->m_numverts;
	vert_t* merged_verts = new vert_t[new_vert_count];
	memset(merged_verts, 0, sizeof(vert_t) * new_vert_count);
	for (int i = 0; i < m_numverts; i++)
	{
		CopyVertexData(merged_verts[i], m_vertlist[i], num_weights);
	}
	for (int i = 0; i < append_ply->m_numverts; i++)
	{
		int dst_i = oldVertCount + i;
		CopyVertexData(merged_verts[dst_i], append_ply->m_vertlist[i], num_weights);
		if (bone_remap && has_matrix_indices)
		{
			for (int b = 0; b < 4; b++)
			{
				int src_bone = (unsigned char)append_ply->m_vertlist[i].bones[b];
				if (src_bone < append_ply->m_bones)
				{
					merged_verts[dst_i].bones[b] = (char)bone_remap[src_bone];
				}
			}
		}
	}

	if (m_vertlist)
	{
		for (int i = 0; i < m_numverts; i++)
		{
			if (m_vertlist[i].WeightsData)
			{
				delete[] m_vertlist[i].WeightsData;
			}
		}
		delete[] m_vertlist;
	}
	m_vertlist = merged_verts;
	m_numverts = new_vert_count;

	int new_poly_count = m_numpolys + append_ply->m_numpolys;
	indx_t* merged_polys = new indx_t[new_poly_count];
	memset(merged_polys, 0, sizeof(indx_t) * new_poly_count);
	for (int i = 0; i < m_numpolys; i++)
	{
		memcpy(&merged_polys[i], &m_polylist[i], sizeof(indx_t));
	}
	for (int i = 0; i < append_ply->m_numpolys; i++)
	{
		int dst_i = oldPolyCount + i;
		merged_polys[dst_i].v[0] = append_ply->m_polylist[i].v[0] + oldVertCount;
		merged_polys[dst_i].v[1] = append_ply->m_polylist[i].v[1] + oldVertCount;
		merged_polys[dst_i].v[2] = append_ply->m_polylist[i].v[2] + oldVertCount;
	}
	if (m_polylist)
	{
		delete[] m_polylist;
	}
	m_polylist = merged_polys;
	m_numpolys = new_poly_count;
	INDXcount = m_numpolys * 3;
	IndexType = (IndexType == 2 || append_ply->IndexType == 2 || new_vert_count > 65535) ? 2 : 1;

	if (ADJA && append_ply->ADJA && m_adjalist && append_ply->m_adjalist)
	{
		int new_adja_count = m_numadjas + append_ply->m_numadjas;
		adja_t* merged_adja = new adja_t[new_adja_count];
		memset(merged_adja, 0, sizeof(adja_t) * new_adja_count);
		for (int i = 0; i < oldAdjaCount; i++)
		{
			memcpy(&merged_adja[i], &m_adjalist[i], sizeof(adja_t));
		}
		for (int i = 0; i < append_ply->m_numadjas; i++)
		{
			merged_adja[oldAdjaCount + i] = append_ply->m_adjalist[i];
			if (merged_adja[oldAdjaCount + i].a >= 0)
			{
				merged_adja[oldAdjaCount + i].a += oldPolyCount;
			}
			if (merged_adja[oldAdjaCount + i].b >= 0)
			{
				merged_adja[oldAdjaCount + i].b += oldPolyCount;
			}
			if (merged_adja[oldAdjaCount + i].c >= 0)
			{
				merged_adja[oldAdjaCount + i].c += oldPolyCount;
			}
		}
		delete[] m_adjalist;
		m_adjalist = merged_adja;
		m_numadjas = new_adja_count;
	}
	else
	{
		ADJA = FALSE;
		m_numadjas = 0;
		if (m_adjalist)
		{
			delete[] m_adjalist; m_adjalist = NULL;
		}
	}

	if (SHDW && append_ply->SHDW && m_shdwlist && append_ply->m_shdwlist)
	{
		int new_shdw_count = m_numshdws + append_ply->m_numshdws;
		unsigned char* merged_shdw = new unsigned char[new_shdw_count];
		memset(merged_shdw, 0, new_shdw_count);
		memcpy(merged_shdw, m_shdwlist, oldShdwCount);
		memcpy(merged_shdw + oldShdwCount, append_ply->m_shdwlist, append_ply->m_numshdws);
		delete[] m_shdwlist;
		m_shdwlist = merged_shdw;
		m_numshdws = new_shdw_count;
	}
	else
	{
		SHDW = FALSE;
		m_numshdws = 0;
		if (m_shdwlist)
		{
			delete[] m_shdwlist; m_shdwlist = NULL;
		}
	}

	CMesh* mesh = append_ply->m_meshlist->GetFirst();
	while (mesh)
	{
		char texname[4][_MAX_PATH] = { 0 };
		for (int i = 0; i < 4; i++)
		{
			if (mesh->m_texname[i])
			{
				strncpy(texname[i], mesh->m_texname[i], _MAX_PATH - 1); texname[i][_MAX_PATH - 1] = '\0';
			}
		}
		BYTE remapped_subskin[MAX_PATH] = { 0 };
		if (mesh->subskin_count > 0)
		{
			for (int i = 0; i < mesh->subskin_count; i++)
			{
				BYTE sub = mesh->subskin_bones[i];
				if (bone_remap && sub < append_ply->m_bones)
				{
					remapped_subskin[i] = (BYTE)bone_remap[sub];
				}
				else
				{
					remapped_subskin[i] = sub;
				}
			}
		}
		m_meshlist->AddToTail(mesh->m_first + oldPolyCount, mesh->m_count, mesh->m_fvf, texname, mesh->m_texcount, mesh->m_flags, mesh->specular_rgba_color, mesh->subskin_count, remapped_subskin);
		mesh = mesh->next;
	}

	Bply = (Bply || append_ply->Bply);
	GlFvf = (GlFvf) ? GlFvf : append_ply->GlFvf;
	GlFlags |= append_ply->GlFlags;
	SKINNED = (SKINNED || append_ply->SKINNED);
	MROR = (MROR || append_ply->MROR);
	m_mirror = (MROR) ? 1 : 0;

	if (!BNDS && append_ply->BNDS)
	{
		BNDS = TRUE;
	}
	if (BNDS && m_numverts > 0 && has_pos)
	{
		m_bbox[0][0] = m_bbox[1][0] = m_vertlist[0].xyz.x;
		m_bbox[0][1] = m_bbox[1][1] = m_vertlist[0].xyz.y;
		m_bbox[0][2] = m_bbox[1][2] = m_vertlist[0].xyz.z;
		for (int i = 1; i < m_numverts; i++)
		{
			if (m_vertlist[i].xyz.x < m_bbox[0][0])
			{
				m_bbox[0][0] = m_vertlist[i].xyz.x;
			}
			if (m_vertlist[i].xyz.y < m_bbox[0][1])
			{
				m_bbox[0][1] = m_vertlist[i].xyz.y;
			}
			if (m_vertlist[i].xyz.z < m_bbox[0][2])
			{
				m_bbox[0][2] = m_vertlist[i].xyz.z;
			}
			if (m_vertlist[i].xyz.x > m_bbox[1][0])
			{
				m_bbox[1][0] = m_vertlist[i].xyz.x;
			}
			if (m_vertlist[i].xyz.y > m_bbox[1][1])
			{
				m_bbox[1][1] = m_vertlist[i].xyz.y;
			}
			if (m_vertlist[i].xyz.z > m_bbox[1][2])
			{
				m_bbox[1][2] = m_vertlist[i].xyz.z;
			}
		}
	}

	if (bone_remap)
	{
		delete[] bone_remap;
	}
	return true;
}

bool CPly::WriteFile(char* plyname)
{
	m_fp = fopen(plyname, "wb");
	if (!m_fp)
	{
		return false;
	}
	if (Bply)
	{
		fwrite("BPLY", 4, 1, m_fp);
	}
	else
	{
		fwrite("EPLY", 4, 1, m_fp);
	}
	if (BNDS)
	{
		fwrite("BNDS", 4, 1, m_fp);
		fwrite(m_bbox, sizeof(v3_t), 2, m_fp);
	}
	if (m_bones > 0 && m_bonelist)
	{
		fwrite("SKIN", 4, 1, m_fp);
		fwrite((int*)&m_bones, sizeof(int), 1, m_fp);
		BYTE len_bone = 0;
		char str_bone[256] = { 0 };
		for (int i = 0; i < m_bones; i++)
		{
			len_bone = strlen(m_bonelist[i]);
			strncpy((char*)&str_bone, m_bonelist[i], len_bone);
			fwrite((BYTE*)&len_bone, 1, 1, m_fp);
			fwrite(str_bone, len_bone, 1, m_fp);
		}
	}

	CMesh* pMesh;
	BYTE plen = 0;
	pMesh = m_meshlist->GetFirst();
	while (pMesh)
	{
		fwrite("MESH", 4, 1, m_fp);
		fwrite((int*)&pMesh->m_fvf, sizeof(int), 1, m_fp);
		fwrite((int*)&pMesh->m_first, sizeof(int), 1, m_fp);
		fwrite((int*)&pMesh->m_count, sizeof(int), 1, m_fp);
		fwrite((int*)&pMesh->m_flags, sizeof(int), 1, m_fp);
		if (pMesh->m_flags & MESH_FLAG_SPECULAR)
		{
			fwrite((DWORD*)&pMesh->specular_rgba_color, sizeof(DWORD), 1, m_fp);
		}
		for (int mats = 0; mats < 1 + ((Bply && !(pMesh->m_flags & MESH_FLAG_MATERIAL)) ? 1 : 0); mats++)
		{
			plen = (pMesh->m_texname[mats + 2]) ? strlen(pMesh->m_texname[mats + 2]) : 0;
			fwrite((BYTE*)&plen, sizeof(BYTE), 1, m_fp);
			if (plen)
			{
				fwrite((char*)pMesh->m_texname[mats + 2], plen, 1, m_fp);
			}
			else
			{
				fwrite("", 1, 1, m_fp);
			}
		}
		if (pMesh->m_flags & MESH_FLAG_SUBSKIN)
		{
			fwrite((BYTE*)&pMesh->subskin_count, sizeof(BYTE), 1, m_fp);
			for (int i2 = 0; i2 < pMesh->subskin_count; i2++)
			{
				fwrite((BYTE*)&pMesh->subskin_bones[i2], sizeof(BYTE), 1, m_fp);
			}
		}
		pMesh = pMesh->next;
	}
	if (BAS2)
	{
		fwrite("BAS2", 4, 1, m_fp);
		fwrite(m_basis, sizeof(v3_t) * 4, 1, m_fp);
	}
	if (m_numverts > 0 && m_vertlist)
	{
		fwrite("VERT", 4, 1, m_fp);
		fwrite((int*)&m_numverts, sizeof(int), 1, m_fp);
		fwrite((short*)&m_vsize, sizeof(short), 1, m_fp);
		fwrite((short*)&m_vflags, sizeof(short), 1, m_fp);
		for (int i = 0; i < m_numverts; i++)
		{
			if (has_pos)
			{
				fwrite((v3_t*)&m_vertlist[i].xyz, sizeof(v3_t), 1, m_fp);
				if (has_rhw || has_w)
				{
					fwrite((DWORD*)&m_vertlist[i].rhw, sizeof(DWORD), 1, m_fp);
				}
				if (num_weights > 0)
				{
					for (int i2 = 0; i2 < num_weights; i2++)
					{
						fwrite((DWORD*)&m_vertlist[i].WeightsData[i2], sizeof(DWORD), 1, m_fp);
					}
				}
				if (has_matrix_indices)
				{
					fwrite((BYTE*)&m_vertlist[i].bones[0], 1, 1, m_fp);
					fwrite((BYTE*)&m_vertlist[i].bones[1], 1, 1, m_fp);
					fwrite((BYTE*)&m_vertlist[i].bones[2], 1, 1, m_fp);
					fwrite((BYTE*)&m_vertlist[i].bones[3], 1, 1, m_fp);
				}
			}
			if (has_normal)
			{
				fwrite((v3_t*)&m_vertlist[i].vn, sizeof(v3_t), 1, m_fp);
			}
			if (has_psize)
			{
				fwrite((DWORD*)&m_vertlist[i].psize, sizeof(DWORD), 1, m_fp);
			}
			if (has_diffuse)
			{
				fwrite((int*)&m_vertlist[i].diffuse, sizeof(int), 1, m_fp);
			}
			if (has_specular)
			{
				fwrite((DWORD*)&m_vertlist[i].specular, sizeof(DWORD), 1, m_fp);
			}
			if (num_tex_coords > 0 && m_vertlist[i].uv)
			{
				for (int i2 = 0; i2 < num_tex_coords; i2++)
				{
					fwrite((v2_t*)&m_vertlist[i].uv[i2], sizeof(v2_t), 1, m_fp);
				}
			}
			if (has_mesh_bump)
			{
				fwrite((BYTE*)&m_vertlist[i].bump12, sizeof(v3_t), 1, m_fp);
				fwrite((BYTE*)&m_vertlist[i].bump4, sizeof(float), 1, m_fp);
			}
			if (unknown_data_size > 0)
			{
				fwrite((BYTE*)&m_vertlist[i].other_buff, unknown_data_size, 1, m_fp);
			}
		}
	}
	if (m_polylist)
	{
		switch (IndexType)
		{
		case 1: //"INDX"
		{
			fwrite("INDX", 4, 1, m_fp);
			fwrite((int*)&INDXcount, sizeof(int), 1, m_fp);
			for (int i = 0; i < m_numpolys; i++)
			{
				WORD tshort[3];
				tshort[0] = m_polylist[i].v[0];
				tshort[1] = m_polylist[i].v[1];
				tshort[2] = m_polylist[i].v[2];
				fwrite((WORD*)tshort, sizeof(WORD), 3, m_fp);
			}
		}
		break;
		case 2: //"IND4"
		{
			fwrite("IND4", 4, 1, m_fp);
			fwrite((int*)&INDXcount, sizeof(int), 1, m_fp);
			for (int i = 0; i < m_numpolys; i++)
			{
				fwrite((indx_t*)&m_polylist[i], sizeof(indx_t), 1, m_fp);
			}
		}
		break;
		};
	}
	if (m_numadjas > 0 && m_adjalist)
	{
		fwrite("ADJA", 4, 1, m_fp);
		fwrite((int*)&m_numadjas, sizeof(int), 1, m_fp);
		for (int i = 0; i < m_numadjas; i++)
		{
			fwrite((adja_t*)&m_adjalist[i], sizeof(adja_t), 1, m_fp);
		}
	}
	if (m_numshdws > 0 && m_shdwlist)
	{
		fwrite("SHDW", 4, 1, m_fp);
		fwrite((int*)&m_numshdws, sizeof(int), 1, m_fp);
		for (int i = 0; i < m_numshdws; i++)
		{
			fwrite((BYTE*)&m_shdwlist[i], 1, 1, m_fp);
		}
	}
	if (m_mirror)
	{
		fwrite("MROR", 4, 1, m_fp);
	}
	fclose(m_fp);
	return true;
}
//---------------------------------------------------------------------------
