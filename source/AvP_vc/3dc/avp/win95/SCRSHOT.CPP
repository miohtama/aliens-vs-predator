#include "3dc.h"
#include "bmp2.h"
#include "endianio.h"
#include <String.hpp>
#include "scrshot.hpp"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "ourasert.h"
#include "frontend\avp_menus.h"
extern "C"{
 extern DDPIXELFORMAT DisplayPixelFormat;
 extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
 extern int VideoModeTypeScreen;
 extern long BackBufferPitch;
 extern unsigned char *ScreenBuffer;
 extern unsigned char TestPalette[];
 extern unsigned char KeyboardInput[];
 extern unsigned char DebouncedKeyboardInput[];
 extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
 extern MODULE* playerPherModule;	
};

typedef VOID (*PutWord_F)(WORD, FILE *);
typedef VOID (*PutDword_F)(DWORD, FILE *);

extern void LoadModuleData();

void LogCameraPosForModuleLinking()
{
	if(!playerPherModule) return;
	if(!playerPherModule->name) return;
	
	char Filename[100]={"avp_rifs\\"};
	
	strcat(Filename,Env_List[AvP.CurrentEnv]->main);
	strcat(Filename,".mlf");

	FILE* file=fopen(Filename,"ab");
	if(!file)return;

	char output_buffer[300];
	int length=0;
	
	strcpy(output_buffer,playerPherModule->name);
	length+=(strlen(playerPherModule->name)+4)&~3;

	*(VECTORCH*)&output_buffer[length]=playerPherModule->m_world;
	length+=sizeof(VECTORCH);
	*(MATRIXCH*)&output_buffer[length]=Global_VDB_Ptr->VDB_Mat;
	length+=sizeof(MATRIXCH);
	*(VECTORCH*)&output_buffer[length]=Global_VDB_Ptr->VDB_World;
	length+=sizeof(VECTORCH);
	
	if(length%4 !=0)
	{
		GLOBALASSERT(0);
	}

	fwrite(&output_buffer[0],4,length/4,file);
	fclose(file);		
	textprint("Saving camera for module links");
}
int SaveCameraPosKeyPressed=0;
static BOOL ModuleLinkAssist=FALSE;

void HandleScreenShot()
{
	#ifdef AVP_DEBUG_VERSION

	if (DebouncedKeyboardInput[KEY_F8])
		ScreenShot();

	if (KeyboardInput[KEY_F7])
	{
		if(!SaveCameraPosKeyPressed)
		{
			if(KeyboardInput[KEY_LEFTSHIFT]||KeyboardInput[KEY_RIGHTSHIFT])
			{
				ModuleLinkAssist=TRUE;
				DeleteFile("avp_rifs\\module.aaa");
			}
			else
			{
				LogCameraPosForModuleLinking();
				SaveCameraPosKeyPressed=1;
			}
		}
	}
	else
		SaveCameraPosKeyPressed=0;
	
	if(AvP.MainLoopRunning && ModuleLinkAssist)LoadModuleData();

	#endif
}
					 
void ScreenShot()
{
	char Name[40];
	strcpy(Name,"AVP");
	int length=strlen(Name);
	strncpy(&Name[length],"00.bmp",8);
	for(int i=0;i<100;i++)
	{
		Name[length]=i/10+'0';
		Name[length+1]=(i%10)+'0';
		FILE* tempfp=fopen(Name,"r");
		if(!tempfp)break;
		else
		{
			fclose(tempfp);
		}
	}
	if(i==100) return;
	

	FILE * fp = fopen(Name,"wb");
	if (!fp)
	{
		return;
	}
	
	BMPHEADER2 h;

	// fill out header
	
	h.Header.Type      = 'B'+'M'*256;
	h.Header.Reserved1 = 0;
	h.Header.Reserved2 = 0;
	h.Header.Offset    = 14+40+0;

	/*
	** The type of information found in a BMP structure is indicated by
	** the Size (Information Headere Size) field with a non-zero value.
	*/
	h.PmInfo.Size   = 0;
	h.Pm2Info.Size  = 0;

	h.WinInfo.Size          = 40;
	h.WinInfo.Width         = ScreenDescriptorBlock.SDB_Width;
	h.WinInfo.Height        = ScreenDescriptorBlock.SDB_Height;
	h.WinInfo.Planes        = 1;
	h.WinInfo.BitCount      = 24;
	h.WinInfo.Compression   = 0;
	h.WinInfo.SizeImage     = h.WinInfo.Width*h.WinInfo.Height*3;
	h.WinInfo.XPelsPerMeter = h.WinInfo.Width;
	h.WinInfo.YPelsPerMeter = h.WinInfo.Height;
	h.WinInfo.ClrUsed       = 0;
	h.WinInfo.ClrImportant  = 0;

	h.Header.FileSize  = h.WinInfo.SizeImage + h.Header.Offset + 8;

	// write header

	PutWord_F PutWord = PutLittleWord;
	PutDword_F PutDword = PutLittleDword;

    PutWord(h.Header.Type, fp);
    PutDword(h.Header.FileSize, fp);
    PutWord(h.Header.Reserved1, fp);
    PutWord(h.Header.Reserved2, fp);
    PutDword(h.Header.Offset, fp);

	PutDword(h.WinInfo.Size, fp);

    PutDword(h.WinInfo.Width, fp);
    PutDword(h.WinInfo.Height, fp);
    PutWord(h.WinInfo.Planes, fp);
    PutWord(h.WinInfo.BitCount, fp);
    PutDword(h.WinInfo.Compression, fp);
    PutDword(h.WinInfo.SizeImage, fp);
    PutDword(h.WinInfo.XPelsPerMeter, fp);
    PutDword(h.WinInfo.YPelsPerMeter, fp);
    PutDword(h.WinInfo.ClrUsed, fp);
    PutDword(h.WinInfo.ClrImportant, fp);

	
	int red_shift,red_scale,green_shift,green_scale,blue_shift,blue_scale;
	if(VideoModeTypeScreen==VideoModeType_15)
	{
    	int m;
    	for (red_shift = 0, m = DisplayPixelFormat.dwRBitMask; 
    	   !(m & 1); red_shift++, m >>= 1);
    	red_scale=255/m;
    	
    	for (green_shift = 0, m = DisplayPixelFormat.dwGBitMask; 
    	   !(m & 1); green_shift++, m >>= 1);
    	green_scale=255/m;
    	
    	for (blue_shift = 0, m = DisplayPixelFormat.dwBBitMask; 
    	   !(m & 1); blue_shift++, m >>= 1);
    	blue_scale=255/m;
		
	}
	
	// write 24 bit image

	LockSurfaceAndGetBufferPointer();
	unsigned char* BufferPtr=ScreenBuffer+BackBufferPitch*(h.WinInfo.Height-1);
	for (i=h.WinInfo.Height-1; i>=0; --i)
	{
		int j;
		if(VideoModeTypeScreen==VideoModeType_8)
		{
			for (j=0; j<h.WinInfo.Width; ++j)
			{
				PutByte((BYTE)TestPalette[BufferPtr[j]*3+2]<<2,fp);  //b
				PutByte((BYTE)TestPalette[BufferPtr[j]*3+1]<<2,fp);  //g
				PutByte((BYTE)TestPalette[BufferPtr[j]*3]<<2,fp);  //r
			}
		}
		else if(VideoModeTypeScreen==VideoModeType_15)
		{
			short colour;
			for (j=0; j<h.WinInfo.Width; ++j)
			{
				colour=*(short*)&BufferPtr[j*2];
				PutByte((BYTE)(((colour & DisplayPixelFormat.dwBBitMask)>>blue_shift)*blue_scale),fp);  //b
				PutByte((BYTE)(((colour & DisplayPixelFormat.dwGBitMask)>>green_shift)*green_scale),fp);  //g
				PutByte((BYTE)(((colour & DisplayPixelFormat.dwRBitMask)>>red_shift)*red_scale),fp);  //r
			}
		}
		else if(VideoModeTypeScreen==VideoModeType_24)
		{
			for (j=0; j<h.WinInfo.Width; ++j)
			{
				PutByte((BYTE)BufferPtr[j*3+2],fp);  //b
				PutByte((BYTE)BufferPtr[j*3+1],fp);  //g
				PutByte((BYTE)BufferPtr[j*3],fp);  //r
			}
			
		}
		// pad to 4 byte boundary
		for (j=~(h.WinInfo.Width*3-1) & 3; j; --j) PutByte(0,fp);
		BufferPtr-=BackBufferPitch;
	}
	UnlockSurface();

	fclose(fp);
	
}


