/* KJL 11:14:25 04/25/97 - hud_map.h */

/* platform independent prototypes */
extern void InitHUDMap(void);
extern void UpdateHUDMap(void);


/* fns to change the map display */
extern void HUDMapOn(void);
extern void HUDMapOff(void);
extern void HUDMapZoomIn(void);
extern void HUDMapZoomOut(void);
extern void HUDMapSmaller(void);
extern void HUDMapLarger(void);
extern void HUDMapLeft(void);
extern void HUDMapRight(void);
extern void HUDMapUp(void);
extern void HUDMapDown(void);
extern void HUDMapWorldCentreLeft(void);
extern void HUDMapWorldCentreRight(void);
extern void HUDMapWorldCentreUp(void);
extern void HUDMapWorldCentreDown(void);

extern void HUDMapResetDefaults(void);
extern void HUDMapRotateLeft(void);
extern void HUDMapRotateRight(void);
extern void HUDMapWorldForward(void);
extern void HUDMapWorldBackward(void);
extern void HUDMapWorldSlideLeft(void);
extern void HUDMapWorldSlideRight(void);

void HUDMapClamp(void);

/* platform specific stuff */
enum MAP_COLOUR_ID
{
	MAP_COLOUR_WHITE,
	MAP_COLOUR_GREY, 
	MAP_COLOUR_RED
};

extern void PlatformSpecificInitHUDMap(void);
extern void PlatformSpecificEnteringHUDMap(void);
extern void PlatformSpecificExitingHUDMap(void);
/* This is called once from StartGame */

extern void DrawHUDMapLine(VECTOR2D *vertex1, VECTOR2D *vertex2, enum MAP_COLOUR_ID colourID);




