/*
	
	rebitems.hpp

	Derived classes for the item types; all except CompositeItem go in this
	file.
*/

#ifndef _rebitems_hpp
#define _rebitems_hpp 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

	#ifndef _rebmenus_hpp
		#include "rebmenus.hpp"
	#endif

	#if UseRebMenus
		#ifndef _included_pcmenus_h_
			#include "pcmenus.h"
				// for enum KeyConfigItems 
		#endif

		#ifndef _scstring
			#include "scstring.hpp"
		#endif
	#endif

/* Version settings *****************************************************/

/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/
#if UseRebMenus
// Derived classes for the item types:
namespace RebMenus
{
	class Item_Unimplemented : public Item
	{
	public:
		Item_Unimplemented
		(
			OnOffAppearance theOnOffApp
		) : Item
			(
				theOnOffApp
			)
		{
		}

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	};

	class Item_Command : public Item
	{
	public:
		Item_Command
		(
			OnOffAppearance theOnOffApp,
			Command* pCommand
		) : Item
			(
				theOnOffApp
			),
			pCommand_Val(pCommand)
		{
			pCommand_Val -> R_AddRef();
		}

		~Item_Command()
		{
			pCommand_Val -> R_Release();
		}

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	private:
		Command *const pCommand_Val;		
	};
		// contains a command; can contain a "goto menu page" command

	class Item_Selection : public Item
	{
	public:
		Item_Selection
		(
			OnOffAppearance theOnOffApp,
			enum Direction initDir,
			SelectionVariable* pSelectionVar
		) : Item
			(
				theOnOffApp
			),
			theDir(initDir),
			pSelectionVar_Val(pSelectionVar)
		{
		}

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		void Min(void);
		void Max(void);
		void Inc(void);
		void Dec(void);

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	private:
		// Direction:
		const enum Direction theDir;
		SelectionVariable* const pSelectionVar_Val;

	};

	class Item_Slider : public Item
	{
	public:
		Item_Slider
		(
			OnOffAppearance theOnOffApp,
			enum Direction initDir,
			BoundedExportVariable<int>* pBoundedExpVar
				// takes responsibility for deallocating this
		) : Item
			(
				theOnOffApp
			),
			theDir(initDir),
			pBoundedExpVar_Val(pBoundedExpVar)
		{
		}

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		// Handy ways to process discrete movements of the slider
		void SetToMin(void);
		void SetToMax(void);
		void Inc(void);
		void Dec(void);

		// Handy ways to values of the slider:
		int GetMin(void) const
		{
			return pBoundedExpVar_Val -> GetMin();
		}
		int GetMax(void) const
		{
			return pBoundedExpVar_Val -> GetMax();
		}
		int GetRange(void) const
		{
			return ( GetMax()  - GetMin() );
		}
		int GetVal(void) const
		{
			return pBoundedExpVar_Val -> Get();
		}

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	private:
		static void RenderSlider
		(
			const RenderContext& theContext,
			OurBool bSelected,
			float floatPos
		);

		int GetFraction(void) const
		{
			// returns a fractional amout good for a small increment/decrement

			int Amt = (GetRange() / 32);

			return ( (Amt>1) ? Amt : 1);
				// ensure amount is at least 1
		}

	private:
		// Direction:
		const enum Direction theDir;
		BoundedExportVariable<int>* const pBoundedExpVar_Val;

	};

	class Item_Toggle : public Item
	{
	public:
		Item_Toggle
		(
			OnOffAppearance theOnOffApp_Label,

			enum Direction initDir,
			OnOffAppearance theOnOffApp_Choice,
			ExportVariable<OurBool>* pExpVar
				// takes responsibility for deallocating this
		) : Item
			(
				theOnOffApp_Label
			),
			theDir(initDir),
			theOnOffApp_Val(theOnOffApp_Choice),
			pExpVar_Val(pExpVar)
		{
		}

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		void Toggle(void);
		void TurnOn(void);
		void TurnOff(void);

		// Methods relating to rendering:
		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

	private:
		// Direction:
		const enum Direction theDir;

		OnOffAppearance theOnOffApp_Val;

		ExportVariable<OurBool>* pExpVar_Val;
	};

	#if 0
	// A 2d analogue of the composite item:
	class Item_Table : public Item
	{
	public:
		Item_Table
		(
			OnOffAppearance theOnOffApp_New
		);

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		Item* GetSelected(void) const;
			// will return NULL iff there are no items in the composition

	private:
		#if 1
		void AddControlConfigRow();
		#else
		void AddRow
		(
			Item** ppItem_ToAdd
		);
			// takes a NULL-terminated list of Item*s

		void AddColumn
		(
			Item** ppItem_ToAdd
		);
			// takes a NULL-terminated list of Item*s
		#endif

		// List of selectables:
		enum {MAX_X = 10};
		enum {MAX_Y = 20};

		int NumX;
		int NumY;
		Item* pItem_A[ MAX_X ][ MAX_Y ];

		int SelectedX;
		int SelectedY;

	};
	#endif


	#if 0
	// Decorator contains another item, and passes on calls to it ??
	class Item_Decorator_VScroll : public Item
	{
	public:
	private:
	};
	#endif

};
#endif // UseRebMenus

/* Exported globals *****************************************************/

/* Function prototypes **************************************************/



/* End of the header ****************************************************/
#endif
