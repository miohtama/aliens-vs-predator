/***** Menus.c *****/

#include "3dc.h"
#include "inline.h"
#include "module.h"

#include "stratdef.h"
#include "gamedef.h"

#include "menudefs.h"
#include "bh_types.h"
#include "equipmnt.h"
#include "weapons.h"
#include "inventry.h"
#include "saveload.h"

#define UseLocalAssert Yes
#include "ourasert.h"

#if SupportWindows95
#include "dp_func.h"
#include "multmenu.h"
#endif

#include "psnd.h"

#include "rebmenus.hpp"

int menustate;

#define CANCEL -1

int LoadGame;

// this tells us if the item is selected for each list
int CurrentMenuStatus[128];

extern int IDemandSelect(void);
extern int IDemandCancel(void);

extern AVP_GAME_DESC AvP;
extern int executeDemo;
extern int NormalFrameTime;

/*-------------------------------------------*/

static int ChooseGamestartMenu(void);
static void ChooseDemoLevel(void);

int StartUpMenus(void) 
{
#if OverrideOldMenus
	/*
		New version 18/3/98 by DHM:

		Passes most of the work control to a new function
		(so that I can avoid having to touch this code)
	*/
	AvP.GameMode = I_GM_Menus;
	executeDemo = 0;

	AvP.ElapsedSeconds = 0;
	AvP.ElapsedMinutes = 0;
	AvP.ElapsedHours = 0;

	{
		int RebMenusResult = REBMENUS_DoTheMenus();

		AvP.GameMode = I_GM_Playing;

		return RebMenusResult;
	}
	

#else
	int playGameOnExitFromMenus=1;
	int func_output;

	AvP.GameMode = I_GM_Menus;
	executeDemo = 0;

	PlatformSpecificEnteringMenus();

	// if we are here, then we must have been killed, exited or
	// be starting a new game, hence we can:

	// initialise 
	FormatSaveBuffer();
	AvP.ElapsedSeconds = 0;
	AvP.ElapsedMinutes = 0;
	AvP.ElapsedHours = 0;
 
	menustate=CHOOSE_GAMESTART;

	/* special test to see if we've been lobbied... */
	#if SupportWindows95
	if(CheckForAVPLobbyLaunch())
	{
		/* aha, we've been lobbied: go directly to multiplayer menus. */
						
		if(RunMultiplayerStartUp(1))	
		{
			playGameOnExitFromMenus = 1;
			menustate = END;
		}
		else
		{
			playGameOnExitFromMenus = 0;
			menustate = END;
		}
	}
	#endif

	/***** Anything linear and pre-character choice in here. *****/
	while (menustate!=END) 
	{
		switch (menustate) 
		{
			case CHOOSE_GAMESTART:
			{
				LoadGame=0;

				func_output=ChooseGamestartMenu();

				/* Jump to LoadGame, Options or ChooseCharacter.         *
				 * Sorry, you only get to choose language once. */
				
				switch (func_output) 
				{
					case NEW_GAME_ITEM:
						menustate=CHOOSE_CHARACTER;
						break;
					case OPTIONS_ITEM:
						menustate=CHOOSE_OPTIONS;
						break;
					case LOAD_GAME_ITEM:
						if (LoadGameMenu()) 
						{
							LoadGame=1;
							menustate=END;
						} 
						break;
					#if SupportWindows95
					case MULTIPLAYER_ITEM:
					{
						/* we keep the multiplayer stuff away from the PSX and Saturn */
						if(RunMultiplayerStartUp(0))	
						{
							playGameOnExitFromMenus = 1;
							menustate = END;
						}
						else
						{
							menustate = CHOOSE_GAMESTART;
						}
						break;							
					}

					case DEMO_ITEM:
					{
						menustate = END;
						playGameOnExitFromMenus = 1;	
						executeDemo = 1;	

						ChooseDemoLevel();
				 		AvP.CurrentEnv = AvP.StartingEnv;
					
						break;
					}								

					case QUIT_ITEM:
					{
						menustate = END;
						playGameOnExitFromMenus = 0;
						break;
					}
					#endif

					default:
						/* Too bad. */
						break;
				}		

				break;
			}
			case CHOOSE_OPTIONS:
			{
				func_output=OptionsMenu();
				menustate=CHOOSE_GAMESTART;

				break;
			}
			case CHOOSE_CHARACTER:
			{
				func_output=ChooseCharacterMenu();
				switch (func_output) 
				{
					case ALIEN_ITEM:
						AvP. PlayerType = I_Alien;
						menustate=ALIEN_BRIEFING;
						break;
					case MARINE_ITEM:
						AvP. PlayerType = I_Marine;
						menustate=MARINE_BRIEFING;
						break;
					case PREDATOR_ITEM:
						AvP. PlayerType = I_Predator;
						menustate=PREDATOR_BRIEFING;
						break;
					case EXIT_ITEM:	
						menustate=CHOOSE_GAMESTART;
						break;
				}											
				break;
			}
			case ALIEN_BRIEFING:
			{
				func_output=AlienBriefingMenu();
				switch (func_output) 
				{
					case EXIT_ITEM_ALIEN:
						menustate=CHOOSE_CHARACTER;
						break;
					default:
						AvP.StartingEnv = I_Medlab;
						AvP.CurrentEnv = AvP.StartingEnv;
						menustate=END;
						break;
				}
				break;
			}
			case PREDATOR_BRIEFING:
			{
				func_output=PredatorBriefingMenu();
				switch (func_output) 
				{
					case EXIT_ITEM_PREDATOR:
						menustate=CHOOSE_CHARACTER;
						break;
					default:
						AvP.StartingEnv = I_Cmc4;
						AvP.CurrentEnv = AvP.StartingEnv;
						menustate=END;
						break;
				}
				break;
			}
			case MARINE_BRIEFING:
			{
				func_output=MarineBriefingMenu();
				switch (func_output) 
				{
					case EXIT_ITEM_MARINE:
						menustate=CHOOSE_CHARACTER;
						break;
					default:
						#if PSX_DEMO
						AvP.StartingEnv = I_Gen1;
						#else
						AvP.StartingEnv = I_Entrance;
						#endif
						AvP.CurrentEnv = AvP.StartingEnv;
						menustate=END;
						break;
				}
				break;
			}
			default:
				/* Eh? Better crash here. */
				GLOBALASSERT(1==2);
				break;
		}

	}
	PlatformSpecificExitingMenus();
	
	AvP.GameMode = I_GM_Playing;
	
	
	return playGameOnExitFromMenus;
#endif
}



/*-------------------------------------------*/


/* Modify this to include different levels for the demo AI */
static int DemoLevels[] = 
{
	I_Gen1,
	I_Gen2,
	I_Gen3,
	I_Cmc2,
	I_Cmc4,
	I_Cmc6,
	I_Mps2,
	I_Mps4,
	I_Entrance,
	
	-1	// Must be included
};	

static void ChooseDemoLevel(void)
{
	int random;
	int numDemoLevels = 0;

	while (DemoLevels[numDemoLevels] != -1)
	{
		numDemoLevels++;
	}

	random = FastRandom() % numDemoLevels;
			
	AvP.StartingEnv = DemoLevels[random];
}


int ChooseGamestartMenu(void) 
{
	int a,current_item=NEW_GAME_ITEM;
	int	selection = -1,debounce = 0;
	int demoTimer = 0;

	FrameCounterHandler();

	// initialise this screen
	for (a=0; a<MAX_GAMESTART_MENU_ITEMS; a++) 
		{
			CurrentMenuStatus[a]=0;		
		}
	CurrentMenuStatus[MAX_GAMESTART_MENU_ITEMS]=-1;
	CurrentMenuStatus[current_item]=1;

	// load and draw
	LoadMenuGraphics(CHOOSE_GAMESTART);
	DrawEntireMenuScreen();

	while (selection==-1) 
		{
			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			ReadUserInput();

			if (IDemandGoBackward() && debounce) 
				{
					demoTimer = 0;
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item++;
					#if PSX_DEMO
					if (current_item==LOAD_GAME_ITEM) current_item++; 	
					#endif
					if (current_item==MAX_GAMESTART_MENU_ITEMS) 	
						current_item=0;
					CurrentMenuStatus[current_item]=1;

					MenuChangeSound();
				} 
			else if (IDemandGoForward() && debounce) 
				{
					demoTimer = 0;

					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item--;
					#if PSX_DEMO
					if (current_item==LOAD_GAME_ITEM) current_item--; 	
					#endif
					if (current_item==-1) 
						current_item=(MAX_GAMESTART_MENU_ITEMS-1);
					CurrentMenuStatus[current_item]=1;

					MenuChangeSound();
		 		} 
		 	else if (IDemandSelect() && debounce) 
		 		{
					demoTimer = 0;

					debounce=0;
					selection=current_item;

					MenuSelectSound();
				} 
			else if ((IDemandGoForward()==0) && (IDemandGoBackward()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}
			DrawEntireMenuScreen();
			SoundSys_Management();

			#if SupportWindows95
				
			/* Auto start the demo from the main menu if there are no keypresses for 30 seconds */

			FrameCounterHandler();
			demoTimer += NormalFrameTime + 1;
	
			/* KJL 15:44:21 28/02/98 - deactivated the demo mode - it can be quite annoying */
			#if 0
			if (demoTimer > ONE_FIXED * 30)
			{
				selection = DEMO_ITEM;
			}
			#endif
			
			#endif

		}

	UnLoadMenuGraphics(CHOOSE_GAMESTART);
	return(selection);
}


int ChooseCharacterMenu(void) 
{
	int a,current_item = MARINE_ITEM;
	int selection = -1, debounce = 0;

	// initialise
	for (a=0; a<MAX_CHARACTER_MENU_ITEMS; a++) 
		{
			CurrentMenuStatus[a]=0;		
		}
	CurrentMenuStatus[current_item]=1;
	CurrentMenuStatus[MAX_CHARACTER_MENU_ITEMS]=-1;

	// load and draw the screeen
	LoadMenuGraphics(CHOOSE_CHARACTER);
	DrawEntireMenuScreen();

	while (selection==-1) 
		{
			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			/***** First loop, choose character. *****/
			// this wraps if we run over either end of
			// the enum
			ReadUserInput();

			if (IDemandTurnRight() && debounce) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item++;
					#if PSX_DEMO
					if (current_item==PREDATOR_ITEM) current_item++;
					#endif
					if (current_item >= MAX_CHARACTER_MENU_ITEMS) 
						{
							current_item = ALIEN_ITEM;
						}

					#if PSX_DEMO
					if (current_item==ALIEN_ITEM) current_item++;
					#endif
					CurrentMenuStatus[current_item]=1;

					MenuChangeSound();
				} 
			else if (IDemandTurnLeft() && debounce) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item--;
					#if PSX_DEMO
					if (current_item==PREDATOR_ITEM) current_item--;
					if (current_item==ALIEN_ITEM) current_item--;
					#endif
					if (current_item < 0) 
						{
							current_item = MAX_CHARACTER_MENU_ITEMS - 1;
						}
					CurrentMenuStatus[current_item]=1;

					MenuChangeSound();
				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;

					MenuSelectSound();
				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}
			DrawEntireMenuScreen();
			SoundSys_Management();

		}

	UnLoadMenuGraphics(CHOOSE_CHARACTER);
	return(selection);
}


int AlienBriefingMenu(void) 
{
	int a,current_item,selection,debounce;

	LoadMenuGraphics(ALIEN_BRIEFING);

	for (a=0; a<MAX_ALIEN_MISSION_PROFILE_ITEMS; a++) {

		CurrentMenuStatus[a]=0;		

	}

	CurrentMenuStatus[MAX_ALIEN_MISSION_PROFILE_ITEMS]=-1;

	current_item=ENTER_GAME_ALIEN;
	CurrentMenuStatus[current_item]=1;
	selection=-1;
	debounce=0;

	DrawEntireMenuScreen();

	/***** Screen set up. *****/

	while (selection==-1) 
		{

			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			ReadUserInput();

			if ( (IDemandTurnLeft() && debounce) || (IDemandTurnRight() && debounce) ) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item=((current_item==ENTER_GAME_ALIEN)? EXIT_ITEM_ALIEN:ENTER_GAME_ALIEN);
					CurrentMenuStatus[current_item]=1;
					MenuChangeSound();
				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;
					MenuSelectSound();
				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}

			DrawEntireMenuScreen();
			SoundSys_Management();

		}

	UnLoadMenuGraphics(ALIEN_BRIEFING);
	return(selection);
}
	

int OptionsMenu(void) 
{
#if PSX
	return(PsxOptionsMenu(0));
#elif SupportWindows95
	extern int PcOptionsMenu(void);
	return PcOptionsMenu();
return CANCEL;
#else /* Saturn */
	/* Don't know what's in here yet... */
	/* probably 'return SaturnOptionsMenu()' !!! */
	return(CANCEL);
#endif
}


int PredatorBriefingMenu(void) 
{
	int a,current_item,selection,debounce;

	LoadMenuGraphics(PREDATOR_BRIEFING);

	for (a=0; a<MAX_PREDATOR_MISSION_PROFILE_ITEMS; a++) {

		CurrentMenuStatus[a]=0;		

	}

	CurrentMenuStatus[MAX_PREDATOR_MISSION_PROFILE_ITEMS]=-1;

	current_item=ENTER_GAME_PREDATOR;
	CurrentMenuStatus[current_item]=1;
	selection=-1;
	debounce=0;

	DrawEntireMenuScreen();

	/***** Screen set up. *****/

	while (selection==-1) 
		{

			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			ReadUserInput();

			if ( (IDemandTurnLeft() && debounce) || (IDemandTurnRight() && debounce) ) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item=((current_item==ENTER_GAME_PREDATOR)? EXIT_ITEM_PREDATOR:ENTER_GAME_PREDATOR);
					CurrentMenuStatus[current_item]=1;

					MenuChangeSound();
					DrawEntireMenuScreen();

				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;

					MenuSelectSound();
					DrawEntireMenuScreen();

				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}

			SoundSys_Management();

		}

	UnLoadMenuGraphics(PREDATOR_BRIEFING);

	return(selection);
}


int MarineBriefingMenu(void) 
{
	/***** Hey, this one IS different! *****/

	int a,current_item;
	int selection = -1,debounce = 0;


	for (a=0; a<MAX_MARINE_MISSION_PROFILE_ITEMS; a++) 
		{
			CurrentMenuStatus[a]=0;		
		}

	CurrentMenuStatus[MAX_MARINE_MISSION_PROFILE_ITEMS]=-1;
	current_item=MALE_MARINE_FACE;
	CurrentMenuStatus[current_item]=1;

	LoadMenuGraphics(MARINE_BRIEFING);
	DrawEntireMenuScreen();

#if 0
	// RWH removed the gender selection
	// though it is still in the enum - could be used 
	// at some point
	while (selection==-1) 
		{
			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			

			ReadUserInput();

			if ( (IDemandTurnLeft() && debounce) || (IDemandTurnRight() && debounce) ) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item=((current_item==MALE_MARINE_FACE)? FEMALE_MARINE_FACE:MALE_MARINE_FACE);
					CurrentMenuStatus[current_item]=1;
					MenuChangeSound();
				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;
					MenuSelectSound();
				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}

		DrawEntireMenuScreen();
		SoundSys_Management();

	}


	switch (selection) {
		case MALE_MARINE_FACE:
		case FEMALE_MARINE_FACE:
			/***** Now act on gender choice.  Somehow. *****/
			break;
	}
#endif

	current_item=ENTER_GAME_MARINE;
	CurrentMenuStatus[current_item]=1;
	selection=-1;
	debounce=0;

	/***** Screen set up. *****/

	while (selection==-1) 
		{
			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			ReadUserInput();

			if ( (IDemandTurnLeft() && debounce) || (IDemandTurnRight() && debounce) ) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item=((current_item==ENTER_GAME_MARINE)? EXIT_ITEM_MARINE:ENTER_GAME_MARINE);
					CurrentMenuStatus[current_item]=1;
					MenuChangeSound();
				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;
					MenuSelectSound();
				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}
			DrawEntireMenuScreen();
			SoundSys_Management();

		}

	UnLoadMenuGraphics(MARINE_BRIEFING);
	return(selection);
}

#if PSX||Saturn
/* Roxby, this is PLATFORM SPECIFIC!!! */
#else
int LoadGameMenu(void) 
{

	int a,current_item;
	int selection = -1,debounce = 0;


	for (a=0; a<MAX_MARINE_MISSION_PROFILE_ITEMS; a++) 
		{
			CurrentMenuStatus[a]=0;		
		}

	CurrentMenuStatus[MAX_MARINE_MISSION_PROFILE_ITEMS]=-1;
	current_item=MALE_MARINE_FACE;
	CurrentMenuStatus[current_item]=1;

	LoadMenuGraphics(MARINE_BRIEFING);
	DrawEntireMenuScreen();

	current_item=ENTER_GAME_MARINE;
	CurrentMenuStatus[current_item]=1;
	selection=-1;
	debounce=0;

	/***** Screen set up. *****/

	while (selection==-1) 
		{
			// JCWH 18/02/98: allow ALT+TAB
			CheckForWindowsMessages();
			
			ReadUserInput();

			if ( (IDemandTurnLeft() && debounce) || (IDemandTurnRight() && debounce) ) 
				{
					debounce=0;
					CurrentMenuStatus[current_item]=0;
					current_item=((current_item==ENTER_GAME_MARINE)? EXIT_ITEM_MARINE:ENTER_GAME_MARINE);
					CurrentMenuStatus[current_item]=1;
					MenuChangeSound();
				} 
			else if (IDemandSelect() && debounce) 
				{
					debounce=0;
					selection=current_item;
					MenuSelectSound();
				} 
			else if ((IDemandTurnLeft()==0) && (IDemandTurnRight()==0) && (IDemandSelect()==0)) 
				{
					debounce=1;
				}
			DrawEntireMenuScreen();
			SoundSys_Management();

		}

	UnLoadMenuGraphics(MARINE_BRIEFING);
	return(selection);
}
/*-------------------------------------------*/

#endif