/*******************************************************************
 *
 *    DESCRIPTION: 	rebitems.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 9/3/98
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"

	#include "rebitems.hpp"

	#if UseRebMenus
		#include "strtab.hpp"
		#include "indexfnt.hpp"

		#include "module.h"
			// to include stratdef.h

		#include "stratdef.h"
			// to include usr_io.h

		#include "usr_io.h"

		#include "db.h"

		#define UseLocalAssert Yes
		#include "ourasert.h"

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
#if UseRebMenus
// static 
const FontIndex RebMenus :: Fonts :: LabelFont_InGame = DATABASE_MESSAGE_FONT;
// static 
const FontIndex RebMenus :: Fonts :: LabelFont_OutOfGame_Selected = IntroFont_Light;
const FontIndex RebMenus :: Fonts :: LabelFont_OutOfGame_Unselected = IntroFont_Dark;
#endif

/* Internal type definitions ***************************************/

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
#if UseRebMenus
// Derived classes for the item types:
//class Item_Unimplemented : public Item
// public:
// Process various keypresses:
OurBool
RebMenus :: Item_Unimplemented :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch ( aNavOp )
	{
		case NavOp_Trigger:
			textprint("this is an unimplemented item\n");
			return Yes;

		case NavOp_Up:
		case NavOp_Down:
		case NavOp_Left:
		case NavOp_Right:
		case NavOp_Home:
		case NavOp_End:
		case NavOp_Cancel:
			// No way of processing
			return No;

		default:
			GLOBALASSERT(0);
	}
	return No;
}

void
RebMenus :: Item_Unimplemented :: Diagnostic(OurBool bSelected) const
{
	textprint("Unimplemented:");

	DiagnosticAppearance(bSelected);
}

// Methods relating to rendering:
void
RebMenus :: Item_Unimplemented :: Render
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
}

RebMenus :: SizeInfo
RebMenus :: Item_Unimplemented :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo();
}
// public:
// Process various keypresses:
OurBool
RebMenus :: Item_Command :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch ( aNavOp )
	{
		case NavOp_Trigger:
			return pCommand_Val -> Execute();

		case NavOp_Up:
		case NavOp_Down:
		case NavOp_Left:
		case NavOp_Right:
		case NavOp_Home:
		case NavOp_End:
		case NavOp_Cancel:
			// No way of processing
			return No;

		default:
			GLOBALASSERT(0);
	}
	return No;
}

void
RebMenus :: Item_Command :: Diagnostic(OurBool bSelected) const
{
	textprint("Item_Command:");

	DiagnosticAppearance(bSelected);
}
// contains a command; can contain a "goto menu page" command

// Methods relating to rendering:
void
RebMenus :: Item_Command :: Render
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
}

RebMenus :: SizeInfo
RebMenus :: Item_Command :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo();
}


// class Item_Selection : public Item
// public:
#if 1
// Process various keypresses:
OurBool
RebMenus :: Item_Selection :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch ( aNavOp )
	{
		case NavOp_Up:
			if ( theDir == D_Vert )
			{
				Dec();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Down:
			if ( theDir == D_Vert )
			{
				Inc();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Left:
			if ( theDir == D_Horiz )
			{
				Dec();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Right:
			if ( theDir == D_Horiz )
			{
				Inc();
				return Yes;
			}
			else
			{
				return No;
			}
		case NavOp_Home:
			Min();
			return Yes;

		case NavOp_End:
			Max();
			return Yes;

		case NavOp_Trigger:
			Inc();
			return Yes;

		case NavOp_Cancel:
			// No way of processing
			return No;

		default:
			GLOBALASSERT(0);
	}
	return No;
}
#endif

void
RebMenus :: Item_Selection :: Diagnostic(OurBool bSelected) const
{
	textprint("Item_Selection: ");

	#if 1
	DiagnosticAppearance(bSelected);
	#endif

}

void
RebMenus :: Item_Selection :: Min(void)
{
	textprint("Min\n");

	pSelectionVar_Val -> Set
	(
		pSelectionVar_Val -> GetMin()
	);
}

void
RebMenus :: Item_Selection :: Max(void)
{
	textprint("Max\n");

	pSelectionVar_Val -> Set
	(
		pSelectionVar_Val -> GetMax()
	);
}

void
RebMenus :: Item_Selection :: Inc(void)
{
	textprint("Inc\n");

	pSelectionVar_Val -> Set
	(
		pSelectionVar_Val -> GetNxt()
	);

}

void
RebMenus :: Item_Selection :: Dec(void)
{
	textprint("Dec\n");

	pSelectionVar_Val -> Set
	(
		pSelectionVar_Val -> GetPrv()
	);
}

// Methods relating to rendering:
void
RebMenus :: Item_Selection :: Render
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

	pSelectionVar_Val -> GetAppearance( bSelected ) . Render
	(
		valueContext,
		bSelected
	);
}

RebMenus :: SizeInfo
RebMenus :: Item_Selection :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo();
}


// class Item_Slider : public Item
// public:
// Process various keypresses:
OurBool
RebMenus :: Item_Slider :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch ( aNavOp )
	{
		case NavOp_Up:
			if ( theDir == D_Vert )
			{
				Inc();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Down:
			if ( theDir == D_Vert )
			{
				Dec();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Left:
			if ( theDir == D_Horiz )
			{
				Dec();
				return Yes;
			}
			else
			{
				return No;
			}

		case NavOp_Right:
			if ( theDir == D_Horiz )
			{
				Inc();
				return Yes;
			}
			else
			{
				return No;
			}
		case NavOp_Home:
			SetToMax();
			return Yes;

		case NavOp_End:
			SetToMin();
			return Yes;

		case NavOp_Trigger:
			Inc();
			return Yes;

		case NavOp_Cancel:
			// No way of processing
			return No;

		default:
			GLOBALASSERT(0);
	}
	return No;
}

void
RebMenus :: Item_Slider :: Diagnostic(OurBool bSelected) const
{
	textprint("Item_Slider:");

	#if 1
	DiagnosticAppearance(bSelected);
	#endif
}

// Handy ways to process discrete movements of the slider
void
RebMenus :: Item_Slider :: SetToMin(void)
{
	textprint("SetToMin\n");

	pBoundedExpVar_Val -> Set
	(
		pBoundedExpVar_Val -> GetMin()
	);
}

void
RebMenus :: Item_Slider :: SetToMax(void)
{
	textprint("SetToMax\n");

	pBoundedExpVar_Val -> Set
	(
		pBoundedExpVar_Val -> GetMax()
	);
}

void
RebMenus :: Item_Slider :: Inc(void)
{
	textprint("Inc\n");

	// The Set() method silently rejects attempt to set outside range
	// For this reason, if close to the bounding value,
	// set direct to bounding value
		
	int NewVal = pBoundedExpVar_Val -> Get() + GetFraction();

	int Bound = pBoundedExpVar_Val -> GetMax();

	pBoundedExpVar_Val -> Set
	(	
		(NewVal < Bound) ? NewVal : Bound
	);
}

void
RebMenus :: Item_Slider :: Dec(void)
{
	textprint("Dec\n");

	// Analogous to the Inc() method above

	int NewVal = pBoundedExpVar_Val -> Get() - GetFraction();

	int Bound = pBoundedExpVar_Val -> GetMin();

	pBoundedExpVar_Val -> Set
	(	
		(NewVal > Bound) ? NewVal : Bound
	);
}

// Methods relating to rendering:
void
RebMenus :: Item_Slider :: Render
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

	RenderContext sliderContext = theContext . NextColumn();

	float floatPos =
	(
		float( GetVal() - GetMin() )
		/
		float( GetRange() )
	);

	RenderSlider
	(
		sliderContext,
		bSelected,
		floatPos
	);

	#if 0
	textprintXY
	(
		sliderContext . Pos() . x,
		sliderContext . Pos() . y,
		"                   [%i,%i] range(%i) val(%i)",
		GetMin(),
		GetMax(),
		GetRange(),
		GetVal()

	);
	#endif
}

RebMenus :: SizeInfo
RebMenus :: Item_Slider :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo() . AddColumn
	(
		SizeInfo
		(
			Bitmap :: GetSize("Graphics\\NewMenus\\SliderBar.rim"),
			0
		)
	);
}

// private:
// static
void
RebMenus :: Item_Slider :: RenderSlider
(
	const RenderContext& theContext,
	OurBool bSelected,
	float floatAmount
)
{
	#if 1
	Bitmap :: Blit_Transparent
	(
		(
			bSelected
			?
			"Graphics\\NewMenus\\SliderBar.rim" // const char* const pCh_Name,
			:
			"Graphics\\NewMenus\\SliderBarDark.rim" // const char* const pCh_Name,
		),
		theContext . ClipRect(),
		theContext . Pos()
	);
	Bitmap :: Blit_Transparent
	(
		(
			bSelected
			?
			"Graphics\\NewMenus\\Slider.rim" // const char* const pCh_Name,
			:
			"Graphics\\NewMenus\\SliderDark.rim" // const char* const pCh_Name,
		),
		theContext . ClipRect(),
		r2pos
		(
			(
				theContext . Pos() . x + 3 +
				(
					int(200.0f * floatAmount)
				)
			),
			(
				theContext . Pos() . y + 4
			)
		)
	);
	#else
	textprintXY
	(
		theContext . Pos() . x,
		theContext . Pos() . y,
		"[---------------]"
	);

	const int TotalWidth = (16*CharWidth);
	int Offset =
	(
		int(float(TotalWidth) * floatAmount)
	);

	textprintXY
	(
		theContext . Pos() . x + Offset,
		theContext . Pos() . y,
		"|"
	);
	#endif
}


// class Item_Toggle : public Item
// public:
// Process various keypresses:
OurBool
RebMenus :: Item_Toggle :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	switch ( aNavOp )
	{
		case NavOp_Up:
		case NavOp_Down:
			if ( theDir == D_Vert )
			{
				Toggle();
				return Yes;
			}
			else
			{
				return No;
			}
		case NavOp_Left:
		case NavOp_Right:
			if ( theDir == D_Horiz )
			{
				Toggle();
				return Yes;
			}
			else
			{
				return No;
			}
		case NavOp_Trigger:
			Toggle();
			return Yes;

		case NavOp_Home:
			TurnOn();
			return Yes;

		case NavOp_End:
			TurnOff();
			return Yes;

		case NavOp_Cancel:
			// No way of processing
			return No;

		default:
			GLOBALASSERT(0);
	}
	return No;
}

void
RebMenus :: Item_Toggle :: Diagnostic(OurBool bSelected) const
{
	textprint("Item_Toggle:");

	DiagnosticAppearance(bSelected);

	theOnOffApp_Val . GetAppearance
	(
		pExpVar_Val -> Get()
	) . Diagnostic();
}

void
RebMenus :: Item_Toggle :: Toggle(void)
{
	OurBool bCurrent = pExpVar_Val -> Get();
	pExpVar_Val -> Set
	(
		!bCurrent
	);
}

void
RebMenus :: Item_Toggle :: TurnOn(void)
{
	pExpVar_Val -> Set(Yes);
}

void
RebMenus :: Item_Toggle :: TurnOff(void)
{
	pExpVar_Val -> Set(No);
}

// Methods relating to rendering:
void
RebMenus :: Item_Toggle :: Render
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

	theOnOffApp_Val .GetAppearance
	(
		pExpVar_Val -> Get()
	) . Render
	(
		valueContext,
		bSelected
	);
}

RebMenus :: SizeInfo
RebMenus :: Item_Toggle :: GetSizeInfo(void) const
{
	return AppearanceSizeInfo() . AddColumn
	(
		theOnOffApp_Val . GetSizeInfo()
	);
}

#if 0
// class Item_Table : public Item
// public:
RebMenus :: Item_Table :: Item_Table
(
	OnOffAppearance theOnOffApp_New
) : Item(theOnOffApp_New),
	NumX(0),
	NumY(0),
	SelectedX(0),
	SelectedY(0)
{
}

// Process various keypresses:
OurBool
RebMenus :: Item_Table :: Navigate( enum NavigationOp aNavOp )
{
	// return = was message processed
	return No;
}

void
RebMenus :: Item_Table :: Diagnostic(OurBool bSelected) const
{
}

Item*
RebMenus :: Item_Table :: GetSelected(void) const
{
	// will return NULL iff there are no items in the composition
	if (NumX == 0)
	{
		GLOBALASSERT(NumY==0);
		return NULL;
	}

	GLOBALASSERT( NumY > 0 );

	GLOBALASSERT( NumX < MAX_X );
	GLOBALASSERT( NumY < MAX_Y );

	GLOBALASSERT( SelectedX >= 0 );
	GLOBALASSERT( SelectedY >= 0 );

	GLOBALASSERT( SelectedX < NumX );
	GLOBALASSERT( SelectedY < NumY );

	Item* pItem = pItem_A[SelectedX][SelectedY];

	GLOBALASSERT( pItem );

	return pItem;
}

// private:
void
RebMenus :: Item_Table :: AddRow
(
	Item** ppItem_ToAdd
)
{
	// takes a NULL-terminated list of Item*s

	GLOBALASSERT(NumY < (MAX_Y-1) );
		// can't get any bigger

	int Pos_I = 0;
	
	while ( *ppItem_ToAdd )
	{
		GLOBALASSERT( Pos_I < MAX_X );

		pItem_A
		[
			Pos_I++
		]
		[
			NumY
		]
		=
		(
			*(ppItem_ToAdd++)
		);
	}	

	if (NumX < Pos_I)
	{
	}

	NumY++;
	
}

void
RebMenus :: Item_Table :: AddColumn
(
	Item** ppItem_ToAdd
)
{
	// takes a NULL-terminated list of Item*s
}

#if 0
// List of selectables:
enum {MAX_X = 10};
enum {MAX_Y = 20};
#endif

#endif

#endif
	// UseRebMenus

/* Internal function definitions ***********************************/
