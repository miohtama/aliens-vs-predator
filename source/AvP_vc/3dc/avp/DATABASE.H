
#define NUM_SAVE_SLOTS 10

enum DB_STATE_ID
{
	DB_STATE_MAINMENU,
	DB_STATE_QUIT,
	DB_STATE_OPTIONS,
	DB_STATE_MISSION,
	DB_STATE_LOAD,
	DB_STATE_SAVE,
	DB_STATE_RESUME,
	DB_STATE_ACCESSDENIED,

	MAX_NO_OF_DB_STATES
};

enum DB_MENUOPTION_ID
{
	DB_MENUOPTION_RESUME,
	DB_MENUOPTION_OPTIONS,
	DB_MENUOPTION_MISSION,
	DB_MENUOPTION_LOAD,
	DB_MENUOPTION_SAVE,
	DB_MENUOPTION_QUIT,
	DB_MENUOPTION_YESQUIT,
	DB_MENUOPTION_NO,
	DB_MENUOPTION_MAINMENU,
	DB_MENUOPTION_GOBACK,
	DB_MENUOPTION_ACCESSDENIED,
	DB_MENUOPTION_SAVESLOT0,
	DB_MENUOPTION_SAVESLOT1,
	DB_MENUOPTION_SAVESLOT2,
	DB_MENUOPTION_SAVESLOT3,
	DB_MENUOPTION_SAVESLOT4,
	DB_MENUOPTION_SAVESLOT5,
	DB_MENUOPTION_SAVESLOT6,
	DB_MENUOPTION_SAVESLOT7,

	MAX_NO_OF_DB_MENUOPTIONS
};


struct DatabaseInput
{
	unsigned char RequestSelectItem :1;
	unsigned char RequestPreviousItem :1;
	unsigned char RequestNextItem :1;
};

struct MenuOptionDescriptor
{
	/* top-left coords */
	int X;
	int Y;
	FONT_JUST justification;
};




extern void AccessDatabase(int databaseID);

/* KJL 15:43:16 04/11/97 - platform specific */
extern void PlatformSpecificEnteringDatabase(void);
extern void PlatformSpecificExitingDatabase(void);

extern void DrawDatabaseBackground(void);

extern void UpdateDatabaseScreen(void);

extern int DatabaseStateChange();