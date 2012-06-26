/*
	
	rebmenus.hpp

*/

#ifndef _rebmenus_hpp
#define _rebmenus_hpp 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

/* Version settings *****************************************************/
	#define UseRebMenus Yes
		// If this is set, then the new menus are compiled in, and take
		// effect in-game
		// If this is set to no, then the new menus don't compile in

	#define OverrideOldMenus Yes
		// If this is set, then the new menus also override the out-of-game
		// menus.  Don't set this without setting UseRebMenus

	#define SupportTextprintBeforeGame	No
		// however, this causes a problem in the second call
		// to InitialiseTextures(), but only outside the debugger,
		// so it's a real pain to debug

/* Includes  *************************************************************/
#if UseRebMenus
	#ifdef __cplusplus
		#ifndef _ourbool
			#include "ourbool.h"
		#endif

		#if 0
			#ifndef _scstring
				#include "scstring.hpp"
			#endif
		#endif

		#ifndef _command
			#include "command.hpp"
		#endif

		#ifndef _r2base
			#include "r2base.h"
		#endif

		#if 0
			// Expects an enum "FontIndex" for the font to be defined in PROJFONT.H
			#ifndef _projfont
				#include "projfont.h"
			#endif
		#endif

		#ifndef _expvar_hpp
			#include "expvar.hpp"
		#endif

		#ifndef _projmenu_hpp
			#include "projmenu.hpp"
				// Include project specific header.
				/*
					It is assumed that this project specific header will define
					the following:

						- an "enum PageID" enumeration of the various pages in the menus;
						they must range in value from 0 inclusive to NUM_PAGE_IDS exclusive
						(currently gaps are allowed in the range, but an array of pointers
						gets defined so don't be excessive)

						- a TextID typedef giving an enumeration of text string IDs from
						the language localisation table

						- functions:
							void Init(void)
							void UnInit(void)
							void Maintain(void)
						within scope ProjectSpecific (either namespace or public static in a class)
						inside namespace RebMenus.  They can be empty, but typically
						you should construct your menu pages within them.

						- a scope (class or namespace) Fonts containing
							( static )
							FontIndex RebMenus :: Fonts :: GetIndex(OurBool bSelected)
						to tell the text code which font to use for labels
				*/

		#endif
	#endif
		/* __cplusplus */
#endif
	// UseRebMenus



/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/
#if UseRebMenus
#ifdef __cplusplus
	#define DUMMY_TEXTID ((TextID)0)

namespace RebMenus
{
	#if 0
	// perhaps has some kind of message passing hook:
	class Message
	{
	};
		// derived classes for keydown messages, mouse messages, etc
		// But how is it interrogated?
		// (you have two class hierarchies, with a single process fn)
		// 
		// What class have I forgotten?

	// need a class with somd kind of hook for either processing
	// a messages, or passing it down the chain of responsibility
	// (a boolean return type to say whether it's been processed?)

	class MessageProcessor
	{
		
	};

	class Selectable : MessageProcessor //?????
	{
	};
	#endif
	enum Direction
	{
		D_Horiz,
		D_Vert
	};

	enum Alignment
	{
		Align_Min,
			// means "left-aligned" for V-composition, "top-aligned" for H-Composition

		Align_Centre,
			// means "centred about X" for V-composition, "centred about Y" for H-Composition

		Align_Max
			// means "right-aligned" for V-composition, "bottom-aligned" for H-Composition

	};

	enum NavigationOp
	{
		NavOp_Up = 0,
		NavOp_Down,
		NavOp_Left,
		NavOp_Right,
		NavOp_Home,
		NavOp_End,
		NavOp_Trigger,
		NavOp_Cancel,

		NUM_NAVIGATION_OPS
	};

	// Name of a bitmap, for referring to bitmaps in an appearance:
	class BitmapName
	{
	public:
		BitmapName
		(
			char* pCh_New
		) : pCh_Val(pCh_New)
		{
		}
		BitmapName() : pCh_Val(NULL)
		{			
		}
		const char* const Get(void) const
		{
			return pCh_Val;
		}

	private:
		const char* const pCh_Val;
	};

	// Name of an FMV sequence, for referring to FMV sequences in an appearance:
	class FMVName
	{
	public:
		FMVName
		(
			char* pCh_New
		) : pCh_Val(pCh_New)
		{
		}
		FMVName() : pCh_Val(NULL)
		{			
		}
		const char* const Get(void) const
		{
			return pCh_Val;
		}

	private:
		const char* const pCh_Val;
	};

	namespace Bitmap
	{
		void Precache(const char* const pCh_Name);
		void UnCache(const char* const pCh_Name);
		void EmptyCache(void);

		void Blit_Transparent
		(
			const char* const pCh_Name,
			r2rect R2Rect_Clip,
			r2pos dstR2Pos
		);

		r2size GetSize(const char* const pCh_Name);
	};

	// Menu-specific commands:
	class Command_JumpToPage : public Command
	{
	public:
		Command_JumpToPage
		(
			enum PageID thePage
		) : Command(),
			thePage_Val(thePage)
		{
		}
		OurBool Execute(void);

	private:
		const enum PageID thePage_Val;
	};

	class Command_ReturnFromPage : public Command
	{
	public:
		OurBool Execute(void);
	};

	// Class for describing size requirements of things in the menus
	// Currently only support a "minimum size" idea
	class SizeInfo
	{
	public:
		SizeInfo
		(
			r2size minR2Size,
			int Column0_minW
		) : minR2Size_Val(minR2Size),
			Column0_minW_Val(Column0_minW)
		{
		}
		r2size GetMin(void) const
		{
			return minR2Size_Val;
		}
		int GetColumn0_minW(void) const
		{
			return Column0_minW_Val;
		}

		SizeInfo Overlay
		(
			SizeInfo other
		);

		SizeInfo AddColumn
		(
			SizeInfo nextColumn
		);

		SizeInfo AddRow
		(
			SizeInfo nextRow
		);

		void Compose
		(
			const SizeInfo& other,
			enum Direction theD,
			enum Alignment theAl
		);
	private:
		r2size minR2Size_Val;
		int Column0_minW_Val;
		
	};


	class RenderContext
	{
	public:
		RenderContext
		(
			r2rect R2Rect_Clip_New,
			r2pos R2Pos_At_New,
				// top-left hotspot
			int Column0_W_New
				// width of 1st column
		) : R2Rect_Clip_Val(R2Rect_Clip_New),
			R2Pos_At_Val(R2Pos_At_New),
			Column0_W_Val(Column0_W_New)
		{
		}
		r2rect ClipRect(void) const {return R2Rect_Clip_Val;}
		r2pos Pos(void) const {return R2Pos_At_Val;}
		int Column0_W(void) const {return Column0_W_Val;}
		r2pos Pos_Column1(void) const
		{
			return r2pos
			(
				R2Pos_At_Val . x + Column0_W_Val,
				R2Pos_At_Val . y
			);
		}

		RenderContext NextColumn(void) const
		{
			return RenderContext
			(
				R2Rect_Clip_Val,
				Pos_Column1(),
				0
			);
		}

	private:
		r2rect R2Rect_Clip_Val;
		r2pos R2Pos_At_Val;
		int Column0_W_Val;
	};

	// 9/3/98 DHM: A "policy" pattern for what an item sould look like:
	// ("Design Patterns" would call it a "strategy", but since that word
	// is already overused in 3DC et al I prefer the alternate name "policy")
	class Appearance
	{
	public:
		// Use just a text label:
		Appearance
		(
			TextID ID_Label
		) : bHasTextLabel(Yes),
			ID_Label_Val(ID_Label),
			bHasBitmap(No),
			bHasFMV(No)
		{
		}

		// Use just a bitmap:
		Appearance
		(
			BitmapName inBMapName
		) : bHasTextLabel(No),
			ID_Label_Val(DUMMY_TEXTID),
			bHasBitmap(Yes),
			BMapN_Val(inBMapName),
			bHasFMV(No)
		{
		}
	
		// Don't have any appearance at all = invisible
		Appearance
		(
		) : bHasTextLabel(No),
			ID_Label_Val(DUMMY_TEXTID),
			bHasBitmap(No),
			bHasFMV(No)
		{
		}

		// Other possible appearances might be:
		// - bitmaps,
		// - fmv sequences,
		// - combos of the above
		// etc

		void Diagnostic(void) const;

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	private:
		const OurBool bHasTextLabel;
		const TextID ID_Label_Val;

		const OurBool bHasBitmap;
		const BitmapName BMapN_Val;

		const OurBool bHasFMV;
		const FMVName FMVN_Val;		
	};

	// An appearance with two states:
	class OnOffAppearance
	{
	public:
		// Constructors
		OnOffAppearance
		(
			Appearance App_On,
			Appearance App_Off
		) : App_On_Val(App_On),
			App_Off_Val(App_Off)
		{
			// most general constructor: independent appearances, one for
			// "on", the other for "off"
		}

		OnOffAppearance
		(
			Appearance App_Shared
		) : App_On_Val(App_Shared),
			App_Off_Val(App_Shared)
		{
			// easy way to give something the same appearance both on and off
		}

		OnOffAppearance
		(
			TextID ID_Label
		) : App_On_Val(Appearance(ID_Label)),
			App_Off_Val(Appearance(ID_Label))
		{
			// easy way of setting up a text label
		}

		OnOffAppearance
		(
		) : App_On_Val(Appearance()),
			App_Off_Val(Appearance())
		{
			// easy way to set up something as invisible both on and off
		}
	

		Appearance GetAppearance(OurBool bOn) const
		{
			return
			(
				bOn
				?
				App_On_Val
				:
				App_Off_Val
			);
		}

		SizeInfo GetSizeInfo(void) const;

	private:
		Appearance App_On_Val;
		Appearance App_Off_Val;
	};

	class SelectionVariable
	{
	protected:
		SelectionVariable
		(
			BoundedExportVariable<int>* pBoundedExpVar
				// takes responsibility for deleting this
		) : pBoundedExpVar_Val(pBoundedExpVar)
		{
		}

	public:
		virtual ~SelectionVariable()
		{
			delete pBoundedExpVar_Val;
		}

		virtual Appearance GetAppearance(OurBool bSelected) = 0;

		// Chain-of-responsibility onto the bounded export var:
		int Get(void) const
		{
			return pBoundedExpVar_Val -> Get();
		}

		int GetMin(void) const
		{
			return pBoundedExpVar_Val -> GetMin();
		}
		int GetMax(void) const
		{
			return pBoundedExpVar_Val -> GetMax();
		}
		int GetNxt(void) const
		{
			int CurrentVal = pBoundedExpVar_Val -> Get();
			if ( CurrentVal < pBoundedExpVar_Val -> GetMax() )
			{
				return ++CurrentVal;
			}
			else
			{
				return pBoundedExpVar_Val -> GetMin();
			}
		}
		int GetPrv(void) const
		{
			int CurrentVal = pBoundedExpVar_Val -> Get();
			if ( CurrentVal > pBoundedExpVar_Val -> GetMin() )
			{
				return --CurrentVal;
			}
			else
			{
				return pBoundedExpVar_Val -> GetMax();
			}
		}
		void Set(int NewVal)
		{
			pBoundedExpVar_Val -> Set(NewVal);
		}

	private:
		BoundedExportVariable<int>* pBoundedExpVar_Val;
	};



	// Class for a list of discrete selections with appearances and values:
	template <class T,int MAX_IN_LIST> class Selection
	{
		class Selectable
		{
		public:
			Selectable
			(
				Appearance theApp,
				T theValue
			) : App_Val(theApp),
				T_Val(theValue)
			{
			}
			Appearance GetAppearance(void) const
			{
				return App_Val;
			}
			T GetValue(void) const
			{
				return T_Val;
			}
		private:
			const Appearance App_Val;
			const T T_Val;
		};

		class SelectionList
		{
		public:
			SelectionList
			(
			) :	NumInList(0)
			{
			}

			~SelectionList();

			AddSelection
			(
				Selectable Sel_ToAdd
			)
			{
				if ( NumInList < MAX_IN_LIST )
				{
					theSel_A[NumInList++] = Sel_ToAdd;
				}
			}
		private:
			int NumInList;
			Selectable theSel_A[MAX_IN_LIST];
		};
	};


	class Item
	{
	public:
		OurBool bSelectable(void);
			// items can be temporarily or permanently unselectable
			// e.g. "greyed out"

	/////////////// PURE VIRTUAL FUNCTIONS ///////////////////////////////
	public:
		// Process various keypresses:
		virtual OurBool Navigate( enum NavigationOp aNavOp ) = 0;
			// return = was message processed

		virtual void Diagnostic(OurBool bSelected) const = 0;

		virtual void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const = 0;

		virtual SizeInfo GetSizeInfo(void) const = 0;


	public:
		void DiagnosticAppearance(OurBool bSelected) const;

		// Methods relating to rendering:
		void RenderAppearance
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const
		{
			theOnOffApp_Val . GetAppearance
			(
				bSelected
			) . Render
			(
				theContext,
				bSelected
			);
		}
		SizeInfo AppearanceSizeInfo(void) const
		{
			return theOnOffApp_Val . GetSizeInfo();
		}


	public:
		virtual void Selected_Hook
		(
			enum NavigationOp // theNavOp
		)
		{
			// empty default implementation
		}

	protected:
		Item
		(
			OnOffAppearance theOnOffApp_New
		) : theOnOffApp_Val(theOnOffApp_New)
		{
			// empty inline constructor; but protected so must use
			// a derived class.
		}

		virtual ~Item()
		{
		}
			
	private:
		OnOffAppearance theOnOffApp_Val;
	};

	// Proposed derived classes of Item:
	class Item_Unimplemented;

	class Item_Command;
		// contains a command; can contain a "goto menu page" command

	class Item_Selection;

	class Item_Slider;

	class Item_Toggle;

	class Item_KeyConfig_PageView;

	// A composition of items.
	// There will exist a selected item iff there are any items contained
	// in this composition
	class CompositeItem : public Item
	{
	public:
		CompositeItem
		(
			OnOffAppearance theOnOffApp_New,
			enum Direction theD,
			enum Alignment theAlignment_New
		) : Item(theOnOffApp_New),
			theDir(theD),
			theAlignment_Val(theAlignment_New),
			NumItems(0)
		{
			// what value should SelectedItem have when there aren't any items?
		}

		// Combined factory/"add to list" methods:
		Item_Unimplemented* AddUnimplementedItem
		(
			OnOffAppearance theOnOffApp
		);

		Item_Command* AddNewCommandItem
		(
			OnOffAppearance theOnOffApp,
			Command* pCommand
		);

		Item_Command* AddNewJumpItem
		(
			OnOffAppearance theOnOffApp,
			enum PageID theDst
		);

		Item_Selection* AddNewSelectionItem
		(
			OnOffAppearance theOnOffApp,
			enum Direction initDir,
			SelectionVariable* pSelectionVar
		);

		Item_Slider* AddNewSliderItem
		(
			OnOffAppearance theOnOffApp,
			enum Direction initDir,
			BoundedExportVariable<int>* pBoundedExpVar
		);

		Item_Toggle* AddNewToggleItem
		(
			OnOffAppearance theOnOffApp_Label,
			enum Direction initDir,
			OnOffAppearance theOnOffApp_Choice,
			ExportVariable<OurBool>* pExpVar
		);

		CompositeItem* AddNewCompositeItem
		(
			OnOffAppearance theOnOffApp,
			enum Direction theD,
			enum Alignment theAlignment_New
		);

		Item* AddNewCancelItem
		(
			OnOffAppearance theOnOffApp
		);

		void AddNewItem_Special(Item* pItem);

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		virtual void Diagnostic(OurBool bSelected) const;

		Item* GetSelected(void) const;
			// will return NULL iff there are no items in the composition

		// Get index of next selectable
		#if 0
			// what if all disabled?

			// what if wraparound turned off?

		ItemIndex NxtSelectable(void) const;
		ItemIndex PrvSelectable(void) const;
		#endif

		// Methods relating to rendering:
		virtual void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		virtual SizeInfo GetSizeInfo(void) const;


	private:
		void AddToList
		(
			Item* pItem_ToAdd
		);
		static int ItemSpacing(void);

		// List of selectables:
		enum {MAX_ITEMS_PER_LIST = 20};

		int NumItems;
		Item* pItem_A[ MAX_ITEMS_PER_LIST ];

		#if 1
		int SelectedItem;
		#else
		// Selected item:
		IndexInRange theIIR;
		#endif

		// Direction:
		const enum Direction theDir;

		// Alignment:
		const enum Alignment theAlignment_Val;

		// selection list contains a list of selectables;
		// has a selected child.

		// Has an "enum direction":
		// if D_Horiz, interprets left/right as moving the selection
		// if D_Vert, interprets up/down as moving the selection

		// Considered doing it with two separate classes, but makes it nasty
		// to derive stuff off them
	
	};


	class Page : public CompositeItem
	{
	// This class maintains a static array
	// mapping of PageIDs to Page*s
	// There can be at most one Page of each ID; the class implements
	// a kind of array of singletons 
	// (See "Design Patterns" Addison-Wesley 1994)

	// The precise classes derived from Page are project-specific.  A project might
	// choose to derive a new class used by all of its pages, or have
	// a separate class for each page, or any combination in between.

		// An internal class for storing a history of menu pages visited:
		class MiniStack
		{
			// A small stack; when it gets full, the items
			// at the bottom are lost
			// Originally done as a template, but Jonathon told be to stop being too
			// clever for my own good...

		public:
			enum { NUM_PAGES_MEMORY = 16 };

		public:
			MiniStack() : NumItems(0)
			{
			}

			void Push(enum PageID in);

			OurBool Pop(enum PageID& out);
				// return value: was there anything?

			void Diagnostic(void);

			void Clear(void);

		private:
			int NumItems;
			enum PageID theA[ NUM_PAGES_MEMORY ];
		};

	protected:
		Page
		(
			Appearance theApp_New,
			enum Direction theD,
			enum Alignment theAlignment_New,
			PageID theID
		);

		~Page();

	public:
		void Diagnostic(OurBool bSelected) const;

		static void SelectPage(enum PageID inID);
		static void SelectPage_ClearingStack(enum PageID inID);
		static void TryToReturn(void);
		
		static Page* Get(enum PageID inID);
		static Page* GetSelected(void);

	private:
		// Every page can have hook functions for entering/leaving the page
		// These are private since they're only to be called by the page selection
		// function
		// Default implementation is empty; specific pages can override this.
		virtual void Hook_EnteringPage(void)
		{
		}
		virtual void Hook_LeavingPage(void)
		{
		}

	private:
		static void Internal_SelectPage_NoPush( enum PageID inID );
			// Worker function to call hooks and actually change the page
			// Used by SelectPage() and TryToReturn()
			// Doesn't push current page onto page stack


	private:

		const PageID theID_Val;
		static Page* pPage[ NUM_PAGE_IDS ];

		static enum PageID SelectedPageID;

		static MiniStack thePageStack;
	};
		

		// What happens if you compose various levels of selection list?
		// 

		// Is a menu page a type of selection list; if so, how does it respond
		// to cursor keys?

		// 

		// now 

	class Wraparound
	{
	public:
		static OurBool bEnabled(void)
		{
			return bEnabled_Val;
		}

	private:
		const static OurBool bEnabled_Val;
	};

	// Globals within RebMenus:
	void Init(void);
	void UnInit(void);
	void Maintain(void);
	void Render(void);


}; // end of namespace RebMenus
#endif // __cplusplus
#endif // UseRebMenus
/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif

		extern void REBMENUS_ProjectSpecific_EndOfMainLoopHook(void);

#ifdef __cplusplus
	};
#endif

/* End of the header ****************************************************/


#endif














