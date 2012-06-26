/*******************************************************************
 *
 *    DESCRIPTION: 	rebmenus.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 9/3/98
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"

	#include "rebmenus.hpp"

	#if UseRebMenus
		#include "rebitems.hpp"
		#include "strtab.hpp"
		#include "indexfnt.hpp"

		#include "awTexLd.h"
		#include "alt_tab.h"

		#define UseLocalAssert Yes
		#include "ourasert.h"
	#endif // UseRebMenus

/* Version settings ************************************************/
		
/* Constants *******************************************************/

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#if UseRebMenus
	#ifdef __cplusplus
		extern "C"
		{
	#endif

			extern LPDIRECTDRAWSURFACE lpDDSBack;

	#ifdef __cplusplus
		};
	#endif
#endif // UseRebMenus


/* Exported globals ************************************************/
#if UseRebMenus
#if OverrideOldMenus
// private:
// static
OurBool RebMenus :: InGame :: bInTheGame_Val = No;

// static

// enum RebMenus :: MenuLoop :: ExitReason
// enum ExitReason
RebMenus :: MenuLoop :: ExitReason
RebMenus :: MenuLoop :: theExitReason_Val = ExitReason_None;
#endif
	// OverrideOldMenus
#endif
	// UseRebMenus

/* Internal type definitions ***************************************/

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
#if UseRebMenus
// namespace RebMenus
// class Command_JumpToPage : public Command
// public:
OurBool
RebMenus :: Command_JumpToPage :: Execute(void)
{
	Page :: SelectPage(thePage_Val);

	return Yes;
}


// class Command_ReturnFromPage : public Command
// public:
OurBool
RebMenus :: Command_ReturnFromPage :: Execute(void)
{
	Page :: TryToReturn();

	return Yes;
}

RebMenus :: SizeInfo
RebMenus :: SizeInfo :: Overlay
(
	SizeInfo other
)
{
	int minW_Max =
	(
		(minR2Size_Val . w > other . minR2Size_Val . w)
		?
		minR2Size_Val . w
		:
		other . minR2Size_Val . w
	);
	int minH_Max =
	(
		(minR2Size_Val . h > other . minR2Size_Val . h)
		?
		minR2Size_Val . h
		:
		other . minR2Size_Val . h
	);
	int maxColumn_minW =
	(
		(Column0_minW_Val > other . Column0_minW_Val)
		?
		Column0_minW_Val
		:
		other . Column0_minW_Val
	);
	return SizeInfo
	(
		r2size
		(
			minW_Max,
			minH_Max
		),
		maxColumn_minW
	);
}

RebMenus :: SizeInfo
RebMenus :: SizeInfo :: AddColumn
(
	SizeInfo nextColumn
)
{
	r2size R2Size_New = minR2Size_Val;
	r2size R2Size_NextColumn = nextColumn . minR2Size_Val;

	R2Size_New . HCompose
	(
		R2Size_NextColumn
	);
	
	return SizeInfo
	(
		R2Size_New,
		minR2Size_Val . w
	);
}

RebMenus :: SizeInfo
RebMenus :: SizeInfo :: AddRow
(
	SizeInfo nextRow
)
{
	r2size R2Size_New = minR2Size_Val;

	int maxColumn_minW = 
	(
		(Column0_minW_Val > nextRow . Column0_minW_Val)
		?
		Column0_minW_Val
		:
		nextRow . Column0_minW_Val
	);

	R2Size_New . VCompose(nextRow . minR2Size_Val);
	
	return SizeInfo
	(
		R2Size_New,
		maxColumn_minW
	);	
}

#if 1
void
RebMenus :: SizeInfo :: Compose
(
	const SizeInfo& other,
	enum Direction theD,
	enum Alignment // theAl
)
{
	if ( theD == D_Vert )
	{
		minR2Size_Val . VCompose
		(
			other . minR2Size_Val
		);		
	}
	else
	{
		minR2Size_Val . HCompose
		(
			other . minR2Size_Val
		);
	}

	Column0_minW_Val = 
	(
		(Column0_minW_Val > other . Column0_minW_Val)
		?
		Column0_minW_Val
		:
		other . Column0_minW_Val
	);
}


#endif



// class Appearance
// public:
void
RebMenus :: Appearance :: Diagnostic(void) const
{
	if ( bHasBitmap )
	{
		textprint("bitmap:\"%s\"; ",BMapN_Val . Get());
	}
	else
	{
		textprint("plain; ");
	}

	if ( bHasTextLabel )
	{
		SCString& theLabel = StringTable :: GetSCString
		(
			ID_Label_Val
		);

		textprint
		(
			"label=\"%s\"\n",
			theLabel . pProjCh()
		);

		theLabel . R_Release();
	}
	else
	{
		textprint("unlabelled\n");
	}
}

// Methods relating to rendering:
void
RebMenus :: Appearance :: Render
(
	const RenderContext& theContext,
	OurBool bSelected
) const
{
	if
	(
		bSelected
	)
	{
		textprintXY
		(
			theContext . Pos() . x-20,
			theContext . Pos() . y,
			"*"			
		);
	}

	if ( bHasTextLabel )
	{
		SCString& theLabel = StringTable :: GetSCString
		(
			ID_Label_Val
		);

		#if 1
		IndexedFont* pFont = IndexedFont :: GetFont
		(
			Fonts :: GetIndex
			(
				bSelected
			)
		);
			// can return NULL if no font loaded in that slot
		GLOBALASSERT( pFont );

		r2pos R2Pos_Cursor = theContext . Pos();

		pFont -> RenderString_Clipped
		(
			R2Pos_Cursor,
			theContext . ClipRect(),
			Label_FixP_Alpha,
			theLabel
		);
		#else
		textprintXY
		(
			theContext . Pos() . x,
			theContext . Pos() . y,
			theLabel . pProjCh()
		);
		#endif

		theLabel . R_Release();

	}

	if ( bHasBitmap )
	{
		#if 1
		textprintXY
		(
			theContext . Pos() . x,
			theContext . Pos() . y,
			"bitmap:\"%s\"; ",
			BMapN_Val . Get()
		);
		#endif

		Bitmap :: Blit_Transparent
		(
			BMapN_Val . Get(), // const char* const pCh_Name,
			theContext . ClipRect(), // r2rect R2Rect_Clip,
			theContext . Pos() // r2pos dstR2Pos
		);
	}
}

RebMenus :: SizeInfo
RebMenus :: Appearance :: GetSizeInfo(void) const
{
	r2size runningR2Size;
	int runningColumn0;

	if ( bHasTextLabel )
	{
		SCString& theLabel = StringTable :: GetSCString
		(
			ID_Label_Val
		);

		r2size R2Size_Label = theLabel . CalcSize
		(
			Fonts :: GetIndex
			(
				Yes
					// for the moment
			)
		);

		theLabel . R_Release();

		runningR2Size = R2Size_Label;
		runningColumn0 = R2Size_Label . w + ColumnSpacing;
	}
	else
	{
		runningR2Size = r2size(0,0);
		runningColumn0 = 0;
	}

	SizeInfo theSizeInfo
	(
		runningR2Size,
		runningColumn0
	);

	if ( bHasBitmap )
	{
		theSizeInfo = theSizeInfo . Overlay
		(
			SizeInfo
			(
				Bitmap :: GetSize
				(
					BMapN_Val . Get()
				),
				0
			)
		);
	}

	return theSizeInfo;
}

RebMenus :: SizeInfo
RebMenus :: OnOffAppearance :: GetSizeInfo(void) const
{
	SizeInfo SizeInfo_On = App_On_Val . GetSizeInfo();
	SizeInfo SizeInfo_Off = App_Off_Val . GetSizeInfo();

	return SizeInfo_On . Overlay(SizeInfo_Off);
}


void
RebMenus :: Item :: DiagnosticAppearance(OurBool bSelected) const
{
	theOnOffApp_Val . GetAppearance(bSelected) . Diagnostic();
}



// Combined factory/"add to list" methods:
RebMenus :: Item_Unimplemented*
RebMenus :: CompositeItem :: AddUnimplementedItem
(
	OnOffAppearance theOnOffApp
)
{
	Item_Unimplemented* pItem = new Item_Unimplemented
	(
		theOnOffApp
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item_Command*
RebMenus :: CompositeItem :: AddNewCommandItem
(
	OnOffAppearance theOnOffApp,
	Command* pCommand
)
{
	GLOBALASSERT( pCommand );

	Item_Command* pItem = new Item_Command
	(
		theOnOffApp,
		pCommand
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item_Command*
RebMenus :: CompositeItem :: AddNewJumpItem
(
	OnOffAppearance theOnOffApp,
	enum PageID theDst
)
{
	Command_JumpToPage* pCommand = new Command_JumpToPage
	(
		theDst
	);
	
	Item_Command* pItem = new Item_Command
	(
		theOnOffApp,
		pCommand
	);

	pCommand -> R_Release();

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item_Selection*
RebMenus :: CompositeItem :: AddNewSelectionItem
(
	OnOffAppearance theOnOffApp,
	enum Direction initDir,
	SelectionVariable* pSelectionVar
)
{
	Item_Selection* pItem = new Item_Selection
	(
		theOnOffApp,
		initDir,
		pSelectionVar
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item_Slider*
RebMenus :: CompositeItem :: AddNewSliderItem
(
	OnOffAppearance theOnOffApp,
	enum Direction initDir,
	BoundedExportVariable<int>* pBoundedExpVar
)
{
	Item_Slider* pItem = new Item_Slider
	(
		theOnOffApp,
		initDir,
		pBoundedExpVar
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item_Toggle*
RebMenus :: CompositeItem :: AddNewToggleItem
(
	OnOffAppearance theOnOffApp_Label,
	enum Direction initDir,
	OnOffAppearance theOnOffApp_Choice,
	ExportVariable<OurBool>* pExpVar
)
{
	Item_Toggle* pItem = new Item_Toggle
	(
		theOnOffApp_Label,
		initDir,
		theOnOffApp_Choice,
		pExpVar
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: CompositeItem*
RebMenus :: CompositeItem :: AddNewCompositeItem
(
	OnOffAppearance theOnOffApp,
	enum Direction theD,
	enum Alignment theAlignment_New
)
{
	CompositeItem* pItem = new CompositeItem
	(
		theOnOffApp,
		theD,
		theAlignment_New
	);

	AddToList
	(
		pItem
	);

	return pItem;
}

RebMenus :: Item*
RebMenus :: CompositeItem :: AddNewCancelItem
(
	OnOffAppearance theOnOffApp
)
{
	Command_ReturnFromPage* pCommand = new Command_ReturnFromPage();

	Item* pItem = new Item_Command
	(
		theOnOffApp,
		pCommand
	);

	pCommand -> R_Release();

	AddToList
	(
		pItem
	);

	return pItem;
}

void
RebMenus :: CompositeItem :: AddNewItem_Special(Item* pItem)
{
	GLOBALASSERT(pItem);

	AddToList(pItem);
}


// Process various keypresses:
OurBool
RebMenus :: CompositeItem :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed

	if ( NumItems >0 )
	{
		// Pass to selected child for processing:
		if
		(
			!( GetSelected() -> Navigate( aNavOp ) )
		)
		{
			// then selected child couldn't process it, this composite should have a go:
			switch ( aNavOp )
			{
			case NavOp_Up:
				{
					if ( theDir == D_Vert )
					{
						if ( SelectedItem > 0)
						{
							SelectedItem--;
							GetSelected() -> Selected_Hook
							(
								aNavOp
							);
							return Yes;
						}
						else
						{
							if ( Wraparound :: bEnabled() )
							{
								SelectedItem = NumItems-1;
								GetSelected() -> Selected_Hook
								(
									aNavOp
								);
								return Yes;
							}
							else
							{
								return No;
							}
						}
					}
					else
					{
						return No;
					}
				}
			case NavOp_Down:
				{
					if ( theDir == D_Vert )
					{
						if ( SelectedItem < (NumItems-1))
						{
							SelectedItem++;
							GetSelected() -> Selected_Hook
							(
								aNavOp
							);
							return Yes;
						}
						else
						{
							if ( Wraparound :: bEnabled() )
							{
								SelectedItem = 0;
								GetSelected() -> Selected_Hook
								(
									aNavOp
								);
								return Yes;
							}
							else
							{
								return No;
							}
						}
					}
					else
					{
						return No;
					}
				}

			case NavOp_Left:
				{
					if ( theDir == D_Horiz )
					{
						if ( SelectedItem > 0)
						{
							SelectedItem--;
							GetSelected() -> Selected_Hook
							(
								aNavOp
							);
							return Yes;
						}
						else
						{
							if ( Wraparound :: bEnabled() )
							{
								SelectedItem = NumItems-1;
								GetSelected() -> Selected_Hook
								(
									aNavOp
								);
								return Yes;
							}
							else
							{
								return No;
							}
						}
					}
					else
					{
						return No;
					}
				}
			case NavOp_Right:
				{
					if ( theDir == D_Horiz )
					{
						if ( SelectedItem < (NumItems-1))
						{
							SelectedItem++;
							GetSelected() -> Selected_Hook
							(
								aNavOp
							);
							return Yes;
						}
						else
						{
							if ( Wraparound :: bEnabled() )
							{
								SelectedItem = 0;
								GetSelected() -> Selected_Hook
								(
									aNavOp
								);
								return Yes;
							}
							else
							{
								return No;
							}
						}
					}
					else
					{
						return No;
					}
				}

			case NavOp_Home:
				{
					SelectedItem=0;
					GetSelected() -> Selected_Hook
					(
						aNavOp
					);
					return Yes;
				}

			case NavOp_End:
				{
					SelectedItem=(NumItems-1);
					GetSelected() -> Selected_Hook
					(
						aNavOp
					);
					return Yes;
				}

			case NavOp_Trigger:
				{
					return No;
				}

			case NavOp_Cancel:
				{
					Page :: TryToReturn();
					return Yes;
				}

			default:
				GLOBALASSERT(0);
			}
		}
	}
	// else: no items to navigate

	if ( aNavOp == NavOp_Cancel )
	{
		// don't need any items for this to work
		Page :: TryToReturn();
		return Yes;
	}


	return No;

}

void
RebMenus :: CompositeItem :: Diagnostic(OurBool bSelected) const
{
	{
		textprint("CompositeItem ::Diagnostic()... ");

		DiagnosticAppearance(bSelected);
	}

	textprint("NumItems=%i\n",NumItems);
	
	for (int i=0;i<NumItems;i++)
	{
		textprint
		(
			(i==SelectedItem)
			?
			"-->"
			:
			"   "
		);

		pItem_A[i]->Diagnostic(i==SelectedItem);
			// or should the "selected" nature also depend on whether the
			// parent is selected?
	}
}

RebMenus :: Item*
RebMenus :: CompositeItem :: GetSelected(void) const
{
	// will return NULL iff there are no items in the composition
	if (NumItems>0)
	{
		return pItem_A[SelectedItem];
	}
	else
	{
		return NULL;
	}
}

// Methods relating to rendering:
void
RebMenus :: CompositeItem :: Render
(
	const RenderContext& theContext,
	OurBool bSelected
) const
{
	r2size R2Size_Composite = GetSizeInfo() . GetMin();
	int nonIterationAxisSize_Composite =
	(
		theDir == D_Vert
		?
		R2Size_Composite . w
		:
		R2Size_Composite . h
	);

	// Always render appearance (the title) centred
	{
		SizeInfo titleSizeInfo = AppearanceSizeInfo();

		r2pos titlePos = theContext . Pos();

		if ( theDir == D_Vert )
		{
			titlePos . x += ( R2Size_Composite . w - titleSizeInfo . GetMin() . w)/2;
		}
		else
		{
			titlePos . y += ( R2Size_Composite . h - titleSizeInfo . GetMin() . h)/2;
		}

		RenderContext titleContext = RenderContext
		(
			theContext . ClipRect(), // r2rect R2Rect_Clip_New,
			titlePos, // r2pos R2Pos_At_New,
				// top-left hotspot
			0 // int Column0_W_New
				// width of 1st column
		);

		RenderAppearance
		(
			titleContext,
			bSelected
		);
	}

	
	// Iterate through items, calling Render():
	r2pos R2Pos_I
	(
		theContext . Pos()
	);

	// Adjust position for any title:
	{
		SizeInfo titleSizeInfo = AppearanceSizeInfo();

		if ( theDir == D_Horiz )
		{
			R2Pos_I . x += titleSizeInfo . GetMin() . w + ItemSpacing();
		}
		else
		{
			R2Pos_I . y += titleSizeInfo . GetMin() . h + ItemSpacing();
		}
	}

	#if 0
	SizeInfo thisSizeInfo = GetSizeInfo();

	R2Pos_I . x -= (thisSizeInfo . GetMin() . w/2);
	R2Pos_I . y -= (thisSizeInfo . GetMin() . h/2);
	#endif

	for (int i=0;i<NumItems;i++)
	{
		textprintXY(R2Pos_I.x,R2Pos_I.y,"%i",i);

		int theIterationAxisPos_I =
		(
			theDir == D_Vert
			?
			R2Pos_I . y
			:
			R2Pos_I . x
		);
		int nonIterationAxisPos_I =
		(
			theDir == D_Vert
			?
			R2Pos_I . x
			:
			R2Pos_I . y
		);

		r2size R2Size_I = pItem_A[i] -> GetSizeInfo() . GetMin();

		int nonIterationAxisSize_I =
		(
			theDir == D_Vert
			?
			R2Size_I . w
			:
			R2Size_I . h
		);

		switch ( theAlignment_Val )
		{
		case Align_Min:
			{
				// means "left-aligned" for V-composition, "top-aligned" for H-Composition
				// so leave untouched
			}
			break;

		case Align_Centre:
			{
				// means "centred about X" for V-composition, "centred about Y" for H-Composition
				// so centre the nonIterationAxis
				nonIterationAxisPos_I += ( nonIterationAxisSize_Composite - nonIterationAxisSize_I)/2;
					// could be negative; this equation ought to still be valid
			}
			break;

		case Align_Max:
			{
				// increase the nonIterationAxis to maximal value that will fit
			   nonIterationAxisPos_I += ( nonIterationAxisSize_Composite - nonIterationAxisSize_I);
					// could be negative; this equation ought to still be valid
			}
			break;

		default:
			GLOBALASSERT(0);
			break;
		}

		r2pos R2Pos_ToUse =
		(
			theDir == D_Vert
			?
			r2pos( nonIterationAxisPos_I, theIterationAxisPos_I )
			:
			r2pos( theIterationAxisPos_I, nonIterationAxisPos_I )
		);

		RenderContext yourContext
		(
			theContext.ClipRect(), // r2rect R2Rect_Clip_New,
			R2Pos_ToUse, // r2pos R2Pos_At_New
			theContext.Column0_W()
		);

		pItem_A[i] -> Render
		(
			yourContext,
			(
				bSelected && (SelectedItem == i)
			)
		);		

		if ( theDir == D_Horiz )
		{
			R2Pos_I . x += R2Size_I . w + ItemSpacing();
		}
		else
		{
			R2Pos_I . y += R2Size_I . h + ItemSpacing();
		}
	}
}

RebMenus :: SizeInfo
RebMenus :: CompositeItem :: GetSizeInfo(void) const
{
	SizeInfo SizeInfo_ToReturn
	(	
		#if 1
		r2size(0,0),
		0
		#else
		AppearanceSizeInfo() . GetMin(),
		AppearanceSizeInfo() . GetColumn0_minW()
		#endif
	);

	for (int i=0;i<NumItems;i++)
	{		
		SizeInfo itemSizeInfo = pItem_A[i] -> GetSizeInfo();

		#if 1
		SizeInfo_ToReturn . Compose
		(
			itemSizeInfo,
			theDir,
			theAlignment_Val
		);

		#else
		
		r2size ItemSize_Min = itemSizeInfo . GetMin();		

		if ( theDir == D_Horiz )
		{
			AppSize_Min . HCompose
			(
				ItemSize_Min
			);

			if (i!=0)
			{
				AppSize_Min . w += ItemSpacing();
			}
		}
		else
		{
			AppSize_Min . VCompose
			(
				ItemSize_Min
			);

			if ( max_Column0_minW < itemSizeInfo . GetColumn0_minW() )
			{
				max_Column0_minW = itemSizeInfo . GetColumn0_minW();
			}

			if (i!=0)
			{
				AppSize_Min . h += ItemSpacing();
			}
		}
		#endif
	}

	#if 1
	return SizeInfo_ToReturn;
	#else
	return SizeInfo
	(
		AppSize_Min,
		max_Column0_minW
	);
	#endif
}


// private:
void
RebMenus :: CompositeItem :: AddToList
(
	Item* pItem_ToAdd
)
{
	GLOBALASSERT(NumItems < MAX_ITEMS_PER_LIST );

	pItem_A[ NumItems++ ] = pItem_ToAdd;


	if (NumItems == 1)
	{
		// then this was the first item added:
		SelectedItem = 0;
	}
}

// static
int RebMenus :: CompositeItem :: ItemSpacing(void)
{
	return 2;
}


void
RebMenus :: Page :: MiniStack :: Push(enum PageID in)
{
	if (NumItems < NUM_PAGES_MEMORY)
	{
		theA[ NumItems++ ] = in;
	}
	else
	{
		// one falls off the bottom; shuffle everything up
		GLOBALASSERT( NumItems == NUM_PAGES_MEMORY );

		for (int i=0;i<(NUM_PAGES_MEMORY-1);i++)
		{
			theA[i] = theA[i+1];
		}

		// ...and set the end element:
		theA[NUM_PAGES_MEMORY-1] = in;
	}
}

OurBool
RebMenus :: Page :: MiniStack :: Pop(enum PageID& out)
{
	// return value: was there anything?
	if ( NumItems > 0)
	{
		out = theA[--NumItems];
		return Yes;
	}
	else
	{
		return No;
	}
}

void
RebMenus :: Page :: MiniStack :: Diagnostic(void)
{
	textprint("page stack: num pages = %i\n",NumItems);

	for (int i=0;i<NumItems;i++)
	{
		textprint("entry:%2i page=%2i\n",i,theA[i]);
	}
}

void
RebMenus :: Page :: MiniStack :: Clear(void)
{
	NumItems = 0;
}



// protected:
RebMenus :: Page :: Page
(
	Appearance theApp_New,
	enum Direction theD,
	enum Alignment theAlignment_New,
	PageID theID
) : CompositeItem
	(
		OnOffAppearance
		(
			theApp_New,
			theApp_New
		),
			// for the moment, menu pages have identical appearances whether
			// selected or not.  Would only make a difference if deselected pages
			// animate away rather than instantly vanish
		theD,
		theAlignment_New
	),
	theID_Val(theID)
{
	GLOBALASSERT( pPage[theID_Val] == NULL );

	pPage[theID_Val] = this;
}
RebMenus :: Page :: ~Page()
{
	GLOBALASSERT( pPage[theID_Val] == this );

	pPage[theID_Val] = NULL;
}

void
RebMenus :: Page :: Diagnostic(OurBool bSelected) const
{
	thePageStack . Diagnostic();

	textprint("Menu page:%i\n",theID_Val);

	CompositeItem :: Diagnostic(bSelected);
}

// static
// static
void
RebMenus :: Page :: SelectPage(enum PageID inID)
{
	GLOBALASSERT(inID>=0);
	GLOBALASSERT(inID<NUM_PAGE_IDS);

	thePageStack . Push( SelectedPageID );

	Internal_SelectPage_NoPush( inID );
}

// static
void
RebMenus :: Page :: SelectPage_ClearingStack(enum PageID inID)
{
	GLOBALASSERT(inID>=0);
	GLOBALASSERT(inID<NUM_PAGE_IDS);

	// Clear page stack:
	thePageStack . Clear();

	// Select page:
	Internal_SelectPage_NoPush( inID );
}

// static
void
RebMenus :: Page :: TryToReturn(void)
{
	enum PageID popPage;

	if
	(
		!thePageStack . Pop
		(
			popPage
		)
	)
	{
		textprint("empty page stack; picking initial menu\n");
		#if 1
		popPage = PageID_Initial;
		#else
		popPage = PageID_NoMenu;
		#endif
	}
	
	// Set page, without pushing where we were:
	{
		Internal_SelectPage_NoPush( popPage );
	}
}


RebMenus :: Page*
RebMenus :: Page :: Get(enum PageID inID)
{
	return pPage[ inID ];
}

// static
RebMenus :: Page*
RebMenus :: Page :: GetSelected(void)
{
	return Get(SelectedPageID);
}

// private:
// static 
void
RebMenus :: Page :: Internal_SelectPage_NoPush( enum PageID inID )
{
	// Worker function to call hooks and actually change the page
	// Used by SelectPage() and TryToReturn()
	// Doesn't push current page onto page stack

	// Call hook for leaving the current page:
	GetSelected() -> Hook_LeavingPage();

	// Set the all-important variable:
	SelectedPageID = inID;

	// Call hook for entering the new page:
	GetSelected() -> Hook_EnteringPage();
}


// private:
// static
RebMenus :: Page* RebMenus :: Page :: pPage[ NUM_PAGE_IDS ];
// static
enum PageID RebMenus :: Page :: SelectedPageID = PageID_Initial;
// static
RebMenus :: Page :: MiniStack
RebMenus :: Page :: thePageStack;


// class Wraparound
// static
const OurBool RebMenus :: Wraparound :: bEnabled_Val = Yes;

// Globals within RebMenus:
void RebMenus :: Init(void)
{
	ProjectSpecific :: Init();
}

void RebMenus :: UnInit(void)
{
	ProjectSpecific :: UnInit();

	// Destroy all menu pages:
	{
		
	}
}

void RebMenus :: Maintain(void)
{
	textprint("RebMenus::Maintain()\n");

	ProjectSpecific :: Maintain();

	#if 1
	{
		Page* pPage = Page :: GetSelected();
		GLOBALASSERT( pPage );

		pPage -> Diagnostic(Yes);
	}
	#endif
}

// static
void
RebMenus :: Render(void)
{
	textprint("RebMenus :: Render()\n");

	Page* pPage = Page :: GetSelected();
	GLOBALASSERT( pPage );

	SizeInfo theSizeInfo = pPage -> GetSizeInfo();

	pPage -> Render
	(
		RenderContext
		(
			r2rect::PhysicalScreen(), // r2rect R2Rect_Clip_New,

			r2rect::PhysicalScreen() . CentredWithSize_TL
			(
				theSizeInfo . GetMin()
			), // r2pos R2Pos_At_New

			theSizeInfo . GetColumn0_minW() // int Column0_W_New
		),
		Yes //OurBool bSelected		
	);	
}


// namespace Bitmap
namespace RebMenus
{
	namespace Bitmap
	{
		class Implementation
		{
		private:
			class CacheEntry
			{
			public:
				CacheEntry
				(
					const char* const pCh_Name_New
				);
				~CacheEntry();

				static void Empty(void)
				{
					while (List_pCacheEntry . size() > 0)
					{
						delete List_pCacheEntry . first_entry();
					}
				}

				void Blit_Transparent
				(
					r2rect R2Rect_Clip,
					r2pos dstR2Pos
				);

				r2size GetSize(void) const
				{
					return R2Size;
				}
				
				static CacheEntry* Get(const char* const pCh_Name);

			private:
				const char* const pCh_Name_Val;
				LPDIRECTDRAWSURFACE pSurface;
				AW_BACKUPTEXTUREHANDLE hBackup;
				r2size R2Size;

				static List<CacheEntry*> List_pCacheEntry;
					// maintains a set of all of these
					// really ought to do this as a hash table (potentially using Jake's
					// template), but no need for the moment
			};
		private:
			


		public:
			static void Precache(const char* const pCh_Name)
			{
				new CacheEntry(pCh_Name);
			}

			static void UnCache(const char* const pCh_Name)
			{
				CacheEntry* pEntry = CacheEntry :: Get(pCh_Name);

				if (!pEntry) { return; }

				delete pEntry;
			}

			static void EmptyCache(void)
			{
				CacheEntry :: Empty();
			}

			static void Blit_Transparent
			(
				const char* const pCh_Name,
				r2rect R2Rect_Clip,
				r2pos dstR2Pos
			)
			{
				CacheEntry* pEntry = CacheEntry :: Get(pCh_Name);

				if (!pEntry)
				{
					pEntry = new CacheEntry(pCh_Name);
				}

				GLOBALASSERT( pEntry );

				pEntry -> Blit_Transparent
				(
					R2Rect_Clip,
					dstR2Pos
				);
			}				    
			static r2size GetSize(const char* const pCh_Name)
			{
				CacheEntry* pEntry = CacheEntry :: Get(pCh_Name);

				if (!pEntry)
				{
					pEntry = new CacheEntry(pCh_Name);
				}
				
				GLOBALASSERT( pEntry );

				return pEntry -> GetSize();
			}

		};
	};
};

RebMenus :: Bitmap :: Implementation :: CacheEntry :: CacheEntry
(
	const char* const pCh_Name_New
) : pCh_Name_Val(pCh_Name_New)
{
	unsigned nWidth,nHeight;
		
	pSurface = AwCreateSurface
	(
		"sfXYB",
		pCh_Name_Val,
		(
			0
		), // flags
		&nWidth,
		&nHeight,
		&hBackup
	);
	ATIncludeSurface(pSurface,hBackup);	


	R2Size . w = nWidth;
	R2Size . h = nHeight;

	List_pCacheEntry . add_entry(this);
}

RebMenus :: Bitmap :: Implementation :: CacheEntry :: ~CacheEntry()
{
	List_pCacheEntry . delete_entry(this);

	GLOBALASSERT(pSurface);
	ATRemoveSurface(pSurface);
	ReleaseDDSurface(pSurface);
	pSurface = NULL;
	
	if (hBackup)
	{
		AwDestroyBackupTexture(hBackup);
	}
	
	hBackup = NULL;

}

void
RebMenus :: Bitmap :: Implementation :: CacheEntry :: Blit_Transparent
(
	r2rect R2Rect_Clip,
	r2pos dstR2Pos
)
{
	if
	(
		r2rect
		(
			dstR2Pos,
			GetSize()			
		) . bFitsIn( R2Rect_Clip )
	)
	{
		RECT destRect;

		destRect.left = dstR2Pos . x;
		destRect.top = dstR2Pos . y;

		destRect.right = dstR2Pos . x + R2Size . w;
		destRect.bottom = dstR2Pos . y + R2Size . h;

		
		RECT srcRect;

		srcRect.left =0;
		srcRect.top =0;
		srcRect.right = R2Size . w;
		srcRect.bottom = R2Size . h;

		#if 1
		DDBLTFX tempDDBltFx;

		memset(&tempDDBltFx,0,sizeof(DDBLTFX));

		tempDDBltFx . dwSize = sizeof(DDBLTFX);
		tempDDBltFx . ddckSrcColorkey . dwColorSpaceLowValue = 0;
		tempDDBltFx . ddckSrcColorkey . dwColorSpaceHighValue = 0;
		#endif

		HRESULT ddrval = lpDDSBack->Blt
		(
			&destRect,
			pSurface,
			#if 1 
			NULL,
			#else
			&srcRect,
			#endif
			(
				DDBLT_WAIT
				#if 0
				| DDBLT_KEYSRC
				#else
				| DDBLT_KEYSRCOVERRIDE 
				#endif
			),
			&tempDDBltFx // LPDDBLTFX lpDDBltFx 
		);

		#if 0
		if(ddrval != DD_OK)
		{
			ReleaseDirect3D();
			exit(0x666009);
		}
		#endif
	}
	// else reject as I can't be bothered to write clipping code
}

// static
RebMenus :: Bitmap :: Implementation :: CacheEntry*
RebMenus :: Bitmap :: Implementation :: CacheEntry :: Get(const char* const pCh_Name)
{
	GLOBALASSERT( pCh_Name );

	// A simple linear search for now:
	for
	(
		LIF<CacheEntry*>oi(&List_pCacheEntry);
		!oi . done();
		oi . next()
	)
	{
		if
		(
			strcmp
			(
				pCh_Name,
				oi() -> pCh_Name_Val
			) == 0
		)
		{
			return oi();
		}
	}
	return NULL;
}

// static
List<RebMenus :: Bitmap :: Implementation :: CacheEntry*>
RebMenus :: Bitmap :: Implementation :: CacheEntry :: List_pCacheEntry;


void
RebMenus :: Bitmap :: Precache(const char* const pCh_Name)
{
	GLOBALASSERT( pCh_Name );

	Implementation :: Precache(pCh_Name);
}

void
RebMenus :: Bitmap :: UnCache(const char* const pCh_Name)
{
	GLOBALASSERT( pCh_Name );
	Implementation :: UnCache(pCh_Name);
}

void
RebMenus :: Bitmap :: EmptyCache(void)
{
	Implementation :: EmptyCache();
}
void
RebMenus :: Bitmap :: Blit_Transparent
(
	const char* const pCh_Name,
	r2rect R2Rect_Clip,
	r2pos dstR2Pos
)
{
	GLOBALASSERT( pCh_Name );
	Implementation :: Blit_Transparent
	(
		pCh_Name,
		R2Rect_Clip,
		dstR2Pos
	);
}

r2size
RebMenus :: Bitmap :: GetSize(const char* const pCh_Name)
{
	GLOBALASSERT( pCh_Name );

	return Implementation :: GetSize(pCh_Name);
}

#endif
	// UseRebMenus





/* Internal function definitions ***********************************/
