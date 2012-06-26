#ifndef _included__d3_image_hpp_
#define _included__d3_image_hpp_

#error "This file is obsolete"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list_tem.hpp"

#include "ddraw.h"
#include "d3d.h"
#include "vramtime.h"

#include "system.h"
#include "mem3dc.h"

#include "dxlog.h"
// define = 1 if you want an output log file of all texture files loaded and for which DD surface format
#if debug
#define OUTPUT_LOG 1
#else
#define OUTPUT_LOG 0
#endif

// define = 1 to use fast files for loading (will still try normal files if fastfile doesn't contain correct data)
#define USE_FASTFILE 1

#if USE_FASTFILE

#include "ffstdio.h"
#define D3I_FILE FFILE
#define d3i_fpos_t ffpos_t
#define d3i_fclearerr ffclearerr
#define d3i_fclose ffclose
#define d3i_fcloseall ffcloseall
#define d3i_feof ffeof
#define d3i_ferror fferror
#define d3i_fgetc ffgetc
#define d3i_fgetpos ffgetpos
#define d3i_fgets ffgets
#define d3i_flook fflook
#define d3i_flookb fflookb
#define d3i_fopen ffopen
#define d3i_fread ffread
#define d3i_freadb ffreadb
#define d3i_fseek ffseek
#define d3i_fsetpos ffsetpos
#define d3i_ftell fftell

#else

#define D3I_FILE FILE
#define d3i_fpos_t fpos_t
#define d3i_fclearerr fclearerr
#define d3i_fclose fclose
#define d3i_fcloseall fcloseall
#define d3i_feof feof
#define d3i_ferror ferror
#define d3i_fgetc fgetc
#define d3i_fgetpos fgetpos
#define d3i_fgets fgets
#define d3i_fopen fopen
#define d3i_fread fread
#define d3i_fseek fseek
#define d3i_fsetpos fsetpos
#define d3i_ftell ftell

#endif


// define = 1 if you do not want to handle the error return codes, and require a load failure to cause an exit
#define EXIT_ON_LOAD_FAIL 0

// define = 1 if you are locating images through rif files and expect this to always succeed
// works like EXIT_ON_LOAD_FAIL, and only if EXIT_ON_LOAD_FAIL is set, but most failures are handled
// by assuming the given name to be a complete path and filename
#define EXIT_ON_LOCATE_FAIL 0

// define = 1 to time file opens, file closes and file reads
#define TIME_LOADS 0

// I tested the times with a typically complex set of images from AVP,
// loaded over a nework.
// Opening took 16secs, Reading 38secs and Closing 2secs for approx
// for about 500 images (including mip maps)

#if TIME_LOADS
#include <time.h>
#define START_TIMER {clock_t __stime = clock();
#define END_TIMER(__var) __var += (unsigned int) ((clock() - __stime) * 1000 / CLOCKS_PER_SEC);}
#else
#define START_TIMER
#define END_TIMER(__var)
#endif

#if TIME_LOADS
extern unsigned int OpenTime;
extern unsigned int CloseTime;
extern unsigned int ReadTime;
#endif

#if OUTPUT_LOG
#include "debuglog.hpp"
extern LogFile CL_LogFile;
#endif

// externs for 3dc things
extern "C" {
	extern LPDIRECTDRAW lpDD;
	extern DDPIXELFORMAT DisplayPixelFormat;
};

enum CL_RGBFormat // for loading from files
{
	CLF_RGB, CLF_BGR, CLF_BGRX
};

struct CL_Pixel_24
{
	union
	{
		struct
		{
			unsigned char r,g,b;
			unsigned char xx;
		};
		unsigned long l;
	};

	CL_Pixel_24() : l(0) {}
	CL_Pixel_24(unsigned char const r,unsigned char const g,unsigned char const b) : r(r),g(g),b(b),xx(0) {}
	CL_Pixel_24(CL_Pixel_24 const & p24) : l(p24.l) {}
	CL_Pixel_24(unsigned long const i) : l(i) {}

	operator unsigned long (void) const { return l; }

	inline void Read (D3I_FILE * const f, CL_RGBFormat const t)
	{
		switch(t)
		{
			case CLF_RGB:
				d3i_fread(&r,1,1,f);
				d3i_fread(&g,1,1,f);
				d3i_fread(&b,1,1,f);
				break;
			case CLF_BGR:
				d3i_fread(&b,1,1,f);
				d3i_fread(&g,1,1,f);
				d3i_fread(&r,1,1,f);
				break;
			case CLF_BGRX:
				d3i_fread(&b,1,1,f);
				d3i_fread(&g,1,1,f);
				d3i_fread(&r,1,1,f);
				d3i_fseek(f,1,SEEK_CUR);
			default:
				break;
		}
	}

	inline void Read (D3I_FILE * const f, CL_RGBFormat const t, unsigned int const maxval)
	{
		Read(f,t);
		if (maxval != 255)
		{
			r = (unsigned char) ( ((unsigned int)r << 8) / (maxval+1) );
			g = (unsigned char) ( ((unsigned int)g << 8) / (maxval+1) );
			b = (unsigned char) ( ((unsigned int)b << 8) / (maxval+1) );
		}
	}

	unsigned int FVD_Distance(CL_Pixel_24 const & p2) const;
};


struct CL_DX_Format
{
	unsigned int
		red_mask,
		red_shift,
		red_bits,
		red_bits_gt8,
		red_bits_lt8,
		blue_mask,
		blue_shift,
		blue_bits,
		blue_bits_gt8,
		blue_bits_lt8,
		green_mask,
		green_shift,
		green_bits,
		green_bits_gt8,
		green_bits_lt8;

	CL_Pixel_24 dx_black;

	inline void Init(unsigned int rmask,unsigned int gmask,unsigned int bmask)
	{
		red_mask = rmask;
		blue_mask = bmask;
		green_mask = gmask;
		for (red_shift = 0; !(rmask & 1); red_shift++, rmask>>=1);
		for (green_shift = 0; !(gmask & 1); green_shift++, gmask>>=1);
		for (blue_shift = 0; !(bmask & 1); blue_shift++, bmask>>=1);
		for (red_bits = 0; rmask; red_bits++, rmask>>=1);
		for (green_bits = 0; gmask; green_bits++, gmask>>=1);
		for (blue_bits = 0; bmask; blue_bits++, bmask>>=1);
		if (blue_bits >= green_bits && blue_bits >= red_bits)
		{
			dx_black = CL_Pixel_24(0,0,1);
		}
		else if (red_bits >= green_bits)
		{
			dx_black = CL_Pixel_24(1,0,0);
		}
		else
		{
			dx_black = CL_Pixel_24(0,1,0);
		}
		if (red_bits >= 8)
		{
			red_bits_gt8 = red_bits-8;
			red_bits_lt8 = 0;
		}
		else
		{
			red_bits_lt8 = 8-red_bits;
			red_bits_gt8 = 0;
		}
		if (green_bits >= 8)
		{
			green_bits_gt8 = green_bits-8;
			green_bits_lt8 = 0;
		}
		else
		{
			green_bits_lt8 = 8-green_bits;
			green_bits_gt8 = 0;
		}
		if (blue_bits >= 8)
		{
			blue_bits_gt8 = blue_bits-8;
			blue_bits_lt8 = 0;
		}
		else
		{
			blue_bits_lt8 = 8-blue_bits;
			blue_bits_gt8 = 0;
		}
	}
};


// for 32 bit and 24 bit DirectX formats
template <class S>
struct CL_Pixel_T
{
	static CL_DX_Format f;
	static CL_DX_Format f_d3d;
	static CL_DX_Format f_ddraw;

	S p;

	inline S r(void) const { return (S)((p & f.red_mask)>>f.red_shift); }
	inline S g(void) const { return (S)((p & f.green_mask)>>f.green_shift); }
	inline S b(void) const { return (S)((p & f.blue_mask)>>f.blue_shift); }

	CL_Pixel_T() {}
	CL_Pixel_T(CL_Pixel_T<S> const & p32) : p(p32.p) {}
	CL_Pixel_T(CL_Pixel_24 const & p24)
	: p((S)(
		(S)p24.r>>f.red_bits_lt8<<f.red_shift+f.red_bits_gt8 |
		(S)p24.g>>f.green_bits_lt8<<f.green_shift+f.green_bits_gt8 |
		(S)p24.b>>f.blue_bits_lt8<<f.blue_shift+f.blue_bits_gt8 ))
	{
		// make the 32bit pixel non-black to avoid transparency problems
		if (!p && p24.l) // if the 24bit pixel is not black and the 32 bit pixel is black
		{
			// choose which of r,g or b to make non-zero - try and get as close to black as poss
			if ((1<<f.blue_bits_lt8)-p24.b <= (1<<f.red_bits_lt8)-p24.r && (1<<f.blue_bits_lt8)-p24.b <= (1<<f.green_bits_lt8)-p24.g)
				p = (S)(1<<f.blue_shift);
			else if ((1<<f.red_bits_lt8)-p24.r <= (1<<f.green_bits_lt8)-p24.g)
				p = (S)(1<<f.red_shift);
			else
				p = (S)(1<<f.green_shift);
		}
	}

	inline operator CL_Pixel_24 (void) const
	{
		S rr = (S)(r()>>f.red_bits_gt8<<f.red_bits_lt8);
		S gg = (S)(g()>>f.green_bits_gt8<<f.green_bits_lt8);
		S bb = (S)(b()>>f.blue_bits_gt8<<f.blue_bits_lt8);

		if (!(rr|gg|bb) && p)
		{
			if (b()<<f.red_bits>=r()<<f.blue_bits && b()<<f.green_bits>=g()<<f.blue_bits) // make the 16bit pixel non-black to avoid transparency problems
				bb = 1;
			else if (r()<<f.green_bits>=g()<<f.red_bits)
				rr = 1;
			else
				gg = 1;
		}
		
		return CL_Pixel_24((unsigned char)rr,(unsigned char)gg,(unsigned char)bb);
	};
	operator S (void) const { return p; }

	inline void Read (D3I_FILE * const f, CL_RGBFormat const t)
	{
		CL_Pixel_24 p24;
		p24.Read(f,t);
		*this = (CL_Pixel_T<S>)p24;
	}
	
	inline void Read (D3I_FILE * const f, CL_RGBFormat const t, unsigned int const maxval)
	{
		CL_Pixel_24 p24;
		p24.Read(f,t,maxval);
		*this = (CL_Pixel_T<S>)p24;
	}
};


typedef CL_Pixel_T<unsigned long> CL_Pixel_32;
typedef CL_Pixel_T<unsigned short> CL_Pixel_16;


enum CL_Error
{
	CLE_OK,
	CLE_LOADERROR, // file cannot be loaded - format is wrong
	CLE_OPENERROR, // file cannot be opened - does not exist?
	CLE_FINDERROR, // file cannot be found - not listed in .RIF file
	CLE_RIFFERROR, // rif file not loaded, or invalid
	CLE_INVALIDGAMEMODE, // specified game mode does not exist
	CLE_INVALIDDXMODE, // video mode is invalid
	CLE_DXERROR, // other direct X related error
	CLE_ALLOCFAIL // failed memory allocation
};


enum CL_ImageMode {

	CLM_GLOBALPALETTE, // image shares a global palette in a palettized mode
	CLM_TLTPALETTE, // images may also share an abstract palette which remaps via a tlt to a global display palette
	CLM_ATTACHEDPALETTE, // 256 colour image with attached palette
	CLM_16BIT, // 16 bit in specified RGB format
	CLM_32BIT, // 32 bit in specified RGB format
	CLM_24BIT // 24 bit truecolour image in 888 format
};
// Note:
// currently 24-bit and 16-bit image formats are not output.
// If the desired format is 24-bit or 16-bit then 256 colour BMPs are loaded
// and 'unquantized' to generate the required format.


enum CL_LoadMode {

	CLL_D3DTEXTURE,
	CLL_DDSURFACE
};

void CL_Select_Mode(CL_LoadMode const lmode);


void CL_Init_D3DMode(LPDDSURFACEDESC const format);

enum CL_VideoMode {

	CLV_8,
	CLV_15,
	CLV_24,
	CLV_8T,
	CLV_8TLT
};

void CL_Init_DirectDrawMode(CL_VideoMode const vmode);



// test!!!
#if HwTextureHack
static int craptest = 0;
#endif

#if EXIT_ON_LOAD_FAIL
	extern "C"
	{
		#include "3dc.h"
	}
	#if EXIT_ON_LOCATE_FAIL
		#define EXITONLOCATEFAIL(__errcode,__iname,__enum_id) \
			if (CLE_RIFFERROR != __errcode) { \
				if (__iname) textprint("Cannot figure path for:\n%s\n",__iname); \
				else textprint("Cannot figure path for:\nImage ID %d\n",__enum_id); \
				WaitForReturn(); \
				ExitSystem(); \
				exit(0x10cafa11); \
			}
	#else
		#define EXITONLOCATEFAIL(__errcode,__iname,__enum_id)
	#endif
	#define EXITONLOADFAIL(__iname) \
		{ \
			textprint("Cannot open:\n%s\n",__iname); \
			WaitForReturn(); \
			ExitSystem(); \
			exit(0x10adfa11); \
		}
	#define EXITONREADFAIL(__iname) \
		{ \
			textprint("Cannot read:\n%s\n",__iname); \
			WaitForReturn(); \
			ExitSystem(); \
			exit(0x10adfa11); \
		}
#else
	#define EXITONLOCATEFAIL(__errcode,__iname,__enum_id)
	#define EXITONLOADFAIL(__iname)
	#define EXITONREADFAIL(__iname)
#endif

struct CL_Flags
{
	unsigned int loaded : 1;
	unsigned int located : 1;
	unsigned int raw16bit : 1;
	unsigned int tltpalette : 1;

	CL_Flags()
		: loaded(0)
		, located(0)
		, tltpalette(0)
		, raw16bit(0)
			{}
};

#define CL_EID_INVALID (-1)


#if 0
template <class I>
class CL_MIP_Image
{
public:
	unsigned int num_mipmaps;
	I * * mipmaps; // array of CL_Image pointers of decreasing image size

	unsigned int width; // == mipmaps[0]->width
	unsigned int height; // == mipmaps[0]->height
	unsigned int size; // == mipmaps[0]->size

	char * fname; // full filename (including directory/path) of mipmap with index 0
	char * name; // name of image without directory/path or extension

	CL_MIP_Image() : mipmaps(0), num_mipmaps(0), fname(0), name(0) {}
	~CL_MIP_Image() { Delete(); }

	CL_MIP_Image(CL_MIP_Image<I> const & i2)
		: width (i2.width)
		, height (i2.height)
		, size (i2.size)
		, num_mipmaps (i2.num_mipmaps)
		{ Copy(i2); }

	CL_MIP_Image<I> & operator = (CL_MIP_Image<I> const & i2)
	{
		if (&i2 != this)
		{
			Delete();

			width = i2.width;
			height = i2.height;
			size = i2.size;
			num_mipmaps = i2.num_mipmaps;

			Copy(i2);
		}
		return *this;
	}

private:
	void Copy(CL_MIP_Image<I> const & i2)
	{
		if (i2.mipmaps)
		{
			mipmaps = new I * [num_mipmaps];
			for (int i=0; i<num_mipmaps; ++i)
			{
				mipmaps[i] = new I(*i2.mipmaps[i]);
			}
		}

		if (i2.fname)
		{
			fname = new char[strlen(i2.fname)+1];
			strcpy(fname,i2.fname);
		}

		if (i2.name)
		{
			name = new char[strlen(i2.name)+1];
			strcpy(name,i2.name);
		}
	}
	void Delete(void)
	{
		if (mipmaps)
		{
			for (int i=0; i<num_mipmaps; ++i) delete mipmaps[i];
			delete[] mipmaps;
			mipmaps = 0;
		}

		if (name)
		{
			delete[] name;
			name = 0;
		}
		
		if (fname)
		{
			delete[] fname;
			fname = 0;
		}
	}

	CL_Flags flags;

public:
	inline CL_Error Load(int const enum_id)
	{
		return Load(0,enum_id);
	}
	inline CL_Error Load(char const * const iname)
	{
		return Load(iname,CL_EID_INVALID);
	}
	inline CL_Error Load()
	{
		return Load(0,CL_EID_INVALID);
	}

	inline CL_Error PreLoad(int const enum_id)
	{
		return PreLoad(0,enum_id);
	}
	inline CL_Error PreLoad(char const * const iname)
	{
		return PreLoad(iname,CL_EID_INVALID);
	}

	CL_Error CopyToScanDrawTexture(unsigned char * * const ImagePtrA [], unsigned int maxnummips)
	{
		if (!flags.loaded) return CLE_LOADERROR;

		if (CLL_DDSURFACE != I::lmode) CL_Select_Mode(CLL_DDSURFACE);

		if (num_mipmaps < maxnummips) maxnummips = num_mipmaps;
		if (!maxnummips) return CLE_ALLOCFAIL;

		if (!*ImagePtrA[0]) *ImagePtrA[0] = (unsigned char *) AllocateMem((2*width+1)*(2*height+1)*I::bitsperpixel/24+(maxnummips-3)*I::bitsperpixel/8); // slightly more than 4/3 w*h*bytedepth
		
		for (int i=0; i<maxnummips; ++i)
		{
			if (i) *ImagePtrA[i] = *ImagePtrA[i-1] + mipmaps[i-1]->width*mipmaps[i-1]->height*(I::bitsperpixel>>3);
			CL_Error thismipmaperror = mipmaps[i]->CopyToScanDrawTexture(ImagePtrA[i]);
			if (CLE_OK != thismipmaperror) return thismipmaperror;
		}
		return CLE_OK;
	}

	CL_Error CopyToD3DTexture(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], unsigned int maxnummips, int const MemoryType)
	{
		if (!flags.loaded) return CLE_LOADERROR;

		WaitForVRamReady(VWS_D3DTEXCREATE);

		if (CLL_D3DTEXTURE != I::lmode) CL_Select_Mode(CLL_D3DTEXTURE);

		LPDIRECTDRAWSURFACE lpDDS;
		DDSURFACEDESC ddsd;
		HRESULT ddrval;

		if (width & 3 || height & 3)
		{
			// return error code
			return CLE_DXERROR;
		}

		/* test !!! */
		{
		#if HwTextureHack
		craptest++;
		if (craptest > 10)
			return CLE_DXERROR;
		#endif
		}


		if (num_mipmaps < maxnummips) maxnummips = num_mipmaps;
		// Set up the mip-mapped surface description. starting
		// with the passed texture format and then
		// incorporating the information read from the
		// ppm.
		memcpy(&ddsd, I::format, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
			| DDSD_MIPMAPCOUNT);
		ddsd.dwMipMapCount = maxnummips; // engine standard, primary plus six mip-maps
		ddsd.ddsCaps.dwCaps = (DDSCAPS_TEXTURE | DDSCAPS_MIPMAP
			| DDSCAPS_COMPLEX | MemoryType);
		ddsd.dwHeight = height;
		ddsd.dwWidth = width;


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

		while ((ddrval == DD_OK) && (MipMapNum < maxnummips)) // both tests in case...
		{
			// Call LoadPPMIntoDDSurface with lpThisMipMap, new file name, and
			// other values.

			*DDSurfaceA[MipMapNum] = mipmaps[MipMapNum]->CopyToDDSurface(lpThisMipMap);

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
		}

		return CLE_OK;

	} 

	CL_Error CopyToDirectDrawSurface(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], unsigned int maxnummips, int const MemoryType)
	{
		if (!flags.loaded) return CLE_LOADERROR;

		WaitForVRamReady(VWS_DDCREATE);

		if (CLL_DDSURFACE != I::lmode) CL_Select_Mode(CLL_DDSURFACE);

		LPDIRECTDRAWSURFACE lpDDS;
		DDSURFACEDESC ddsd;
		HRESULT ddrval;

		if (width & 3 || height & 3)
		{
			// return error code
			return CLE_DXERROR;
		}

		if (num_mipmaps < maxnummips) maxnummips = num_mipmaps;
		// Set up the mip-mapped surface description. starting
		// with the passed texture format and then
		// incorporating the information read from the
		// ppm.
		memset(&ddsd, 0, sizeof ddsd);
		ddsd.dwSize = sizeof ddsd;
		ddsd.dwFlags = (DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
			| DDSD_MIPMAPCOUNT);
		ddsd.dwMipMapCount = maxnummips; // engine standard, primary plus six mip-maps
		ddsd.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN | DDSCAPS_MIPMAP
			| DDSCAPS_COMPLEX | MemoryType);
		ddsd.dwHeight = height;
		ddsd.dwWidth = width;


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

		while ((ddrval == DD_OK) && (MipMapNum < maxnummips)) // both tests in case...
		{
			// Call LoadPPMIntoDDSurface with lpThisMipMap, new file name, and
			// other values.

			*DDSurfaceA[MipMapNum] = mipmaps[MipMapNum]->CopyToDDSurface(lpThisMipMap);

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
		}

		return CLE_OK;

	} 

private:
	// I was using Dan's list template for this,
	// but since it is not a standard part of 3DC,
	// I have had to write a specific simple list handler
	struct TempListMember
	{
		I * mip;
		TempListMember * next;
		TempListMember(I * data) : mip(data), next(0) {}
		//TempListMember() : next(0) {}
		~TempListMember() { if (next) delete next; }
	};
	struct TempList
	{
		unsigned int n_entries;
		TempListMember * first;
		TempListMember * last;
		TempList(I * data) : first(new TempListMember(data)), n_entries(1) { last = first; }
		TempList & operator += (I * data)
		{
			if (!first)
			{
				first = new TempListMember(data);
				last = first;
			}
			else
			{
				last->next = new TempListMember(data);
				last = last->next;
			}
			++ n_entries;
			return *this;
		}
		~TempList()
		{
			if (first) delete first;
		}
	};
	
	CL_Error Load(char const * const iname, int const enum_id)
	{
		if (iname || CL_EID_INVALID != enum_id) flags.located = 0;
		
		I * mip0;
		
		if (flags.located && mipmaps)
		{
			mip0 = mipmaps[0];
			// we have grabbed the previously allocated pointer
			// it will now be treated as if it were allocated here,
			// so we must remove it from the class, in case the
			// deconstructor tries to deallocate it
			delete[] mipmaps;
			mipmaps = 0;
		}
		else
		{
			mip0 = new I;

			CL_Error locate_err = mip0->Locate(iname,enum_id);

			if (locate_err != CLE_OK)
			{
				delete mip0;
				EXITONLOCATEFAIL(locate_err,iname,enum_id)
				return locate_err;
			}
		}

		#if OUTPUT_LOG
		char const * texformat;
		unsigned int redbits = 8;
		unsigned int greenbits = 8;
		unsigned int bluebits = 8;
		switch (I::imode)
		{
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
			texformat = "Palettized Display";
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
		CL_LogFile.lprintf("--%s %s (%u-bit, %u-%u-%u)\n",mip0->fname,texformat,I::bitsperpixel,redbits,greenbits,bluebits);
		#endif

		START_TIMER
		D3I_FILE * fp = d3i_fopen(mip0->fname,"rb");
		END_TIMER(OpenTime)

		if (!fp)
		{
			#if OUTPUT_LOG
			CL_LogFile.lputs("** ERROR: unable to open\n");
			#endif
			EXITONLOADFAIL(mip0->fname)
			delete mip0;
			return CLE_OPENERROR;
		}

		CL_Error load_err = mip0->Load_Image(fp);
		START_TIMER
		d3i_fclose(fp);
		END_TIMER(CloseTime)

		if (load_err != CLE_OK)
		{
			if (!flags.located) delete mip0;
			#if OUTPUT_LOG
			CL_LogFile.lputs("** ERROR: unable to read\n");
			#endif
			EXITONREADFAIL(mip0->fname)
			return load_err;
		}

		TempList miplist(mip0);

		width = mip0->width;
		height = mip0->height;
		size = mip0->size;

		if (!flags.located)
		{
			if (fname) delete[] fname;
			if (name) delete[] name;
			fname = new char[strlen(mip0->fname)+1];
			name = new char[strlen(mip0->name)+1];
			strcpy(fname,mip0->fname);
			strcpy(name,mip0->name);
		}

		for (int mip_idx = 1; mip_idx < 7; ++mip_idx)
		{
			I * mip_n;
			
			mip_n = new I;
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

			#if OUTPUT_LOG
			char const * texformat;
			unsigned int redbits = 8;
			unsigned int greenbits = 8;
			unsigned int bluebits = 8;
			switch (I::imode)
			{
			case CLM_GLOBALPALETTE:
			case CLM_TLTPALETTE:
				texformat = "Palettized Display";
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
			CL_LogFile.lprintf("--%s %s (%u-bit, %u-%u-%u)\n",mip_n->fname,texformat,I::bitsperpixel,redbits,greenbits,bluebits);
			#endif

			START_TIMER
			fp = d3i_fopen(mip_n->fname,"rb");
			END_TIMER(OpenTime)

			if (!fp)
			{
				delete mip_n;
				#if OUTPUT_LOG
				CL_LogFile.lputs("** Warning: unable to open\n");
				#endif
				break;
			}

			load_err = mip_n->Load_Image(fp);
			START_TIMER
			d3i_fclose(fp);
			END_TIMER(CloseTime)

			if (load_err != CLE_OK)
			{
				delete mip_n;
				#if OUTPUT_LOG
				CL_LogFile.lputs("** Warning: unable to read\n");
				#endif
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

			miplist += mip_n;
		}

		if (mipmaps)
		{
			for (int i = 0; i<num_mipmaps; ++i) delete mipmaps[i];
			delete[] mipmaps;
		}

		num_mipmaps = miplist.n_entries;
		mipmaps = new I * [num_mipmaps];

		TempListMember * listP = miplist.first;
		for (int i=0; i<num_mipmaps; ++i)
		{
			mipmaps[i] = listP->mip;
			listP = listP->next;
		}

		flags.loaded = 1;
		flags.located = 1;
		return CLE_OK;
	}
	
	CL_Error PreLoad(char const * const iname, int const enum_id)
	{
		I * mip0 = new I;

		CL_Error locate_err = mip0->Locate(iname,enum_id);

		if (locate_err != CLE_OK)
		{
			delete mip0;
			return locate_err;
		}

		if (fname) delete[] fname;
		if (name) delete[] name;
		fname = new char[strlen(mip0->fname)+1];
		name = new char[strlen(mip0->name)+1];
		strcpy(fname,mip0->fname);
		strcpy(name,mip0->name);

		if (mipmaps)
		{
			for (int i=0; i<num_mipmaps; ++i) delete mipmaps[i];
			delete[] mipmaps;
		}

		num_mipmaps = 1;
		mipmaps = new I * [1];
		*mipmaps = mip0;
		
		flags.loaded = 0;
		flags.located = 1;

		return CLE_OK;
	}
};
#endif

struct CL_Image
{
public:
	static CL_ImageMode imode; // video mode defines which format images should be loaded
	static CL_ImageMode imode_d3d; // video mode defines which format images should be loaded
	static CL_ImageMode imode_ddraw; // video mode defines which format images should be loaded
	static LPDDSURFACEDESC format;
	static unsigned int bitsperpixel;
	static unsigned int bitsperpixel_d3d;
	static unsigned int bitsperpixel_ddraw;
	static CL_LoadMode lmode;

	// any one of these 3 may be valid
	union
	{ // array of 24 bit or 32 bit pixels
		CL_Pixel_24 * * im24; // array of 24 bit pixels
		CL_Pixel_32 * * im32; // array of 32 bit pixels
		// the data storage sizes of CL_Pixel_24 and CL_Pixel_32 must match
	};
	union
	{
		CL_Pixel_16 * * im16; // array of 16 bit pixels
		unsigned short * * im16raw; // raw double-byte pixel data
	};
	unsigned char * * im8; // array of entrys into CLUT
	
	unsigned int palette_size;
	CL_Pixel_24 * palette; // CLUT (applicable only if 'im8' is valid)

	unsigned int width;
	unsigned int height;
	unsigned int size; // width * height

	char * fname; // full name of image including directory/path and extension
	char * name; // name of image without directory/path or extension

	List<CL_Image *> mipmaps;

	CL_Image() : im8(0),im16(0),im24(0),palette(0),palette_size(0),fname(0),name(0) {}
	virtual ~CL_Image();

	CL_Image(CL_Image const &);
	CL_Image & operator = (CL_Image const &);

	inline CL_Error Load(char const * const iname) // looks at rif file and video mode to determine which file to load
	{
		return Load(iname,CL_EID_INVALID);
	}
	inline CL_Error Load(int const enum_id) // looks at rif file and video mode to determine which file to load
	{
		return Load(0,enum_id);
	}
	inline CL_Error Load() // assumes preload has been called
	{
		return Load(0,CL_EID_INVALID);
	}
	
	inline CL_Error PreLoad(char const * const iname) // looks at rif file and video mode to determine which file to load
	{
		return PreLoad(iname,CL_EID_INVALID);
	}
	inline CL_Error PreLoad(int const enum_id) // looks at rif file and video mode to determine which file to load
	{
		return PreLoad(0,enum_id);
	}

	CL_Error LoadMipMaps(int const n_mips = 7); // assumes locating has been done (ie. PreLoad or Load has been called)

	CL_Error Load_BMP(D3I_FILE * f); // videomode != CLM_GLOBALPALETTE
	CL_Error Load_PPM(D3I_FILE * f); // videomode == CLM_16BIT || CLM_24BIT
	CL_Error Load_PGM(D3I_FILE * f); // videomode == CLM_GLOBALPALETTE
	CL_Error Load_PWM(D3I_FILE * f); // videomode == CLM_GLOBALPALETTE

	CL_Error Load_Image(D3I_FILE * f); // calls one of the above if correct format

	CL_Error GetBitsPerPixel(unsigned int* bpp); //returns bitsperpixel;

	// needs CL_Init_D3DMode((LPDDSURFACEDESC)format) to have been called
	inline CL_Error CopyToD3DTexture(LPDIRECTDRAWSURFACE * const DDPtrA, LPVOID * const DDSurfaceA, int const MemoryType)
	{
		return CopyToD3DTexture(&DDPtrA,&DDSurfaceA,MemoryType);
	}
	CL_Error CopyToD3DTexture(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], int const MemoryType, unsigned int n_mips_max = 1);
	// needs CL_Init_ScanDrawMode((CL_VideoMode) videomode) to have been called
	inline CL_Error CopyToDirectDrawSurface(LPDIRECTDRAWSURFACE * const DDPtrA, LPVOID * const DDSurfaceA, int const MemoryType)
	{
		return CopyToDirectDrawSurface(&DDPtrA,&DDSurfaceA,MemoryType);
	}
	CL_Error CopyToDirectDrawSurface(LPDIRECTDRAWSURFACE * const DDPtrA [], LPVOID * const DDSurfaceA [], int const MemoryType, unsigned int n_mips_max = 1);
	// needs CL_Init_ScanDrawMode((CL_VideoMode) videomode) to have been called
	inline CL_Error CopyToScanDrawTexture(unsigned char * * const ImagePtrA)
	{
		return CopyToScanDrawTexture(&ImagePtrA);
	}
	CL_Error CopyToScanDrawTexture(unsigned char * * const ImagePtrA [], unsigned int n_mips_max = 1);
	// create an empty image for user manipulation
	CL_Error Make(int const _width,int const _height);
	// create an empty random image for user manipulation
	CL_Error MakeRandom(int const _width,int const _height,int const seed = -1);
	// ensure with and height are multiples of given units
	void PadTo(int const width_unit,int const height_unit);

	inline CL_Flags const & GetFlags() const
	{
		return flags;
	}
protected:
	CL_Flags flags;

	virtual CL_Error Locate(char const * iname, int const enum_id);
	CL_Error Load(char const * const iname, int const enum_id);
	CL_Error PreLoad(char const * const iname, int const enum_id);

	CL_Error ReducePalette(unsigned int const num_colours);

	// returns DDSURFACEDESC.lpSurface
	// does what LoadPPMIntoDDSurface did
	LPVOID CopyToDDSurface(LPDIRECTDRAWSURFACE lpDDS);

private:
	void Copy(CL_Image const &);
	void Delete(void);
	void DeleteNotMips(void);
};

#endif // !_included__d3_image_hpp_
