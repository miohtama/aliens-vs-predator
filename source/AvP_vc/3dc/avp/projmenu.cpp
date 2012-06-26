/*******************************************************************
 *
 *    DESCRIPTION: 	projmenu.cpp
 *
 *		Project-specific menu code
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 23/3/98 
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"

	#include "rebmenus.hpp"

	#if UseRebMenus
		#include "rebitems.hpp"

		#include "menudefs.h"
		#include "psnd.h"

		#include "tallfont.hpp"
		#include "strtab.hpp"

		#include "module.h"
			// to include "gamedef.h"

		#include "gamedef.h"

		// Network code includes:
		#include "dp_func.h"

			#include "stratdef.h"
			#include "equipmnt.h"
				// needed to include pldnet.h				

		#include "pldnet.h"

		#include "avppages.hpp"
		#include "avpitems.hpp"

		#include "db.h"

		#include "intro.hpp"
				
		#define UseLocalAssert Yes
		#include "ourasert.h"

	#else // UseRebMenus

		#include "menugfx.h"
			// defn of the MENU_GRAPHIC struct since we export one of
			// these for the intros in any case

	#endif // UseRebMenus

/* Version settings ************************************************/

/* Constants *******************************************************/

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern unsigned char KeyboardInput[];
		#include "smacker.h"
#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/
// 30/3/98 DHM:
// This bitmap used to be wrapped with OverrideOldMenus and UseRebMenus but is now
// also used by the intro code:
MENUGRAPHIC Starfield_Backdrop = 
{
	#if 1
	"Graphics\\NewMenus\\StarField.rim",	-1,	NULL, NULL, 0, 0,  640, 480,
	#else
	"Graphics\\NewMenus\\TestBack.rim",	-1,	NULL, NULL, 0, 0,  640, 480,
	#endif
};

/* Internal type definitions ***************************************/
#if UseRebMenus
// Additional implementation-specific code for the RebMenus namespace: Input handling
namespace RebMenus
{
	class InputHandler
	{
	public:
		static void Maintain(void);

		static OurBool bPressedKey
		(
			unsigned char& outKey
		);
			// return val= was a key pressed; if so, output area is written to

	private:
		static OurBool bLastFrame[NUM_NAVIGATION_OPS];
		static int KeyForNavOp[NUM_NAVIGATION_OPS];
	};
}; // end of namespace RebMenus

// Additional project-specific menu state:
namespace RebMenus
{
	namespace ProjectSpecific
	{
		class Networking
		{
		public:
			#if 0
			static void MenuLoopMaintenance(void)
			#endif
		private:
		};
	};
};
#endif
	// UseRebMenus

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Function definitions ***********************************/
#if UseRebMenus
// namespace RebMenus
#if OverrideOldMenus
// class MenuLoop
// static
void
RebMenus :: MenuLoop :: StartSoloGame
(
	I_PLAYER_TYPE inPlayerType,
	I_AVP_ENVIRONMENTS inStartingEnv
)
{
	AvP . PlayerType = inPlayerType;
	AvP . CurrentEnv = AvP.StartingEnv = inStartingEnv;
	AvP . Network = I_No_Network;

	SetExit
	(
		ExitReason_StartGame
	);

	Page :: SelectPage(PageID_NoMenu);
}

// static
void
RebMenus :: MenuLoop :: StartNetworkGame
(
	I_PLAYER_TYPE inPlayerType,
	I_AVP_ENVIRONMENTS inStartingEnv
)
{
	AvP . PlayerType = inPlayerType;
	AvP . CurrentEnv = AvP.StartingEnv = inStartingEnv;
	
	SetExit
	(
		ExitReason_StartGame
	);
}
#endif
	// OverrideOldMenus

// namespace ProjectSpecific
void
RebMenus :: ProjectSpecific :: Init(void)
{
	// Create the menu page singletons:
	new Page_NoMenu();
	new Page_Initial();
	new Page_ChooseCharacter();
	new Page_Options();
	new Page_VideoOptions();
	new Page_AudioOptions();

	new Page_LevelsOfDetail();
	new Page_MarineBriefing();
	new Page_PredatorBriefing();
	new Page_AlienBriefing();
	new Page_LoadGame();
	new Page_InputOptions();
	new Page_ConfigMouse();

	new Page_PlaceholderMultiplayer();
	new Page_MultiplayerErrorScreen();
}

void
RebMenus :: ProjectSpecific :: UnInit(void)
{
}

void
RebMenus :: ProjectSpecific :: Maintain(void)
{
	InputHandler :: Maintain();
}


// Additional implementation-specific code for the RebMenus namespace:
// class InputHandler
// public:
// static
void
RebMenus :: InputHandler :: Maintain(void)
{
	if ( Item_KeyConfig_PageView :: ExpectingKey() )
	{
		// Then the key configuration system intercepts the input; the player
		// is redefining a key:
		Item_KeyConfig_PageView :: Maintain();
	}
	else
	{
		// Otherwise, proceed normally:
		#if 0
		if ( KeyboardInput[ KEY_1 ] )
		#endif
		{
			for
			(
				int i=0;
				i < NUM_NAVIGATION_OPS;
				i++
			)
			{
				enum NavigationOp theNavOp = static_cast<enum NavigationOp>(i);

				OurBool bThisFrame = KeyboardInput
				[
					KeyForNavOp[theNavOp]
				];

				if (bThisFrame)
				{
					textprint("undebounced NavOp:%i\n",theNavOp);
				}
				
				// Call the key-handlers for "key down" transitions:
				if
				(
					bThisFrame
					&&
					( !bLastFrame[ theNavOp ] )
				)
				{
					// Recompute here (in case page changes mid-computation)
					Page* pPage = Page :: GetSelected();
					GLOBALASSERT( pPage );

					pPage -> Navigate( theNavOp );
				}	

				bLastFrame[ theNavOp ] = bThisFrame;
			}
		}
	}
}

// static
void
RebMenus :: Item_KeyConfig_PageView :: Maintain(void)
{
	GLOBALASSERT( pActive );
	unsigned char theKey;

	if ( InputHandler :: bPressedKey( theKey ) )
	{
		if (pActive -> bDebounced )
		{
			pActive -> bDebounced = No;
			pActive -> SetMethod(theKey);
		}
	}
	else
	{
		pActive -> bDebounced = Yes;
	}		
}


// static
OurBool
RebMenus :: InputHandler :: bPressedKey
(
	unsigned char& outKey
)
{
	// return val= was a key pressed; if so, output area is written to

	#if ( MAX_NUMBER_OF_INPUT_KEYS >= 255 )
		#error Range problem in this routine; it will need rewriting
	#endif

	for (unsigned char key = 0 ; key <= MAX_NUMBER_OF_INPUT_KEYS ; key++)
	{
		if (!(key == KEY_ESCAPE) &&
			!(key >= KEY_F1 && key <= KEY_F12) &&
			!(key >= KEY_0 && key <= KEY_9) )
		{
			if ( KeyboardInput[key] )
			{
				outKey = key;
				return Yes;
			}
		}
	}

	return No;
}




// private:
// static
OurBool
RebMenus :: InputHandler :: bLastFrame[NUM_NAVIGATION_OPS];

// static
int
RebMenus :: InputHandler :: KeyForNavOp[NUM_NAVIGATION_OPS] =
{
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_HOME,
	KEY_END,
	KEY_CR,
		// NavOp_Trigger

	#if 1
	KEY_ESCAPE
	#else
	KEY_R
	#endif
		// NavOp_Cancel 
		// for the moment

};

// end of namespace RebMenus

void
RebMenus :: PrecacheGraphics(void)
{
	#if OverrideOldMenus
	// Grab all the bitmaps we'll ever need now, before the music starts
	// so as to avoid sound glitches:

	// Graphics used by menus in general:
	{
		// Sliders:
		{
			Bitmap :: Precache("Graphics\\NewMenus\\SliderBar.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\SliderBarDark.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\Slider.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\SliderDark.rim");
		}
	}

	// Graphics used by specific menu pages:
	{
		// Start solo game:
		{
			Bitmap :: Precache("Graphics\\NewMenus\\Alien.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\AlienDark.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\MarineDark.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\Predator.rim");
			Bitmap :: Precache("Graphics\\NewMenus\\PredatorDark.rim");
		}
	}
	#endif
}

#if OverrideOldMenus
int REBMENUS_DoTheMenus(void)
{
	// This code created from the routines in PCMENUS.CPP:
	RebMenus :: InGame :: Set_OutsideTheGame();

	ResetFrameCounter();
	
	PlatformSpecificEnteringMenus();

	// Ensure r2rect for screen size is properly setup:
	{
		R2BASE_ScreenModeChange_Cleanup();
	}

	// Experiment to get WAV file play working:
	{
		#if 0
		BOOL bSuccess = PlaySound
		(
			"NewMenus\\Intro.wav", // LPCSTR pszSound, 
			NULL, // HMODULE hmod, 
			(
				SND_ASYNC 
				| SND_FILENAME
			) // DWORD fdwSound 
		);

		db_logf1(("PlaySound() returned %i",bSuccess));
		#endif
	}

	#if SupportTextprintBeforeGame
	// Load debugging font:
	{
		#if 0
		InitialiseImageHeaders();
			// some kind of init needed ???
		#else
		InitialiseTextures();
		#endif
	}
	#endif

	// Load the starfield backdrop graphic
	// Ultimately will want to replace with e.g. an FMV sequence init call
	{
		LoadMenuGraphic(&Starfield_Backdrop);
	}

	// Load the fonts:
	{
		#if 1
		IndexedFont_Kerned_Column :: Create
		(
			IntroFont_Light, // FontIndex I_Font_New,
			"Graphics\\NewMenus\\IntroFont.rim",
			21, // int HeightPerChar_New,
			5, // int SpaceWidth_New,
			32 // ASCIICodeForInitialCharacter
		);

		IndexedFont_Kerned_Column :: Create
		(
			IntroFont_Dark, // FontIndex I_Font_New,
			"Graphics\\NewMenus\\IntroFontDark.rim",
			21, // int HeightPerChar_New,
			5, // int SpaceWidth_New,
			32 // ASCIICodeForInitialCharacter
		);
		#else
		IndexedFont_Proportional_Column :: Create
		(
			IntroFont_Light, // FontIndex I_Font_New,
			"Graphics\\NewMenus\\IntroFont.rim",
			21, // int HeightPerChar_New,
			5, // int SpaceWidth_New,
			32 // ASCIICodeForInitialCharacter
		);

		IndexedFont_Proportional_Column :: Create
		(
			IntroFont_Dark, // FontIndex I_Font_New,
			"Graphics\\NewMenus\\IntroFontDark.rim",
			21, // int HeightPerChar_New,
			5, // int SpaceWidth_New,
			32 // ASCIICodeForInitialCharacter
		);
		#endif
	}

	// 30/3/98 DHM: Avoid menu sound glitches by doing all the loading now:
	{
		RebMenus :: PrecacheGraphics();
	}

	
	/* KJL 15:00:30 28/03/98 - hook to play the intro sequence
	
	This call checks to see if this is the first time into the
	menus, and plays the intro accordingly
	
	*/
	PlayIntroSequence();


	RebMenus :: MenuLoop :: Start();

	while ( RebMenus :: MenuLoop :: bStillGoing() )
	{
		#if 0
		// Allow ALT+TAB
		{
			CheckForWindowsMessages();
		}
		#endif

		// Network messaging; replaces code from RunMultiPlayerStartUp()
		// see pp89-91 of DHMS's AvP book
		{
			if
			(
				( AvP.Network != I_No_Network )
				&&
				( netGameData . myGameState == NGS_Joining )
			)
			{
				NetCollectMessages();
			}

			if
			(
				( AvP.Network != I_No_Network )
				&&
				( netGameData . myGameState == NGS_Joining )
			)
			{
				/* we are still in start-up after collecting our messages */
				if(AvP.Network==I_Host)
				{
					AddNetMsg_GameDescription();
				}
				else
				{
					AddNetMsg_PlayerDescription();
				}

		 		NetSendMessages();
			}

		}


		// Do stuff to replace DrawEntireMenuScreen()
		{
			// DrawMenuBackdrop(); ultimately called BLTMenuToScreen()
			// Could replace with a call to render a new flat backdrop
			// eg Al's pretty starfield.
			// Ultimately might play an FMV sequence
			{
				// For now:
				{
					BLTMenuToScreen
					(
						&Starfield_Backdrop
					);
				}
			}
	
			/* play music */
			PlayMenuMusic();

			// Add stuff relating to current menu state:
			{
				RebMenus :: Render();
			}

			// For now, manually flush textprint buffer:
			{
				#if SupportTextprintBeforeGame
				FlushTextprintBuffer();
				#endif
			}

			// Flip buffers etc:
			{
				FlipBuffers();
			}
		}
		
		 
		// Keep input handling code up-to-date:
		{
			ReadUserInput();
		}

		// Process user input and update bExit accordingly
		{
			RebMenus :: Maintain();
		}	
	
		// Update sound system:
		{
			SoundSys_Management();
		}

		// Handle timing:
		{
			FrameCounterHandler();
		}

		// Handle possible requests to run the multiplayer dialog
		{
			Command_Multiplayer :: EndOfMenuLoopProcessing();
		}

	} // end of while loop
		

	// Set some globals:
	{
		#if 0
		AvP . PlayerType = I_Marine;
		AvP . CurrentEnv = AvP.StartingEnv;
		AvP . Network = I_No_Network;
		#endif
			// the above are now set in the MenuLoop::StartGame() method

	}

	IndexedFont :: UnloadFont( IntroFont_Dark );
	IndexedFont :: UnloadFont( IntroFont_Light );

	// Unload any graphics loaded for menu items:
	{
		RebMenus :: Bitmap :: EmptyCache();
	}

	// Unload the starfeild backdrop; replace with FMV unloading calls
	{
		ReleaseMenuGraphic
		(
			&Starfield_Backdrop
		);
	}

	/* play music */
	EndMenuMusic();
	
	PlatformSpecificExitingMenus();

	RebMenus :: InGame :: Set_InTheGame();

	return RebMenus :: MenuLoop :: MenuRoutineReturnVal();

}
#endif
	// OverrideOldMenus

// Pause hook, used by PLAYER.C:
void REBMENUS_ProcessPauseRequest(void)
{
	textprint("REBMENUS_ProcessPauseRequest()\n");

	Command* pCommand = new Command_ExitCurrentGame();
	pCommand -> Execute();

	pCommand -> R_Release();
}
#endif
	// UseRebMenus

void REBMENUS_ProjectSpecific_EndOfMainLoopHook(void)
{
	#if UseRebMenus
	RebMenus :: Bitmap :: EmptyCache();
	#endif

	// Set menu page to initial one; ultimately might want to set
	// a briefing screen for next level at this point etc.
	#if UseRebMenus
	RebMenus :: Page :: SelectPage_ClearingStack( PageID_Initial );
	#endif
}

