#if 0
/* KJL 13:52:20 28/03/98 - game intro */
#include "3dc.h"
#include "intro.hpp"
#include "menugfx.h"

extern "C"
{
	#include "inline.h"
	#include "smacker.h"
	extern int NormalFrameTime;
	extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
	extern DDPIXELFORMAT DisplayPixelFormat;
	extern LPDIRECTDRAWSURFACE lpDDSBack;
	extern int GotAnyKey;

};
extern void DirectReadKeyboard(void);




static MENUGRAPHIC Presents = 
{
 	"Graphics\\NewMenus\\presents.rim",	-1,	NULL, NULL, 0, 0,  640, 480,
};

static MENUGRAPHIC RebellionLogo = 
{
 	"Graphics\\NewMenus\\ARebellionGame.rim",	-1,	NULL, NULL, 0, 0,  640, 480,
};
static MENUGRAPHIC AvPLogo = 
{
 	"Graphics\\NewMenus\\AliensVPredator.rim",	-1,	NULL, NULL, 0, 0,  640, 480,
};

extern MENUGRAPHIC Starfield_Backdrop;
static IntroHasAlreadyBeenPlayed = 1;


void Show_Presents(void);
void Show_ARebellionGame(void);
void Show_AvPLogo(void);
void DrawGraphicWithAlphaChannel(MENUGRAPHIC *image, int alpha);
void DrawGraphicWithFadingLevel(MENUGRAPHIC *image, int alpha);

extern "C"
{
	void WeWantAnIntro(void)
	{
		IntroHasAlreadyBeenPlayed = 0;
	}
}

void PlayIntroSequence(void)
{
	if (IntroHasAlreadyBeenPlayed)
	{
		/* play music */
		StartMenuMusic();
		return;
	}
	IntroHasAlreadyBeenPlayed=1;

	/* load the graphics we'll need later, so as to avoid glitches */
	LoadMenuGraphic(&Presents);
	LoadMenuGraphic(&RebellionLogo);
	LoadMenuGraphic(&AvPLogo);

	/* play the Fox Interactive FMV */
	PlayFMV("FMVs/fox640.smk");

	/* play music */
	StartMenuMusic();

	ResetFrameCounter();
	
	Show_Presents();
	if (!GotAnyKey) Show_ARebellionGame();
	if (!GotAnyKey) Show_AvPLogo();

	/* unload the graphics */
	ReleaseMenuGraphic(&Presents);
	ReleaseMenuGraphic(&RebellionLogo);
	ReleaseMenuGraphic(&AvPLogo);

}
void Show_Presents(void)
{
	int timeRemaining = 8*ONE_FIXED;
	do
	{
		{
			PlayMenuMusic();

			if (timeRemaining > 7*ONE_FIXED)
			{
				DrawGraphicWithFadingLevel(&Starfield_Backdrop,timeRemaining-7*ONE_FIXED);
			}
			else if (timeRemaining > 6*ONE_FIXED)
			{
				BLTMenuToScreen(&Starfield_Backdrop);
			}
			else if (timeRemaining > 5*ONE_FIXED)
			{
				BLTMenuToScreen(&Starfield_Backdrop);
				DrawGraphicWithAlphaChannel(&Presents,timeRemaining-5*ONE_FIXED);
			}
			else if (timeRemaining > 4*ONE_FIXED)
			{
				BLTMenuToScreen(&Starfield_Backdrop);
				DrawGraphicWithAlphaChannel(&Presents,0);
			}
			else if (timeRemaining > 3*ONE_FIXED)
			{
				BLTMenuToScreen(&Starfield_Backdrop);
				DrawGraphicWithAlphaChannel(&Presents,4*ONE_FIXED-timeRemaining);
			}
			else
			{
				BLTMenuToScreen(&Starfield_Backdrop);
			}
			
			FlipBuffers();
		}
		DirectReadKeyboard();	
		FrameCounterHandler();
		timeRemaining-=NormalFrameTime;
	}
	while(timeRemaining>0 && !GotAnyKey);
}

void Show_ARebellionGame(void)
{
	int timeRemaining = 7*ONE_FIXED;
	do
	{
		{
			BLTMenuToScreen(&Starfield_Backdrop);
			PlayMenuMusic();

			if (timeRemaining > 13*ONE_FIXED/2)
			{
				DrawGraphicWithAlphaChannel(&RebellionLogo,timeRemaining*2-13*ONE_FIXED);
 			}
			else if (timeRemaining > 5*ONE_FIXED)
			{
				DrawGraphicWithAlphaChannel(&RebellionLogo,0);
			}
			else if (timeRemaining > 3*ONE_FIXED)
			{
				DrawGraphicWithAlphaChannel(&RebellionLogo, ONE_FIXED - (timeRemaining-3*ONE_FIXED)/2);
			}

			FlipBuffers();
		}
		DirectReadKeyboard();	
		FrameCounterHandler();
		timeRemaining-=NormalFrameTime;
	}
	while(timeRemaining>ONE_FIXED/4 && !GotAnyKey);
}
void Show_AvPLogo(void)
{
	int timeRemaining = 5*ONE_FIXED;
	do
	{
		{
			BLTMenuToScreen(&Starfield_Backdrop);
			PlayMenuMusic();

			if (timeRemaining > 9*ONE_FIXED/2)
			{
				DrawGraphicWithAlphaChannel(&AvPLogo,timeRemaining*2-9*ONE_FIXED);
 			}
			else if (timeRemaining > 4*ONE_FIXED)
			{
				DrawGraphicWithAlphaChannel(&AvPLogo,0);
			}
			else
			{
				DrawGraphicWithAlphaChannel(&AvPLogo, ONE_FIXED - timeRemaining/4);
			}

			FlipBuffers();
		}
		DirectReadKeyboard();	
		FrameCounterHandler();
		timeRemaining-=NormalFrameTime;
	}
	while(timeRemaining>0 && !GotAnyKey);
}



	
void DrawGraphicWithAlphaChannel(MENUGRAPHIC *image, int alpha)
{
	DDSURFACEDESC ddsdback, ddsdimage;
	
	unsigned short *srcPtr;
	unsigned short *destPtr;

	long destPitch;
	
	memset(&ddsdback, 0, sizeof(ddsdback));
	memset(&ddsdimage, 0, sizeof(ddsdimage));
	ddsdback.dwSize = sizeof(ddsdback);
	ddsdimage.dwSize = sizeof(ddsdimage);

	alpha = ONE_FIXED-alpha;


	/* lock the image */
	while ((image->image_ptr)->Lock(NULL, &ddsdimage, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	/* lock the backbuffer */
	while (lpDDSBack->Lock(NULL, &ddsdback, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	destPtr = (unsigned short *)ddsdimage.lpSurface;
	destPitch = ddsdimage.lPitch - ddsdimage.dwWidth;

	// okay, now we have the surfaces, we can copy from one to the other,
	// darkening pixels as we go
	{
		int x,y;

		for (y=(ScreenDescriptorBlock.SDB_Height - image->height)/2; y<(ScreenDescriptorBlock.SDB_Height - image->height)/2+image->height; y++)
		{
			srcPtr = (unsigned short *)ddsdback.lpSurface + (ScreenDescriptorBlock.SDB_Width - image->width)/2;
			srcPtr += y*ddsdback.lPitch/2;

			for (x=0; x<image->width; x++)
			{

				if (*destPtr)
				{
					unsigned int srcR,srcG,srcB;
					unsigned int destR,destG,destB;

					srcR = (int)(*srcPtr) & DisplayPixelFormat.dwRBitMask;
					srcG = (int)(*srcPtr) & DisplayPixelFormat.dwGBitMask;
					srcB = (int)(*srcPtr) & DisplayPixelFormat.dwBBitMask;

					destR = (int)(*destPtr) & DisplayPixelFormat.dwRBitMask;
					destG = (int)(*destPtr) & DisplayPixelFormat.dwGBitMask;
					destB = (int)(*destPtr) & DisplayPixelFormat.dwBBitMask;
					#if 0
					srcR = destR + MUL_FIXED(alpha,srcR - destR);
					if (srcR>DisplayPixelFormat.dwRBitMask) srcR = DisplayPixelFormat.dwRBitMask;
					else srcR &= DisplayPixelFormat.dwRBitMask;

					srcG = destG + MUL_FIXED(alpha,srcG - destG);
					if (srcG>DisplayPixelFormat.dwGBitMask) srcG = DisplayPixelFormat.dwGBitMask;
					else srcG &= DisplayPixelFormat.dwGBitMask;

					srcB = destB + MUL_FIXED(alpha,srcB - destB);
					if (srcB>DisplayPixelFormat.dwBBitMask) srcB = DisplayPixelFormat.dwBBitMask;
					else srcB &= DisplayPixelFormat.dwBBitMask;
					#else
					srcR += MUL_FIXED(alpha,destR);
					if (srcR>DisplayPixelFormat.dwRBitMask) srcR = DisplayPixelFormat.dwRBitMask;
					else srcR &= DisplayPixelFormat.dwRBitMask;

					srcG += MUL_FIXED(alpha,destG);
					if (srcG>DisplayPixelFormat.dwGBitMask) srcG = DisplayPixelFormat.dwGBitMask;
					else srcG &= DisplayPixelFormat.dwGBitMask;

					srcB += MUL_FIXED(alpha,destB);
					if (srcB>DisplayPixelFormat.dwBBitMask) srcB = DisplayPixelFormat.dwBBitMask;
					else srcB &= DisplayPixelFormat.dwBBitMask;
					#endif
					*srcPtr = (short)(srcR|srcG|srcB);
				}
				srcPtr++;
				destPtr++;
			}
			destPtr += (ddsdimage.lPitch/2) - image->width; 
		}
	}
   	
   	lpDDSBack->Unlock((LPVOID)ddsdback.lpSurface);
   	(image->image_ptr)->Unlock((LPVOID)ddsdimage.lpSurface);

}
void DrawGraphicWithFadingLevel(MENUGRAPHIC *image, int alpha)
{
	DDSURFACEDESC ddsdback, ddsdimage;
	
	unsigned short *srcPtr;
	unsigned short *destPtr;

	long destPitch;
	
	memset(&ddsdback, 0, sizeof(ddsdback));
	memset(&ddsdimage, 0, sizeof(ddsdimage));
	ddsdback.dwSize = sizeof(ddsdback);
	ddsdimage.dwSize = sizeof(ddsdimage);

	alpha = ONE_FIXED-alpha;


	/* lock the image */
	while ((image->image_ptr)->Lock(NULL, &ddsdimage, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	/* lock the backbuffer */
	while (lpDDSBack->Lock(NULL, &ddsdback, DDLOCK_WAIT, NULL) == DDERR_WASSTILLDRAWING);

	destPtr = (unsigned short *)ddsdimage.lpSurface;
	destPitch = ddsdimage.lPitch - ddsdimage.dwWidth;

	// okay, now we have the surfaces, we can copy from one to the other,
	// darkening pixels as we go
	{
		int x,y;

		for (y=(ScreenDescriptorBlock.SDB_Height - image->height)/2; y<(ScreenDescriptorBlock.SDB_Height - image->height)/2+image->height; y++)
		{
			srcPtr = (unsigned short *)ddsdback.lpSurface + (ScreenDescriptorBlock.SDB_Width - image->width)/2;
			srcPtr += y*ddsdback.lPitch/2;

			for (x=0; x<image->width; x++)
			{

				if (*destPtr)
				{
					unsigned int srcR,srcG,srcB;

					srcR = (int)(*destPtr) & DisplayPixelFormat.dwRBitMask;
					srcG = (int)(*destPtr) & DisplayPixelFormat.dwGBitMask;
					srcB = (int)(*destPtr) & DisplayPixelFormat.dwBBitMask;
					
					srcR = MUL_FIXED(alpha,srcR);
					srcR &= DisplayPixelFormat.dwRBitMask;

					srcG = MUL_FIXED(alpha,srcG);
					srcG &= DisplayPixelFormat.dwGBitMask;
					
					srcB = MUL_FIXED(alpha,srcB);
					srcB &= DisplayPixelFormat.dwBBitMask;

					*srcPtr = (short)(srcR|srcG|srcB);
				}
				else
				{
					*srcPtr = 0;
				}
				srcPtr++;
				destPtr++;
			}
			destPtr += (ddsdimage.lPitch/2) - image->width; 
		}
	}
   	
   	lpDDSBack->Unlock((LPVOID)ddsdback.lpSurface);
   	(image->image_ptr)->Unlock((LPVOID)ddsdimage.lpSurface);

}
#endif