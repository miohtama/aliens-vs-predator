/* KJL 15:25:20 8/16/97
 *
 * smacker.c - functions to handle FMV playback
 *
 */
#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "stratdef.h"
#include "gamedef.h"
#include "smacker.h"
#include "avp_menus.h"
#include "avp_userprofile.h"
#include "d3_func.h"

#define UseLocalAssert 1
#include "ourasert.h"

int VolumeOfNearestVideoScreen;
int PanningOfNearestVideoScreen;

extern char *ScreenBuffer;
extern LPDIRECTSOUND DSObject; 
extern int GotAnyKey;
extern void DirectReadKeyboard(void);
extern IMAGEHEADER ImageHeaderArray[];
#if MaxImageGroups>1
extern int NumImagesArray[];
#else
extern int NumImages;
#endif

void PlayFMV(char *filenamePtr);
static int NextSmackerFrame(Smack *smackHandle);
static UpdatePalette(Smack *smackHandle);

static int GetSmackerPixelFormat(DDPIXELFORMAT *pixelFormatPtr);
void FindLightingValueFromFMV(unsigned short *bufferPtr);
void FindLightingValuesFromTriggeredFMV(unsigned char *bufferPtr, FMVTEXTURE *ftPtr);

int SmackerSoundVolume=ONE_FIXED/512;
int MoviesAreActive;
int IntroOutroMoviesAreActive=1;

int FmvColourRed;
int FmvColourGreen;
int FmvColourBlue;

void PlayFMV(char *filenamePtr)
{
	Smack* smackHandle;
	int playing = 1;

	if (!IntroOutroMoviesAreActive) return;

	/* use Direct sound */
	SmackSoundUseDirectSound(DSObject);
	/* open smacker file */
	smackHandle = SmackOpen(filenamePtr,SMACKTRACKS,SMACKAUTOEXTRA);
	if (!smackHandle)
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
	    if (!SmackWait(smackHandle)) 
			playing = NextSmackerFrame(smackHandle);
		
		/* do something else */
//		*(ScreenBuffer + 300*640 + smackHandle->FrameNum) = 255;
		
		FlipBuffers();
		#if ALLOW_SKIP_INTRO
		DirectReadKeyboard();
		if (GotAnyKey) playing = 0;
		#endif
	}
	/* close file */
	SmackClose(smackHandle);
}




static int NextSmackerFrame(Smack *smackHandle)
{
	/* do we have to change the palette? */
//	if (smackHandle->NewPalette) UpdatePalette(smackHandle);

	/* unpack frame */
	extern DDPIXELFORMAT DisplayPixelFormat;
	SmackToBuffer(smackHandle,(640-smackHandle->Width)/2,(480-smackHandle->Height)/2,640*2,480,(void*)ScreenBuffer,GetSmackerPixelFormat(&DisplayPixelFormat));
	SmackDoFrame(smackHandle);

	/* are we at the last frame yet? */
	if ((smackHandle->FrameNum==(smackHandle->Frames-1))) return 0;

	/* next frame, please */								  
	SmackNextFrame(smackHandle);
	return 1;
}

Smack* SmackHandle[4];
#define FMV_ON 0

void InitFMV(void)
{
#if FMV_ON
//	char *filenamePtr = "fmvs/hugtest.smk";//"dd64_64.smk";//nintendo.smk";//"trailer.smk";//"m_togran.smk";

	/* use Direct sound */
//  	SmackSoundUseDirectSound(DSObject);

	/* open smacker file */
	SmackHandle[0] = SmackOpen("fmvs/tyrargo.smk",SMACKTRACKS,SMACKAUTOEXTRA);
	SmackHandle[1] = SmackOpen("fmvs/final.smk",SMACKTRACKS,SMACKAUTOEXTRA);
	SmackHandle[2] = SmackOpen("fmvs/hugtest.smk",SMACKTRACKS,SMACKAUTOEXTRA);
	SmackHandle[3] = SmackOpen("fmvs/alien.smk",SMACKTRACKS,SMACKAUTOEXTRA);

#endif	
}

int NextFMVFrame(void*bufferPtr, int x, int y, int w, int h, int fmvNumber)
{
#if FMV_ON
	int smackerFormat;

	if(!SmackHandle[fmvNumber]) return 0;

    if (SmackWait(SmackHandle[fmvNumber])) return 0;

	/* unpack frame */
	{
		extern D3DINFO d3d;
		smackerFormat = GetSmackerPixelFormat(&(d3d.TextureFormat[d3d.CurrentTextureFormat].ddsd.ddpfPixelFormat));
	}
	if (smackerFormat) w*=2;
//	if (fmvNumber==0) FindLightingValueFromFMV((unsigned short *)bufferPtr);
  	SmackToBuffer(SmackHandle[fmvNumber],x,y,w,h,(void*)bufferPtr,smackerFormat);

	SmackDoFrame(SmackHandle[fmvNumber]);
//	textprint("at frame %d\n",SmackHandle->FrameNum);	

	/* next frame, please */
	SmackNextFrame(SmackHandle[fmvNumber]);
#endif
	return 1;

}

void UpdateFMVPalette(PALETTEENTRY *FMVPalette, int fmvNumber)
{
	unsigned char *c;
	int i;
	if(!SmackHandle[fmvNumber]) return;
	c=SmackHandle[fmvNumber]->Palette;
	
	for(i=0;i<256;i++)
	{    
		FMVPalette[i].peRed=(*c++);
	    FMVPalette[i].peGreen=(*c++);
    	FMVPalette[i].peBlue=(*c++);
	}	
}


void CloseFMV(void)
{
#if FMV_ON
	extern void KillFMVTexture(void);
	if(SmackHandle[0]) SmackClose(SmackHandle[0]);
	if(SmackHandle[1]) SmackClose(SmackHandle[1]);
	if(SmackHandle[2]) SmackClose(SmackHandle[2]);
	if(SmackHandle[3]) SmackClose(SmackHandle[3]);
	KillFMVTexture();
#endif
}

static int GetSmackerPixelFormat(DDPIXELFORMAT *pixelFormatPtr)
{

	if( (pixelFormatPtr->dwFlags & DDPF_RGB) && !(pixelFormatPtr->dwFlags & DDPF_PALETTEINDEXED8) )
	{
	    int m;
		int redShift=0;

		m = pixelFormatPtr->dwRBitMask;
		LOCALASSERT(m);
		while(!(m&1)) m>>=1;
		while(m&1)
		{
			m>>=1;
			redShift++;
		}

		if(redShift == 5)
		{
			/* Red componant is 5. */
			int greenShift=0;
			m = pixelFormatPtr->dwGBitMask;
			LOCALASSERT(m);
			while(!(m&1)) m>>=1;
			while(m&1)
			{
				m>>=1;
				greenShift++;
			}
			if(greenShift == 5)
			{
				/* Green componant is 5. */
				int blueShift=0;
				m = pixelFormatPtr->dwBBitMask;
				LOCALASSERT(m);
				while(!(m&1)) m>>=1;
				while(m&1)
				{
					m>>=1;
					blueShift++;
				}
				if(blueShift == 5)
				{
					/* Blue componant is 5. */
					return SMACKBUFFER555;
				}
				else
				{
					/* Blue componant is 6. */
					// not supported
					LOCALASSERT("Smacker does not support this pixel format"==0);
					return SMACKBUFFER555;
					//return SMACKBUFFER556;
				}
			}
			else
			{
				/* Green componant is 6. */
				return SMACKBUFFER565;
			}
		}
		else
		{
			/* Red componant is 6. */
			// not supported
			LOCALASSERT("Smacker does not support this pixel format"==0);
			return SMACKBUFFER555;
			//return SMACKBUFFER655;
		}
	}
	else
	{
		return 0;
	}
}











void StartMenuMusic(void)
{
	char *filenamePtr = "fmvs/introsound.smk";

	/* use Direct sound */
	SmackSoundUseDirectSound(DSObject);

	/* open smacker file */
	SmackHandle[0] = SmackOpen(filenamePtr,SMACKTRACKS|SMACKNEEDVOLUME|SMACKNEEDPAN,SMACKAUTOEXTRA);
}

void PlayMenuMusic(void)
{
	if(!SmackHandle[0]) return;

	SmackVolumePan(SmackHandle[0],SMACKTRACKS,SmackerSoundVolume*256,32768);

	if (SmackWait(SmackHandle[0])) return;

	/* unpack frame */
	SmackDoFrame(SmackHandle[0]);

	/* next frame, please */
	SmackNextFrame(SmackHandle[0]);
}
void EndMenuMusic(void)
{
	if(!SmackHandle[0]) return;
	
	SmackClose(SmackHandle[0]);
}


void FindLightingValueFromFMV(unsigned short *bufferPtr)
{
	unsigned int totalRed=0;
	unsigned int totalBlue=0;
	unsigned int totalGreen=0;

	int pixels = 128*96;//64*48;//256*192;
	do
	{
		int source = (int)(*bufferPtr++);
		totalBlue += source&31;
		source>>=5;
		totalGreen += source&63;
		source>>=6;
		totalRed += source&31; 
	}
	while(--pixels);
	FmvColourRed = totalRed*4/24;
	FmvColourGreen = totalGreen*4/48;
	FmvColourBlue = totalBlue*4/24;

}


void FindLightingValueFromCentredFMV(unsigned short *bufferPtr)
{
	unsigned int totalRed=0;
	unsigned int totalBlue=0;
	unsigned int totalGreen=0;

	int x,y;

	for (y=32; y<32+48; y++)
	for (x=32; x<32+64; x++)
	{
		int source = (int)(*(unsigned short*)(bufferPtr+x+y*128));
		totalBlue += source&31;
		source>>=5;
		totalGreen += source&63;
		source>>=6;
		totalRed += source&31; 
	}
	FmvColourRed = totalRed*4/24;
	FmvColourGreen = totalGreen*4/48;
	FmvColourBlue = totalBlue*4/24;

}





/* KJL 12:45:23 10/08/98 - FMVTEXTURE stuff */
#define MAX_NO_FMVTEXTURES 10
FMVTEXTURE FMVTexture[MAX_NO_FMVTEXTURES];
int NumberOfFMVTextures;

void ScanImagesForFMVs(void)
{
	extern void SetupFMVTexture(FMVTEXTURE *ftPtr);
	int i;
	IMAGEHEADER *ihPtr;
	NumberOfFMVTextures=0;

	#if MaxImageGroups>1
	for (j=0; j<MaxImageGroups; j++)
	{
		if (NumImagesArray[j])
		{
			ihPtr = &ImageHeaderArray[j*MaxImages];
			for (i = 0; i<NumImagesArray[j]; i++, ihPtr++)
			{
	#else
	{
		if(NumImages)
		{
			ihPtr = &ImageHeaderArray[0];
			for (i = 0; i<NumImages; i++, ihPtr++)
			{
	#endif
				char *strPtr;
				if(strPtr = strstr(ihPtr->ImageName,"FMVs"))
				{
					Smack *smackHandle;
					char filename[30];
					{
						char *filenamePtr = filename;
						do
						{
							*filenamePtr++ = *strPtr;
						}
						while(*strPtr++!='.');

						*filenamePtr++='s';
						*filenamePtr++='m';
						*filenamePtr++='k';
						*filenamePtr=0;
					}
					
					smackHandle = SmackOpen(filename,SMACKTRACKS|SMACKNEEDVOLUME|SMACKNEEDPAN,SMACKAUTOEXTRA);
					if (smackHandle)
					{
						FMVTexture[NumberOfFMVTextures].IsTriggeredPlotFMV = 0;
					}
					else
					{
						FMVTexture[NumberOfFMVTextures].IsTriggeredPlotFMV = 1;
					}

					{
						FMVTexture[NumberOfFMVTextures].SmackHandle = smackHandle;
						FMVTexture[NumberOfFMVTextures].ImagePtr = ihPtr;
						FMVTexture[NumberOfFMVTextures].StaticImageDrawn=0;
						SetupFMVTexture(&FMVTexture[NumberOfFMVTextures]);
						NumberOfFMVTextures++;
					}
				}
			}		
		}
	}


}

void UpdateAllFMVTextures(void)
{	
	extern void UpdateFMVTexture(FMVTEXTURE *ftPtr);
	int i = NumberOfFMVTextures;

	while(i--)
	{
		UpdateFMVTexture(&FMVTexture[i]);
	}

}

void ReleaseAllFMVTextures(void)
{	
	extern void UpdateFMVTexture(FMVTEXTURE *ftPtr);
	int i = NumberOfFMVTextures;

	while(i--)
	{
		FMVTexture[i].MessageNumber = 0;
		if(FMVTexture[i].SmackHandle)
		{
			SmackClose(FMVTexture[i].SmackHandle);
			FMVTexture[i].SmackHandle=0;
		}
		if (FMVTexture[i].SrcTexture)
		{
			ReleaseD3DTexture(FMVTexture[i].SrcTexture);
			FMVTexture[i].SrcTexture=0;
		}
		if (FMVTexture[i].SrcSurface)
		{
			ReleaseDDSurface(FMVTexture[i].SrcSurface);
			FMVTexture[i].SrcSurface=0;
		}
		if (FMVTexture[i].DestTexture)
		{	
			ReleaseD3DTexture(FMVTexture[i].DestTexture);
			FMVTexture[i].DestTexture = 0;
		}
	}

}


int NextFMVTextureFrame(FMVTEXTURE *ftPtr, void *bufferPtr)
{
	int smackerFormat;
	int w = 128;
	
	{
		extern D3DINFO d3d;
		smackerFormat = GetSmackerPixelFormat(&(d3d.TextureFormat[d3d.CurrentTextureFormat].ddsd.ddpfPixelFormat));
	}
	if (smackerFormat) w*=2;

	if (MoviesAreActive && ftPtr->SmackHandle)
	{
		int volume = MUL_FIXED(SmackerSoundVolume*256,GetVolumeOfNearestVideoScreen());
		SmackVolumePan(ftPtr->SmackHandle,SMACKTRACKS,volume,PanningOfNearestVideoScreen);
		ftPtr->SoundVolume = SmackerSoundVolume;
	    
	    if (SmackWait(ftPtr->SmackHandle)) return 0;
		/* unpack frame */
	  	SmackToBuffer(ftPtr->SmackHandle,0,0,w,96,bufferPtr,smackerFormat);

		SmackDoFrame(ftPtr->SmackHandle);

		/* are we at the last frame yet? */
		if (ftPtr->IsTriggeredPlotFMV && (ftPtr->SmackHandle->FrameNum==(ftPtr->SmackHandle->Frames-1)) )
		{
			SmackClose(ftPtr->SmackHandle);
			ftPtr->SmackHandle = 0;
			ftPtr->MessageNumber = 0;
		}
		else
		{
			/* next frame, please */
			SmackNextFrame(ftPtr->SmackHandle);
		}
		ftPtr->StaticImageDrawn=0;
	}
	else if (!ftPtr->StaticImageDrawn || smackerFormat)
	{
		int i = w*96/4;
		unsigned int seed = FastRandom();
		int *ptr = (int*)bufferPtr;
		do
		{
			seed = ((seed*1664525)+1013904223);
			*ptr++ = seed;
		}
		while(--i);
		ftPtr->StaticImageDrawn=1;
	}
	FindLightingValuesFromTriggeredFMV((unsigned char*)bufferPtr,ftPtr);
	return 1;

}

void UpdateFMVTexturePalette(FMVTEXTURE *ftPtr)
{
	unsigned char *c;
	int i;

	if (MoviesAreActive && ftPtr->SmackHandle)
	{
		c=ftPtr->SmackHandle->Palette;
		
		for(i=0;i<256;i++)
		{   
			ftPtr->SrcPalette[i].peRed=(*c++);
		    ftPtr->SrcPalette[i].peGreen=(*c++);
	    	ftPtr->SrcPalette[i].peBlue=(*c++);
		}	
	}
	else
	{
	  	{
			unsigned int seed = FastRandom();
			for(i=0;i<256;i++)
			{   
				int l = (seed&(seed>>24)&(seed>>16));
				seed = ((seed*1664525)+1013904223);
				ftPtr->SrcPalette[i].peRed=l;
			    ftPtr->SrcPalette[i].peGreen=l;
		   		ftPtr->SrcPalette[i].peBlue=l;
		 	}	
		}
	}
}

extern void StartTriggerPlotFMV(int number)
{
	int i = NumberOfFMVTextures;
	char buffer[25];

	if (CheatMode_Active != CHEATMODE_NONACTIVE) return;

	sprintf(buffer,"FMVs//message%d.smk",number);
	{
		FILE* file=fopen(buffer,"rb");
		if(!file)
		{
			return;
		}
		fclose(file);
	}
	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
				SmackClose(FMVTexture[i].SmackHandle);
			}

			FMVTexture[i].SmackHandle = SmackOpen(&buffer,SMACKTRACKS|SMACKNEEDVOLUME|SMACKNEEDPAN,SMACKAUTOEXTRA);
			FMVTexture[i].MessageNumber = number;
		}
	}
}
extern void StartFMVAtFrame(int number, int frame)
{
	int i = NumberOfFMVTextures;

	StartTriggerPlotFMV(number);

	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
			    SmackSoundOnOff(FMVTexture[i].SmackHandle,0);  // turn off sound so that it is synched
				SmackGoto(FMVTexture[i].SmackHandle,frame);
			    SmackSoundOnOff(FMVTexture[i].SmackHandle,1);  // turn on sound so that it is synched
//				SmackNextFrame(FMVTexture[i].SmackHandle);
			}
		}
	}

}
extern void GetFMVInformation(int *messageNumberPtr, int *frameNumberPtr)
{
	int i = NumberOfFMVTextures;

	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
				*messageNumberPtr = FMVTexture[i].MessageNumber;
				*frameNumberPtr = FMVTexture[i].SmackHandle->FrameNum;
				return;
			}
		}
	}

	*messageNumberPtr = 0;
	*frameNumberPtr = 0;
}


extern void InitialiseTriggeredFMVs(void)
{
	int i = NumberOfFMVTextures;
	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
				SmackClose(FMVTexture[i].SmackHandle);
				FMVTexture[i].MessageNumber = 0;
			}

			FMVTexture[i].SmackHandle = 0;
		}
	}
}

void FindLightingValuesFromTriggeredFMV(unsigned char *bufferPtr, FMVTEXTURE *ftPtr)
{
	unsigned int totalRed=0;
	unsigned int totalBlue=0;
	unsigned int totalGreen=0;
	#if 0
	int pixels = 128*96;//64*48;//256*192;
	do
	{
		unsigned char source = (*bufferPtr++);
		totalBlue += ftPtr->SrcPalette[source].peBlue;
		totalGreen += ftPtr->SrcPalette[source].peGreen;
		totalRed += ftPtr->SrcPalette[source].peRed; 
	}
	while(--pixels);
	#else
	int pixels = 128*96/4;//64*48;//256*192;
	unsigned int *source = (unsigned int*) (bufferPtr);
	do
	{
		int s = *source++;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		#if 0
		s >>=8;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		s >>=8;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		s >>=8;
		{
			int t = s;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		#endif
	}
	while(--pixels);
	#endif
	FmvColourRed = totalRed/48*16;
	FmvColourGreen = totalGreen/48*16;
	FmvColourBlue = totalBlue/48*16;

}

extern int NumActiveBlocks;
extern DISPLAYBLOCK *ActiveBlockList[];
#include "showcmds.h"
int GetVolumeOfNearestVideoScreen(void)
{											  
	extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
	int numberOfObjects = NumActiveBlocks;
	int leastDistanceRecorded = 0x7fffffff;
	VolumeOfNearestVideoScreen = 0;

	{
		extern char LevelName[];
		if (!_stricmp(LevelName,"invasion_a"))
		{
			VolumeOfNearestVideoScreen = ONE_FIXED;
			PanningOfNearestVideoScreen = ONE_FIXED/2;
		}
	}

	while (numberOfObjects)
	{
		DISPLAYBLOCK* objectPtr = ActiveBlockList[--numberOfObjects];
		STRATEGYBLOCK* sbPtr = objectPtr->ObStrategyBlock;

		if (sbPtr)
		{
			if (sbPtr->I_SBtype == I_BehaviourVideoScreen)
			{
				int dist;
				VECTORCH disp;

				disp.vx = objectPtr->ObWorld.vx - Global_VDB_Ptr->VDB_World.vx;
				disp.vy = objectPtr->ObWorld.vy - Global_VDB_Ptr->VDB_World.vy;
				disp.vz = objectPtr->ObWorld.vz - Global_VDB_Ptr->VDB_World.vz;
				
				dist = Approximate3dMagnitude(&disp);
				if (dist<leastDistanceRecorded && dist<ONE_FIXED)
				{
					leastDistanceRecorded = dist;
					VolumeOfNearestVideoScreen = ONE_FIXED + 1024 - dist/2;
					if (VolumeOfNearestVideoScreen>ONE_FIXED) VolumeOfNearestVideoScreen = ONE_FIXED;

					{
						VECTORCH rightEarDirection;
						#if 0
						rightEarDirection.vx = Global_VDB_Ptr->VDB_Mat.mat11;
						rightEarDirection.vy = Global_VDB_Ptr->VDB_Mat.mat12;
						rightEarDirection.vz = Global_VDB_Ptr->VDB_Mat.mat13;
						Normalise(&disp);
						#else
						rightEarDirection.vx = Global_VDB_Ptr->VDB_Mat.mat11;
						rightEarDirection.vy = 0;
						rightEarDirection.vz = Global_VDB_Ptr->VDB_Mat.mat31;
						disp.vy=0;
						Normalise(&disp);
						Normalise(&rightEarDirection);
						#endif
						PanningOfNearestVideoScreen = 32768 + DotProduct(&disp,&rightEarDirection)/2;
					}
				}
			}
		}
	}
	PrintDebuggingText("Volume: %d, Pan %d\n",VolumeOfNearestVideoScreen,PanningOfNearestVideoScreen);
	return VolumeOfNearestVideoScreen;
}