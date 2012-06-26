/* KJL 15:25:20 8/16/97
 *
 * smacker.h - functions to handle FMV playback
 *
 */
#include "smack.h"

extern void PlayFMV(char *filenamePtr);
extern void StartMenuMusic(void);
extern void PlayMenuMusic(void);
extern void EndMenuMusic(void);



typedef struct
{
	IMAGEHEADER *ImagePtr;
	Smack *SmackHandle;
	int SoundVolume;
	int IsTriggeredPlotFMV;
	int StaticImageDrawn;

	int MessageNumber;

	LPDIRECTDRAWSURFACE SrcSurface;
	LPDIRECT3DTEXTURE SrcTexture;
	LPDIRECT3DTEXTURE DestTexture;
	PALETTEENTRY SrcPalette[256];

	int RedScale;
	int GreenScale;
	int BlueScale;

} FMVTEXTURE;


extern int NextFMVTextureFrame(FMVTEXTURE *ftPtr, void *bufferPtr);
extern void UpdateFMVTexturePalette(FMVTEXTURE *ftPtr);
extern void InitialiseTriggeredFMVs(void);
extern void StartTriggerPlotFMV(int number);

extern void StartFMVAtFrame(int number, int frame);
extern void GetFMVInformation(int *messageNumberPtr, int *frameNumberPtr);
