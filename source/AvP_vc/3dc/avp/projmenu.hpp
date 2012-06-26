/*
	
	projmenu.hpp

	New menu system for AvP.

	This file created 23/3/98 by DHM, by extracting project specific bits
	out of other headers.
	
	I've attempted to impose a split between project-specific and project-independent
	portions of the code.  This is perhaps a little academic given that the code is (so
	far) just used by AvP.  Still, Devil's Own have requested I try to lock off the
	project-specific bits.  I hope I've done a good job.

	Some of the content of this file is required by REBMENUS.HPP; see comment
	at the top of that header file.  Other content is specific to getting the
	code to work both in the game and outside, given that the code has to work
	ASAP within an extensive framework of existing code.

*/

#ifndef _projmenu_hpp
#define _projmenu_hpp 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

	#if UseRebMenus

		#ifndef _projfont
			#include "projfont.h"
		#endif

		#ifndef GAMEDEF_INCLUDED
				#ifndef MODULE_INCLUDED
					#include "module.h"
						// needed to include gamedef.h
				#endif
				
			#include "gamedef.h"
		#endif

	#endif

/* Version settings *****************************************************/

/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/
#if UseRebMenus
	enum PageID
	{
		PageID_NoMenu,

		PageID_Initial,


		PageID_ChooseCharacter,
		PageID_MarineBriefing,
		PageID_PredatorBriefing,
		PageID_AlienBriefing,

		PageID_LoadGame,

		PageID_Options,
		PageID_VideoOptions,
		PageID_AudioOptions,
		PageID_InputOptions,
		
		PageID_LevelsOfDetail,

		PageID_MouseConfig,

		PageID_PlaceholderMultiplayer,
		PageID_MultiplayerErrorScreen,

		NUM_PAGE_IDS
	};

	// Grab Avp's string table enum, and typedef "TextID" to it:
	#ifndef _langenum_h_
		#include "langenum.h"
	#endif

	typedef enum TEXTSTRING_ID TextID;

	#ifdef __cplusplus
	// Extend the RebMenus namespace with stuff to handle an out-of-the-game
	// menu loop for AvP, plus some project-supplied hooks needed for the menu code
	namespace RebMenus
	{
		namespace ProjectSpecific
		{
			void Init(void);
			void UnInit(void);
			void Maintain(void);
		};

		#if OverrideOldMenus
		// Additions to the RebMenus namespace: handling a loop outside the main game
		class MenuLoop
		{
			// This class handles the exit condition from the out-of-game menu loop
			// Functions are provided to request exit next time round the loop, giving
			// a reason why the loop is to terminate
		public:
			
			enum ExitReason
			{
				ExitReason_None,
				ExitReason_QuitProgram,
				ExitReason_StartGame
			};

			static void Start(void)
			{
				theExitReason_Val = ExitReason_None;
			}

			static OurBool bStillGoing(void)
			{
				return ( ExitReason_None == theExitReason_Val );
			}

			static int MenuRoutineReturnVal(void)
			{
				// value to be returned by the dedicated menu loop to WinMain()
				if ( theExitReason_Val == ExitReason_QuitProgram )
				{
					// Then the while( themenus ) loop should receive a false value
					// which will kill the main loop
					return No;
				}
				else
				{
					// Then the while( themenus ) loop should receive a true value
					// which will pass control into the innards of the main loop
					return Yes;
				}
			}

			static void QuitProgram(void)
			{
				SetExit
				(
					ExitReason_QuitProgram
				);
			}

			static void StartSoloGame
			(
				I_PLAYER_TYPE inPlayerType,
				I_AVP_ENVIRONMENTS inStartingEnv
			);

			static void StartNetworkGame
			(
				I_PLAYER_TYPE inPlayerType,
				I_AVP_ENVIRONMENTS inStartingEnv
			);


		private:
			static void SetExit
			(
				enum ExitReason theExitReason_New
			)
			{
				theExitReason_Val = theExitReason_New;
			}

		private:
			static enum ExitReason theExitReason_Val;
		};

		// A class to handle knowledge for the menus of whether
		// they are being run outisde the game in their own loop,
		// or within the main game loop:
		class InGame
		{
		public:
			static void Set_InTheGame(void)
			{
				bInTheGame_Val = Yes;
			}
			static void Set_OutsideTheGame(void)
			{
				bInTheGame_Val = No;
			}

			static OurBool Get(void)
			{
				return bInTheGame_Val;
			}

		private:
			static OurBool bInTheGame_Val;
		};
		#endif
			// OverrideOldMenus

		// A class required by the project-independent menu code
		// to tell label rendering which fonts to use, and when:
		class Fonts
		{
		private:
			static const FontIndex LabelFont_InGame;
			static const FontIndex LabelFont_OutOfGame_Selected;
			static const FontIndex LabelFont_OutOfGame_Unselected;

		public:
			static FontIndex GetIndex
			(
				#if OverrideOldMenus
				OurBool bSelected
				#else
				OurBool
				#endif
			)
			{
				#if OverrideOldMenus
				if (InGame::Get())
				{
					return LabelFont_InGame;
				}
				else
				{
					if (bSelected)
					{
						return LabelFont_OutOfGame_Selected;
					}
					else
					{
						return LabelFont_OutOfGame_Unselected;
					}
				}
				#else
				return LabelFont_InGame;
				#endif
			}
		};

		const int Label_FixP_Alpha = ONE_FIXED;
		const int ColumnSpacing = 15;

		void PrecacheGraphics(void);

	};
	#endif
#endif // UseRebMenus
/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
#ifdef __cplusplus
	extern "C" {
#endif
		// Function call hook for AvP:
		extern int REBMENUS_DoTheMenus(void);

		// Pause hook, used by PLAYER.C:
		extern void REBMENUS_ProcessPauseRequest(void);
#ifdef __cplusplus
	};
#endif



/* End of the header ****************************************************/

#endif
