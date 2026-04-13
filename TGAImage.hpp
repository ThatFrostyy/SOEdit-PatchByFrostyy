//  TGAimage.hpp
//  OpenLGL(Open Light Graphics Library)
//  Modified by TonaTor
 
#ifndef OpenLGL_TGAimage_h
#define OpenLGL_TGAimage_h

#include <stdio.h>

#pragma pack(push,1)
struct TGA_Header
{
	char Id_Length;
	char Color_Map_Type;
	char Data_Type_Code;
	short Color_Map_Origin;
	short Color_Map_Length;
	char Color_Map_Depth;
	short X_Origin;
	short Y_Origin;
	short Width;
	short Height;
	char  Bits_Per_Pixel;
	char  Image_Descriptor;
};
#pragma pack(pop)

struct TGAColor
{
    union
	{
		struct {unsigned char b, g, r, a;};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;
	TGAColor() : val(0), bytespp(1) {}
	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {}
	TGAColor(int v, int bpp) : val(v), bytespp(bpp){}
	TGAColor(const TGAColor &c) : val(c.val), bytespp(c.bytespp){}
	//--------------------------------------------------------------------------
	TGAColor(const unsigned char *p, int bpp) : val(0), bytespp(bpp)
    {
        for (int i=0; i<bpp; i++)
			{raw[i] = p[i];}
	}
	TGAColor & operator =(const TGAColor &c)
    {
        if (this != &c)
        {
            bytespp = c.bytespp;
            val = c.val;
        }
        return *this;
	}
};

typedef enum Errors_TGA
{
	TGA_ERR_OK = 0,            //It's oK
	TGA_ERR_OPEN = 1,          //Can't open file
	TGA_ERR_FORMAT = 2,        //Unknown file format
	TGA_ERR_HEADER = 3,        //An error occured while reading the header
	TGA_ERR_BPP = 4,           //Bad bpp (or width/height) value
	TGA_ERR_READ_DATA = 5,     //An error occured while reading the data
	TGA_ERR_MUCH_MANU_PIX = 6, //Too many pixels read
	TGA_ERR_WRITE = 7,         //Can't write the tga file
	TGA_ERR_WRITE_BUFF = 8,    //Can't unload raw data
	TGA_ERR_WRITE_RLE = 9      //Can't unload rle data
}Errors_TGA;

class TGAImage
{
protected:
	int width;
	int height;
	int bytespp;
	int Bits_Per_Pixel;
	unsigned char* data;
	bool monohrome;
	TGA_Header header;
	DWORD load_rle_data(HANDLE &in);
	bool unload_rle_data(HANDLE &out);
public:
	enum Format
	{
		GRAY_SCALE = 1,
		APROX      = 2,
		RGB        = 3,
		RGBA       = 4
	};
	enum tga_image_type
	{
		TYPE_NODATA       = 0,   ///< image with no data (why even spec it?)
		TYPE_PALETTED     = 1,   ///< paletted RGB
		TYPE_RGB          = 2,   ///< can include alpha
		TYPE_GRAY         = 3,   ///< can include alpha
		TYPE_PALETTED_RLE = 9,   ///< same as PALETTED but run-length encoded
		TYPE_RGB_RLE      = 10,  ///< same as RGB but run-length encoded
		TYPE_GRAY_RLE     = 11   ///< same as GRAY but run-length encoded
	};
	DWORD initial_size, result_size;
	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);

	TGAColor get(int x, int y);
	TGAImage & operator =(const TGAImage &img);

	DWORD Read_TGA_File(char *FileName);
	DWORD Write_TGA_File(char *filename, bool rle = true);
	bool Cheeck_TGA_Signature(char *FileName);
	bool flip_horizontally();
	bool flip_vertically();
	bool Alpha_analize();
	bool Delete_alpha();
	bool scale(int w, int h);
	bool set(int x, int y, TGAColor c);
	int get_width() {return width;}
	int get_height() {return height;}
	int get_bytespp() {return bytespp;}
	int get_BitPP() {return Bits_Per_Pixel;}
	unsigned char *buffer() {return data;}
	char get_type() {return header.Data_Type_Code;}
	void ClearImg() {memset((void *)data, 0, (width * height * bytespp));}
	void Zeroize() {monohrome = false; if(data){delete[] data; data = NULL;} Bits_Per_Pixel = width = height = bytespp = 0; ZeroMemory(&header, sizeof(TGA_Header)); initial_size = result_size = 0;}
	~TGAImage();
};

TGAImage::TGAImage()
{
	ZeroMemory(&header, sizeof(TGA_Header));
	Bits_Per_Pixel = width = height = bytespp = 0;
	data = NULL;
	monohrome = false;
}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp)
{
	unsigned long nbytes = width*height*bytespp;
	Bits_Per_Pixel = bytespp * 8;
	data = new unsigned char[nbytes];
	memset(data, 0, nbytes);
	monohrome = false;
}

TGAImage::TGAImage(const TGAImage &img)
{
	width = img.width;
    height = img.height;
    bytespp = img.bytespp;
	Bits_Per_Pixel = bytespp * 8;
	unsigned long nbytes = width*height*bytespp;
    data = NULL;
	data = new unsigned char[nbytes];
	memcpy(data, img.data, nbytes);
	monohrome = img.monohrome;
}

TGAImage::~TGAImage()
{
	monohrome = false;
	if(data)
	{
		delete[] data;
		data = NULL;
	}
}

TGAImage & TGAImage::operator =(const TGAImage &img)
{
	if(this != &img)
	{
		if (data) delete [] data;
		width  = img.width;
		height = img.height;
		bytespp = img.bytespp;
		Bits_Per_Pixel = img.Bits_Per_Pixel;
		unsigned long nbytes = width*height*bytespp;
		data = new unsigned char[nbytes];
		memcpy(data, img.data, nbytes);
		monohrome = img.monohrome;
	}
    return *this;
}

DWORD TGAImage::Read_TGA_File(char *FileName)
{
	DWORD result = 0;
	DWORD BytesIO = 0;
	if(data)
		{delete[] data; data = NULL;}
	width = height = bytespp = 0;
	monohrome = false;
	ZeroMemory(&header, sizeof(TGA_Header));
	initial_size = result_size = 0;

	HANDLE in = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(in == INVALID_HANDLE_VALUE)
		{in = NULL; return TGA_ERR_OPEN;}
	FILE_STANDARD_INFO fileInfo;
	GetFileInformationByHandleEx(in, FileStandardInfo, &fileInfo, sizeof(fileInfo));
	initial_size = fileInfo.EndOfFile.LowPart;
	if(!ReadFile(in, (char *)&header, sizeof(TGA_Header), &BytesIO, NULL))
		{CloseHandle(in); return TGA_ERR_HEADER;}
	if(BytesIO != sizeof(TGA_Header))
		{CloseHandle(in); return TGA_ERR_HEADER;}
	SetFilePointer(in, header.Id_Length + sizeof(TGA_Header), NULL, FILE_BEGIN);
	initial_size += header.Id_Length;
	width   = header.Width;
	height  = header.Height;
	bytespp = (header.Bits_Per_Pixel == 15) ? 2 : header.Bits_Per_Pixel / 8;
	Bits_Per_Pixel = header.Bits_Per_Pixel;
	int palbytespp = (header.Color_Map_Depth == 15) ? 2 : (header.Color_Map_Depth / 8);

	//cheeck signature
	if(width <= 0 || height <= 0 || (bytespp != GRAY_SCALE && bytespp != RGB && bytespp != RGBA && bytespp != APROX))
		{CloseHandle(in); return TGA_ERR_BPP;}
	else
	if(header.Data_Type_Code != TYPE_RGB && header.Data_Type_Code != TYPE_GRAY \
	&& header.Data_Type_Code != TYPE_GRAY_RLE && header.Data_Type_Code != TYPE_RGB_RLE \
	&& header.Data_Type_Code != TYPE_PALETTED && header.Data_Type_Code != TYPE_PALETTED_RLE)
		{CloseHandle(in); return TGA_ERR_FORMAT;}

	if(bytespp == GRAY_SCALE)
		{monohrome = true;}
	//--------------------------------------------------------------------------
	unsigned long nbytes = bytespp*width*height;
	if(TYPE_GRAY == header.Data_Type_Code || TYPE_RGB == header.Data_Type_Code)
	{
		data = new unsigned char[nbytes];
		if(!ReadFile(in, data, nbytes, &BytesIO, NULL))
			{CloseHandle(in); return TGA_ERR_READ_DATA;}
		if(BytesIO != nbytes)
			{CloseHandle(in); return TGA_ERR_READ_DATA;}
	}
	else
	if(TYPE_RGB_RLE == header.Data_Type_Code || TYPE_GRAY_RLE == header.Data_Type_Code)
	{
		result = load_rle_data(in);
		if(result != TGA_ERR_OK)
			{CloseHandle(in); return result;}
	}
	else
	if(header.Data_Type_Code == TYPE_PALETTED || header.Data_Type_Code == TYPE_PALETTED_RLE)
	{
		if(header.Color_Map_Type != 1 || header.Color_Map_Origin > 0 || header.Bits_Per_Pixel != 8)
			{CloseHandle(in); return TGA_ERR_FORMAT;}
		BYTE *table = new BYTE[header.Color_Map_Length * palbytespp];
		BYTE *inxs = new BYTE[width * height * bytespp];
		memset(table, 0, header.Color_Map_Length * palbytespp);
		memset(inxs, 0, width * height * bytespp);
		if(!ReadFile(in, (BYTE *)table, header.Color_Map_Length * palbytespp, &BytesIO, NULL))
			{CloseHandle(in); delete[] table; return TGA_ERR_HEADER;}
		if(BytesIO != header.Color_Map_Length * palbytespp)
			{CloseHandle(in); return TGA_ERR_HEADER;}
		if(!ReadFile(in, (BYTE *)inxs, width * height * bytespp, &BytesIO, NULL))
			{CloseHandle(in); delete[] table; return TGA_ERR_HEADER;}
		data = new BYTE[width * height * palbytespp];
		int i = 0, inx = 0;
		if(header.Data_Type_Code == TYPE_PALETTED)
		{
			for(int x = 0; x < width; x++)
			{
				for(int y = 0; y < height; y++)
				{
					unsigned int k = inxs[inx] * palbytespp;
					/*data[i + 0] = table[k + 0];
					data[i + 1] = table[k + 1];
					data[i + 2] = table[k + 2];*/
					switch(palbytespp)
					{
						case GRAY_SCALE:
							{data[i + 0] = table[k + 0];}
						break;
						case RGB:
							{data[i + 0] = table[k + 0]; data[i + 1] = table[k + 1]; data[i + 2] = table[k + 2];}
						break;
						case RGBA:
							{data[i + 0] = table[k + 0]; data[i + 1] = table[k + 1]; data[i + 2] = table[k + 2]; data[i + 3] = table[k + 3];}
						break;
						case APROX:
						{
							/*data[i + 0] = (table[k + 0] & 0x1F) << 3;
							data[i + 1] = ((((table[k + 0] & 0xE0) >> 5) | ((table[k + 1] & 0x03) << 3))) << 3;
							data[i + 2] = ((table[k + 1] & 0x7C) >> 2) << 3;*/
							data[i + 0] = table[k + 0];
							data[i + 1] = table[k + 1];
							data[i + 2] = table[k + 2];
						}
						break;
					}
					inx++;
					i += palbytespp;
				}
			}
		}
		else
		{
			BYTE chunkheader = 0, chunk = 0;
			int MaxSize = width * height * palbytespp;
			int MaxInx = width * height;
			for(;i < MaxSize;)
			{
				chunk = inxs[inx];
				chunkheader = 1 + (chunk & 0x7f);
				inx += 1;
				unsigned int k = inxs[inx] * palbytespp;
				inx += 1;
				if(chunk & 0x80)
				{
					for(int i2 = 0; i2 < chunkheader; i2++)
					{
						switch(palbytespp)
						{
							case GRAY_SCALE:
								{data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];}
							break;
							case RGB:
							{
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
							}
							break;
							case RGBA:
							{
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
								data[((i + 3) >= MaxSize) ? MaxSize - 1 : i + 3] = table[k + 3];
							}
							break;
							case APROX:
							{
								/*data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = (table[k + 0] & 0x1F) << 3;
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = ((((table[k + 0] & 0xE0) >> 5) | ((table[k + 1] & 0x03) << 3))) << 3;
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = ((table[k + 1] & 0x7C) >> 2) << 3;*/
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
							}
							break;
						}
						i += palbytespp;
					}
				}
				else
				{
					for(int i2 = 0; i2 < chunkheader; i2++)
					{
						switch(palbytespp)
						{
							case GRAY_SCALE:
								{data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];}
							break;
							case RGB:
							{
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
							}
							break;
							case RGBA:
							{
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
								data[((i + 3) >= MaxSize) ? MaxSize - 1 : i + 3] = table[k + 3];
							}
							break;
							case APROX:
							{
								/*data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = (table[k + 0] & 0x1F) << 3;
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = ((((table[k + 0] & 0xE0) >> 5) | ((table[k + 1] & 0x03) << 3))) << 3;
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = ((table[k + 1] & 0x7C) >> 2) << 3;*/
								data[((i + 0) >= MaxSize) ? MaxSize - 1 : i + 0] = table[k + 0];
								data[((i + 1) >= MaxSize) ? MaxSize - 1 : i + 1] = table[k + 1];
								data[((i + 2) >= MaxSize) ? MaxSize - 1 : i + 2] = table[k + 2];
							}
							break;
						}
						i += palbytespp;
						if(i2 < chunkheader - 1)
						{
							k = inxs[(inx >= MaxInx) ? MaxInx - 1 : inx] * palbytespp;
							inx += 1;
						}
					}
				}
			}
		}
		bytespp = palbytespp;
		delete[] inxs;
		delete[] table;
	}
	if(!(header.Image_Descriptor & 0x20))
		{flip_vertically();}
	if(header.Image_Descriptor & 0x10)
		{flip_horizontally();}
	CloseHandle(in);
	return TGA_ERR_OK;
}

DWORD TGAImage::load_rle_data(HANDLE &in)
{
	DWORD currentpixel = 0, pixelcount = width*height, currentbyte = 0, BytesIO = 0, ByteCount = 0;
	TGAColor colorbuffer;
	BYTE chunkheader = 0, colors[4], OldSpp = bytespp;//bytespp и OldSpp могли отличаться в старых версиях
	DWORD useful_size = initial_size - (sizeof(TGA_Header) + header.Id_Length);
	BYTE *FileBuffer = (BYTE*) malloc(useful_size);
	delete []data; data = NULL;
	data = new BYTE[width * height * bytespp];
	memset(data, 0, width * height * bytespp);
	if(!ReadFile(in, FileBuffer, useful_size, &BytesIO, NULL))
		{CloseHandle(in); return TGA_ERR_READ_DATA;}
	for(;currentpixel < pixelcount;)
	{
		chunkheader = FileBuffer[ByteCount++];
		if(chunkheader < 128)
		{
			chunkheader++;
			for(int i = 0; i < chunkheader; i++)
			{
				for(int i2 = 0; i2 < bytespp; i2++)
				{
					if(ByteCount > BytesIO)
						{free(FileBuffer); FileBuffer = NULL; return TGA_ERR_READ_DATA;}
					data[currentbyte++] = (i2 < OldSpp) ? FileBuffer[ByteCount++] : ((OldSpp == GRAY_SCALE) ? ((i2 < 3) ? FileBuffer[ByteCount-1] : 255) : 255);
				}
				currentpixel++;
				if(currentpixel > pixelcount)
					{free(FileBuffer); FileBuffer = NULL; return TGA_ERR_MUCH_MANU_PIX;}
			}
		}
		else
		{
			chunkheader -= 127;
			colors[0] = colors[1] = colors[2] = colors[3] = 0;
			for(int i2 = 0; i2 < bytespp; i2++)
			{
				if(ByteCount > BytesIO)
					{free(FileBuffer); FileBuffer = NULL; return TGA_ERR_READ_DATA;}
				colors[i2] = (i2 < OldSpp) ? FileBuffer[ByteCount++] : ((OldSpp == GRAY_SCALE) ? ((i2 < 3) ? FileBuffer[ByteCount-1] : 255) : 255);
			}
			for(int i = 0; i < chunkheader; i++)
			{
				data[currentbyte++] = colors[0];
				if(bytespp > 1)
					{data[currentbyte++] = colors[1];}
				if(bytespp > 2)
					{data[currentbyte++] = colors[2];}
				if(bytespp > 3)
					{data[currentbyte++] = colors[3];}
                currentpixel++;
				if(currentpixel > pixelcount)
					{free(FileBuffer); FileBuffer = NULL; return TGA_ERR_MUCH_MANU_PIX;}
			}
		}
	}
	free(FileBuffer); FileBuffer = NULL;
	return TGA_ERR_OK;
}

DWORD TGAImage::Write_TGA_File(char *filename, bool rle)
{
	unsigned char developer_area_ref[4] = {0, 0, 0, 0};
	unsigned char extension_area_ref[4] = {0, 0, 0, 0};
	unsigned char footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
	HANDLE out;
	DWORD BytesIO = 0, size_buff = 0;
	out = CreateFileA(filename, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, 0, 0);
	if(out == INVALID_HANDLE_VALUE)
		{CloseHandle(out); return TGA_ERR_OPEN;}
	TGA_Header header;
	ZeroMemory(&header, sizeof(TGA_Header));
	header.Bits_Per_Pixel = bytespp << 3;
	header.Width = width;
	header.Height = height;
	header.Data_Type_Code = ((bytespp == GRAY_SCALE) ? ((rle) ? TYPE_GRAY_RLE : TYPE_GRAY) : ((rle) ? TYPE_RGB_RLE : TYPE_RGB));
	header.Image_Descriptor = 0x20; // top-left origin
	size_buff = width*height*bytespp;
	result_size = sizeof(header);
	if(!WriteFile(out, (char*)&header, sizeof(header), &BytesIO, NULL))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(BytesIO != sizeof(header))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(!rle)
	{
		result_size += size_buff;
		if(!WriteFile(out, (char*)data, size_buff, &BytesIO, NULL))
			{CloseHandle(out); return TGA_ERR_WRITE_BUFF;}
		if(BytesIO != size_buff)
			{CloseHandle(out); return TGA_ERR_WRITE_BUFF;}
	}
    else
    {
		if(!unload_rle_data(out))
			{CloseHandle(out); return TGA_ERR_WRITE_RLE;}
	}
	result_size += sizeof(developer_area_ref) + sizeof(extension_area_ref) + sizeof(footer);
	if(!WriteFile(out, (BYTE*)&developer_area_ref, sizeof(developer_area_ref), &BytesIO, NULL))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(BytesIO != sizeof(developer_area_ref))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(!WriteFile(out, (BYTE*)&extension_area_ref, sizeof(extension_area_ref), &BytesIO, NULL))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(BytesIO != sizeof(extension_area_ref))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(!WriteFile(out, (BYTE*)&footer, sizeof(footer), &BytesIO, NULL))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	if(BytesIO != sizeof(footer))
		{CloseHandle(out); return TGA_ERR_WRITE;}
	CloseHandle(out);
	return TGA_ERR_OK;
}

bool TGAImage::unload_rle_data(HANDLE &out)
{
	const unsigned char max_chunk_length = 128;
	unsigned long npixels = width*height;
	unsigned long curpix = 0;
	DWORD BytesIO = 0, size_buff = 0;
	BYTE *FileBuffer = (BYTE*) malloc(0xA00000); // 10 mb
	DWORD StackSize = 0;
	memset(FileBuffer, 0, 0xA00000);
	while(curpix < npixels)
	{
		unsigned long chunkstart = curpix * bytespp;
		unsigned long curbyte = curpix * bytespp;
		unsigned char run_length = 1;
		bool raw = true;
		while((curpix + run_length) < npixels && run_length < max_chunk_length)
        {
			bool succ_eq = true;
			for(int t = 0; succ_eq && t < bytespp; t++)
				{succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);}
			curbyte += bytespp;
			if(1 == run_length)
				{raw = !succ_eq;}
			if(raw && succ_eq)
				{run_length--; break;}
			if(!raw && !succ_eq)
				{break;}
            run_length++;
		}
		curpix += run_length;
		FileBuffer[StackSize++] = (raw) ? (run_length - 1) : (run_length + 127);
		size_buff = (raw) ? (run_length * bytespp) : bytespp;
		memcpy(FileBuffer + StackSize, (data + chunkstart), size_buff);
		StackSize += size_buff;
		if(StackSize > 0x9F0000)
		{
			result_size += StackSize;
			if(!WriteFile(out, FileBuffer, StackSize, &BytesIO, NULL))
				{free(FileBuffer); FileBuffer = NULL; return false;}
			if(BytesIO != StackSize)
				{free(FileBuffer); FileBuffer = NULL; return false;}
			StackSize = 0;
			memset(FileBuffer, 0, 0xA00000);
		}
	}
	if(StackSize > 0)
	{
		result_size += StackSize;
		if(!WriteFile(out, FileBuffer, StackSize, &BytesIO, NULL))
			{free(FileBuffer); FileBuffer = NULL; return false;}
		if(BytesIO != StackSize)
			{free(FileBuffer); FileBuffer = NULL; return false;}
		StackSize = 0;
		memset(FileBuffer, 0, 0xA00000);
	}
	free(FileBuffer); FileBuffer = NULL;
	return true;
}

TGAColor TGAImage::get(int x, int y)
{
	if (!data || x<0 || y<0 || x>=width || y>=height) {
		return TGAColor();
	}
	return TGAColor((data + (x + y * width) * bytespp), bytespp);
}

bool TGAImage::set(int x, int y, TGAColor c)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);
	return true;
}

bool TGAImage::flip_horizontally()
{
	if (!data) return false;
	int half = width>>1;
	for (int i=0; i<half; i++)
    {
		for (int j=0; j<height; j++)
        {
			TGAColor c1 = get(i, j);
			TGAColor c2 = get(width-1-i, j);
            set(i, j, c2);
            set(width-1-i, j, c1);
        }
	}
    return true;
}

bool TGAImage::flip_vertically()
{
	if (!data) return false;
    unsigned long bytes_per_line = width*bytespp;
	unsigned char *line = new unsigned char[bytes_per_line];
	int half = height >> 1;
	for (int j=0; j<half; j++)
    {
        unsigned long l1 = j*bytes_per_line;
        unsigned long l2 = (height - 1 - j) * bytes_per_line;

        memmove((void *)line,      (void *)(data + l1), bytes_per_line);
		memmove((void *)(data+l1), (void *)(data + l2), bytes_per_line);
        memmove((void *)(data+l2), (void *)line,        bytes_per_line);
    }
    delete [] line;
    return true;
}

bool TGAImage::scale(int w, int h) {
    if (w <= 0 || h <= 0 || !data) return false;

	unsigned char *tdata = new unsigned char[(w * h * bytespp)];

    int nscanline = 0;
    int oscanline = 0;
	int erry = 0;

    unsigned long nlinebytes = w * bytespp;
    unsigned long olinebytes = width * bytespp;

    for (int j = 0; j < height; j++)
    {
        int errx = width-w;
        int nx   = -bytespp;
        int ox   = -bytespp;
        for (int i=0; i<width; i++) {
            ox += bytespp;
            errx += w;
            while (errx>=(int)width) {
                errx -= width;
                nx += bytespp;
                memcpy(tdata+nscanline+nx, data+oscanline+ox, bytespp);
            }
        }
        erry += h;
        oscanline += olinebytes;
        while (erry>=(int)height) {
			if (erry>=(int)height<<1)
				memcpy(tdata+nscanline+nlinebytes, tdata+nscanline, nlinebytes);
            erry -= height;
            nscanline += nlinebytes;
		}
	}
	delete [] data;
	data = tdata;
	width = w;
	height = h;
	return true;
}

bool TGAImage::Cheeck_TGA_Signature(char *FileName)
{
	bool result = true;
	int spp = 0;
	DWORD BytesIO = 0;
	HANDLE in;
	TGA_Header header_cheeck;
	ZeroMemory(&header_cheeck, sizeof(TGA_Header));
	in = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(in == INVALID_HANDLE_VALUE)
		{CloseHandle(in); return false;}
	if(!ReadFile(in, (char *)&header_cheeck, sizeof(TGA_Header), &BytesIO, NULL))
		{CloseHandle(in); return false;}
	CloseHandle(in);
	if(BytesIO != sizeof(TGA_Header))
		{return false;}
	spp = (header_cheeck.Bits_Per_Pixel == 15) ? 2 : header_cheeck.Bits_Per_Pixel / 8;
	if(header_cheeck.Width <= 0 || header_cheeck.Height <= 0 || (spp != GRAY_SCALE && spp != RGB && spp != RGBA && spp != APROX))
		{return false;}
	if(header_cheeck.Data_Type_Code != TYPE_RGB && header_cheeck.Data_Type_Code != TYPE_GRAY \
	&& header_cheeck.Data_Type_Code != TYPE_RGB_RLE && header_cheeck.Data_Type_Code != TYPE_GRAY_RLE \
	&& header_cheeck.Data_Type_Code != TYPE_PALETTED && header_cheeck.Data_Type_Code != TYPE_PALETTED_RLE)
		{return false;}
	width = header_cheeck.Width;
	height = header_cheeck.Height;
	return result;
}

bool TGAImage::Alpha_analize()
{
	if(!data || bytespp != RGBA)
		{return false;}
	//wprintf(L"[Анализ альфа-канала] -> ");
	TGAColor col;
	bool b = true;
	for(int w = 0; w < width && b; w++)
	for(int h = 0; h < height && b; h++)
	{
		col = get(w, h);
		if(col.a != 255)
			{b = false;}
	}
	return b;
}

bool TGAImage::Delete_alpha()
{
	if(!data || bytespp != RGBA)
		{return false;}
	DWORD npixels = width * height;
	BYTE *tdata = (BYTE*) malloc(npixels * 3);
	memset(tdata, 0, npixels * 3);
	DWORD pixr = 0, pixw = 0;
	for(DWORD i = 0; i < npixels; i++)
	{
		tdata[pixw] = data[pixr];
		tdata[pixw+1] = data[pixr+1];
		tdata[pixw+2] = data[pixr+2];
		pixr += 4;
		pixw += 3;
	}
	delete[] data; data = NULL;
	bytespp = 3;
	data = new BYTE[npixels * bytespp];
	memset(data, 0, npixels * 3);
	memmove(data, tdata, npixels * bytespp);
	free(tdata); tdata = NULL;
	return true;
}

#endif
