#include <stdlib.h>
#include <string.h>

#include "d3_image.hpp"
#include "platform.h"

// define = 1 if you dont have pre-quantized images to load into palettized D3D texture surfaces
#define QUANTISE_ON_LOAD 0

#if debug
#define DEBUG_TRANSPARENCY 1
#else
#define DEBUG_TRANSPARENCY 0
#endif

// image loader stuff

#if OUTPUT_LOG
#include "debuglog.hpp"
LogFile CL_LogFile("DB_IMLD.LOG");
#endif

#if TIME_LOADS
unsigned int OpenTime;
unsigned int CloseTime;
unsigned int ReadTime;
#endif

static inline unsigned long read_le_dword(D3I_FILE * f)
{
	unsigned char d1,d2,d3,d4;
	d3i_fread(&d1,1,1,f);
	d3i_fread(&d2,1,1,f);
	d3i_fread(&d3,1,1,f);
	d3i_fread(&d4,1,1,f);

	return (unsigned long)d1 | (unsigned long)d2<<8 | (unsigned long)d3<<16 | (unsigned long)d4<<24;
}

static inline unsigned short read_le_word(D3I_FILE * f)
{
	unsigned char d1,d2;
	d3i_fread(&d1,1,1,f);
	d3i_fread(&d2,1,1,f);

	return (unsigned short)d1 | (unsigned short)((unsigned short)d2<<8);
}

// some default globals
CL_ImageMode CL_Image::imode = CLM_GLOBALPALETTE;
CL_ImageMode CL_Image::imode_d3d = CLM_ATTACHEDPALETTE;
CL_ImageMode CL_Image::imode_ddraw = CLM_GLOBALPALETTE;
LPDDSURFACEDESC CL_Image::format = 0;
unsigned int CL_Image::bitsperpixel;
unsigned int CL_Image::bitsperpixel_d3d;
unsigned int CL_Image::bitsperpixel_ddraw;
CL_LoadMode CL_Image::lmode = CLL_DDSURFACE;

CL_DX_Format CL_Pixel_32::f;
CL_DX_Format CL_Pixel_32::f_d3d;
CL_DX_Format CL_Pixel_32::f_ddraw;
CL_DX_Format CL_Pixel_16::f;
CL_DX_Format CL_Pixel_16::f_d3d;
CL_DX_Format CL_Pixel_16::f_ddraw;


void CL_Image::DeleteNotMips(void)
{
	if (im24)
	{
		delete[] *im24;
		delete[] im24;
		im24 = 0;
	}
	if (im16)
	{
		delete[] *im16;
		delete[] im16;
		im16 = 0;
	}
	if (im8)
	{
		delete[] *im8;
		delete[] im8;
		im8 = 0;
	}
	if (palette)
	{
		delete[] palette;
		palette = 0;
		palette_size = 0;
	}
	flags.loaded = 0;
	flags.raw16bit = 0;
}

void CL_Image::Delete(void)
{
	DeleteNotMips();
	while (mipmaps.size())
	{
		delete mipmaps.last_entry();
		mipmaps.delete_last_entry();
	}
}


void CL_Image::Copy(CL_Image const & i2)
{
	if (i2.im24)
	{
		im24 = new CL_Pixel_24 * [height];
		*im24 = new CL_Pixel_24 [size];

		CL_Pixel_24 const * sptr = *i2.im24;
		CL_Pixel_24 * dptr = *im24;

		for (int i=size; i; --i)
		{
			*dptr++ = *sptr++;
		}

		CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
		CL_Pixel_24 const * aptr = *im24;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}

	if (i2.im16)
	{
		im16 = new CL_Pixel_16 * [height];
		*im16 = new CL_Pixel_16 [size];

		CL_Pixel_16 const * sptr = *i2.im16;
		CL_Pixel_16 * dptr = *im16;

		for (int i=size; i; --i)
		{
			*dptr++ = *sptr++;
		}

		CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
		CL_Pixel_16 const * aptr = *im16;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}

	if (i2.im8)
	{
		im8 = new unsigned char * [height];
		*im8 = new unsigned char [size];

		unsigned char const * sptr = *i2.im8;
		unsigned char * dptr = *im8;

		for (int i=size; i; --i)
		{
			*dptr++ = *sptr++;
		}

		unsigned char const * * dptrptr = (unsigned char const * *) im8;
		unsigned char const * aptr = *im8;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}

	if (i2.palette)
	{
		palette = new CL_Pixel_24 [palette_size];

		CL_Pixel_24 const * sptr = i2.palette;
		CL_Pixel_24 * dptr = palette;

		for (int i=palette_size; i; --i)
		{
			*dptr++ = *sptr;
		}
	}

	if (i2.name)
	{
		name = new char [strlen(i2.name)+1];
		strcpy(name,i2.name);
	}

	if (i2.fname)
	{
		fname = new char [strlen(i2.fname)+1];
		strcpy(fname,i2.fname);
	}
	
	for (CLIF<CL_Image *> i_mip(&i2.mipmaps); !i_mip.done(); i_mip.next())
	{
		mipmaps.add_entry_end(new CL_Image(*i_mip()));
	}
}


CL_Image::~CL_Image()
{
	Delete();
	if (fname) delete[] fname;
	if (name) delete[] name;
}


CL_Image::CL_Image(CL_Image const & i2)
: width(i2.width)
, height(i2.height)
, size(i2.size)
, im24(0)
, im16(0)
, im8(0)
, palette(0)
, palette_size(i2.palette_size)
, fname(0)
, name(0)
, flags(i2.flags)
{
	Copy(i2);
}


CL_Image & CL_Image::operator = (CL_Image const & i2)
{
	if (&i2 != this)
	{
		Delete();
		if (fname) delete[] fname;
		if (name) delete[] name;

		width = i2.width;
		height = i2.height;
		size = i2.size;
		palette_size = i2.palette_size;

		fname = 0;
		name = 0;

		flags = i2.flags;

		Copy(i2);
	}
	return *this;
}


CL_Error CL_Image::Make(int const _width, int const _height)
{
	Delete();

	width = _width;
	height = _height;
	palette_size = 0;
	size = width*height;

	switch (imode)
	{
		case CLM_ATTACHEDPALETTE:
		{
			palette_size = 256;
			palette = new CL_Pixel_24 [palette_size];
		}
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
		{
			im8 = new unsigned char * [height];
			*im8 = new unsigned char [size];

			unsigned char const * * dptrptr = (unsigned char const * *) im8;
			unsigned char * dptr = *im8;
			for (int i=height; i; --i, dptr+=width)
			{
				*dptrptr++ = dptr;
			}

			break;
		}
		case CLM_24BIT:
		{
			im24 = new CL_Pixel_24 * [height];
			*im24 = new CL_Pixel_24 [size];

			CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
			CL_Pixel_24 const * aptr = *im24;
			for (int i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}

			break;
		}
		case CLM_16BIT:
		{
			im16 = new CL_Pixel_16 * [height];
			*im16 = new CL_Pixel_16 [size];

			CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
			CL_Pixel_16 const * aptr = *im16;
			for (int i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}

			break;
		}
		case CLM_32BIT:
		{
			im32 = new CL_Pixel_32 * [height];
			*im32 = new CL_Pixel_32 [size];

			CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
			CL_Pixel_32 const * aptr = *im32;
			for (int i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}

			break;
		}
		default:
			return CLE_INVALIDDXMODE;
	}
	
	flags.loaded = 1;
	return CLE_OK;
}


CL_Error CL_Image::MakeRandom(int const _width, int const _height, int const seed)
{
	Delete();

	if (-1!=seed) srand((unsigned int)seed);

	width = _width;
	height = _height;
	size = width*height;
	palette_size = 0;

	switch (imode)
	{
		case CLM_ATTACHEDPALETTE:
		{
			palette_size = 256;
			palette = new CL_Pixel_24 [palette_size];
			CL_Pixel_24 * palP = palette;
			for (int i=palette_size; i; --i, ++palP)
			{
				*palP = CL_Pixel_24(rand()&255,rand()&255,rand()&255);
			}
		}
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
		{
			im8 = new unsigned char * [height];
			*im8 = new unsigned char [size];

			unsigned char const * * dptrptr = (unsigned char const * *) im8;
			unsigned char * dptr = *im8;
			for (int i=height; i; --i)
			{
				*dptrptr++ = dptr;
				for (int j=width; j; --j, ++dptr)
					*dptr = (unsigned char)(rand()&(palette_size-1));
			}

			break;
		}
		case CLM_24BIT:
		{
			im24 = new CL_Pixel_24 * [height];
			*im24 = new CL_Pixel_24 [size];

			CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
			CL_Pixel_24 * aptr = *im24;
			for (int i=height; i; --i)
			{
				*dptrptr++ = aptr;
				for (int j=width; j; --j, ++aptr)
					*aptr = CL_Pixel_24(rand()&255,rand()&255,rand()&255);
			}

			break;
		}
		case CLM_16BIT:
		{
			im16 = new CL_Pixel_16 * [height];
			*im16 = new CL_Pixel_16 [size];

			CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
			CL_Pixel_16 * aptr = *im16;
			for (int i=height; i; --i)
			{
				*dptrptr++ = aptr;
				for (int j=width; j; --j, ++aptr)
					*aptr = CL_Pixel_24(rand()&255,rand()&255,rand()&255);
			}

			break;
		}
		case CLM_32BIT:
		{
			im32 = new CL_Pixel_32 * [height];
			*im32 = new CL_Pixel_32 [size];

			CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
			CL_Pixel_32 * aptr = *im32;
			for (int i=height; i; --i)
			{
				*dptrptr++ = aptr;
				for (int j=width; j; --j, ++aptr)
					*aptr = CL_Pixel_24(rand()&255,rand()&255,rand()&255);
			}

			break;
		}
		default:
			return CLE_INVALIDDXMODE;
	}
	
	flags.loaded = 1;
	return CLE_OK;
}


CL_Error CL_Image::Load_BMP(D3I_FILE * f)
{
	if (!f) return CLE_OPENERROR;
	
	DeleteNotMips();

	switch (imode)
	{
		case CLM_ATTACHEDPALETTE:
		case CLM_24BIT:
		case CLM_16BIT:
		case CLM_32BIT:
			break;
		default:
			return CLE_INVALIDDXMODE;
	}

	d3i_fseek(f,0,SEEK_SET);
	unsigned short magic;
	d3i_fread(&magic,2,1,f);
	if (magic != *(unsigned short const *)"BM") return CLE_LOADERROR;

	size_t filesize = read_le_dword(f);
	d3i_fseek(f,4,SEEK_CUR);
	size_t offset = read_le_dword(f);
	size_t headsize = read_le_dword(f);

	unsigned short bitdepth;

	if (12 == headsize) // OS/2 1.x
	{
		width = read_le_word(f);
		height = read_le_word(f);
		size = width * height;
		if (!width || !height) return CLE_LOADERROR;
		unsigned short planes = read_le_word(f);
		if (1 != planes) return CLE_LOADERROR;
		bitdepth = read_le_word(f);

		if (bitdepth != 24)
		{
			palette_size = 1<<bitdepth;
			palette = new CL_Pixel_24 [palette_size];
			CL_Pixel_24 * rptr = palette;
			for (int i=palette_size; i; --i)
			{
				rptr++->Read(f,CLF_BGR);
			}
		}
	}
	else if (40 == headsize || 64 == headsize) // Windows 3.x || OS/2 2.x
	{
		width = read_le_dword(f);
		height = read_le_dword(f);
		unsigned short planes = read_le_word(f);
		if (1 != planes) return CLE_LOADERROR;
		bitdepth = read_le_word(f);

		if (read_le_dword(f)) return CLE_LOADERROR; // compressed bmps not supported

		size = read_le_dword(f);

		d3i_fseek(f,8,SEEK_CUR);
		palette_size = read_le_dword(f);
		if (!palette_size && bitdepth != 24) palette_size = 1<<bitdepth;

		if (palette_size)
		{
			d3i_fseek(f,headsize+14,SEEK_SET);
			palette = new CL_Pixel_24 [palette_size];
			CL_Pixel_24 * rptr = palette;
			for (int i=palette_size; i; --i)
			{
				rptr++->Read(f,CLF_BGRX);
			}
		}
	}
	else return CLE_LOADERROR;

	d3i_fseek(f,offset,SEEK_SET);
	
	if (palette_size)
	{
		if (bitdepth < 4)
		{
			delete[] palette;
			palette_size = 0;
			return CLE_LOADERROR;
		}
		
		im8 = new unsigned char * [height];
		*im8 = new unsigned char [size];

		unsigned char const * * dptrptr = (unsigned char const * *) im8;
		unsigned char * dptr = *im8;
		for (int i=height; i; --i, dptr+=width)
		{
			*dptrptr++ = dptr;
		}
		
		#if DEBUG_TRANSPARENCY
		int num_tps=0;
		#endif
		for (i=height-1; i>=0; --i)
		{
			dptr = im8[i];
			
			if (4==bitdepth)
			{
				for (int i=width>>1; i; --i)
				{
					unsigned char byte;
					d3i_fread(&byte,1,1,f);
					#if DEBUG_TRANSPARENCY
					*dptr = (unsigned char)(byte >> 4 & 0xf);
					if (!*dptr) num_tps++;
					*++dptr = (unsigned char)(byte & 0xf);
					if (!*dptr) num_tps++;
					++dptr;
					#else
					*dptr++ = (unsigned char)(byte >> 4 & 0xf);
					*dptr++ = (unsigned char)(byte & 0xf);
					#endif
				}
				if (width & 1)
				{
					d3i_fread(dptr,1,1,f);
					*dptr &= 0xf;
					#if DEBUG_TRANSPARENCY
					if (!*dptr) num_tps++;
					#endif
				}

				d3i_fseek(f,(~(width-1) & 7)>>1,SEEK_CUR);
			}
			else
			{
				for (int i=width; i; --i)
				{
					#if DEBUG_TRANSPARENCY
					d3i_fread(dptr,1,1,f);
					if (!*dptr) num_tps++;
					dptr++;
					#else
					d3i_fread(dptr++,1,1,f);
					#endif
				}
				d3i_fseek(f,~(width-1) & 3,SEEK_CUR);
			}
		}
		#if DEBUG_TRANSPARENCY
		if (num_tps) CL_LogFile.lprintf("-- %d TRANSPARENT PIXELS FOUND\n",num_tps);
		#endif
	}
	else if (CLM_16BIT == imode)
	{
		im16 = new CL_Pixel_16 * [height];
		*im16 = new CL_Pixel_16 [size];

		CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
		CL_Pixel_16 const * aptr = *im16;
		for (int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}

		for (i=height-1; i>=0; --i)
		{
			CL_Pixel_16 * dptr = im16[i];
			
			for (int i=width; i; --i)
			{
				dptr++->Read(f,CLF_BGR);
			}
			d3i_fseek(f,~(width*3-1) & 3,SEEK_CUR);
		}
		
	}
	else if (CLM_32BIT == imode)
	{
		im32 = new CL_Pixel_32 * [height];
		*im32 = new CL_Pixel_32 [size];

		CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
		CL_Pixel_32 const * aptr = *im32;
		for (int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}

		for (i=height-1; i>=0; --i)
		{
			CL_Pixel_32 * dptr = im32[i];
			
			for (int i=width; i; --i)
			{
				dptr++->Read(f,CLF_BGR);
			}
			d3i_fseek(f,~(width*3-1) & 3,SEEK_CUR);
		}
		
	}
	else if (CLM_24BIT == imode)
	{
		im24 = new CL_Pixel_24 * [height];
		*im24 = new CL_Pixel_24 [size];

		CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
		CL_Pixel_24 const * aptr = *im24;
		for (int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}

		for (i=height-1; i>=0; --i)
		{
			CL_Pixel_24 * dptr = im24[i];
			
			for (int i=width; i; --i)
			{
				dptr++->Read(f,CLF_BGR);
			}
			d3i_fseek(f,~(width*3-1) & 3,SEEK_CUR);
		}
		
	}
	else return CLE_INVALIDDXMODE;

	if (palette_size && CLM_ATTACHEDPALETTE != imode)
	{
		if (CLM_16BIT == imode)
		{
			im16 = new CL_Pixel_16 * [height];
			*im16 = new CL_Pixel_16 [size];

			unsigned char const * sptr = *im8;
			CL_Pixel_16 * dptr = *im16;

			for (int i=size; i; --i, ++dptr,++sptr)
			{
				if (*sptr && !palette[*sptr])
					*dptr = dptr->f.dx_black;
				else
					*dptr = palette[*sptr];
			}

			CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
			CL_Pixel_16 const * aptr = *im16;
			for (i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}
		}
		else if (CLM_32BIT == imode)
		{
			im32 = new CL_Pixel_32 * [height];
			*im32 = new CL_Pixel_32 [size];

			unsigned char const * sptr = *im8;
			CL_Pixel_32 * dptr = *im32;

			for (int i=size; i; --i, ++dptr,++sptr)
			{
				if (*sptr && !palette[*sptr])
					*dptr = dptr->f.dx_black;
				else
					*dptr = palette[*sptr];
			}

			CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
			CL_Pixel_32 const * aptr = *im32;
			for (i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}
		}
		else
		{
			im24 = new CL_Pixel_24 * [height];
			*im24 = new CL_Pixel_24 [size];

			unsigned char const * sptr = *im8;
			CL_Pixel_24 * dptr = *im24;

			for (int i=size; i; --i, ++dptr,++sptr)
			{
				if (*sptr && !palette[*sptr])
					*dptr = CL_Pixel_24(0,1,0);
				else
					*dptr = palette[*sptr];
			}

			CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
			CL_Pixel_24 const * aptr = *im24;
			for (i=height; i; --i, aptr+=width)
			{
				*dptrptr++ = aptr;
			}
		}

		delete[] palette;
		palette = 0;
		palette_size = 0;
		delete[] *im8;
		delete[] im8;
		im8 = 0;
	}
	
	if (d3i_ferror(f) || d3i_feof(f))
	{
		DeleteNotMips();
		return CLE_LOADERROR;
	}
	
	flags.loaded = 1;
	return CLE_OK;
}


CL_Error CL_Image::Load_PPM(D3I_FILE * f)
{
	if (!f) return CLE_OPENERROR;
	
	DeleteNotMips();

	switch (imode)
	{
		case CLM_24BIT:
		case CLM_16BIT:
		case CLM_32BIT:
			break;
		default:
			return CLE_INVALIDDXMODE;
	}

	d3i_fseek(f,0,SEEK_SET);
	unsigned short magic;
	d3i_fread(&magic,2,1,f);
	if (magic != *(unsigned short const *)"P6") return CLE_LOADERROR;

	d3i_fseek(f,1,SEEK_CUR);
	char buf[256];
	char * bufptr = buf;
	unsigned int fields[5];
	unsigned int fields_read = 0;
	while (fields_read < 3 && bufptr)
	{
		do bufptr = d3i_fgets(buf, sizeof buf, f);
		while ('#'==buf[0] && bufptr);

		int num_fields_read = sscanf(buf,"%u %u %u",&fields[fields_read],&fields[fields_read+1],&fields[fields_read+2]);
		if (EOF != num_fields_read) fields_read += num_fields_read;
	}
	if (fields_read < 3) return CLE_LOADERROR;
	width = fields[0];
	height = fields[1];
	unsigned int maxval = fields[2];
	if (maxval > 255) return CLE_LOADERROR;

	size = width * height;

	if (CLM_16BIT == imode)
	{
		im16 = new CL_Pixel_16 * [height];
		*im16 = new CL_Pixel_16 [size];
		
		CL_Pixel_16 * rptr = *im16;
		for (int i=size; i; --i)
		{
			rptr++->Read(f,CLF_RGB,maxval);
		}

		CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
		CL_Pixel_16 const * aptr = *im16;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}
	else if (CLM_32BIT == imode)
	{
		im32 = new CL_Pixel_32 * [height];
		*im32 = new CL_Pixel_32 [size];
		
		CL_Pixel_32 * rptr = *im32;
		for (int i=size; i; --i)
		{
			rptr++->Read(f,CLF_RGB,maxval);
		}

		CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
		CL_Pixel_32 const * aptr = *im32;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}
	else
	{
		im24 = new CL_Pixel_24 * [height];
		*im24 = new CL_Pixel_24 [size];

		CL_Pixel_24 * rptr = *im24;
		for (int i=size; i; --i)
		{
			rptr++->Read(f,CLF_RGB,maxval);
		}

		CL_Pixel_24 const * * dptrptr = (CL_Pixel_24 const * *) im24;
		CL_Pixel_24 const * aptr = *im24;
		for (i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
	}

	if (d3i_ferror(f) || d3i_feof(f))
	{
		DeleteNotMips();
		return CLE_LOADERROR;
	}
	
	flags.loaded = 1;
	return CLE_OK;
}


CL_Error CL_Image::Load_PGM(D3I_FILE * f)
{
	if (!f) return CLE_OPENERROR;
	
	DeleteNotMips();

	switch (imode)
	{
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
			break;
		default:
			return CLE_INVALIDDXMODE;
	}

	d3i_fseek(f,0,SEEK_SET);
	unsigned short magic;
	d3i_fread(&magic,2,1,f);
	if (magic != *(unsigned short const *)"P5") return CLE_LOADERROR;

	d3i_fseek(f,1,SEEK_CUR);
	char buf[256];
	char * bufptr = buf;
	unsigned int fields[5];
	unsigned int fields_read = 0;
	while (fields_read < 3 && bufptr)
	{
		do bufptr = d3i_fgets(buf, sizeof buf, f);
		while ('#'==buf[0] && bufptr);

		int num_fields_read = sscanf(buf,"%u %u %u",&fields[fields_read],&fields[fields_read+1],&fields[fields_read+2]);
		if (EOF != num_fields_read) fields_read += num_fields_read;
	}
	if (fields_read < 3) return CLE_LOADERROR;
	width = fields[0];
	height = fields[1];
	unsigned int maxval = fields[2];
	if (maxval > 255) return CLE_LOADERROR;

	size = width * height;

	im8 = new unsigned char * [height];
	*im8 = new unsigned char [size];

	d3i_fread(*im8,1,size,f);

	unsigned char const * * dptrptr = (unsigned char const * *) im8;
	unsigned char const * aptr = *im8;
	for (int i=height; i; --i, aptr+=width)
	{
		*dptrptr++ = aptr;
	}

	if (d3i_ferror(f) || d3i_feof(f))
	{
		DeleteNotMips();
		return CLE_LOADERROR;
	}
	
	flags.loaded = 1;
	return CLE_OK;
}


CL_Error CL_Image::Load_PWM(D3I_FILE * f)
{
	if (!f) return CLE_OPENERROR;
	
	DeleteNotMips();

	switch (imode)
	{
		case CLM_TLTPALETTE:
			break;
		default:
			return CLE_INVALIDDXMODE;
	}

	d3i_fseek(f,0,SEEK_SET);
	unsigned short magic;
	d3i_fread(&magic,2,1,f);
	if (magic != *(unsigned short const *)"P8") return CLE_LOADERROR;

	d3i_fseek(f,1,SEEK_CUR);
	char buf[256];
	char * bufptr = buf;
	unsigned int fields[5];
	unsigned int fields_read = 0;
	while (fields_read < 3 && bufptr)
	{
		do bufptr = d3i_fgets(buf, sizeof buf, f);
		while ('#'==buf[0] && bufptr);

		int num_fields_read = sscanf(buf,"%u %u %u",&fields[fields_read],&fields[fields_read+1],&fields[fields_read+2]);
		if (EOF != num_fields_read) fields_read += num_fields_read;
	}
	if (fields_read < 3) return CLE_LOADERROR;
	width = fields[0];
	height = fields[1];
	unsigned int maxval = fields[2];
	if (maxval > 65535) return CLE_LOADERROR;

	size = width * height;

	im16raw = new unsigned short * [height];
	*im16raw = new unsigned short [size];

	d3i_fread(*im16raw,2,size,f);

	unsigned short const * * dptrptr = (unsigned short const * *) im16raw;
	unsigned short const * aptr = *im16raw;
	for (int i=height; i; --i, aptr+=width)
	{
		*dptrptr++ = aptr;
	}

	if (d3i_ferror(f) || d3i_feof(f))
	{
		DeleteNotMips();
		return CLE_LOADERROR;
	}
	
	flags.loaded = 1;
	flags.raw16bit = 1;
	return CLE_OK;
}



CL_Error CL_Image::GetBitsPerPixel(unsigned int* bpp)
{
	*bpp = bitsperpixel;
	return CLE_OK;
}

CL_Error CL_Image::Load_Image(D3I_FILE * f)
{
	if (!f) return CLE_OPENERROR;

	CL_Error retval = CLE_LOADERROR;
	
	START_TIMER

	d3i_fseek(f,0,SEEK_SET);
	unsigned short magic;
	d3i_fread(&magic,2,1,f);

	if ( *(unsigned short const *)"BM" == magic)
		retval = Load_BMP(f);
	else if ( *(unsigned short const *)"P6" == magic)
		retval = Load_PPM(f);
	else if ( *(unsigned short const *)"P5" == magic)
		retval = Load_PGM(f);
	else if ( *(unsigned short const *)"P8" == magic)
		retval = Load_PWM(f);
	#if OUTPUT_LOG
	else CL_LogFile.lputs("** ERROR: Not a recognized file format\n");
	#endif

	END_TIMER(ReadTime)
	
	#if OUTPUT_LOG
	#if TIME_LOADS
	CL_LogFile.lprintf("-- Timer Stats (ms): Open %d  Read %d  Close %d\n",OpenTime,ReadTime,CloseTime);
	#endif
	#endif

	return retval;
}


void CL_Image::PadTo(int const width_unit,int const height_unit)
{
	if (!flags.loaded) return;
	
	unsigned int const owidth = width;
	unsigned int const oheight = height;

	width += width_unit-1;
	width &= ~(width_unit-1);
	height += height_unit-1;
	height &= ~(height_unit-1);
	
	if (width == owidth && height == oheight) return; // already in spec
	
	size = width * height;

	CL_Pixel_32 * * const oim32 = im32;
	CL_Pixel_16 * * const oim16 = im16;
	unsigned char * * const oim8 = im8;

	if (oim32)
	{
		im32 = new CL_Pixel_32 * [height];
		*im32 = new CL_Pixel_32 [size];

		CL_Pixel_32 const * * dptrptr = (CL_Pixel_32 const * *) im32;
		CL_Pixel_32 const * aptr = *im32;
		for (unsigned int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
		
		for (unsigned int y = 0; y < oheight; ++y)
		{
			CL_Pixel_32 const * sptr = oim32[y];
			CL_Pixel_32 * dptr = im32[y];

			for (unsigned int x = 0; x < owidth; ++x, ++dptr, ++sptr)
				*dptr = *sptr;

			for (; x < width; ++x, ++dptr)
				*dptr = CL_Pixel_24(0,0,0);
		}

		for (; y < height; ++y)
		{
			CL_Pixel_32 * dptr = im32[y];
			for (unsigned int x = 0; x < width; ++x, ++dptr)
				*dptr = CL_Pixel_24(0,0,0);
		}

		delete[] *oim32;
		delete[] oim32;
	}

	if (oim16)
	{
		im16 = new CL_Pixel_16 * [height];
		*im16 = new CL_Pixel_16 [size];

		CL_Pixel_16 const * * dptrptr = (CL_Pixel_16 const * *) im16;
		CL_Pixel_16 const * aptr = *im16;
		for (unsigned int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
		
		for (unsigned int y = 0; y < oheight; ++y)
		{
			CL_Pixel_16 const * sptr = oim16[y];
			CL_Pixel_16 * dptr = im16[y];

			for (unsigned int x = 0; x < owidth; ++x, ++dptr, ++sptr)
				*dptr = *sptr;

			for (; x < width; ++x, ++dptr)
				*dptr = CL_Pixel_24(0,0,0);
		}

		for (; y < height; ++y)
		{
			CL_Pixel_16 * dptr = im16[y];
			for (unsigned int x = 0; x < width; ++x, ++dptr)
				*dptr = CL_Pixel_24(0,0,0);
		}

		delete[] *oim16;
		delete[] oim16;
	}

	if (oim8)
	{
		im8 = new unsigned char * [height];
		*im8 = new unsigned char [size];

		unsigned char const * * dptrptr = (unsigned char const * *) im8;
		unsigned char const * aptr = *im8;
		for (unsigned int i=height; i; --i, aptr+=width)
		{
			*dptrptr++ = aptr;
		}
		
		for (unsigned int y = 0; y < oheight; ++y)
		{
			unsigned char const * sptr = oim8[y];
			unsigned char * dptr = im8[y];

			for (unsigned int x = 0; x < owidth; ++x, ++dptr, ++sptr)
				*dptr = *sptr;

			for (; x < width; ++x, ++dptr)
				*dptr = 0;
		}

		for (; y < height; ++y)
		{
			unsigned char * dptr = im8[y];
			for (unsigned int x = 0; x < width; ++x, ++dptr)
				*dptr = 0;
		}

		delete[] *oim8;
		delete[] oim8;
	}
}

// implementation of the 'popularity algorithm' for reducing palette

unsigned int CL_Pixel_24::FVD_Distance(CL_Pixel_24 const & p2) const
{
	unsigned int dg = g > p2.g ? g - p2.g : p2.g - g;
	unsigned int db = b > p2.b ? b - p2.b : p2.b - b;

	unsigned int dgb = dg + db + ((dg > db ? dg : db)<<1);

	unsigned int dr = r > p2.r ? r - p2.r : p2.r - r;
	dr += dr<<1; // *= 3

	return dr + dgb + ((dr > dgb ? dr : dgb)<<1);
}

struct _CL_palpixcnt
{
	unsigned int cnt;
	unsigned int palpos;
};

int _CL_cmp_palpixcnt(void const * e1, void const * e2)
{
	return ((_CL_palpixcnt *)e2)->cnt - ((_CL_palpixcnt *)e1)->cnt;
}

CL_Error CL_Image::ReducePalette(unsigned int const num_colours)
{
	if (!flags.loaded) return CLE_LOADERROR;
	
	if (!palette) return CLE_INVALIDDXMODE;
	if (!im8) return CLE_INVALIDDXMODE;

	if (palette_size <= num_colours) return CLE_OK;

	_CL_palpixcnt * const sortarray = new _CL_palpixcnt[palette_size];

	for (unsigned int i=0; i<palette_size; ++i)
	{
		sortarray[i].cnt = 0;
		sortarray[i].palpos = i;
	}

	for (unsigned int y=0; y<height; ++y)
	{
		unsigned char const * sptr = im8[y];

		for (unsigned int x=width; x; --x, ++sptr)
			++ sortarray[*sptr].cnt;
	}

	unsigned int const palstart = CL_Pixel_24(0,0,0) == palette[0] ? 1 : 0;
	qsort(sortarray + palstart, palette_size - palstart, sizeof(_CL_palpixcnt), _CL_cmp_palpixcnt);

	CL_Pixel_24 * const opalette = palette;
	unsigned int const opalette_size = palette_size;
	palette = new CL_Pixel_24[num_colours];
	palette_size = num_colours;
	
	for (i=0; i<palette_size; ++i)
		palette[i] = opalette[sortarray[i].palpos];

	delete[] sortarray;
	
	// umm - if theres black thats not transparent, make sure it doesnt become colour 0 in palette
	for (unsigned int pswap=1; pswap<palette_size && 0==palstart && CL_Pixel_24(0,0,0)==palette[0]; ++pswap)
	{
		palette[0]=palette[pswap];
		palette[pswap]=CL_Pixel_24(0,0,0);
	}

	// emergency?
	if (0==palstart && CL_Pixel_24(0,0,0)==palette[0])
		palette[0] = CL_Pixel_24(1,1,1);

	unsigned int * const palmaps = new unsigned int [opalette_size];
	palmaps[0] = 0;
	for (i=palstart; i<opalette_size; ++i)
	{
		unsigned int closest = palstart;
		unsigned int distance = opalette[i].FVD_Distance(palette[palstart]);

		for (unsigned int j = palstart+1; j<palette_size; ++j)
		{
			unsigned int thisdist = opalette[i].FVD_Distance(palette[j]);
			if (thisdist < distance)
			{
				distance = thisdist;
				closest = j;
			}
		}

		palmaps[i] = closest;
	}

	delete[] opalette;

	for (y=0; y<height; ++y)
	{
		unsigned char * sptr = im8[y];

		for (unsigned int x=width; x; --x, ++sptr)
			*sptr = (unsigned char)palmaps[*sptr];
	}

	delete[] palmaps;

	return CLE_OK;	
}

// useful filename handling functions

// returns pointer into string pointing to filename without dirname
static char const * strip_path(char const * n)
{
	char const * rm = strrchr(n,':');
	if (rm) n = rm+1;
	rm = strrchr(n,'/');
	if (rm) n = rm+1;
	rm = strrchr(n,'\\');
	if (rm) n = rm+1;

	return n;
}

#if 0
static char * strip_path(char * n)
{
	char * rm = strrchr(n,':');
	if (rm) n = rm+1;
	rm = strrchr(n,'/');
	if (rm) n = rm+1;
	rm = strrchr(n,'\\');
	if (rm) n = rm+1;

	return n;
}
#endif

// removes any .extension from filename by inserting null character
static void strip_file_extension(char * n)
{
	char * dotpos = strrchr(n,'.');
	if (dotpos) *dotpos = 0;
}

static char * strip_file_extension(char const * n)
{
	char * nn = new char[strlen(n)+1];
	strcpy(nn,n);
	strip_file_extension(nn);
	return nn;
}

// CL_Image functions

CL_Error CL_Image::Locate(char const * iname, int const /* enum_id */)
{
	if (!iname) return CLE_RIFFERROR;
	
	if (name)
		delete[] name;
	name = strip_file_extension(strip_path(iname));

	if (fname)
		delete[] fname;
	fname = new char[strlen(iname)+1];
	strcpy(fname,iname);

	flags.located = 1;
	return CLE_OK;
}

CL_Error CL_Image::Load(char const * const iname, int const enum_id)
{
	if (iname || CL_EID_INVALID != enum_id || !flags.located)
	{
		CL_Error locate_err = Locate(iname,enum_id);

		if (locate_err != CLE_OK)
		{
			EXITONLOCATEFAIL(locate_err,iname,enum_id)
			return locate_err;
		}
	}

	#if OUTPUT_LOG
	char const * texformat;
	unsigned int redbits = 8;
	unsigned int greenbits = 8;
	unsigned int bluebits = 8;
	switch (imode)
	{
	case CLM_GLOBALPALETTE:
		texformat = "Palettized Display";
		break;
	case CLM_TLTPALETTE:
		texformat = "Palettized Display with Abstract TLT Palette";
		break;
	case CLM_ATTACHEDPALETTE:
		texformat = "Palettized Textures";
		break;
	case CLM_32BIT:
		texformat = "32BIT DX";
		redbits += CL_Pixel_32::f.red_bits_gt8;
		redbits -= CL_Pixel_32::f.red_bits_lt8;
		greenbits += CL_Pixel_32::f.green_bits_gt8;
		greenbits -= CL_Pixel_32::f.green_bits_lt8;
		bluebits += CL_Pixel_32::f.blue_bits_gt8;
		bluebits -= CL_Pixel_32::f.blue_bits_lt8;
		break;
	case CLM_24BIT:
		texformat = "24-bit for runtime conversion";
		break;
	case CLM_16BIT:
		texformat = "16BIT DX";
		redbits += CL_Pixel_16::f.red_bits_gt8;
		redbits -= CL_Pixel_16::f.red_bits_lt8;
		greenbits += CL_Pixel_16::f.green_bits_gt8;
		greenbits -= CL_Pixel_16::f.green_bits_lt8;
		bluebits += CL_Pixel_16::f.blue_bits_gt8;
		bluebits -= CL_Pixel_16::f.blue_bits_lt8;
		break;
	}
	CL_LogFile.lprintf("--%s %s (%u-bit, %u-%u-%u)\n",fname,texformat,bitsperpixel,redbits,greenbits,bluebits);
	#endif

	START_TIMER
	D3I_FILE * fp = d3i_fopen(fname,"rb");
	END_TIMER(OpenTime)
	
	if (fp)
	{
		CL_Error retval = Load_Image(fp);
		START_TIMER
		d3i_fclose(fp);
		END_TIMER(CloseTime)
		if (CLE_OK != retval)
		{
			#if OUTPUT_LOG
			CL_LogFile.lputs("** ERROR: unable to read\n");
			#endif
			EXITONREADFAIL(fname)
		}
		return retval;
	}

	#if OUTPUT_LOG
	CL_LogFile.lputs("** ERROR: unable to open\n");
	#endif
	EXITONLOADFAIL(fname)
	return CLE_OPENERROR;
}

CL_Error CL_Image::PreLoad(char const * const iname, int const enum_id)
{
	CL_Error locate_err = Locate(iname,enum_id);

	if (locate_err != CLE_OK)
	{
		return locate_err;
	}

	return CLE_OK;
}

CL_Error CL_Image::LoadMipMaps(int const n_mips) // assumes locating has been done (ie. PreLoad or Load has been called)
{
	if (!flags.located) return CLE_FINDERROR;

	while (mipmaps.size())
	{
		delete mipmaps.last_entry();
		mipmaps.delete_last_entry();
	}

	for (int mip_idx = 1; mip_idx < n_mips; ++mip_idx)
	{
		CL_Image * mip_n = new CL_Image;

		mip_n->name = new char[strlen(name)+1];
		mip_n->fname = new char[strlen(fname)+1];
		strcpy(mip_n->name,name);
		strcpy(mip_n->fname,fname);

		char * dotpos = strrchr(mip_n->fname,'.');
		if (!dotpos)
		{
			delete mip_n;
			break;
		}
		sprintf(dotpos+3,"%1d",mip_idx);
		
		mip_n->flags.located = 1;

		if (CLE_OK!=mip_n->Load())
		{
			delete mip_n;
			break;
		}

		if (mip_n->width << mip_idx < width || mip_n->height << mip_idx < height)
		{
			delete mip_n;
			#if OUTPUT_LOG
			CL_LogFile.lputs("** Warning: less than half size\n");
			#endif
			break;
		}

		mipmaps.add_entry_end(mip_n);
	}
	return CLE_OK;
}

void CL_Select_Mode(CL_LoadMode const lmode)
{
	switch (lmode)
	{
	case CLL_D3DTEXTURE:
		CL_Image::imode = CL_Image::imode_d3d;
		CL_Image::bitsperpixel = CL_Image::bitsperpixel_d3d;
		CL_Pixel_16::f = CL_Pixel_16::f_d3d;
		CL_Pixel_32::f = CL_Pixel_32::f_d3d;
		break;
	case CLL_DDSURFACE:
		CL_Image::imode = CL_Image::imode_ddraw;
		CL_Image::bitsperpixel = CL_Image::bitsperpixel_ddraw;
		CL_Pixel_16::f = CL_Pixel_16::f_ddraw;
		CL_Pixel_32::f = CL_Pixel_32::f_ddraw;
		break;
	}
	CL_Image::lmode = lmode;
}

// 3DC interface

void CL_Init_DirectDrawMode(CL_VideoMode const vmode)
{
	switch (vmode)
	{
		case CLV_8:
			CL_Image::bitsperpixel_ddraw = 8;
			CL_Image::imode_ddraw = CLM_GLOBALPALETTE;
			break;
		case CLV_8TLT:
			CL_Image::bitsperpixel_ddraw = 8;
			CL_Image::imode_ddraw = CLM_TLTPALETTE;
			break;
		case CLV_15:
			CL_Image::bitsperpixel_ddraw = 16;
			CL_Image::imode_ddraw = CLM_16BIT;
			CL_Pixel_16::f_ddraw.Init
			(
				DisplayPixelFormat.dwRBitMask,
				DisplayPixelFormat.dwGBitMask,
				DisplayPixelFormat.dwBBitMask
			);
			break;
		case CLV_24:
			CL_Image::bitsperpixel_ddraw = DisplayPixelFormat.dwRGBBitCount;
			if (24 == CL_Image::bitsperpixel_ddraw)
				CL_Image::imode_ddraw = CLM_24BIT;
			else
			{
				CL_Image::imode_ddraw = CLM_32BIT;
				CL_Pixel_32::f_ddraw.Init
				(
					DisplayPixelFormat.dwRBitMask,
					DisplayPixelFormat.dwGBitMask,
					DisplayPixelFormat.dwBBitMask
				);
			}
			break;
		case CLV_8T:
			CL_Image::bitsperpixel_ddraw = 8;
			CL_Image::imode_ddraw = CLM_16BIT;
			CL_Pixel_16::f_ddraw.Init(7<<5,7<<2,3);
			break;
	}
	CL_Select_Mode(CLL_DDSURFACE);
}


CL_Error CL_Image::CopyToScanDrawTexture(unsigned char * * const ImagePtrA [], unsigned int n_mips_max)
{
	if (!flags.loaded) return CLE_LOADERROR;

	if (CLL_DDSURFACE != lmode) CL_Select_Mode(CLL_DDSURFACE);

	if (n_mips_max > mipmaps.size()+1) n_mips_max = mipmaps.size()+1;

	if (!*ImagePtrA[0])
	{
		if (flags.raw16bit)
		{
			if (n_mips_max>1)
				*ImagePtrA[0] = (unsigned char *) AllocateMem((2*width+1)*(2*height+1)*2/3+(n_mips_max-3)*2); // slightly more than 4/3 w*h*bytedepth
			else
				*ImagePtrA[0] = (unsigned char *) AllocateMem(width*height*2);
		}
		else
		{
			if (n_mips_max>1)
				*ImagePtrA[0] = (unsigned char *) AllocateMem((2*width+1)*(2*height+1)*bitsperpixel/24+(n_mips_max-3)*(bitsperpixel/8)); // slightly more than 4/3 w*h*bytedepth
			else
				*ImagePtrA[0] = (unsigned char *) AllocateMem(width*height*(bitsperpixel>>3));
		}
	}
	if (!*ImagePtrA[0])
		return CLE_ALLOCFAIL;

	unsigned int my_bitsperpixel = bitsperpixel;
	
	switch (imode)
	{
		unsigned char * tptr;
		unsigned short * tSptr;
		unsigned long * tLptr;

		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
			if (flags.raw16bit)
			{
				my_bitsperpixel = 16;
				tSptr = (unsigned short *)*ImagePtrA[0];
				if (tSptr)
				{
					for (int i=0; i<height; ++i)
					{
						memcpy(tSptr,im16raw[i],width*sizeof(unsigned short));
						tSptr += width;
					}
				}
			}
			else
			{
				tptr = *ImagePtrA[0];
				if (tptr)
				{
					for (int i=0; i<height; ++i)
					{
						memcpy(tptr,im8[i],width);
						tptr += width;
					}
				}
			}
			break;
		case CLM_16BIT:
			switch (bitsperpixel)
			{
				case 8:
					tptr = *ImagePtrA[0];
					if (tptr)
					{
						for (int i=0; i<height; ++i)
						{
							unsigned char const * i8ptr = im8[i];
							for (int j=width; j; --j)
							{
								*tptr++ = *i8ptr++;
							}
						}
					}
					break;
				case 16:
					tSptr = (unsigned short *)*ImagePtrA[0];
					if (tSptr)
					{
						for (int i=0; i<height; ++i)
						{
							CL_Pixel_16 const * i16ptr = im16[i];
							for (int j=width; j; --j)
							{
								*tSptr++ = *i16ptr++;
							}
						}
					}
					break;
			}
			break;
		case CLM_24BIT:
			switch (bitsperpixel)
			{
				case 24:
					tptr = *ImagePtrA[0];
					if (tptr)
					{
						for (int i=0; i<height; ++i)
						{
							CL_Pixel_24 const * i24ptr = im24[i];
							for (int j=width; j; --j)
							{
								*tptr++ = i24ptr->b;
								*tptr++ = i24ptr->g;
								*tptr++ = i24ptr->r;
								i24ptr++;
							}
						}
					}
					break;
				case 32:
					tLptr = (unsigned long *)*ImagePtrA[0];
					if (tLptr)
					{
						for (int i=0; i<height; ++i)
						{
							CL_Pixel_32 const * i32ptr = im32[i];
							for (int j=width; j; --j)
							{
								*tLptr++ = *i32ptr++;
							}
						}
					}
					break;
			}
			break;
		default:
			return CLE_INVALIDDXMODE;
	}

	// now do mipmaps if avail

	CL_Image * last_mipP = this;
	
	LIF<CL_Image *> i_mip(&mipmaps);
	for (int i=1; i<n_mips_max && !i_mip.done(); ++i, i_mip.next())
	{
		*ImagePtrA[i] = *ImagePtrA[i-1] + last_mipP->width*last_mipP->height*(my_bitsperpixel>>3);
		CL_Error thismipmaperror = i_mip()->CopyToScanDrawTexture(&ImagePtrA[i],1);
		if (CLE_OK != thismipmaperror) return thismipmaperror;
		last_mipP = i_mip();
	}
	return CLE_OK;
}



// Direct X interface

void CL_Init_D3DMode(LPDDSURFACEDESC const format)
{
	CL_Image::format = format;

	if (format->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
	{
		#if QUANTISE_ON_LOAD
		CL_Image::imode_d3d = CLM_24BIT;
		#else
		CL_Image::imode_d3d = CLM_ATTACHEDPALETTE;
		#endif
		CL_Image::bitsperpixel_d3d = 8;
	}
	else if (format->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
	{
		#if QUANTISE_ON_LOAD
		CL_Image::imode_d3d = CLM_24BIT;
		#else
		CL_Image::imode_d3d = CLM_ATTACHEDPALETTE;
		#endif
		CL_Image::bitsperpixel_d3d = 4;
	}
	else if (format->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
	{
		#if QUANTISE_ON_LOAD
		CL_Image::imode_d3d = CLM_24BIT;
		#else
		CL_Image::imode_d3d = CLM_ATTACHEDPALETTE;
		#endif
		CL_Image::bitsperpixel_d3d = 2;
	}
	else if (format->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
	{
		#if QUANTISE_ON_LOAD
		CL_Image::imode_d3d = CLM_24BIT;
		#else
		CL_Image::imode_d3d = CLM_ATTACHEDPALETTE;
		#endif
		CL_Image::bitsperpixel_d3d = 1;
	}
	else
	{
		CL_Image::bitsperpixel_d3d = format->ddpfPixelFormat.dwRGBBitCount;
		if (format->ddpfPixelFormat.dwRGBBitCount > 16)
		{
			CL_Image::imode_d3d = CLM_32BIT;	
			CL_Pixel_32::f_d3d.Init
			(
				format->ddpfPixelFormat.dwRBitMask,
				format->ddpfPixelFormat.dwGBitMask,
				format->ddpfPixelFormat.dwBBitMask
			);
		}
		else
		{
			CL_Image::imode_d3d = CLM_16BIT;
			CL_Pixel_16::f_d3d.Init
			(
				format->ddpfPixelFormat.dwRBitMask,
				format->ddpfPixelFormat.dwGBitMask,
				format->ddpfPixelFormat.dwBBitMask
			);
		}
	}
	CL_Select_Mode(CLL_D3DTEXTURE);
}


CL_Error CL_Image::CopyToD3DTexture(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], int const MemoryType, unsigned int n_mips_max)
{
	if (!flags.loaded) return CLE_LOADERROR;

	WaitForVRamReady(VWS_D3DTEXCREATE);
	
	if (CLL_D3DTEXTURE != lmode) CL_Select_Mode(CLL_D3DTEXTURE);
	
	LPDIRECTDRAWSURFACE lpDDS;
	DDSURFACEDESC ddsd;
	HRESULT ddrval;

	// Check for image being 4 byte aligned
	// and fail if it is not
	if (width & 3 || height & 3)
	{
		// return error code
		return CLE_DXERROR;
	}


	// Set up the surface description. starting
	// with the passed texture format and then
	// incorporating the information read from the
	// ppm.
	memcpy(&ddsd, format, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT);
	ddsd.ddsCaps.dwCaps = (DDSCAPS_TEXTURE | MemoryType);
	ddsd.dwHeight = height;
	ddsd.dwWidth = width;

	if (n_mips_max > mipmaps.size()+1) n_mips_max = mipmaps.size()+1;

	if (n_mips_max>1)
	{
		ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
		ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
		ddsd.dwMipMapCount = n_mips_max;
	}

	// Create the surface
	ddrval = lpDD->CreateSurface(&ddsd, &lpDDS, NULL);

	LOGDXERR(ddrval);
	if (ddrval != DD_OK)
	{
		#if debug
		ReleaseDirect3D();
		exit(ddrval);
		#else
		lpDDS->Release();
		return CLE_DXERROR;
		#endif
	}


	// now do mipmaps if avail

	if (n_mips_max>1)
	{
		// We must now traverse the mip-map chain from highest to lowest
		// resolutions,  For each surface AFTER the first one, we must 
		// load a new file, using a name obtained from the mip map number

		int MipMapNum = 0;
		LPDIRECTDRAWSURFACE lpThisMipMap, lpNextMipMap;
		DDSCAPS ddsCaps;


		lpThisMipMap = lpDDS;
		// Component Object Model, increase reference count on 
		// mip-map surface by one.
		lpThisMipMap->AddRef(); 
		ddsCaps.dwCaps = (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP);
		ddrval = DD_OK;

		LIF<CL_Image *> i_mip(&mipmaps);

		CL_Image * last_mipP = this;

		while ((ddrval == DD_OK) && (MipMapNum < n_mips_max)) // both tests in case...
		{
			// Call LoadPPMIntoDDSurface with lpThisMipMap, new file name, and
			// other values.

			*DDSurfaceA[MipMapNum] = last_mipP->CopyToDDSurface(lpThisMipMap);

			// Death trap
			if (!*DDSurfaceA[MipMapNum])
			{
				return CLE_DXERROR;
			}
			*DDPtrA[MipMapNum] = lpThisMipMap;

			// Proceed to the next level.
			// Collect bonus rings.
			ddrval = lpThisMipMap->GetAttachedSurface(&ddsCaps, &lpNextMipMap);
			// Necessary to match the manual increment of the reference count on the
			// COM texture.  I think.
			lpThisMipMap->Release();
			// ?? lpNextMipMap = lpThisMipMap;
			lpThisMipMap = lpNextMipMap;

			MipMapNum++;
			if (!i_mip.done())
			{
				last_mipP = i_mip();
				i_mip.next();
			}
		}
	}
	else
	{
		*DDSurfaceA[0] = CopyToDDSurface(lpDDS);

		if (!*DDSurfaceA[0]) return CLE_DXERROR;
		
		*DDPtrA[0] = lpDDS;
	}
	
	return CLE_OK;
}


CL_Error CL_Image::CopyToDirectDrawSurface(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], int const MemoryType, unsigned int n_mips_max)
{
	if (!flags.loaded) return CLE_LOADERROR;
	
	WaitForVRamReady(VWS_DDCREATE);
	
	if (CLL_DDSURFACE != lmode) CL_Select_Mode(CLL_DDSURFACE);

	LPDIRECTDRAWSURFACE lpDDS;
	DDSURFACEDESC ddsd;
	HRESULT ddrval;

	// Check for image being 4 byte aligned
	// and fail if it is not
	if (width & 3 || height & 3)
	{
		// return error code
		return CLE_DXERROR;
	}

	if (n_mips_max > mipmaps.size()+1) n_mips_max = mipmaps.size()+1;

	// Set up the surface description. starting
	// with the passed texture format and then
	// incorporating the information read from the
	// ppm.
	memset(&ddsd, 0, sizeof ddsd);
	ddsd.dwSize = sizeof ddsd;
	ddsd.dwFlags = (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT);
	// Ensure that created surface has same pixel desription
	// as primary
	memcpy(&ddsd.ddpfPixelFormat, &DisplayPixelFormat, sizeof(DDPIXELFORMAT));
	ddsd.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN | MemoryType);
	ddsd.dwHeight = height;
	ddsd.dwWidth = width;

	if (n_mips_max>1)
	{
		ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
		ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
		ddsd.dwMipMapCount = n_mips_max;
	}

	// Create the surface
	ddrval = lpDD->CreateSurface(&ddsd, &lpDDS, NULL);

	LOGDXERR(ddrval);
	if (ddrval != DD_OK)
	{
		#if debug
		ReleaseDirect3D();
		exit(ddrval);
		#else
		lpDDS->Release();
		return CLE_DXERROR;
		#endif
	}

	DDCOLORKEY set_zero = {0,0};
	ddrval = lpDDS->SetColorKey(DDCKEY_SRCBLT, &set_zero);

	LOGDXERR(ddrval);
	if(ddrval != DD_OK)
	{
		#if debug
		ReleaseDirect3D();
		exit(ddrval);
		#else
		lpDDS->Release();
		return CLE_DXERROR;
		#endif
	}


	// now do mipmaps if avail

	if (n_mips_max>1)
	{
		// We must now traverse the mip-map chain from highest to lowest
		// resolutions,  For each surface AFTER the first one, we must 
		// load a new file, using a name obtained from the mip map number

		int MipMapNum = 0;
		LPDIRECTDRAWSURFACE lpThisMipMap, lpNextMipMap;
		DDSCAPS ddsCaps;


		lpThisMipMap = lpDDS;
		// Component Object Model, increase reference count on 
		// mip-map surface by one.
		lpThisMipMap->AddRef(); 
		ddsCaps.dwCaps = (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP);
		ddrval = DD_OK;

		LIF<CL_Image *> i_mip(&mipmaps);

		CL_Image * last_mipP = this;

		while ((ddrval == DD_OK) && (MipMapNum < n_mips_max)) // both tests in case...
		{
			// Call LoadPPMIntoDDSurface with lpThisMipMap, new file name, and
			// other values.

			*DDSurfaceA[MipMapNum] = last_mipP->CopyToDDSurface(lpThisMipMap);

			// Death trap
			if (!*DDSurfaceA[MipMapNum])
			{
				return CLE_DXERROR;
			}
			*DDPtrA[MipMapNum] = lpThisMipMap;

			// Proceed to the next level.
			// Collect bonus rings.
			ddrval = lpThisMipMap->GetAttachedSurface(&ddsCaps, &lpNextMipMap);
			// Necessary to match the manual increment of the reference count on the
			// COM texture.  I think.
			lpThisMipMap->Release();
			// ?? lpNextMipMap = lpThisMipMap;
			lpThisMipMap = lpNextMipMap;

			MipMapNum++;
			if (!i_mip.done())
			{
				last_mipP = i_mip();
				i_mip.next();
			}
		}
	}
	else
	{
		*DDSurfaceA[0] = CopyToDDSurface(lpDDS);

		if (!*DDSurfaceA[0]) return CLE_DXERROR;
		
		*DDPtrA[0] = lpDDS;
	}
	
	return CLE_OK;
}


LPVOID CL_Image::CopyToDDSurface(LPDIRECTDRAWSURFACE lpDDS)
{
	if (!flags.loaded) return 0;
	
	LPDIRECTDRAWPALETTE lpDDPPMPal;
	PALETTEENTRY ppe[256];
	DDSURFACEDESC ddsd;
	#if QUANTISE_ON_LOAD
	D3DCOLOR colors[256];
	D3DCOLOR c;
	int color_count;
	#endif
	int psize;
	char *lpC;
	HRESULT ddrval;
	unsigned int i, j;
	unsigned int pcaps;

	// Lock the surface so it can be filled with the PPM file

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddrval = lpDDS->Lock(NULL, &ddsd, 0, NULL);

	if (!ddsd.lpSurface)
	{
		lpDDS->Unlock(NULL);
		lpDDS->Release();
		LOGDXSTR("No surface");
		return 0;
	}

	LOGDXERR(ddrval);
	if (ddrval != DD_OK) 
	{
		#if debug
		ReleaseDirect3D();
		exit(ddrval);
		#else
		lpDDS->Release();
		return 0;
		#endif
	}

    // The method of loading depends on the pixel format of the dest surface

	switch (imode)
	{
		case CLM_32BIT:
			switch (bitsperpixel)
			{
				case 32:
					for (j = 0; j < height; j++)
					{
						// Point to next row in texture surface
						unsigned long * lpLP = (unsigned long*)((char*)ddsd.lpSurface
							+ ddsd.lPitch * j);
						CL_Pixel_32 * lpP32 = im32[j];
						for (i = width; i; --i)
						{
							*lpLP++ = *lpP32++;
						}
					}
					break;
				case 24:
					for (j = 0; j < height; j++)
					{
						// Point to next row in texture surface
						unsigned char * lpCP = (unsigned char*)ddsd.lpSurface
							+ ddsd.lPitch * j;
						CL_Pixel_32 * lpP32 = im32[j];
						for (i = width; i; --i)
						{
							unsigned long lv = *lpP32++;
							unsigned char const * lpSrcCP = (unsigned char const *)&lv;
							// dodgy - makes assumtions about 24-bit values...
							*lpCP++ = *lpSrcCP++;
							*lpCP++ = *lpSrcCP++;
							*lpCP++ = *lpSrcCP++;
						}
					}
					break;
				default:
					#if debug
					ReleaseDirect3D();
					exit(ddrval);
					#else
					lpDDS->Unlock(NULL);
					lpDDS->Release();
					return 0;
					#endif
			}
			lpDDS->Unlock(NULL);
			break;
		case CLM_16BIT:
			switch (bitsperpixel)
			{
				case 16:
					for (j = 0; j < height; j++)
					{
						// Point to next row in texture surface
						unsigned short * lpSP = (unsigned short*)((char*)ddsd.lpSurface
							+ ddsd.lPitch * j);
						CL_Pixel_16 * lpP16 = im16[j];
						for (i = width; i; --i)
						{
							*lpSP++ = *lpP16++;
						}
					}
					break;
				case 8:
					for (j = 0; j < height; j++)
					{
						// Point to next row in texture surface
						unsigned char * lpCP = (unsigned char*)ddsd.lpSurface
							+ ddsd.lPitch * j;
						CL_Pixel_16 * lpP16 = im16[j];
						for (i = width; i; --i)
						{
							*lpCP++ = *lpP16++;
						}
					}
					break;
				default:
				// Everything's gone pear-shaped...
					#if debug
					ReleaseDirect3D();
					exit(ddrval);
					#else
					lpDDS->Unlock(NULL);
					lpDDS->Release();
					return 0;
					#endif
			}
			lpDDS->Unlock(NULL);
			break;
		case CLM_24BIT:
			if (24==bitsperpixel)
			{
				for (j = 0; j < height; j++)
				{
					// Point to next row in surface
					unsigned char * lpCP = ((unsigned char*)ddsd.lpSurface) + ddsd.lPitch * j;
					CL_Pixel_24 * lpP24 = im24[j];
					for (i = 0; i < width; i++)
					{
						// making an assumption about the ordering of r,g,b on the video card
						*lpCP++ = lpP24->b;
						*lpCP++ = lpP24->g;
						*lpCP++ = lpP24->r;
						lpP24++;
					}
				}
				lpDDS->Unlock(NULL);
			}
			else
			{
		#if QUANTISE_ON_LOAD // Neal's quantize on load stuff
				
			// Paletted target texture surface
				psize = 1<<bitsperpixel;
				color_count = 0;    // number of colors in the texture
				for (j = 0; j < height; j++)
				{
					// Point to next row in surface
					lpC = ((char*)ddsd.lpSurface) + ddsd.lPitch * j;
					CL_Pixel_24 * lpP24 = im24[j];
					for (i = 0; i < width; i++, lpP24++)
					{
						int k;
						// Get the next red, green and blue values and turn them into a
						// D3DCOLOR.
						c = RGB_MAKE(lpP24->r, lpP24->g, lpP24->b);
						// Search for this color in a table of colors in this texture
						for (k = 0; k < color_count; k++)
							if (c == colors[k]) break;
						if (k == color_count)
						{
							// This is a new color, so add it to the list
							color_count++;
							// More than 256 and we fail (8-bit) 
							if (color_count > psize)
							{
								color_count--;
								k = color_count - 1;
							}
							colors[k] = c;
						}
						// Set the "pixel" value on the surface to be the index into the
						// color table
						if (psize<256)
						{
							unsigned int bitmask;
							switch (psize)
							{
								default:
									bitmask = 1;
									break;
								case 4:
									bitmask = 3;
									break;
								case 2:
									bitmask = 7;
									break;
							}
							if ((i & bitmask) == 0)
								*lpC = (char)(k & (psize-1));
							else
								*lpC |= (char)((k & (psize-1)) << (i & (psize-1)));
							if ((~i & bitmask) == 0)
								lpC++;
						} 
						else
						{
							*lpC = (char)k;
							lpC++;
						}
					}
				}

				// Close the file and unlock the surface

				lpDDS->Unlock(NULL);

				if (color_count > psize)
				{
					// If there are more than 256 colors, we overran our palette
					#if debug
					ReleaseDirect3D();
					exit(0x321123);
					#else
					lpDDS->Release();
					return 0;
					#endif
				}

				// Create a palette with the colors in our color table

				memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
				for (i = 0; i < color_count; i++)
				{
					ppe[i].peRed = (unsigned char)RGB_GETRED(colors[i]);
					ppe[i].peGreen = (unsigned char)RGB_GETGREEN(colors[i]);
					ppe[i].peBlue = (unsigned char)RGB_GETBLUE(colors[i]);
				}

				// Set all remaining entry flags to D3DPAL_RESERVED, which are ignored by
				// the renderer.

				for (; i < 256; i++)
					ppe[i].peFlags = D3DPAL_RESERVED;

				// Create the palette with the DDPCAPS_ALLOW256 flag because we want to
				// have access to all entries.

				switch (bitsperpixel)
				{
					default:
						pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
						break;
					case 4:
						pcaps = DDPCAPS_4BIT;
						break;
					case 2:
						pcaps = DDPCAPS_2BIT;
						break;
					case 1:
						pcaps = DDPCAPS_1BIT;
						break;
				}

				ddrval = lpDD->CreatePalette
					(DDPCAPS_INITIALIZE | pcaps,
						ppe, &lpDDPPMPal, NULL);
				LOGDXERR(ddrval);

				if (ddrval != DD_OK)
				{
					#if debug
					ReleaseDirect3D();
					exit(ddrval);
					#else
					lpDDS->Release();
					return 0;
					#endif
				}

				// Finally, bind the palette to the surface

				ddrval = lpDDS->SetPalette(lpDDPPMPal);
				LOGDXERR(ddrval);
				if (ddrval != DD_OK)
				{
					#if debug
					ReleaseDirect3D();
					exit(ddrval);
					#else
					lpDDS->Release();
					lpDDPPMPal->Release();
					return 0;
					#endif
				}
			    
			    lpDDPPMPal->Release();
			}
			break;
		#else
			// Everything's gone pear-shaped...
				#if debug
				ReleaseDirect3D();
				exit(ddrval);
				#else
				lpDDS->Unlock(NULL);
				lpDDS->Release();
				return 0;
				#endif
			}
			break;
		case CLM_ATTACHEDPALETTE:
		#endif
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
			// Paletted target texture surface

			psize = 1<<bitsperpixel;
			ReducePalette(psize); // will do nothing if palette is already small enough
			// ignore returned error code, because I don't care if it failed
			for (j = 0; j < height; j++)
			{
				// Point to next row in surface
				lpC = ((char*)ddsd.lpSurface) + ddsd.lPitch * j;
				unsigned char * lpP8 = im8[j];
				
				if (psize<256)
				{
					unsigned int bitmask;
					unsigned int nextpixelshift;
					switch (psize)
					{
						default:
							nextpixelshift = 2;
							break;
						case 4:
							nextpixelshift = 1;
							break;
						case 2:
							nextpixelshift = 0;
							break;
					}
					bitmask = 7 >> nextpixelshift;
					for (i = 0; i < width; i++)
					{
						unsigned char k = *lpP8++;
						// Set the "pixel" value on the surface to be the index into the
						// color table
						if ((i & bitmask) == 0)
							*lpC = (char) (k & (psize-1));
						else
							*lpC |= (char) ((k & (psize-1)) << ((i & bitmask)<<nextpixelshift));
						if ((~i & bitmask) == 0)
							lpC++;
					}
				} 
				else
				{
					memcpy(lpC,lpP8,width);
				}
			}

			// Close the file and unlock the surface

			lpDDS->Unlock(NULL);

			// Create a palette with the colors in our color table

			memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
			for (i = 0; i < palette_size; i++)
			{
				ppe[i].peRed = palette[i].r;
				ppe[i].peGreen = palette[i].g;
				ppe[i].peBlue = palette[i].b;
			}

			// Set all remaining entry flags to D3DPAL_RESERVED, which are ignored by
			// the renderer.

			for (; i < 256; i++)
				ppe[i].peFlags = D3DPAL_RESERVED;

			// Create the palette with the DDPCAPS_ALLOW256 flag because we want to
			// have access to all entries.

			switch (bitsperpixel)
			{
				default:
					pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
					break;
				case 4:
					pcaps = DDPCAPS_4BIT;
					break;
				case 2:
					pcaps = DDPCAPS_2BIT;
					break;
				case 1:
					pcaps = DDPCAPS_1BIT;
					break;
			}

			ddrval = lpDD->CreatePalette
				(DDPCAPS_INITIALIZE | pcaps,
					ppe, &lpDDPPMPal, NULL);
			LOGDXERR(ddrval);

			if (ddrval != DD_OK)
			{
				#if debug
				ReleaseDirect3D();
				exit(ddrval);
				#else
				lpDDS->Release();
				return 0;
				#endif
			}

			// Finally, bind the palette to the surface

			ddrval = lpDDS->SetPalette(lpDDPPMPal);
			LOGDXERR(ddrval);
			if (ddrval != DD_OK)
			{
				#if debug
				ReleaseDirect3D();
				exit(ddrval);
				#else
				lpDDS->Release();
				lpDDPPMPal->Release();
				return 0;
				#endif
			}
		    
		    lpDDPPMPal->Release();
			break;
		default:
		// Everything's gone pear-shaped...
			#if debug
			ReleaseDirect3D();
			exit(ddrval);
			#else
			lpDDS->Unlock(NULL);
			lpDDS->Release();
			return 0;
			#endif
					

	}

	return ddsd.lpSurface;
}


