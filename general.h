// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#ifndef _GENERAL_
#define _GENERAL_

#define B_POSITION		0x0001
#define B_ORIENTATION	0x0002
#define B_LEFT_HANDED	0x0004
#define B_VISIBLE_ON	0x0008
#define B_VISIBLE_OFF	0x0010
#define B_MESH			0x0020

#define ALTERNATIVE_LANG

#define Copy_files_on_saving 0
#define ColorUPD 0

#define GetRValue(rgba) (LOBYTE(rgba))
#define GetGValue(rgba) (LOBYTE(((WORD)(rgba)) >> 8))
//#define GetBValue(rgba) (LOBYTE((rgba) >> 16))
#define GetAValue(rgba) (LOBYTE((rgba) >> 24))
#define RGBA(r,g,b,a) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))

#define sbi(reg, bit) reg |= (1 << bit)
#define cbi(reg, bit) reg &= ~(1 << bit)
#define CheckBit(reg, bit) (reg & (1 << bit))

static char *ply_siglist[] = {"BNDS", "MESH", "VERT", "INDX", "ADJA", "SHDW", "SKIN", "IND4", "MROR", "SUBM", "BAS2", 0};

#define MESH_FLAG_TWO_SIDED 0x01  // render this mesh without culling
#define MESH_FLAG_USE_ALPHA 0x02  // (unused, compatibility mode)
#define MESH_FLAG_LIGHT     0x04  // use realtime scene light (unused, compatibility mode)
#define MESH_FLAG_PLCR      0x08  // use player color light
#define MESH_FLAG_SKINNED   0x10  // skinned mesh
#define MESH_FLAG_SHADOW    0x20  // shadow volume mesh
#define MESH_FLAG_MIRRORED  0x40  // has negative scaling
#define MESH_FLAG_BLENDTEX  0x80  // blend by second texture alpha
#define MESH_FLAG_BUMP      0x100  // bump-mapped
#define MESH_FLAG_SPECULAR  0x200  // specular color stored
#define MESH_FLAG_MATERIAL  0x400  // format with material, not textures
#define MESH_FLAG_SUBSKIN   0x800  // sub-skin feature
#define MESH_FLAG_TWOTEX    0x1000  // two textures with one texcoord
#define MESH_FLAG_USINGVD   0x2000  // using vertex declaration instead of fvf
#define MESH_FLAG_LIGHTMAP  0x4000  // has lightmap

#define D3DFVF_TEXTUREFORMAT1		3
#define D3DFVF_TEXTUREFORMAT2		0
#define D3DFVF_TEXTUREFORMAT3		1
#define D3DFVF_TEXTUREFORMAT4		2


char *GetModelPath(void);
void FixPathDelim(char *s);
void InvertFixPathDelim(char *s);
void ÑommaFixer(char *s);
void ForbiddenSymbolFixer(char *s);
int isFloat(char *s);

typedef float v3_t[3];
typedef float v2_t[2];
typedef double dv3_t[3];

typedef struct
{
    float v[4][3];
}matrix34_t;

typedef struct
{
    float v[3][3];
}matrix33_t;

typedef struct
{
    float v[4][4];
}matrix44_t;

typedef struct
{
    v3_t  Position; // Where is the object?
    v3_t  Orient;   // What direction is the object pointing in?
    // fov, etc...?
}camera_t;

typedef struct
{
    int v[3];
}indx_t;

typedef struct
{
    unsigned short v[4];
}ind4_t;

typedef struct
{
    short a,b,c;
}adja_t;

typedef struct
{
    int v[3];
}triangle_t;

typedef struct
{
    float s, t;
}texc_t;

typedef struct
{
    v3_t xyz;
    v3_t vn;
    char  bones[4];
    DWORD  diffuse;
    DWORD  real_diffuse;
    v2_t uv[2];
	DWORD rhw;
	DWORD *WeightsData;
	DWORD psize;
	DWORD specular;
	v3_t bump12;
	float  bump4;
	BYTE other_buff[20];

}vert_t;

typedef struct
{
    float u1;
    float u2;
}extra_t;

typedef struct
{
    int  version;
    int  begpoly;
    int  numpolys;
    char texname[256];
}mesh_t;

typedef struct
{
    char *name;
    void ((*handler)(char *));
}entity_t;

typedef struct
{
	int start_frame;
	int stop_frame;
	int action;
	BOOL x, y, z;
	v3_t min;
	v3_t max;
	int law;
	float koef;
} anm_task;

typedef struct
{
	BOOL task_on;
	BOOL matrix_rewrite;
	anm_task task[3];
} anm_tasks;

typedef unsigned char color_t[3];
const int frame_colors = 21;
typedef enum
{
	C_BG2D, C_BG3D, C_GRID, C_3D_GRID, C_AXIS, C_MESH, C_MESHS, C_VOLUME, C_VOLUMES,
	C_ENTITY, C_ENTITYS, C_OBSTACLE, C_OBSTACLES, C_AREA, C_AREAS,
	VOLUME_FRONT, VOLUME_REAR, VOLUME_RIGHT, VOLUME_LEFT, VOLUME_TOP, VOLUME_BOTTOM
} element_t;

typedef unsigned char byte;
typedef enum {OT_UNK, OT_RECT, OT_CIRCLE, OT_POLYGON} obstacle_t;
typedef enum {VT_UNK, VT_BOX, VT_CYLINDER, VT_PLY} volume_t;
typedef enum {IT_NONE, IT_BONE, IT_VOLUME, IT_BONETOP, IT_VOLUMETOP, IT_OBSTACLE, IT_OBSTACLETOP, IT_AREA, IT_AREATOP} itemtype_t;
typedef enum {v_front, v_right, v_left, v_top, v_rear, v_bottom, v_camera} view_t;
typedef enum {m_select, m_scale, m_move, m_rotate} mode_t;
typedef enum {r_wire, r_flat, r_smooth, r_texture} rmode_t;
typedef enum {l_none, l_ambient, l_diffuse} lmode_t;
typedef enum {rl_ambient, rl_diffuse, rl_wire, rl_tex_off, rl_ambient_tex_off} rlmode_t;
typedef enum {vm3_visual, vm3_hybrid, vm3_collision} View_mode_3d;

static char *tex_siglist[] = {".BMP", ".DDS", ".TGA", ".PNG", ".TIF", ".TIFF", ".GIF", ".JPG", ".JPEG", ".TEX", ".MTL", 0};

typedef enum {t_none = 0x00, t_front = 0x01, t_rear = 0x02, t_right = 0x04, t_left = 0x08, t_top = 0x10, t_bottom = 0x20} thickness_sides_t;

typedef struct
{
	float v[2];
	BOOL l;

} thickness_vars_t;

typedef struct
{
	thickness_vars_t GT;
	thickness_vars_t FT;
	thickness_vars_t ReT;
	thickness_vars_t TT;
	thickness_vars_t BT;
	thickness_vars_t LT;
	thickness_vars_t RiT;
	BYTE sides;
} thickness_t;

#endif