#ifndef _included_menudefs_h_
#define _included_menudefs_h_

#ifdef __cplusplus
extern "C" {
#endif

/***** Menudefs.h - CDF 22/10/96 *****/

/* Welcome to the menus system.  Here are
  some globals that seemed useful. */

extern int GlobalLanguage;
extern int PlayerCharacter;
extern int LoadGame;

extern int CurrentMenuStatus[];

/* PlayerCharacter is what character you are,
 according to the AVP_PLAYER_TYPES enum.
   GlobalLanguage is what language the game
 should be in, as per the LANGUAGE_MENU_ITEMS
 enum below. :-).
   CurrentMenuStatus I'll come to in a minute.

   There are also some enums at the bottom of
 this file, but I'll come to them in a minute.

*/

/*-------------------------------------------*/

/* Your mission, should you decide to accept
 it, is to implement the following functions:*/

extern void ChooseLanguageMenu(void);

extern int StartUpMenus(void);

/* Well, not those two.  They call the menu 
 system. Place them in main.c as you see fit.*/

extern void PlatformSpecificEnteringMenus();
extern void PlatformSpecificExitingMenus();

#if PSX
extern int PsxOptionsMenu(int state);
#endif

/* Those functions I need, so they're here.
 Non-playstation coders, feel free to leave
 them empty. They should at least be callable
 multiple times without messing up. */

extern void LoadMenuGraphics(int menu_id);
extern void UnLoadMenuGraphics(int menu_id);

/* Should load all the graphics for a menu:
 menu_id refers to the START_MENU_STATES
 enum at the foot of this file. I also
 use it to set a (platform specific) 
 global pointer, Current_Menu, used by...*/

extern void DrawEntireMenuScreen(void);

/* This should sweep the CurrentMenuStatus[]
 array.  Each entry corresponds to an item
 on that menu screen according to the appropriate
 enum, and can (at time of writing) be
 either 0 (unlit), 1 (lit) or -1 (terminator):
 e.g. CurrentMenuStatus[MARINE_ITEM]=whatever. */

extern void PlayAVI(char *filename);

extern int LoadGameMenu(void);

/* Not implemented... yet. */

/*-------------------------------------------*/

/* The following are enums for first all the
 menus, and then for the items on each menu in
 turn.  The order of the enums control the order
 in which the items are stepped through by button
 presses.  In a couple of cases one menu screen
 has two functions, e.g. select character.  In 
 this case some of the connectivity is coded in
 (yuk).  Also the SELECT_CHARACTER_LIGHT item is
 not really a menu item... it's a light, but it
 can still be On or Off. */

/***** Master Menus Enum *****/

typedef enum {
	CHOOSE_GAMESTART,
	CHOOSE_CHARACTER,
	CHOOSE_OPTIONS,
	MARINE_BRIEFING,
	PREDATOR_BRIEFING,
	ALIEN_BRIEFING,
	LOAD_GAME,
#if PSX
	JOYPAD_CONFIG,
#endif
	END
} START_MENU_STATES;


/***** Game Start Menu *****/

typedef enum {

	NEW_GAME_ITEM =0,
	LOAD_GAME_ITEM,
	OPTIONS_ITEM,
#if PSX || Saturn
//	DEMO_ITEM,
#else
	DEMO_ITEM,
	MULTIPLAYER_ITEM,
	QUIT_ITEM,
#endif
	MAX_GAMESTART_MENU_ITEMS

} GAMESTART_MENU_ITEMS;

/***** Choose Character Menu *****/

typedef enum {

	ALIEN_ITEM = 0,
	MARINE_ITEM,
	PREDATOR_ITEM,
	EXIT_ITEM,
	MAX_CHARACTER_MENU_ITEMS

} CHARACTER_MENU_ITEMS;

/***** Briefing Menus *****/

typedef enum {
	ENTER_GAME_ALIEN=0,
	EXIT_ITEM_ALIEN,
	MAX_ALIEN_MISSION_PROFILE_ITEMS
} ALIEN_MISSION_PROFILE_ITEMS;

typedef enum {
	ENTER_GAME_PREDATOR=0,
	EXIT_ITEM_PREDATOR,
	MAX_PREDATOR_MISSION_PROFILE_ITEMS
} PREDATOR_MISSION_PROFILE_ITEMS;

typedef enum {
	ENTER_GAME_MARINE=0,
	EXIT_ITEM_MARINE,
	MALE_MARINE_FACE,
	FEMALE_MARINE_FACE,
	MAX_MARINE_MISSION_PROFILE_ITEMS
} MARINE_MISSION_PROFILE_ITEMS;

/***** Choose Language Menu *****/

typedef enum {
	ENGLISH_ITEM=0,
	FRANCAIS_ITEM,
	ESPANOL_ITEM,
	DEUTSH_ITEM,
	ITALIANO_ITEM,
	SVENSKA_ITEM,
	MAX_LANGUAGE_MENU_ITEMS
} LANGUAGE_MENU_ITEMS;


typedef enum {
	SLOT0_ITEM=0,
	SLOT1_ITEM,
	SLOT2_ITEM,
	SLOT3_ITEM,
	SLOT4_ITEM,
	SLOT5_ITEM,
	SLOT6_ITEM,
	SLOT7_ITEM,
	MAX_LOAD_MENU_ITEMS
} LOAD_MENU_ITEMS;


#if PSX
typedef enum {

	MUSIC_ITEM = 0,
	SOUND_ITEM,
	DIFFICULTY_ITEM,
	CONTROLS_ITEM,
	EXIT_ITEM_OPTIONS,
	EASY_ITEM,
	MEDIUM_ITEM,
	HARD_ITEM,
	IMPOSSIBLE_ITEM,
	MAX_OPTIONS_MENU_ITEMS

} OPTIONS_MENU_ITEMS;

typedef enum {

	OPTIONS_PAUSE_LIGHT = 0,
	PAUSED_MUSIC_ITEM,
	PAUSED_SOUND_ITEM,
	PAUSED_EXIT_ITEM_OPTIONS,
	MAX_PAUSED_OPTIONS_MENU_ITEMS

} PAUSED_OPTIONS_MENU_ITEMS;

typedef enum {

	PAUSE_LIGHT = 0,
	PAUSED_RESUME_ITEM,
	PAUSED_OPTIONS_ITEM,
	PAUSED_EXIT_ITEM,
	MAX_PAUSED_MENU_OPTIONS

} PAUSED_MENU_OPTIONS;

typedef enum {

	ALIEN_PAUSE_LIGHT = 0,
	ALIEN_PAUSED_RESUME_ITEM,
	ALIEN_PAUSED_OBJECTIVE_ITEM,
	ALIEN_PAUSED_OPTIONS_ITEM,
	ALIEN_PAUSED_LOAD_ITEM,
	ALIEN_PAUSED_SAVE_ITEM_ENABLED,
	ALIEN_PAUSED_SAVE_ITEM_DISABLED,
	ALIEN_PAUSED_EXIT_ITEM,
	MAX_ALIEN_PAUSED_MENU_OPTIONS

} ALIEN_PAUSED_MENU_OPTIONS;

#endif

/* Well, that's it.  Yes, I know that
 much of menus.c was done with cut'n'paste,
 but there were differences, honest.  I
 considered having just one universal
 menu function but thought is wasn't
 worth the hassle.
   Besides, I suspect that the Load Game
 menu may have to be entirely platform
 specific anyway...

   If any of your code doesn't work, the
 Secretary will deny all knowledge of your
 actions. This file will self destruct in
 five seconds.  */

#ifdef __cplusplus
}
#endif

#endif /* ! _included_menudefs_h_ */
