#include "3dc.h"
#include "module.h"

#include "stratdef.h"
#include "gamedef.h"

#include "equipmnt.h"
#include "saveload.h"
#include "font.h"
#include "database.h"

#include "svldplat.h"

#define UseLocalAssert Yes
#include "ourasert.h"
// extern old movement functions

extern int IDemandGoForward();
extern int IDemandSelect();
extern int IDemandGoBackward();
extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

extern void LoadDatabaseMessage(void);
extern void UnloadDatabaseMessage(void);
extern void WriteDatabaseMessage(void);


unsigned char *GetTextForMenuOption(enum DB_MENUOPTION_ID optionID);
#if 0

struct MenuOptionDescriptor MenuOptionData[MAX_NO_OF_DB_MENUOPTIONS] =
{
	{145,40,FJ_CENTRED},	/* DB_MENUOPTION_RESUME*/
	{145,60,FJ_CENTRED},	/* DB_MENUOPTION_OPTIONS*/
	{145,80,FJ_CENTRED}, /*DB_MENUOPTION_MISSION*/
	{145,100,FJ_CENTRED},/* DB_MENUOPTION_LOAD, */
	{145,120,FJ_CENTRED},/* DB_MENUOPTION_SAVE, */
	{145,140,FJ_CENTRED},/* DB_MENUOPTION_QUIT, */
	{145,80,FJ_CENTRED}, /*DB_MENUOPTION_YESQUIT*/
	{145,100,FJ_CENTRED},/*DB_MENUOPTION_NO*/
	{237,150,FJ_CENTRED},/* DB_MENUOPTION_GOBACK, */
	{125,90,FJ_CENTRED},/* DB_MENUOPTION_ACCESSDENIED */
	{30,30,FJ_LEFT_JUST},/*SLOT0*/
	{30,45,FJ_LEFT_JUST},/*SLOT1*/
	{30,60,FJ_LEFT_JUST},/*SLOT2*/
	{30,75,FJ_LEFT_JUST},/*SLOT3*/
	{30,90,FJ_LEFT_JUST},/*SLOT4*/
	{30,105,FJ_LEFT_JUST},/*SLOT5*/
	{30,120,FJ_LEFT_JUST},/*SLOT6*/
	{30,135,FJ_LEFT_JUST},/*SLOT7*/
};
#else
struct MenuOptionDescriptor MenuOptionData[MAX_NO_OF_DB_MENUOPTIONS] =
{
	{0,(ONE_FIXED*4)/32,FJ_CENTRED},	/* DB_MENUOPTION_RESUME*/
	{0,(ONE_FIXED*7)/32,FJ_CENTRED},	/* DB_MENUOPTION_OPTIONS*/
	{0,(ONE_FIXED*10)/32,FJ_CENTRED}, 	/* DB_MENUOPTION_MISSION*/
	{0,(ONE_FIXED*13)/32,FJ_CENTRED},	/* DB_MENUOPTION_LOAD, */
	{0,(ONE_FIXED*16)/32,FJ_CENTRED},	/* DB_MENUOPTION_SAVE, */
	{0,(ONE_FIXED*19)/32,FJ_CENTRED},	/* DB_MENUOPTION_QUIT, */
	{0,(ONE_FIXED*10)/32,FJ_CENTRED}, 	/* DB_MENUOPTION_YESQUIT*/
	{0,(ONE_FIXED*13)/32,FJ_CENTRED},	/* DB_MENUOPTION_NO*/
	{0,(ONE_FIXED*16)/32,FJ_CENTRED},   /* DB_MENUOPTION_STARTMENU*/
	{0,(ONE_FIXED*29)/32,FJ_CENTRED},	/* DB_MENUOPTION_GOBACK, */
	{0,(ONE_FIXED*16)/32,FJ_CENTRED},	/* DB_MENUOPTION_ACCESSDENIED */
	{(ONE_FIXED*1)/32,(ONE_FIXED*4)/32,FJ_LEFT_JUST},	/*SLOT0*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*7)/32,FJ_LEFT_JUST},	/*SLOT1*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*10)/32,FJ_LEFT_JUST},	/*SLOT2*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*13)/32,FJ_LEFT_JUST},	/*SLOT3*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*16)/32,FJ_LEFT_JUST},	/*SLOT4*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*19)/32,FJ_LEFT_JUST},	/*SLOT5*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*22)/32,FJ_LEFT_JUST},	/*SLOT6*/
	{(ONE_FIXED*1)/32,(ONE_FIXED*25)/32,FJ_LEFT_JUST},	/*SLOT7*/
};
#endif


/* KJL 13:09:25 03/17/97 - to be rewritten after alpha */

unsigned char *TestMenuOptionText[] =
{
"RESUME",
"OPTIONS",
"MISSION",
"LOAD",
"SAVE",
"QUIT",
"QUIT TO WINDOWS",
"RESUME",
"QUIT TO START MENU",
"BACK",
"ACCESS DENIED",
"1:",
"2:",
"3:",
"4:",
"5:",
"6:",
"7:",
"8:",
};

							

static int NumberOfCurrentDbOptions;
static enum DB_MENUOPTION_ID CurrentDbOptions[MAX_NO_OF_DB_MENUOPTIONS];
static struct DatabaseInput DatabaseInput={0,0,0};

static enum DB_STATE_ID CurrentState;
static int DBStateHasChanged;

void SetupDatabaseState(enum DB_STATE_ID stateID);
void ClearMenuOptions(void);
void AddMenuOption(enum DB_MENUOPTION_ID optionID);
void ActUponChosenOption(enum DB_MENUOPTION_ID optionID);
enum DB_MENUOPTION_ID DisplayCurrentDatabaseState(enum DB_STATE_ID stateID);
void LoadGraphicsForDatabaseState(enum DB_STATE_ID stateID);
void UnloadGraphicsForDatabaseState(enum DB_STATE_ID stateID);
void DrawSpecialGraphicsForState(enum DB_STATE_ID stateID);

/* fns that should be platform specific but aren't yet */
void GetDatabaseInput(void);
void DrawDatabaseMenuOption(enum DB_MENUOPTION_ID optionID, int highlighted);
void DrawDatabaseTextString(unsigned char *textPtr, int x, int y, FONT_JUST justification, int highlighted);
unsigned char *GetTextForMenuOption(enum DB_MENUOPTION_ID optionID);

void AccessDatabase(int databaseID)
{
	CurrentState = DB_STATE_MAINMENU;
	
	PlatformSpecificEnteringDatabase();

	/* database main loop */
	while(CurrentState != DB_STATE_RESUME)
	{
		enum DB_MENUOPTION_ID chosenOption;

		/* load gfx and setup menu options for current state */
		SetupDatabaseState(CurrentState);

		/* display state and let user select option */
		chosenOption = DisplayCurrentDatabaseState(CurrentState);
	
		/* kill gfx for current state */
		UnloadGraphicsForDatabaseState(CurrentState);
		
		/* act upon chosen option, which usually changes state */
		ActUponChosenOption(chosenOption);
	}

	PlatformSpecificExitingDatabase();
}

void SetupDatabaseState(enum DB_STATE_ID stateID)
{
	LoadGraphicsForDatabaseState(stateID);
	
	ClearMenuOptions();

	switch (stateID)
	{
		case DB_STATE_MAINMENU:
		{
			AddMenuOption(DB_MENUOPTION_RESUME);
			AddMenuOption(DB_MENUOPTION_OPTIONS);
			AddMenuOption(DB_MENUOPTION_MISSION);
			AddMenuOption(DB_MENUOPTION_LOAD);
			AddMenuOption(DB_MENUOPTION_SAVE);
			AddMenuOption(DB_MENUOPTION_QUIT);
			break;
		}
		case DB_STATE_OPTIONS:
		{
			AddMenuOption(DB_MENUOPTION_GOBACK);
			break;
		}
		case DB_STATE_QUIT:
		{
			AddMenuOption(DB_MENUOPTION_MAINMENU);
			AddMenuOption(DB_MENUOPTION_YESQUIT);
			AddMenuOption(DB_MENUOPTION_NO);
			break;
		}
		case DB_STATE_MISSION:
		{
			AddMenuOption(DB_MENUOPTION_GOBACK);
			break;
		}
		case DB_STATE_LOAD:
		{
			AddMenuOption(DB_MENUOPTION_SAVESLOT0);
			AddMenuOption(DB_MENUOPTION_SAVESLOT1);
			AddMenuOption(DB_MENUOPTION_SAVESLOT2);
			AddMenuOption(DB_MENUOPTION_SAVESLOT3);
			AddMenuOption(DB_MENUOPTION_SAVESLOT4);
			AddMenuOption(DB_MENUOPTION_SAVESLOT5);
			AddMenuOption(DB_MENUOPTION_SAVESLOT6);
			AddMenuOption(DB_MENUOPTION_SAVESLOT7);
			AddMenuOption(DB_MENUOPTION_GOBACK);
		
			break;
		}
		case DB_STATE_SAVE:
		{
			AddMenuOption(DB_MENUOPTION_SAVESLOT0);
			AddMenuOption(DB_MENUOPTION_SAVESLOT1);
			AddMenuOption(DB_MENUOPTION_SAVESLOT2);
			AddMenuOption(DB_MENUOPTION_SAVESLOT3);
			AddMenuOption(DB_MENUOPTION_SAVESLOT4);
			AddMenuOption(DB_MENUOPTION_SAVESLOT5);
			AddMenuOption(DB_MENUOPTION_SAVESLOT6);
			AddMenuOption(DB_MENUOPTION_SAVESLOT7);
			AddMenuOption(DB_MENUOPTION_GOBACK);

			break;
		}
		case DB_STATE_ACCESSDENIED:
		{
			AddMenuOption(DB_MENUOPTION_ACCESSDENIED);
			break;
		}


		default:
		{
			/* not a valid state, assert */
			LOCALASSERT(0);
			break;
		}
	}
}


void ClearMenuOptions(void)
{
	NumberOfCurrentDbOptions=0;
}


void AddMenuOption(enum DB_MENUOPTION_ID optionID)
{
	CurrentDbOptions[NumberOfCurrentDbOptions] = optionID;
	NumberOfCurrentDbOptions++;
}


void ActUponChosenOption(enum DB_MENUOPTION_ID optionID)
{
	switch (optionID)
	{
		case DB_MENUOPTION_RESUME:
		{
			CurrentState = DB_STATE_RESUME;
			DBStateHasChanged = 1;
			break;
		}
		case DB_MENUOPTION_OPTIONS:
		{
			CurrentState = DB_STATE_ACCESSDENIED;
			DBStateHasChanged = 1;
			break;
		}
		case DB_MENUOPTION_MISSION:
		{
			CurrentState = DB_STATE_MISSION;
			DBStateHasChanged = 1;
			break;
		}
		case DB_MENUOPTION_LOAD:
		{
			CurrentState = DB_STATE_LOAD;
//			LoadGameFromFile();
			break;
		}
		case DB_MENUOPTION_SAVE:	
		{
		 	CurrentState = DB_STATE_SAVE;
//			SaveGameToFile();
			break;
		}
		case DB_MENUOPTION_NO:
		case DB_MENUOPTION_GOBACK:
		case DB_MENUOPTION_ACCESSDENIED:
		{
			/* return to main menu */
			CurrentState = DB_STATE_MAINMENU;
			DBStateHasChanged = 1;
			break;
		}
		case DB_MENUOPTION_SAVESLOT0:
		case DB_MENUOPTION_SAVESLOT1:
		case DB_MENUOPTION_SAVESLOT2:
		case DB_MENUOPTION_SAVESLOT3:
		case DB_MENUOPTION_SAVESLOT4:
		case DB_MENUOPTION_SAVESLOT5:
		case DB_MENUOPTION_SAVESLOT6:
		case DB_MENUOPTION_SAVESLOT7:
		{
			if(DB_STATE_LOAD ==	CurrentState)
			{
				LoadSaveSlot(optionID - DB_MENUOPTION_SAVESLOT0);
			}
	
			if(DB_STATE_SAVE ==	CurrentState)
			{
				SaveSaveSlot(optionID - DB_MENUOPTION_SAVESLOT0);
			}
			
			CurrentState = DB_STATE_RESUME;
			break;
		}
		case DB_MENUOPTION_QUIT:
		{
			CurrentState = DB_STATE_QUIT;
			DBStateHasChanged = 1;
			break;
		}
		case DB_MENUOPTION_YESQUIT:
		{
			ExitSystem();
			exit(0);
		}
		case DB_MENUOPTION_MAINMENU:
		{
			AvP.MainLoopRunning = 0;
			CurrentState = DB_STATE_RESUME;
			break;
		}
		default:
		{
			/* invalid option */
			LOCALASSERT(0);
			break;
		}																	    
	}
}


static int selectedOption;

enum DB_MENUOPTION_ID DisplayCurrentDatabaseState(enum DB_STATE_ID stateID)
{
	int selectionNotMade = 1;
	
	selectedOption = 0;

	LOCALASSERT(NumberOfCurrentDbOptions != 0);

	do
	{
		// JCWH 18/02/98: allow ALT+TAB
		CheckForWindowsMessages();
			
		/* draw background */
		DrawDatabaseBackground();

		/* draw weapons, maps etc. as required */
		DrawSpecialGraphicsForState(stateID);

		/* draw all menu options */
		{
			int o;
			for (o=0; o<NumberOfCurrentDbOptions; o++)
			{
				if (o == selectedOption)
					DrawDatabaseMenuOption(CurrentDbOptions[o],1);
				else
					DrawDatabaseMenuOption(CurrentDbOptions[o],0);
			}
		}
		
		/* update screen */
		UpdateDatabaseScreen();

		/* allow user to select an option */
		GetDatabaseInput();
		
		if (DatabaseInput.RequestSelectItem)
		{
			selectionNotMade=0;
		}	
		else if (DatabaseInput.RequestPreviousItem)
		{
			if (selectedOption == 0)
			{
				selectedOption = NumberOfCurrentDbOptions;
			}
			selectedOption--;
		}
		else if (DatabaseInput.RequestNextItem)
		{
			selectedOption++;
			if (selectedOption == NumberOfCurrentDbOptions)
			{
				selectedOption = 0;
			}
		}
	}
	while(selectionNotMade);
	
	return(CurrentDbOptions[selectedOption]);
}


void LoadGraphicsForDatabaseState(enum DB_STATE_ID stateID)
{
	switch (stateID)
	{
		case DB_STATE_MISSION:
		{
			
			LoadDatabaseMessage();
			break;		 
		}
	
		default:
			break; 
	}
}


void UnloadGraphicsForDatabaseState(enum DB_STATE_ID stateID)
{
	switch (stateID)
	{
		case DB_STATE_MISSION:
		{
			UnloadDatabaseMessage();
			break;		 
		}
		default:
			break;
	}
}


void DrawSpecialGraphicsForState(enum DB_STATE_ID stateID)
{
	if(stateID == DB_STATE_MISSION)
	{
		WriteDatabaseMessage();
	}

	if((stateID == DB_STATE_LOAD) || (stateID == DB_STATE_SAVE))
	{
		char game_name[GAME_NAME_LENGTH];
		int i = GAME_NAME_LENGTH, j;		

		while(i-- > 0)
		{
			*(game_name + i) = 0;
		}

		i = NUM_SAVE_SLOTS;
		
		
		while(i-- >0)
		{	   
			FONT_DESC packet;
			int posx, posy;
			AVP_FONTS font_num = DATABASE_FONT_DARK;
			
			
			j = GAME_NAME_LENGTH;

			while(j-- > 0)
			{
				*(game_name + j) = 0;
			}


		  	ReadSaveSlotGameName(i, game_name);
			
			posx = ((MenuOptionData[i + DB_MENUOPTION_SAVESLOT0].X*ScreenDescriptorBlock.SDB_Width) >> 16) + 20;
			posy = ((MenuOptionData[i + DB_MENUOPTION_SAVESLOT0].Y*ScreenDescriptorBlock.SDB_Height) >> 16);
			
			if(selectedOption == i)
			{
				font_num = 	DATABASE_FONT_LITE;
			}
			
			
			packet.fontnum = font_num;
			packet.string = game_name;
			packet.destx = posx;
			packet.desty = posy;
			packet.just = FJ_LEFT_JUST;
			packet.width = ScreenDescriptorBlock.SDB_Width;

			BLTString(packet);
   			
	   	}
	}
	
}

/* platform specific fns */



static int debounced = 0;
void GetDatabaseInput(void)
{
	ReadUserInput();
	
	DatabaseInput.RequestSelectItem = 0;
	DatabaseInput.RequestPreviousItem = 0;
	DatabaseInput.RequestNextItem = 0;
	
	if (IDemandSelect())
	{
		if (debounced)
		{
			DatabaseInput.RequestSelectItem =1;
			debounced = 0;
		}
	}
	else if (IDemandGoBackward())
	{
		if (debounced)
		{
			DatabaseInput.RequestNextItem =1;
			debounced = 0;
		}
	}
	else if (IDemandGoForward())
	{
		if (debounced)
		{
			DatabaseInput.RequestPreviousItem =1;
			debounced = 0;
		}
	}
	else  
	{
		debounced=1;
	}

}

void DrawDatabaseMenuOption(enum DB_MENUOPTION_ID optionID, int highlighted)
{
	unsigned char *textPtr = GetTextForMenuOption(optionID);

	DrawDatabaseTextString
	(
		textPtr,
		MenuOptionData[optionID].X,
		MenuOptionData[optionID].Y,
		MenuOptionData[optionID].justification,		
		highlighted
	);
}


void DrawDatabaseTextString
(
	unsigned char *textPtr, 
	int x, 
	int y, 
	FONT_JUST justification,
	int highlighted)
{
	FONT_DESC packet;
	if (highlighted) packet.fontnum = DATABASE_FONT_LITE;
	else  packet.fontnum = DATABASE_FONT_DARK;

	packet.string = textPtr;
	packet.destx = (x*ScreenDescriptorBlock.SDB_Width) >>16;
	packet.desty = (y*ScreenDescriptorBlock.SDB_Height) >>16;
	packet.just = justification;
	packet.width = ScreenDescriptorBlock.SDB_Width;

	BLTString(packet);
}










unsigned char *GetTextForMenuOption(enum DB_MENUOPTION_ID optionID)
{
	return TestMenuOptionText[optionID];		
}


int DatabaseStateChange()
{
	if(DBStateHasChanged)
	{
		DBStateHasChanged = 0;

		return 1;
	}
	
	return 0;
}