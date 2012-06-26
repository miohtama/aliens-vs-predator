/*******************************************************************
 *
 *    DESCRIPTION: 	avpitems.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 30/3/98: AvP-specific menu items   
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"

	#include "avpitems.hpp"

	#if UseRebMenus
		#include "indexfnt.hpp"
		#include "strtab.hpp"
		#include "usr_io.h"
		#include "db.h"

		#define UseLocalAssert Yes
		#include "ourasert.h"

		#ifdef __WATCOMC__
			#pragma warning 139 5
			#pragma message("Disabled Warning W139")
		#endif
	#endif

	

/* Version settings ************************************************/

/* Constants *******************************************************/

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		#if 0
		extern OurBool			DaveDebugOn;
		extern FDIEXTENSIONTAG	FDIET_Dummy;
		extern IFEXTENSIONTAG	IFET_Dummy;
		extern FDIQUAD			FDIQuad_WholeScreen;
		extern FDIPOS			FDIPos_Origin;
		extern FDIPOS			FDIPos_ScreenCentre;
		extern IFOBJECTLOCATION IFObjLoc_Origin;
		extern UncompressedGlobalPlotAtomID UGPAID_StandardNull;
		extern IFCOLOUR			IFColour_Dummy;
 		extern IFVECTOR			IFVec_Zero;
		#endif
#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/

/* Internal type definitions ***************************************/
#if UseRebMenus
namespace RebMenus
{
	namespace TextLabels
	{
		class Effects
		{
		private:
			static TextID theID[KEYCONFIG_NUMITEMS];

		public:
			static TextID Get( enum KeyConfigItems in )
			{
				GLOBALASSERT( in < KEYCONFIG_NUMITEMS );

				return theID[in];
			}
		};

		class Methods
		{
		public:
		private:
		};
	};
}; // namespace RebMenus
#endif // UseRebMenus

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/
#if UseRebMenus
// static
TextID
RebMenus :: TextLabels :: Effects :: theID[KEYCONFIG_NUMITEMS] = 
{
	TEXTSTRING_PCKEY_FORWARD, // KEYCONFIG_FORWARD, 
	TEXTSTRING_PCKEY_BACKWARD, // KEYCONFIG_BACKWARD, 
	TEXTSTRING_PCKEY_LEFT, // KEYCONFIG_TURN_LEFT, 
	TEXTSTRING_PCKEY_RIGHT, // KEYCONFIG_TURN_RIGHT, 
	TEXTSTRING_PCKEY_STRAFE, // KEYCONFIG_STRAFE, 
	TEXTSTRING_PCKEY_STRAFE_LEFT, // KEYCONFIG_STRAFE_LEFT, 
	TEXTSTRING_PCKEY_STRAFE_RIGHT, // KEYCONFIG_STRAFE_RIGHT, 
	TEXTSTRING_PCKEY_LOOK_UP, // KEYCONFIG_LOOK_UP, 
	TEXTSTRING_PCKEY_LOOK_DOWN, // KEYCONFIG_LOOK_DOWN, 
	TEXTSTRING_PCKEY_CENTRE_VIEW, // KEYCONFIG_CENTRE_VIEW, 
	TEXTSTRING_PCKEY_WALK, // KEYCONFIG_WALK, 
	TEXTSTRING_PCKEY_CROUCH, // KEYCONFIG_CROUCH, 
	TEXTSTRING_PCKEY_JUMP, // KEYCONFIG_JUMP, 
	TEXTSTRING_PCKEY_OPERATE, // KEYCONFIG_OPERATE, 
	TEXTSTRING_PCKEY_CHANGE_VISION, // KEYCONFIG_VISION, 
	TEXTSTRING_PCKEY_NEXT_WEAPON, // KEYCONFIG_NEXT_WEAPON, 
	TEXTSTRING_PCKEY_PREVIOUS_WEAPON, // KEYCONFIG_PREVIOUS_WEAPON, 
	TEXTSTRING_PCKEY_FIRE_PRIMARY, // KEYCONFIG_FIRE_PRIMARY, 
	TEXTSTRING_PCKEY_FIRE_SECONDARY // KEYCONFIG_FIRE_SECONDARY, 
};
#endif

/* Exported function definitions ***********************************/
#if UseRebMenus
///////////////////////////////////////////////////////////////////////////////
// Project-specific item types: ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// class Item_NetworkStatus : public Item
// Process various keypresses:
OurBool
RebMenus :: Item_NetworkStatus :: Navigate
(
	enum NavigationOp // aNavOp
)
{
	// return = was message processed

	return No;
}

void
RebMenus :: Item_NetworkStatus :: Diagnostic
(
	OurBool // bSelected
) const
{
}


// Methods relating to rendering:
void
RebMenus :: Item_NetworkStatus :: Render
(
	const RenderContext& theContext,
	OurBool // bSelected
) const
{
	#if 0
	textprintXY
	(
		theContext . Pos() . x,
		theContext . Pos() . y,
		"Item_NetworkStatus"
	);
	#endif

	IndexedFont* pFont = IndexedFont :: GetFont
	(
		Fonts :: GetIndex
		(
			Yes // OurBool bSelected
		)
	);
	GLOBALASSERT(pFont);

	r2pos R2Pos_StartOfRow = theContext . Pos();

	// Title row:
	{
		// Column title: player name
		{
			r2pos R2Pos_Where = R2Pos_StartOfRow;
			SCString* pSCString_Name = new SCString("NAME");

			pFont -> RenderString_Clipped
			(
				R2Pos_Where, // struct r2pos& R2Pos_Cursor,
				theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
				(ONE_FIXED-1), // int FixP_Alpha,
				*pSCString_Name // const SCString& SCStr
			);

			pSCString_Name -> R_Release();
		}

		// Column title: player species
		{
			r2pos R2Pos_Where = theContext . Pos_Column1();

			SCString* pSCString_Name = new SCString("SPECIES");

			pFont -> RenderString_Clipped
			(
				R2Pos_Where, // struct r2pos& R2Pos_Cursor,
				theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
				(ONE_FIXED-1), // int FixP_Alpha,
				*pSCString_Name // const SCString& SCStr
			);

			pSCString_Name -> R_Release();
		}

		R2Pos_StartOfRow . y += pFont -> GetHeight();		
	}

	for (int i=0;i<NET_MAXPLAYERS;i++)
	{
		const NETGAME_PLAYERDATA& slotPlayerData = netGameData . playerData[i];

		if(slotPlayerData . playerId!=NULL)
		{
			// Name:
			{
				r2pos R2Pos_Where = R2Pos_StartOfRow;
				SCString* pSCString_Name = new SCString(&(slotPlayerData . name[0]));

				pFont -> RenderString_Clipped
				(
					R2Pos_Where, // struct r2pos& R2Pos_Cursor,
					theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
					(ONE_FIXED-1), // int FixP_Alpha,
					*pSCString_Name // const SCString& SCStr
				);

				pSCString_Name -> R_Release();
			}

			// Species:
			{
				r2pos R2Pos_Where = R2Pos_StartOfRow;
				R2Pos_Where . x += theContext . Column0_W();

				SCString* pSCString_Species = &StringTable :: GetSCString
				(
					GetTextIDForCharacterType(slotPlayerData . characterType)
				);

				pFont -> RenderString_Clipped
				(
					R2Pos_Where, // struct r2pos& R2Pos_Cursor,
					theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
					(ONE_FIXED-1), // int FixP_Alpha,
					*pSCString_Species // const SCString& SCStr
				);

				pSCString_Species -> R_Release();
			}

			// Ready to start "Ok" text:
			if ( slotPlayerData . startFlag )
			{
				r2pos R2Pos_Where = R2Pos_StartOfRow;
				R2Pos_Where . x += theContext . Column0_W() + 150;
					// hardcoded size

				SCString* pSCString_Ok = &StringTable :: GetSCString
				(
					TEXTSTRING_MULTI_OK
				);

				pFont -> RenderString_Clipped
				(
					R2Pos_Where, // struct r2pos& R2Pos_Cursor,
					theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
					(ONE_FIXED-1), // int FixP_Alpha,
					*pSCString_Ok // const SCString& SCStr
				);
			}
		}
		else
		{
			// Empty slot:
			r2pos R2Pos_Where = R2Pos_StartOfRow;

			SCString* pSCString_EmptySlot = new SCString("--------");
				// LOCALISEME()

			pFont -> RenderString_Clipped
			(
				R2Pos_Where, // struct r2pos& R2Pos_Cursor,
				theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
				(ONE_FIXED-1), // int FixP_Alpha,
				*pSCString_EmptySlot // const SCString& SCStr
			);
			
			pSCString_EmptySlot -> R_Release();
		}

		R2Pos_StartOfRow . y += pFont -> GetHeight();
	}
	
}

RebMenus :: SizeInfo
RebMenus :: Item_NetworkStatus :: GetSizeInfo(void) const
{
	IndexedFont* pFont = IndexedFont :: GetFont
	(
		Fonts :: GetIndex
		(
			Yes // OurBool bSelected
		)
	);
	GLOBALASSERT(pFont);

	int WidthPlayerName =
	(
		(
			(NET_PLAYERNAMELENGTH )
			*
			( pFont -> GetMaxWidth() )
		)
		+5 // add a little spacing
	);

	int WidthSpecies = 80;
		// for now

	r2size R2Size_PlayerNames
	(
		// Width:
		(WidthPlayerName + WidthSpecies),

		// Height:
		(
			( NET_MAXPLAYERS + 1 )
				// + 1 to allow for title row
			*
			( pFont -> GetHeight() )
		)
	);

	return SizeInfo
	(
		R2Size_PlayerNames,
		WidthPlayerName
	);
}

// static
TextID
RebMenus :: Item_NetworkStatus :: GetTextIDForCharacterType
(
	NETGAME_CHARACTERTYPE characterType
)
{
	switch(characterType)
	{
		default: GLOBALASSERT(0); // then fall through
		case(NGCT_Marine):
		{
			return TEXTSTRING_MULTI_MARINE;
		}
		case(NGCT_Predator):
		{
			return TEXTSTRING_MULTI_PREDATOR;
		}
		case(NGCT_Alien):
		{
			return TEXTSTRING_MULTI_ALIEN;
		}
	}
}


#if 0
typedef struct netgame_playerdata
{
	DPID playerId;
	char name[NET_PLAYERNAMELENGTH];
	NETGAME_CHARACTERTYPE characterType;
	int scores[NET_MAXPLAYERS];
	unsigned char startFlag;
}NETGAME_PLAYERDATA;

typedef struct netgame_gamedata
{
	NETGAME_STATES myGameState;
	NETGAME_CHARACTERTYPE myCharacterType;
	unsigned char myStartFlag;
	NETGAME_PLAYERDATA playerData[NET_MAXPLAYERS];
	int teamScores[NET_MAXTEAMS];
	NETGAME_TYPE gameType;
	unsigned char levelNumber;
	unsigned int scoreLimit;
	unsigned char timeLimit;
	int GameTimeElapsed;
}NETGAME_GAMEDATA;
#endif


#if 0
#endif

#if 0
			case(MMSelect_Start):
			{
				netGameData.myStartFlag=1;
				if(AvP.Network==I_Host)
				{
					int myIndex;
					myIndex = PlayerIdInPlayerList(AVPDPNetID);
					LOCALASSERT(myIndex!=NET_IDNOTINPLAYERLIST);
					netGameData.playerData[myIndex].startFlag = 1;
				}
				break;
			}
#endif



// class Item_NetworkErrorView : public Item
RebMenus :: Item_NetworkErrorView :: Item_NetworkErrorView
(
) : Item
	(
		OnOffAppearance()
	)
{
}

RebMenus :: Item_NetworkErrorView :: ~Item_NetworkErrorView()
{
}

// Process various keypresses:
OurBool
RebMenus :: Item_NetworkErrorView :: Navigate
(
	enum NavigationOp // aNavOp
)
{
	// return = was message processed
	return No;
}

void
RebMenus :: Item_NetworkErrorView :: Diagnostic
(
	OurBool // bSelected
) const
{
	textprint("Item_NetworkErrorView :: Diagnostic()\n");
}


// Methods relating to rendering:
void
RebMenus :: Item_NetworkErrorView :: Render
(
	const RenderContext& theContext,
	OurBool // bSelected
) const
{
	SCString* pSCString_Error = GetStringForCurrentNetworkError();

	#if 0
	textprintXY
	(
		theContext . Pos() . x,
		theContext . Pos() . y,
		"Item_NetworkErrorView:%s",
		pSCString_Error -> pProjCh()		
	);
	#endif

	IndexedFont* pFont = IndexedFont :: GetFont
	(
		Fonts :: GetIndex
		(
			Yes // OurBool bSelected
		)
	);
	GLOBALASSERT(pFont);

	r2pos R2Pos_Where = theContext . Pos();

	pFont -> RenderString_Clipped
	(
		R2Pos_Where, // struct r2pos& R2Pos_Cursor,
		theContext . ClipRect(), // const struct r2rect& R2Rect_Clip,
		(ONE_FIXED-1), // int FixP_Alpha,
		*pSCString_Error // const SCString& SCStr
	);

	pSCString_Error -> R_Release();
}

RebMenus :: SizeInfo
RebMenus :: Item_NetworkErrorView :: GetSizeInfo(void) const
{
	SCString* pSCString_Error = GetStringForCurrentNetworkError();

	SizeInfo theSizeInfo
	(
		pSCString_Error -> CalcSize
		(
			Fonts::GetIndex
			(
				Yes // OurBool bSelected
			)
		),
		0
	);

	pSCString_Error -> R_Release();
	
	return theSizeInfo;
}

// private:
// static
SCString*
RebMenus :: Item_NetworkErrorView :: GetStringForCurrentNetworkError(void)
{
	return &StringTable :: GetSCString
	(
		GetTextIDForCurrentNetworkError()
	);
}

// static
TextID
RebMenus :: Item_NetworkErrorView :: GetTextIDForCurrentNetworkError(void)
{
	switch(netGameData.myGameState)
	{
		default: LOCALASSERT(1==0);
		case(NGS_Error_GameFull):
		{
			return TEXTSTRING_MULTI_GAMEFULL;
		}
		case(NGS_Error_GameStarted):
		{
			return TEXTSTRING_MULTI_ALREADYSTARTED;
		}
		case(NGS_Error_HostLost):
		{
			return TEXTSTRING_MULTI_CONNECTIONLOST;
		}
	}
}

// class Item_VideoModeSelector : public Item
// public:
RebMenus :: Item_VideoModeSelector :: Item_VideoModeSelector
(			
	OnOffAppearance theOnOffApp_New
) : Item
	(
		theOnOffApp_New
	)
{
}

// Process various keypresses:
OurBool
RebMenus :: Item_VideoModeSelector :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch (aNavOp)
	{
		case NavOp_Left:
			Dec();
			return Yes;

		case NavOp_Right:
		case NavOp_Trigger:
			Inc();
			return Yes;
	}
	return No;
}

void
RebMenus :: Item_VideoModeSelector :: Diagnostic(OurBool bSelected) const
{
}

void
RebMenus :: Item_VideoModeSelector :: Render
(
	const RenderContext& theContext,
	OurBool bSelected
) const
{
	RenderAppearance
	(
		theContext,
		bSelected
	);

	RenderContext valueContext = theContext . NextColumn();

	IndexedFont* pFont = IndexedFont :: GetFont
	(
		Fonts :: GetIndex
		(
			bSelected			
		)
	);
	GLOBALASSERT( pFont );

	r2pos R2Pos_Cursor = valueContext . Pos();

	SCString* pSCString_Value = VideoModeSelection :: DescribeCurrentSelection();
	GLOBALASSERT(pSCString_Value);

	pFont -> RenderString_Clipped
	(
		R2Pos_Cursor,
		valueContext . ClipRect(),
		Label_FixP_Alpha,
		*pSCString_Value		
	);

	pSCString_Value -> R_Release();

}

RebMenus :: SizeInfo
RebMenus :: Item_VideoModeSelector :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo() . AddColumn
	(
		SizeInfo
		(
			r2size
			(
				150,
				21
			),
			0
		)
	);
}

void
RebMenus :: Item_VideoModeSelector :: Dec(void)
{
	VideoModeSelection :: Dec();
}

void
RebMenus :: Item_VideoModeSelector :: Inc(void)
{
	VideoModeSelection :: Inc();
}

// class Item_KeyConfig_PageView : public Item
// public:
RebMenus :: Item_KeyConfig_PageView :: Item_KeyConfig_PageView
(
	OnOffAppearance theOnOffApp_New,
	int NumEffectsPerPage_New
) : Item
	(
		theOnOffApp_New
	),
	bDebounced(No),
	NumEffectsPerPage_Val(NumEffectsPerPage_New),
	NumPages_Val
	(
		// total num effects divided by num effects per page,
		// using an old trick to round up:
		(KEYCONFIG_NUMITEMS + (NumEffectsPerPage_New-1))/ NumEffectsPerPage_New
	),
	CurrentPage_Val(0)
{	
	GLOBALASSERT( pSingleton == NULL );
	pSingleton = this;
}

RebMenus :: Item_KeyConfig_PageView :: ~Item_KeyConfig_PageView()
{
	if (this == pActive)
	{
		pActive = NULL;
	}

	GLOBALASSERT( pSingleton == this );
	pSingleton = NULL;
}


// Process various keypresses:
OurBool
RebMenus :: Item_KeyConfig_PageView :: Navigate( enum NavigationOp aNavOp )
{
	GLOBALASSERT( NULL == pActive );

	// return = was message processed
	switch (aNavOp)
	{
	case NavOp_Up:
		if (SelectionY>0)
		{
			SelectionY--;
			return Yes;
		}
		else
		{
			return No;
		}
	case NavOp_Down:
		if (SelectionY< GetFinalYForPage(CurrentPage_Val))
		{
			SelectionY++;
			return Yes;
		}
		else
		{
			return No;
		}
	case NavOp_Left:
	case NavOp_Right:
		SelectionX = 1 - SelectionX;
		return Yes;

	case NavOp_Home:
		SelectionY = 0;
		return Yes;
		
	case NavOp_End:
		SelectionY = GetFinalYForPage(CurrentPage_Val);
		return Yes;
		
	case NavOp_Trigger:
		// Expect input:
		pActive = this;
		return Yes;
		
	case NavOp_Cancel:
		return No;
		
	default: GLOBALASSERT(0);
	}
	return No;
}

void
RebMenus :: Item_KeyConfig_PageView :: Diagnostic(OurBool bSelected) const
{
	textprint("Item_KeyConfig_PageView :: Diagnostic():");

	DiagnosticAppearance(bSelected);

	for (int i=0;i<KEYCONFIG_NUMITEMS;i++)
	{
		enum KeyConfigItems theEffect = i;

		SCString& theSCString_EffectLabel = StringTable :: GetSCString
		(
			GetEffectLabel(theEffect)
		);

		textprint
		(
			"%s\n",theSCString_EffectLabel . pProjCh()
		);

		theSCString_EffectLabel . R_Release();
	} 
}

// Methods relating to rendering:
void
RebMenus :: Item_KeyConfig_PageView :: Render
(
	const RenderContext& theContext,
	OurBool bSelected
) const
{
	RenderAppearance
	(
		theContext,
		bSelected
	);

	r2pos R2Pos_I
	(
		theContext . Pos()
	);

	SizeInfo thisSizeInfo = GetSizeInfo();

	const player_input_configuration* const pPrimary = &PlayerInputPrimaryConfig;
	const player_input_configuration* const pSecondary = &PlayerInputSecondaryConfig;

	for (int viewRow=0;viewRow<NumEffectsPerPage_Val;viewRow++)
	{
		enum KeyConfigItems theEffect;
		
		if
		(
			GetEffectForRowOnPage
			(
				viewRow, // int inY,
				CurrentPage_Val, // int inPage
				theEffect // enum KeyConfigItems& outEffect
			)
		)
		{
			// Font to use for the selected effect/method:
			IndexedFont* pFont_Selected = IndexedFont :: GetFont
			(
				Fonts :: GetIndex
				(
					bSelected			
						// so only appears selected if the item as a whole is selected
				)
			);
			GLOBALASSERT( pFont_Selected );

			// Font to use for everything else:
			IndexedFont* pFont_Unselected = IndexedFont :: GetFont
			(
				Fonts :: GetIndex
				(
					No
				)
			);
			GLOBALASSERT( pFont_Unselected );

			SCString& theSCString_EffectLabel = StringTable :: GetSCString
			(
				GetEffectLabel(theEffect)
			);

			OurBool bSelectedY = (viewRow==SelectionY);
			{

				r2pos R2Pos_Cursor = R2Pos_I;

				(
					bSelectedY
					?
					pFont_Selected
					:
					pFont_Unselected
				) -> RenderString_Clipped
				(
					R2Pos_Cursor,
					theContext . ClipRect(),
					Label_FixP_Alpha,
					theSCString_EffectLabel
				);

			}

			// Primary method:
			if
			(
				!
				(
					ExpectingKey()
					&&
					bSelected
					&&
					(SelectionX==0)
					&&
					(SelectionY==viewRow)
				)

			)
			{
				SCString* pSCString_Method0 = GetMethodString
				(
					pPrimary -> GetMethod(theEffect)
				);
				  
				r2pos R2Pos_Cursor = R2Pos_I;
				R2Pos_Cursor .x += theContext . Column0_W();

				#if 0
				if (bSelected && (SelectionY == viewRow) && (SelectionX==0))
				{
					textprintXY
					(
						R2Pos_Cursor . x -20,
						R2Pos_Cursor . y,
						"*"
					);
				}
				#endif
				
				(
					bSelectedY && (SelectionX==0)
					?
					pFont_Selected
					:
					pFont_Unselected
				) -> RenderString_Clipped
				(
					R2Pos_Cursor,
					theContext . ClipRect(),
					Label_FixP_Alpha,
					*pSCString_Method0
				);

				pSCString_Method0 -> R_Release();
			}

			// Secondary method:
			if
			(
				!
				(
					ExpectingKey()
					&&
					bSelected
					&&
					(SelectionX==1)
					&&
					(SelectionY==viewRow)
				)
			)
			{
				SCString* pSCString_Method1 = GetMethodString
				(
					pSecondary -> GetMethod(theEffect)
				);
				  
				r2pos R2Pos_Cursor = R2Pos_I;
				R2Pos_Cursor .x += theContext . Column0_W() + 110;

				#if 0
				if (bSelected && (SelectionY == viewRow) && (SelectionX==1))
				{
					textprintXY
					(
						R2Pos_Cursor . x -20,
						R2Pos_Cursor . y,
						"*"
					);
				}
				#endif

				(
					bSelectedY && (SelectionX==1)
					?
					pFont_Selected
					:
					pFont_Unselected
				) -> RenderString_Clipped
				(
					R2Pos_Cursor,
					theContext . ClipRect(),
					Label_FixP_Alpha,
					*pSCString_Method1
				);

				pSCString_Method1 -> R_Release();
			}


			R2Pos_I . y += theSCString_EffectLabel . CalcSize
			(
				Fonts :: GetIndex
				(
					bSelected
				)
			) . h;

			theSCString_EffectLabel . R_Release();
		}
	}
}

RebMenus :: SizeInfo
RebMenus :: Item_KeyConfig_PageView :: GetSizeInfo(void) const
{
	SizeInfo thisSizeInfo = AppearanceSizeInfo();

	// Calc size as if all items were present:
	for (int i=0;i<KEYCONFIG_NUMITEMS;i++)
	{
		enum KeyConfigItems theEffect = i;
		
		SCString& theSCString_EffectLabel = StringTable :: GetSCString
		(
			GetEffectLabel(theEffect)
		);

		SizeInfo rowSizeInfo = SizeInfo
		(
			theSCString_EffectLabel . CalcSize
			(
				Fonts :: GetIndex
				(
					Yes
					// for the moment??
				)
			),
			0
		)
		#if 1
		. AddColumn
		(
			SizeInfo
			(
				r2size(250,21),
				0
			)
		);
		#else
		. AddColumn
		(
			SizeInfo
			(
				r2size(200,21),
				0
			)
		) . AddColumn
		(
			SizeInfo
			(
				r2size(200,21),
				0
			)
		);
		#endif

		theSCString_EffectLabel . R_Release();

		thisSizeInfo = thisSizeInfo . AddRow
		(
			rowSizeInfo
		);
	}

	// Now reduce height to be just n times the (hacked in) height of the font
	// Doind it in two setps like this is to avoid glitching when cycling through
	// the pages of effects:

	#if 0
	db_logf1
	(
		(
			"keyconfig SizeInfo(r2size(%i,%i),%i)",
			thisSizeInfo.GetMin().w,
			thisSizeInfo.GetMin().h,
			thisSizeInfo.GetColumn0_minW()
		)
	);
	#endif

	return SizeInfo
	(
		r2size
		(
			thisSizeInfo . GetMin() . w,
			21 * NumEffectsPerPage_Val
		),
		thisSizeInfo . GetColumn0_minW()
	);
}

void
RebMenus :: Item_KeyConfig_PageView :: SetMethod(unsigned char theMethod)
{
	GLOBALASSERT( this == pActive );

	player_input_configuration* const pPrimary = &PlayerInputPrimaryConfig;
	player_input_configuration* const pSecondary = &PlayerInputSecondaryConfig;

	player_input_configuration* const pConfig =
	(
		(SelectionX == 0)
		?
		pPrimary
		:
		pSecondary
	);

	GLOBALASSERT( SelectionY >=0);
	GLOBALASSERT( SelectionY < KEYCONFIG_NUMITEMS );


	enum KeyConfigItems theEffect;
	OurBool bGotEffect = GetEffectForRowOnPage
	(
		SelectionY,
		CurrentPage_Val,
		theEffect // enum KeyConfigItems& outEffect
	);

	pConfig -> SetMethod
	(
		theEffect,
		theMethod
	);

	pActive = NULL;
	
}

void
RebMenus :: Item_KeyConfig_PageView :: NxtPage(void)
{
	if (CurrentPage_Val < (NumPages_Val-1))
	{
		CurrentPage_Val++;
	}
	else
	{
		CurrentPage_Val=0;
	}
}
void
RebMenus :: Item_KeyConfig_PageView :: PrvPage(void)
{
	if (CurrentPage_Val > 0)
	{
		CurrentPage_Val--;
	}
	else
	{
		CurrentPage_Val=(NumPages_Val-1);
	}
}

// static
SCString*
RebMenus :: Item_KeyConfig_PageView :: GetMethodString( unsigned char inPhysicalKey )
{
	TextID theTextID;

	if
	(
		RebMenus :: Item_KeyConfig_PageView :: GetKeyLabel
		(
			inPhysicalKey,
			theTextID // TextID& outTextID
		)
	)
	{
		return &StringTable :: GetSCString(theTextID);
	}
	else
	{
		ProjChar theProjChar[2];

		if (inPhysicalKey >= KEY_A && inPhysicalKey <= KEY_Z)
		{
			theProjChar[0] = ProjChar(int(inPhysicalKey) - KEY_A + 'A');
		}
		else if (inPhysicalKey >= KEY_0 && inPhysicalKey <= KEY_9)
		{
			theProjChar[0] = ProjChar(int(inPhysicalKey) - KEY_0 + '0');
		}
		else
		{
			theProjChar[0] = 0;
		}
					
		theProjChar[1] = 0;

		return new SCString(theProjChar);
	}	
}

void
RebMenus :: Item_KeyConfig_PageView :: Selected_Hook
(
	enum NavigationOp theNavOp
)
{
	switch (theNavOp)
	{
	default: GLOBALASSERT(0);
	case NavOp_Up:
		SelectionY = GetFinalYForPage(CurrentPage_Val);
		break;
	case NavOp_Down:
		SelectionY = 0;
		break;
	case NavOp_Left:
		SelectionX = 1;
		break;
	case NavOp_Right:
		SelectionX = 0;
		break;
	case NavOp_Home:
		SelectionY = 0;
		break;
	case NavOp_End:
		SelectionY = GetFinalYForPage(CurrentPage_Val);
		break;
	case NavOp_Trigger:
		break;
	case NavOp_Cancel:
		break;
	}
}


// private:
// static
TextID
RebMenus :: Item_KeyConfig_PageView :: GetEffectLabel( enum KeyConfigItems inEffect)
{
	// takes an effect (e.g. jump) and returns
	// a text label to use
	return TextLabels :: Effects :: Get( inEffect );
}


OurBool 
RebMenus :: Item_KeyConfig_PageView :: GetKeyLabel
(
	int inPhysicalKey,
	TextID& outTextID
)
{
	// takes a physical method key and attempts to find a text
	// string to use for it, returning whether it does.
	// If it fails, output area is untouched

	switch (inPhysicalKey)
	{
		case KEY_UP: outTextID = TEXTSTRING_KEYS_UP; return Yes;
		case KEY_DOWN: outTextID = TEXTSTRING_KEYS_DOWN; return Yes;
		case KEY_LEFT: outTextID = TEXTSTRING_KEYS_LEFT; return Yes;
		case KEY_RIGHT: outTextID = TEXTSTRING_KEYS_RIGHT; return Yes;
		case KEY_CR: outTextID = TEXTSTRING_KEYS_RETURN; return Yes;
		case KEY_TAB: outTextID = TEXTSTRING_KEYS_TAB; return Yes;
		case KEY_INS: outTextID = TEXTSTRING_KEYS_INSERT; return Yes;
		case KEY_DEL: outTextID = TEXTSTRING_KEYS_DELETE; return Yes;
		case KEY_END: outTextID = TEXTSTRING_KEYS_END; return Yes;
		case KEY_HOME: outTextID = TEXTSTRING_KEYS_HOME; return Yes;
		case KEY_PAGEUP: outTextID = TEXTSTRING_KEYS_PGUP; return Yes;
		case KEY_PAGEDOWN: outTextID = TEXTSTRING_KEYS_PGDOWN; return Yes;
		case KEY_BACKSPACE: outTextID = TEXTSTRING_KEYS_BACKSP; return Yes;
		case KEY_COMMA: outTextID = TEXTSTRING_KEYS_COMMA; return Yes;
		case KEY_FSTOP: outTextID = TEXTSTRING_KEYS_PERIOD; return Yes;
		case KEY_SPACE: outTextID = TEXTSTRING_KEYS_SPACE; return Yes;
		case KEY_LMOUSE: outTextID = TEXTSTRING_KEYS_LMOUSE; return Yes;
		case KEY_RMOUSE: outTextID = TEXTSTRING_KEYS_RMOUSE; return Yes;
		case KEY_LEFTALT: outTextID = TEXTSTRING_KEYS_LALT; return Yes;
		case KEY_RIGHTALT: outTextID = TEXTSTRING_KEYS_RALT; return Yes;
		case KEY_LEFTCTRL: outTextID = TEXTSTRING_KEYS_LCTRL; return Yes;
		case KEY_RIGHTCTRL: outTextID = TEXTSTRING_KEYS_RCTRL; return Yes;
		case KEY_LEFTSHIFT: outTextID = TEXTSTRING_KEYS_LSHIFT; return Yes;
		case KEY_RIGHTSHIFT: outTextID = TEXTSTRING_KEYS_RSHIFT; return Yes;
		case KEY_CAPS: outTextID = TEXTSTRING_KEYS_CAPS; return Yes;
		case KEY_NUMLOCK: outTextID = TEXTSTRING_KEYS_NUMLOCK; return Yes;
		case KEY_SCROLLOK: outTextID = TEXTSTRING_KEYS_SCRLOCK; return Yes;
		case KEY_NUMPAD0: outTextID = TEXTSTRING_KEYS_PAD0; return Yes;
		case KEY_NUMPAD1: outTextID = TEXTSTRING_KEYS_PAD1; return Yes;
		case KEY_NUMPAD2: outTextID = TEXTSTRING_KEYS_PAD2; return Yes;
		case KEY_NUMPAD3: outTextID = TEXTSTRING_KEYS_PAD3; return Yes;
		case KEY_NUMPAD4: outTextID = TEXTSTRING_KEYS_PAD4; return Yes;
		case KEY_NUMPAD5: outTextID = TEXTSTRING_KEYS_PAD5; return Yes;
		case KEY_NUMPAD6: outTextID = TEXTSTRING_KEYS_PAD6; return Yes;
		case KEY_NUMPAD7: outTextID = TEXTSTRING_KEYS_PAD7; return Yes;
		case KEY_NUMPAD8: outTextID = TEXTSTRING_KEYS_PAD8; return Yes;
		case KEY_NUMPAD9: outTextID = TEXTSTRING_KEYS_PAD9; return Yes;
		case KEY_NUMPADSUB: outTextID = TEXTSTRING_KEYS_PADSUB; return Yes;
		case KEY_NUMPADADD: outTextID = TEXTSTRING_KEYS_PADADD; return Yes;
		case KEY_NUMPADDEL: outTextID = TEXTSTRING_KEYS_PADDEL; return Yes;
		default: return No;
	}
}

OurBool
RebMenus :: Item_KeyConfig_PageView :: GetEffectForRowOnPage
(
	int inY,
	int inPage,
	enum KeyConfigItems& outEffect
) const
	// returns truth if it can output a valid effect into the specified area
{
	GLOBALASSERT(inY>=0);
	GLOBALASSERT(inY<NumEffectsPerPage_Val);

	GLOBALASSERT(inPage>=0);
	GLOBALASSERT(inPage<NumPages_Val);

	int i = 
	(
		inY + (NumEffectsPerPage_Val*inPage)
	);

	if (i < KEYCONFIG_NUMITEMS)
	{
		outEffect = (enum KeyConfigItems) i;
		return Yes;
	}
	else
	{
		return No;
	}

}

int
RebMenus :: Item_KeyConfig_PageView :: GetFinalYForPage
(
	int inPage
) const
{
	// Y on this page can range from 0 to this inclusive
	GLOBALASSERT( inPage >= 0);
	GLOBALASSERT( inPage < NumPages_Val );

	if (inPage<(NumPages_Val-1))
	{
		// If not the final page, it has all the rows:
		return (NumEffectsPerPage_Val-1);
	}
	else
	{
		// The final page has only got the rows that remain after dealing
		// with all the other pages:
		int ReturnVal =
		(
			(KEYCONFIG_NUMITEMS-1) - (NumEffectsPerPage_Val*inPage)
		);

		GLOBALASSERT( ReturnVal >=0 );
		GLOBALASSERT( ReturnVal < NumEffectsPerPage_Val );

		return ReturnVal;
	}
}


// static
RebMenus :: Item_KeyConfig_PageView*
RebMenus :: Item_KeyConfig_PageView :: pActive = NULL;

// static
RebMenus :: Item_KeyConfig_PageView*
RebMenus :: Item_KeyConfig_PageView :: pSingleton = NULL;




unsigned char
player_input_configuration :: GetMethod( enum KeyConfigItems theEffect ) const
{
	switch (theEffect)
	{
		case KEYCONFIG_FORWARD:
			return Forward;
		case KEYCONFIG_BACKWARD:
			return Backward;
		case KEYCONFIG_TURN_LEFT:
			return Left;
		case KEYCONFIG_TURN_RIGHT:
			return Right;
		case KEYCONFIG_STRAFE:
			return Strafe;
		case KEYCONFIG_STRAFE_LEFT:
			return StrafeLeft;
		case KEYCONFIG_STRAFE_RIGHT:
			return StrafeRight;
		case KEYCONFIG_LOOK_UP:
			return LookUp;
		case KEYCONFIG_LOOK_DOWN:
			return LookDown;
		case KEYCONFIG_CENTRE_VIEW:
			return CentreView;
		case KEYCONFIG_WALK:
			return Walk;
		case KEYCONFIG_CROUCH:
			return Crouch;
		case KEYCONFIG_JUMP:
			return Jump;
		case KEYCONFIG_OPERATE:
			return Operate;
		case KEYCONFIG_VISION:
			return ChangeVision;
		case KEYCONFIG_NEXT_WEAPON:
			return NextWeapon;
		case KEYCONFIG_PREVIOUS_WEAPON:
			return PreviousWeapon;
		case KEYCONFIG_FIRE_PRIMARY:
			return FirePrimaryWeapon;
		case KEYCONFIG_FIRE_SECONDARY:
			return FireSecondaryWeapon;
		default: GLOBALASSERT(0);
			return 0;
	}
}

void
player_input_configuration :: SetMethod
(
	enum KeyConfigItems theEffect,
	unsigned char newMethod
)
{
	switch (theEffect)
	{
		case KEYCONFIG_FORWARD:
			Forward = newMethod;
			break;
		case KEYCONFIG_BACKWARD:
			Backward = newMethod;
			break;
		case KEYCONFIG_TURN_LEFT:
			Left = newMethod;
			break;
		case KEYCONFIG_TURN_RIGHT:
			Right = newMethod;
			break;
		case KEYCONFIG_STRAFE:
			Strafe = newMethod;
			break;
		case KEYCONFIG_STRAFE_LEFT:
			StrafeLeft = newMethod;
			break;
		case KEYCONFIG_STRAFE_RIGHT:
			StrafeRight = newMethod;
			break;
		case KEYCONFIG_LOOK_UP:
			LookUp = newMethod;
			break;
		case KEYCONFIG_LOOK_DOWN:
			LookDown = newMethod;
			break;
		case KEYCONFIG_CENTRE_VIEW:
			CentreView = newMethod;
			break;
		case KEYCONFIG_WALK:
			Walk = newMethod;
			break;
		case KEYCONFIG_CROUCH:
			Crouch = newMethod;
			break;
		case KEYCONFIG_JUMP:
			Jump = newMethod;
			break;
		case KEYCONFIG_OPERATE:
			Operate = newMethod;
			break;
		case KEYCONFIG_VISION:
			ChangeVision = newMethod;
			break;
		case KEYCONFIG_NEXT_WEAPON:
			NextWeapon = newMethod;
			break;
		case KEYCONFIG_PREVIOUS_WEAPON:
			PreviousWeapon = newMethod;
			break;
		case KEYCONFIG_FIRE_PRIMARY:
			FirePrimaryWeapon = newMethod;
			break;
		case KEYCONFIG_FIRE_SECONDARY:
			FireSecondaryWeapon = newMethod;
			break;
		default: GLOBALASSERT(0);
	}
}

#if 0
// class Item_KeyConfig_Selector : public CompositeItem
// public:
RebMenus :: Item_KeyConfig_Selector :: Item_KeyConfig_Selector
(
	OnOffAppearance theOnOffApp_New
) : CompositeItem
	(
		theOnOffApp_New, // OnOffAppearance theOnOffApp_New,
		D_Vert, // enum Direction theD,
		Align_Centre // enum Alignment theAlignment_New
	)
{
	{
		Command* pCommand = new Command_KeyConfig_PrvPage();
		AddNewCommandItem
		(
			OnOffAppearance(TEXTSTRING_PSXCOMPUTER_PREVIOUS),
			pCommand
		);
		pCommand -> R_Release();
	}
	{
		Command* pCommand = new Command_KeyConfig_NxtPage();
		AddNewCommandItem
		(
			OnOffAppearance(TEXTSTRING_PSXCOMPUTER_NEXT),
			pCommand
		);
		pCommand -> R_Release();
	}
		// ...revise these text strings at some point
}
// private:
#endif


#endif
	// UseRebMenus

/* Internal function definitions ***********************************/
