#ifndef _included_menugfx_h_
#define _included_menugfx_h_

#include "aw.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "langenum.h"
// structures used for the menu code

typedef struct menugraphic {
	char *filename;
	int image_number;
	LPDIRECTDRAWSURFACE image_ptr;
	AW_BACKUPTEXTUREHANDLE hBackup;
	short destx;
	short desty;
	short width;
	short height;

} MENUGRAPHIC;

/* JH 12/5/97 these were static in menuplat.cpp - but they're quite useful functions */
extern void LoadMenuGraphic(MENUGRAPHIC* menugraphptr);
extern void BLTMenuToScreen(MENUGRAPHIC* menuptr);
extern void ReleaseMenuGraphic(MENUGRAPHIC* mptr);

typedef struct menu_graphic_item{
	int id;
	MENUGRAPHIC On;
	MENUGRAPHIC Off;
} MENU_GRAPHIC_ITEM;



/* KJL 15:23:39 05/03/97 - new menu_text_item, which
looks as much like the menu_font_item as possible
while enabling me to add language internationalization */
typedef struct menu_text_item
{
	int id;
	enum TEXTSTRING_ID StringID;
	char *SecondStringPtr;
	int X;
	int Y;
} MENU_TEXT_ITEM;



// OVERALL menus - this describes one menu screen

typedef struct avp_menu 
{
 	MENUGRAPHIC *backdrop;
	MENU_GRAPHIC_ITEM *menugitems;
	MENU_TEXT_ITEM *menufitems;
	int previous_selection;
	int default_selection;

} AVP_MENU;


#define BASEX 112
#define BASEY 43


extern AVP_MENU *Current_Menu;
extern void ProcessGraphicForLoading(AVP_MENU *menu_to_load);
extern void ProcessGraphicForUnloading(AVP_MENU *menu_to_load);

extern void PlatformSpecificEnteringDatabase(void);
extern void PlatformSpecificExitingDatabase(void);

#ifdef __cplusplus
}
#endif

#endif /* ! _included_menugfx_h_ */
