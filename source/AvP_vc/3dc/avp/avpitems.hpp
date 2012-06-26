/*
	
	avpitems.hpp

	AvP-specific menu items

*/

#ifndef _avpitems
#define _avpitems 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

	#ifndef _rebmenus_hpp
		#include "rebmenus.hpp"
	#endif

	#if UseRebMenus
		#ifndef _scstring
			#include "scstring.hpp"
		#endif

		#include "pcmenus.h"

		#ifndef pldnet_h_included
				#include "stratdef.h"
				#include "equipmnt.h"
					// needed to include pldnet.h				

			#include "pldnet.h"
		#endif
	#endif // RebMenus 


#ifdef __cplusplus
///////////////////////////////////////////////////////////////////////////////
// Project-specific item types: ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Additions to the RebMenus namespace: AvP specific item types:
namespace RebMenus
{
	class Item_NetworkStatus : public Item
	{
	public:
		Item_NetworkStatus
		(
			OnOffAppearance theOnOffApp_New
		) : Item
			(
				theOnOffApp_New
			)
		{
		}
		~Item_NetworkStatus()
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

		static TextID GetTextIDForCharacterType
		(
			NETGAME_CHARACTERTYPE characterType
		);
	};

	class Item_NetworkErrorView : public Item
	{
	public:
		Item_NetworkErrorView();
		~Item_NetworkErrorView();

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
		static SCString* GetStringForCurrentNetworkError(void);
		static TextID GetTextIDForCurrentNetworkError(void);
	};

	class Item_VideoModeSelector : public Item
	{
	public:
		Item_VideoModeSelector
		(			
			OnOffAppearance theOnOffApp_New
		);

		// Process various keypresses:
		OurBool Navigate( enum NavigationOp aNavOp );
			// return = was message processed

		void Diagnostic(OurBool bSelected) const;

		void Render
		(
			const RenderContext& theContext,
			OurBool bSelected
		) const;

		SizeInfo GetSizeInfo(void) const;

		void Dec(void);
		void Inc(void);

	};


	class Item_KeyConfig_PageView : public Item
	{
	// DHM 16/3/98: I make a distinction between "methods" and "effects":
	// a "method" is something you use to request an "effect"
	// For example you might use the "method" of the space key to request
	// the effect "jump".

	public:
		Item_KeyConfig_PageView
		(
			OnOffAppearance theOnOffApp_New,
			int NumEffectsPerPage_New
		);
		~Item_KeyConfig_PageView();

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

		void SetMethod(unsigned char theMethod);

		static OurBool ExpectingKey(void)
		{
			// if this is set, then this class takes over input handling
			return (pActive != NULL);
		}

		static void Maintain(void);

		int GetPage(void) const
		{
			return CurrentPage_Val;
		}
		int GetNumPages(void) const
		{
			return NumPages_Val;
		}
		void NxtPage(void);
		void PrvPage(void);

		// It's a singleton class:
		static Item_KeyConfig_PageView* Get(void)
		{
			return pSingleton;
		}

		static SCString* GetMethodString( unsigned char inPhysicalKey );

	public:
		void Selected_Hook
		(
			enum NavigationOp theNavOp
		);


	private:
		static TextID GetEffectLabel( enum KeyConfigItems );
			// takes an effect (e.g. jump) and returns
			// a text label to use

		static OurBool GetKeyLabel
		(
			int inPhysicalKey,
			TextID& outTextID
		);
			// takes a physical method key and attempts to find a text
			// string to use for it, returning whether it does.
			// If it fails, output area is untouched

		OurBool GetEffectForRowOnPage
		(
			int inY,
			int inPage,
			enum KeyConfigItems& outEffect
		) const;
			// returns truth if it can output a valid effect into the specified area

		int GetFinalYForPage
		(
			int inPage
		) const;
			// Y on this page can range from 0 to this inclusive

	private:
		// Selection: only meaningful if the item as a whole is selected
		int SelectionX;
			// range: [0,1]

		int SelectionY;
			// range: [0,NumEffectsPerPage_Val)

		static Item_KeyConfig_PageView* pActive;
			// iff this is non-NULL, then there's one of these expecting a key
			// to change the ketboard setup;  change input handling accordingly

		OurBool bDebounced;

		const int NumEffectsPerPage_Val;
		const int NumPages_Val;

		int CurrentPage_Val;
			// ranges from [0,NumPages_Val)

		static Item_KeyConfig_PageView* pSingleton;

	};

	class Item_KeyConfig_Selector : public CompositeItem
	{
	public:
		Item_KeyConfig_Selector
		(
			OnOffAppearance theOnOffApp_New
		);		
	private:
		
	};

	// Some simple command classes to embed specific function calls into buttons:
	class Command_KeyConfig_PrvPage : public Command
	{
	public:
		OurBool Execute(void)
		{
			Item_KeyConfig_PageView :: Get() -> PrvPage();

			return Yes;
		}
	};
	class Command_KeyConfig_NxtPage : public Command
	{
	public:
		OurBool Execute(void)
		{
			Item_KeyConfig_PageView :: Get() -> NxtPage();

			return Yes;
		}
	};

};



/* End of the header ****************************************************/
#endif // __cplusplus


#endif
