/*******************************************************************
 *
 *    DESCRIPTION: 	string.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created ages ago
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "scstring.hpp"

#if SupportHHStuff
	#include "hhfonts.h"
	#include "hhfile.h"
	#include "ifmisc.h"
	#include "holder.hpp"
	#include "mcstring.hpp"
#else
	#include "strutil.h"
	#include "indexfnt.hpp"
#endif

	#if TrackReferenceCounted
	#include "dcontext.hpp"
	#endif

	#define UseLocalAssert Yes
	#include "ourasert.h"

#ifdef __WATCOMC__
#pragma warning 139 5
#pragma message("Disabled Warning W139")
#endif


/* Version settings ************************************************/
	#define LogStringTables	No

/* Constants *******************************************************/
	#define MAX_BYTES_IN_NUMERIC_STRING	(100)

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern DISPLAYBLOCK *Player;

	#if 0
	extern OurBool			DaveDebugOn;
	extern FDIEXTENSIONTAG	FDIET_Dummy;
	extern IFEXTENSIONTAG	IFET_Dummy;
	extern FDIQUAD			FDIQuad_WholeScreen;
	extern FDIPOS			FDIPos_Origin;
	extern IFOBJECTLOCATION IFObjLoc_Origin;
	extern UncompressedGlobalPlotAtomID UGPAID_StandardNull;
	#endif
#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/
	/*static*/ SCString* SCString :: pFirst = NULL;

/* Internal type definitions ***************************************/

/* Internal function prototypes ************************************/
	#if SupportHHStuff
	static void strutil_Failure(Failure_Behaviour FailBehav);
	#endif

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
// class SCString
// public:
#if TrackReferenceCounted
void SCString :: DumpIDForReferenceDump(R_DumpContext& theContext) const
{
	theContext . dprintf("SCString \"%s\" refs remainining=%i\n",pProjCh_Val,CheckRef());
}
#endif

ProjChar* SCString :: pProjCh(void) const
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_Val );
	}

	/* CODE */
	{
		return pProjCh_Val;
	}
}


SCString :: SCString
(
	const ProjChar* pProjCh_Init
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_Init );
	}

	/* CODE */
	{
		AllocatedSize = (size_t) STRUTIL_SC_NumBytes
		(
			pProjCh_Init
		);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );

		STRUTIL_SC_StrCpy
		(
			pProjCh_Val,
			pProjCh_Init
		);

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif
					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif
					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}
	}
}

SCString :: SCString
(
	signed int Number
)
{
	// forms a new string object that describes the number passed
	// standard decimal representation

	/* PRECONDITION */
	{
	}

	/* CODE */
	{
		ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

		sprintf
		(
			pProjCh_Init,
			"%i",
			(int)Number
		);

		#if 0
		LOCALISEME();
		#endif

		AllocatedSize = (size_t) STRUTIL_SC_NumBytes
		(
			pProjCh_Init
		);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );
			// this is always "owned" by the String

		STRUTIL_SC_StrCpy
		(
			pProjCh_Val,
			pProjCh_Init
		);

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif

					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif
					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

	}
}

SCString :: SCString
(
	unsigned int Number
)
{
	// forms a new string object that describes the number passed
	// standard decimal representation

	/* PRECONDITION */
	{
	}

	/* CODE */
	{
		ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

		sprintf
		(
			pProjCh_Init,
			"%u",
			Number
		);

		#if 0
		LOCALISEME();
		#endif

		AllocatedSize = (size_t) STRUTIL_SC_NumBytes
		(
			pProjCh_Init
		);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );
			// this is always "owned" by the String

		STRUTIL_SC_StrCpy
		(
			pProjCh_Val,
			pProjCh_Init
		);

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif

					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif
					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

	}
}

SCString :: SCString
(
	float Number
)
{
	/* PRECONDITION */
	{
	}

	/* CODE */
	{
		ProjChar pProjCh_Init[ MAX_BYTES_IN_NUMERIC_STRING ];

		sprintf
		(
			pProjCh_Init,
			"%6f",
			Number
		);

		#if 0
		LOCALISEME();
		#endif

		AllocatedSize = (size_t) STRUTIL_SC_NumBytes
		(
			pProjCh_Init
		);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );
			// this is always "owned" by the String

		STRUTIL_SC_StrCpy
		(
			pProjCh_Val,
			pProjCh_Init
		);

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif

					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif
					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

	}
}



SCString :: SCString
(
	ProjChar* pProjCh_Init,
	unsigned int Length
)
{
	// Forms a string of length at most Length (with 1 extra for NULL-terminator)

	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_Init );
	}

	/* CODE */
	{
		NumberOfCharacters = STRUTIL_SC_Strlen( pProjCh_Init );
			// doesn't include NULL terminator

		if ( NumberOfCharacters > Length )
		{
			NumberOfCharacters = Length;
		}		

		AllocatedSize = (size_t) STRUTIL_SC_NumBytes
		(
			pProjCh_Init
		);

		{
			size_t TruncSize = sizeof(ProjChar) * (Length + 1);

			if (AllocatedSize > TruncSize )
			{
				AllocatedSize = TruncSize;
			}
		}

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );
			// this is always "owned" by the String

		STRUTIL_SC_SafeCopy
		(
			pProjCh_Val,
			(NumberOfCharacters+1),

			pProjCh_Init
		);

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif

					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif

					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

	}
}


SCString :: SCString
(
	SCString* pStringObj_0,
	SCString* pStringObj_1
)
{
	// forms a new string object by concatenating the strings in 0
	// and 1

	/* PRECONDITION */
	{
		GLOBALASSERT( pStringObj_0 );
		GLOBALASSERT( pStringObj_1 );
	}

	/* CODE */
	{
		ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
		GLOBALASSERT( pProjCh_Init_0 );
		
		ProjChar* pProjCh_Init_1 = pStringObj_1 -> pProjCh();
		GLOBALASSERT( pProjCh_Init_1 );
		
		AllocatedSize = (size_t)
		(
			STRUTIL_SC_NumBytes
			(
				pProjCh_Init_0
			)
			+
			STRUTIL_SC_NumBytes
			(
				pProjCh_Init_1
			)
			- sizeof(ProjChar)	// only one null terminator needed				
		);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );

			// this is always "owned" by the String
		STRUTIL_SC_FastCat
		(
			pProjCh_Val,
			pProjCh_Init_0,
			pProjCh_Init_1
		);

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				#if EnableSizeData
				R2Size[ i ] = pStringObj_0 -> CalcSize( i );
				R2Size[ i ] . w += pStringObj_1 -> CalcSize( i ) . w;
				#endif

				bCanRender[ i ] =
				(
					pStringObj_0 -> bCanRenderFully( i )
					&&
					pStringObj_1 -> bCanRenderFully( i )
				);				
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

	}
}


SCString :: SCString
(
	SCString* pStringObj_0,
	SCString* pStringObj_1,
	SCString* pStringObj_2
)
{
	// forms a new string object by concatenating the strings in 0, 1 and 2

	/* PRECONDITION */
	{
		GLOBALASSERT( pStringObj_0 );
		GLOBALASSERT( pStringObj_1 );
		GLOBALASSERT( pStringObj_2 );
	}

	/* CODE */
	{
		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

		SCString* pStringObj_Intermediate = new SCString
		(
			pStringObj_1,
			pStringObj_2		
		);
		
		{

			ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
			GLOBALASSERT( pProjCh_Init_0 );
			
			ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
			GLOBALASSERT( pProjCh_Intermediate );
			
			AllocatedSize = (size_t)
			(
				STRUTIL_SC_NumBytes
				(
					pProjCh_Init_0
				)
				+
				STRUTIL_SC_NumBytes
				(
					pProjCh_Intermediate
				)
				- sizeof(ProjChar)	// only one null terminator needed				
			);

			pProjCh_Val = new ProjChar[	AllocatedSize ];
			GLOBALASSERT( pProjCh_Val );
				// this is always "owned" by the String
			STRUTIL_SC_FastCat
			(
				pProjCh_Val,
				pProjCh_Init_0,
				pProjCh_Intermediate
			);

			NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

			#if SupportHHStuff
			{
				FDIBMAP FDIBMap_Temp;

				HHFONTS_GetSizeOfSingleLine
				(
					&FDIBMap_Temp,

					pProjCh_Val,
					pStandardFont
				);

				WidthGivenStandardFont = FDIBMap_Temp . Width;
					// width in pixels, to save constant recalc
			}
			#else
			{
				FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
				
				while ( i>0 )
				{
					i = (FontIndex)(i-1);

					#if EnableSizeData
					R2Size[ i ] = pStringObj_0 -> CalcSize( i );
					R2Size[ i ] . w +=
					(
						pStringObj_1 -> CalcSize( i ) . w
						+
						pStringObj_2 -> CalcSize( i ) . w
					);
					#endif

					bCanRender[ i ] =
					(
						pStringObj_0 -> bCanRenderFully( i )
						&&
						pStringObj_1 -> bCanRenderFully( i )
						&&
						pStringObj_2 -> bCanRenderFully( i )
					);
				}
			}
			#endif
		}

		
		pStringObj_Intermediate -> R_Release();

	}
}

SCString :: SCString
(
	SCString* pStringObj_0,
	SCString* pStringObj_1,
	SCString* pStringObj_2,
	SCString* pStringObj_3
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pStringObj_0 );
		GLOBALASSERT( pStringObj_1 );
		GLOBALASSERT( pStringObj_2 );
		GLOBALASSERT( pStringObj_3 );
	}

	/* CODE */
	{
		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

		SCString* pStringObj_Intermediate = new SCString
		(
			pStringObj_1,
			pStringObj_2,
			pStringObj_3
		);
		
		{

			ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
			GLOBALASSERT( pProjCh_Init_0 );
			
			ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
			GLOBALASSERT( pProjCh_Intermediate );
			
			AllocatedSize = (size_t)
			(
				STRUTIL_SC_NumBytes
				(
					pProjCh_Init_0
				)
				+
				STRUTIL_SC_NumBytes
				(
					pProjCh_Intermediate
				)
				- sizeof(ProjChar)	// only one null terminator needed				
			);

			pProjCh_Val = new ProjChar[	AllocatedSize ];
			GLOBALASSERT( pProjCh_Val );
				// this is always "owned" by the String
			STRUTIL_SC_FastCat
			(
				pProjCh_Val,
				pProjCh_Init_0,
				pProjCh_Intermediate
			);

			NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

			#if SupportHHStuff
			{
				FDIBMAP FDIBMap_Temp;

				HHFONTS_GetSizeOfSingleLine
				(
					&FDIBMap_Temp,

					pProjCh_Val,
					pStandardFont
				);

				WidthGivenStandardFont = FDIBMap_Temp . Width;
					// width in pixels, to save constant recalc
			}
			#else
			{
				FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
				
				while ( i>0 )
				{
					i = (FontIndex)(i-1);

					#if EnableSizeData
					R2Size[ i ] = pStringObj_0 -> CalcSize( i );
					R2Size[ i ] . w +=
					(
						pStringObj_1 -> CalcSize( i ) . w
						+
						pStringObj_2 -> CalcSize( i ) . w
						+
						pStringObj_3 -> CalcSize( i ) . w
					);
					#endif

					bCanRender[ i ] =
					(
						pStringObj_0 -> bCanRenderFully( i )
						&&
						pStringObj_1 -> bCanRenderFully( i )
						&&
						pStringObj_2 -> bCanRenderFully( i )
						&&
						pStringObj_3 -> bCanRenderFully( i )
					);
				}
			}
			#endif
		}

		
		pStringObj_Intermediate -> R_Release();

	}
}


SCString :: SCString
(
	SCString* pStringObj_0,
	SCString* pStringObj_1,
	SCString* pStringObj_2,
	SCString* pStringObj_3,
	SCString* pStringObj_4
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pStringObj_0 );
		GLOBALASSERT( pStringObj_1 );
		GLOBALASSERT( pStringObj_2 );
		GLOBALASSERT( pStringObj_3 );
		GLOBALASSERT( pStringObj_4 );
	}

	/* CODE */
	{
		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}

		SCString* pStringObj_Intermediate = new SCString
		(
			pStringObj_1,
			pStringObj_2,
			pStringObj_3,
			pStringObj_4
		);
		
		{

			ProjChar* pProjCh_Init_0 = pStringObj_0 -> pProjCh();
			GLOBALASSERT( pProjCh_Init_0 );
			
			ProjChar* pProjCh_Intermediate = pStringObj_Intermediate -> pProjCh();
			GLOBALASSERT( pProjCh_Intermediate );
			
			AllocatedSize = (size_t)
			(
				STRUTIL_SC_NumBytes
				(
					pProjCh_Init_0
				)
				+
				STRUTIL_SC_NumBytes
				(
					pProjCh_Intermediate
				)
				- sizeof(ProjChar)	// only one null terminator needed				
			);

			pProjCh_Val = new ProjChar[	AllocatedSize ];
			GLOBALASSERT( pProjCh_Val );
				// this is always "owned" by the String
			STRUTIL_SC_FastCat
			(
				pProjCh_Val,
				pProjCh_Init_0,
				pProjCh_Intermediate
			);

			NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

			#if SupportHHStuff
			{
				FDIBMAP FDIBMap_Temp;

				HHFONTS_GetSizeOfSingleLine
				(
					&FDIBMap_Temp,

					pProjCh_Val,
					pStandardFont
				);

				WidthGivenStandardFont = FDIBMap_Temp . Width;
					// width in pixels, to save constant recalc
			}
			#else
			{
				FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
				
				while ( i>0 )
				{
					i = (FontIndex)(i-1);

					#if EnableSizeData
					R2Size[ i ] = pStringObj_0 -> CalcSize( i );
					R2Size[ i ] . w +=
					(
						pStringObj_1 -> CalcSize( i ) . w
						+
						pStringObj_2 -> CalcSize( i ) . w
						+
						pStringObj_3 -> CalcSize( i ) . w
						+
						pStringObj_4 -> CalcSize( i ) . w
					);
					#endif

					bCanRender[ i ] =
					(
						pStringObj_0 -> bCanRenderFully( i )
						&&
						pStringObj_1 -> bCanRenderFully( i )
						&&
						pStringObj_2 -> bCanRenderFully( i )
						&&
						pStringObj_3 -> bCanRenderFully( i )
						&&
						pStringObj_4 -> bCanRenderFully( i )
					);
				}
			}
			#endif
		}

		
		pStringObj_Intermediate -> R_Release();

	}
}


SCString :: SCString
(
	List<ProjChar> List_ProjChar
)
{
	{
		AllocatedSize = (size_t) (List_ProjChar . size() + 1) * sizeof(ProjChar);

		pProjCh_Val = new ProjChar[	AllocatedSize ];
		GLOBALASSERT( pProjCh_Val );

		#if 1
		{
			ProjChar* pDst = pProjCh_Val;

			for
			(
				LIF<ProjChar> oi(&(List_ProjChar));
				!oi.done();
				oi.next()
			)
			{
				*(pDst++) = oi();
			}

			// Write terminator:
			*pDst = 0;
		}
		#else
		STRUTIL_SC_StrCpy
		(
			pProjCh_Val,
			pProjCh_Init
		);
		#endif

		NumberOfCharacters = ( AllocatedSize / sizeof(ProjChar ) ) - 1;
			// doesn't include NULL terminator

		#if SupportHHStuff
		{
			FDIBMAP FDIBMap_Temp;

			HHFONTS_GetSizeOfSingleLine
			(
				&FDIBMap_Temp,

				pProjCh_Val,
				pStandardFont
			);

			WidthGivenStandardFont = FDIBMap_Temp . Width;
				// width in pixels, to save constant recalc
		}
		#else
		{
			FontIndex i = IndexedFonts_MAX_NUMBER_OF_FONTS; 
			
			while ( i>0 )
			{
				i = (FontIndex)(i-1);

				IndexedFont* pFont = IndexedFont :: GetFont( i );
				
				if ( pFont )
				{
					#if EnableSizeData
					R2Size[ i ] = pFont -> CalcSize
					(
						pProjCh_Val
					);
					#endif
					bCanRender[ i ] = pFont -> bCanRenderFully
					(
						pProjCh_Val
					);
				}
				else
				{
					#if EnableSizeData
					R2Size[ i ] = r2size(0,0);
					#endif
					bCanRender[ i ] = No;					
				}
			}
		}
		#endif

		// Insert at head of list:
		{
			if ( pFirst )
			{
				pFirst -> pPrv = this;
			}
			
			pNxt = pFirst;
			pPrv = NULL;

			pFirst = this;
		}
	}
	
}


#if SupportHHStuff
void SCString :: GetSizeOfSingleLineGivenStandardFont
(
	FDIBMAP* pFDIBMap_Out
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pFDIBMap_Out );
	}

	/* CODE */
	{
		pFDIBMap_Out -> Width = WidthGivenStandardFont;

		pFDIBMap_Out -> Height = HHFONTS_GetMaxHeight
		(
			pStandardFont
		);
	}
}

FDIBMAP SCString :: GetSizeOfSingleLineGivenStandardFont(void)
{
	/* PRECONDITION */
	{
	}

	/* CODE */
	{
		FDIBMAP FDIBMap_Temp;

		FDIBMap_Temp . Width = WidthGivenStandardFont;

		FDIBMap_Temp . Height = HHFONTS_GetMaxHeight
		(
			pStandardFont
		);

		return FDIBMap_Temp;
	}
}

int SCString :: GetWidthOfSingleLineGivenStandardFont(void)
{
	return WidthGivenStandardFont;
}

FDIQUAD SCString :: MinFDIQuadForSCStringAtPos
(
	FDIPOS FDIPos
)
{
	FDIQUAD FDIQuad_Temp = FDIQuad
	(
		FDIPos . LeftX,
		FDIPos . TopY,
		WidthGivenStandardFont,
		HHFONTS_GetMaxHeight
		(
			pStandardFont
		)
	);

	return FDIQuad_Temp;
}
#endif

/*static*/ void SCString :: UpdateAfterFontChange( FontIndex I_Font_Changed )
{
	// called by the font code whenever fonts are loaded/unloaded

	/* PRECONDITION */
	{
		GLOBALASSERT( I_Font_Changed < IndexedFonts_MAX_NUMBER_OF_FONTS );
	}

	/* CODE */
	{
		IndexedFont* pFont = IndexedFont :: GetFont( I_Font_Changed );

		SCString* pSCString = pFirst;

		while ( pSCString )
		{
			if ( pFont )
			{
				#if EnableSizeData
				pSCString -> R2Size[ I_Font_Changed ] = pFont -> CalcSize
				(
					pSCString -> pProjCh_Val
				);
				#endif

				pSCString -> bCanRender[ I_Font_Changed ] = pFont -> bCanRenderFully
				(
					pSCString -> pProjCh_Val
				);
			}
			else
			{
				#if EnableSizeData
				pSCString -> R2Size[ I_Font_Changed ] = r2size(0,0);
				#endif

				pSCString -> bCanRender[ I_Font_Changed ] = No;					
			}
			
			pSCString = pSCString -> pNxt;
		}
	}
}

/*static*/ List<SCString*> SCString :: Parse
(
	ProjChar* pProjChar_Start
)
{
	// takes a string and builds a list of new SCStrings, in which
	// each string in the list consists of non-whitespace characters from
	// the input, and the whitespace is used to separate individual
	// strings

	// I call the strings "words"

	/* PRECONDITION */
	{
		GLOBALASSERT( pProjChar_Start );
	}

	/* CODE */
	{
		List<SCString*> List_Return;

		ProjChar* pProjChar_Iterate = pProjChar_Start;
		int NumCharsNonWhitespace = 0;

		while
		(
			*pProjChar_Iterate
		)
		{
			if
			(
				*pProjChar_Iterate == ' '
			)
			{
				// Whitespace:
				if ( NumCharsNonWhitespace > 0 )
				{
					// End of a word; add the string to the list:
					List_Return . add_entry
					(
						new SCString
						(
							pProjChar_Start,
							NumCharsNonWhitespace
						)
					);
					NumCharsNonWhitespace = 0;
				}
				else
				{
					// Already processing a block of whitespace; do nothing
				}
			}
			else
			{
				// Non-whitespace:
				if ( NumCharsNonWhitespace > 0 )
				{
					// In the middle of a word:
				}
				else
				{
					// Start of a word:
					pProjChar_Start = pProjChar_Iterate;
				}

				NumCharsNonWhitespace++;

			}

			pProjChar_Iterate++;
		}

		// End of the string; flush any remaining whitespace:
		if ( NumCharsNonWhitespace > 0 )
		{
			List_Return . add_entry
			(
				new SCString
				(
					pProjChar_Start,
					NumCharsNonWhitespace
				)
			);
		}

		return List_Return;
	}
}


//private:
SCString :: ~SCString()
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pProjCh_Val );
	}

	/* CODE */
	{
		delete[] pProjCh_Val;

		// Remove from list:
		{
			if ( pFirst == this )
			{
				pFirst = pNxt;
			}
			else
			{
				pPrv -> pNxt = pNxt;
			}

			if (pNxt)
			{
				pNxt -> pPrv = pPrv;
			}			
		}

	}
}

#if SupportHHStuff
HHStringTable* STRUTIL_LoadStringTable(char* Filename,Failure_Behaviour FailBehav)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(Filename);
	}
	/* CODE */
	{
		/* Allocate memory for a table of pointers to strings in memory */
		HHStringTable* pHHST= (HHStringTable*)AllocateMem
		(
			sizeof(HHStringTable)
		);

		if (!pHHST)
		{
			/* Out of memory */
						
			return 0;
		}
		else
		{
		
			/* Load the file into a buffer in memory */
			OurByteBuffer ByteBuf;
			OurByte* pTextBuffer;

			{
				/* clear the pointers in the new string table */
				int i = MAX_ENTRIES_PER_STRING_TABLE;
				while (i>0)
				{
					i--;

					pHHST -> pStringObj[i] = NULL;
				}

			}
			
			HHFILE_LoadLogicalFile
			(
				Filename,
				FailBehav,
				&ByteBuf
			);

			pTextBuffer=ByteBuf.pbData;
			
			if (!pTextBuffer)
			{
				/* Error loading file */
				return 0;
			}
			else
			{
				/* Build table of string pointers in the buffer */

				#define START_OF_COMMENT_TC 	('#')
				#define END_OF_COMMENT_TC		(0x0D)
				#define SKIP_TC					(0x0A)

				/* 
					Scan through loaded buffer, finding strings, making copies with NULL termination, 
					until you reach the end (can be signified by EOF or by two successive # characters
					without intermediate end-of-comment chars)

					When done, free up loaded buffer.
				*/

				OurByte* pbExtract=pTextBuffer;
				unsigned int CountChars=0;
				OurBool StillGoing=Yes;

				OurBool FoundStartChar=No;
				OurBool FoundEndChar=No;
				ProjChar*	pChStart=NULL;
				unsigned int CountSinceEndChar=0;
				unsigned int SkipChars=0;

				pHHST->NumEntries_Val=0;

				while
				(
					(CountChars<(ByteBuf.NumBytes))
					&&
					(StillGoing)
				)
				{
				ProjChar TChar=(ProjChar)*pbExtract;

					switch (TChar)
					{
						case SKIP_TC:
							/* Skip this char */
							{
								SkipChars++;
							}
							break;

						case START_OF_COMMENT_TC:
							if (FoundStartChar)
							{
								/* Two successive StartOf.. TChars without a EndOf... are a termination sequence */
								StillGoing=No;
							}
							else
							{
								/* It is the start of a comment */
								FoundStartChar=Yes;
							}
							break;
						case END_OF_COMMENT_TC:
							if (FoundEndChar)
							{
								/* This is the terminating character of a string; copy it up */
								LOCALASSERT(pChStart);
								
								/* copy it up here...*/
								#if LimitedStringLengths
								LOCALASSERT(CountSinceEndChar<MAX_STRING_LENGTH);
								#endif

								{
									ProjChar* NewString=(ProjChar*)AllocateMem((CountSinceEndChar+1)*sizeof(ProjChar));

									if (!NewString)
									{
										/* Out of memory... */
										strutil_Failure(FailBehav);

									}
									else
									{
										/* Copy up chars, except for SKIP_TC... */
									ProjChar*	pSrc=pChStart;
									ProjChar*	pDst=NewString;

										int i;
										for (i=0;i<(CountSinceEndChar+SkipChars);i++)
										{
										ProjChar ChSrc=*(pSrc++);

											if (ChSrc!=SKIP_TC)
											{
												*(pDst++)=ChSrc;
											}
										}

										STRUTIL_SC_WriteTerminator(pDst);

										GLOBALASSERT(pHHST->NumEntries() < MAX_ENTRIES_PER_STRING_TABLE);
										
										#if 1
										pHHST->pStringObj[ (pHHST->NumEntries_Val++) ] = new SCString
										(
											NewString
										); // you get an automatic reference when you construct it.

										DeallocateMem( NewString );

										#else
										pHHST->pEntry[(pHHST->NumEntries_Val++)]=NewString;
										#endif

										
									}
								}

								FoundEndChar=No;
								FoundStartChar=No;
								CountSinceEndChar=0;
								SkipChars=0;
								pChStart=NULL;
							}
							else
							{
								if (FoundStartChar)
								{
									/* Then this signifies the end of a comment and the start of the string */
									FoundEndChar=Yes;
									CountSinceEndChar=0;
									SkipChars=0;
									
									/* Start extracting string from next character onwards...*/
									pChStart=(pbExtract+1);
								}
								/* else it is padding; ignore */

							}
							
							break;
						default:
							/* It is a normal character...*/
							if (FoundEndChar)
							{
								CountSinceEndChar++;
							}

							break;
					}						

					/* On to next character */
					pbExtract++;	
					CountChars++;
				}

		
				DeallocateMem(pTextBuffer);
				
				#if (debug  && LogStringTables )
				{
					int i;
					char Temp[200];
	
					sprintf(Temp,"Loaded string table:\"%s\"",Filename);
					DAVELOG(Temp);

					sprintf(Temp,"Number of strings=%i",pHHST->NumEntries);
					DAVELOG(Temp);

					for (i=0;i<pHHST->NumEntries();i++)
					{
						GLOBALASSERT(pHHST->pStringObj[i]);

						if 
						(
							STRUTIL_SC_Strlen
							(
								pHHST -> pStringObj[i] -> pProjCh()
							) < 100
						)
						{
							sprintf
							(
								Temp,
								"Entry %3i=\"%s\"",
								i,
								pHHST -> pStringObj[i] -> pProjCh()
							);
							DAVELOG(Temp);
						}
						else
						{
							DAVELOG("String too long to log");
						}
					}
					sprintf(Temp,"End of string table:\"%s\"",Filename);
					DAVELOG(Temp);
				}				
				#endif
				
				// Clear the lazy evaluation table:
				{
					MCSCString** ppMCStringObj_Clear = &(pHHST -> pMCStringObj[ 0 ][ 0 ]);

					for ( int i=pHHST->NumEntries(); i>0; i--)
					{
						for ( int j = NUM_BASE_COLOURS; j > 0; j-- )
						{
							*(ppMCStringObj_Clear++) = NULL;
						}
					}
				}

				/* Return a pointer to this table */			
				return pHHST;
			}
		}
	}
}

void STRUTIL_UnloadStringTable(HHStringTable* pHHST)
/* This function DOES deallocate the passed structure */
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pHHST);
	}
	/* CODE */
	{
		/* Kill off strings in table */
		int i;

		for (i=0;i<pHHST->NumEntries();i++)
		{
			#if 1
			LOCALASSERT( pHHST->pStringObj[i] );
			pHHST -> pStringObj[i] -> R_Release();
			#else
			LOCALASSERT(pHHST->pEntry[i]);
			DeallocateMem(pHHST->pEntry[i]);
			#endif
		}

		// R_Release any refs in the lazy evaluation table:
		{
			for (i=0;i<pHHST->NumEntries();i++)
			{
				for ( int j=0; j< NUM_BASE_COLOURS; j++ )
				{
					if ( pHHST -> pMCStringObj[i][j] )
					{
						pHHST -> pMCStringObj[ i ][ j ] -> R_Release();
					}
				}
			}
		}

		/* Kill off table of pointers */
		DeallocateMem(pHHST);
	}
}

ProjChar* STRUTIL_SC_GetString(HHStringTable* pHHST,unsigned int StrNum)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pHHST);
		GLOBALASSERT(StrNum<pHHST->NumEntries());
		GLOBALASSERT(StrNum< MAX_ENTRIES_PER_STRING_TABLE);
	}
	/* CODE */
	{
		GLOBALASSERT( pHHST -> pStringObj[StrNum] );
		GLOBALASSERT( pHHST -> pStringObj[StrNum] -> pProjCh() );

		return pHHST -> pStringObj[StrNum] -> pProjCh();
	}
}

unsigned int STRUTIL_GetNumEntries
(
	HHStringTable* pHHST
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pHHST);
	}
	/* CODE */
	{
		unsigned int ReturnVal = pHHST -> NumEntries();
		GLOBALASSERT( ReturnVal <= MAX_ENTRIES_PER_STRING_TABLE);

		return ReturnVal;
	}
}

SCString* STRUTIL_GetSCString
(
	HHStringTable* pHHST,
	unsigned int StrNum
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(pHHST);
		GLOBALASSERT(StrNum<pHHST->NumEntries());
		GLOBALASSERT(StrNum< MAX_ENTRIES_PER_STRING_TABLE);
	}
	/* CODE */
	{
		GLOBALASSERT( pHHST -> pStringObj[StrNum] );
		GLOBALASSERT
		(
			pHHST -> pStringObj[StrNum] -> CheckRef()
			>
			0
		);
			// check that the reference count is above zero
			// otherwise some code has messed up the reference
			// count and the string will have been deallocated...
		
		pHHST -> pStringObj[StrNum] -> R_AddRef();

		return pHHST -> pStringObj[StrNum];
	}
}

SCString* hhstringtablestruct :: GetSCString
(
	unsigned int StrNum
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(StrNum< NumEntries() );
		GLOBALASSERT(StrNum< MAX_ENTRIES_PER_STRING_TABLE);
	}

	/* CODE */
	{
		GLOBALASSERT( pStringObj[StrNum] );
		GLOBALASSERT
		(
			pStringObj[StrNum] -> CheckRef()
			>
			0
		);
			// check that the reference count is above zero
			// otherwise some code has messed up the reference
			// count and the string will have been deallocated...
		
		pStringObj[StrNum] -> R_AddRef();

		return pStringObj[StrNum];
	}
}

MCSCString* hhstringtablestruct :: GetMCSCString
(
	unsigned int StrNum,
	LogicalColour LogCol
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT(StrNum< NumEntries() );
		GLOBALASSERT(StrNum< MAX_ENTRIES_PER_STRING_TABLE);
	}

	/* CODE */
	{
		// Lazily evaluate:
		if
		(
			NULL == pMCStringObj[ StrNum ][ LogCol ]
		)
		{
			// no updating of reference count of SCString
			// due to direct access to the array
			pMCStringObj[ StrNum ][ LogCol ] = new MCSCString
			(
				pStringObj[StrNum],
				LogCol
			);
		}
	
		GLOBALASSERT( pMCStringObj[ StrNum ][ LogCol ] );

		pMCStringObj[ StrNum ][ LogCol ] -> R_AddRef();

		return pMCStringObj[ StrNum ][ LogCol ];
	}
}





#if debug
void STRUTIL_Diagnostics(void)
{
	/* CODE */
	{
		#if 0
		int i;

		for (i=0;i<pHHST_UserInterface->NumEntries;i++)
		{
			GLOBALASSERT(strlen(pHHST_UserInterface->pEntry[0])<90);
			textprint("String %i = \"%s\"\n",i,pHHST_UserInterface->pEntry[i]);
		}
		#endif
	}

	#if 0
	{
		int i;

		for 
		textprint("%50s",pHHST_UserInterface->pEntry[0]);
	}
	#endif
}
#endif

/* Internal function definitions ***********************************/
static void strutil_Failure(Failure_Behaviour FailBehav)
{
	/* unwritten */

	return;
}
#endif // SupportHHStuff

