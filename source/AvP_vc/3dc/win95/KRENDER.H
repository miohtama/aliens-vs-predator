/* If this define is set to a non-zero value then the new scandraws will be used */
#define KRENDER_ON 1

/* prototypes of the replacement scandraw functions */
extern void KR_ScanDraw_Item_2dTexturePolygon_VideoModeType_8(int *itemptr);
extern void KR_ScanDraw_Item_Gouraud2dTexturePolygon_VideoModeType_8(int *itemptr);
extern void KR_ScanDraw_Item_Gouraud3dTexturePolygon_Linear_S_VideoModeType_8(int *itemptr);
extern void MotionTrackerRotateBlit(void);
extern void MotionTrackerRotateBlit8(void);


/*KJL*****************************************************
* Palette fading; a value of 65536 corresponds to normal *
* palette, 0 is completely other (eg. all white).        *
*****************************************************KJL*/
extern void SetPaletteFadeLevel(int fadeLevel);
extern void FadeBetweenPalettes(unsigned char *palPtr, int fadeLevel);
extern void FadePaletteToWhite(unsigned char *palPtr,int fadeLevel);

/*KJL*********************************************
* Fill the screen with black & flip then repeat. *
*********************************************KJL*/
extern void BlankScreen(void);


/* KJL 16:06:24 04/04/97 - To choose between laced and full screen modes */
extern int KRenderDrawMode;






extern void KDraw_Item_GouraudPolygon(int *itemptr);
extern void KDraw_Item_2dTexturePolygon(int *itemptr);
extern void KDraw_Item_Gouraud2dTexturePolygon(int *itemptr);
extern void KDraw_Item_Gouraud3dTexturePolygon(int *itemptr);












extern void MakeViewingWindowSmaller(void);
extern void MakeViewingWindowLarger(void);

/* routines to draw a star filled sky */
extern void CreateStarArray(void);
extern void DrawStarfilledSky(void);