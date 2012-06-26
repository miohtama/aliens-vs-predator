#ifndef _included_pcmenus_h_
#define _included_pcmenus_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "menugfx.h"

#define ENABLE_SHADING_OPTION 0
#define ENABLE_MIPMAP_OPTION 0

typedef enum OptionsMenuItem
{
	OMI_DIRECT3D,
	OMI_DISPLAY,
	OMI_ZBUFFER,
	#if ENABLE_MIPMAP_OPTION
	OMI_MIPMAP,
	#endif
	#if ENABLE_SHADING_OPTION
	OMI_SHADING,
	#endif
	OMI_TEXTUREFORMAT,
	OMI_BILINFILTER,
  OMI_KEYCONFIG,
	OMI_RETURN,
	
	OMI_NUMMENUITEMS

} OPTIONSMENUITEM;

typedef enum KeyConfigItems
{
  KEYCONFIG_FORWARD,
  KEYCONFIG_BACKWARD,
  KEYCONFIG_TURN_LEFT,
  KEYCONFIG_TURN_RIGHT,
  KEYCONFIG_STRAFE,
  KEYCONFIG_STRAFE_LEFT,
  KEYCONFIG_STRAFE_RIGHT,
  KEYCONFIG_LOOK_UP,
  KEYCONFIG_LOOK_DOWN,
  KEYCONFIG_CENTRE_VIEW,
  KEYCONFIG_WALK,
  KEYCONFIG_CROUCH,
  KEYCONFIG_JUMP,
  KEYCONFIG_OPERATE,
  KEYCONFIG_VISION,
  KEYCONFIG_NEXT_WEAPON,
  KEYCONFIG_PREVIOUS_WEAPON,
  KEYCONFIG_FIRE_PRIMARY,
  KEYCONFIG_FIRE_SECONDARY,
  
  KEYCONFIG_NUMITEMS

} KEYCONFIGITEMS;

typedef enum MouseConfigItems
{
  MOUSECONFIG_XSENSITIVITY,
  MOUSECONFIG_YSENSITIVITY,
  MOUSECONFIG_VAXIS,
  MOUSECONFIG_HAXIS,
  MOUSECONFIG_FLIPVERTICAL,
  MOUSECONFIG_AUTOCENTRE,
  MOUSECONFIG_EXIT,
  
  MOUSECONFIG_NUMITEMS

} MOUSECONFIGITEMS;
  

typedef enum Shading {

	SHADE_FLAT,
	SHADE_GOURAUD

} SHADING;

typedef enum ImageTypeIdx
{
	ITI_HUD = 0,
	ITI_TEXTURE,
	ITI_SPRITE,
	ITI_MAX

} IMAGETYPEIDX;

extern SHADING desired_shading;

int PcOptionsMenu(void);
void MouseOptionsMenu(void);
void DrawMouseOptionsScreen(int selection);
void PCKeyConfigMenu(void);
void DrawKeyConfigScreen(int currentRow,int currentColumn);
void RedefineKey(int currentRow,int currentColumn);

extern int SetGameVideoMode(void);
void RestoreGameVideoMode(void);
void InitOptionsMenu(void);

extern void SaveVideoModeSettings(void);

BOOL PreferTextureFormat(struct D3DTextureFormat const * oldfmt,struct D3DTextureFormat const * newfmt);

extern const char * GenTex4bit_Directory;
extern const char * GenTex8bit_Directory;
extern const char * GenTex75pc_Directory;
extern const char * GenTex50pc_Directory;

void SelectGenTexDirectory(IMAGETYPEIDX);
float GetUVScale(IMAGETYPEIDX);

/* This will change the video mode to 640x480x8 (or rather, the menu video mode) if it is not already in that mode */
#define AMB_MODELESS 0x00000001 /* do not wait for select or blank screen */
void AvpMessageBox(char const * text, char const * title, int flags);

extern AVP_MENU OptionsMenuData;
extern AVP_MENU VideoModeOptionsMenu;

#ifdef __cplusplus
}
#endif




/*
	NEW CONTENT 1/4/98 by DHM:
	--------------------------

	Building the video options page for the new menu system, I need
	access to various variables declared as static within PCMENUS.CPP

	I've decided to access them through an export variable interface,
	hoping this will make the code cleaner.
*/
#ifdef __cplusplus
#	ifndef _expvar_hpp
#		include "expvar.hpp"
#	endif
#	ifndef _scstring
#		include "scstring.hpp"
#	endif

	namespace RebMenus
	{
		class ExportVariable_3dAcceleration : public ExportVariable<OurBool>
		{
		public:
			OurBool Get(void) const;
			void Set(OurBool NewVal);
		};

		class ExportVariable_ZBuffering : public ExportVariable<OurBool>
		{
		public:
			OurBool Get(void) const;
			void Set(OurBool NewVal);
		};

		class ExportVariable_BilinearFiltering : public ExportVariable<OurBool>
		{
		public:
			OurBool Get(void) const;
			void Set(OurBool NewVal);
		};

		class VideoModeSelection
		{
		public:
			static SCString* DescribeCurrentSelection(void);
			static void Dec(void);
			static void Inc(void);
		};
	};

#endif /* __cplusplus */




#endif /* ! _included_pcmenus_h_ */
