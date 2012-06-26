/* Bink! player, KJL 99/4/30 */
#include "bink.h"

#include "3dc.h"
#include "d3_func.h"

#define UseLocalAssert 1
#include "ourasert.h"

extern char *ScreenBuffer;
extern LPDIRECTSOUND DSObject; 
extern int GotAnyKey;
extern int IntroOutroMoviesAreActive;
extern DDPIXELFORMAT DisplayPixelFormat;

extern void DirectReadKeyboard(void);


static int BinkSurfaceType;

void PlayBinkedFMV(char *filenamePtr)
{
	BINK* binkHandle;
	int playing = 1;

	if (!IntroOutroMoviesAreActive) return;

//	if (!IntroOutroMoviesAreActive) return;
	BinkSurfaceType = GetBinkPixelFormat();
	
	/* skip FMV if surface type not supported */
	if (BinkSurfaceType == -1) return;
	
	/* use Direct sound */
	BinkSoundUseDirectSound(DSObject);
	/* open smacker file */
	binkHandle = BinkOpen(filenamePtr,0);
	if (!binkHandle)
	{
		char message[100];
		sprintf(message,"Unable to access file: %s\n",filenamePtr);
		MessageBox(NULL,message,"AvP Error",MB_OK+MB_SYSTEMMODAL);
		exit(0x111);
		return;
	}

	while(playing)
	{
		CheckForWindowsMessages();
	    if (!BinkWait(binkHandle)) 
			playing = NextBinkFrame(binkHandle);
		
		FlipBuffers();
		DirectReadKeyboard();
		if (GotAnyKey) playing = 0;
	}
	/* close file */
	BinkClose(binkHandle);
}

static int NextBinkFrame(BINK *binkHandle)
{
	/* unpack frame */
	BinkDoFrame(binkHandle);

	BinkCopyToBuffer(binkHandle,(void*)ScreenBuffer,640*2,480,(640-binkHandle->Width)/2,(480-binkHandle->Height)/2,BinkSurfaceType);

	//BinkToBuffer(binkHandle,(640-binkHandle->Width)/2,(480-binkHandle->Height)/2,640*2,480,(void*)ScreenBuffer,GetBinkPixelFormat(&DisplayPixelFormat));

	/* are we at the last frame yet? */
	if ((binkHandle->FrameNum==(binkHandle->Frames-1))) return 0;

	/* next frame, please */								  
	BinkNextFrame(binkHandle);
	return 1;
}

static int GetBinkPixelFormat(void)
{
	if( (DisplayPixelFormat.dwFlags & DDPF_RGB) && !(DisplayPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) )
	{
	    int m;
		int redShift=0;
		int greenShift=0;
		int blueShift=0;

		m = DisplayPixelFormat.dwRBitMask;
		LOCALASSERT(m);
		while(!(m&1)) m>>=1;
		while(m&1)
		{
			m>>=1;
			redShift++;
		}

		m = DisplayPixelFormat.dwGBitMask;
		LOCALASSERT(m);
		while(!(m&1)) m>>=1;
		while(m&1)
		{
			m>>=1;
			greenShift++;
		}

		m = DisplayPixelFormat.dwBBitMask;
		LOCALASSERT(m);
		while(!(m&1)) m>>=1;
		while(m&1)
		{
			m>>=1;
			blueShift++;
		}

		if(redShift == 5)
		{
			if (greenShift == 5)
			{
				if (blueShift == 5)
				{
					return BINKSURFACE555;
				}
				else // not supported
				{
					return -1;
				}
			}
			else if (greenShift == 6)
			{
				if (blueShift == 5)
				{
					return BINKSURFACE565;
				}
				else // not supported
				{
					return -1;
				}
			}
		}
		else if (redShift == 6)
		{
			if (greenShift == 5)
			{
				if (blueShift == 5)
				{
					return BINKSURFACE655;
				}
				else // not supported
				{
					return -1;
				}
			}
			else if (greenShift == 6)
			{
				if (blueShift == 4)
				{
					return BINKSURFACE664;
				}
				else // not supported
				{
					return -1;
				}
			}
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

BINK *MenuBackground = 0;

extern void StartMenuBackgroundBink(void)
{
	char *filenamePtr = "fmvs/menubackground.bik";//newer.bik";

	/* open smacker file */
	MenuBackground = BinkOpen(filenamePtr,0);
	BinkSurfaceType = GetBinkPixelFormat();
	BinkDoFrame(MenuBackground);
}

extern int PlayMenuBackgroundBink(void)
{
	int newframe = 0;
	if(!MenuBackground) return 0;

	if (!BinkWait(MenuBackground)&&IntroOutroMoviesAreActive) newframe=1;

	if(newframe) BinkDoFrame(MenuBackground);

	BinkCopyToBuffer(MenuBackground,(void*)ScreenBuffer,640*2,480,(640-MenuBackground->Width)/2,(480-MenuBackground->Height)/2,BinkSurfaceType|BINKSURFACECOPYALL);

	/* next frame, please */								  
	if(newframe)BinkNextFrame(MenuBackground);

	return 1;
}
extern void EndMenuBackgroundBink(void)
{
	if(!MenuBackground) return;
	
	BinkClose(MenuBackground);
	MenuBackground = 0;
}

