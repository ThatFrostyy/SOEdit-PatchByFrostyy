// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor

#include "stdafx.h"
#include "FreeImage.h"
#pragma comment(lib, "FreeImage.lib")
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "dds_meta.h"
#include "Dxt_unpack.hpp"
#include "TGAImage.hpp"

#include "gl/gl.h"
#include "gl/glu.h"

#include "general.h"

#include "CTexFile.h"

enum {t_bad, t_bmp, t_tga, t_tif, t_dds, t_jpg, t_png, t_gif, t_checker} t_textype;

void readUnsignedImage(BYTE *in_data, DWORD *out_data, const DDS_HEADER &dds, bool hasAlpha, bool BGR);
inline int maskToShift(UINT mask);
inline int maskLength(UINT mask);
inline DWORD PF_BitCounter(DDS_PIXELFORMAT &ddspf);
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);
FIBITMAP* GenericLoader(const char* lpszPathName, int flag);

CTexFile::CTexFile(char *texname)
{
	m_TexName = NULL;
	m_Bits = NULL;
	m_transparency = false;
	if(texname)
	{
		m_TexName = new char[strlen(texname) + 1];
		strcpy(m_TexName, texname);
	}
	else
	{
		m_TexName = new char[1];
		m_TexName[0] = 0;
	}
	LoadBits();
}

CTexFile::~CTexFile()
{
	if(m_TexName)
	{
		delete[] m_TexName;
		m_TexName = NULL;
	}
	if(m_Bits)
	{
		delete[] m_Bits;
		m_Bits = NULL;
	}
}

unsigned char *CTexFile::GetBits()
	{return m_Bits;}

int CTexFile::GetBPP()
	{return m_BPP;}

int CTexFile::GetWidth()
	{return m_Width;}

int CTexFile::GetHeight()
	{return m_Height;}

int CTexFile::DetermineType()
{
    int textype = t_bad;
    char *ext;
    if(ext = strrchr(m_TexName, '.'))
	{
		ext++;
		if(!stricmp(ext, "PNG"))
		{
			//Windows DI PNG File
			textype = t_png;
		}
		else
        if(!stricmp(ext, "JPG"))
		{
			//Windows DI JPG File
			textype = t_jpg;
		}
		else
        if(!stricmp(ext, "BMP"))
		{
			//Windows DI Bitmap File
			textype = t_bmp;
		}
		else
        if(!stricmp(ext, "GIF"))
		{
			//Windows GIF File
			textype = t_gif;
		}
		else
        if(!stricmp(ext, "TGA"))
		{
			// Targa TGA File
			textype = t_tga;
		}
		else
        if(!stricmp(ext, "TIF") || !stricmp(ext, "TIFF"))
		{
			// Tagged Information Format File
			textype = t_tif;
		}
		else
        if(!stricmp(ext, "DDS"))
		{
			// DirectDraw Surface
			textype = t_dds;
		}
	}
	else
		{textype = t_checker;}
	return textype;
}

bool CTexFile::LoadBits()
{
    int textype;
	FILE *fp;
    if(t_bad == (textype = DetermineType()))
		{textype = t_checker;}
	if((fp = fopen(m_TexName, "rb")) == NULL)
		{textype = t_checker;}
	else
		{fclose(fp);}
    switch(textype)
	{
		case t_bmp:
		case t_jpg:
		case t_png:
		case t_gif:
		case t_tif:
		{
			#ifdef FREEIMAGE_LIB
			FreeImage_Initialise();
			#endif //FREEIMAGE_LIB
			FreeImage_SetOutputMessage(FreeImageErrorHandler);
			FIBITMAP *dib = GenericLoader(m_TexName, 0);
			if(dib)
			{
				FIBITMAP* src32 = FreeImage_ConvertTo32Bits(dib);
				int size_buff, scan_width;
				FreeImage_Unload(dib);
				m_Width = FreeImage_GetWidth(src32);
				m_Height = FreeImage_GetHeight(src32);
				m_BPP = FreeImage_GetBPP(src32);
				scan_width = m_Width * m_BPP / 8;
				size_buff = (m_Width * m_Height) * m_BPP;
				m_Bits = new unsigned char[size_buff];
				memset(m_Bits, 0, size_buff);
				FreeImage_ConvertToRawBits(m_Bits, src32, scan_width, m_BPP, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
				SwapRB();
				FreeImage_Unload(src32);//free the loaded FIBITMAP
			}
			#ifdef FREEIMAGE_LIB
			FreeImage_DeInitialise();
			#endif // FREEIMAGE_LIB
			m_transparency = Alpha_Analysis((DWORD*)m_Bits, m_Width, m_Height, false);
			if(dib)
				{return true;}
		}
		break;
        case t_tga:
		{
			if(LoadTGA())
			{
				m_transparency = Alpha_Analysis((DWORD*)m_Bits, m_Width, m_Height, false);
				return true;
			}
		}
		break;
        case t_dds:
		{
			if(LoadDDS())
			{
				m_transparency = Alpha_Analysis((DWORD*)m_Bits, m_Width, m_Height, false);
				return true;
			}
		}
		break;
        case t_checker:
		{
			int CW = 0, CH = 0, size_buff = (64 * 64) * 4;
			bool col = true;
			unsigned char *Bptr;
			m_Bits = new unsigned char[size_buff];//Новое 32-битное изображение
			Bptr = m_Bits;
			m_Width = 64;
			m_Height = 64;
			m_BPP = 32;
			for(int cx = 0; cx < 64; cx++)
			{
				for(int cy = 0; cy < 64; cy++)
				{
					*Bptr = (col) ? 0xff : 0x00; Bptr++;
					*Bptr = (col) ? 0xff : 0x00; Bptr++;
					*Bptr = (col) ? 0xff : 0x00; Bptr++;
					*Bptr = 0xff; Bptr++;
					if(cy % 4 == 0)
						{col = !col;}
				}
				if(cx % 4 == 0)
					{col = !col;}
			}
		}
		break;
	}
	return false;
}

bool CTexFile::LoadTGA()
{
	TGAImage *TGA_Img = NULL;
	DWORD size_buff = 0;
	int OByte = 0, IByte = 0, W = 0, H = 0;
	TGA_Img = new TGAImage();
	int result = TGA_Img -> Read_TGA_File(m_TexName);
	if(result == TGA_ERR_OK)
	{
		m_Width = TGA_Img -> get_width();
		m_Height = TGA_Img -> get_height();
		m_BPP = 32;
		size_buff = (m_Width * m_Height) * 4;
		BYTE *TGABuffer = new BYTE[size_buff];
		m_Bits = new unsigned char[size_buff];
		memset(m_Bits, 0, size_buff);
		memset(TGABuffer, 0, size_buff);
		W = m_Width; H = m_Height;
		if(TGA_Img -> get_bytespp() == TGAImage::RGBA)
			{memcpy(TGABuffer, TGA_Img -> buffer(), size_buff);}
		else
		if(TGA_Img -> get_bytespp() != TGAImage::APROX)
		{
			for(int i = 0; i < W * H; i++)
			{
				for(int i2 = 0; i2 < 4; i2++)
					{TGABuffer[OByte++] = (i2 < TGA_Img -> get_bytespp()) ? TGA_Img -> buffer()[IByte++] : ((TGA_Img -> get_bytespp() == 1) ? ((i2 < 3) ? TGA_Img -> buffer()[IByte-1] : 255) : 255);}
			}
		}
		else
		{
			for(int i = 0; i < W * H; i++)
			{
				TGABuffer[OByte++] = (TGA_Img -> buffer()[IByte] & 0x1F) << 3;
				TGABuffer[OByte++] = ((((TGA_Img -> buffer()[IByte] & 0xE0) >> 5) | ((TGA_Img -> buffer()[IByte + 1] & 0x03) << 3))) << 3;
				TGABuffer[OByte++] = ((TGA_Img -> buffer()[IByte + 1] & 0x7C) >> 2) << 3;
				TGABuffer[OByte++] = 255;
				IByte += 2;
			}
		}
		memcpy(m_Bits, TGABuffer, size_buff);
		delete[] TGABuffer; TGABuffer = NULL;
		SwapRB();
	}
	if(TGA_Img)
		{delete TGA_Img; TGA_Img = NULL;}
	return (result == TGA_ERR_OK);
}

bool CTexFile::SwapRB()
{
    byte     *newbits;    // New RGB bits
	BYTE a = 0, r = 0, g = 0, b = 0;
	int X = 0, Y = 0, counter = 0;
	DWORD bitsize = (m_Width * m_Height) * ((m_BPP == 24) ? 3 : 4);
    // Allocate memory for the RGB bitmap...
    if(!(newbits = new unsigned char[bitsize]))
		{return false;}
    // Null out the new memory area
    memset(newbits, 0, bitsize);
    for(X = 0; X < m_Width; X++)
	{
		for(Y = 0; Y < m_Height; Y++)
		{
			r = m_Bits[counter];
			newbits[counter + 2] = r;
			counter++;
			g = m_Bits[counter];
			newbits[counter] = g;
			counter++;
			b = m_Bits[counter];
			newbits[counter - 2] = b;
			counter++;
			if(m_BPP == 32)
			{
				a = m_Bits[counter];
				newbits[counter] = a;
				counter++;
			}
		}
	}
    delete[] m_Bits;
    m_Bits = newbits;
    return true;
}

bool CTexFile::LoadDDS()//эта функция ещё со старой версии DDS_Optimizer_(v0.8m6)
{
	HANDLE HFile = NULL;
	char buffer[1024] = {0};
	FullTexMetadata FMD;
	DWORD Hedaer_format = 0, FileStartOffset = 0, BytesIO = 0, size_buff = 0, result = 0;
	HFile = CreateFileA(m_TexName, GENERIC_READ/* | GENERIC_WRITE*/, NULL, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0);
	if(HFile == INVALID_HANDLE_VALUE/* || HFile == NULL*/)
	{
		HFile = NULL;
		#ifdef ALTERNATIVE_LANG
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Unable to open DDS-file:\n" + CString(m_TexName)), "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
		#else
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("Невозможно открыть DDS-файл:\n" + CString(m_TexName)), "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
		#endif
		/*printf("ОШИБКА: НЕВОЗМОЖНО ОТКРЫТЬ:\n%s\n\n", m_TexName);*/
		return false;
	}
	else
	{
		ZeroMemory(&FMD, sizeof(FullTexMetadata));
		FMD.SlicesPitchs = (DWORD*)malloc(1000 * sizeof(DWORD*));//TODO: юзать Vector
		memset(FMD.SlicesPitchs, 0, 1000 * sizeof(DWORD*));
		DWORD get_dds_meta = GetMetadataFromDDSFile(HFile, 0, FMD);
		if(get_dds_meta == ERR_MD_ALL_OK_NO_EXCEPTION)
		{
			Hedaer_format = FMD.MData.format;
			FileStartOffset = sizeof(DDS_HEADER) + ((FMD.Dx10) ? sizeof(DDS_HEADER_DXT10) : 0) + sizeof(UINT);
			SetFilePointer(HFile, FileStartOffset, NULL, FILE_BEGIN);
			if((Hedaer_format == DXGI_FORMAT_R8G8B8A8_UNORM || Hedaer_format == DXGI_FORMAT_B8G8R8A8_UNORM || Hedaer_format == DXGI_FORMAT_B5G6R5_UNORM || Hedaer_format == DXGI_FORMAT_R10G10B10A2_UNORM || Hedaer_format == DXGI_FORMAT_BC5_UNORM\
				|| Hedaer_format == DXGI_FORMAT_B4G4R4A4_UNORM || Hedaer_format == DXGI_FORMAT_B8G8R8X8_UNORM || Hedaer_format == DXGI_FORMAT_B5G5R5A1_UNORM || Hedaer_format == DXGI_FORMAT_BC2_UNORM || Hedaer_format == DXGI_FORMAT_BC3_UNORM \
				|| Hedaer_format == DXGI_FORMAT_BC1_UNORM) && !FMD.Dx10 && !FMD.Cube/* && !(FMD.DDSH.dwFlags & DDS_HEADER_FLAGS_VOLUME)*/)
			{//Поддерживает форматы 24 28 71 74 77 85 86 87 88 115 !Не читает Dx10, Cube, Volume.
				size_buff = (FMD.MData.width * FMD.MData.height) * 4;
				BYTE *FileBuff = NULL;
				DWORD *ImageBuffer = NULL;
				DWORD useful_size = FMD.SlicesPitchs[0];
				FileBuff = (BYTE*)malloc(useful_size);
				ImageBuffer = (DWORD*)malloc(size_buff);
				memset(FileBuff, 0, useful_size);
				memset(ImageBuffer, 0, size_buff);
				DWORD AlphaBit = FMD.DDSH.ddspf.dwRGBAlphaBitMask;
				DWORD SumBit = 0;
				bool has_alpha = true;
				ReadFile(HFile, FileBuff, useful_size, &BytesIO, NULL);
				if(BytesIO != useful_size)
				{
					//printf("[ОШИБКА: ФАЙЛ ПОВРЕЖДЁН И/ИЛИ НЕ МОЖЕТ БЫТЬ ПРОЧИТАН!] -> \n");
					//printf("[Прочитанно %d из %d необходимых байт!] -> \n", BytesIO, useful_size);
					CString SBytesIO, Suseful_size;
					SBytesIO.Format("%d", BytesIO);
					Suseful_size.Format("%d", useful_size);
					#ifdef ALTERNATIVE_LANG
						MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("The DDS-file is corrupted and/or cannot be read:\n" + CString(m_TexName) + "\n\n[Readed " + SBytesIO + " from " + Suseful_size + " required bytes!]"), "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
					#else
						MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, CString("DDS-файл повреждён и/или не может быть прочитан:\n" + CString(m_TexName) + "\n\n[Прочитанно " + SBytesIO + " из " + Suseful_size + " необходимых байт!]"), "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
					#endif
				}
				if(Hedaer_format == DXGI_FORMAT_BC1_UNORM || Hedaer_format == DXGI_FORMAT_BC2_UNORM || Hedaer_format == DXGI_FORMAT_BC3_UNORM || Hedaer_format == DXGI_FORMAT_BC5_UNORM)
				{
					DXT_unpack DU;
					DU.dxt_decompress((BYTE *)ImageBuffer, FileBuff, Hedaer_format, FMD.DDSH.dwWidth, FMD.DDSH.dwHeight, FMD.DDSH.ddspf.dwFlags & DDS_NORMAL);
				}
				else
				{
					readUnsignedImage(FileBuff, ImageBuffer, FMD.DDSH, ((AlphaBit) ? true : false), false);
					if(AlphaBit == 0 || SumBit > FMD.DDSH.ddspf.dwRGBBitCount)
						{has_alpha = false;}
					else
						{has_alpha = Alpha_Analysis(ImageBuffer, FMD.DDSH.dwWidth, FMD.DDSH.dwHeight, true);}
					if(!has_alpha)
					{
						for(int NP = 0; NP < (int)(FMD.DDSH.dwWidth * FMD.DDSH.dwHeight); NP++)
							{ImageBuffer[NP] |= 0xff << 24;}
					}
				}
				m_Bits = new unsigned char[size_buff];
				memcpy(m_Bits, ImageBuffer, size_buff);
				m_Width = FMD.DDSH.dwWidth;
				m_Height = FMD.DDSH.dwHeight;
				m_BPP = 32;
				if(HFile)
					{CloseHandle(HFile); HFile = NULL;}
				if(ImageBuffer)
					{free(ImageBuffer); ImageBuffer = NULL;}
				if(FileBuff)
					{free(FileBuff); FileBuff = NULL;}
			}
			else
			{
				#ifdef ALTERNATIVE_LANG
					sprintf(buffer, "Texture:\n%s\n", m_TexName);
					if(FMD.Dx10)
						{strcat(buffer, "Has a title Dx10;\n");}
					if(FMD.Cube)
						{strcat(buffer, "Is cubic;\n");}
					/*if(FMD.DDSH.dwFlags & DDS_HEADER_FLAGS_VOLUME)
						{strcat(buffer, "Является объёмом;\n");}*/
					if(!FMD.Dx10 && !FMD.Cube && !(FMD.DDSH.dwFlags & DDS_HEADER_FLAGS_VOLUME))
						{sprintf(buffer, "%sHas the format %d;\n", buffer, Hedaer_format);}
					strcat(buffer, "This condition(s) prevents the texture from loading.\n");
				#else
					sprintf(buffer, "Текстура:\n%s\n", m_TexName);
					if(FMD.Dx10)
						{strcat(buffer, "Имеет заголовок Dx10;\n");}
					if(FMD.Cube)
						{strcat(buffer, "Является кубической;\n");}
					/*if(FMD.DDSH.dwFlags & DDS_HEADER_FLAGS_VOLUME)
						{strcat(buffer, "Является объёмом;\n");}*/
					if(!FMD.Dx10 && !FMD.Cube && !(FMD.DDSH.dwFlags & DDS_HEADER_FLAGS_VOLUME))
						{sprintf(buffer, "%sИмеет формат %d;\n", buffer, Hedaer_format);}
					strcat(buffer, "Это(и) условие(я) мешает(ют) загрузиться текстуре.\n");
				#endif
				MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, buffer, "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
				if(FMD.SlicesPitchs)
					{free(FMD.SlicesPitchs); FMD.SlicesPitchs = NULL;}
				if(HFile)
					{CloseHandle(HFile); HFile = NULL;}
				return false;
			}
		}
		else
		{
			#ifdef ALTERNATIVE_LANG
				sprintf(buffer, "The pre-loading analysis module reports a texture malfunction:\n%s\nCritical error found in texture №%d\n", m_TexName, get_dds_meta);
				switch(get_dds_meta)
				{
					case ERR_MD_RAED_INFO:
						{strcat(buffer, "Unable to read file information");}
					break;
					case ERR_MD_FILE_TOO_SMALL:
						{strcat(buffer, "The file is too small");}
					break;
					case ERR_MD_FILE_TOO_BIG:
						{strcat(buffer, "The file is too large");}
					break;
					case ERR_MD_READ_DATA:
						{strcat(buffer, "Unable to read file data");}
					break;
					case ERR_MD_INVALID_DATA:
						{strcat(buffer, "Invalid file data (header is corrupted)");}
					break;
					case ERR_MD_SIGNATURE:
						{strcat(buffer, "The base signature is incorrect");}
					break;
					case ERR_MD_HEADER_SIZE:
						{strcat(buffer, "Invalid header size");}
					break;
					case ERR_MD_NOT_SUPPORTED:
						{strcat(buffer, "The format is not registered");}
					break;
					default:
						{strcat(buffer, "Unidentified error");}
					break;
				};
			#else
				sprintf(buffer, "Модуль предзагрузочного анализа сообщает о неисправности текстуры:\n%s\nВ текстуре найдена критическая ошибка №%d\n", m_TexName, get_dds_meta);
				switch(get_dds_meta)
				{
					case ERR_MD_RAED_INFO:
						{strcat(buffer, "Невозможно прочитать информацию о файле");}
					break;
					case ERR_MD_FILE_TOO_SMALL:
						{strcat(buffer, "Файл слишком маленький");}
					break;
					case ERR_MD_FILE_TOO_BIG:
						{strcat(buffer, "Файл слишком большой");}
					break;
					case ERR_MD_READ_DATA:
						{strcat(buffer, "Невозможно прочитать данные файла");}
					break;
					case ERR_MD_INVALID_DATA:
						{strcat(buffer, "Неверные данные файла (повреждён заголовок)");}
					break;
					case ERR_MD_SIGNATURE:
						{strcat(buffer, "Базовая сигнатура неверна");}
					break;
					case ERR_MD_HEADER_SIZE:
						{strcat(buffer, "Недопустимый размер заголовка");}
					break;
					case ERR_MD_NOT_SUPPORTED:
						{strcat(buffer, "Формат не зарегистрирован");}
					break;
					default:
						{strcat(buffer, "Неопознанная ошибка");}
					break;
				};
			#endif
			MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, buffer, "ERROR: CTexFile::LoadDDS", MB_ICONHAND);
		}
		if(FMD.SlicesPitchs)
			{free(FMD.SlicesPitchs); FMD.SlicesPitchs = NULL;}
		if(HFile)
			{CloseHandle(HFile); HFile = NULL;}
	}
	return true;
}

void readUnsignedImage(BYTE *in_data, DWORD *out_data, const DDS_HEADER &dds, bool hasAlpha, bool BGR)
{
	const UINT ColorCount = 4;
	register UINT value = 0, ByteCount = 0, colors[ColorCount] = { 0 }, PixelCount = 0;
	UINT flags = dds.ddspf.dwFlags;
	UINT masks[ColorCount];
	BYTE shifts[ColorCount];
	BYTE bits[ColorCount];
	masks[0] = dds.ddspf.dwRBitMask;
	masks[1] = dds.ddspf.dwGBitMask;
	masks[2] = dds.ddspf.dwBBitMask;
	masks[3] = hasAlpha ? dds.ddspf.dwRGBAlphaBitMask : 0;
	for(int i = 0; i < ColorCount; ++i)
	{
		shifts[i] = maskToShift(masks[i]);
		bits[i] = maskLength(masks[i]);
		// move mask to the left
		if(bits[i] <= 8)
			{masks[i] = (masks[i] >> shifts[i]) << (8 - bits[i]);}
	}
	for(UINT y = 0; y < dds.dwHeight; y++)
	{
		for(UINT x = 0; x < dds.dwWidth; x++, PixelCount++)
		{
			value = 0;
			for(BYTE i = 0; i < dds.ddspf.dwRGBBitCount / 8; i++, ByteCount++)
				{value |= BYTEBUILD(in_data[ByteCount], i);}
			for(int c = 0; c < ColorCount; ++c)
			{
				if(bits[c] > 8)
					{colors[c] = (value & masks[c]) >> shifts[c] >> (bits[c] - 8);}// truncate unneseccary bits
				else
				{
					// move color to the left
					BYTE color = value >> shifts[c] << (8 - bits[c]) & masks[c];
					if(masks[c])
						{colors[c] = color * 0xff / masks[c];}
					else
						{colors[c] = 0;}
				}
			}
			if(flags & DDS_LUMINANCE)
				{out_data[PixelCount] = RGBA(colors[0], colors[0], colors[0], colors[3]);}
			else
			if(flags & DDS_YUV)
				{out_data[PixelCount] = YUY2RGB(colors[0], colors[1], colors[2]);}
			else
			if(!BGR)
				{out_data[PixelCount] = RGBA(colors[0], colors[1], colors[2], colors[3]);}
			else
				{out_data[PixelCount] = RGBA(colors[2], colors[1], colors[0], colors[3]);}
		}
	}
}

inline int maskToShift(UINT mask)
{
	if(mask == 0)
		{return 0;}
	int result = 0;
	while(!((mask >> result) & 1))
		{result++;}
	return result;
}

inline int maskLength(UINT mask)
{
	int result = 0;
	while(mask)
	{
		if(mask & 1)
			{result++;}
		mask >>= 1;
	}
	return result;
}
inline DWORD PF_BitCounter(DDS_PIXELFORMAT &ddspf)
{
	DWORD BitNumber = 0;
	DWORD data[4] = {0};
	data[0] = ddspf.dwRBitMask;
	data[1] = ddspf.dwGBitMask;
	data[2] = ddspf.dwBBitMask;
	data[3] = ddspf.dwRGBAlphaBitMask;
	for(int a1 = 0; a1 < 4; a1++)
	{
		for(int a2 = 0; a2 < 32; a2++)
			{BitNumber = BitNumber + ((data[a1] & (1 << a2)) ? 1 : 0);}
	}
	return BitNumber;
}

bool CTexFile::Alpha_Analysis(DWORD *ImageBuffer, DWORD w, DWORD h, bool mod)
{
	bool b = false;
	int npixels = w * h;
	BYTE alpha_byte = 0;
	if(mod)
	{
		for(int i = 0; i < npixels; i++)
		{
			alpha_byte = GetAValue(ImageBuffer[i]);
			if(alpha_byte != 0x00)
				{b = true; break;}
		}
	}
	else
	{
		for(int i = 0; i < npixels; i++)
		{
			alpha_byte = GetAValue(ImageBuffer[i]);
			if(alpha_byte != 0xff)
				{b = true; break;}
		}
	}
	return b;
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	char msg[512] = {0};
	#ifdef ALTERNATIVE_LANG
		sprintf(msg, "Format [%s]\r\n%s", FreeImage_GetFormatFromFIF(fif), message);
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "Library error FreeImage.", MB_ICONHAND);
	#else
		sprintf(msg, "Формат [%s]\r\n%s", FreeImage_GetFormatFromFIF(fif), message);
		MessageBoxA(AfxGetApp() -> m_pMainWnd -> m_hWnd, msg, "Ошибка библиотеки FreeImage.", MB_ICONHAND);
	#endif
}

FIBITMAP* GenericLoader(const char* lpszPathName, int flag)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	// check the file signature and deduce its format
	// (the second argument is currently not used by FreeImage)

	fif = FreeImage_GetFileType(lpszPathName, 0);
	if (fif == FIF_UNKNOWN)
	{
		// no signature ?
		// try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(lpszPathName);
	}
	// check that the plugin has reading capabilities ...
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif))
	{
		// ok, let's load the file
		FIBITMAP *dib = FreeImage_Load(fif, lpszPathName, flag);
		// unless a bad file format, we are done !
		return dib;
	}
	return NULL;
}
