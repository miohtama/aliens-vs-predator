/*KJL***************************
* Platform specific menu stuff *
* Transplanted from ddplat.cpp *
***************************KJL*/

/* KJL 14:41:13 04/28/97 - I hate this code so much, I put it
in a separate file so that I could keep an eye on it. */
extern "C" {

#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "dxlog.h"

#include <sys\stat.h>

#include "stratdef.h"
#include "gamedef.h"
#include "gameplat.h"
#include "bh_types.h"

#include "equipmnt.h"

#include "huddefs.h"
#include "hudgfx.h"

#include "font.h"

#include "menudefs.h"
#include "menugfx.h"
#include "pcmenus.h"

#include "krender.h"
#include "chnktexi.h"
#include "language.h"

#include "awTexLd.h"
#include "alt_tab.h"

extern "C++" 
{
#include "projload.hpp" // c++ header which ignores class definitions/member functions if __cplusplus is not defined ?
#include "chnkload.hpp" // c++ header which ignores class definitions/member functions if __cplusplus is not defined ?
};
extern void SelectMenuDisplayMode(void);


extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

extern DDPIXELFORMAT DisplayPixelFormat;

extern RIFFHANDLE player_rif;

/***************** Functions ******************/
// RWH - unashamably stolen from the PSX
/****************** GAMESTART *****************/
// THESE ARE THE GRAPHICS WE NEED
MENU_GRAPHIC_ITEM Gamestart_MenuItems[] = 
{
	{
		-1,
	}
};

MENUGRAPHIC Gamestart_Backdrop = 
{
	"menugfx\\pg0.pg0",	-1,	NULL, NULL, 0, 0,  640, 480,
};


/* Hardcoded text centring since FJ_CENTRED doesn't work in BLTstring
   (some highlighted characters have different widths from their non-highlighted versions?!)
*/
MENU_TEXT_ITEM Gamestart_MenuFItems[] = 
{
	{ NEW_GAME_ITEM,TEXTSTRING_MAINMENUS_NEWGAME, NULL,  271, 160 },
	{ LOAD_GAME_ITEM, TEXTSTRING_MAINMENUS_LOADGAME, NULL,  268, 180 },
	{ OPTIONS_ITEM, TEXTSTRING_MAINMENUS_OPTIONS, NULL,  282, 200 },
	{ DEMO_ITEM, TEXTSTRING_MAINMENUS_DEMO, NULL,  293, 220 },		
 	{ MULTIPLAYER_ITEM, TEXTSTRING_MAINMENUS_MULTIPLAYER, NULL,  232, 240 },
	{ QUIT_ITEM, TEXTSTRING_MAINMENUS_EXIT, NULL,  297, 270 },
	{ -1, }
};


AVP_MENU GameStartMenu = 
{
	&Gamestart_Backdrop,
	&Gamestart_MenuItems[0],
	&Gamestart_MenuFItems[0],
	-1,
	0,
};


/**************** CHOOSE CHAR ****************/

MENU_GRAPHIC_ITEM ChooseCharacter_MenuGItems[] = 
{
	{
		ALIEN_ITEM,
		{"menugfx\\pg1A.pg0",-1,NULL, NULL,  0, 0,	 0, 0,},	
		{"NONE", -1, NULL,NULL,},
	},
	{
		MARINE_ITEM,
		{"menugfx\\pg1M.pg0",-1,NULL, NULL, 208, 0,	 0,	0,},	
		{"NONE",-1, NULL,NULL,},	
	},
	{
		PREDATOR_ITEM,
		{"menugfx\\pg1P.pg0",-1,NULL, NULL, 420, 0,  0, 0,},	
		{"NONE",-1, NULL,NULL,},	
	},
	{
		-1,
	}

};


MENU_TEXT_ITEM ChooseCharacter_MenuFItems[] = 
{
	{ EXIT_ITEM, TEXTSTRING_MAINMENUS_EXIT, NULL,  300, 440 },
	{ -1, }
};


MENUGRAPHIC ChooseCharacter_Backdrop = 
{
	"menugfx\\pg1.pg0",	-1,	NULL, NULL, 0, 0, 640, 480,
};


AVP_MENU CharacterMenu = 
{
	&ChooseCharacter_Backdrop,
	&ChooseCharacter_MenuGItems[0],
	&ChooseCharacter_MenuFItems[0],
	-1,
	0,
};



/**************** ALIEN BRIEFING ****************/

MENU_GRAPHIC_ITEM AlienBriefing_MenuGItems[] = 
{
	{	-1,	}
};


MENU_TEXT_ITEM AlienBriefing_MenuFItems[] = 
{
	{	ENTER_GAME_ALIEN, TEXTSTRING_MAINMENUS_STARTGAME, NULL,  200, 440 },
	{	EXIT_ITEM_ALIEN, TEXTSTRING_MAINMENUS_EXIT, NULL,  400, 440	},
	{	-1,	}
};

MENUGRAPHIC AlienBriefing_Backdrop = 
{"menugfx\\pg2a.pg0",-1,	NULL, NULL,	0,0,0,0,};

AVP_MENU AlienBriefing = 
{
	&AlienBriefing_Backdrop,
	AlienBriefing_MenuGItems,
	AlienBriefing_MenuFItems,
	-1,
	0,
};


/******************* PREDATOR BRIEFING ***************/

MENU_GRAPHIC_ITEM PredatorBriefing_MenuGItems[] = 
{
	{ -1, }
};

MENU_TEXT_ITEM PredatorBriefing_MenuFItems[] = 
{
	{ ENTER_GAME_PREDATOR, TEXTSTRING_MAINMENUS_STARTGAME, NULL,  200, 440 },
	{ EXIT_ITEM_PREDATOR, TEXTSTRING_MAINMENUS_EXIT, NULL,  400, 440 },
	{ -1, }
};

MENUGRAPHIC PredatorBriefing_Backdrop = 
{"menugfx\\pg2b.pg0",	-1,	NULL, NULL, 0,0,0,0};

AVP_MENU PredatorBriefing = 
{
	&PredatorBriefing_Backdrop,
	&PredatorBriefing_MenuGItems[0],
	&PredatorBriefing_MenuFItems[0],
	-1,
	0,
};


/********************** MARINE BRIEFING **************/


MENU_TEXT_ITEM MarineBriefing_MenuFItems[] = 
{
	{ ENTER_GAME_MARINE, TEXTSTRING_MAINMENUS_STARTGAME, NULL,  200, 440 },
	{ EXIT_ITEM_MARINE, TEXTSTRING_MAINMENUS_EXIT, NULL,  400, 440 },
	{ -1, }
};

MENU_GRAPHIC_ITEM MarineBriefing_MenuGItems[] = 
{

#if 0
	{
		MALE_MARINE_FACE,
		{MARPROF2,-1,NULL,NULL,   189,	   42,  52,52,},	
		{MARPROF2,-1,NULL,NULL,     189,	   42,  52,52,},	
	},
	{
		FEMALE_MARINE_FACE,
		{MARPROF2,-1,NULL,NULL,    249,	   42,  52,52,},	
		{MARPROF2,-1,NULL,NULL,     249,	   42,  52,52,},	
	},
#endif
	{
		-1,
	}
};

MENUGRAPHIC MarineBriefing_Backdrop = {"menugfx\\pg2c.pg0", -1, NULL, NULL, 0,0,0,0};

AVP_MENU MarineBriefing = 
{
	&MarineBriefing_Backdrop,
	&MarineBriefing_MenuGItems[0],
	&MarineBriefing_MenuFItems[0],
	-1,
	0,
};


#if 0
/********************** LOAD MENU **************/

MENU_TEXT_ITEM LoadGame_MenuFItems[] = 
{
	{ ENTER_GAME_MARINE, TEXTSTRING_MAINMENUS_STARTGAME, NULL,  200, 440 },
	{ EXIT_ITEM_MARINE, TEXTSTRING_MAINMENUS_EXIT, NULL,  400, 440 },
	{ -1, }
};

MENU_GRAPHIC_ITEMLoadGame_MenuItems[] = 
{
	{ -1, }
};


MENUGRAPHIC Load_Backdrop = {"menugfx\\pg2c.pg0", -1, NULL, NULL, 0,0,0,0};

AVP_MENU LoadGame = 
{
	&LoadGame_Backdrop,
	&LoadGame_MenuGItems[0],
	&LoadGame_MenuFItems[0],
	-1,
	0,
};

#endif


AVP_MENU *Current_Menu;

extern void DrawInternationalizedString(MENU_TEXT_ITEM *itemPtr, int highlighted);
extern void DrawMenuBackdrop(void);

void LoadMenuGraphics(int menu_id) 
{
	// this loads up the correct menu items - effectively
	// it is a draw menu items function
  
 	switch (menu_id) 
		{
			case CHOOSE_GAMESTART:
				Current_Menu=&GameStartMenu;
				break;
			case CHOOSE_CHARACTER:
				Current_Menu=&CharacterMenu;
				break;
			case CHOOSE_OPTIONS:
				Current_Menu=&OptionsMenuData;
				break;
			case MARINE_BRIEFING:
				Current_Menu=&MarineBriefing;
				break;	
			case PREDATOR_BRIEFING:
				Current_Menu=&PredatorBriefing;
				break;	
			case ALIEN_BRIEFING:
				Current_Menu=&AlienBriefing;
				break;	
			default:
				break;
		}
	ProcessGraphicForLoading(Current_Menu);
}
		
// okay lets load the menu graphics

void ProcessGraphicForLoading(AVP_MENU *menu_to_load)	
{
	MENU_GRAPHIC_ITEM *menuitemptr;
	menuitemptr = menu_to_load->menugitems;

	LoadMenuGraphic(menu_to_load->backdrop);

	while (menuitemptr->id != -1) 
	{
		if(strcmp(menuitemptr->On.filename, "NONE"))
			LoadMenuGraphic(&menuitemptr->On);
		if(strcmp(menuitemptr->Off.filename, "NONE"))
			LoadMenuGraphic(&menuitemptr->Off);
		menuitemptr++;
	}
}


void UnLoadMenuGraphics(int menu_id) 
{
	// this loads up the correct menu items - effectively
	// it is a draw menu items function

//	int a;

	switch (menu_id) 
		{
			case CHOOSE_GAMESTART:
				Current_Menu=&GameStartMenu;
				break;
			case CHOOSE_CHARACTER:
				Current_Menu=&CharacterMenu;
				break;
			case CHOOSE_OPTIONS:
				Current_Menu=&OptionsMenuData;
				break;
			case MARINE_BRIEFING:
				Current_Menu=&MarineBriefing;
				break;	
			case PREDATOR_BRIEFING:
				Current_Menu=&PredatorBriefing;
				break;	
			case ALIEN_BRIEFING:
				Current_Menu=&AlienBriefing;
				GLOBALASSERT(menu_id==ALIEN_BRIEFING);
				break;	
			default:
				break;
		}
	ProcessGraphicForUnloading(Current_Menu);
}

void ProcessGraphicForUnloading(AVP_MENU *menu_to_unload)
{
	MENU_GRAPHIC_ITEM *menuitemptr;
	// okay lets Unload the menu graphics
	ReleaseMenuGraphic(menu_to_unload->backdrop);
	menuitemptr=menu_to_unload->menugitems;

	while (menuitemptr->id != -1) 
		{
			if(strcmp(menuitemptr->On.filename, "NONE"))
				ReleaseMenuGraphic(&menuitemptr->On);
			if(strcmp(menuitemptr->Off.filename, "NONE"))
				ReleaseMenuGraphic(&menuitemptr->Off);
			menuitemptr++;
		}
}


/* JH 12/5/97 - no longer static function */
void LoadMenuGraphic(MENUGRAPHIC* menugraphptr)
{
	/*
		 Weapons

		 The  hud direct draw surfaces are stored in
		 system memory. It is only the current weapon that is
		 cashed into video memory

 		 First we attach the weapons direct draw surface to the
		 Direct Draw lpDD. We then lock the newly created surface and draw the
		 weapon image into it. FIle in the rest of the HUD graphic and then
		 exit
	*/

	/*
		set up the direct draw surface. we can take the width and height
		from the imageheader image
	*/
	// JH 17/2/98 - just a bit of a wrapper to AwCreateSurface now...
	
	unsigned nWidth,nHeight;
	
	GLOBALASSERT(menugraphptr);
	menugraphptr->image_ptr = AwCreateSurface("sfXYB",menugraphptr->filename,AW_TLF_TRANSP|AW_TLF_CHROMAKEY,&nWidth,&nHeight,&menugraphptr->hBackup);
	menugraphptr->width = nWidth;
	menugraphptr->height = nHeight;
	GLOBALASSERT(menugraphptr->image_ptr);
	GLOBALASSERT(menugraphptr->hBackup);
	GLOBALASSERT(menugraphptr->width>0);
	GLOBALASSERT(menugraphptr->height>0);
	ATIncludeSurface(menugraphptr->image_ptr,menugraphptr->hBackup);
}



void DeIlluminate(int menuitem) 
{

	/***** Menuitem comes from the enum.  Match it to id. *****/

	int num_menu_item = 0;

	MENU_GRAPHIC_ITEM *ThisMenuGItem = NULL;
	MENU_TEXT_ITEM *ThisMenuFItem = NULL;


	// lok throught the array of graphic oitems and font
	// items, and draw the menu item we have been passed
	
	while (Current_Menu->menugitems[num_menu_item].id!=-1) 
	{

		if(Current_Menu->menugitems[num_menu_item].id==menuitem) 
		{
			ThisMenuGItem=&Current_Menu->menugitems[num_menu_item];
		}
		num_menu_item++;
	}

	num_menu_item = 0;
	while (Current_Menu->menufitems[num_menu_item].id!=-1) 
	{
		if(Current_Menu->menufitems[num_menu_item].id==menuitem) 
		{
			ThisMenuFItem=&Current_Menu->menufitems[num_menu_item];
		}
		num_menu_item++;
	}


	if(ThisMenuGItem != NULL)
		{
			if(strcmp(ThisMenuGItem->Off.filename, "NONE"))
				BLTMenuToScreen(&ThisMenuGItem->Off);
		}

	if(ThisMenuFItem != NULL)
	{
		DrawInternationalizedString(ThisMenuFItem,0);
	}

}



void Illuminate(int menuitem) 
{

	/***** Menuitem comes from the enum.  Match it to id. *****/

	int num_menu_item = 0;

	MENU_GRAPHIC_ITEM *ThisMenuGItem = NULL;
	MENU_TEXT_ITEM *ThisMenuFItem = NULL;


	// lok throught the array of graphic oitems and font
	// items, and draw the menu item we have been passed
	
	while (Current_Menu->menugitems[num_menu_item].id!=-1) 
	{

		if(Current_Menu->menugitems[num_menu_item].id==menuitem) 
		{
			ThisMenuGItem=&Current_Menu->menugitems[num_menu_item];
		}
		num_menu_item++;
	}


	num_menu_item = 0;
	while (Current_Menu->menufitems[num_menu_item].id!=-1) 
	{
		if(Current_Menu->menufitems[num_menu_item].id==menuitem) 
		{
			ThisMenuFItem=&Current_Menu->menufitems[num_menu_item];
		}
		num_menu_item++;
	}


	if(ThisMenuGItem != NULL)
	{
		if(strcmp(ThisMenuGItem->On.filename, "NONE"))
			BLTMenuToScreen(&ThisMenuGItem->On);
	}

	if(ThisMenuFItem != NULL)
	{
		DrawInternationalizedString(ThisMenuFItem,1);
	}

	// okay, now we draw a
}

extern HINSTANCE AVP_HInstance, hInst;
extern int AVP_NCmd;


static RIFFHANDLE menu_rif = INVALID_RIFFHANDLE;

void PlatformSpecificEnteringMenus(void) 
{
	// Load and process the font
	SelectMenuDisplayMode();
		
	SetCursor(NULL);
	
	
	
	cl_pszGameMode = "menus";

	if(menu_rif == INVALID_RIFFHANDLE)
	{
		menu_rif = load_rif("menugfx\\menugfx.rif");
		LoadPFFont(MENU_FONT_1);
	}

	if(menu_rif == INVALID_RIFFHANDLE)
	{	
		GLOBALASSERT(0);
	}
				
}

void PlatformSpecificExitingMenus(void) 
{
	// just need to release the DD surface
	// lets leave the rects as valid, so we can
	// do lazy eval of the font char rects if we want
	int fadeLevel=65536;
	while(fadeLevel>=0)
	{
		/* fade screen to black */
		SetPaletteFadeLevel(fadeLevel);
  		fadeLevel-= 2048;
	}
	
	UnloadFont(&AvpFonts[MENU_FONT_1]);
	unload_rif(menu_rif);//get rid of File_Chunk   
	menu_rif = INVALID_RIFFHANDLE;	
	BlankScreen();
}


void DrawMenuBackdrop(void) 
{
	BLTMenuToScreen(Current_Menu->backdrop);
}

/* KJL 15:54:28 03/20/97 - intro hack variable */
int WhiteOutLevel;
extern int NormalFrameTime;

extern int VideoMode;
extern void (*UpdateScreen[]) (void);

void DrawEntireMenuScreen(void) 
{
	/***** Frankly, I don't care which menu. *****/

	int a;
	
	DrawMenuBackdrop();

	a=0;

	while (CurrentMenuStatus[a]!=-1) 
	{

		if (CurrentMenuStatus[a]!=0) 
			{
				Illuminate(a);
			} 
		else 
			{
				DeIlluminate(a);
			}

		a++;
	}

	UpdateScreen[VideoMode]();
	
	/* KJL 15:52:41 03/20/97 - hack from intro that's ended up here */
	{
	// fade in
	if (WhiteOutLevel==0)
	{
		int fadeLevel = 0;
		do
		{
			ResetFrameCounter();
			FadePaletteToWhite(TestPalette,fadeLevel);
			fadeLevel+=NormalFrameTime*4;
			FrameCounterHandler();
		}
		while(fadeLevel<65536);

		ChangePalette(TestPalette);
		WhiteOutLevel=65536;
	}
   	}

}



/* JH 12/5/97 - no longer static functions */

void ReleaseMenuGraphic(MENUGRAPHIC* mptr)
{
	GLOBALASSERT(mptr);
	GLOBALASSERT(mptr->image_ptr);
	ATRemoveSurface(mptr->image_ptr);
	ReleaseDDSurface(mptr->image_ptr);
	mptr->image_ptr = NULL;
	if (mptr->hBackup)
		AwDestroyBackupTexture(mptr->hBackup);
	mptr->hBackup = NULL;
}



/*
	Windows externs. See win_func
*/



void BLTMenuToScreen(MENUGRAPHIC* menuptr)
{
	/*
		 sets up the drawing of general hud graphics. Bltted
		 to full screen if there is no width and heiht information in
		 the DD HUDGRAPHIC
	*/

	RECT destRect;
	RECT scrRect;
	HRESULT ddrval;

	GLOBALASSERT(menuptr != NULL);
	
	scrRect.top = 0;
	scrRect.bottom = menuptr->height;
	scrRect.left = 0;
	scrRect.right = menuptr->width;


	destRect.top = menuptr->desty;
	destRect.bottom = menuptr->desty + menuptr->height;
	destRect.left = menuptr->destx;
	destRect.right = menuptr->destx + menuptr->width;

	ddrval = lpDDSBack->Blt(&destRect, menuptr->image_ptr, &scrRect, DDBLT_WAIT, NULL);

	if(ddrval != DD_OK)
		{
			ReleaseDirect3D();
			exit(0x666009);
		}
}


void DrawInternationalizedString(MENU_TEXT_ITEM *itemPtr, int highlighted)
{
	char textBuffer[100];
	char *destPtr = textBuffer;
	char offset;
	
	/* KJL 17:39:03 05/03/97 - I hate Roxby. Due to the way he's set up the menu font
	I have to add 27+'a'-'A' (!) to each letter if I want it *not* to be highlighted. */
	if (!highlighted)
	{
		offset = 27+'a'-'A';	 
	}
	else
	{
		offset = 0;
	}

	if (itemPtr->StringID != TEXTSTRING_BLANK)
	{
		/* copy the string into the buffer */
		char *sourcePtr = GetTextString(itemPtr->StringID);
	
		while(*sourcePtr)
		{
			/* Roxby has a lot to answer for... */
			*destPtr++ = offset + (*sourcePtr++);
		}
   		
		/* if another string follows this string, add a space */
   		if (itemPtr->SecondStringPtr)
		{
			*destPtr++=' ';
		}
   	}
	
	/* add second string, if it exists */
	if (itemPtr->SecondStringPtr)
	{
		char *sourcePtr = itemPtr->SecondStringPtr;
	
		while(*sourcePtr)
		{
			/* Roxby has a lot to answer for... */
			*destPtr++ = offset + (*sourcePtr++);
		}
	}
	
	/* add null terminator to end of string */
	*destPtr = 0;
	
	{
		FONT_DESC packet;
		packet.fontnum = MENU_FONT_1;
		packet.string = textBuffer;
		packet.destx = itemPtr->X;
		packet.desty = itemPtr->Y;
		packet.just = FJ_LEFT_JUST;
		packet.width = 320;

		BLTString(packet);
	}
}




/*KJL*******
* DATABASE *
*******KJL*/
#include "database.h"
void LoadDDGraphic2(struct DDGraphicTag *DDGfxPtr, char *Filename);
static void GetMessageName(char* messagename);

static RIFFHANDLE dbgfx_rif = INVALID_RIFFHANDLE;



struct DDGraphicTag	DatabaseBackground;
struct DatabaseGraphicsDataTag
{
	char *Filename;
	int X; /* top-left coords */
	int Y;
};



LPDIRECTDRAWSURFACE lpDDSInGameMenuBackdrop;
extern LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface


static int DatabaseInitialised = 1;

void InitialiseDatabaseGraphics()
{
	// load up the fonts

	LoadPFFont(DATABASE_FONT_DARK);
	LoadPFFont(DATABASE_FONT_LITE);
	LoadPFFont(DATABASE_MESSAGE_FONT);
	  
	
	{ 
	    DDSURFACEDESC   ddsd;
	  	HRESULT ddrval;
		
		// attach a system memory back buffer so we can record
		// what is on the screen before we enter the menus

		// Create back buffer
		memset(&ddsd,0,sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwHeight = ScreenDescriptorBlock.SDB_Height;
		ddsd.dwWidth = ScreenDescriptorBlock.SDB_Width;

		// Request a 3D capable device so that
		// Direct3D accesses will work
		ddsd.ddsCaps.dwCaps = 
		(
			DDSCAPS_OFFSCREENPLAIN | 
			DDSCAPS_SYSTEMMEMORY | 
			DDSCAPS_3DDEVICE
		);

		ddrval = lpDD->CreateSurface(&ddsd, &lpDDSInGameMenuBackdrop, NULL);
		LOGDXERR(ddrval);
		if (ddrval != DD_OK)
		#if debug
		{
			ReleaseDirect3D();
			exit(0x1055);
		}
		#else
			return;
		#endif
	}
}
		
		
	
		  
	  

void PlatformSpecificEnteringDatabase(void)
{															  


	// lets make a copy of the back buffer, and in the process darken the
	// colours by half
	
	// first, take a lock for both surfaces
	
	
	DDSURFACEDESC    ddsdback;
	DDSURFACEDESC    ddsdmenu;
	unsigned char	*src, *dest, *srcStart, *destStart;
	long srcPitch, destPitch;
	HRESULT         ddrval;
	
	
   memset(&ddsdback, 0, sizeof(ddsdback));
   ddsdback.dwSize = sizeof(ddsdback);

   memset(&ddsdmenu, 0, sizeof(ddsdmenu));
   ddsdmenu.dwSize = sizeof(ddsdmenu);

	
	// take a lock of the back buffer
	
	while ((ddrval = lpDDSBack->Lock(NULL, &ddsdback, DDLOCK_WAIT, NULL)) == DDERR_WASSTILLDRAWING)
	{
		LOGDXERR(ddrval);
 		ReleaseDirect3D();
 		exit(0x2021);
	}
	
	LOGDXERR(ddrval);
	if (ddrval != DD_OK)
	  {
	   #if debug
	   ReleaseDirect3D();
	   exit(ddrval);
	   #else
	   return;
	   #endif
	  }

	/* ddsdback now contains my lpSurface)*/

	src = srcStart = (unsigned char *)ddsdback.lpSurface;
	srcPitch = ddsdback.lPitch - ddsdback.dwWidth;
	
	
	// take a lock of the menubackdrop buffer	


	while ((ddrval = lpDDSInGameMenuBackdrop->Lock(NULL, &ddsdmenu, DDLOCK_WAIT, NULL)) == DDERR_WASSTILLDRAWING)
	{
 		LOGDXERR(ddrval);
 		ReleaseDirect3D();
 		exit(0x2022);
	}
	
	LOGDXERR(ddrval);
	if (ddrval != DD_OK)
	  {
	   #if debug
	   ReleaseDirect3D();
	   exit(ddrval);
	   #else
	   return;
	   #endif
	  }

	destStart = (unsigned char *)ddsdmenu.lpSurface;
	destPitch = ddsdmenu.lPitch - ddsdmenu.dwWidth;



	// okay, now we have the surfaces, we can copy from one to the other,
	// darkening pixels as we go
	
	
	{
		int i = 0, j = 0;
		extern int VideoModeColourDepth;

		
		switch(VideoModeColourDepth)
		{
			case 8:
			{
				unsigned char* dest = (unsigned char *)ddsdmenu.lpSurface;
				unsigned char* src = (unsigned char *)ddsdback.lpSurface;
			
				for(j=0; j < ScreenDescriptorBlock.SDB_Height; j++)
				{
					for(i=0;i < ScreenDescriptorBlock.SDB_Width; i++)
					{
						extern unsigned char* TextureLightingTable;
				
						*dest++ = *(TextureLightingTable + (32 * 256) + *src++);		  			
					}
					
					dest += destPitch;
					src	 += srcPitch;
				}
				break;
			}
#if 1
			case 16:
			{
			    short r,g,b;
				short *destShort = (short*)ddsdmenu.lpSurface;
			   	short *srcShort	= (short*)ddsdback.lpSurface;
			   
				for(j=0; j < ScreenDescriptorBlock.SDB_Height; j++)
				{
					destShort = (short*)(((char*)ddsdmenu.lpSurface)
                      			+ ddsdmenu.lPitch * j);
					srcShort = 	(short*)(((char*)ddsdback.lpSurface)
                      			+ ddsdback.lPitch * j);
			
				
					for(i=0;i < ScreenDescriptorBlock.SDB_Width; i++)
					{
					  r = (*srcShort & DisplayPixelFormat.dwRBitMask)>>1;
					  g = (*srcShort & DisplayPixelFormat.dwGBitMask)>>1;
					  b = (*srcShort & DisplayPixelFormat.dwBBitMask)>>1;
					  srcShort++;

					  r &= DisplayPixelFormat.dwRBitMask;
					  g &= DisplayPixelFormat.dwGBitMask;
					  b &= DisplayPixelFormat.dwBBitMask;

					  *destShort++ = r|g|b;
					}
				}
				break;
			}
			case 32:
			{
			   int *destInt	= (int*)ddsdmenu.lpSurface;
			   int *srcInt	= (int*)ddsdback.lpSurface;
			   int r,g,b;
			   
				for(j=0; j < ScreenDescriptorBlock.SDB_Height; j++)
				{
					for(i=0;i < ScreenDescriptorBlock.SDB_Width; i++)
					{
					  r = (*srcInt & DisplayPixelFormat.dwRBitMask)>>1;
					  g = (*srcInt & DisplayPixelFormat.dwGBitMask)>>1;
					  b = (*srcInt++ & DisplayPixelFormat.dwBBitMask)>>1;

					  r &= DisplayPixelFormat.dwRBitMask;
					  g &= DisplayPixelFormat.dwGBitMask;
					  b &= DisplayPixelFormat.dwBBitMask;

					  *destInt++ = r|g|b;
					  
					  	
					}
					
					destInt += (destPitch >> 2); 
					srcInt  += (srcPitch >> 2);
				}
				break;
				
			}
#endif			
			default:
				break;
		}
	}								


	// unlock the two surfaces

	ddrval = lpDDSInGameMenuBackdrop->Unlock((LPVOID)destStart);
	LOGDXERR(ddrval);
    #if debug
	if (ddrval != DD_OK)
	  {
	   ReleaseDirect3D();
	   exit(ddrval);
	  }
	#endif
	ddrval = lpDDSBack->Unlock((LPVOID)srcStart);
	LOGDXERR(ddrval);

    #if debug
	if (ddrval != DD_OK)
	  {
	   ReleaseDirect3D();
	   exit(ddrval);
	  }
	#endif
}


	


void DrawDatabaseBackground(void)
{
  	HRESULT ddrval;
	RECT screen_coords;
	
	screen_coords.left = 0;
	screen_coords.top =0;
	screen_coords.right = ScreenDescriptorBlock.SDB_Width;
	screen_coords.bottom = ScreenDescriptorBlock.SDB_Height;
	

   	ddrval = lpDDSBack->BltFast
   	(
   		0,
   		0,
   		lpDDSInGameMenuBackdrop,
   		&screen_coords,
   		DDBLTFAST_WAIT
   	);
	
	if (ddrval != DD_OK)
	#if debug
	{
		ReleaseDirect3D();
		exit(0x2055);
	}
	#else
		return;
	#endif



	return;

	/* draw to top-left corner of the screen, opaque */
   	ddrval = lpDDSBack->BltFast
   	(
   		0,
   		0,
   		DatabaseBackground.LPDDS,
   		&(DatabaseBackground.SrcRect),
   		DDBLTFAST_WAIT
   	);
	if(ddrval != DD_OK)
   	{
   		ReleaseDirect3D();
   		exit(0);
   	}
	return;
}


extern int IDemandGoForward();
extern int IDemandGoBackward();
extern int IDemandTurnRight();
extern int IDemandTurnLeft();
extern int IDemandSelect();
#define MAPSCREENWIDTH 251
#define MAPSCREENHEIGHT 137

int MapXOffset = (640-MAPSCREENWIDTH)/2;
int MapYOffset = (480-MAPSCREENHEIGHT)/2;



void PlatformSpecificExitingDatabase(void) 
{
//	extern void SetVisionPalette(void);
	
//	unload_rif(dbgfx_rif);//get rid of File_Chunk

//	UnloadFont(&AvpFonts[DATABASE_FONT_LITE]);
//	UnloadFont(&AvpFonts[DATABASE_FONT_DARK]);
	
//	ReleaseDDSurface(DatabaseBackground.LPDDS);
	
	/* blank screen before changing palette to avoid screen glitches */
//	BlankScreen();
	/* KJL 12:35:21 03/12/97 - get correct palette */
//	SetVisionPalette();
}
	 

void LoadDDGraphic2(struct DDGraphicTag *DDGfxPtr, char *Filename)
{
	/*
		set up the direct draw surface. we can take the width and height
		from the imageheader image
	*/
	
	GLOBALASSERT(DDGfxPtr);
    GLOBALASSERT(Filename);
    
    unsigned nWidth,nHeight;
    DDGfxPtr->LPDDS = AwCreateSurface("sfXYB",Filename,AW_TLF_TRANSP|AW_TLF_CHROMAKEY,&nWidth,&nHeight,&DDGfxPtr->hBackup);
    GLOBALASSERT(DDGfxPtr->LPDDS);
    
    ATIncludeSurface(DDGfxPtr->LPDDS,DDGfxPtr->hBackup);
	
 	DDGfxPtr->SrcRect.left = 0;
	DDGfxPtr->SrcRect.right = nWidth;
	DDGfxPtr->SrcRect.top = 0;
	DDGfxPtr->SrcRect.bottom = nHeight;
}


static void* MessageString;

#define DATABASE_MESSAGE_NAME_LENGTH 50

#if 1
void LoadDatabaseMessage()
{
	char filename[DATABASE_MESSAGE_NAME_LENGTH] = {"\000"};
	FILE* message_fp;
	struct _stat filestatus;
	int filesize;
	
	GetMessageName(filename);
	
	message_fp = fopen(filename, "rb");	
	
	GLOBALASSERT(message_fp);
	
 	_stat(filename, &filestatus);
	
	filesize = filestatus.st_size;//filestatus.off_t;
	
	MessageString = AllocateMem(filesize + 1);
	
	if (!MessageString)
	{
		memoryInitialisationFailure = 1;
		return;
	}
	
	fread(MessageString, 1, filesize, message_fp);
	
	// add the terminator
	
	*((char*)MessageString + filesize) = 0;
}


static void GetMessageName(char* messagename)
{
	char messagenum[5];
	
	sprintf(messagenum,"%d", (int)PlayerStatusPtr->CurrentMission);
	
	strcat(messagename, "DBMESS\\");
	strcat(messagename, LanguageDirNames[AvP.Language]);
	strcat(messagename, "\\MSG");
	strcat(messagename, messagenum);
	strcat(messagename, ".TXT");

}


#define MESSAGE_TOP_MARGIN    25
#define MESSAGE_SIDE_MARGIN	  35

void WriteDatabaseMessage()
{
	
	extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

	FONT_DESC db_message;

	db_message.fontnum = DATABASE_MESSAGE_FONT;
	db_message.string = (char*)MessageString;
	db_message.destx = MESSAGE_SIDE_MARGIN;
	db_message.desty = MESSAGE_TOP_MARGIN;
	db_message.width = ScreenDescriptorBlock.SDB_Width - (MESSAGE_SIDE_MARGIN*2);
	db_message.just =  FJ_LEFT_JUST;


	BLTString(db_message);
} 


void UnloadDatabaseMessage()
{
	if (MessageString) DeallocateMem(MessageString);
}
#endif

void UpdateDatabaseScreen(void)
{
	FlipBuffers();
}





}; // extern "C"