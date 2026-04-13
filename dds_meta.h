#ifndef _DDS_META_
#define _DDS_META_

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "dxgiformat.h"
#include <stdlib.h>
#include <assert.h>

#define BYTEBUILD(data, offset) (data << offset * 8)
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define RGBA(r,g,b,a) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))
#define DWORDBUILD(r,g,b,a) RGBA(r,g,b,a)
#define N64YUV_CLAMP(val) (val > 255 ? 255 : (val < 0 ? 0 : val))
#define YUY2RGB(y,u,v) ((DWORD)RGB(\
N64YUV_CLAMP((int)(1.164f * (y - 16) + 1.596f * (v - 128))),\
N64YUV_CLAMP((int)(1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128))),\
N64YUV_CLAMP((int)(1.164f * (y - 16) + 2.018f * (u - 128)))))
#define GetRValue(rgba) (LOBYTE(rgba))
#define GetGValue(rgba) (LOBYTE(((WORD)(rgba)) >> 8))
//#define GetBValue(rgba) (LOBYTE((rgba) >> 16))
#define GetAValue(rgba) (LOBYTE((rgba) >> 24))

enum TEX_DIMENSION
{// Subset here matches D3D10_RESOURCE_DIMENSION and D3D11_RESOURCE_DIMENSION
	TEX_DIMENSION_TEXTURE1D    = 2,
	TEX_DIMENSION_TEXTURE2D    = 3,
	TEX_DIMENSION_TEXTURE3D    = 4
};

enum DDS_FLAGS
{
	DDS_FLAGS_NONE = 0x0,
	DDS_FLAGS_LEGACY_DWORD = 0x1, // Assume pitch is DWORD aligned instead of BYTE aligned (used by some legacy DDS files)
	DDS_FLAGS_NO_LEGACY_EXPANSION = 0x2, // Do not implicitly convert legacy formats that result in larger pixel sizes (24 bpp, 3:3:2, A8L8, A4L4, P8, A8P8)
	DDS_FLAGS_NO_R10B10G10A2_FIXUP = 0x4, // Do not use work-around for long-standing D3DX DDS file format issue which reversed the 10:10:10:2 color order masks
	DDS_FLAGS_FORCE_RGB = 0x8, // Convert DXGI 1.1 BGR formats to DXGI_FORMAT_NEW_R8G8B8A8_UNORM to avoid use of optional WDDM 1.1 formats
	DDS_FLAGS_NO_16BPP = 0x10, // Conversions avoid use of 565, 5551, and 4444 formats and instead expand to 8888 to avoid use of optional WDDM 1.2 formats
	DDS_FLAGS_EXPAND_LUMINANCE = 0x20, // When loading legacy luminance formats expand replicating the color channels rather than leaving them packed (L8, L16, A8L8)
	DDS_FLAGS_BAD_DXTN_TAILS = 0x40, // Some older DXTn DDS files incorrectly handle mipchain tails for blocks smaller than 4x4
	DDS_FLAGS_FORCE_DX10_EXT = 0x10000, // Always use the 'DX10' header extension for DDS writer (i.e. don't try to write DX9 compatible DDS files)
	DDS_FLAGS_FORCE_DX10_EXT_MISC2 = 0x20000 // DDS_FLAGS_FORCE_DX10_EXT including miscFlags2 information (result may not be compatible with D3DX10 or D3DX11)
};

enum TEX_ALPHA_MODE
// Matches DDS_ALPHA_MODE, encoded in MISC_FLAGS2
{
	TEX_ALPHA_MODE_UNKNOWN       = 0,
	TEX_ALPHA_MODE_STRAIGHT      = 1,
	TEX_ALPHA_MODE_PREMULTIPLIED = 2,
	TEX_ALPHA_MODE_OPAQUE        = 3,
	TEX_ALPHA_MODE_CUSTOM        = 4
};

enum CONVERSION_FLAGS
{
	CONV_FLAGS_NONE = 0x0,
	CONV_FLAGS_EXPAND = 0x1,      // Conversion requires expanded pixel size
	CONV_FLAGS_NOALPHA = 0x2,      // Conversion requires setting alpha to known value
	CONV_FLAGS_SWIZZLE = 0x4,      // BGR/RGB order swizzling required
	CONV_FLAGS_PAL8 = 0x8,      // Has an 8-bit palette
	CONV_FLAGS_888 = 0x10,     // Source is an 8:8:8 (24bpp) format
	CONV_FLAGS_565 = 0x20,     // Source is a 5:6:5 (16bpp) format
	CONV_FLAGS_5551 = 0x40,     // Source is a 5:5:5:1 (16bpp) format
	CONV_FLAGS_4444 = 0x80,     // Source is a 4:4:4:4 (16bpp) format
	CONV_FLAGS_44 = 0x100,    // Source is a 4:4 (8bpp) format
	CONV_FLAGS_332 = 0x200,    // Source is a 3:3:2 (8bpp) format
	CONV_FLAGS_8332 = 0x400,    // Source is a 8:3:3:2 (16bpp) format
	CONV_FLAGS_A8P8 = 0x800,    // Has an 8-bit palette with an alpha channel
	CONV_FLAGS_DX10 = 0x10000,  // Has the 'DX10' extension header
	CONV_FLAGS_PMALPHA = 0x20000,  // Contains premultiplied alpha data
	CONV_FLAGS_L8 = 0x40000,  // Source is a 8 luminance format
	CONV_FLAGS_L16 = 0x80000,  // Source is a 16 luminance format
	CONV_FLAGS_A8L8 = 0x100000 // Source is a 8:8 luminance format
};

enum CP_FLAGS
{
	CP_FLAGS_NONE               = 0x0,      // Normal operation
	CP_FLAGS_LEGACY_DWORD       = 0x1,      // Assume pitch is DWORD aligned instead of BYTE aligned
	CP_FLAGS_PARAGRAPH          = 0x2,      // Assume pitch is 16-byte aligned instead of BYTE aligned
	CP_FLAGS_YMM                = 0x4,      // Assume pitch is 32-byte aligned instead of BYTE aligned
	CP_FLAGS_ZMM                = 0x8,      // Assume pitch is 64-byte aligned instead of BYTE aligned
	CP_FLAGS_PAGE4K             = 0x200,    // Assume pitch is 4096-byte aligned instead of BYTE aligned
	CP_FLAGS_BAD_DXTN_TAILS     = 0x1000,   // BC formats with malformed mipchain blocks smaller than 4x4
	CP_FLAGS_24BPP              = 0x10000,  // Override with a legacy 24 bits-per-pixel format size
	CP_FLAGS_16BPP              = 0x20000,  // Override with a legacy 16 bits-per-pixel format size
	CP_FLAGS_8BPP               = 0x40000,  // Override with a legacy 8 bits-per-pixel format size
};

enum TEX_MISC_FLAG
// Subset here matches D3D10_RESOURCE_MISC_FLAG and D3D11_RESOURCE_MISC_FLAG
{
	TEX_MISC_TEXTURECUBE = 0x4L
};
enum DDS_RESOURCE_MISC_FLAG
{
    DDS_RESOURCE_MISC_TEXTURECUBE = 0x4L
};
enum DDS_RESOURCE_DIMENSION
{
    DDS_DIMENSION_TEXTURE1D	= 2,
    DDS_DIMENSION_TEXTURE2D	= 3,
    DDS_DIMENSION_TEXTURE3D	= 4
};
enum Errors_MD
{
	ERR_MD_OK = 0,             //it's oK
	ERR_MD_RAED_INFO = 1,      //don't can have read file info
	ERR_MD_FILE_TOO_SMALL = 2, //file too small
	ERR_MD_FILE_TOO_BIG = 3,   //file too big
	ERR_MD_READ_DATA = 4,      //don't can have read file data
	ERR_MD_INVALID_DATA = 5,   //invalid file data
	ERR_MD_SIGNATURE = 6,      //invalid signature
	ERR_MD_HEADER_SIZE = 7,    //invalid header size
	ERR_MD_NOT_SUPPORTED = 8,   //file not supported
	ERR_MD_ALL_OK_NO_EXCEPTION = 9 //костыль, сообщающий, что вылетов точно нет.
};
typedef enum ReadDDSFile_Result2_0
{
	ERR_NF_DLL      = 0x01, //Not found DDS DLL
	ERR_LFF_DLL     = 0x02, //Mistake of the loading function from DLL
	ERR_O_DDS_F     = 0x04, //Mistake open dds file
	ERR_EHR         = 0x08, //Mistake execution GetMetadataFromDDSFile
	ERR_HR_IA_DFOAS = 0x10, //Mistake "GetMetadataFromDDSFile" informed about damage file or absence supporting
	ERR_R_DDS_F     = 0x20, //Mistake read dds file
	ERR_SBIS        = 0x40, //Stopped by the immune system
	ERR_IPFA        = 0x80, //Invalid processings file attributes
	ERR_RF         = 0x100, //Error restoring file
	ERR_ERDF       = 0x200, //Mistake execution "NVd_DDS_Read"
	ERR_AC         = 0x400, //Mistake compute alpha channel
	ERR_DIA        = 0x800  //Mistake execution "DetermineImageArray"

}ReadDDSFile_Result2_0;


#pragma pack(push, 4)
struct TexMetadata
{
	UINT width;
	UINT height;     // Should be 1 for 1D textures
	UINT depth;      // Should be 1 for 1D or 2D textures
	UINT arraySize;  // For cubemap, this is a multiple of 6
	UINT mipLevels;
	UINT miscFlags;
	UINT miscFlags2;
	DXGI_FORMAT_NEW format;
	TEX_DIMENSION dimension;
};

struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwRGBAlphaBitMask;
};

struct DDS_HEADER
{
    UINT dwSize;
    UINT dwFlags;
    UINT dwHeight;
    UINT dwWidth;
    UINT dwPitchOrLinearSize;
    UINT dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwFlags
    UINT dwMipMapCount;
    UINT dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
    UINT dwCaps;
    UINT dwCaps2;
    UINT dwCaps3;
    UINT dwCaps4;
    UINT dwReserved2;
};

struct DDS_HEADER_DXT10
{
    DXGI_FORMAT_NEW dxgiFormat;
    UINT resourceDimension;
    UINT miscFlag; // see DDS_RESOURCE_MISC_FLAG
    UINT arraySize;
    UINT miscFlags2; // see DDS_MISC_FLAGS2
};

struct LegacyDDS
{
	DXGI_FORMAT_NEW     format;
	DWORD           convFlags;
	DDS_PIXELFORMAT ddpf;
};

struct ImageArray
{
	DWORD pixelSize;
	DWORD nImages;
};

struct FullTexMetadata
{
	TexMetadata MData;
	DDS_HEADER DDSH;
	DDS_HEADER_DXT10 DDSH10;
	DWORD Cube;
	ImageArray IA;
	DWORD FileSize;
	DWORD *SlicesPitchs;
	DWORD sRGB;
	DWORD Dx10;
};

struct HedaerData
{
	DWORD result;
	FullTexMetadata FMD;
};


#pragma pack(pop)

#define DDS_HEADER_FLAGS_CAPS				0x00000001L // DDSD_CAPS
#define DDS_HEADER_FLAGS_HEIGHT				0x00000002L // DDSD_HEIGHT
#define DDS_HEADER_FLAGS_WIDTH				0x00000004L // DDSD_WIDTH
#define DDS_HEADER_FLAGS_PITCH				0x00000008L // DDSD_PITCH
#define DDS_HEADER_FLAGS_BACKBUFFERCOUNT    0x00000020L // DDSD_BACKBUFFERCOUNT
#define DDS_HEADER_FLAGS_ZBUFFERBITDEPTH    0x00000040L // DDSD_ZBUFFERBITDEPTH
#define DDS_HEADER_FLAGS_ALPHABITDEPTH      0x00000080L // DDSD_ALPHABITDEPTH
#define DDS_HEADER_FLAGS_LPSURFACE          0x00000800L // DDSD_LPSURFACE
#define DDS_HEADER_FLAGS_PIXELFORMAT		0x00001000L // DDS_HEADER_FLAGS_PIXELFORMAT
#define DDS_HEADER_FLAGS_TEXTURE        	0x00001007L // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
#define DDS_HEADER_FLAGS_CKDESTOVERLAY      0x00002000L // DDSD_CKDESTOVERLAY
#define DDS_HEADER_FLAGS_CKDESTBLT          0x00004000L // DDSD_CKDESTBLT
#define DDS_HEADER_FLAGS_CKSRCOVERLAY       0x00008000L // DDSD_CKSRCOVERLAY
#define DDS_HEADER_FLAGS_CKSRCBLT           0x00010000L // DDSD_CKSRCBLT
#define DDS_HEADER_FLAGS_MIPMAP         	0x00020000L // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_REFRESHRATE        0x00040000L // DDSD_REFRESHRATE
#define DDS_HEADER_FLAGS_LINEARSIZE     	0x00080000L // DDSD_LINEARSIZE
#define DDS_HEADER_FLAGS_VOLUME         	0x00800000L // DDSD_DEPTH
#define DDS_HEADER_FLAGS_TEXTURESTAGE       0x00100000L // DDSD_TEXTURESTAGE
#define DDS_HEADER_FLAGS_FVF                0x00200000L // DDSD_FVF
#define DDS_HEADER_FLAGS_SRCVBHANDLE        0x00400000L // DDSD_SRCVBHANDLE
#define DDS_HEADER_FLAGS_DEPTH              0x00800000L // DDSD_DEPTH
#define DDS_HEADER_FLAGS_ALL                0x00fff9eeL // DDSD_ALL

#define DDS_ALPHAPIXELS  		0x00000001  // DDS_ALPHAPIXELS
#define DDS_ALPHA        		0x00000002  // DDPF_ALPHA
#define DDS_FOURCC       		0x00000004  // DDPF_FOURCC
#define DDS_PAL4         		0x00000008  // DDPF_PALETTEINDEXED4
#define DDS_PALETTEINDEXEDTO8	0x00000010  // DDPF_PALETTEINDEXEDTO8
#define DDS_PAL8         		0x00000020  // DDPF_PALETTEINDEXED8
#define DDS_PAL8A        		0x00000021  // DDPF_PALETTEINDEXED8 | DDS_ALPHAPIXELS
#define DDS_RGB          		0x00000040  // DDPF_RGB
#define DDS_RGBA         		DDS_RGB | DDS_ALPHAPIXELS  // DDPF_RGB | DDS_ALPHAPIXELS
#define DDS_COMPRESSED          0x00000080  // DDPF_COMPRESSED
#define DDS_RGBTOYUV            0x00000100  // DDPF_RGBTOYUV
#define DDS_YUV			 		0x00000200  // DDPF_YUV
#define DDS_ZBUFFER             0x00000400  // DDPF_ZBUFFER
#define DDS_PALETTEINDEXED1     0x00000800  // DDPF_PALETTEINDEXED1
#define DDS_PALETTEINDEXED2     0x00001000  // DDPF_PALETTEINDEXED2
#define DDS_ZPIXELS             0x00002000  // DDPF_ZPIXELS
#define DDS_STENCILBUFFER       0x00004000  // DDPF_STENCILBUFFER
#define DDS_ALPHAPREMULT        0x00008000  // DDPF_ALPHAPREMULT
#define DDS_LUMINANCE    		0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA   		DDS_LUMINANCE | DDS_ALPHAPIXELS  // DDPF_LUMINANCE | DDS_ALPHAPIXELS
#define DDS_BUMPLUMINANCE       0x00040000  //DDPF_BUMPLUMINANCE
#define DDS_BUMPDUDV     		0x00080000  // DDPF_BUMPDUDV
#define DDS_BUMPDUDVA    		DDS_BUMPDUDV | DDS_ALPHAPIXELS  // DDPF_BUMPDUDV | DDS_ALPHAPIXELS
#define DDS_SRGB		 		0x40000000  //DDPF_SRGB
#define DDS_NORMAL       		0x80000000  // nvidia specific

#define DDSCAPS_RESERVED1           0x00000001L
#define DDSCAPS_ALPHA               0x00000002L
#define DDSCAPS_BACKBUFFER          0x00000004L
#define DDSCAPS_COMPLEX             0x00000008L
#define DDSCAPS_FLIP                0x00000010L
#define DDSCAPS_FRONTBUFFER         0x00000020L
#define DDSCAPS_OFFSCREENPLAIN      0x00000040L
#define DDSCAPS_OVERLAY             0x00000080L
#define DDSCAPS_PALETTE             0x00000100L
#define DDSCAPS_PRIMARYSURFACE      0x00000200L
#define DDSCAPS_RESERVED3           0x00000400L
#define DDSCAPS_PRIMARYSURFACELEFT  0x00000000L
#define DDSCAPS_SYSTEMMEMORY        0x00000800L
#define DDSCAPS_TEXTURE             0x00001000L
#define DDSCAPS_3DDEVICE            0x00002000L
#define DDSCAPS_VIDEOMEMORY         0x00004000L
#define DDSCAPS_VISIBLE             0x00008000L
#define DDSCAPS_WRITEONLY           0x00010000L
#define DDSCAPS_ZBUFFER             0x00020000L
#define DDSCAPS_OWNDC               0x00040000L
#define DDSCAPS_LIVEVIDEO           0x00080000L
#define DDSCAPS_HWCODEC             0x00100000L
#define DDSCAPS_MODEX               0x00200000L
#define DDSCAPS_MIPMAP              0x00400000L
#define DDSCAPS_RESERVED2           0x00800000L
#define DDSCAPS_ALLOCONLOAD         0x04000000L
#define DDSCAPS_VIDEOPORT           0x08000000L
#define DDSCAPS_LOCALVIDMEM         0x10000000L
#define DDSCAPS_NONLOCALVIDMEM      0x20000000L
#define DDSCAPS_STANDARDVGAMODE     0x40000000L
#define DDSCAPS_OPTIMIZED           0x80000000L

#define DDSCAPS2_HARDWAREDEINTERLACE 	0x00000000L
#define DDSCAPS2_RESERVED4           	0x00000002L
#define DDSCAPS2_HINTDYNAMIC         	0x00000004L
#define DDSCAPS2_HINTSTATIC          	0x00000008L
#define DDSCAPS2_TEXTUREMANAGE       	0x00000010L
#define DDSCAPS2_RESERVED1              0x00000020L
#define DDSCAPS2_RESERVED2              0x00000040L
#define DDSCAPS2_OPAQUE              	0x00000080L
#define DDSCAPS2_HINTANTIALIASING    	0x00000100L
#define DDSCAPS2_CUBEMAP			 	0x00000200L // DDSCAPS2_CUBEMAP
#define DDSCAPS2_CUBEMAP_POSITIVEX	 	0x00000400L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDSCAPS2_CUBEMAP_NEGATIVEX	 	0x00000800L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDSCAPS2_CUBEMAP_POSITIVEY	 	0x00001000L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDSCAPS2_CUBEMAP_NEGATIVEY	 	0x00002000L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDSCAPS2_CUBEMAP_POSITIVEZ	 	0x00004000L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	 	0x00008000L // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ
#define DDSCAPS2_CUBEMAP_ALL_FACES \
   (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | \
	DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | \
	DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)
#define DDSCAPS2_MIPMAPSUBLEVEL         0x00010000L
#define DDSCAPS2_D3DTEXTUREMANAGE       0x00020000L
#define DDSCAPS2_DONOTPERSIST           0x00040000L
#define DDSCAPS2_STEREOSURFACELEFT      0x00080000L
#define DDSCAPS2_VOLUME                 0x00200000L // DDSCAPS2_VOLUME
#define DDSCAPS2_NOTUSERLOCKABLE        0x00400000L
#define DDSCAPS2_POINTS                 0x00800000L
#define DDSCAPS2_RTPATCHES              0x01000000L
#define DDSCAPS2_NPATCHES               0x02000000L
#define DDSCAPS2_RESERVED3              0x04000000L
#define DDSCAPS2_DISCARDBACKBUFFER      0x10000000L
#define DDSCAPS2_ENABLEALPHACHANNEL     0x20000000L
#define DDSCAPS2_EXTENDEDFORMATPRIMARY  0x40000000L
#define DDSCAPS2_ADDITIONALPRIMARY      0x80000000L

#define MONOHROM(m) ((COLORREF)(((BYTE)(m)|((WORD)((BYTE)(m))<<8))|(((DWORD)(BYTE)(m))<<16)))

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif

//const DDS_PIXELFORMAT DDSPF_DXT1 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_DXT2 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_DXT3 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_DXT4 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_DXT5 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_BC4_UNORM = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','4','U'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_BC4_SNORM = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','4','S'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_BC5_UNORM = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','5','U'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_BC5_SNORM = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','5','S'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_R8G8_B8G8 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R','G','B','G'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_G8R8_G8B8 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('G','R','G','B'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_YUY2 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('Y','U','Y','2'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_A8R8G8B8 = {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_X8R8G8B8 = {sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_A8B8G8R8 = {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_X8B8G8R8 = {sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_G16R16 = {sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_R5G6B5 = {sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_A1R5G5B5 = {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000};
//const DDS_PIXELFORMAT DDSPF_A4R4G4B4 = {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000};
//const DDS_PIXELFORMAT DDSPF_R8G8B8 = {sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_A0R8G8B8 = {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_L8 = {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCE, 0,  8, 0xff, 0x00, 0x00, 0x00};
//const DDS_PIXELFORMAT DDSPF_L16 = {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCE, 0, 16, 0xffff, 0x0000, 0x0000, 0x0000};
//const DDS_PIXELFORMAT DDSPF_A8L8 = {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 16, 0x00ff, 0x0000, 0x0000, 0xff00};
//const DDS_PIXELFORMAT DDSPF_A8L8_ALT = {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 8, 0x00ff, 0x0000, 0x0000, 0xff00};
//const DDS_PIXELFORMAT DDSPF_A8 = {sizeof(DDS_PIXELFORMAT), DDS_ALPHA, 0, 8, 0x00, 0x00, 0x00, 0xff};
//const DDS_PIXELFORMAT DDSPF_V8U8 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0x0000, 0x0000};
//const DDS_PIXELFORMAT DDSPF_Q8W8V8U8 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_V16U16 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000};
//// D3DFMT_A2R10G10B10/D3DFMT_A2B10G10R10 should be written using DX10 extension to avoid D3DX 10:10:10:2 reversal issue
//// This indicates the DDS_HEADER_DXT10 extension is present (the format is in dxgiFormat)
//const DDS_PIXELFORMAT DDSPF_DX10 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','1','0'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_A2W10V10U10 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDVA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000};
//const DDS_PIXELFORMAT DDSPF_A4L4 = {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 8, 0x0000000f, 0x00000000, 0x00000000, 0x000000f0};
//const DDS_PIXELFORMAT DDSPF_CxV8U8 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 117, 0, 0x00, 0x00, 0x00, 0x00};
//const DDS_PIXELFORMAT DDSPF_L6V5U5 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPLUMINANCE, 0, 16, 0x0000001f, 0x000003e0, 0x0000fc00, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_P4 = {sizeof(DDS_PIXELFORMAT), DDS_PAL4, 0, 4, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_RXGB_PF = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC | DDS_NORMAL, MAKEFOURCC('R', 'X', 'G', 'B'), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_UYVY = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('U','Y','V','Y'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ATI1 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'I', '1'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ATI2 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'I', '2'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_P4P8A = {sizeof(DDS_PIXELFORMAT), DDS_ALPHAPIXELS | DDS_PAL4 | DDS_PAL8, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_X8L8V8U8 = {sizeof(DDS_PIXELFORMAT), DDS_BUMPLUMINANCE, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00};
//const DDS_PIXELFORMAT DDSPF_Q8W8V8U8_FCC = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 63, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
//const DDS_PIXELFORMAT DDSPF_D16_LOCKABLE = {sizeof(DDS_PIXELFORMAT), DDS_ZBUFFER, 0, 16, 0x00000000, 0x0000ffff, 0x00000000, 0x00000000};
//const DDS_PIXELFORMAT DDSPF_D32F_LOCKABLE = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 82, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

//const DDS_PIXELFORMAT DDSPF_ATC = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', ' '), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ATCA = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', 'A'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ATCI = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', 'I'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_A2D5 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', '2', 'D', '5'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_RBXG = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'B', 'x', 'G'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_RGXB = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'G', 'x', 'B'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_RXBG = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'x', 'B', 'G'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_XGBR = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'G', 'B', 'R'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_XGXR = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'G', 'x', 'R'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_XRBG = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'R', 'B', 'G'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ETC = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', ' '), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ETC2 = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', '2'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ETCA = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', 'A'), 0, 0, 0, 0, 0};
//const DDS_PIXELFORMAT DDSPF_ETCP = {sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', 'P'), 0, 0, 0, 0, 0};

const LegacyDDS g_LegacyDDSMap[] =
{
	{DXGI_FORMAT_NEW_BC1_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0}}, // D3DFMT_DXT1
	{DXGI_FORMAT_NEW_BC2_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0}}, // D3DFMT_DXT3
	{DXGI_FORMAT_NEW_BC3_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0}}, // D3DFMT_DXT5
	{DXGI_FORMAT_NEW_BC2_UNORM,			CONV_FLAGS_PMALPHA,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0}}, // D3DFMT_DXT2
	{DXGI_FORMAT_NEW_BC3_UNORM,			CONV_FLAGS_PMALPHA,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0}}, // D3DFMT_DXT4
	{DXGI_FORMAT_NEW_BC4_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','4','U'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_BC4_SNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','4','S'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_BC5_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','5','U'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_BC5_SNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('B','C','5','S'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_BC4_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A','T','I','1'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_BC5_UNORM,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A','T','I','2'), 0, 0, 0, 0, 0}},
	{DXGI_FORMAT_NEW_R8G8_B8G8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R','G','B','G'), 0, 0, 0, 0, 0}}, // D3DFMT_R8G8_B8G8
	{DXGI_FORMAT_NEW_G8R8_G8B8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('G','R','G','B'), 0, 0, 0, 0, 0}}, // D3DFMT_G8R8_G8B8
	{DXGI_FORMAT_NEW_B8G8R8A8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}}, // D3DFMT_A8R8G8B8 (uses DXGI 1.1 format)
	{DXGI_FORMAT_NEW_B8G8R8X8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000}}, // D3DFMT_X8R8G8B8 (uses DXGI 1.1 format)
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}}, // D3DFMT_A8B8G8R8
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_NOALPHA,	{sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000}}, // D3DFMT_X8B8G8R8
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_NOALPHA,	{sizeof(DDS_PIXELFORMAT), DDS_ALPHAPIXELS | DDS_PAL4 | DDS_PAL8, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}}, // D3DFMT_X8B8G8R8
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_NOALPHA,	{sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000}}, // D3DFMT_X8B8G8R8
	{DXGI_FORMAT_NEW_R16G16_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGB,  0, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000}}, // D3DFMT_G16R16
	{DXGI_FORMAT_NEW_R10G10B10A2_UNORM,	CONV_FLAGS_SWIZZLE,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA,	0, 32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000}}, // D3DFMT_A2R10G10B10 (D3DX reversal issue workaround)
	{DXGI_FORMAT_NEW_R10G10B10A2_UNORM,  CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA,	0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000}}, // D3DFMT_A2B10G10R10 (D3DX reversal issue workaround)
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_EXPAND | CONV_FLAGS_NOALPHA | CONV_FLAGS_888, {sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000}}, // D3DFMT_R8G8B8
	{DXGI_FORMAT_NEW_B5G6R5_UNORM,		CONV_FLAGS_565,	{sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000}}, // D3DFMT_R5G6B5
	{DXGI_FORMAT_NEW_B5G6R5_UNORM,		CONV_FLAGS_565,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 23, 0, 0, 0, 0, 0}}, // D3DFMT_R5G6B5
	{DXGI_FORMAT_NEW_B5G5R5A1_UNORM,	CONV_FLAGS_5551, {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000}}, // D3DFMT_A1R5G5B5
	{DXGI_FORMAT_NEW_B5G5R5A1_UNORM,	CONV_FLAGS_5551 | CONV_FLAGS_NOALPHA, {sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x0000}}, // D3DFMT_X1R5G5B5
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,	CONV_FLAGS_EXPAND | CONV_FLAGS_8332, {sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00e0, 0x001c, 0x0003, 0xff00}}, // D3DFMT_A8R3G3B2
	{DXGI_FORMAT_NEW_R3G3B2, CONV_FLAGS_EXPAND | CONV_FLAGS_332, {sizeof(DDS_PIXELFORMAT), DDS_RGB, 0,  8, 0xe0, 0x1c, 0x03, 0x00}}, // D3DFMT_R3G3B2
	{DXGI_FORMAT_NEW_R8_UNORM,			CONV_FLAGS_NONE, {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCE, 0,  8, 0xff, 0x00, 0x00, 0x00}}, // D3DFMT_L8
	{DXGI_FORMAT_NEW_R16_UNORM,			CONV_FLAGS_NONE, {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCE, 0, 16, 0xffff, 0x0000, 0x0000, 0x0000}}, // D3DFMT_L16
	{DXGI_FORMAT_NEW_A8L8,		CONV_FLAGS_NONE, {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 16, 0x00ff, 0x0000, 0x0000, 0xff00}}, // D3DFMT_A8L8
	{DXGI_FORMAT_NEW_A8L8,		CONV_FLAGS_NONE, {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA, 0, 8, 0x00ff, 0x0000, 0x0000, 0xff00}}, // D3DFMT_A8L8 (alternative bitcount)
	{DXGI_FORMAT_NEW_A8_UNORM, CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_ALPHA, 0, 8, 0x00, 0x00, 0x00, 0xff}}, // D3DFMT_A8
	{DXGI_FORMAT_NEW_R16G16B16A16_UNORM, CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 36, 0, 0, 0, 0, 0}}, // D3DFMT_A16B16G16R16
	{DXGI_FORMAT_NEW_R16G16B16A16_SNORM, CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 110, 0, 0, 0, 0, 0}}, // D3DFMT_Q16W16V16U16
	{DXGI_FORMAT_NEW_R16_FLOAT,          CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 111, 0, 0, 0, 0, 0}}, // D3DFMT_R16F
	{DXGI_FORMAT_NEW_R16G16_FLOAT,       CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 112, 0, 0, 0, 0, 0}}, // D3DFMT_G16R16F
	{DXGI_FORMAT_NEW_R16G16B16A16_FLOAT, CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 113, 0, 0, 0, 0, 0}}, // D3DFMT_A16B16G16R16F
	{DXGI_FORMAT_NEW_R32_FLOAT,          CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 114, 0, 0, 0, 0, 0}}, // D3DFMT_R32F
	{DXGI_FORMAT_NEW_R32G32_FLOAT,       CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 115, 0, 0, 0, 0, 0}}, // D3DFMT_G32R32F
	{DXGI_FORMAT_NEW_R32G32B32A32_FLOAT, CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 116, 0, 0, 0, 0, 0}}, // D3DFMT_A32B32G32R32F
	{DXGI_FORMAT_NEW_R32_FLOAT,          CONV_FLAGS_NONE,	{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 32, 0xffffffff, 0x00000000, 0x00000000, 0x00000000}}, // D3DFMT_R32F (D3DX uses FourCC 114 instead)
	{DXGI_FORMAT_NEW_R8G8B8A8_UNORM,     CONV_FLAGS_EXPAND | CONV_FLAGS_PAL8 | CONV_FLAGS_A8P8,	{ sizeof(DDS_PIXELFORMAT), DDS_PAL8A, 0, 16, 0, 0, 0, 0}}, // D3DFMT_A8P8
	{DXGI_FORMAT_NEW_P8,				 CONV_FLAGS_EXPAND | CONV_FLAGS_PAL8,	{ sizeof(DDS_PIXELFORMAT), DDS_PAL8, 0,  8, 0, 0, 0, 0}}, // D3DFMT_P8
	{DXGI_FORMAT_NEW_B4G4R4A4_UNORM,     CONV_FLAGS_4444,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000} }, // D3DFMT_A4R4G4B4 (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_B4G4R4A4_UNORM,     CONV_FLAGS_NOALPHA | CONV_FLAGS_4444,	{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0f00, 0x00f0, 0x000f, 0x0000}}, // D3DFMT_X4R4G4B4 (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_R4G4B4A4_UNORM,     CONV_FLAGS_4444,	{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 26, 16, 0x0f00, 0x000f0, 0x0000f, 0xf000}}, // D3DFMT_X4R4G4B4 (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_A4L4, CONV_FLAGS_EXPAND | CONV_FLAGS_44, {sizeof(DDS_PIXELFORMAT), DDS_LUMINANCEA,0, 8, 0x0f, 0x00, 0x00, 0xf0}}, // D3DFMT_A4L4 (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_YUY2,               CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('Y','U','Y','2'), 0, 0, 0, 0, 0}}, // D3DFMT_YUY2 (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_UYVY,               CONV_FLAGS_SWIZZLE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('U','Y','V','Y'), 0, 0, 0, 0, 0}}, // D3DFMT_UYVY (uses DXGI 1.2 format)
	{DXGI_FORMAT_NEW_R8G8_SNORM,         CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0x0000, 0x0000}}, // D3DFMT_V8U8
	{DXGI_FORMAT_NEW_Q8W8V8U8,     CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}}, // D3DFMT_Q8W8V8U8
	{DXGI_FORMAT_NEW_R16G16_SNORM,       CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000}}, // D3DFMT_V16U16
	{DXGI_FORMAT_NEW_A2W10V10U10,       CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPDUDVA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000}}, //DDSPF_A2W10V10U10
	{DXGI_FORMAT_NEW_CxV8U8,       CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 117, 0, 0x00, 0x00, 0x00, 0x00}}, //DDSPF_CxV8U8
	{DXGI_FORMAT_NEW_L6V5U5,       CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPLUMINANCE, 0, 16, 0x0000001f, 0x000003e0, 0x0000fc00, 0x00000000}}, //DDSPF_L6V5U5
	{DXGI_FORMAT_NEW_P4,       CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_PAL4, 0, 4, 0x00000000, 0x00000000, 0x00000000, 0x00000000}}, //DDSPF_P4
	{DXGI_FORMAT_NEW_B8G8R8A8_UNORM,	CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}}, // D3DFMT_X8B8G8R8
	{DXGI_FORMAT_NEW_RXGB,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC | DDS_NORMAL, MAKEFOURCC('R', 'X', 'G', 'B'), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000}}, // DDSPF_RXGB_PF
	{DXGI_FORMAT_NEW_X8L8V8U8,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_BUMPLUMINANCE, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00}}, // D3DFMT_X8L8V8U8
	{DXGI_FORMAT_NEW_Q8W8V8U8,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 63, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000}}, // DDSPF_Q8W8V8U8_FCC

	{DXGI_FORMAT_NEW_ATC,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', ' '), 0, 0, 0, 0, 0}}, // DDSPF_ATC
	{DXGI_FORMAT_NEW_ATCA,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', 'A'), 0, 0, 0, 0, 0}}, // DDSPF_ATCA
	{DXGI_FORMAT_NEW_ATCI,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', 'T', 'C', 'I'), 0, 0, 0, 0, 0}}, // DDSPF_ATCI
	{DXGI_FORMAT_NEW_A2D5,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('A', '2', 'D', '5'), 0, 0, 0, 0, 0}}, // DDSPF_A2D5
	{DXGI_FORMAT_NEW_RBXG,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'B', 'x', 'G'), 0, 0, 0, 0, 0}}, // DDSPF_RBXG
	{DXGI_FORMAT_NEW_RGXB,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'G', 'x', 'B'), 0, 0, 0, 0, 0}}, // DDSPF_RGXB
	{DXGI_FORMAT_NEW_RXBG,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('R', 'x', 'B', 'G'), 0, 0, 0, 0, 0}}, // DDSPF_RXBG
	{DXGI_FORMAT_NEW_XGBR,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'G', 'B', 'R'), 0, 0, 0, 0, 0}}, // DDSPF_XGBR
	{DXGI_FORMAT_NEW_XGXR,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'G', 'x', 'R'), 0, 0, 0, 0, 0}}, // DDSPF_XGXR
	{DXGI_FORMAT_NEW_XRBG,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('x', 'R', 'B', 'G'), 0, 0, 0, 0, 0}}, // DDSPF_XRBG
	{DXGI_FORMAT_NEW_ETC,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', ' '), 0, 0, 0, 0, 0}}, // DDSPF_ETC
	{DXGI_FORMAT_NEW_ETC2,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', '2'), 0, 0, 0, 0, 0}}, // DDSPF_ETC2
	{DXGI_FORMAT_NEW_ETCA,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', 'A'), 0, 0, 0, 0, 0}}, // DDSPF_ETCA
	{DXGI_FORMAT_NEW_ETCP,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('E', 'T', 'C', 'P'), 0, 0, 0, 0, 0}}, // DDSPF_ETCP

	{DXGI_FORMAT_NEW_D16_LOCKABLE,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_ZBUFFER, 0, 16, 0x00000000, 0x0000ffff, 0x00000000, 0x00000000}}, // DDSPF_D16_LOCKABLE
	{DXGI_FORMAT_NEW_D32F_LOCKABLE,			CONV_FLAGS_NONE,	{sizeof(DDS_PIXELFORMAT), DDS_FOURCC, 82, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000000}}, // DDSPF_D32F_LOCKABLE
};

DWORD GetMetadataFromDDSFile(HANDLE fl, DWORD flags, FullTexMetadata& FMD);
DXGI_FORMAT_NEW GetDXGIFormat(const DDS_HEADER& hdr, const DDS_PIXELFORMAT& ddpf, DWORD flags, DWORD& convFlags);
DXGI_FORMAT_NEW MakeSRGB(DXGI_FORMAT_NEW fmt);
void DetermineImageArray(FullTexMetadata& FMD, DWORD cpFlags, bool dx10);
void ComputePitch(DXGI_FORMAT_NEW fmt, size_t width, size_t height,
	size_t& rowPitch, size_t& slicePitch, DWORD flags, DWORD PF_BPP);
bool IsCompressed(DXGI_FORMAT_NEW fmt);
bool IsPalettized(DXGI_FORMAT_NEW fmt);
bool IsPacked(DXGI_FORMAT_NEW fmt);
bool IsPlanar(DXGI_FORMAT_NEW fmt);
bool IsValid(DXGI_FORMAT_NEW fmt);
size_t BitsPerPixel(DXGI_FORMAT_NEW fmt);
bool IsSRGB(DXGI_FORMAT_NEW fmt);

DWORD GetMetadataFromDDSFile(HANDLE fl, DWORD flags, FullTexMetadata& FMD)
{
	// Get the file size
	FILE_STANDARD_INFO fileInfo;
    if(!GetFileInformationByHandleEx(fl, FileStandardInfo, &fileInfo, sizeof(fileInfo)))
		{return ERR_MD_RAED_INFO;}
    // File is too big for 32-bit allocation, so reject read (4 GB should be plenty large enough for a valid DDS file)
    if(fileInfo.EndOfFile.HighPart > 0)
		{return ERR_MD_FILE_TOO_BIG;}
    // Need at least enough data to fill the standard header and magic number to be a valid DDS
    if(fileInfo.EndOfFile.LowPart < (sizeof(DDS_HEADER) + sizeof(UINT)))
		{return ERR_MD_FILE_TOO_SMALL;}
    // Read the header in (including extended header if present)
    const size_t MAX_HEADER_SIZE = sizeof(UINT) + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10);
    BYTE header[MAX_HEADER_SIZE];
	DWORD bytesRead = 0;
    if(!ReadFile(fl, header, MAX_HEADER_SIZE, &bytesRead, NULL))
		{return ERR_MD_READ_DATA;}
    DWORD convFlags = 0;
	if(bytesRead < (sizeof(DDS_HEADER) + sizeof(UINT)))
		{return ERR_MD_INVALID_DATA;}
	// DDS files always start with the same magic number ("DDS ")
	UINT dwMagicNumber = *reinterpret_cast<const UINT*>(header);
	if(dwMagicNumber != 0x20534444)//"DDS "
		{return ERR_MD_SIGNATURE;}
	DDS_HEADER *pHeader = reinterpret_cast<DDS_HEADER*>((BYTE*)header + sizeof(UINT));
	memcpy((void*)&FMD.DDSH, (void*)pHeader, sizeof(DDS_HEADER));
	FMD.FileSize = fileInfo.EndOfFile.LowPart;

	/*char text_buff[2048] = {0};
	char making_text_buff[100] = {0};
	sprintf(making_text_buff, "dwSize %s\n", Int32ToHexString(pHeader -> dwSize)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwFlags %s\n", Int32ToHexString(pHeader -> dwFlags)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwHeight %s\n", Int32ToHexString(pHeader -> dwHeight)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwWidth %s\n", Int32ToHexString(pHeader -> dwWidth )); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwPitchOrLinearSize %s\n", Int32ToHexString(pHeader -> dwPitchOrLinearSize)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwMipMapCount %s\n", Int32ToHexString(pHeader -> dwMipMapCount)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwReserved1 [%s]", Int32ToHexString(pHeader -> dwReserved1[0])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[1])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[2])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[3])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[4])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[5])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[6])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[7])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[8])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]", Int32ToHexString(pHeader -> dwReserved1[9])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "[%s]\n", Int32ToHexString(pHeader -> dwReserved1[10])); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwSize %s\n", Int32ToHexString(pHeader -> ddspf.dwSize)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwFlags %s\n", Int32ToHexString(pHeader -> ddspf.dwFlags)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwFourCC %s\n", Int32ToHexString(pHeader -> ddspf.dwFourCC)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwRGBBitCount %s\n", Int32ToHexString(pHeader -> ddspf.dwRGBBitCount)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwRBitMask %s\n", Int32ToHexString(pHeader -> ddspf.dwRBitMask)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwGBitMask %s\n", Int32ToHexString(pHeader -> ddspf.dwGBitMask)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwBBitMask %s\n", Int32ToHexString(pHeader -> ddspf.dwBBitMask)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "ddspf dwABitMask %s\n", Int32ToHexString(pHeader -> ddspf.dwRGBAlphaBitMask)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwCaps %s\n", Int32ToHexString(pHeader -> dwCaps)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwCaps2 %s\n", Int32ToHexString(pHeader -> dwCaps2)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwCaps3 %s\n", Int32ToHexString(pHeader -> dwCaps3)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwCaps4 %s\n", Int32ToHexString(pHeader -> dwCaps4)); strcat(text_buff, making_text_buff);
	sprintf(making_text_buff, "dwReserved2 %s\n", Int32ToHexString(pHeader -> dwReserved2)); strcat(text_buff, making_text_buff);
	MessageBoxA(NULL, text_buff, "", MB_OK | MB_ICONEXCLAMATION);*/

	// Verify header to validate DDS file
	//--------------------------------------------------------------------------
	if(pHeader -> dwSize != sizeof(DDS_HEADER))
		{return ERR_MD_INVALID_DATA;}
	if(pHeader->ddspf.dwSize != sizeof(DDS_PIXELFORMAT))
		{return ERR_MD_INVALID_DATA;}
	//--------------------------------------------------------------------------
	FMD.MData.mipLevels = pHeader->dwMipMapCount;
	if(FMD.MData.mipLevels == 0)
		{FMD.MData.mipLevels = 1;}
	// Check for DX10 extension

	if((pHeader->ddspf.dwFlags & DDS_FOURCC)
            && (MAKEFOURCC('D', 'X', '1', '0') == pHeader->ddspf.dwFourCC))
	{
		// Buffer must be big enough for both headers and magic value
		if(bytesRead < (sizeof(DDS_HEADER) + sizeof(UINT) + sizeof(DDS_HEADER_DXT10)))
			{return ERR_MD_HEADER_SIZE;}
		const DDS_HEADER_DXT10 *d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>((const BYTE*)header + sizeof(UINT) + sizeof(DDS_HEADER));
		memcpy((void*)&FMD.DDSH10, (void*)d3d10ext, sizeof(DDS_HEADER_DXT10));
		convFlags |= CONV_FLAGS_DX10;
		FMD.MData.arraySize = d3d10ext->arraySize;
		if(FMD.MData.arraySize == 0)
			{return ERR_MD_INVALID_DATA;}
		FMD.MData.format = d3d10ext->dxgiFormat;
		if(!((UINT)FMD.MData.format >= 0 && (UINT)FMD.MData.format <= 190))
		{
			return ERR_MD_NOT_SUPPORTED;
		}
		FMD.MData.miscFlags = d3d10ext->miscFlag & ~TEX_MISC_TEXTURECUBE;
		switch(d3d10ext->resourceDimension)
		{
			case DDS_DIMENSION_TEXTURE1D:
                // D3DX writes 1D textures with a fixed Height of 1
				if((pHeader->dwFlags & DDS_HEADER_FLAGS_HEIGHT) && pHeader->dwHeight != 1)
					{return ERR_MD_INVALID_DATA;}
				FMD.MData.width = pHeader->dwWidth;
				FMD.MData.height = 1;
				FMD.MData.depth = 1;
				FMD.MData.dimension = TEX_DIMENSION_TEXTURE1D;
			break;
            case DDS_DIMENSION_TEXTURE2D:
				if(d3d10ext->miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
				{
					FMD.MData.miscFlags |= TEX_MISC_TEXTURECUBE;
					FMD.MData.arraySize *= 6;
				}
				FMD.MData.width = pHeader->dwWidth;
				FMD.MData.height = pHeader->dwHeight;
				FMD.MData.depth = 1;
				FMD.MData.dimension = TEX_DIMENSION_TEXTURE2D;
			break;
			case DDS_DIMENSION_TEXTURE3D:
				if(!(pHeader->dwFlags & DDS_HEADER_FLAGS_VOLUME))
					{return ERR_MD_INVALID_DATA;}
				if(FMD.MData.arraySize > 1)
					{return ERR_MD_NOT_SUPPORTED;}
				FMD.MData.width = pHeader->dwWidth;
				FMD.MData.height = pHeader->dwHeight;
				FMD.MData.depth = pHeader->dwDepth;
				FMD.MData.dimension = TEX_DIMENSION_TEXTURE3D;
			break;
			default:
				{return ERR_MD_INVALID_DATA;}
		}
		FMD.MData.miscFlags2 = d3d10ext->miscFlags2;
	}
	else
	{
		FMD.MData.arraySize = 1;
		if(pHeader->dwFlags & DDS_HEADER_FLAGS_VOLUME)
		{
			FMD.MData.width = pHeader->dwWidth;
			FMD.MData.height = pHeader->dwHeight;
			FMD.MData.depth = pHeader->dwDepth;
			FMD.MData.dimension = TEX_DIMENSION_TEXTURE3D;
		}
		else
		{
			if(pHeader->dwCaps2 & DDSCAPS2_CUBEMAP)
			{
				// We require all six faces to be defined
				if((pHeader->dwCaps2 & DDSCAPS2_CUBEMAP_ALL_FACES) != DDSCAPS2_CUBEMAP_ALL_FACES)
					{return ERR_MD_NOT_SUPPORTED;}
				FMD.MData.arraySize = 6;
				FMD.MData.miscFlags |= TEX_MISC_TEXTURECUBE;
			}
			FMD.MData.width = pHeader->dwWidth;
			FMD.MData.height = pHeader->dwHeight;
			FMD.MData.depth = 1;
			FMD.MData.dimension = TEX_DIMENSION_TEXTURE2D;
			// Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
		}
		FMD.MData.format = GetDXGIFormat(*pHeader, pHeader->ddspf, flags, convFlags);
		if(convFlags & CONV_FLAGS_PMALPHA)
			{FMD.MData.miscFlags2 |= TEX_ALPHA_MODE_PREMULTIPLIED;}
		// Special flag for handling LUMINANCE legacy formats
		if(flags & DDS_FLAGS_EXPAND_LUMINANCE)
		{
			switch(FMD.MData.format)
			{
				case DXGI_FORMAT_NEW_R8_UNORM:
					FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM;
					convFlags |= CONV_FLAGS_L8 | CONV_FLAGS_EXPAND;
				break;
				case DXGI_FORMAT_NEW_R8G8_UNORM:
					FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM;
					convFlags |= CONV_FLAGS_A8L8 | CONV_FLAGS_EXPAND;
				break;
				case DXGI_FORMAT_NEW_R16_UNORM:
					FMD.MData.format = DXGI_FORMAT_NEW_R16G16B16A16_UNORM;
					convFlags |= CONV_FLAGS_L16 | CONV_FLAGS_EXPAND;
				break;
				default:
				break;
			}
		}
	}
	// Special flag for handling BGR DXGI 1.1 formats
	if(flags & DDS_FLAGS_FORCE_RGB)
	{
		switch(FMD.MData.format)
		{
			case DXGI_FORMAT_NEW_B8G8R8A8_UNORM:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM;
				convFlags |= CONV_FLAGS_SWIZZLE;
			break;
			case DXGI_FORMAT_NEW_B8G8R8X8_UNORM:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM;
				convFlags |= CONV_FLAGS_SWIZZLE | CONV_FLAGS_NOALPHA;
			break;
			case DXGI_FORMAT_NEW_B8G8R8A8_TYPELESS:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_TYPELESS;
				convFlags |= CONV_FLAGS_SWIZZLE;
			break;
			case DXGI_FORMAT_NEW_B8G8R8A8_UNORM_SRGB:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM_SRGB;
				convFlags |= CONV_FLAGS_SWIZZLE;
			break;
			case DXGI_FORMAT_NEW_B8G8R8X8_TYPELESS:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_TYPELESS;
				convFlags |= CONV_FLAGS_SWIZZLE | CONV_FLAGS_NOALPHA;
			break;
			case DXGI_FORMAT_NEW_B8G8R8X8_UNORM_SRGB:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM_SRGB;
				convFlags |= CONV_FLAGS_SWIZZLE | CONV_FLAGS_NOALPHA;
			break;
			default:
			break;
		}
	}
	// Special flag for handling 16bpp formats

	if(flags & DDS_FLAGS_NO_16BPP)
	{
		switch(FMD.MData.format)
		{
			case DXGI_FORMAT_NEW_B5G6R5_UNORM:
			case DXGI_FORMAT_NEW_B5G5R5A1_UNORM:
			case DXGI_FORMAT_NEW_B4G4R4A4_UNORM:
			case DXGI_FORMAT_NEW_R4G4B4A4_UNORM:
				FMD.MData.format = DXGI_FORMAT_NEW_R8G8B8A8_UNORM;
				convFlags |= CONV_FLAGS_EXPAND;
				if(FMD.MData.format == DXGI_FORMAT_NEW_B5G6R5_UNORM)
					{convFlags |= CONV_FLAGS_NOALPHA;}
			default:
			break;
		}
	}
	FMD.sRGB = IsSRGB(FMD.MData.format);
	if(FMD.MData.format)
		{DetermineImageArray(FMD, 0, (convFlags & CONV_FLAGS_DX10) ? true : false);}
	else
		{return ERR_MD_NOT_SUPPORTED;}
	if(pHeader -> dwCaps2 & DDSCAPS2_CUBEMAP)
		{FMD.Cube = TRUE;}
	FMD.Dx10 = (FMD.DDSH.ddspf.dwFourCC == MAKEFOURCC('D', 'X', '1', '0') && FMD.DDSH.ddspf.dwFlags & DDS_FOURCC);
	return ERR_MD_ALL_OK_NO_EXCEPTION;
}

DXGI_FORMAT_NEW GetDXGIFormat(const DDS_HEADER& hdr, const DDS_PIXELFORMAT& ddpf, DWORD flags, DWORD& convFlags)
{
	UINT ddpfFlags = ddpf.dwFlags;
	if(hdr.dwReserved1[9] == MAKEFOURCC('N', 'V', 'T', 'T'))
	{
		// Clear out non-standard nVidia DDS flags
		ddpfFlags &= ~(DDS_SRGB | DDS_NORMAL);
	}
	const size_t MAP_SIZE = sizeof(g_LegacyDDSMap) / sizeof(LegacyDDS);
	size_t index = 0;
	for(index = 0; index < MAP_SIZE; ++index)
	{
		const LegacyDDS* entry = &g_LegacyDDSMap[index];
		//if(ddpfFlags == entry->ddpf.dwFlags)
		//{
			if(entry->ddpf.dwFlags & DDS_FOURCC && ddpfFlags & DDS_FOURCC)
			{
				if(ddpf.dwFourCC == entry->ddpf.dwFourCC)
					break;
			}
			else
			if(entry->ddpf.dwFlags & DDS_PAL8 && ddpfFlags & DDS_PAL8)
			{
				if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount)
					{break;}
			}
			else
			if(entry->ddpf.dwFlags & DDS_ALPHA && ddpfFlags & DDS_ALPHA)
			{
				if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount
					&& ddpf.dwRGBAlphaBitMask == entry->ddpf.dwRGBAlphaBitMask)
					{break;}
			}
			else
			if(entry->ddpf.dwFlags & DDS_LUMINANCE && ddpfFlags & DDS_LUMINANCE)
			{
				if(entry->ddpf.dwFlags & DDS_ALPHAPIXELS && ddpfFlags & DDS_ALPHAPIXELS)
				{
					// LUMINANCEA
					if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount
					&& ddpf.dwRBitMask == entry->ddpf.dwRBitMask
					&& ddpf.dwRGBAlphaBitMask == entry->ddpf.dwRGBAlphaBitMask)
						{break;}
				}
				else
				{
					// LUMINANCE
					if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount
						&& ddpf.dwRBitMask == entry->ddpf.dwRBitMask)
						{break;}
				}
			}
			else
			if(entry->ddpf.dwFlags & DDS_BUMPDUDV && ddpfFlags & DDS_BUMPDUDV)
			{
				//MessageBoxA(NULL, "111", "g_LegacyDDSMap", MB_OK);
				if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount
				&& ddpf.dwRBitMask == entry->ddpf.dwRBitMask
				&& ddpf.dwGBitMask == entry->ddpf.dwGBitMask
				&& ddpf.dwBBitMask == entry->ddpf.dwBBitMask
				&& ddpf.dwRGBAlphaBitMask == entry->ddpf.dwRGBAlphaBitMask)
					{break;}
			}
			else
			if(ddpf.dwRGBBitCount == entry->ddpf.dwRGBBitCount)
			{
				if(entry->ddpf.dwFlags & DDS_ALPHAPIXELS && ddpfFlags & DDS_ALPHAPIXELS && !(ddpfFlags & DDS_BUMPDUDV))
				{
					// RGBA
					if(ddpf.dwRBitMask == entry->ddpf.dwRBitMask
					&& ddpf.dwGBitMask == entry->ddpf.dwGBitMask
					&& ddpf.dwBBitMask == entry->ddpf.dwBBitMask
					&& ddpf.dwRGBAlphaBitMask == entry->ddpf.dwRGBAlphaBitMask)
						{break;}
				}
				else
				{
					// RGB
					if(ddpf.dwRBitMask == entry->ddpf.dwRBitMask
					&& ddpf.dwGBitMask == entry->ddpf.dwGBitMask
					&& ddpf.dwBBitMask == entry->ddpf.dwBBitMask
					&& ddpfFlags == entry->ddpf.dwFlags)
					{break;}
				}
			}
		//}
	}
	if(index >= MAP_SIZE)
		{return DXGI_FORMAT_NEW_UNKNOWN;}
	DWORD cflags = g_LegacyDDSMap[index].convFlags;
	DXGI_FORMAT_NEW format = g_LegacyDDSMap[index].format;
	if((cflags & CONV_FLAGS_EXPAND) && (flags & DDS_FLAGS_NO_LEGACY_EXPANSION))
		return DXGI_FORMAT_NEW_UNKNOWN;
	if((format == DXGI_FORMAT_NEW_R10G10B10A2_UNORM) && (flags & DDS_FLAGS_NO_R10B10G10A2_FIXUP))
		{cflags ^= CONV_FLAGS_SWIZZLE;}
	if((hdr.dwReserved1[9] == MAKEFOURCC('N', 'V', 'T', 'T'))
	&& (ddpf.dwFlags & DDS_SRGB))
		{format = MakeSRGB(format);}
	convFlags = cflags;
	return format;
}

DXGI_FORMAT_NEW MakeSRGB(DXGI_FORMAT_NEW fmt)
{
    switch(fmt)
    {
		case DXGI_FORMAT_NEW_R8G8B8A8_UNORM:
			return DXGI_FORMAT_NEW_R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_NEW_BC1_UNORM:
			return DXGI_FORMAT_NEW_BC1_UNORM_SRGB;
		case DXGI_FORMAT_NEW_BC2_UNORM:
			return DXGI_FORMAT_NEW_BC2_UNORM_SRGB;
		case DXGI_FORMAT_NEW_BC3_UNORM:
			return DXGI_FORMAT_NEW_BC3_UNORM_SRGB;
		case DXGI_FORMAT_NEW_B8G8R8A8_UNORM:
			return DXGI_FORMAT_NEW_B8G8R8A8_UNORM_SRGB;
		case DXGI_FORMAT_NEW_B8G8R8X8_UNORM:
			return DXGI_FORMAT_NEW_B8G8R8X8_UNORM_SRGB;
		case DXGI_FORMAT_NEW_BC7_UNORM:
			return DXGI_FORMAT_NEW_BC7_UNORM_SRGB;
		default:
			return fmt;
    }
}


void DetermineImageArray(FullTexMetadata& FMD, DWORD cpFlags, bool dx10)
{
	assert(FMD.MData.width > 0 && FMD.MData.height > 0 && FMD.MData.depth > 0);
	assert(FMD.MData.arraySize > 0);
	assert(FMD.MData.mipLevels > 0);

	DWORD PF_BPP = FMD.DDSH.ddspf.dwRGBBitCount;
	if (PF_BPP == 0 && dx10)
	{
		PF_BPP = 32;
		if (FMD.DDSH.ddspf.dwFlags & DDS_RGB)
			{PF_BPP = 24;}
		if (FMD.DDSH.ddspf.dwFlags & DDS_RGBA)
			{PF_BPP = 32;}
	}
	size_t _pixelSize = 0, _nimages = 0, SliceIndex = 0;
	
	size_t size = 0;
	switch (FMD.MData.dimension)
	{
	case TEX_DIMENSION_TEXTURE1D:
	case TEX_DIMENSION_TEXTURE2D: //DDDSCAPS2_CUBEMAP
		for (size_t item = 0; item < FMD.MData.arraySize; ++item)
		{
			size_t w = FMD.MData.width;
			size_t h = FMD.MData.height;
			for (size_t level = 0; level < FMD.MData.mipLevels; ++level)
			{
				size_t rowPitch, slicePitch;
				ComputePitch(FMD.MData.format, w, h, rowPitch, slicePitch, cpFlags, PF_BPP);
				_pixelSize += slicePitch;
				FMD.SlicesPitchs[(SliceIndex < 1000) ? SliceIndex : 1000] = slicePitch;
				SliceIndex++;
				++_nimages;
				if(h > 1)
					{h >>= 1;}
				if(w > 1)
					{w >>= 1;}
			}
		}
		break;
	case TEX_DIMENSION_TEXTURE3D: //DDS_HEADER_FLAGS_VOLUME
	{
		size_t w = FMD.MData.width;
		size_t h = FMD.MData.height;
		size_t d = FMD.MData.depth;
		for (size_t level = 0; level < FMD.MData.mipLevels; ++level)
		{
			size_t rowPitch, slicePitch;
			ComputePitch(FMD.MData.format, w, h, rowPitch, slicePitch, cpFlags, PF_BPP);
			for (size_t slice = 0; slice < d; ++slice)
			{
				_pixelSize += slicePitch;
				FMD.SlicesPitchs[(SliceIndex < 1000) ? SliceIndex : 1000] = slicePitch;
				SliceIndex++;
				++_nimages;
			}
			if(h > 1)
				{h >>= 1;}
			if(w > 1)
				{w >>= 1;}
			if(d > 1)
				{d >>= 1;}
		}
	}
	break;
	default:
		assert(false); 
		break;
	}
	FMD.IA.pixelSize = _pixelSize;
	FMD.IA.nImages = _nimages;
}


void ComputePitch(DXGI_FORMAT_NEW fmt, size_t width, size_t height,
	size_t& rowPitch, size_t& slicePitch, DWORD flags, DWORD PF_BPP)
{
	switch (static_cast<int>(fmt))
	{
	case DXGI_FORMAT_NEW_BC1_TYPELESS:
	case DXGI_FORMAT_NEW_BC1_UNORM:
	case DXGI_FORMAT_NEW_BC1_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC4_TYPELESS:
	case DXGI_FORMAT_NEW_BC4_UNORM:
	case DXGI_FORMAT_NEW_BC4_SNORM:
		assert(IsCompressed(fmt));
		{
			if (flags & CP_FLAGS_BAD_DXTN_TAILS)
			{
				size_t nbw = width >> 2;
				size_t nbh = height >> 2;
				rowPitch = max(1, nbw * 8);
				slicePitch = max(1, rowPitch * nbh);
			}
			else
			{
				size_t nbw = max(1, (width + 3) / 4);
				size_t nbh = max(1, (height + 3) / 4);
				rowPitch = nbw * 8;
				slicePitch = rowPitch * nbh;
			}
		}
		break;

	case DXGI_FORMAT_NEW_BC2_TYPELESS:
	case DXGI_FORMAT_NEW_BC2_UNORM:
	case DXGI_FORMAT_NEW_BC2_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC3_TYPELESS:
	case DXGI_FORMAT_NEW_BC3_UNORM:
	case DXGI_FORMAT_NEW_BC3_UNORM_SRGB:
	case DXGI_FORMAT_NEW_RXGB:
	case DXGI_FORMAT_NEW_RBXG:
	case DXGI_FORMAT_NEW_RGXB:
	case DXGI_FORMAT_NEW_RXBG:
	case DXGI_FORMAT_NEW_XGBR:
	case DXGI_FORMAT_NEW_XGXR:
	case DXGI_FORMAT_NEW_XRBG:
	case DXGI_FORMAT_NEW_A2D5:
	case DXGI_FORMAT_NEW_BC5_TYPELESS:
	case DXGI_FORMAT_NEW_BC5_UNORM:
	case DXGI_FORMAT_NEW_BC5_SNORM:
	case DXGI_FORMAT_NEW_BC6H_TYPELESS:
	case DXGI_FORMAT_NEW_BC6H_UF16:
	case DXGI_FORMAT_NEW_BC6H_SF16:
	case DXGI_FORMAT_NEW_BC7_TYPELESS:
	case DXGI_FORMAT_NEW_BC7_UNORM:
	case DXGI_FORMAT_NEW_BC7_UNORM_SRGB:
	case DXGI_FORMAT_NEW_ATCA:
	case DXGI_FORMAT_NEW_ATCI:
		assert(IsCompressed(fmt));
		{
			if (flags & CP_FLAGS_BAD_DXTN_TAILS)
			{
				size_t nbw = width >> 2;
				size_t nbh = height >> 2;
				rowPitch = max(1, nbw * 16);
				slicePitch = max(1, rowPitch * nbh);
			}
			else
			{
				size_t nbw = max(1, (width + 3) / 4);
				size_t nbh = max(1, (height + 3) / 4);
				rowPitch = nbw * 16;
				slicePitch = rowPitch * nbh;
			}
		}
		break;

	case DXGI_FORMAT_NEW_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_NEW_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_NEW_YUY2:
	case DXGI_FORMAT_NEW_UYVY:
		assert(IsPacked(fmt));
		rowPitch = ((width + 1) >> 1) * 4;
		slicePitch = rowPitch * height;
		break;

	case DXGI_FORMAT_NEW_Y210:
	case DXGI_FORMAT_NEW_Y216:
		assert(IsPacked(fmt));
		rowPitch = ((width + 1) >> 1) * 8;
		slicePitch = rowPitch * height;
		break;

	case DXGI_FORMAT_NEW_NV12:
	case DXGI_FORMAT_NEW_420_OPAQUE:
		assert(IsPlanar(fmt));
		rowPitch = ((width + 1) >> 1) * 2;
		slicePitch = rowPitch * (height + ((height + 1) >> 1));
		break;

	case DXGI_FORMAT_NEW_P010:
	case DXGI_FORMAT_NEW_P016:
	case XBOX_DXGI_FORMAT_NEW_D16_UNORM_S8_UINT:
	case XBOX_DXGI_FORMAT_NEW_R16_UNORM_X8_TYPELESS:
	case XBOX_DXGI_FORMAT_NEW_X16_TYPELESS_G8_UINT:
		assert(IsPlanar(fmt));
		rowPitch = ((width + 1) >> 1) * 4;
		slicePitch = rowPitch * (height + ((height + 1) >> 1));
		break;

	case DXGI_FORMAT_NEW_NV11:
		assert(IsPlanar(fmt));
		rowPitch = ((width + 3) >> 2) * 4;
		slicePitch = rowPitch * height * 2;
		break;

	case WIN10_DXGI_FORMAT_NEW_P208:
		assert(IsPlanar(fmt));
		rowPitch = ((width + 1) >> 1) * 2;
		slicePitch = rowPitch * height * 2;
		break;

	case WIN10_DXGI_FORMAT_NEW_V208:
		assert(IsPlanar(fmt));
		rowPitch = width;
		slicePitch = rowPitch * (height + (((height + 1) >> 1) * 2));
		break;

	case WIN10_DXGI_FORMAT_NEW_V408:
		assert(IsPlanar(fmt));
		rowPitch = width;
		slicePitch = rowPitch * (height + ((height >> 1) * 4));
		break;

	default:
		assert(IsValid(fmt));
		assert(!IsCompressed(fmt) && !IsPacked(fmt) && !IsPlanar(fmt));
		{
			size_t bpp;
			if (flags & CP_FLAGS_24BPP)
				{bpp = 24;}
			else if (flags & CP_FLAGS_16BPP)
				{bpp = 16;}
			else if (flags & CP_FLAGS_8BPP)
				{bpp = 8;}
			else
			{
				if(fmt == DXGI_FORMAT_NEW_R8G8B8A8_UNORM)
					{bpp = PF_BPP;}
				else
					{bpp = BitsPerPixel(fmt);}
			}
			if (flags & (CP_FLAGS_LEGACY_DWORD | CP_FLAGS_PARAGRAPH | CP_FLAGS_YMM | CP_FLAGS_ZMM | CP_FLAGS_PAGE4K))
			{
				if (flags & CP_FLAGS_PAGE4K)
				{
					rowPitch = ((width * bpp + 32767) / 32768) * 4096;
					slicePitch = rowPitch * height;
				}
				else if (flags & CP_FLAGS_ZMM)
				{
					rowPitch = ((width * bpp + 511) / 512) * 64;
					slicePitch = rowPitch * height;
				}
				else if (flags & CP_FLAGS_YMM)
				{
					rowPitch = ((width * bpp + 255) / 256) * 32;
					slicePitch = rowPitch * height;
				}
				else if (flags & CP_FLAGS_PARAGRAPH)
				{
					rowPitch = ((width * bpp + 127) / 128) * 16;
					slicePitch = rowPitch * height;
				}
				else // DWORD alignment
				{
					// Special computation for some incorrectly created DDS files based on
					// legacy DirectDraw assumptions about pitch alignment
					rowPitch = ((width * bpp + 31) / 32) * sizeof(UINT);
					slicePitch = rowPitch * height;
				}
			}
			else
			{
				// Default byte alignment
				rowPitch = (width * bpp + 7) / 8;
				slicePitch = rowPitch * height;
			}
		}
		break;
	}
}

bool IsCompressed(DXGI_FORMAT_NEW fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_NEW_BC1_TYPELESS:
	case DXGI_FORMAT_NEW_BC1_UNORM:
	case DXGI_FORMAT_NEW_BC1_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC2_TYPELESS:
	case DXGI_FORMAT_NEW_BC2_UNORM:
	case DXGI_FORMAT_NEW_BC2_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC3_TYPELESS:
	case DXGI_FORMAT_NEW_BC3_UNORM:
	case DXGI_FORMAT_NEW_BC3_UNORM_SRGB:
	case DXGI_FORMAT_NEW_RXGB:
	case DXGI_FORMAT_NEW_RBXG:
	case DXGI_FORMAT_NEW_RGXB:
	case DXGI_FORMAT_NEW_RXBG:
	case DXGI_FORMAT_NEW_XGBR:
	case DXGI_FORMAT_NEW_XGXR:
	case DXGI_FORMAT_NEW_XRBG:
	case DXGI_FORMAT_NEW_A2D5:
	case DXGI_FORMAT_NEW_BC4_TYPELESS:
	case DXGI_FORMAT_NEW_BC4_UNORM:
	case DXGI_FORMAT_NEW_BC4_SNORM:
	case DXGI_FORMAT_NEW_BC5_TYPELESS:
	case DXGI_FORMAT_NEW_BC5_UNORM:
	case DXGI_FORMAT_NEW_BC5_SNORM:
	case DXGI_FORMAT_NEW_BC6H_TYPELESS:
	case DXGI_FORMAT_NEW_BC6H_UF16:
	case DXGI_FORMAT_NEW_BC6H_SF16:
	case DXGI_FORMAT_NEW_BC7_TYPELESS:
	case DXGI_FORMAT_NEW_BC7_UNORM:
	case DXGI_FORMAT_NEW_BC7_UNORM_SRGB:
	case DXGI_FORMAT_NEW_ATCA:
	case DXGI_FORMAT_NEW_ATCI:
		return true;

	default:
		return false;
	}
}


bool IsPalettized(DXGI_FORMAT_NEW fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_NEW_AI44:
	case DXGI_FORMAT_NEW_IA44:
	case DXGI_FORMAT_NEW_P8:
	case DXGI_FORMAT_NEW_A8P8:
	case DXGI_FORMAT_NEW_P4:
		return true;
	default:
		return false;
	}
}

bool IsPacked(DXGI_FORMAT_NEW fmt)
{
	switch (static_cast<int>(fmt))
	{
	case DXGI_FORMAT_NEW_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_NEW_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_NEW_YUY2: // 4:2:2 8-bit
	case DXGI_FORMAT_NEW_UYVY: // 4:2:2 16-bit
	case DXGI_FORMAT_NEW_Y210: // 4:2:2 10-bit
	case DXGI_FORMAT_NEW_Y216: // 4:2:2 16-bit
		return true;

	default:
		return false;
	}
}

bool IsPlanar(DXGI_FORMAT_NEW fmt)
{
	switch (static_cast<int>(fmt))
	{
	case DXGI_FORMAT_NEW_NV12:      // 4:2:0 8-bit
	case DXGI_FORMAT_NEW_P010:      // 4:2:0 10-bit
	case DXGI_FORMAT_NEW_P016:      // 4:2:0 16-bit
	case DXGI_FORMAT_NEW_420_OPAQUE:// 4:2:0 8-bit
	case DXGI_FORMAT_NEW_NV11:      // 4:1:1 8-bit

	case WIN10_DXGI_FORMAT_NEW_P208: // 4:2:2 8-bit
	case WIN10_DXGI_FORMAT_NEW_V208: // 4:4:0 8-bit
	case WIN10_DXGI_FORMAT_NEW_V408: // 4:4:4 8-bit
								 // These are JPEG Hardware decode formats (DXGI 1.4)

	case XBOX_DXGI_FORMAT_NEW_D16_UNORM_S8_UINT:
	case XBOX_DXGI_FORMAT_NEW_R16_UNORM_X8_TYPELESS:
	case XBOX_DXGI_FORMAT_NEW_X16_TYPELESS_G8_UINT:
		// These are Xbox One platform specific types
		return true;

	default:
		return false;
	}
}

bool IsValid(DXGI_FORMAT_NEW fmt)
{
	return (static_cast<size_t>(fmt) >= 1 && static_cast<size_t>(fmt) <= 190);
}

size_t BitsPerPixel(DXGI_FORMAT_NEW fmt)
{
	switch (static_cast<int>(fmt))
	{
	case DXGI_FORMAT_NEW_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_NEW_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_NEW_R32G32B32A32_UINT:
	case DXGI_FORMAT_NEW_R32G32B32A32_SINT:
		return 128;
		break;
	case DXGI_FORMAT_NEW_R32G32B32_TYPELESS:
	case DXGI_FORMAT_NEW_R32G32B32_FLOAT:
	case DXGI_FORMAT_NEW_R32G32B32_UINT:
	case DXGI_FORMAT_NEW_R32G32B32_SINT:
		return 96;
		break;
	case DXGI_FORMAT_NEW_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_NEW_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_NEW_R16G16B16A16_UNORM:
	case DXGI_FORMAT_NEW_R16G16B16A16_UINT:
	case DXGI_FORMAT_NEW_R16G16B16A16_SNORM:
	case DXGI_FORMAT_NEW_R16G16B16A16_SINT:
	case DXGI_FORMAT_NEW_R32G32_TYPELESS:
	case DXGI_FORMAT_NEW_R32G32_FLOAT:
	case DXGI_FORMAT_NEW_R32G32_UINT:
	case DXGI_FORMAT_NEW_R32G32_SINT:
	case DXGI_FORMAT_NEW_R32G8X24_TYPELESS:
	case DXGI_FORMAT_NEW_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_NEW_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_NEW_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_NEW_Y416:
	case DXGI_FORMAT_NEW_Y210:
	case DXGI_FORMAT_NEW_Y216:
		return 64;
		break;
	case DXGI_FORMAT_NEW_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_NEW_R10G10B10A2_UNORM:
	case DXGI_FORMAT_NEW_R10G10B10A2_UINT:
	case DXGI_FORMAT_NEW_R11G11B10_FLOAT:
	case DXGI_FORMAT_NEW_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_NEW_R8G8B8A8_UNORM:
	case DXGI_FORMAT_NEW_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_R8G8B8A8_UINT:
	case DXGI_FORMAT_NEW_R8G8B8A8_SNORM:
	case DXGI_FORMAT_NEW_R8G8B8A8_SINT:
	case DXGI_FORMAT_NEW_R16G16_TYPELESS:
	case DXGI_FORMAT_NEW_R16G16_FLOAT:
	case DXGI_FORMAT_NEW_R16G16_UNORM:
	case DXGI_FORMAT_NEW_R16G16_UINT:
	case DXGI_FORMAT_NEW_R16G16_SNORM:
	case DXGI_FORMAT_NEW_R16G16_SINT:
	case DXGI_FORMAT_NEW_R32_TYPELESS:
	case DXGI_FORMAT_NEW_D32_FLOAT:
	case DXGI_FORMAT_NEW_R32_FLOAT:
	case DXGI_FORMAT_NEW_R32_UINT:
	case DXGI_FORMAT_NEW_R32_SINT:
	case DXGI_FORMAT_NEW_R24G8_TYPELESS:
	case DXGI_FORMAT_NEW_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_NEW_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_NEW_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_NEW_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_NEW_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_NEW_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_NEW_B8G8R8A8_UNORM:
	case DXGI_FORMAT_NEW_B8G8R8X8_UNORM:
	case DXGI_FORMAT_NEW_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_NEW_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_NEW_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_NEW_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_AYUV:
	case DXGI_FORMAT_NEW_Y410:
	case DXGI_FORMAT_NEW_YUY2:
	case XBOX_DXGI_FORMAT_NEW_R10G10B10_7E3_A2_FLOAT:
	case XBOX_DXGI_FORMAT_NEW_R10G10B10_6E4_A2_FLOAT:
	case XBOX_DXGI_FORMAT_NEW_R10G10B10_SNORM_A2_UNORM:
	case DXGI_FORMAT_NEW_A2W10V10U10:
	case DXGI_FORMAT_NEW_X8L8V8U8:
	case DXGI_FORMAT_NEW_Q8W8V8U8:
	case DXGI_FORMAT_NEW_D32F_LOCKABLE:
		return 32;
		break;
	case DXGI_FORMAT_NEW_P010:
	case DXGI_FORMAT_NEW_P016:
	case XBOX_DXGI_FORMAT_NEW_D16_UNORM_S8_UINT:
	case XBOX_DXGI_FORMAT_NEW_R16_UNORM_X8_TYPELESS:
	case XBOX_DXGI_FORMAT_NEW_X16_TYPELESS_G8_UINT:
	case WIN10_DXGI_FORMAT_NEW_V408:
		return 24;
		break;
	case DXGI_FORMAT_NEW_R8G8_TYPELESS:
	case DXGI_FORMAT_NEW_R8G8_UNORM:
	case DXGI_FORMAT_NEW_R8G8_UINT:
	case DXGI_FORMAT_NEW_R8G8_SNORM:
	case DXGI_FORMAT_NEW_R8G8_SINT:
	case DXGI_FORMAT_NEW_R16_TYPELESS:
	case DXGI_FORMAT_NEW_R16_FLOAT:
	case DXGI_FORMAT_NEW_D16_UNORM:
	case DXGI_FORMAT_NEW_R16_UNORM:
	case DXGI_FORMAT_NEW_R16_UINT:
	case DXGI_FORMAT_NEW_R16_SNORM:
	case DXGI_FORMAT_NEW_R16_SINT:
	case DXGI_FORMAT_NEW_B5G6R5_UNORM:
	case DXGI_FORMAT_NEW_B5G5R5A1_UNORM:
	case DXGI_FORMAT_NEW_A8P8:
	case DXGI_FORMAT_NEW_B4G4R4A4_UNORM:
	case DXGI_FORMAT_NEW_R4G4B4A4_UNORM:
	case WIN10_DXGI_FORMAT_NEW_P208:
	case WIN10_DXGI_FORMAT_NEW_V208:
	case DXGI_FORMAT_NEW_L6V5U5:
	case DXGI_FORMAT_NEW_UYVY:
	case DXGI_FORMAT_NEW_A8L8:
	case DXGI_FORMAT_NEW_CxV8U8:
	case DXGI_FORMAT_NEW_D16_LOCKABLE:
		return 16;
		break;
	case DXGI_FORMAT_NEW_NV12:
	case DXGI_FORMAT_NEW_420_OPAQUE:
	case DXGI_FORMAT_NEW_NV11:
		return 12;
		break;
	case DXGI_FORMAT_NEW_BC2_TYPELESS:
	case DXGI_FORMAT_NEW_BC2_UNORM:
	case DXGI_FORMAT_NEW_BC2_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC3_TYPELESS:
	case DXGI_FORMAT_NEW_BC3_UNORM:
	case DXGI_FORMAT_NEW_BC3_UNORM_SRGB:
	case DXGI_FORMAT_NEW_RXGB:
	case DXGI_FORMAT_NEW_RBXG:
	case DXGI_FORMAT_NEW_RGXB:
	case DXGI_FORMAT_NEW_RXBG:
	case DXGI_FORMAT_NEW_XGBR:
	case DXGI_FORMAT_NEW_XGXR:
	case DXGI_FORMAT_NEW_XRBG:
	case DXGI_FORMAT_NEW_A2D5:
	case DXGI_FORMAT_NEW_BC5_TYPELESS:
	case DXGI_FORMAT_NEW_BC5_UNORM:
	case DXGI_FORMAT_NEW_BC5_SNORM:
	case DXGI_FORMAT_NEW_BC6H_TYPELESS:
	case DXGI_FORMAT_NEW_BC6H_UF16:
	case DXGI_FORMAT_NEW_BC6H_SF16:
	case DXGI_FORMAT_NEW_BC7_TYPELESS:
	case DXGI_FORMAT_NEW_BC7_UNORM:
	case DXGI_FORMAT_NEW_BC7_UNORM_SRGB:
	case DXGI_FORMAT_NEW_R8_TYPELESS:
	case DXGI_FORMAT_NEW_R8_UNORM:
	case DXGI_FORMAT_NEW_R8_UINT:
	case DXGI_FORMAT_NEW_R8_SNORM:
	case DXGI_FORMAT_NEW_R8_SINT:
	case DXGI_FORMAT_NEW_A8_UNORM:
	case DXGI_FORMAT_NEW_AI44:
	case DXGI_FORMAT_NEW_IA44:
	case DXGI_FORMAT_NEW_P8:
	case XBOX_DXGI_FORMAT_NEW_R4G4_UNORM:
	case DXGI_FORMAT_NEW_A4L4:
	case DXGI_FORMAT_NEW_R3G3B2:
	case DXGI_FORMAT_NEW_ATCA:
	case DXGI_FORMAT_NEW_ATCI:
		return 8;
		break;
	case DXGI_FORMAT_NEW_BC1_TYPELESS:
	case DXGI_FORMAT_NEW_BC1_UNORM:
	case DXGI_FORMAT_NEW_BC1_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC4_TYPELESS:
	case DXGI_FORMAT_NEW_BC4_UNORM:
	case DXGI_FORMAT_NEW_BC4_SNORM:
	case DXGI_FORMAT_NEW_P4:
		return 4;
		break;
	case DXGI_FORMAT_NEW_R1_UNORM:
		return 1;
		break;
	default:
		return 0;
	};
}

bool IsSRGB(DXGI_FORMAT_NEW fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_NEW_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC1_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC2_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC3_UNORM_SRGB:
	case DXGI_FORMAT_NEW_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_NEW_BC7_UNORM_SRGB:
		return true;
	default:
		return false;
	}
}

#endif