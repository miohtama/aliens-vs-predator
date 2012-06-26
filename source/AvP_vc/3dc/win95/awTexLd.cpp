#include "advwin32.h"
#ifndef DB_LEVEL
#define DB_LEVEL 4
#endif
#include "db.h"

#ifndef NDEBUG
	#define HT_FAIL db_log1
	#include "hash_tem.hpp" // for the backup surfaces memory leak checking
#endif

#ifdef _MSC_VER
	#include "iff.hpp"
#endif

#include "list_tem.hpp"

#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>

#include "awTexLd.h"
#pragma warning(disable: 4701)
#include "awTexLd.hpp"
#pragma warning(default: 4701)

#ifdef _CPPRTTI
	#include <typeinfo.h>
#endif

/* awTexLd.cpp - Author: Jake Hotson */

/*****************************************/
/* Preprocessor switches for experiments */
/*****************************************/

#define MIPMAPTEST 0 // experiment to create mip map surfaces for textures, but doesn't bother putting any data into them

/*****************************/
/* DB_LEVEL dependent macros */
/*****************************/

#if DB_LEVEL >= 5
#define inline // prevent function inlining at level 5 debugging
#endif

/*****************************************************/
/* ZEROFILL and SETDWSIZE macros ensure that I won't */
/* accidentally get the parameters wrong             */
/*****************************************************/

#if 1 // which do you prefer?

// zero mem
template <class X>
static inline void ZEROFILL(X & x)
{
	memset(&x,0,sizeof(X));
}

// set dwSize
template <class X>
static inline void SETDWSIZE(X & x)
{
	x.dwSize = sizeof(X);
}

template <class X>
static inline void INITDXSTRUCT(X & x)
{
	ZEROFILL(x);
	SETDWSIZE(x);
}

#else

#define ZEROFILL(x) (memset(&x,0,sizeof x))
#define SETDWSIZE(x) (x.dwSize = sizeof x)
#define INITDXSTRUCT(x) (ZEROFILL(x),SETDWSIZE(x))

#endif

/*****************************************************************/
/* Put everything I can in a namespace to avoid naming conflicts */
/*****************************************************************/

namespace AwTl
{
	/**************************************************/
	/* Allow breakpoints to be potentially hard coded */
	/* into macros and template functions             */
	/**************************************************/
	
	db_code5(void BrkPt(){})
	#define BREAKPOINT db_code5(::AwTl::BrkPt();)
	
	#if DB_LEVEL > 4
	static unsigned GetRefCount(IUnknown * pUnknown)
	{
		if (!pUnknown) return 0;
		pUnknown->AddRef();
		return static_cast<unsigned>(pUnknown->Release());
	}
	#endif
	
	/*********************************/
	/* Pixel format global structure */
	/*********************************/

	PixelFormat pixelFormat;
	
	PixelFormat pfTextureFormat;
	PixelFormat pfSurfaceFormat;
	
	static inline void SetBitShifts(unsigned * leftShift,unsigned * rightShift,unsigned mask)
	{
		if (!mask)
			*leftShift = 0;
		else
			for (*leftShift = 0; !(mask & 1); ++*leftShift, mask>>=1)
				;
		for (*rightShift = 8; mask; --*rightShift, mask>>=1)
			;
	}
		
	/************************************/
	/* D3D Driver info global structure */
	/************************************/

	static
	struct DriverDesc
	{
		DriverDesc() : validB(false), ddP(NULL) {}
		
		bool validB : 1;
		bool needSquareB : 1;
		bool needPow2B : 1;
		
		unsigned minWidth;
		unsigned minHeight;
		unsigned maxWidth;
		unsigned maxHeight;
		
		DWORD memFlag;
		
		DDObject * ddP;
	}
		driverDesc;

	/*************************************************************************/
	/* Class used to hold all the parameters for the CreateTexture functions */
	/*************************************************************************/
	
	class CreateTextureParms
	{
		public:
			inline CreateTextureParms()
				: fileNameS(NULL)
				, fileH(INVALID_HANDLE_VALUE)
				, dataP(NULL)
				, restoreH(NULL)
				, maxReadBytes(UINT_MAX)
				, bytesReadP(NULL)
				, flags(AW_TLF_DEFAULT)
				, originalWidthP(NULL)
				, originalHeightP(NULL)
				, widthP(NULL)
				, heightP(NULL)
				, backupHP(NULL)
				, prevTexP(static_cast<D3DTexture *>(NULL))
				, prevTexB(false)
				, loadTextureB(false)
				, callbackF(NULL)
				, rectA(NULL)
			{
			}
			
			SurfUnion DoCreate() const;
			
			bool loadTextureB;
			
			LPCTSTR fileNameS;
			HANDLE fileH;
			PtrUnionConst dataP;
			AW_BACKUPTEXTUREHANDLE restoreH;
			
			unsigned maxReadBytes;
			unsigned * bytesReadP;
			
			unsigned flags;
			
			unsigned * widthP;
			unsigned * heightP;
			
			unsigned * originalWidthP;
			unsigned * originalHeightP;
			
			AW_BACKUPTEXTUREHANDLE * backupHP;
			
			SurfUnion prevTexP;
			bool prevTexB; // used when rectA is non-NULL, otherwise prevTexP is used
			
			AW_TL_PFN_CALLBACK callbackF;
			void * callbackParam;
			
			unsigned numRects;
			AwCreateGraphicRegion * rectA;
	};
	

	/****************************************/
	/* Reference Count Object Debug Support */
	/****************************************/
	
	#ifndef NDEBUG
	
		static bool g_bAllocListActive = false;
		
		class AllocList : public ::HashTable<RefCntObj *>
		{
			public:
				AllocList()
				{
					g_bAllocListActive = true;
				}
				~AllocList()
				{
					if (Size())
					{
						db_log1(("AW: Potential Memory Leaks Detected!!!"));
					}
					#ifdef _CPPRTTI
						#pragma message("Run-Time Type Identification (RTTI) is enabled")
						for (Iterator itLeak(*this) ; !itLeak.Done() ; itLeak.Next())
						{
							db_logf1(("\tAW Object not deallocated: Type: %s RefCnt: %u",typeid(*itLeak.Get()).name(),itLeak.Get()->m_nRefCnt));
						}
						if (Size())
						{
							db_log1(("AW: Object dump complete"));
						}
					#else // ! _CPPRTTI
						#pragma message("Run-Time Type Identification (RTTI) is not enabled - memory leak checking will not report types")
						unsigned nRefs(0);
						for (Iterator itLeak(*this) ; !itLeak.Done() ; itLeak.Next())
						{
							nRefs += itLeak.Get()->m_nRefCnt;
						}
						if (Size())
						{
							db_logf1(("AW: Objects not deallocated: Number of Objects: %u Number of References: %u",Size(),nRefs));
						}
					#endif // ! _CPPRTTI
					g_bAllocListActive = false;
				}
		};
		
		static AllocList g_listAllocated;

		void DbRemember(RefCntObj * pObj)
		{
			g_listAllocated.AddAsserted(pObj);
		}
		
		void DbForget(RefCntObj * pObj)
		{
			if (g_bAllocListActive)
				g_listAllocated.RemoveAsserted(pObj);
		}
		
	#endif // ! NDEBUG
	
	/********************************************/
	/* structure to contain loading information */
	/********************************************/
	
	struct LoadInfo
	{
		DDSurface * surfaceP;
		bool surface_lockedB;
		DDSurface * dst_surfaceP;
		D3DTexture * textureP;
		D3DTexture * dst_textureP;
		
		unsigned surface_width;
		unsigned surface_height;
		PtrUnion surface_dataP;
		LONG surface_pitch;
		DWORD dwCapsCaps;
		
		unsigned * widthP;
		unsigned * heightP;
		SurfUnion prevTexP;
		SurfUnion resultP;
		unsigned top,left,bottom,right;
		unsigned width,height; // set to right-left and bottom-top
		
		AwCreateGraphicRegion * rectP;
		
		bool skipB; // used to indicate that a surface/texture was not lost and .`. does not need restoring
		
		LoadInfo()
			: surfaceP(NULL)
			, surface_lockedB(false)
			, dst_surfaceP(NULL)
			, textureP(NULL)
			, dst_textureP(NULL)
			, skipB(false)
		{
		}
	};
	
	/*******************************/
	/* additiional texture formats */
	/*******************************/
	
	struct AdditionalPixelFormat : PixelFormat
	{
		bool canDoTranspB;
		unsigned maxColours;
		
		// for List
		bool operator == (AdditionalPixelFormat const &) const { return false; }
		bool operator != (AdditionalPixelFormat const &) const { return true; }
	};
	
	static List<AdditionalPixelFormat> listTextureFormats;

} // namespace AwTl

/*******************/
/* Generic Loaders */
/*******************/
	
#define HANDLE_DXERROR(s) \
	if (DD_OK != awTlLastDxErr)	{ \
		awTlLastErr = AW_TLE_DXERROR; \
		db_logf3(("AwCreateGraphic() failed whilst %s",s)); \
		db_log1("AwCreateGraphic(): ERROR: DirectX SDK call failed"); \
		goto EXIT_WITH_ERROR; \
	} else { \
		db_logf5(("\tsuccessfully completed %s",s)); \
	}
	
#define ON_ERROR_RETURN_NULL(s) \
	if (awTlLastErr != AW_TLE_OK) { \
		db_logf3(("AwCreateGraphic() failed whilst %s",s)); \
		db_logf1(("AwCreateGraphic(): ERROR: %s",AwTlErrorToString())); \
		return static_cast<D3DTexture *>(NULL); \
	} else { \
		db_logf5(("\tsuccessfully completed %s",s)); \
	}
	
#define CHECK_MEDIA_ERRORS(s) \
	if (pMedium->m_fError) { \
		db_logf3(("AwCreateGraphic(): The following media errors occurred whilst %s",s)); \
		if (pMedium->m_fError & MediaMedium::MME_VEOFMET) { \
			db_log3("\tA virtual end of file was met"); \
			if (awTlLastErr == AW_TLE_OK) awTlLastErr = AW_TLE_EOFMET; \
		} \
		if (pMedium->m_fError & MediaMedium::MME_EOFMET) { \
			db_log3("\tAn actual end of file was met"); \
			if (awTlLastErr == AW_TLE_OK) awTlLastErr = AW_TLE_EOFMET; \
		} \
		if (pMedium->m_fError & MediaMedium::MME_OPENFAIL) { \
			db_log3("\tThe file could not be opened"); \
			if (awTlLastErr == AW_TLE_OK) { awTlLastErr = AW_TLE_CANTOPENFILE; awTlLastWinErr = GetLastError(); } \
		} \
		if (pMedium->m_fError & MediaMedium::MME_CLOSEFAIL) { \
			db_log3("\tThe file could not be closed"); \
			if (awTlLastErr == AW_TLE_OK) { awTlLastErr = AW_TLE_CANTOPENFILE; awTlLastWinErr = GetLastError(); } \
		} \
		if (pMedium->m_fError & MediaMedium::MME_UNAVAIL) { \
			db_log3("\tA requested operation was not available"); \
			if (awTlLastErr == AW_TLE_OK) { awTlLastErr = AW_TLE_CANTREADFILE; awTlLastWinErr = GetLastError(); } \
		} \
		if (pMedium->m_fError & MediaMedium::MME_IOERROR) { \
			db_log3("\tA read error occurred"); \
			if (awTlLastErr == AW_TLE_OK) { awTlLastErr = AW_TLE_CANTREADFILE; awTlLastWinErr = GetLastError(); } \
		} \
	}
	
AwTl::SurfUnion AwBackupTexture::Restore(AwTl::CreateTextureParms const & rParams)
{
	using namespace AwTl;
	
	ChoosePixelFormat(rParams);
	
	if (!pixelFormat.validB)
		db_log3("AwCreateGraphic(): ERROR: pixel format not valid");
	if (!driverDesc.ddP || !driverDesc.validB && rParams.loadTextureB)
		db_log3("AwCreateGraphic(): ERROR: driver description not valid");
	
	awTlLastErr = pixelFormat.validB && driverDesc.ddP && (driverDesc.validB || !rParams.loadTextureB) ? AW_TLE_OK : AW_TLE_NOINIT;
	
	ON_ERROR_RETURN_NULL("initializing restore")
	
	OnBeginRestoring(pixelFormat.palettizedB ? 1<<pixelFormat.bitsPerPixel : 0);
	
	ON_ERROR_RETURN_NULL("initializing restore")
	
	SurfUnion pTex = CreateTexture(rParams);
	
	OnFinishRestoring(AW_TLE_OK == awTlLastErr ? true : false);
	
	return pTex;
}

bool AwBackupTexture::HasTransparentMask(bool bDefault)
{
	return bDefault;
}

DWORD AwBackupTexture::GetTransparentColour()
{
	return 0;
}

void AwBackupTexture::ChoosePixelFormat(AwTl::CreateTextureParms const & _parmsR)
{
	using namespace AwTl;
	
	pixelFormat.validB = false; // set invalid first
	
	// which flags to use?
	unsigned fMyFlags =
		_parmsR.flags & AW_TLF_PREVSRCALL ? db_assert1(_parmsR.restoreH), m_fFlags
		: _parmsR.flags & AW_TLF_PREVSRC ? db_assert1(_parmsR.restoreH),
			_parmsR.flags & ~AW_TLF_TRANSP | m_fFlags & AW_TLF_TRANSP
		: _parmsR.flags;
		
	// transparency?
	m_bTranspMask = HasTransparentMask(fMyFlags & AW_TLF_TRANSP ? true : false);
		
	#if 0
	if (_parmsR.prevTexP.voidP)
	{
		// use the previous format
	}
	else if (_parmsR.prevTexB)
	{
		// use the previous format from one of the regiouns
	}
	else
	#endif
	
	if (_parmsR.loadTextureB || fMyFlags & AW_TLF_TEXTURE)
	{
		// use a texture format
		unsigned nColours = GetNumColours();
		unsigned nMinPalSize = GetMinPaletteSize();
		
		PixelFormat const * pFormat = &pfTextureFormat;
		
		for (LIF<AdditionalPixelFormat> itFormat(&listTextureFormats); !itFormat.done(); itFormat.next())
		{
			AdditionalPixelFormat const * pThisFormat = &itFormat();
			// is this format suitable?
			// ignoring alpha for now
			if
			(
				   (nMinPalSize <= 1U<<pThisFormat->bitsPerPixel && nMinPalSize || !pThisFormat->palettizedB) // few enough colours for palettized format
				&& (nColours <= pThisFormat->maxColours && nColours || !pThisFormat->maxColours) // pass the max colours test
				&& (pThisFormat->canDoTranspB || !m_bTranspMask) // pass the transparency test
			)
			{
				pFormat = pThisFormat;
			}
		}
		
		pixelFormat = *pFormat;
		
		#if DB_LEVEL >= 4
		if (pixelFormat.palettizedB)
		{
			db_logf4(("\tchosen %u-bit palettized texture format",pixelFormat.bitsPerPixel));
		}
		else
		{
			if (pixelFormat.alphaB)
			{
				unsigned alpha_l_shft,alpha_r_shft;
				SetBitShifts(&alpha_l_shft,&alpha_r_shft,pixelFormat.ddpf.dwRGBAlphaBitMask);
			
				db_logf4(("\tchosen %u-bit %u%u%u%u texture format",
					pixelFormat.bitsPerPixel,
					8U-pixelFormat.redRightShift,
					8U-pixelFormat.greenRightShift,
					8U-pixelFormat.blueRightShift,
					8U-alpha_r_shft));
			}
			else
			{
				db_logf4(("\tchosen %u-bit %u%u%u texture format",
					pixelFormat.bitsPerPixel,
					8U-pixelFormat.redRightShift,
					8U-pixelFormat.greenRightShift,
					8U-pixelFormat.blueRightShift));
			}
		}
		#endif
	}
	else
	{
		// use display surface format
		pixelFormat = pfSurfaceFormat;
	}
		
}

AwTl::SurfUnion AwBackupTexture::CreateTexture(AwTl::CreateTextureParms const & _parmsR)
{
	using namespace AwTl;
	
	// which flags to use?
	unsigned fMyFlags =
		_parmsR.flags & AW_TLF_PREVSRCALL ? db_assert1(_parmsR.restoreH),
			_parmsR.flags & (AW_TLF_CHECKLOST|AW_TLF_SKIPNOTLOST) | m_fFlags & ~(AW_TLF_CHECKLOST|AW_TLF_SKIPNOTLOST)
		: _parmsR.flags & AW_TLF_PREVSRC ? db_assert1(_parmsR.restoreH),
			_parmsR.flags & ~AW_TLF_TRANSP | m_fFlags & AW_TLF_TRANSP
		: _parmsR.flags;
		
	db_code1(ULONG refcnt;)
	
	DDPalette * dd_paletteP = NULL;
	LoadInfo * arrLoadInfo = NULL;
	unsigned nLoadInfos = 0;
	{
		// quick error check
		if (pixelFormat.palettizedB && (!m_nPaletteSize || 1U<<pixelFormat.bitsPerPixel < m_nPaletteSize))
			awTlLastErr = AW_TLE_CANTPALETTIZE;
		if (!m_nHeight || !m_nWidth)
			awTlLastErr = AW_TLE_BADFILEDATA;
		if (AW_TLE_OK != awTlLastErr)
		{
			db_log1("AwCreateGraphic() failed whilst interpreting the header data or palette");
			goto EXIT_WITH_ERROR;
		}
		
		if (_parmsR.originalWidthP) *_parmsR.originalWidthP = m_nWidth;
		if (_parmsR.originalHeightP) *_parmsR.originalHeightP = m_nHeight;
		
		if (_parmsR.rectA)
		{
			nLoadInfos = 0;
			arrLoadInfo = _parmsR.numRects ? new LoadInfo[_parmsR.numRects] : NULL; 
			for (unsigned i=0; i<_parmsR.numRects; ++i)
			{
				_parmsR.rectA[i].width = 0;
				_parmsR.rectA[i].height = 0;
				if
				(
					   _parmsR.rectA[i].top < m_nHeight
					&& _parmsR.rectA[i].left < m_nWidth
					&& (!_parmsR.prevTexB || (_parmsR.loadTextureB ? (_parmsR.rectA[i].pTexture != NULL) : (_parmsR.rectA[i].pSurface != NULL)))
					&& _parmsR.rectA[i].right > _parmsR.rectA[i].left
					&& _parmsR.rectA[i].bottom > _parmsR.rectA[i].top
				)
				{
					// rectangle covers at least some of the image and non-null previous texture
					arrLoadInfo[nLoadInfos].widthP = &_parmsR.rectA[i].width;
					arrLoadInfo[nLoadInfos].heightP = &_parmsR.rectA[i].height;
					if (_parmsR.prevTexB)
					{
						if (_parmsR.loadTextureB)
							arrLoadInfo[nLoadInfos].prevTexP = _parmsR.rectA[i].pTexture;
						else
							arrLoadInfo[nLoadInfos].prevTexP = _parmsR.rectA[i].pSurface;
					}
					else
					{
						arrLoadInfo[nLoadInfos].prevTexP = static_cast<D3DTexture *>(NULL);
						if (_parmsR.loadTextureB)
							_parmsR.rectA[i].pTexture = NULL;
						else
							_parmsR.rectA[i].pSurface = NULL;
					}
					
					arrLoadInfo[nLoadInfos].rectP = &_parmsR.rectA[i];
					arrLoadInfo[nLoadInfos].top = _parmsR.rectA[i].top;
					arrLoadInfo[nLoadInfos].left = _parmsR.rectA[i].left;
					arrLoadInfo[nLoadInfos].bottom = _parmsR.rectA[i].bottom;
					arrLoadInfo[nLoadInfos].right = _parmsR.rectA[i].right;
					
					if (arrLoadInfo[nLoadInfos].right > m_nWidth) arrLoadInfo[nLoadInfos].right = m_nWidth;
					if (arrLoadInfo[nLoadInfos].bottom > m_nHeight) arrLoadInfo[nLoadInfos].bottom = m_nHeight;
					
					arrLoadInfo[nLoadInfos].width = arrLoadInfo[nLoadInfos].right - arrLoadInfo[nLoadInfos].left;
					arrLoadInfo[nLoadInfos].height = arrLoadInfo[nLoadInfos].bottom - arrLoadInfo[nLoadInfos].top;

					++nLoadInfos;
				}
				else
				{
					if (!_parmsR.prevTexB)
						_parmsR.rectA[i].pTexture = NULL;
				}
			}
		}
		else
		{
			nLoadInfos = 1;
			arrLoadInfo = new LoadInfo[1];
			arrLoadInfo[0].widthP = _parmsR.widthP;
			arrLoadInfo[0].heightP = _parmsR.heightP;
			arrLoadInfo[0].prevTexP = _parmsR.prevTexP;
			arrLoadInfo[0].rectP = NULL;
			arrLoadInfo[0].top = 0;
			arrLoadInfo[0].left = 0;
			arrLoadInfo[0].bottom = m_nHeight;
			arrLoadInfo[0].right = m_nWidth;
			arrLoadInfo[0].width = m_nWidth;
			arrLoadInfo[0].height = m_nHeight;
		}
		
		bool bSkipAll = true;
		
		// loop creating surfaces
		{for (unsigned i=0; i<nLoadInfos; ++i)
		{
			LoadInfo * pLoadInfo = &arrLoadInfo[i];
			
			db_logf4(("\trectangle from image (%u,%u)-(%u,%u)",pLoadInfo->left,pLoadInfo->top,pLoadInfo->right,pLoadInfo->bottom));
			db_logf5(("\treference count on input surface %u",_parmsR.loadTextureB ? GetRefCount(pLoadInfo->prevTexP.textureP) : GetRefCount(pLoadInfo->prevTexP.surfaceP)));
			
			// determine what the width and height of the surface will be
			
			if (_parmsR.loadTextureB || fMyFlags & AW_TLF_TEXTURE)
			{
				awTlLastErr =
					AwGetTextureSize
					(
						&pLoadInfo->surface_width,
						&pLoadInfo->surface_height,
						fMyFlags & AW_TLF_MINSIZE && pLoadInfo->rectP ? pLoadInfo->rectP->right - pLoadInfo->rectP->left : pLoadInfo->width,
						fMyFlags & AW_TLF_MINSIZE && pLoadInfo->rectP ? pLoadInfo->rectP->bottom - pLoadInfo->rectP->top : pLoadInfo->height
					);
				if (awTlLastErr != AW_TLE_OK)
					goto EXIT_WITH_ERROR;
			}
			else
			{
				pLoadInfo->surface_width = fMyFlags & AW_TLF_MINSIZE && pLoadInfo->rectP ? pLoadInfo->rectP->right - pLoadInfo->rectP->left : pLoadInfo->width;
				pLoadInfo->surface_height = fMyFlags & AW_TLF_MINSIZE && pLoadInfo->rectP ? pLoadInfo->rectP->bottom - pLoadInfo->rectP->top : pLoadInfo->height;
				#if 1 // not sure if this is required...
				pLoadInfo->surface_width += 3;
				pLoadInfo->surface_width &= ~3;
				pLoadInfo->surface_height += 3;
				pLoadInfo->surface_height &= ~3;
				#endif
			}
		
			if (pLoadInfo->widthP) *pLoadInfo->widthP = pLoadInfo->surface_width;
			if (pLoadInfo->heightP) *pLoadInfo->heightP = pLoadInfo->surface_height;

			// Create DD Surface	
		
			DD_SURFACE_DESC ddsd;
			INITDXSTRUCT(ddsd);
			ddsd.ddpfPixelFormat = pixelFormat.ddpf;
			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
			if (_parmsR.loadTextureB || fMyFlags & AW_TLF_TEXTURE)
				pLoadInfo->dwCapsCaps = DDSCAPS_TEXTURE | (fMyFlags & (AW_TLF_COMPRESS|AW_TLF_TEXTURE) ? DDSCAPS_SYSTEMMEMORY : driverDesc.memFlag);
			else
				pLoadInfo->dwCapsCaps = DDSCAPS_OFFSCREENPLAIN | (fMyFlags & AW_TLF_VIDMEM ? DDSCAPS_VIDEOMEMORY : DDSCAPS_SYSTEMMEMORY);
			ddsd.ddsCaps.dwCaps = pLoadInfo->dwCapsCaps;
			ddsd.dwHeight = pLoadInfo->surface_height;
			ddsd.dwWidth = pLoadInfo->surface_width;
		
			#if MIPMAPTEST
			/*
			D3DPTEXTURECAPS_POW2 
			All nonmipmapped textures must have widths and heights specified as powers of two if this flag is set.
			(Note that all mipmapped textures must always have dimensions that are powers of two.)
			*/
			if (128==pLoadInfo->surface_width && 128==pLoadInfo->surface_height)
			{
				ddsd.ddsCaps.dwCaps |= DDSCAPS_MIPMAP | DDSCAPS_COMPLEX;
				ddsd.dwFlags |= DDSD_MIPMAPCOUNT;
				ddsd.dwMipMapCount = 3;
			}
			#endif
		
			if (pLoadInfo->prevTexP.voidP && (!_parmsR.loadTextureB || !(fMyFlags & AW_TLF_COMPRESS)))
			{
				if (_parmsR.loadTextureB)
					awTlLastDxErr = pLoadInfo->prevTexP.textureP->QueryInterface(GUID_DD_SURFACE,(LPVOID *)&pLoadInfo->surfaceP);
				else
					awTlLastDxErr = pLoadInfo->prevTexP.surfaceP->QueryInterface(GUID_DD_SURFACE,(LPVOID *)&pLoadInfo->surfaceP);
				HANDLE_DXERROR("getting direct draw surface interface")
				#if DB_LEVEL >= 5
				if (_parmsR.loadTextureB)
					db_logf5(("\t\tnow prev tex ref %u new surface i/f ref %u",GetRefCount(pLoadInfo->prevTexP.textureP),GetRefCount(pLoadInfo->surfaceP)));
				else
					db_logf5(("\t\tnow prev surf ref %u new surface i/f ref %u",GetRefCount(pLoadInfo->prevTexP.surfaceP),GetRefCount(pLoadInfo->surfaceP)));
				#endif
				
				// check for lost surfaces
				if (fMyFlags & AW_TLF_CHECKLOST)
				{
					awTlLastDxErr = pLoadInfo->surfaceP->IsLost();
					
					if (DDERR_SURFACELOST == awTlLastDxErr)
					{
						db_log4("\tRestoring Lost Surface");
						
						awTlLastDxErr = pLoadInfo->surfaceP->Restore();
					}
					else if (DD_OK == awTlLastDxErr && (fMyFlags & AW_TLF_SKIPNOTLOST))
					{
						db_log4("\tSkipping Surface which was not Lost");
						
						pLoadInfo->skipB = true;
					}
					
					HANDLE_DXERROR("testing for lost surface and restoring if necessary");
				}
				
				if (!pLoadInfo->skipB)
				{
					// check that the surface desc is OK
					// note that SetSurfaceDesc is *only* supported for changing the surface memory pointer
					DD_SURFACE_DESC old_ddsd;
					INITDXSTRUCT(old_ddsd);
					awTlLastDxErr = pLoadInfo->surfaceP->GetSurfaceDesc(&old_ddsd);
					HANDLE_DXERROR("getting previous surface desc")
					// check width, height, RGBBitCount and memory type
					if (old_ddsd.dwFlags & DDSD_ALL || (old_ddsd.dwFlags & (DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT)) == (DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT))
					{
						if (old_ddsd.dwHeight == pLoadInfo->surface_height && old_ddsd.dwWidth == pLoadInfo->surface_width && (old_ddsd.ddsCaps.dwCaps & (DDSCAPS_SYSTEMMEMORY|DDSCAPS_VIDEOMEMORY|DDSCAPS_TEXTURE)) == pLoadInfo->dwCapsCaps)
						{
							unsigned bpp = 0;
							if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
								bpp = 8;
							else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
								bpp = 4;
							else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
								bpp = 2;
							else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
								bpp = 1;
							else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB)
								bpp = old_ddsd.ddpfPixelFormat.dwRGBBitCount;
							if (pixelFormat.bitsPerPixel != bpp)
								awTlLastErr = AW_TLE_CANTRELOAD;	
						}
						else
							awTlLastErr = AW_TLE_CANTRELOAD;
					}
					else
						awTlLastErr = AW_TLE_CANTRELOAD;
					if (AW_TLE_OK != awTlLastErr)
					{
						db_log1("AwCreateGraphic() failed because existing surface is incompatible");
						goto EXIT_WITH_ERROR;
					}
				}
				else
				{
					pLoadInfo->surfaceP->Release();
					pLoadInfo->surfaceP = NULL;
				}
			}
			else
			{
				if (pLoadInfo->prevTexP.voidP && (fMyFlags & AW_TLF_CHECKLOST))
				{
					db_assert1(_parmsR.loadTextureB);
					
					awTlLastDxErr = pLoadInfo->prevTexP.textureP->QueryInterface(GUID_DD_SURFACE,(LPVOID *)&pLoadInfo->surfaceP);
					HANDLE_DXERROR("getting direct draw surface interface")
					
					db_logf5(("\t\tnow prev tex ref %u new surface i/f ref %u",GetRefCount(pLoadInfo->prevTexP.textureP),GetRefCount(pLoadInfo->surfaceP)));
					
					awTlLastDxErr = pLoadInfo->surfaceP->IsLost();
					
					if (DDERR_SURFACELOST == awTlLastDxErr)
					{
						db_log4("\tRestoring Lost Surface");
						
						awTlLastDxErr = pLoadInfo->surfaceP->Restore();
					}
					else if (DD_OK == awTlLastDxErr && (fMyFlags & AW_TLF_SKIPNOTLOST))
					{
						db_log4("\tSkipping Surface which was not Lost");
						
						pLoadInfo->skipB = true;
					}
					
					HANDLE_DXERROR("testing for lost surface and restoring if necessary");
					
					pLoadInfo->surfaceP->Release();
					pLoadInfo->surfaceP = NULL;
				}
				
				if (!pLoadInfo->skipB)
				{
					do
					{
						awTlLastDxErr = driverDesc.ddP->CreateSurface(&ddsd,&pLoadInfo->surfaceP,NULL);
					}
						while
						(
							DDERR_OUTOFVIDEOMEMORY == awTlLastDxErr
							&& _parmsR.callbackF
							&& _parmsR.callbackF(_parmsR.callbackParam)
						);
					
					HANDLE_DXERROR("creating direct draw surface")
				}
			}
			
			if (pLoadInfo->skipB)
			{
				db_assert1(pLoadInfo->prevTexP.voidP);
				
				// skipping so result is same as input
				pLoadInfo->resultP = pLoadInfo->prevTexP;
				
				if (_parmsR.loadTextureB)
					pLoadInfo->prevTexP.textureP->AddRef();
				else
					pLoadInfo->prevTexP.surfaceP->AddRef();
			}
		
			#if MIPMAPTEST
			if (128==surface_width && 128==surface_height)
			{
				// test if we can get attached surfaces...
				DDSCAPS ddscaps;
				ZEROFILL(ddscaps);
				ddscaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
				DDSurface * mip2P;
				awTlLastDxErr = pLoadInfo->surfaceP->GetAttachedSurface(&ddscaps,&mip2P);
				HANDLE_DXERROR("getting first mipmap")
				DDSurface * mip3P;
				awTlLastDxErr = mip2P->GetAttachedSurface(&ddscaps,&mip3P);
				HANDLE_DXERROR("getting second mipmap")
				db_logf5(("\tabout to release 2nd mip with ref %u",GetRefCount(mip2P)));
				db_code1(refcnt =)
				mip2P->Release();
				db_onlyassert1(1==refcnt);
				db_logf5(("\tabout to release 3nd mip with ref %u",GetRefCount(mip3P)));
				db_code1(refcnt =)
				mip3P->Release();
				db_onlyassert1(1==refcnt);
			}
			#endif
			
			bSkipAll = bSkipAll && pLoadInfo->skipB;
		}}
		
		if (!bSkipAll)
		{
			Colour * paletteP = m_nPaletteSize ? GetPalette() : NULL;
			
			unsigned y = 0;
			bool reversed_rowsB = AreRowsReversed();
			if (reversed_rowsB)
			{
				y = m_nHeight-1;
			}
			
			for (unsigned rowcount = m_nHeight; rowcount; --rowcount)
			{
				PtrUnion src_rowP = GetRowPtr(y);
				db_assert1(src_rowP.voidP);
				
				// allow loading of the next row from the file
				LoadNextRow(src_rowP);
				
				// loop for copying data to surfaces
				for (unsigned i=0; i<nLoadInfos; ++i)
				{
					LoadInfo * pLoadInfo = &arrLoadInfo[i];
					
					if (!pLoadInfo->skipB)
					{
						// are we in the vertical range of this surface?
						if (y>=pLoadInfo->top && y<pLoadInfo->bottom)
						{
							if (!pLoadInfo->surface_lockedB)
							{
								// lock the surfaces
								DD_SURFACE_DESC ddsd;
								INITDXSTRUCT(ddsd);
								awTlLastDxErr = pLoadInfo->surfaceP->Lock(NULL,&ddsd,DDLOCK_WRITEONLY|DDLOCK_NOSYSLOCK,NULL);
								HANDLE_DXERROR("locking direct draw surface")
								pLoadInfo->surface_lockedB = true;
								pLoadInfo->surface_dataP.voidP = ddsd.lpSurface;
								pLoadInfo->surface_dataP.byteP += ddsd.lPitch * (y-pLoadInfo->top);
								pLoadInfo->surface_pitch = ddsd.lPitch;
							}
							
							// convert and copy the section of the row to the direct draw surface
							ConvertRow(pLoadInfo->surface_dataP,pLoadInfo->surface_width,src_rowP,pLoadInfo->left,pLoadInfo->width,paletteP db_code1(DB_COMMA m_nPaletteSize));
							
							// do the bottom row twice if the dd surface is bigger
							if (pLoadInfo->bottom-1 == y && pLoadInfo->surface_height > pLoadInfo->height)
							{
								PtrUnion next_surface_rowP = pLoadInfo->surface_dataP;
								next_surface_rowP.byteP += pLoadInfo->surface_pitch;
								ConvertRow(next_surface_rowP,pLoadInfo->surface_width,src_rowP,pLoadInfo->left,pLoadInfo->width,paletteP db_code1(DB_COMMA m_nPaletteSize));
							}
							
							// next ddsurface row
							if (reversed_rowsB)
								pLoadInfo->surface_dataP.byteP -= pLoadInfo->surface_pitch;
							else
								pLoadInfo->surface_dataP.byteP += pLoadInfo->surface_pitch;
						}
						else if (pLoadInfo->surface_lockedB)
						{
							// unlock the surface
							awTlLastDxErr = pLoadInfo->surfaceP->Unlock(NULL);
							HANDLE_DXERROR("unlocking direct draw surface")
							pLoadInfo->surface_lockedB = false;
						}
					}
				}
				
				// next row
				if (reversed_rowsB)
					--y;
				else
					++y;
				
				if (AW_TLE_OK != awTlLastErr)
				{
					db_log1("AwCreateGraphic() failed whilst copying data to direct draw surface");
					goto EXIT_WITH_ERROR;
				}
			}
			
			// create a palette for the surfaces if there is one
			DWORD palcreateflags = 0;
			PALETTEENTRY colour_tableA[256];
			if (pixelFormat.palettizedB)
			{
				if (!_parmsR.loadTextureB && !(fMyFlags & AW_TLF_TEXTURE))
				{
					db_log3("AwCreateGraphic(): WARNING: setting a palette on a DD surface may have no effect");
				}
				
				#if 0
				if (m_bTranspMask)
				{
					colour_tableA[0].peRed = 0;
					colour_tableA[0].peGreen = 0;
					colour_tableA[0].peBlue = 0;
					colour_tableA[0].peFlags = 0;
					for (unsigned i=1; i<m_nPaletteSize; ++i)
					{
						colour_tableA[i].peRed = paletteP[i].r;
						colour_tableA[i].peGreen = paletteP[i].g;
						colour_tableA[i].peBlue = paletteP[i].b;
						if (!(paletteP[i].r + paletteP[i].g + paletteP[i].b))
							colour_tableA[i].peRed = 1;
						colour_tableA[i].peFlags = 0;
					}
				}
				else
				#endif
				{
					for (unsigned i=0; i<m_nPaletteSize; ++i)
					{
						colour_tableA[i].peRed = paletteP[i].r;
						colour_tableA[i].peGreen = paletteP[i].g;
						colour_tableA[i].peBlue = paletteP[i].b;
						colour_tableA[i].peFlags = 0;
					}
				}
				for (unsigned i=m_nPaletteSize; i<256; ++i)
					colour_tableA[i].peFlags = 0;
				switch (pixelFormat.bitsPerPixel)
				{
					default:
						CANT_HAPPEN
					case 8:
						palcreateflags = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
						break;
					case 4:
						palcreateflags = DDPCAPS_4BIT;
						break;
					case 2:
						palcreateflags = DDPCAPS_2BIT;
						break;
					case 1:
						palcreateflags = DDPCAPS_1BIT;
						break;
				}
				awTlLastDxErr = driverDesc.ddP->CreatePalette(palcreateflags,colour_tableA,&dd_paletteP,NULL);
				HANDLE_DXERROR("creating palette for direct draw surface")
			}
				
			{for (unsigned i=0; i<nLoadInfos; ++i)
			{
				LoadInfo * pLoadInfo = &arrLoadInfo[i];
			
				if (!pLoadInfo->skipB)
				{
					// unlock the surface
					if (pLoadInfo->surface_lockedB)
					{
						awTlLastDxErr = pLoadInfo->surfaceP->Unlock(NULL);
						HANDLE_DXERROR("unlocking direct draw surface")
						pLoadInfo->surface_lockedB = false;
					}
					
					if (pixelFormat.palettizedB)
					{
						// set the palette on the surface
						awTlLastDxErr = pLoadInfo->surfaceP->SetPalette(dd_paletteP);
						HANDLE_DXERROR("setting palette on direct draw surface")
					}
				}
				
			}}
			
			if (pixelFormat.palettizedB)
			{
				db_logf5(("\tabout to release palette with ref %u",GetRefCount(dd_paletteP)));
				dd_paletteP->Release();
				dd_paletteP = NULL;
			}
			
			DWORD dwColourKey;
			DDCOLORKEY invis;
			// get colour for chroma keying if required
			if (m_bTranspMask && (!pixelFormat.alphaB || fMyFlags & AW_TLF_CHROMAKEY))
			{
				dwColourKey = GetTransparentColour();
				invis.dwColorSpaceLowValue = dwColourKey;
				invis.dwColorSpaceHighValue = dwColourKey;
			}
			
			{for (unsigned i=0; i<nLoadInfos; ++i)
			{
				LoadInfo * pLoadInfo = &arrLoadInfo[i];
				
				if (!pLoadInfo->skipB)
				{
					// do the copying crap and Texture::Load() stuff - see CopyD3DTexture in d3_func.cpp
					
					if (_parmsR.loadTextureB)
					{
						// get a texture pointer
						awTlLastDxErr = pLoadInfo->surfaceP->QueryInterface(GUID_D3D_TEXTURE,(LPVOID *)&pLoadInfo->textureP);
						HANDLE_DXERROR("getting texture interface on direct draw surface")
						db_logf5(("\t\tnow surface ref %u texture ref %u",GetRefCount(pLoadInfo->surfaceP),GetRefCount(pLoadInfo->textureP)));
						
						if (fMyFlags & AW_TLF_COMPRESS) // deal with Texture::Load and ALLOCONLOAD flag
						{
							if (pLoadInfo->prevTexP.voidP)
							{
								// load into the existing texture
								awTlLastDxErr = pLoadInfo->prevTexP.textureP->QueryInterface(GUID_DD_SURFACE,(LPVOID *)&pLoadInfo->dst_surfaceP);
								HANDLE_DXERROR("getting direct draw surface interface")
								db_logf5(("\t\tnow prev texture ref %u dst surface ref %u",GetRefCount(pLoadInfo->prevTexP.textureP),GetRefCount(pLoadInfo->dst_surfaceP)));
								// check that the surface desc is OK
								// note that SetSurfaceDesc is *only* supported for changing the surface memory pointer
								DD_SURFACE_DESC old_ddsd;
								INITDXSTRUCT(old_ddsd);
								awTlLastDxErr = pLoadInfo->surfaceP->GetSurfaceDesc(&old_ddsd);
								HANDLE_DXERROR("getting previous surface desc")
								// check width, height, RGBBitCount and memory type
								if (old_ddsd.dwFlags & DDSD_ALL || (old_ddsd.dwFlags & (DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT)) == (DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT))
								{
									if (old_ddsd.dwHeight == pLoadInfo->surface_height && old_ddsd.dwWidth == pLoadInfo->surface_width && (old_ddsd.ddsCaps.dwCaps & (DDSCAPS_SYSTEMMEMORY|DDSCAPS_VIDEOMEMORY|DDSCAPS_TEXTURE)) == pLoadInfo->dwCapsCaps)
									{
										unsigned bpp = 0;
										if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
											bpp = 8;
										else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
											bpp = 4;
										else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2)
											bpp = 2;
										else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1)
											bpp = 1;
										else if (old_ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB)
											bpp = old_ddsd.ddpfPixelFormat.dwRGBBitCount;
										if (pixelFormat.bitsPerPixel != bpp)
											awTlLastErr = AW_TLE_CANTRELOAD;	
									}
									else
										awTlLastErr = AW_TLE_CANTRELOAD;
								}
								else
									awTlLastErr = AW_TLE_CANTRELOAD;
								if (AW_TLE_OK != awTlLastErr)
								{
									db_log1("AwCreateGraphic() failed because existing surface is incompatible");
									goto EXIT_WITH_ERROR;
								}
							}
							else
							{
								DD_SURFACE_DESC ddsd;
								
								INITDXSTRUCT(ddsd);
								
								awTlLastDxErr = pLoadInfo->surfaceP->GetSurfaceDesc(&ddsd);
								HANDLE_DXERROR("getting direct draw surface desc")

								ddsd.ddsCaps.dwCaps &= ~DDSCAPS_SYSTEMMEMORY;
								ddsd.ddsCaps.dwCaps |= DDSCAPS_ALLOCONLOAD | driverDesc.memFlag;
								do
								{
									awTlLastDxErr = driverDesc.ddP->CreateSurface(&ddsd,&pLoadInfo->dst_surfaceP,NULL);
								}
									while
									(
										DDERR_OUTOFVIDEOMEMORY == awTlLastDxErr
										&& _parmsR.callbackF
										&& _parmsR.callbackF(_parmsR.callbackParam)
									);
								HANDLE_DXERROR("creating destination direct draw surface")
							}
							
							// create a zero palette if required -> Texture::Load() will copy in correct palette
							if (pixelFormat.palettizedB)
							{
								memset(colour_tableA,0,sizeof colour_tableA);
								
								awTlLastDxErr = driverDesc.ddP->CreatePalette(palcreateflags,colour_tableA,&dd_paletteP,NULL);
								HANDLE_DXERROR("creating palette for destination direct draw surface")
								awTlLastDxErr = pLoadInfo->dst_surfaceP->SetPalette(dd_paletteP);
								HANDLE_DXERROR("setting palette on destination direct draw surface")
								db_logf5(("\tabout to release dest palette with ref %u",GetRefCount(dd_paletteP)));
								dd_paletteP->Release();
								dd_paletteP = NULL;
							}
							
							// get a texture pointer on the destination
							awTlLastDxErr = pLoadInfo->dst_surfaceP->QueryInterface(GUID_D3D_TEXTURE,(LPVOID *)&pLoadInfo->dst_textureP);
							HANDLE_DXERROR("getting texture interface on destination direct draw surface")
							db_logf5(("\t\tnow dst surface ref %u dst texture ref %u",GetRefCount(pLoadInfo->dst_surfaceP),GetRefCount(pLoadInfo->dst_textureP)));
							
							do
							{
								awTlLastDxErr = pLoadInfo->dst_textureP->Load(pLoadInfo->textureP);
							}
								while
								(
									DDERR_OUTOFVIDEOMEMORY == awTlLastDxErr
									&& _parmsR.callbackF
									&& _parmsR.callbackF(_parmsR.callbackParam)
								);
							HANDLE_DXERROR("loading texture into destination")
							
							// release src texture and surface, and set pointers to point to dst texture and surface
							db_logf5(("\tabout to release internal surface with ref %u",GetRefCount(pLoadInfo->surfaceP)));
							db_code1(refcnt =)
							pLoadInfo->surfaceP->Release();
							db_onlyassert1(1==refcnt);
							pLoadInfo->surfaceP = pLoadInfo->dst_surfaceP;
							pLoadInfo->dst_surfaceP = NULL;
							
							db_logf5(("\tabout to release internal texture i/f with ref %u",GetRefCount(pLoadInfo->textureP)));
							db_code1(refcnt =)
							pLoadInfo->textureP->Release();
							db_onlyassert1(!refcnt);
							pLoadInfo->textureP = pLoadInfo->dst_textureP;
							pLoadInfo->dst_textureP = NULL;
						}
					}
						
					// set chroma keying if required
					if (m_bTranspMask && (!pixelFormat.alphaB || fMyFlags & AW_TLF_CHROMAKEY))
					{
						awTlLastDxErr = pLoadInfo->surfaceP->SetColorKey(DDCKEY_SRCBLT,&invis);
						HANDLE_DXERROR("setting the colour key")
					}
					
					if (_parmsR.loadTextureB)
					{
						// release the direct draw interface:
						// since the textureP was obtained with a call
						// to QueryInterface on the surface (increasing
						// its referenc count), this wont actually release
						// the surface
						
						db_logf5(("\tabout to release surface i/f with ref %u",GetRefCount(pLoadInfo->surfaceP)));
						db_code1(refcnt =)
						pLoadInfo->surfaceP->Release();
						pLoadInfo->surfaceP = NULL;
						// if loading into a previous texture, refcnt may be two or more, our ref and the ref passed to us
						db_onlyassert1(1==refcnt|| pLoadInfo->prevTexP.voidP);
						
						pLoadInfo->resultP = pLoadInfo->textureP;
					}
					else
					{
						db_assert1(pLoadInfo->surfaceP);
						
						DDSurface * pSurfaceReturn = NULL;
						
						awTlLastDxErr = pLoadInfo->surfaceP->QueryInterface(GUID_DD_SURFACE, (LPVOID *)&pSurfaceReturn);
						HANDLE_DXERROR("getting the required DDSurface interface")
						db_logf5(("\t\tnow surface ref %u return surface ref %u",GetRefCount(pLoadInfo->surfaceP),GetRefCount(pSurfaceReturn)));
						
						pLoadInfo->resultP = pSurfaceReturn;
					}
				}
			}}
			
			// release the IDirectDrawSurface interfaces if returning DDSurface interfaces
			if (!_parmsR.loadTextureB)
			{
				for (unsigned i=0; i<nLoadInfos; ++i)
				{
					if (!arrLoadInfo[i].skipB)
					{
						db_assert1(arrLoadInfo[i].surfaceP);
						db_logf5(("\tabout to release internal surface i/f with ref %u",GetRefCount(arrLoadInfo[i].surfaceP)));
						arrLoadInfo[i].surfaceP->Release();
					}
				}
			}
		}

		// OK
		db_log4("AwCreateGraphic() OK");
		
		SurfUnion pRet = static_cast<D3DTexture *>(NULL);
		
		if (!_parmsR.rectA)
		{
			// if loading the entire graphic as one surface/texture, return pointer to that
			pRet = arrLoadInfo[0].resultP;
		}
		else
		{
			// return NULL, but fill in the pTexture or pSurface members of the AwCreateGraphicRegion
			for (unsigned i=0; i<nLoadInfos; ++i)
			{
				LoadInfo * pLoadInfo = &arrLoadInfo[i];
				
				db_assert1(pLoadInfo->rectP);
				
				if (_parmsR.loadTextureB)
				{
					if (pLoadInfo->prevTexP.voidP)
					{
						db_assert1(pLoadInfo->prevTexP.textureP == pLoadInfo->rectP->pTexture);
						db_assert1(pLoadInfo->resultP.textureP == pLoadInfo->rectP->pTexture);
						db_logf5(("\tabout to release duplicate texture i/f with ref %u",GetRefCount(pLoadInfo->resultP.textureP)));
						pLoadInfo->resultP.textureP->Release();
					}
					else
					{
						pLoadInfo->rectP->pTexture = pLoadInfo->resultP.textureP;
					}
					db_logf5(("\tresultant texture for region with ref count %u",GetRefCount(pLoadInfo->rectP->pTexture)));
				}
				else
				{
					if (pLoadInfo->prevTexP.voidP)
					{
						db_assert1(pLoadInfo->prevTexP.surfaceP == pLoadInfo->rectP->pSurface);
						db_assert1(pLoadInfo->resultP.surfaceP == pLoadInfo->rectP->pSurface);
						db_logf5(("\tabout to release duplicate surface i/f with ref %u",GetRefCount(pLoadInfo->resultP.surfaceP)));
						pLoadInfo->resultP.surfaceP->Release();
					}
					else
					{
						pLoadInfo->rectP->pSurface = pLoadInfo->resultP.surfaceP;
					}
					db_logf5(("\tresultant texture for surface with ref count %u",GetRefCount(pLoadInfo->rectP->pSurface)));
				}
			}
		}
		delete[] arrLoadInfo;
		
		#if DB_LEVEL >= 5
		if (_parmsR.loadTextureB)
			db_logf5(("AwCreateGraphic(): returning texture with ref cnt %u",GetRefCount(pRet.textureP)));
		else
			db_logf5(("AwCreateGraphic(): returning surface with ref cnt %u",GetRefCount(pRet.surfaceP)));
		#endif
		
		return pRet;
	}
		
	EXIT_WITH_ERROR:
	{
		
		db_logf2(("AwCreateGraphic(): ERROR: %s",AwTlErrorToString()));
		
		if (arrLoadInfo)
		{
			for (unsigned i=0; i<nLoadInfos; ++i)
			{
				LoadInfo * pLoadInfo = &arrLoadInfo[i];
				
				db_logf5(("\tref counts: dst tex %u dst surf %u int tex %u int surf %u",
					GetRefCount(pLoadInfo->dst_textureP),
					GetRefCount(pLoadInfo->dst_surfaceP),
					GetRefCount(pLoadInfo->textureP),
					GetRefCount(pLoadInfo->surfaceP)));
				
				if (pLoadInfo->dst_textureP)
				{
					pLoadInfo->dst_textureP->Release();
				}
				if (pLoadInfo->textureP)
				{
					pLoadInfo->textureP->Release();
				}
				if (pLoadInfo->dst_surfaceP)
				{
					pLoadInfo->dst_surfaceP->Release();
				}
				if (pLoadInfo->surfaceP)
				{
					if (pLoadInfo->surface_lockedB)
						pLoadInfo->surfaceP->Unlock(NULL);
					db_code1(refcnt =)
					pLoadInfo->surfaceP->Release();
					db_onlyassert1(!refcnt);
				}
				
				if (pLoadInfo->rectP)
				{
					pLoadInfo->rectP->width = 0;
					pLoadInfo->rectP->height = 0;
				}
			}
			
			delete[] arrLoadInfo;
		}
		
		if (dd_paletteP)
		{
			db_code1(refcnt =)
			dd_paletteP->Release();
			db_onlyassert1(!refcnt);
		}
		
		return static_cast<D3DTexture *>(NULL);
	}
}

void AwBackupTexture::OnBeginRestoring(unsigned nMaxPaletteSize)
{
	if (nMaxPaletteSize && (nMaxPaletteSize < m_nPaletteSize || !m_nPaletteSize))
	{
		awTlLastErr = AW_TLE_CANTPALETTIZE;
		db_logf3(("AwCreateGraphic(): [restoring] ERROR: Palette size is %u, require %u",m_nPaletteSize,nMaxPaletteSize));
	}
}

bool AwBackupTexture::AreRowsReversed()
{
	return false;
}

void AwBackupTexture::ConvertRow(AwTl::PtrUnion pDest, unsigned nDestWidth, AwTl::PtrUnionConst pSrc, unsigned nSrcOffset, unsigned nSrcWidth, AwTl::Colour * pPalette db_code1(DB_COMMA unsigned nPaletteSize))
{
	using namespace AwTl;

	if (pPalette)
	{
		if (pixelFormat.palettizedB)
		{
			GenericConvertRow<Colour::ConvNull,BYTE>::Do(pDest,nDestWidth,pSrc.byteP+nSrcOffset,nSrcWidth);
		}
		else
		{
			if (m_bTranspMask)
				GenericConvertRow<Colour::ConvTransp,BYTE>::Do(pDest,nDestWidth,pSrc.byteP+nSrcOffset,nSrcWidth,pPalette db_code1(DB_COMMA nPaletteSize));
			else
				GenericConvertRow<Colour::ConvNonTransp,BYTE>::Do(pDest,nDestWidth,pSrc.byteP+nSrcOffset,nSrcWidth,pPalette db_code1(DB_COMMA nPaletteSize));
		}
	}
	else
	{
		if (m_bTranspMask)
			GenericConvertRow<Colour::ConvTransp,Colour>::Do(pDest,nDestWidth,pSrc.colourP+nSrcOffset,nSrcWidth);
		else
			GenericConvertRow<Colour::ConvNonTransp,Colour>::Do(pDest,nDestWidth,pSrc.colourP+nSrcOffset,nSrcWidth);
	}
}

void AwBackupTexture::OnFinishRestoring(bool)
{
}

namespace AwTl {

	Colour * TypicalBackupTexture::GetPalette()
	{
		return m_pPalette;
	}

	PtrUnion TypicalBackupTexture::GetRowPtr(unsigned nRow)
	{
		return m_ppPixMap[nRow];
	}

	void TypicalBackupTexture::LoadNextRow(PtrUnion)
	{
		// already loaded
	}

	unsigned TypicalBackupTexture::GetNumColours()
	{
		return m_nPaletteSize;
	}

	unsigned TypicalBackupTexture::GetMinPaletteSize()
	{
		return m_nPaletteSize;
	}
	

	SurfUnion TexFileLoader::Load(MediaMedium * pMedium, CreateTextureParms const & rParams)
	{
		m_fFlags = rParams.flags;
		
		awTlLastErr = AW_TLE_OK;
		
		LoadHeaderInfo(pMedium);
		
		CHECK_MEDIA_ERRORS("loading file headers")
		ON_ERROR_RETURN_NULL("loading file headers")
		
		ChoosePixelFormat(rParams);

		if (!pixelFormat.validB)
			db_log3("AwCreateGraphic(): ERROR: pixel format not valid");
		if (!driverDesc.ddP || !driverDesc.validB && rParams.loadTextureB)
			db_log3("AwCreateGraphic(): ERROR: driver description not valid");
		
		awTlLastErr = pixelFormat.validB && driverDesc.ddP && (driverDesc.validB || !rParams.loadTextureB) ? AW_TLE_OK : AW_TLE_NOINIT;
		
		ON_ERROR_RETURN_NULL("initializing load")
		
		AllocateBuffers(rParams.backupHP ? true : false, pixelFormat.palettizedB ? 1<<pixelFormat.bitsPerPixel : 0);
		
		CHECK_MEDIA_ERRORS("allocating buffers")
		ON_ERROR_RETURN_NULL("allocating buffers")
		
		db_logf4(("\tThe image in the file is %ux%u with %u %spalette",m_nWidth,m_nHeight,m_nPaletteSize ? m_nPaletteSize : 0,m_nPaletteSize ? "colour " : ""));
		
		SurfUnion pTex = CreateTexture(rParams);
		
		bool bOK = AW_TLE_OK == awTlLastErr;
		
		CHECK_MEDIA_ERRORS("loading image data")
		
		if (bOK && awTlLastErr != AW_TLE_OK)
		{
			// an error occurred which was not detected in CreateTexture()
			if (pTex.voidP)
			{
				if (rParams.loadTextureB)
					pTex.textureP->Release();
				else
					pTex.surfaceP->Release();
				pTex.voidP = NULL;
			}
			else
			{
				db_assert1(rParams.rectA);
				
				for (unsigned i=0; i<rParams.numRects; ++i)
				{
					AwCreateGraphicRegion * pRect = &rParams.rectA[i];
					
					if (!rParams.prevTexB)
					{
						// release what was created
						if (rParams.loadTextureB)
							pRect->pTexture->Release();
						else
							pRect->pSurface->Release();
					}
					pRect->width = 0;
					pRect->height = 0;
				}
			}
			db_logf1(("AwCreateGraphic(): ERROR: %s",AwTlErrorToString()));
			bOK = false;
		}
		
		OnFinishLoading(bOK);
		
		if (bOK && rParams.backupHP)
		{
			*rParams.backupHP = CreateBackupTexture();
		}
		
		return pTex;
	}

	void TexFileLoader::OnFinishLoading(bool)
	{
	}


	TypicalTexFileLoader::~TypicalTexFileLoader()
	{
		if (m_pPalette)
		{
			delete[] m_pPalette;
			
			if (m_pRowBuf) delete[] m_pRowBuf.byteP;
			if (m_ppPixMap)
			{
				delete[] m_ppPixMap->byteP;
				delete[] m_ppPixMap;
			}
		}
		else
		{
			if (m_pRowBuf) delete[] m_pRowBuf.colourP;
			if (m_ppPixMap)
			{
				delete[] m_ppPixMap->colourP;
				delete[] m_ppPixMap;
			}
		}
	}
	
	unsigned TypicalTexFileLoader::GetNumColours()
	{
		return m_nPaletteSize;
	}

	unsigned TypicalTexFileLoader::GetMinPaletteSize()
	{
		return m_nPaletteSize;
	}

	void TypicalTexFileLoader::AllocateBuffers(bool bWantBackup, unsigned /*nMaxPaletteSize*/)
	{
		if (m_nPaletteSize)
		{
			m_pPalette = new Colour [ m_nPaletteSize ];
		}
		
		if (bWantBackup)
		{
			m_ppPixMap = new PtrUnion [m_nHeight];
			if (m_nPaletteSize)
			{
				m_ppPixMap->byteP = new BYTE [m_nHeight*m_nWidth];
				BYTE * pRow = m_ppPixMap->byteP;
				for (unsigned y=1;y<m_nHeight;++y)
				{
					pRow += m_nWidth;
					m_ppPixMap[y].byteP = pRow;
				}
			}
			else
			{
				m_ppPixMap->colourP = new Colour [m_nHeight*m_nWidth];
				Colour * pRow = m_ppPixMap->colourP;
				for (unsigned y=1;y<m_nHeight;++y)
				{
					pRow += m_nWidth;
					m_ppPixMap[y].colourP = pRow;
				}
			}
		}
		else
		{
			if (m_nPaletteSize)
				m_pRowBuf.byteP = new BYTE [m_nWidth];
			else
				m_pRowBuf.colourP = new Colour [m_nWidth];
		}
	}

	PtrUnion TypicalTexFileLoader::GetRowPtr(unsigned nRow)
	{
		if (m_ppPixMap)
		{
			return m_ppPixMap[nRow];
		}
		else
		{
			return m_pRowBuf;
		}
	}

	AwBackupTexture * TypicalTexFileLoader::CreateBackupTexture()
	{
		AwBackupTexture * pBackup = new TypicalBackupTexture(*this,m_ppPixMap,m_pPalette);
		m_ppPixMap = NULL;
		m_pPalette = NULL;
		return pBackup;
	}
	
	/****************************************************************************/
	/* For determining which loader should be used for the file format detected */
	/****************************************************************************/

	static
	class MagicFileIdTree
	{
		public:
			MagicFileIdTree()
				: m_pfnCreate(NULL)
				#ifdef _MSC_VER
				, hack(0)
				#endif
			{
				for (unsigned i=0; i<256; ++i)
					m_arrNextLayer[i]=NULL;
			}
			
			~MagicFileIdTree()
			{
				for (unsigned i=0; i<256; ++i)
					if (m_arrNextLayer[i]) delete m_arrNextLayer[i];
			}
		
			MagicFileIdTree * m_arrNextLayer [256];
			
			TexFileLoader * (* m_pfnCreate) ();
			
		#ifdef _MSC_VER
			unsigned hack;
		#endif
	}
		* g_pMagicFileIdTree = NULL;
	
	void RegisterLoader(char const * pszMagic, TexFileLoader * (* pfnCreate) () )
	{
		static MagicFileIdTree mfidt;

#ifdef _MSC_VER
		// Touch the loaders.
		{
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwBmpLoader_187);
			
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwIffLoader_428);
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwIffLoader_429);
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwIffLoader_430);
			
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwPpmLoader_229);
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwPgmLoader_230);
			mfidt.hack += reinterpret_cast<unsigned>(&rlcAwPbmLoader_231);

			mfidt.hack += reinterpret_cast<unsigned>(&rccIlbmBmhdChunk_4);
			mfidt.hack += reinterpret_cast<unsigned>(&rccIlbmCmapChunk_5);
			mfidt.hack += reinterpret_cast<unsigned>(&rccIlbmBodyChunk_6);
			mfidt.hack += reinterpret_cast<unsigned>(&rccIlbmGrabChunk_7);
		}
#endif
		g_pMagicFileIdTree = &mfidt;
		
		MagicFileIdTree * pLayer = g_pMagicFileIdTree;
		
		while (*pszMagic)
		{
			BYTE c = static_cast<BYTE>(*pszMagic++);
			
			if (!pLayer->m_arrNextLayer[c])
				pLayer->m_arrNextLayer[c] = new MagicFileIdTree;
				
			pLayer = pLayer->m_arrNextLayer[c];
		}
		
		db_assert1(!pLayer->m_pfnCreate);
		
		pLayer->m_pfnCreate = pfnCreate;
	}
	
	static
	TexFileLoader * CreateLoaderObject(MediaMedium * pMedium)
	{
		TexFileLoader * (* pfnBest) () = NULL;
		
		signed nMoveBack = 0;
		
		BYTE c;
		
		MagicFileIdTree * pLayer = g_pMagicFileIdTree;
		
		while (pLayer)
		{
			if (pLayer->m_pfnCreate)
				pfnBest = pLayer->m_pfnCreate;
			
			MediaRead(pMedium,&c);
			
			-- nMoveBack;
			
			pLayer = pLayer->m_arrNextLayer[c];
		}
		
		pMedium->MovePos(nMoveBack);
		
		if (pfnBest)
			return pfnBest();
		else
			return NULL;
	}

	/**********************************/
	/* These are the loader functions */
	/**********************************/
	
	static inline SurfUnion DoLoadTexture(MediaMedium * pMedium, CreateTextureParms const & rParams)
	{
		TexFileLoader * pLoader = CreateLoaderObject(pMedium);
		
		if (!pLoader)
		{
			awTlLastErr = AW_TLE_BADFILEFORMAT;
			db_log1("AwCreateGraphic(): ERROR: file format not recognized");
			return static_cast<D3DTexture *>(NULL);
		}
		else
		{
			SurfUnion pTex = pLoader->Load(pMedium,rParams);
			pLoader->Release();
			return pTex;
		}
	}
	
	static inline SurfUnion LoadTexture(MediaMedium * pMedium, CreateTextureParms const & _parmsR)
	{
		if (_parmsR.bytesReadP||_parmsR.maxReadBytes!=UINT_MAX)
		{
			MediaSection * pMedSect = new MediaSection;
			pMedSect->Open(pMedium,_parmsR.maxReadBytes);
			SurfUnion pTex = DoLoadTexture(pMedSect,_parmsR);
			pMedSect->Close();
			if (_parmsR.bytesReadP) *_parmsR.bytesReadP = pMedSect->GetUsedSize();
			delete pMedSect;
			return pTex;
		}
		else
		{
			return DoLoadTexture(pMedium,_parmsR);
		}
	}
	
	SurfUnion CreateTextureParms::DoCreate() const
	{
		if (INVALID_HANDLE_VALUE!=fileH)
		{
			MediaWinFileMedium * pMedium = new MediaWinFileMedium;
			pMedium->Attach(fileH);
			SurfUnion pTex = LoadTexture(pMedium,*this);
			pMedium->Detach();
			pMedium->Release();
			return pTex;
		}
		else if (dataP)
		{
			MediaMemoryReadMedium * pMedium = new MediaMemoryReadMedium;
			pMedium->Open(dataP);
			SurfUnion pTex = LoadTexture(pMedium,*this);
			pMedium->Close();
			pMedium->Release();
			return pTex;
		}
		else
		{
			db_assert1(restoreH);
			return restoreH->Restore(*this);
		}
	}
	
	#if DB_LEVEL >= 4
	static void LogPrimCaps(LPD3DPRIMCAPS _pcP, bool _triB)
	{
		#define DEVCAP(mask,can_or_does,explanation) \
			db_logf4(("\t\t" can_or_does "%s " explanation, _pcP->MEMBER & (mask) ? "" : "not"));
			
		#define MEMBER dwMiscCaps
		DEVCAP(D3DPMISCCAPS_CONFORMANT,"Does ","conform to OpenGL standard")
		if (_triB)
		{
			DEVCAP(D3DPMISCCAPS_CULLCCW,"Does ","support counterclockwise culling through the D3DRENDERSTATE_CULLMODE state")
			DEVCAP(D3DPMISCCAPS_CULLCW,"Does ","support clockwise triangle culling through the D3DRENDERSTATE_CULLMODE state")
			db_logf4(("\t\tDoes %s perform triangle culling", _pcP->dwMiscCaps & (D3DPMISCCAPS_CULLNONE) ? "not" : ""));
		}
		else
		{
			DEVCAP(D3DPMISCCAPS_LINEPATTERNREP,"Can","handle values other than 1 in the wRepeatFactor member of the D3DLINEPATTERN structure")
		}
		DEVCAP(D3DPMISCCAPS_MASKPLANES,"Can","perform a bitmask of color planes")
		DEVCAP(D3DPMISCCAPS_MASKZ,"Can","enable and disable modification of the z-buffer on pixel operations")
		#undef MEMBER
		#define MEMBER dwRasterCaps
		DEVCAP(D3DPRASTERCAPS_ANISOTROPY,"Does ","support anisotropic filtering")
		DEVCAP(D3DPRASTERCAPS_ANTIALIASEDGES,"Can","antialias lines forming the convex outline of objects")
		DEVCAP(D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT,"Does","support antialiasing that is dependent on the sort order of the polygons")
		DEVCAP(D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT,"Does","support antialiasing that is not dependent on the sort order of the polygons")
		DEVCAP(D3DPRASTERCAPS_DITHER,"Can","dither to improve color resolution")
		DEVCAP(D3DPRASTERCAPS_FOGRANGE,"Does ","support range-based fog")
		DEVCAP(D3DPRASTERCAPS_FOGTABLE,"Does ","calculate the fog value by referring to a lookup table containing fog values that are indexed to the depth of a given pixel")
		DEVCAP(D3DPRASTERCAPS_FOGVERTEX,"Does ","calculate the fog value during the lighting operation")
		DEVCAP(D3DPRASTERCAPS_MIPMAPLODBIAS,"Does ","support level-of-detail (LOD) bias adjustments")
		DEVCAP(D3DPRASTERCAPS_PAT,"Can","perform patterned drawing for the primitive being queried")
		DEVCAP(D3DPRASTERCAPS_ROP2,"Can","support raster operations other than R2_COPYPEN")
		DEVCAP(D3DPRASTERCAPS_STIPPLE,"Can","stipple polygons to simulate translucency")
		DEVCAP(D3DPRASTERCAPS_SUBPIXEL,"Does ","perform subpixel placement of z, color, and texture data, rather than working with the nearest integer pixel coordinate")
		DEVCAP(D3DPRASTERCAPS_SUBPIXELX,"Is ","subpixel accurate along the x-axis only and is clamped to an integer y-axis scan line")
		DEVCAP(D3DPRASTERCAPS_XOR,"Can","support XOR operations")
		DEVCAP(D3DPRASTERCAPS_ZBIAS,"Does ","support z-bias values")
		DEVCAP(D3DPRASTERCAPS_ZBUFFERLESSHSR,"Can","perform hidden-surface removal without requiring the application to sort polygons, and without requiring the allocation of a z-buffer")
		DEVCAP(D3DPRASTERCAPS_ZTEST,"Can","perform z-test operations")
		#undef MEMBER
		#define MEMBER dwZCmpCaps
		DEVCAP(D3DPCMPCAPS_ALWAYS,"Can","always pass the z test")
		DEVCAP(D3DPCMPCAPS_EQUAL,"Can","pass the z test if the new z equals the current z")
		DEVCAP(D3DPCMPCAPS_GREATER,"Can","pass the z test if the new z is greater than the current z")
		DEVCAP(D3DPCMPCAPS_GREATEREQUAL,"Can","pass the z test if the new z is greater than or equal to the current z")
		DEVCAP(D3DPCMPCAPS_LESS,"Can","pass the z test if the new z is less than the current z")
		DEVCAP(D3DPCMPCAPS_LESSEQUAL,"Can","pass the z test if the new z is less than or equal to the current z")
		DEVCAP(D3DPCMPCAPS_NEVER,"Can","always fail the z test")
		DEVCAP(D3DPCMPCAPS_NOTEQUAL,"Can","pass the z test if the new z does not equal the current z")
		#undef MEMBER
		#define MEMBER dwSrcBlendCaps
		DEVCAP(D3DPBLENDCAPS_BOTHINVSRCALPHA,"Can","source blend with source blend factor of (1-As, 1-As, 1-As, 1-As) and destination blend factor of (As, As, As, As); the destination blend selection is overridden")
		DEVCAP(D3DPBLENDCAPS_BOTHSRCALPHA,"Can","source blend with source blend factor of (As, As, As, As) and destination blend factor of (1-As, 1-As, 1-As, 1-As); the destination blend selection is overridden")
		DEVCAP(D3DPBLENDCAPS_DESTALPHA,"Can","source blend with blend factor of (Ad, Ad, Ad, Ad)")
		DEVCAP(D3DPBLENDCAPS_DESTCOLOR,"Can","source blend with blend factor of (Rd, Gd, Bd, Ad)")
		DEVCAP(D3DPBLENDCAPS_INVDESTALPHA,"Can","source blend with blend factor of (1-Ad, 1-Ad, 1-Ad, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_INVDESTCOLOR,"Can","source blend with blend factor of (1-Rd, 1-Gd, 1-Bd, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_INVSRCALPHA,"Can","source blend with blend factor of (1-As, 1-As, 1-As, 1-As)")
		DEVCAP(D3DPBLENDCAPS_INVSRCCOLOR,"Can","source blend with blend factor of (1-Rd, 1-Gd, 1-Bd, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_ONE,"Can","source blend with blend factor of (1, 1, 1, 1)")
		DEVCAP(D3DPBLENDCAPS_SRCALPHA,"Can","source blend with blend factor of (As, As, As, As)")
		DEVCAP(D3DPBLENDCAPS_SRCALPHASAT,"Can","source blend with blend factor of (f, f, f, 1); f = min(As, 1-Ad).")
		DEVCAP(D3DPBLENDCAPS_SRCCOLOR,"Can","source blend with blend factor of (Rs, Gs, Bs, As)")
		DEVCAP(D3DPBLENDCAPS_ZERO,"Can","source blend with blend factor of (0, 0, 0, 0)")
		#undef MEMBER
		#define MEMBER dwDestBlendCaps
		DEVCAP(D3DPBLENDCAPS_BOTHINVSRCALPHA,"Can","destination blend with source blend factor of (1-As, 1-As, 1-As, 1-As) and destination blend factor of (As, As, As, As); the destination blend selection is overridden")
		DEVCAP(D3DPBLENDCAPS_BOTHSRCALPHA,"Can","destination blend with source blend factor of (As, As, As, As) and destination blend factor of (1-As, 1-As, 1-As, 1-As); the destination blend selection is overridden")
		DEVCAP(D3DPBLENDCAPS_DESTALPHA,"Can","destination blend with blend factor of (Ad, Ad, Ad, Ad)")
		DEVCAP(D3DPBLENDCAPS_DESTCOLOR,"Can","destination blend with blend factor of (Rd, Gd, Bd, Ad)")
		DEVCAP(D3DPBLENDCAPS_INVDESTALPHA,"Can","destination blend with blend factor of (1-Ad, 1-Ad, 1-Ad, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_INVDESTCOLOR,"Can","destination blend with blend factor of (1-Rd, 1-Gd, 1-Bd, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_INVSRCALPHA,"Can","destination blend with blend factor of (1-As, 1-As, 1-As, 1-As)")
		DEVCAP(D3DPBLENDCAPS_INVSRCCOLOR,"Can","destination blend with blend factor of (1-Rd, 1-Gd, 1-Bd, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_ONE,"Can","destination blend with blend factor of (1, 1, 1, 1)")
		DEVCAP(D3DPBLENDCAPS_SRCALPHA,"Can","destination blend with blend factor of (As, As, As, As)")
		DEVCAP(D3DPBLENDCAPS_SRCALPHASAT,"Can","destination blend with blend factor of (f, f, f, 1); f = min(As, 1-Ad)")
		DEVCAP(D3DPBLENDCAPS_SRCCOLOR,"Can","destination blend with blend factor of (Rs, Gs, Bs, As)")
		DEVCAP(D3DPBLENDCAPS_ZERO,"Can","destination blend with blend factor of (0, 0, 0, 0)")
		#undef MEMBER
		#define MEMBER dwAlphaCmpCaps
		DEVCAP(D3DPCMPCAPS_ALWAYS,"Can","always pass the alpha test")
		DEVCAP(D3DPCMPCAPS_EQUAL,"Can","pass the alpha test if the new alpha equals the current alpha")
		DEVCAP(D3DPCMPCAPS_GREATER,"Can","pass the alpha test if the new alpha is greater than the current alpha")
		DEVCAP(D3DPCMPCAPS_GREATEREQUAL,"Can","pass the alpha test if the new alpha is greater than or equal to the current alpha")
		DEVCAP(D3DPCMPCAPS_LESS,"Can","pass the alpha test if the new alpha is less than the current alpha")
		DEVCAP(D3DPCMPCAPS_LESSEQUAL,"Can","pass the alpha test if the new alpha is less than or equal to the current alpha")
		DEVCAP(D3DPCMPCAPS_NEVER,"Can","always fail the alpha test")
		DEVCAP(D3DPCMPCAPS_NOTEQUAL,"Can","pass the alpha test if the new alpha does not equal the current alpha")
		#undef MEMBER
		#define MEMBER dwShadeCaps
		DEVCAP(D3DPSHADECAPS_ALPHAFLATBLEND,"Can","support an alpha component for flat blended transparency")
		DEVCAP(D3DPSHADECAPS_ALPHAFLATSTIPPLED,"Can","support an alpha component for flat stippled transparency")
		DEVCAP(D3DPSHADECAPS_ALPHAGOURAUDBLEND,"Can","support an alpha component for Gouraud blended transparency")
		DEVCAP(D3DPSHADECAPS_ALPHAGOURAUDSTIPPLED,"Can","support an alpha component for Gouraud stippled transparency")
		DEVCAP(D3DPSHADECAPS_ALPHAPHONGBLEND,"Can","support an alpha component for Phong blended transparency")
		DEVCAP(D3DPSHADECAPS_ALPHAPHONGSTIPPLED,"Can","support an alpha component for Phong stippled transparency")
		DEVCAP(D3DPSHADECAPS_COLORFLATMONO,"Can","support colored flat shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_COLORFLATRGB,"Can","support colored flat shading in the D3DCOLOR_RGB color model")
		DEVCAP(D3DPSHADECAPS_COLORGOURAUDMONO,"Can","support colored flat shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_COLORGOURAUDRGB,"Can","support colored Gouraud shading in the D3DCOLOR_RGB color model")
		DEVCAP(D3DPSHADECAPS_COLORPHONGMONO,"Can","support colored Phong shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_COLORPHONGRGB,"Can","support colored Phong shading in the D3DCOLOR_RGB color model")
		DEVCAP(D3DPSHADECAPS_FOGFLAT,"Can","support fog in the flat shading model")
		DEVCAP(D3DPSHADECAPS_FOGGOURAUD,"Can","support fog in the Gouraud shading model")
		DEVCAP(D3DPSHADECAPS_FOGPHONG,"Can","support fog in the Phong shading model")
		DEVCAP(D3DPSHADECAPS_SPECULARFLATMONO,"Can","support specular highlights in flat shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_SPECULARFLATRGB,"Can","support specular highlights in flat shading in the D3DCOLOR_RGB color model")
		DEVCAP(D3DPSHADECAPS_SPECULARGOURAUDMONO,"Can","support specular highlights in Gouraud shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_SPECULARGOURAUDRGB,"Can","support specular highlights in Gouraud shading in the D3DCOLOR_RGB color model")
		DEVCAP(D3DPSHADECAPS_SPECULARPHONGMONO,"Can","support specular highlights in Phong shading in the D3DCOLOR_MONO color model")
		DEVCAP(D3DPSHADECAPS_SPECULARPHONGRGB,"Can","support specular highlights in Phong shading in the D3DCOLOR_RGB color model")
		#undef MEMBER
		#define MEMBER dwTextureCaps
		DEVCAP(D3DPTEXTURECAPS_ALPHA,"Does ","support RGBA textures in the D3DTEX_DECAL and D3DTEX_MODULATE texture filtering modes")
		DEVCAP(D3DPTEXTURECAPS_BORDER,"Does ","support texture mapping along borders")
		DEVCAP(D3DPTEXTURECAPS_PERSPECTIVE,"Does ","support perspective correction")
		DEVCAP(D3DPTEXTURECAPS_POW2,"Does ","require all nonmipmapped textures to have widths and heights specified as powers of two")
		DEVCAP(D3DPTEXTURECAPS_SQUAREONLY,"Does ","require all textures to be square")
		DEVCAP(D3DPTEXTURECAPS_TRANSPARENCY,"Does ","support texture transparency")
		#undef MEMBER
		#define MEMBER dwTextureFilterCaps
		DEVCAP(D3DPTFILTERCAPS_LINEAR,"Can","use a weighted average of a 2x2 area of texels surrounding the desired pixel")
		DEVCAP(D3DPTFILTERCAPS_LINEARMIPLINEAR,"Can","use a weighted average of a 2x2 area of texels, and also interpolate between mipmaps")
		DEVCAP(D3DPTFILTERCAPS_LINEARMIPNEAREST,"Can","use a weighted average of a 2x2 area of texels, and also use a mipmap")
		DEVCAP(D3DPTFILTERCAPS_MIPLINEAR,"Can","choose two mipmaps whose texels most closely match the size of the pixel to be textured, and interpolate between them")
		DEVCAP(D3DPTFILTERCAPS_MIPNEAREST,"Can","use an appropriate mipmap for texel selection")
		DEVCAP(D3DPTFILTERCAPS_NEAREST,"Can","use the texel with coordinates nearest to the desired pixel value is used")
		#undef MEMBER
		#define MEMBER dwTextureBlendCaps
		DEVCAP(D3DPTBLENDCAPS_ADD,"Can","use the additive texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_COPY,"Can","use copy mode texture-blending")
		DEVCAP(D3DPTBLENDCAPS_DECAL,"Can","use decal texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_DECALALPHA,"Can","use decal-alpha texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_DECALMASK,"Can","use decal-mask texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_MODULATE,"Can","use modulate texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_MODULATEALPHA,"Can","use modulate-alpha texture-blending mode")
		DEVCAP(D3DPTBLENDCAPS_MODULATEMASK,"Can","use modulate-mask texture-blending mode")
		#undef MEMBER
		#define MEMBER dwTextureAddressCaps
		DEVCAP(D3DPTADDRESSCAPS_BORDER,"Does ","support setting coordinates outside the range [0.0, 1.0] to the border color")
		DEVCAP(D3DPTADDRESSCAPS_CLAMP,"Can","clamp textures to addresses")
		DEVCAP(D3DPTADDRESSCAPS_INDEPENDENTUV,"Can","separate the texture-addressing modes of the U and V coordinates of the texture")
		DEVCAP(D3DPTADDRESSCAPS_MIRROR,"Can","mirror textures to addresses")
		DEVCAP(D3DPTADDRESSCAPS_WRAP,"Can","wrap textures to addresses")
		#undef MEMBER
		#undef DEVCAP
		db_logf4(("\t\tMaximum size of the supported stipple is %u x %u",_pcP->dwStippleWidth,_pcP->dwStippleHeight));
	}
	static void LogCaps(LPD3DDEVICEDESC _descP)
	{
		if (_descP->dwFlags & D3DDD_BCLIPPING)
		{
			db_logf4(("\tCan%s perform 3D clipping",_descP->bClipping ? "" : "not"));
		}
		else db_log4("\tHas unknown 3D clipping capability");
		
		if (_descP->dwFlags & D3DDD_COLORMODEL)
		{
			db_logf4(("\tCan%s use mono (ramp) colour model",_descP->dcmColorModel & D3DCOLOR_MONO ? "" : "not"));
			db_logf4(("\tCan%s use full RGB colour model",_descP->dcmColorModel & D3DCOLOR_RGB ? "" : "not"));
		}
		else db_log4("\tHas unknown colour model");
		
		if (_descP->dwFlags & D3DDD_DEVCAPS)
		{
			#define DEVCAP(mask,can_or_does,explanation) \
				db_logf4(("\t" can_or_does "%s " explanation,_descP->dwDevCaps & (mask) ? "" : "not"));
				
			DEVCAP(D3DDEVCAPS_CANRENDERAFTERFLIP,"Can","queue rendering commands after a page flip")
			DEVCAP(D3DDEVCAPS_DRAWPRIMTLVERTEX,"Does ","export a DrawPrimitive-aware HAL")
			DEVCAP(D3DDEVCAPS_EXECUTESYSTEMMEMORY,"Can","use execute buffers from system memory")
			DEVCAP(D3DDEVCAPS_EXECUTEVIDEOMEMORY,"Can","use execute buffer from video memory")
			DEVCAP(D3DDEVCAPS_FLOATTLVERTEX,"Does ","accept floating point for post-transform vertex data")
			DEVCAP(D3DDEVCAPS_SORTDECREASINGZ,"Does ","need Z data sorted for decreasing depth")
			DEVCAP(D3DDEVCAPS_SORTEXACT,"Does ","need data sorted exactly")
			DEVCAP(D3DDEVCAPS_SORTINCREASINGZ,"Does ","need data sorted for increasing depth")
			DEVCAP(D3DDEVCAPS_TEXTURENONLOCALVIDMEM,"Can","retrieve textures from nonlocal video (AGP) memory")
			DEVCAP(D3DDEVCAPS_TEXTURESYSTEMMEMORY,"Can","retrieve textures from system memory")
			DEVCAP(D3DDEVCAPS_TEXTUREVIDEOMEMORY,"Can","retrieve textures from device memory")
			DEVCAP(D3DDEVCAPS_TLVERTEXSYSTEMMEMORY,"Can","use buffers from system memory for transformed and lit vertices")
			DEVCAP(D3DDEVCAPS_TLVERTEXVIDEOMEMORY,"Can","use buffers from video memory for transformed and lit vertices")
			
			#undef DEVCAP
		}
		else db_log4("\tHas unknown device capabilities");
		
		if (_descP->dwFlags & D3DDD_DEVICERENDERBITDEPTH)
		{
			#define DEVCAP(mask,explanation) \
				db_logf4(("\tCan%s render to "explanation" surface",_descP->dwDeviceRenderBitDepth & (mask) ? "" : "not"));
				
			DEVCAP(DDBD_8,"an 8-bit")
			DEVCAP(DDBD_16,"a 16-bit")
			DEVCAP(DDBD_24,"a 24-bit")
			DEVCAP(DDBD_32,"a 32-bit")
			
			#undef DEVCAP
		}
		else db_log4("\tHas unknown rendering target bitdepth requirements");
		
		if (_descP->dwFlags & D3DDD_DEVICEZBUFFERBITDEPTH)
		{
			#define DEVCAP(mask,explanation) \
				db_logf4(("\tCan%s use "explanation" Z-buffer",_descP->dwDeviceZBufferBitDepth & (mask) ? "" : "not"));
				
			DEVCAP(DDBD_8,"an 8-bit")
			DEVCAP(DDBD_16,"a 16-bit")
			DEVCAP(DDBD_24,"a 24-bit")
			DEVCAP(DDBD_32,"a 32-bit")
			
			#undef DEVCAP
		}
		else db_log4("\tHas unknown Z-buffer bitdepth requirements");
		
		if (_descP->dwFlags & D3DDD_TRANSFORMCAPS)
		{
			db_log4("\tTransform capabilities are known");
		}
		else db_log4("\tHas unknown transform capabilities");
		
		if (_descP->dwFlags & D3DDD_LIGHTINGCAPS)
		{
			db_log4("\tLighting capabilities are known");
		}
		else db_log4("\tHas unknown lighting capabilities");
		
		if (_descP->dwFlags & D3DDD_LINECAPS)
		{
			db_log4("\tLine drawing capabilities follow");
			LogPrimCaps(&_descP->dpcLineCaps,false);
		}
		else db_log4("\tHas unknown line drawing capabilities");
		
		if (_descP->dwFlags & D3DDD_TRICAPS)
		{
			db_log4("\tTriangle rendering capabilities follow");
			LogPrimCaps(&_descP->dpcTriCaps,true);
		}
		else db_log4("\tHas unknown triangle rendering capabilities");
		
		if (_descP->dwFlags & D3DDD_MAXBUFFERSIZE)
		{
			unsigned max_exb = _descP->dwMaxBufferSize;
			if (!max_exb) max_exb = UINT_MAX;
			db_logf4(("\tMaximum execute buffer size is %u",max_exb));
		}
		else db_log4("\tHas unknown maximum execute buffer size");
		
		if (_descP->dwFlags & D3DDD_MAXVERTEXCOUNT)
		{
			db_logf4(("\tMaximum vertex count is %u",_descP->dwMaxVertexCount));
		}
		else db_log4("\tHas unknown maximum vertex count");
		
		unsigned max_tw = _descP->dwMaxTextureWidth;
		unsigned max_th = _descP->dwMaxTextureHeight;
		unsigned max_sw = _descP->dwMaxStippleWidth;
		unsigned max_sh = _descP->dwMaxStippleHeight;
		if (!max_tw) max_tw = UINT_MAX;
		if (!max_th) max_th = UINT_MAX;
		if (!max_sw) max_sw = UINT_MAX;
		if (!max_sh) max_sh = UINT_MAX;
		
		db_logf4(("\tMinimum texture size is %u x %u",_descP->dwMinTextureWidth,_descP->dwMinTextureHeight));
		db_logf4(("\tMaximum texture size is %u x %u",max_tw,max_th));
		db_logf4(("\tMinimum stipple size is %u x %u",_descP->dwMinStippleWidth,_descP->dwMinStippleHeight));
		db_logf4(("\tMaximum stipple size is %u x %u",max_sw,max_sh));
	}
	#endif

	// Parse the format string and get the parameters
	
	static bool ParseParams(CreateTextureParms * pParams, char const * _argFormatS, va_list ap)
	{
		bool bad_parmsB = false;
		db_code2(unsigned ch_off = 0;)
		db_code2(char ch = 0;)
		
		while (*_argFormatS && !bad_parmsB)
		{
			db_code2(++ch_off;)
			db_code2(ch = *_argFormatS;)
			switch (*_argFormatS++)
			{
				case 's':
					if (pParams->fileNameS || INVALID_HANDLE_VALUE!=pParams->fileH || pParams->dataP || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->fileNameS = va_arg(ap,LPCTSTR);
						db_logf4(("\tFilename = \"%s\"",pParams->fileNameS));
					}
					break;
				case 'h':
					if (pParams->fileNameS || INVALID_HANDLE_VALUE!=pParams->fileH || pParams->dataP || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->fileH = va_arg(ap,HANDLE);
						db_logf4(("\tFile HANDLE = 0x%08x",pParams->fileH));
					}
					break;
				case 'p':
					if (pParams->fileNameS || INVALID_HANDLE_VALUE!=pParams->fileH || pParams->dataP || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->dataP = va_arg(ap,void const *);
						db_logf4(("\tData Pointer = %p",pParams->dataP));
					}
					break;
				case 'r':
					if (pParams->fileNameS || INVALID_HANDLE_VALUE!=pParams->fileH || pParams->dataP || pParams->restoreH || UINT_MAX!=pParams->maxReadBytes || pParams->bytesReadP || pParams->backupHP)
						bad_parmsB = true;
					else
					{
						pParams->restoreH = va_arg(ap,AW_BACKUPTEXTUREHANDLE);
						db_logf4(("\tRestore Handle = 0x%08x",pParams->restoreH));
					}
					break;
				case 'x':
					if (UINT_MAX!=pParams->maxReadBytes || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->maxReadBytes = va_arg(ap,unsigned);
						db_logf4(("\tMax bytes to read = %u",pParams->maxReadBytes));
					}
					break;
				case 'N':
					if (pParams->bytesReadP || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->bytesReadP = va_arg(ap,unsigned *);
						db_logf4(("\tPtr to bytes read = %p",pParams->bytesReadP));
					}
					break;
				case 'f':
					if (AW_TLF_DEFAULT!=pParams->flags)
						bad_parmsB = true;
					else
					{
						pParams->flags = va_arg(ap,unsigned);
						db_logf4(("\tFlags = 0x%08x",pParams->flags));
					}
					break;
				case 'W':
					if (pParams->widthP || pParams->rectA)
						bad_parmsB = true;
					else
					{
						pParams->widthP = va_arg(ap,unsigned *);
						db_logf4(("\tPtr to width = %p",pParams->widthP));
					}
					break;
				case 'H':
					if (pParams->heightP || pParams->rectA)
						bad_parmsB = true;
					else
					{
						pParams->heightP = va_arg(ap,unsigned *);
						db_logf4(("\tPtr to height = %p",pParams->heightP));
					}
					break;
				case 'X':
					if (pParams->originalWidthP)
						bad_parmsB = true;
					else
					{
						pParams->originalWidthP = va_arg(ap,unsigned *);
						db_logf4(("\tPtr to image width = %p",pParams->originalWidthP));
					}
					break;
				case 'Y':
					if (pParams->originalHeightP)
						bad_parmsB = true;
					else
					{
						pParams->originalHeightP = va_arg(ap,unsigned *);
						db_logf4(("\tPtr to image height = %p",pParams->originalHeightP));
					}
					break;
				case 'B':
					if (pParams->backupHP || pParams->restoreH)
						bad_parmsB = true;
					else
					{
						pParams->backupHP = va_arg(ap,AW_BACKUPTEXTUREHANDLE *);
						db_logf4(("\tPtr to backup handle = %p",pParams->backupHP));
					}
					break;
				case 't':
					if (pParams->prevTexP.voidP)
						bad_parmsB = true;
					else if (pParams->rectA)
					{
						pParams->prevTexB = true;
						db_log4("\tPrevious DDSurface * or D3DTexture * in rectangle array");
					}
					else if (pParams->loadTextureB)
					{
						pParams->prevTexP = va_arg(ap,D3DTexture *);
						db_logf4(("\tPrevious D3DTexture * = %p",pParams->prevTexP.textureP));
					}
					else
					{
						pParams->prevTexP = va_arg(ap,DDSurface *);
						db_logf4(("\tPrevious DDSurface * = %p",pParams->prevTexP.surfaceP));
					}
					break;
				case 'c':
					if (pParams->callbackF)
						bad_parmsB = true;
					else
					{
						pParams->callbackF = va_arg(ap,AW_TL_PFN_CALLBACK);
						pParams->callbackParam = va_arg(ap,void *);
						db_logf4(("\tCallback function = %p, param = %p",pParams->callbackF,pParams->callbackParam));
					}
					break;
				case 'a':
					if (pParams->prevTexP.voidP || pParams->rectA || pParams->widthP || pParams->heightP)
						bad_parmsB = true;
					else
					{
						pParams->numRects = va_arg(ap,unsigned);
						pParams->rectA = va_arg(ap,AwCreateGraphicRegion *);
						db_logf4(("\tRectangle array = %p, size = %u",pParams->rectA,pParams->numRects));
					}
					break;
				default:
					bad_parmsB = true;
			}
		}
		
		if (!pParams->fileNameS && INVALID_HANDLE_VALUE==pParams->fileH && !pParams->dataP && !pParams->restoreH)
		{
			awTlLastErr = AW_TLE_BADPARMS;
			db_log2("AwCreateGraphic(): ERROR: No data medium is specified");
			return false;
		}
		else if (bad_parmsB)
		{
			awTlLastErr = AW_TLE_BADPARMS;
			db_logf2(("AwCreateGraphic(): ERROR: Unexpected '%c' in format string at character %u",ch,ch_off));
			return false;
		}
		else
		{
			db_log5("\tParameters are OK");
			return true;
		}
	}
	
	// Use the parameters parsed to load the surface or texture
	
	SurfUnion LoadFromParams(CreateTextureParms * pParams)
	{
		if (pParams->fileNameS)
		{
			pParams->fileH = CreateFile(pParams->fileNameS,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		
			if (INVALID_HANDLE_VALUE==pParams->fileH)
			{
				awTlLastErr = AW_TLE_CANTOPENFILE;
				awTlLastWinErr = GetLastError();
				db_logf1(("AwCreateGraphic(): ERROR opening file \"%s\"",pParams->fileNameS));
				db_log2(AwTlErrorToString());
				return static_cast<D3DTexture *>(NULL);
			}
			
			SurfUnion textureP = pParams->DoCreate();
		
			CloseHandle(pParams->fileH);
			
			return textureP;
		}
		else return pParams->DoCreate();
	}

} // namespace AwTl

/******************************/
/* PUBLIC: AwSetTextureFormat */
/******************************/

#define IS_VALID_MEMBER(sP,mem) (reinterpret_cast<unsigned>(&(sP)->mem) - reinterpret_cast<unsigned>(sP) < static_cast<unsigned>((sP)->dwSize))

#if defined(__WATCOMC__) && (__WATCOMC__ <= 1100) // currently Watcom compiler crashes when the macro is expanded with the db_logf code in it
#define GET_VALID_MEMBER(sP,mem,deflt) (IS_VALID_MEMBER(sP,mem) ? (sP)->mem : (deflt))
#else
#define GET_VALID_MEMBER(sP,mem,deflt) (IS_VALID_MEMBER(sP,mem) ? (sP)->mem : (db_logf4((FUNCTION_NAME ": WARNING: %s->%s is not valid",#sP ,#mem )),(deflt)))
#endif

#define HANDLE_INITERROR(test,s) \
	if (!(test)) { \
		db_logf3((FUNCTION_NAME " failed becuse %s",s)); \
		db_log1(FUNCTION_NAME ": ERROR: unexpected parameters"); \
		return AW_TLE_BADPARMS; \
	} else { \
		db_logf5(("\t" FUNCTION_NAME " passed check '%s'",#test )); \
	}

#define FUNCTION_NAME "AwSetD3DDevice()"		

AW_TL_ERC AwSetD3DDevice(D3DDevice * _d3ddeviceP)
{
	using AwTl::driverDesc;
	
	driverDesc.validB = false;
	
	db_logf4(("AwSetD3DDevice(%p) called",_d3ddeviceP));
	
	HANDLE_INITERROR(_d3ddeviceP,"D3DDevice * is NULL")
	
	D3DDEVICEDESC hw_desc;
	D3DDEVICEDESC hel_desc;
	INITDXSTRUCT(hw_desc);
	INITDXSTRUCT(hel_desc);
	
	awTlLastDxErr = _d3ddeviceP->GetCaps(&hw_desc,&hel_desc);
	if (DD_OK != awTlLastDxErr)
	{
		db_logf2(("AwSetD3DDevice(): ERROR: %s",AwDxErrorToString()));
		return AW_TLE_DXERROR;
	}
	
	db_log4("Direct3D Device Hardware Capabilities:");
	db_code4(AwTl::LogCaps(&hw_desc);)
	db_log4("Direct3D Device Emulation Capabilities:");
	db_code4(AwTl::LogCaps(&hel_desc);)
	
	LPD3DDEVICEDESC descP = (GET_VALID_MEMBER(&hw_desc,dwFlags,0) & D3DDD_COLORMODEL && GET_VALID_MEMBER(&hw_desc,dcmColorModel,0) & (D3DCOLOR_RGB|D3DCOLOR_MONO)) ? &hw_desc : &hel_desc;
	db_logf4(("Direct3D Device is %s",&hw_desc==descP ? "HAL" : "emulation only"));
	
	HANDLE_INITERROR(GET_VALID_MEMBER(descP,dwFlags,0) & D3DDD_DEVCAPS && IS_VALID_MEMBER(descP,dwDevCaps),"LPD3DDEVICEDESC::dwDevCaps is not valid")
	HANDLE_INITERROR(descP->dwDevCaps & (D3DDEVCAPS_TEXTUREVIDEOMEMORY|D3DDEVCAPS_TEXTURENONLOCALVIDMEM|D3DDEVCAPS_TEXTURESYSTEMMEMORY),"Textures cannot be in ANY type of memory")
	
	driverDesc.memFlag = descP->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY ? DDSCAPS_SYSTEMMEMORY : DDSCAPS_VIDEOMEMORY;
	driverDesc.minWidth = GET_VALID_MEMBER(descP,dwMinTextureWidth,0);
	driverDesc.minHeight = GET_VALID_MEMBER(descP,dwMinTextureHeight,0);
	driverDesc.maxWidth = GET_VALID_MEMBER(descP,dwMaxTextureWidth,0);
	driverDesc.maxHeight = GET_VALID_MEMBER(descP,dwMaxTextureHeight,0);
	driverDesc.needPow2B = GET_VALID_MEMBER(descP,dpcTriCaps.dwTextureCaps,0) & D3DPTEXTURECAPS_POW2 ? true : false;
	driverDesc.needSquareB = GET_VALID_MEMBER(descP,dpcTriCaps.dwTextureCaps,0) & D3DPTEXTURECAPS_SQUAREONLY ? true : false;
	// if max w and h are 0, make them as large as possible
	if (!driverDesc.maxWidth) driverDesc.maxWidth = UINT_MAX;
	if (!driverDesc.maxHeight) driverDesc.maxHeight = UINT_MAX;
	
	db_log4("AwSetD3DDevice() OK");
	
	db_log4("Direct 3D Device texture characteristics follow:");
	db_logf4(("\tMinimum texture size: %u x %u",driverDesc.minWidth,driverDesc.minHeight));
	db_logf4(("\tMaximum texture size: %u x %u",driverDesc.maxWidth,driverDesc.maxHeight));
	db_logf4(("\tTextures %s be sqaure",driverDesc.needSquareB ? "must" : "need not"));
	db_logf4(("\tTextures %s be a power of two in width and height",driverDesc.needPow2B ? "must" : "need not"));
	db_logf4(("\tTextures can%s be in non-local video (AGP) memory",descP->dwDevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM ? "" : "not"));
	db_logf4(("\tTextures can%s be in local video (device) memory",descP->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY ? "" : "not"));
	db_logf4(("\tTextures can%s be in system memory",descP->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY ? "" : "not"));
	db_logf4(("\tTextures will be in %s memory",driverDesc.memFlag & DDSCAPS_SYSTEMMEMORY ? "system" : "video"));
	
	driverDesc.validB = true;
	
	return AW_TLE_OK;
}

AW_TL_ERC AwSetDDObject(DDObject * _ddP)
{
	using AwTl::driverDesc;
	
	db_logf4(("AwSetDDObject(%p) called.",_ddP));
	#ifdef DIRECTDRAW_VERSION
		db_logf4(("\tCompiled with DirectDraw Version %u.%u",DIRECTDRAW_VERSION/0x100U,DIRECTDRAW_VERSION%0x100U));
	#else
		db_log4("\tCompiled with unknown DirectDraw version");
	#endif
	
	
	HANDLE_INITERROR(_ddP,"DDObject * is NULL")
	driverDesc.ddP = _ddP;
	
	return AW_TLE_OK;
}

AW_TL_ERC AwSetD3DDevice(DDObject * _ddP, D3DDevice * _d3ddeviceP)
{
	db_logf4(("AwSetD3DDevice(%p,%p) called",_ddP,_d3ddeviceP));
	
	AW_TL_ERC iResult = AwSetDDObject(_ddP);
	
	if (AW_TLE_OK != iResult)
		return iResult;
	else
		return AwSetD3DDevice(_d3ddeviceP);
}

#undef FUNCTION_NAME
#define FUNCTION_NAME "AwSetPixelFormat()"
static AW_TL_ERC AwSetPixelFormat(AwTl::PixelFormat * _pfP, LPDDPIXELFORMAT _ddpfP)
{
	using AwTl::SetBitShifts;
	
	_pfP->validB = false;
	
	// parameter check
	HANDLE_INITERROR(_ddpfP,"DDPIXELFORMAT is NULL")
	HANDLE_INITERROR(IS_VALID_MEMBER(_ddpfP,dwFlags),"DDPIXELFORMAT::dwFlags is an invalid field")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_ALPHA),"DDPIXELFORMAT describes an alpha only surface")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_PALETTEINDEXEDTO8),"DDPIXELFORMAT describes a 1- 2- or 4- bit surface indexed to an 8-bit palette. This is not yet supported")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_ZBUFFER),"DDPIXELFORMAT describes Z buffer")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_ZPIXELS),"DDPIXELFORMAT describes a RGBZ surface")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_YUV),"DDPIXELFORMAT describes a YUV surface. This is not yet supported")
	HANDLE_INITERROR(!(_ddpfP->dwFlags & DDPF_FOURCC),"DDPIXELFORMAT gives a FourCC code for a non RGB surface. This is not yet supported")
	
	_pfP->palettizedB = true;
	switch (_ddpfP->dwFlags & (DDPF_PALETTEINDEXED8|DDPF_PALETTEINDEXED4|DDPF_PALETTEINDEXED2|DDPF_PALETTEINDEXED1))
	{
		case 0:
			_pfP->palettizedB = false;
			break;
		case DDPF_PALETTEINDEXED1:
			_pfP->bitsPerPixel = 1;
			break;
		case DDPF_PALETTEINDEXED2:
			_pfP->bitsPerPixel = 2;
			break;
		case DDPF_PALETTEINDEXED4:
			_pfP->bitsPerPixel = 4;
			break;
		case DDPF_PALETTEINDEXED8:
			_pfP->bitsPerPixel = 8;
			break;
		default:
			db_log1("AwSetPixelFormat(): ERROR: more than one DDPF_PALETTEINDEXED<n> flags is set");
			return AW_TLE_BADPARMS;
	}
	
	_pfP->alphaB = _ddpfP->dwFlags & DDPF_ALPHAPIXELS ? true : false;
	
	if (_pfP->palettizedB)
	{
		HANDLE_INITERROR(!_pfP->alphaB,"alpha channel info is on a palettized format. This is not yet supported")
		#if DB_LEVEL >= 4
		if (_ddpfP->dwFlags & DDPF_RGB)
		{
			db_log4(FUNCTION_NAME ": WARNING: RGB data supplied for a palettized format is ignored");
			db_logf4(("\tRGB bitcount is %u",GET_VALID_MEMBER(_ddpfP,dwRGBBitCount,0)));
			db_logf4(("\tRed Mask is 0x%08x",GET_VALID_MEMBER(_ddpfP,dwRBitMask,0)));
			db_logf4(("\tGreen Mask is 0x%08x",GET_VALID_MEMBER(_ddpfP,dwGBitMask,0)));
			db_logf4(("\tBlue Mask is 0x%08x",GET_VALID_MEMBER(_ddpfP,dwBBitMask,0)));
		}
		#endif
	}
	else
	{
		HANDLE_INITERROR(IS_VALID_MEMBER(_ddpfP,dwRGBBitCount),"DDPIXELFORMAT::dwRGBBitCount is an invalid field")
		switch (_ddpfP->dwRGBBitCount)
		{
			case 4:
			case 8:
			case 16:
			case 24:
			case 32:
				break;
			default:
				db_log1("AwSetPixelFormat(): ERROR: RGB bit count is not 4,8,16,24 or 32");
				return AW_TLE_BADPARMS;
		}
		
		HANDLE_INITERROR(!_pfP->alphaB || GET_VALID_MEMBER(_ddpfP,dwRGBAlphaBitMask,0),"Pixel format specifies alpha channel info but alpha mask is zero")
		HANDLE_INITERROR(IS_VALID_MEMBER(_ddpfP,dwRBitMask),"DDPIXELFORMAT::dwRBitMask is an invalid field")
		HANDLE_INITERROR(IS_VALID_MEMBER(_ddpfP,dwGBitMask),"DDPIXELFORMAT::dwGBitMask is an invalid field")
		HANDLE_INITERROR(IS_VALID_MEMBER(_ddpfP,dwBBitMask),"DDPIXELFORMAT::dwBBitMask is an invalid field")
		
		_pfP->bitsPerPixel = _ddpfP->dwRGBBitCount;
		SetBitShifts(&_pfP->redLeftShift,&_pfP->redRightShift,_ddpfP->dwRBitMask);
		SetBitShifts(&_pfP->greenLeftShift,&_pfP->greenRightShift,_ddpfP->dwGBitMask);
		SetBitShifts(&_pfP->blueLeftShift,&_pfP->blueRightShift,_ddpfP->dwBBitMask);
	}
	ZEROFILL(_pfP->ddpf);
	memcpy(&_pfP->ddpf,_ddpfP,__min(_ddpfP->dwSize,sizeof(DDPIXELFORMAT)));
	if (!_pfP->alphaB)
		_pfP->ddpf.dwRGBAlphaBitMask = 0;
		
	db_log4("AwSetPixelFormat() OK");
	
	#if DB_LEVEL >= 4
	db_logf4(("Pixel Format is %u-bit %s",_pfP->bitsPerPixel,_pfP->palettizedB ? "palettized" : _pfP->alphaB ? "RGBA" : "RGB"));
	if (!_pfP->palettizedB)
	{
		if (_pfP->alphaB) 
		{
			unsigned alpha_l_shft,alpha_r_shft;
			SetBitShifts(&alpha_l_shft,&alpha_r_shft,_pfP->ddpf.dwRGBAlphaBitMask);
			db_logf4(("\t%u-%u-%u-%u",8-_pfP->redRightShift,8-_pfP->greenRightShift,8-_pfP->blueRightShift,8-alpha_r_shft));
			db_logf4(("\tAlpha->[%u..%u]",alpha_l_shft+7-alpha_r_shft,alpha_l_shft));
		}
		else
		{
			db_logf4(("\t%u-%u-%u",8-_pfP->redRightShift,8-_pfP->greenRightShift,8-_pfP->blueRightShift));
		}
		db_logf4(("\tRed->[%u..%u]",_pfP->redLeftShift+7-_pfP->redRightShift,_pfP->redLeftShift));
		db_logf4(("\tGreen->[%u..%u]",_pfP->greenLeftShift+7-_pfP->greenRightShift,_pfP->greenLeftShift));
		db_logf4(("\tBlue->[%u..%u]",_pfP->blueLeftShift+7-_pfP->blueRightShift,_pfP->blueLeftShift));
	}
	#endif
	
	_pfP->validB = true;
	return AW_TLE_OK;
}

AW_TL_ERC AwSetTextureFormat2(LPDDPIXELFORMAT _ddpfP)
{
	db_logf4(("AwSetTextureFormat(%p) called",_ddpfP));
	
	using namespace AwTl;
	
	while (listTextureFormats.size())
		listTextureFormats.delete_first_entry();
	
	return AwSetPixelFormat(&pfTextureFormat, _ddpfP);
}

AW_TL_ERC AwSetAdditionalTextureFormat2(LPDDPIXELFORMAT _ddpfP, unsigned _maxAlphaBits, int _canDoTransp, unsigned _maxColours)
{
	db_logf4(("AwSetAdditionalTextureFormat(%p.%u,%d,%u) called",_ddpfP,_maxAlphaBits,_canDoTransp,_maxColours));
	
	using namespace AwTl;
	
	AdditionalPixelFormat pf;
	
	AW_TL_ERC erc = AwSetPixelFormat(&pf, _ddpfP);
	
	if (AW_TLE_OK == erc)
	{
		pf.canDoTranspB = _canDoTransp ? true : false;
		pf.maxColours = _maxColours;
		
		listTextureFormats.add_entry_end(pf);
	}
	
	return erc;
}

AW_TL_ERC AwSetSurfaceFormat2(LPDDPIXELFORMAT _ddpfP)
{
	db_logf4(("AwSetSurfaceFormat(%p) called",_ddpfP));
	
	using namespace AwTl;
	
	return AwSetPixelFormat(&pfSurfaceFormat, _ddpfP);
}

/****************************/
/* PUBLIC: AwGetTextureSize */
/****************************/

AW_TL_ERC AwGetTextureSize(register unsigned * _widthP, register unsigned * _heightP, unsigned _width, unsigned _height)
{
	db_assert1(_widthP);
	db_assert1(_heightP);
	
	using AwTl::driverDesc;
	
	if (!driverDesc.validB)
	{
		db_log3("AwGetTextureSize(): ERROR: driver description not valid");
		return AW_TLE_NOINIT;
	}
	
	if (_width < driverDesc.minWidth) _width = driverDesc.minWidth;
	if (_height < driverDesc.minHeight) _height = driverDesc.minHeight;
	
	if (driverDesc.needPow2B)
	{
		*_widthP = 1;
		while (*_widthP < _width) *_widthP <<= 1;
		*_heightP = 1;
		while (*_heightP < _height) *_heightP <<= 1;
	}
	else
	{
		*_widthP = _width;
		*_heightP = _height;
	}
	
	if (driverDesc.needSquareB)
	{
		if (*_widthP < *_heightP) *_widthP = *_heightP;
		else *_heightP = *_widthP;
	}
	
	#if 1 // not sure if this is required...
	*_widthP += 3;
	*_widthP &= ~3;
	*_heightP += 3;
	*_heightP &= ~3;
	#endif
	
	db_logf4(("\tAwGetTextureSize(): d3d texture will be %ux%u",*_widthP,*_heightP));
	
	if (*_widthP > driverDesc.maxWidth || *_heightP > driverDesc.maxHeight)
	{
		db_log3("AwGetTextureSize(): ERROR: image size too large to be a d3d texture");
		return AW_TLE_IMAGETOOLARGE;
	}
	else return AW_TLE_OK;
}


/******************************/
/* PUBLIC: AwCreate functions */
/******************************/

D3DTexture * _AWTL_VARARG AwCreateTexture(char const * _argFormatS, ...)
{
	db_logf4(("AwCreateTexture(\"%s\") called",_argFormatS));
	
	using namespace AwTl;
	
	va_list ap;
	va_start(ap,_argFormatS);
	CreateTextureParms parms;
	parms.loadTextureB = true;
	bool bParmsOK = ParseParams(&parms, _argFormatS, ap);
	va_end(ap);
	return bParmsOK ? LoadFromParams(&parms).textureP : NULL;
}

DDSurface * _AWTL_VARARG AwCreateSurface(char const * _argFormatS, ...)
{
	db_logf4(("AwCreateSurface(\"%s\") called",_argFormatS));
	
	using namespace AwTl;
	
	va_list ap;
	va_start(ap,_argFormatS);
	CreateTextureParms parms;
	parms.loadTextureB = false;
	bool bParmsOK = ParseParams(&parms, _argFormatS, ap);
	va_end(ap);
	return bParmsOK ? LoadFromParams(&parms).surfaceP : NULL;
}

AW_TL_ERC AwDestroyBackupTexture(AW_BACKUPTEXTUREHANDLE _bH)
{
	db_logf4(("AwDestroyBackupTexture(0x%08x) called",_bH));
	if (_bH)
	{
		_bH->Release();
		return AW_TLE_OK;
	}
	else
	{
		db_log1("AwDestroyBackupTexture(): ERROR: AW_BACKUPTEXTUREHANDLE==NULL");
		return AW_TLE_BADPARMS;
	}
}

/*********************************/
/* PUBLIC DEBUG: LastErr globals */
/*********************************/

AW_TL_ERC awTlLastErr;
HRESULT awTlLastDxErr;
DWORD awTlLastWinErr;

/*******************************************/
/* PUBLIC DEBUG: AwErrorToString functions */
/*******************************************/

#ifndef NDEBUG
char const * AwWinErrorToString(DWORD error)
{
	if (NO_ERROR==error) return "No error";
	static TCHAR buffer[1024];
	if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,error,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),buffer,sizeof buffer/sizeof(TCHAR)-1,NULL))
		wsprintf(buffer,TEXT("FormatMessage() failed; previous Windows error code: 0x%08X"),error);
	for (TCHAR * bufP = buffer; *bufP; ++bufP)
	{
		switch (*bufP)
		{
			case '\n':
			case '\r':
				*bufP=' ';
		}
	}
	return reinterpret_cast<char *>(buffer);
}

char const * AwTlErrorToString(AwTlErc error)
{
	char const * defaultS;
	switch (error)
	{
		case AW_TLE_OK:
			return "No error";
		case AW_TLE_DXERROR:
			if (DD_OK==awTlLastDxErr)
				return "Unknown DirectX error";
			else
				return AwDxErrorToString();
		case AW_TLE_BADPARMS:
			return "Invalid parameters or functionality not supported";
		case AW_TLE_NOINIT:
			return "Initialization failed or not performed";
		case AW_TLE_CANTOPENFILE:
			defaultS = "Unknown error opening file";
			goto WIN_ERR;
		case AW_TLE_CANTREADFILE:
			defaultS = "Unknown error reading file";
		WIN_ERR:
			if (NO_ERROR==awTlLastWinErr)
				return defaultS;
			else
				return AwWinErrorToString();
		case AW_TLE_EOFMET:
			return "Unexpected end of file during texture load";
		case AW_TLE_BADFILEFORMAT:
			return "Texture file format not recognized";
		case AW_TLE_BADFILEDATA:
			return "Texture file data not consistent";
		case AW_TLE_CANTPALETTIZE:
			return "Texture file data not palettized";
		case AW_TLE_IMAGETOOLARGE:
			return "Image is too large for a texture";
		case AW_TLE_CANTRELOAD:
			return "New image is wrong size or format to load into existing texture";
		default:
			return "Unknown texture loading error";
	}
}

char const * AwDxErrorToString(HRESULT error)
{
    switch(error) {
        case DD_OK:
            return "No error.\0";
        case DDERR_ALREADYINITIALIZED:
            return "This object is already initialized.\0";
        case DDERR_BLTFASTCANTCLIP:
            return "Return if a clipper object is attached to the source surface passed into a BltFast call.\0";
        case DDERR_CANNOTATTACHSURFACE:
            return "This surface can not be attached to the requested surface.\0";
        case DDERR_CANNOTDETACHSURFACE:
            return "This surface can not be detached from the requested surface.\0";
        case DDERR_CANTCREATEDC:
            return "Windows can not create any more DCs.\0";
        case DDERR_CANTDUPLICATE:
            return "Can't duplicate primary & 3D surfaces, or surfaces that are implicitly created.\0";
        case DDERR_CLIPPERISUSINGHWND:
            return "An attempt was made to set a cliplist for a clipper object that is already monitoring an hwnd.\0";
        case DDERR_COLORKEYNOTSET:
            return "No src color key specified for this operation.\0";
        case DDERR_CURRENTLYNOTAVAIL:
            return "Support is currently not available.\0";
        case DDERR_DIRECTDRAWALREADYCREATED:
            return "A DirectDraw object representing this driver has already been created for this process.\0";
        case DDERR_EXCEPTION:
            return "An exception was encountered while performing the requested operation.\0";
        case DDERR_EXCLUSIVEMODEALREADYSET:
            return "An attempt was made to set the cooperative level when it was already set to exclusive.\0";
        case DDERR_GENERIC:
            return "Generic failure.\0";
        case DDERR_HEIGHTALIGN:
            return "Height of rectangle provided is not a multiple of reqd alignment.\0";
        case DDERR_HWNDALREADYSET:
            return "The CooperativeLevel HWND has already been set. It can not be reset while the process has surfaces or palettes created.\0";
        case DDERR_HWNDSUBCLASSED:
            return "HWND used by DirectDraw CooperativeLevel has been subclassed, this prevents DirectDraw from restoring state.\0";
        case DDERR_IMPLICITLYCREATED:
            return "This surface can not be restored because it is an implicitly created surface.\0";
        case DDERR_INCOMPATIBLEPRIMARY:
            return "Unable to match primary surface creation request with existing primary surface.\0";
        case DDERR_INVALIDCAPS:
            return "One or more of the caps bits passed to the callback are incorrect.\0";
        case DDERR_INVALIDCLIPLIST:
            return "DirectDraw does not support the provided cliplist.\0";
        case DDERR_INVALIDDIRECTDRAWGUID:
            return "The GUID passed to DirectDrawCreate is not a valid DirectDraw driver identifier.\0";
        case DDERR_INVALIDMODE:
            return "DirectDraw does not support the requested mode.\0";
        case DDERR_INVALIDOBJECT:
            return "DirectDraw received a pointer that was an invalid DIRECTDRAW object.\0";
        case DDERR_INVALIDPARAMS:
            return "One or more of the parameters passed to the function are incorrect.\0";
        case DDERR_INVALIDPIXELFORMAT:
            return "The pixel format was invalid as specified.\0";
        case DDERR_INVALIDPOSITION:
            return "Returned when the position of the overlay on the destination is no longer legal for that destination.\0";
        case DDERR_INVALIDRECT:
            return "Rectangle provided was invalid.\0";
        case DDERR_LOCKEDSURFACES:
            return "Operation could not be carried out because one or more surfaces are locked.\0";
        case DDERR_NO3D:
            return "There is no 3D present.\0";
        case DDERR_NOALPHAHW:
            return "Operation could not be carried out because there is no alpha accleration hardware present or available.\0";
        case DDERR_NOBLTHW:
            return "No blitter hardware present.\0";
        case DDERR_NOCLIPLIST:
            return "No cliplist available.\0";
        case DDERR_NOCLIPPERATTACHED:
            return "No clipper object attached to surface object.\0";
        case DDERR_NOCOLORCONVHW:
            return "Operation could not be carried out because there is no color conversion hardware present or available.\0";
        case DDERR_NOCOLORKEY:
            return "Surface doesn't currently have a color key\0";
        case DDERR_NOCOLORKEYHW:
            return "Operation could not be carried out because there is no hardware support of the destination color key.\0";
        case DDERR_NOCOOPERATIVELEVELSET:
            return "Create function called without DirectDraw object method SetCooperativeLevel being called.\0";
        case DDERR_NODC:
            return "No DC was ever created for this surface.\0";
        case DDERR_NODDROPSHW:
            return "No DirectDraw ROP hardware.\0";
        case DDERR_NODIRECTDRAWHW:
            return "A hardware-only DirectDraw object creation was attempted but the driver did not support any hardware.\0";
        case DDERR_NOEMULATION:
            return "Software emulation not available.\0";
        case DDERR_NOEXCLUSIVEMODE:
            return "Operation requires the application to have exclusive mode but the application does not have exclusive mode.\0";
        case DDERR_NOFLIPHW:
            return "Flipping visible surfaces is not supported.\0";
        case DDERR_NOGDI:
            return "There is no GDI present.\0";
        case DDERR_NOHWND:
            return "Clipper notification requires an HWND or no HWND has previously been set as the CooperativeLevel HWND.\0";
        case DDERR_NOMIRRORHW:
            return "Operation could not be carried out because there is no hardware present or available.\0";
        case DDERR_NOOVERLAYDEST:
            return "Returned when GetOverlayPosition is called on an overlay that UpdateOverlay has never been called on to establish a destination.\0";
        case DDERR_NOOVERLAYHW:
            return "Operation could not be carried out because there is no overlay hardware present or available.\0";
        case DDERR_NOPALETTEATTACHED:
            return "No palette object attached to this surface.\0";
        case DDERR_NOPALETTEHW:
            return "No hardware support for 16 or 256 color palettes.\0";
        case DDERR_NORASTEROPHW:
            return "Operation could not be carried out because there is no appropriate raster op hardware present or available.\0";
        case DDERR_NOROTATIONHW:
            return "Operation could not be carried out because there is no rotation hardware present or available.\0";
        case DDERR_NOSTRETCHHW:
            return "Operation could not be carried out because there is no hardware support for stretching.\0";
        case DDERR_NOT4BITCOLOR:
            return "DirectDrawSurface is not in 4 bit color palette and the requested operation requires 4 bit color palette.\0";
        case DDERR_NOT4BITCOLORINDEX:
            return "DirectDrawSurface is not in 4 bit color index palette and the requested operation requires 4 bit color index palette.\0";
        case DDERR_NOT8BITCOLOR:
            return "DirectDrawSurface is not in 8 bit color mode and the requested operation requires 8 bit color.\0";
        case DDERR_NOTAOVERLAYSURFACE:
            return "Returned when an overlay member is called for a non-overlay surface.\0";
        case DDERR_NOTEXTUREHW:
            return "Operation could not be carried out because there is no texture mapping hardware present or available.\0";
        case DDERR_NOTFLIPPABLE:
            return "An attempt has been made to flip a surface that is not flippable.\0";
        case DDERR_NOTFOUND:
            return "Requested item was not found.\0";
        case DDERR_NOTLOCKED:
            return "Surface was not locked.  An attempt to unlock a surface that was not locked at all, or by this process, has been attempted.\0";
        case DDERR_NOTPALETTIZED:
            return "The surface being used is not a palette-based surface.\0";
        case DDERR_NOVSYNCHW:
            return "Operation could not be carried out because there is no hardware support for vertical blank synchronized operations.\0";
        case DDERR_NOZBUFFERHW:
            return "Operation could not be carried out because there is no hardware support for zbuffer blitting.\0";
        case DDERR_NOZOVERLAYHW:
            return "Overlay surfaces could not be z layered based on their BltOrder because the hardware does not support z layering of overlays.\0";
        case DDERR_OUTOFCAPS:
            return "The hardware needed for the requested operation has already been allocated.\0";
        case DDERR_OUTOFMEMORY:
            return "DirectDraw does not have enough memory to perform the operation.\0";
        case DDERR_OUTOFVIDEOMEMORY:
            return "DirectDraw does not have enough video memory to perform the operation.\0";
        case DDERR_OVERLAYCANTCLIP:
            return "The hardware does not support clipped overlays.\0";
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
            return "Can only have ony color key active at one time for overlays.\0";
        case DDERR_OVERLAYNOTVISIBLE:
            return "Returned when GetOverlayPosition is called on a hidden overlay.\0";
        case DDERR_PALETTEBUSY:
            return "Access to this palette is being refused because the palette is already locked by another thread.\0";
        case DDERR_PRIMARYSURFACEALREADYEXISTS:
            return "This process already has created a primary surface.\0";
        case DDERR_REGIONTOOSMALL:
            return "Region passed to Clipper::GetClipList is too small.\0";
        case DDERR_SURFACEALREADYATTACHED:
            return "This surface is already attached to the surface it is being attached to.\0";
        case DDERR_SURFACEALREADYDEPENDENT:
            return "This surface is already a dependency of the surface it is being made a dependency of.\0";
        case DDERR_SURFACEBUSY:
            return "Access to this surface is being refused because the surface is already locked by another thread.\0";
        case DDERR_SURFACEISOBSCURED:
            return "Access to surface refused because the surface is obscured.\0";
        case DDERR_SURFACELOST:
            return "Access to this surface is being refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have Restore called on it.\0";
        case DDERR_SURFACENOTATTACHED:
            return "The requested surface is not attached.\0";
        case DDERR_TOOBIGHEIGHT:
            return "Height requested by DirectDraw is too large.\0";
        case DDERR_TOOBIGSIZE:
            return "Size requested by DirectDraw is too large, but the individual height and width are OK.\0";
        case DDERR_TOOBIGWIDTH:
            return "Width requested by DirectDraw is too large.\0";
        case DDERR_UNSUPPORTED:
            return "Action not supported.\0";
        case DDERR_UNSUPPORTEDFORMAT:
            return "FOURCC format requested is unsupported by DirectDraw.\0";
        case DDERR_UNSUPPORTEDMASK:
            return "Bitmask in the pixel format requested is unsupported by DirectDraw.\0";
        case DDERR_VERTICALBLANKINPROGRESS:
            return "Vertical blank is in progress.\0";
        case DDERR_WASSTILLDRAWING:
            return "Informs DirectDraw that the previous Blt which is transfering information to or from this Surface is incomplete.\0";
        case DDERR_WRONGMODE:
            return "This surface can not be restored because it was created in a different mode.\0";
        case DDERR_XALIGN:
            return "Rectangle provided was not horizontally aligned on required boundary.\0";
        case D3DERR_BADMAJORVERSION:
            return "D3DERR_BADMAJORVERSION\0";
        case D3DERR_BADMINORVERSION:
            return "D3DERR_BADMINORVERSION\0";
        case D3DERR_EXECUTE_LOCKED:
            return "D3DERR_EXECUTE_LOCKED\0";
        case D3DERR_EXECUTE_NOT_LOCKED:
            return "D3DERR_EXECUTE_NOT_LOCKED\0";
        case D3DERR_EXECUTE_CREATE_FAILED:
            return "D3DERR_EXECUTE_CREATE_FAILED\0";
        case D3DERR_EXECUTE_DESTROY_FAILED:
            return "D3DERR_EXECUTE_DESTROY_FAILED\0";
        case D3DERR_EXECUTE_LOCK_FAILED:
            return "D3DERR_EXECUTE_LOCK_FAILED\0";
        case D3DERR_EXECUTE_UNLOCK_FAILED:
            return "D3DERR_EXECUTE_UNLOCK_FAILED\0";
        case D3DERR_EXECUTE_FAILED:
            return "D3DERR_EXECUTE_FAILED\0";
        case D3DERR_EXECUTE_CLIPPED_FAILED:
            return "D3DERR_EXECUTE_CLIPPED_FAILED\0";
        case D3DERR_TEXTURE_NO_SUPPORT:
            return "D3DERR_TEXTURE_NO_SUPPORT\0";
        case D3DERR_TEXTURE_NOT_LOCKED:
            return "D3DERR_TEXTURE_NOT_LOCKED\0";
        case D3DERR_TEXTURE_LOCKED:
            return "D3DERR_TEXTURE_LOCKED\0";
        case D3DERR_TEXTURE_CREATE_FAILED:
            return "D3DERR_TEXTURE_CREATE_FAILED\0";
        case D3DERR_TEXTURE_DESTROY_FAILED:
            return "D3DERR_TEXTURE_DESTROY_FAILED\0";
        case D3DERR_TEXTURE_LOCK_FAILED:
            return "D3DERR_TEXTURE_LOCK_FAILED\0";
        case D3DERR_TEXTURE_UNLOCK_FAILED:
            return "D3DERR_TEXTURE_UNLOCK_FAILED\0";
        case D3DERR_TEXTURE_LOAD_FAILED:
            return "D3DERR_TEXTURE_LOAD_FAILED\0";
        case D3DERR_MATRIX_CREATE_FAILED:
            return "D3DERR_MATRIX_CREATE_FAILED\0";
        case D3DERR_MATRIX_DESTROY_FAILED:
            return "D3DERR_MATRIX_DESTROY_FAILED\0";
        case D3DERR_MATRIX_SETDATA_FAILED:
            return "D3DERR_MATRIX_SETDATA_FAILED\0";
        case D3DERR_SETVIEWPORTDATA_FAILED:
            return "D3DERR_SETVIEWPORTDATA_FAILED\0";
        case D3DERR_MATERIAL_CREATE_FAILED:
            return "D3DERR_MATERIAL_CREATE_FAILED\0";
        case D3DERR_MATERIAL_DESTROY_FAILED:
            return "D3DERR_MATERIAL_DESTROY_FAILED\0";
        case D3DERR_MATERIAL_SETDATA_FAILED:
            return "D3DERR_MATERIAL_SETDATA_FAILED\0";
        case D3DERR_LIGHT_SET_FAILED:
            return "D3DERR_LIGHT_SET_FAILED\0";
        #if 0 // retained mode error codes
        case D3DRMERR_BADOBJECT:
            return "D3DRMERR_BADOBJECT\0";
        case D3DRMERR_BADTYPE:
            return "D3DRMERR_BADTYPE\0";
        case D3DRMERR_BADALLOC:
            return "D3DRMERR_BADALLOC\0";
        case D3DRMERR_FACEUSED:
            return "D3DRMERR_FACEUSED\0";
        case D3DRMERR_NOTFOUND:
            return "D3DRMERR_NOTFOUND\0";
        case D3DRMERR_NOTDONEYET:
            return "D3DRMERR_NOTDONEYET\0";
        case D3DRMERR_FILENOTFOUND:
            return "The file was not found.\0";
        case D3DRMERR_BADFILE:
            return "D3DRMERR_BADFILE\0";
        case D3DRMERR_BADDEVICE:
            return "D3DRMERR_BADDEVICE\0";
        case D3DRMERR_BADVALUE:
            return "D3DRMERR_BADVALUE\0";
        case D3DRMERR_BADMAJORVERSION:
            return "D3DRMERR_BADMAJORVERSION\0";
        case D3DRMERR_BADMINORVERSION:
            return "D3DRMERR_BADMINORVERSION\0";
        case D3DRMERR_UNABLETOEXECUTE:
            return "D3DRMERR_UNABLETOEXECUTE\0";
        #endif
        default:
            return "Unrecognized error value.\0";
    }
}
#endif
