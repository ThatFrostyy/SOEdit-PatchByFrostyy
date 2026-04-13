#ifndef DXT_UNPACK
#define DXT_UNPACK

#include "dxgiformat.h"
#include <math.h>

#define MIN(a,b) ((a < b) ? a : b)
#define MAX(a,b) ((a > b) ? a : b)

#define GETL16(buf) \
   (((unsigned short)(buf)[0]     ) | \
	((unsigned short)(buf)[1] << 8))

#define GETL64(buf) \
   (((unsigned long long)(buf)[0]      ) | \
	((unsigned long long)(buf)[1] <<  8) | \
	((unsigned long long)(buf)[2] << 16) | \
	((unsigned long long)(buf)[3] << 24) | \
	((unsigned long long)(buf)[4] << 32) | \
	((unsigned long long)(buf)[5] << 40) | \
	((unsigned long long)(buf)[6] << 48) | \
	((unsigned long long)(buf)[7] << 56))

struct DXT_unpack
{
	int dxt_decompress(unsigned char *dst, unsigned char *src, int format, unsigned int width, unsigned int height, int normals);
	void decode_color_block(unsigned char *block, unsigned char *src, int format);
	void decode_alpha_block_BC2(unsigned char *block, unsigned char *src);
	void decode_alpha_block_BC3(unsigned char *block, unsigned char *src, int w);
	void normalize_block(unsigned char *block, int format);
	void put_block(unsigned char *dst, unsigned char *block, unsigned int bx, unsigned int by, unsigned int width, unsigned height);
	void unpack_rgb565(unsigned char *dst, unsigned short v);
	void lerp_rgb13(unsigned char *dst, unsigned char *a, unsigned char *b);
	void make_normal(unsigned char *dst, unsigned char x, unsigned char y);
};


int DXT_unpack::dxt_decompress(unsigned char *dst, unsigned char *src, int format, unsigned int width, unsigned int height, int normals)
{
	unsigned char *s;
	unsigned int x, y;
	unsigned char block[16 * 4];
	s = src;

	for(y = 0; y < height; y += 4)
	{
		for(x = 0; x < width; x += 4)
		{
			memset(block, 255, 16 * 4);

			if(format == DXGI_FORMAT_BC1_UNORM)
			{
				decode_color_block(block, s, format);
				s += 8;
			}
			else
			if(format == DXGI_FORMAT_BC2_UNORM)
			{
				decode_alpha_block_BC2(block + 3, s);
				decode_color_block(block, s + 8, format);
				s += 16;
			}
			else
			if(format == DXGI_FORMAT_BC3_UNORM)
			{
				decode_alpha_block_BC3(block + 3, s, width);
				decode_color_block(block, s + 8, format);
				s += 16;
			}
			else
			if(format == DXGI_FORMAT_BC5_UNORM)
			{
				decode_alpha_block_BC3(block, s + 8, width);
				decode_alpha_block_BC3(block + 1, s, width);
				s += 16;
			}
			if(normals)
				{normalize_block(block, format);}
			put_block(dst, block, x, y, width, height);
		}
	}
	return(1);
}

void DXT_unpack::decode_color_block(unsigned char *block, unsigned char *src,
                               int format)
{
   int i, x, y;
   unsigned char *d = block;
   unsigned int indices, idx;
   unsigned char colors[4][3];
   unsigned short c0, c1;

   c0 = GETL16(&src[0]);
   c1 = GETL16(&src[2]);

   unpack_rgb565(colors[0], c0);
   unpack_rgb565(colors[1], c1);
   
   if((c0 > c1) || format == DXGI_FORMAT_BC1_UNORM || format == DXGI_FORMAT_BC3_UNORM)
   {
	  lerp_rgb13(colors[2], colors[0], colors[1]);
	  lerp_rgb13(colors[3], colors[1], colors[0]);
   }
   else
   {
      for(i = 0; i < 3; ++i)
      {
         colors[2][i] = (colors[0][i] + colors[1][i] + 1) >> 1;
         colors[3][i] = 255;
      }
   }

   src += 4;
   for(y = 0; y < 4; ++y)
   {
	  indices = src[y];
      for(x = 0; x < 4; ++x)
      {
         idx = indices & 0x03;
         d[0] = colors[idx][2];
         d[1] = colors[idx][1];
         d[2] = colors[idx][0];
         if(format == DXGI_FORMAT_BC1_UNORM)
            d[3] = ((c0 <= c1) && idx == 3) ? 0 : 255;
		 indices >>= 2;
         d += 4;
      }
   }
}

/* unpack RGB565 to BGR */
void DXT_unpack::unpack_rgb565(unsigned char *dst, unsigned short v)
{
   int r = (v >> 11) & 0x1f;
   int g = (v >>  5) & 0x3f;
   int b = (v      ) & 0x1f;

   dst[0] = (b << 3) | (b >> 2);
   dst[1] = (g << 2) | (g >> 4);
   dst[2] = (r << 3) | (r >> 2);
}

void DXT_unpack::lerp_rgb13(unsigned char *dst, unsigned char *a, unsigned char *b)
{
   /*
   * according to the S3TC/DX10 specs, this is the correct way to do the
   * interpolation (with no rounding bias)
   *
   * dst = (2 * a + b) / 3;
   */
   dst[0] = (2 * a[0] + b[0]) / 3;
   dst[1] = (2 * a[1] + b[1]) / 3;
   dst[2] = (2 * a[2] + b[2]) / 3;
}

void DXT_unpack::decode_alpha_block_BC2(unsigned char *block, unsigned char *src)
{
   int x, y;
   unsigned char *d = block;
   unsigned int bits;

   for(y = 0; y < 4; ++y)
   {
	  bits = GETL16(&src[2 * y]);
	  for(x = 0; x < 4; ++x)
      {
         d[0] = (bits & 0x0f) * 17;
         bits >>= 4;
         d += 4;
      }
   }
}

void DXT_unpack::decode_alpha_block_BC3(unsigned char *block, unsigned char *src, int w)
{
   int x, y, code;
   unsigned char *d = block;
   unsigned char a0 = src[0];
   unsigned char a1 = src[1];
   unsigned long long bits = GETL64(src) >> 16;

   for(y = 0; y < 4; ++y)
   {
      for(x = 0; x < 4; ++x)
      {
         code = ((unsigned int)bits) & 0x07;
         if(code == 0)
			d[0] = a0;
         else if(code == 1)
            d[0] = a1;
         else if(a0 > a1)
            d[0] = ((8 - code) * a0 + (code - 1) * a1) / 7;
         else if(code >= 6)
            d[0] = (code == 6) ? 0 : 255;
         else
            d[0] = ((6 - code) * a0 + (code - 1) * a1) / 5;
         bits >>= 3;
         d += 4;
      }
      if(w < 4) bits >>= (3 * (4 - w));
   }
}

void DXT_unpack::normalize_block(unsigned char *block, int format)
{
	int x, y, tmp;
	for(y = 0; y < 4; ++y)
	{
		for(x = 0; x < 4; ++x)
		{
			if(format == DXGI_FORMAT_BC3_UNORM)
			{
				tmp = block[y * 16 + (x * 4)];
				make_normal(&block[y * 16 + (x * 4)],
							block[y * 16 + (x * 4) + 3],
							block[y * 16 + (x * 4) + 1]);
				block[y * 16 + (x * 4) + 3] = tmp;
			}
		}
	}
}

void DXT_unpack::make_normal(unsigned char *dst, unsigned char x, unsigned char y)
{
   float nx = 2.0f * ((float)x / 255.0f) - 1.0f;
   float ny = 2.0f * ((float)y / 255.0f) - 1.0f;
   float nz = 0.0f;
   float d = 1.0f - nx * nx + ny * ny;
   int z;

   if(d > 0) nz = sqrtf(d);

   z = (int)(255.0f * (nz + 1) / 2.0f);
   z = MAX(0, MIN(255, z));

   dst[0] = x;
   dst[1] = y;
   dst[2] = z;
}

void DXT_unpack::put_block(unsigned char *dst, unsigned char *block,
                      unsigned int bx, unsigned int by,
                      unsigned int width, unsigned height)
{
   int x, y, i;
   unsigned char *d;

   for(y = 0; y < 4 && ((by + y) < height); ++y)
   {
      d = dst + ((y + by) * width + bx) * 4;
      for(x = 0; x < 4 && ((bx + x) < width); ++x)
      {
         for(i = 0; i < 4; ++ i)
			*d++ = block[y * 16 + (x * 4) + i];
      }
   }
}

//
#endif






















