/*KJL*********************************************************************
* krender.c - Kevin's scandrawing code. (New render, new danger.)        *
*                                                                        *
* The new scandraws are called by patching the functions below into the  *
* jumptables used in item.c. If the KRENDER_ON flag in krender.h is zero *
* then the old scandraws will be used.                                   *
*********************************************************************KJL*/

/* this file is a bit messy at the moment, but I'll be back later... */
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "gamedef.h"
#include "krender.h"
#include "vision.h"

#define UseLocalAssert Yes
#include "ourasert.h"

#define PENTIUM_PROFILING_ON 0
#if PENTIUM_PROFILING_ON
#include "pentime.h"
#else
#define ProfileStart();
#define ProfileStop(x);
#endif

int ReciprocalTable[321]; 
#if 1
#define DIVIDE(a,b) ((a)/(b))
#else
#define DIVIDE(a,b) (MUL_FIXED((a),ReciprocalTable[(b)]))
#endif


int Transparent;

/* assembler fns */
extern void ScanDraw2D_Gouraud(void);
extern void ScanDraw2D_GouraudTransparent(void);
extern void ScanDraw2D_VAlignedTransparent(void);
extern void ScanDraw2D_VAlignedOpaque(void);
extern void ScanDraw2D_Transparent(void);
extern void ScanDraw2D_TransparentLit(void);
extern void ScanDraw2D_Opaque(void);
extern void ScanDraw_GouraudScan(void);
extern void ScanDrawF3D_Gouraud(void);

/* globals accessed by the assembler routines */
unsigned char *SCASM_Lighting;
unsigned char *SCASM_Destination;
unsigned char *SCASM_Bitmap;
int SCASM_StartU;
int SCASM_StartV;
int SCASM_StartI;
int SCASM_DeltaU;
int SCASM_DeltaV;
int SCASM_DeltaI;
int SCASM_ScanLength;
int	SCASM_ShadingTableSize;
int	SCASM_TextureDeltaScan;

/* things from item.c */
extern int NumScans;
extern int ItemColour;
extern IMAGEHEADER *ImageHeaderPtrs[MaxImages];
extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
extern unsigned char *TextureLightingTable;
extern int MIP_Index;
extern int ScanData[maxpolys*maxscansize];
extern unsigned char *ScreenBuffer;
extern long BackBufferPitch;

void Draw_Gouraud3dTexture_Spans(int *itemptr);

int KRenderDrawMode = 0;

/***********/

void KR_ScanDraw_Item_Gouraud2dTexturePolygon_VideoModeType_8(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	I_GOURAUD2DTEXTUREPOLYGON_SCAN *sptr;

	if(NumScans)
	{
		int y = NumScans;
		/* Get the Image Data required for the Draw */
		{
			IMAGEHEADER *ImageHdr;
			{
				int TxIndex  = ItemColour & ClrTxDefn;
				ImageHdr = ImageHeaderPtrs[TxIndex];
			}
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
			{
				TEXTURE **TxImagePtr = &ImageHdr->ImagePtr;
				SCASM_Bitmap = TxImagePtr[MIP_Index];
				GLOBALASSERT(SCASM_Bitmap);
				SCASM_TextureDeltaScan = ImageHdr->ImageWidth >> MIP_Index;
				#if MIP_ROUNDUP
				if (ImageHdr->ImageWidth & (1<<MIP_Index)-1) ++SCASM_TextureDeltaScan;
				#endif


			}
			else
			{
				SCASM_TextureDeltaScan = ImageHdr->ImageWidth;
				SCASM_Bitmap = ImageHdr->ImagePtr;
				GLOBALASSERT(SCASM_Bitmap);
			}
		}
		SCASM_Lighting = TextureLightingTable;
		
		if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_Raw256) SCASM_ShadingTableSize = 256;
		else SCASM_ShadingTableSize = 64;

		sptr = (I_GOURAUD2DTEXTUREPOLYGON_SCAN *) ScanData;
		do
		{
			int dx = sptr->ig2s_x2 - sptr->ig2s_x1;
			if (dx>0 && !(sptr->ig2s_y & KRenderDrawMode) )
			{
				/* Fixed Point U for Interpolation */
				{
					int StartU = sptr->ig2s_u1;
					SCASM_DeltaU = DIVIDE((sptr->ig2s_u2 - StartU),dx);
					SCASM_StartU = StartU;
				}
				
				/* Fixed Point V for Interpolation */
				{
					int StartV = sptr->ig2s_v1;
					SCASM_DeltaV = DIVIDE((sptr->ig2s_v2 - StartV),dx);
					SCASM_StartV = StartV;
				}
				/* Fixed Point I for Interpolation */
				{
					int StartI = sptr->ig2s_c1;
					SCASM_DeltaI = DIVIDE((sptr->ig2s_c2 - StartI),dx);
					SCASM_StartI = StartI;
				}
				/* Draw the Gouraud 2d Texture Scan */

				SCASM_Destination = ScreenBuffer + (sptr->ig2s_y * BackBufferPitch) + sptr->ig2s_x1;
				SCASM_ScanLength = dx;
				
				if(pheader->PolyFlags & iflag_ignore0)
				{
					ScanDraw2D_GouraudTransparent();
				}
				else
				{
					ScanDraw2D_Gouraud();
				}	
			}
			sptr++;
			y--;
		}
		while(y);
	}
	
}

void Draw3DScan(I_GOURAUD3DTEXTUREPOLYGON_SCAN *scanPtr, POLYHEADER *pheader)
{
	int dx = scanPtr->ig3s_x2 - scanPtr->ig3s_x1;

	if(dx > 0)
	{
		float uf; 
		float vf; 
		float uf2;
		float vf2;
		/* Get start and end UVs */
		{
			float ooz1;
			ooz1 = 65536.0 / scanPtr->ig3s_z1;
			uf = scanPtr->ig3s_u1 * ooz1;
			vf = scanPtr->ig3s_v1 * ooz1;
		}
		{
			float ooz2;
			ooz2 = 65536.0 / scanPtr->ig3s_z2;
			uf2 = scanPtr->ig3s_u2 * ooz2;
			vf2 = scanPtr->ig3s_v2 * ooz2;
		}
		if ( (uf>16700000.0)
		   ||(uf<0.0)
		   ||(vf>16700000.0)
		   ||(vf<0.0) )
		{
			textprint("WARNING: UV coords invalid\n");
   // 		LOCALASSERT(0);
			return;
		}
		if ( (uf2>16700000.0)
		   ||(uf2<0.0)
		   ||(vf2>16700000.0)
		   ||(vf2<0.0) )
		{
			textprint("WARNING: UV coords invalid\n");
	//		LOCALASSERT(0);
			return;
		}
		/* For UV interpolation */
		f2i(SCASM_StartU,uf);
		f2i(SCASM_StartV,vf);
		{
			int EndU,EndV;
			f2i(EndU,uf2);
			f2i(EndV,vf2);
			SCASM_DeltaU =(EndU-SCASM_StartU)/dx;
			SCASM_DeltaV =(EndV-SCASM_StartV)/dx;
		}

		/* Fixed Point I for Interpolation */
		{
			int StartI = scanPtr->ig3s_c1;
			SCASM_DeltaI = (scanPtr->ig3s_c2 - StartI)/dx;
			SCASM_StartI = StartI;
		}
		
		/* Draw the 3d Texture Scan */
		SCASM_Destination = ScreenBuffer + (scanPtr->ig3s_y * BackBufferPitch) + scanPtr->ig3s_x1;
		SCASM_ScanLength = dx;

		if(pheader->PolyFlags & iflag_ignore0)
		{
			ScanDraw2D_GouraudTransparent();
		}
		else
		{
			ScanDraw2D_Gouraud();
		}	
	}
}


void KR_ScanDraw_Item_Gouraud3dTexturePolygon_Linear_S_VideoModeType_8(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	
	int num_scans_s;
	I_GOURAUD3DTEXTUREPOLYGON_SCAN *sptr_array_ptr;
	I_GOURAUD3DTEXTUREPOLYGON_SCAN *next_free_sptr;
	I_GOURAUD3DTEXTUREPOLYGON_SCAN sptr_array[lin_s_max + 1];
	int StillSubdividing;
	int i;

	
	if(NumScans)
	{
		I_GOURAUD3DTEXTUREPOLYGON_SCAN *sptr;
		int y;
		/* Get the Image Data required for the Draw */
		{
			IMAGEHEADER *ImageHdr;
			{
				int TxIndex  = ItemColour & ClrTxDefn;
				ImageHdr = ImageHeaderPtrs[TxIndex];
			}
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
			{
				TEXTURE **TxImagePtr = &ImageHdr->ImagePtr;
				SCASM_Bitmap = TxImagePtr[MIP_Index];
				GLOBALASSERT(SCASM_Bitmap);
				SCASM_TextureDeltaScan = ImageHdr->ImageWidth >> MIP_Index;
				#if MIP_ROUNDUP
				if (ImageHdr->ImageWidth & (1<<MIP_Index)-1) ++SCASM_TextureDeltaScan;
				#endif

			}
			else
			{
				SCASM_TextureDeltaScan = ImageHdr->ImageWidth;
				SCASM_Bitmap = ImageHdr->ImagePtr;
				GLOBALASSERT(SCASM_Bitmap);
			}
		}
		SCASM_Lighting = TextureLightingTable;
		
		if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_Raw256) SCASM_ShadingTableSize = 256;
		else SCASM_ShadingTableSize = 64;

		
		sptr = (I_GOURAUD3DTEXTUREPOLYGON_SCAN *) ScanData;

		for(y = NumScans; y!=0; y--)
		{
			int dx = sptr->ig3s_x2 - sptr->ig3s_x1;
			if (dx>0 && !(sptr->ig3s_y & KRenderDrawMode) )
			{
				float zmax, zmin, z_ratio;
				
				/* Have a look at the z-ratio */
				if(sptr->ig3s_z1 > sptr->ig3s_z2)
				{
					zmax = sptr->ig3s_z1;
					zmin = sptr->ig3s_z2;
				}
				else
				{
					zmax = sptr->ig3s_z2;
					zmin = sptr->ig3s_z1;
				}
				z_ratio = (zmax * 256);


				/* Draw if the z ratio is inside the threshold */
				if(z_ratio < lin_s_zthr*zmin)
				{
					Draw3DScan(sptr,pheader);
				}
				/* Else subdivide until scan segments are inside threshold */
				else
				{
					/* Copy the current scan to the subdivision array */
					sptr_array_ptr = sptr_array;
					num_scans_s    = 1;

					sptr_array_ptr->ig3s_u1 = sptr->ig3s_u1;
					sptr_array_ptr->ig3s_v1 = sptr->ig3s_v1;
					sptr_array_ptr->ig3s_z1 = sptr->ig3s_z1;
					sptr_array_ptr->ig3s_c1 = sptr->ig3s_c1;

					sptr_array_ptr->ig3s_u2 = sptr->ig3s_u2;
					sptr_array_ptr->ig3s_v2 = sptr->ig3s_v2;
					sptr_array_ptr->ig3s_z2 = sptr->ig3s_z2;
					sptr_array_ptr->ig3s_c2 = sptr->ig3s_c2;

					sptr_array_ptr->ig3s_x1 = sptr->ig3s_x1;
					sptr_array_ptr->ig3s_x2 = sptr->ig3s_x2;

					sptr_array_ptr->ig3s_y = sptr->ig3s_y;


					/* Subdivide until no further divisions are needed */

					next_free_sptr = sptr_array_ptr + 1;
					do
					{
						sptr_array_ptr = sptr_array;

						StillSubdividing = No;					/* Assume not */

						for(i = num_scans_s; i!=0; i--)
						{
							#if 0
							/* z ratio of this scan */
							if(sptr_array_ptr->ig3s_z1 > sptr_array_ptr->ig3s_z2)
							{
								z_ratio = (sptr_array_ptr->ig3s_z1 * 256) /
												sptr_array_ptr->ig3s_z2;
							}
							else
							{
								z_ratio = (sptr_array_ptr->ig3s_z2 * 256) /
												sptr_array_ptr->ig3s_z1;
							}
							/* Is it within the threshold? */
							if(z_ratio > lin_s_zthr && num_scans_s < lin_s_max)
							#endif
							int overThreshold=0;
							/* z ratio of this scan */
							if(sptr_array_ptr->ig3s_z1 > sptr_array_ptr->ig3s_z2)
							{
								if ( (sptr_array_ptr->ig3s_z1 * 256) > (lin_s_zthr*sptr_array_ptr->ig3s_z2))
									overThreshold=1;
							}
							else
							{
								if ( (sptr_array_ptr->ig3s_z2 * 256) > (lin_s_zthr*sptr_array_ptr->ig3s_z1))
									overThreshold=1;
							}
							/* Is it within the threshold? */
							if(overThreshold && num_scans_s < lin_s_max)
							{
								int mid_x, mid_c;
								float mid_u, mid_v, mid_z;
								/* No - subdivide */
								StillSubdividing = Yes;

								mid_u = (sptr_array_ptr->ig3s_u1 + sptr_array_ptr->ig3s_u2) / 2;

								mid_v = (sptr_array_ptr->ig3s_v1 + sptr_array_ptr->ig3s_v2) / 2;

								mid_z = (sptr_array_ptr->ig3s_z1 + sptr_array_ptr->ig3s_z2) / 2;

								mid_c = (sptr_array_ptr->ig3s_c1 + sptr_array_ptr->ig3s_c2) / 2;

								mid_x = (sptr_array_ptr->ig3s_x1 + sptr_array_ptr->ig3s_x2) / 2;

								/* Create new scan */

								next_free_sptr->ig3s_u1 = mid_u;
								next_free_sptr->ig3s_v1 = mid_v;
								next_free_sptr->ig3s_z1 = mid_z;
								next_free_sptr->ig3s_c1 = mid_c;
								next_free_sptr->ig3s_x1 = mid_x;
								next_free_sptr->ig3s_u2 = sptr_array_ptr->ig3s_u2;
								next_free_sptr->ig3s_v2 = sptr_array_ptr->ig3s_v2;
								next_free_sptr->ig3s_z2 = sptr_array_ptr->ig3s_z2;
								next_free_sptr->ig3s_c2 = sptr_array_ptr->ig3s_c2;
								next_free_sptr->ig3s_x2 = sptr_array_ptr->ig3s_x2;
								next_free_sptr->ig3s_y = sptr_array_ptr->ig3s_y;
								/* Redefine old scan */

								sptr_array_ptr->ig3s_u2 = mid_u;
								sptr_array_ptr->ig3s_v2 = mid_v;
								sptr_array_ptr->ig3s_z2 = mid_z;
								sptr_array_ptr->ig3s_c2 = mid_c;
								sptr_array_ptr->ig3s_x2 = mid_x;

								/* Update pointer and counter */

								next_free_sptr++;
								num_scans_s++;

							}
							sptr_array_ptr++;
						}

					}
					while(StillSubdividing);

			   		/* Draw the scan array */
			   		sptr_array_ptr = sptr_array;
			   		for(i = num_scans_s; i!=0; i--)
					{
						Draw3DScan(sptr_array_ptr,pheader);
						sptr_array_ptr++;
					}
				}
			}
			sptr++;
		}
	}
}



void KR_ScanDraw_Item_2dTexturePolygon_VideoModeType_8(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	if(NumScans)
	{
		if(pheader->PolyFlags & iflag_nolight)
		{
			int y;
			I_2DTEXTUREPOLYGON_SCAN *sptr = (I_2DTEXTUREPOLYGON_SCAN *) ScanData;

			/* Offset in Screen Buffer */
			{
				IMAGEHEADER *ImageHdr;
				{
					int TxIndex  = ItemColour & ClrTxDefn;
					ImageHdr = ImageHeaderPtrs[TxIndex];
				}
				if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
				{
					TEXTURE **TxImagePtr = &ImageHdr->ImagePtr;
					SCASM_Bitmap = TxImagePtr[MIP_Index];
					GLOBALASSERT(SCASM_Bitmap);
					SCASM_TextureDeltaScan = ImageHdr->ImageWidth >> MIP_Index;
					#if MIP_ROUNDUP
					if (ImageHdr->ImageWidth & (1<<MIP_Index)-1) ++SCASM_TextureDeltaScan;
					#endif
				}
				else
				{
					SCASM_TextureDeltaScan = ImageHdr->ImageWidth;
					SCASM_Bitmap = ImageHdr->ImagePtr;
					GLOBALASSERT(SCASM_Bitmap);
				}
			}
			
			/* Get the Image Data required for the Draw */
			for(y=NumScans; y!=0; y--)
			{
				int dx = sptr->i2s_x2 - sptr->i2s_x1;

				if (dx>0 && !(sptr->i2s_y & KRenderDrawMode) )
				{
					/* Fixed Point U for Interpolation */
					{
						int StartU = sptr->i2s_u1;
						SCASM_DeltaU = (sptr->i2s_u2 - StartU)/dx;
						SCASM_StartU = StartU;
					}
					
					/* Fixed Point V for Interpolation */
					{
						int StartV = sptr->i2s_v1;
						SCASM_DeltaV = (sptr->i2s_v2 - StartV)/dx;
						SCASM_StartV = StartV;
					}

					SCASM_Destination = ScreenBuffer + (sptr->i2s_y * BackBufferPitch) + sptr->i2s_x1;
					SCASM_ScanLength = dx;

	
					/* Is VDelta = 0? */
					if(SCASM_DeltaV == 0)
					{
						if(pheader->PolyFlags & iflag_ignore0)
						{
							ScanDraw2D_VAlignedTransparent();
						}
						else
						{
							ScanDraw2D_VAlignedOpaque();
						}

					}
					else
					{
						if(pheader->PolyFlags & iflag_ignore0)
						{
						   	ScanDraw2D_Transparent();
						}
						else
						{
							ScanDraw2D_Opaque();
						}
					}
				}
				sptr++;
			}
		}
		#if 1
		else
		{
			int y;
			I_2DTEXTUREPOLYGON_SCAN *sptr = (I_2DTEXTUREPOLYGON_SCAN *) ScanData;
			/* Offset in Screen Buffer */
			{
				IMAGEHEADER *ImageHdr;
				{
					int TxIndex  = ItemColour & ClrTxDefn;
					ImageHdr = ImageHeaderPtrs[TxIndex];
				}
				if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
				{
					TEXTURE **TxImagePtr = &ImageHdr->ImagePtr;
					SCASM_Bitmap = TxImagePtr[MIP_Index];
					GLOBALASSERT(SCASM_Bitmap);
					SCASM_TextureDeltaScan = ImageHdr->ImageWidth >> MIP_Index;
					#if MIP_ROUNDUP
					if (ImageHdr->ImageWidth & (1<<MIP_Index)-1) ++SCASM_TextureDeltaScan;
					#endif
				}
				else
				{
					SCASM_TextureDeltaScan = ImageHdr->ImageWidth;
					SCASM_Bitmap = ImageHdr->ImagePtr;
					GLOBALASSERT(SCASM_Bitmap);
				}
			}
			SCASM_StartI = ItemColour >> TxDefn;
			
			SCASM_Lighting = TextureLightingTable;
			
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_Raw256) SCASM_ShadingTableSize = 256;
			else SCASM_ShadingTableSize = 64;
			
		
			/* Get the Image Data required for the Draw */
			for(y=NumScans; y!=0; y--)
			{
				int dx = sptr->i2s_x2 - sptr->i2s_x1;
				if (dx>0 && !(sptr->i2s_y & KRenderDrawMode) )
				{
					/* Fixed Point U for Interpolation */
					{
						int StartU = sptr->i2s_u1;
						SCASM_DeltaU = (sptr->i2s_u2 - StartU)/dx;
						SCASM_StartU = StartU;
					}
					
					/* Fixed Point V for Interpolation */
					{
						int StartV = sptr->i2s_v1;
						SCASM_DeltaV = (sptr->i2s_v2 - StartV)/dx;
						SCASM_StartV = StartV;
					}

					SCASM_Destination = ScreenBuffer + (sptr->i2s_y * BackBufferPitch) + sptr->i2s_x1;
					SCASM_ScanLength = dx;

	   	  	   	 	ScanDraw2D_TransparentLit();
	   	  	   	 //	PredatorScanDraw();
				}
				sptr++;
			}


		}
		#endif
	}

}
unsigned char LighterTable[255];
int predatorTimer = 0;
void ScanDraw_CloakedScan(void)
{
	unsigned char *screen = SCASM_Destination;
	int x = SCASM_ScanLength;

	unsigned char buffer[1000];
	do
	{
		buffer[x] =	*screen++;
	}
	while(--x);

	screen = SCASM_Destination;
	x = SCASM_ScanLength;

	if (FastRandom()&3==3 && x>2)
	{
		x--;
		screen++;
	}
	if (FastRandom()&3==3 && x>2)
	{
		x--;
	}

	do
	{
		{
			extern sine[];
			extern cosine[];
			unsigned char colour;
			int offset = (GetCos( ((x*2047)/SCASM_ScanLength + predatorTimer)&4095 )+65536)/2;
	   		offset = MUL_FIXED(offset,offset);
		  	colour = (buffer[SCASM_ScanLength-x+1]);
	//	  	colour = (buffer[x]);
			#if 0
			int i;
			for (i=0; i<=255;i++)
			{
				if (colour==*(TextureLightingTable + 256*200 + i))	break;
			}
			#endif
	 		colour=*(TextureLightingTable + 256*(60+MUL_FIXED(4,GetCos(predatorTimer&4095)) ) + colour);
			*screen++ = colour;

		}
	}
	while(--x);

}
#if 0
void PredatorScanDraw(void)
{
	unsigned char *screen = SCASM_Destination;
	unsigned char *source;

	do
	{
		source = SCASM_Bitmap+ (SCASM_StartU>>16) + (SCASM_StartV>>16)*SCASM_TextureDeltaScan;

		*screen++ = *source++;

		SCASM_StartU += SCASM_DeltaU;
		SCASM_StartV += SCASM_DeltaV;
	}
	while(--SCASM_ScanLength);

}
#else
void PredatorScanDraw(void)						   
{
	unsigned char *screen = SCASM_Destination;
	unsigned char *source;
	int x = SCASM_ScanLength;
	do
	{
		source = SCASM_Bitmap+ (SCASM_StartU>>16) + (SCASM_StartV>>16)*SCASM_TextureDeltaScan;

		if (*source) 
		{
			extern sine[];
			extern cosine[];
			unsigned char colour = *(screen);
 	  		*screen = LighterTable[colour];
		}
		screen++;
		SCASM_StartU += SCASM_DeltaU;
		SCASM_StartV += SCASM_DeltaV;
	}
	while(--x);

}
#endif

void Draw_Item_CloakedPolygon(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *)itemptr;
	int minYVertex = 0;
	int maxYVertex = 0;
	int maxVertexNum = 0;

	{
		I_GOURAUDPOLYGON_PT *vertexPtr = (I_GOURAUDPOLYGON_PT*)&pheader->Poly1stPt;
		int minY = vertexPtr->i_y;
		int maxY = vertexPtr->i_y;
		vertexPtr++;

		do
		{
			maxVertexNum++;
		
			if (minY > vertexPtr->i_y)
			{
				minY = vertexPtr->i_y;
				minYVertex = maxVertexNum;
			}
			else if (maxY < vertexPtr->i_y)
			{
				maxY = vertexPtr->i_y;
				maxYVertex = maxVertexNum;
			}
			vertexPtr++;
		}
		while(vertexPtr->i_x != Term);

	}

	
	/* Initialise the Scan Data Buffer */
	{
		I_GOURAUDPOLYGON_PT *vertexPtr = (I_GOURAUDPOLYGON_PT*)&pheader->Poly1stPt;
		I_GOURAUDPOLYGON_SCAN *polyScan;
		int curVertex;
		int SecondOpinion=0;
			
		NumScans=0;
		/* scan out the right edge */
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		curVertex = minYVertex;
		do
		{
			int height;
			int i,x,y;
			int deltaX,deltaI;

			int nextVertex = curVertex-1;
			if (nextVertex<0) nextVertex = maxVertexNum;
			
			height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
			if (height!=0)
			{
				int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
				int contrast = vertexPtr[nextVertex].i_int - vertexPtr[curVertex].i_int;

				deltaX = (width<<16)/height;
				deltaI = contrast/height;

				x = vertexPtr[curVertex].i_x<<16;
				i = vertexPtr[curVertex].i_int;

				for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
				{
					polyScan->igs_x2 = x>>16;
					x+=deltaX;
					polyScan->igs_c2 = i;
					i+=deltaI;

					polyScan->igs_y = y;
					NumScans++;
					polyScan++;
				}
			}
			curVertex--;
			if (curVertex<0) curVertex = maxVertexNum;

		}
		while(curVertex!=maxYVertex);

		/* scan out the left edge */
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		curVertex = minYVertex;
		do
		{
			int height;
			int i,x,y;
			int deltaX,deltaI;

			int nextVertex = curVertex+1;
			if (nextVertex>maxVertexNum) nextVertex = 0;
			
			height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
			if (height!=0)
			{
				int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
				int contrast = vertexPtr[nextVertex].i_int - vertexPtr[curVertex].i_int;

				deltaX = (width<<16)/height;
				deltaI = contrast/height;

				x = vertexPtr[curVertex].i_x<<16;
				i = vertexPtr[curVertex].i_int;

				for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
				{
					polyScan->igs_x1 = x>>16;
					x+=deltaX;
					polyScan->igs_c1 = i;
					i+=deltaI;
					
					SecondOpinion++;
					polyScan++;
				}
			}
			curVertex++;
			if (curVertex>maxVertexNum) curVertex = 0;

		}
		while(curVertex!=maxYVertex);

		if (SecondOpinion<NumScans)
		{
			textprint("WARNING: Rasterization error\n");
			NumScans = SecondOpinion;
//			LOCALASSERT(0);
  			return;
  		}

	}

	//draw 'em
	
	{
		unsigned char colour= pheader->PolyColour;
		I_GOURAUDPOLYGON_SCAN *polyScan;
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		SCASM_Lighting = TextureLightingTable+colour;
		SCASM_ShadingTableSize = 256;

		while(NumScans)
		{
			int dx = polyScan->igs_x2 - polyScan->igs_x1;
			if (dx>0)
			{
				SCASM_Destination = ScreenBuffer+(polyScan->igs_y * BackBufferPitch) + polyScan->igs_x1;
				SCASM_ScanLength = dx;
				SCASM_DeltaI = (polyScan->igs_c2-polyScan->igs_c1)/dx;
				SCASM_StartI = polyScan->igs_c1;
			   	ScanDraw_CloakedScan();
			}
			NumScans--;
			polyScan++;
		}
	}
}
#if 1
void KDraw_Item_GouraudPolygon(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *)itemptr;
	int minYVertex = 0;
	int maxYVertex = 0;
	int maxVertexNum = 0;

	{
		I_GOURAUDPOLYGON_PT *vertexPtr = (I_GOURAUDPOLYGON_PT*)&pheader->Poly1stPt;
		int minY = vertexPtr->i_y;
		int maxY = vertexPtr->i_y;
		vertexPtr++;

		do
		{
			maxVertexNum++;
		
			if (minY > vertexPtr->i_y)
			{
				minY = vertexPtr->i_y;
				minYVertex = maxVertexNum;
			}
			else if (maxY < vertexPtr->i_y)
			{
				maxY = vertexPtr->i_y;
				maxYVertex = maxVertexNum;
			}
			vertexPtr++;
		}
		while(vertexPtr->i_x != Term);

	}

	
	/* Initialise the Scan Data Buffer */
	{
		I_GOURAUDPOLYGON_PT *vertexPtr = (I_GOURAUDPOLYGON_PT*)&pheader->Poly1stPt;
		I_GOURAUDPOLYGON_SCAN *polyScan;
		int curVertex;
		int SecondOpinion=0;
			
		NumScans=0;
		/* scan out the right edge */
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		curVertex = minYVertex;
		do
		{
			int height;
			int i,x,y;
			int deltaX,deltaI;

			int nextVertex = curVertex-1;
			if (nextVertex<0) nextVertex = maxVertexNum;
			
			height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
			if (height!=0)
			{
				int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
				int contrast = vertexPtr[nextVertex].i_int - vertexPtr[curVertex].i_int;

				deltaX = (width<<16)/height;
				deltaI = contrast/height;

				x = vertexPtr[curVertex].i_x<<16;
				i = vertexPtr[curVertex].i_int;

				for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
				{
					polyScan->igs_x2 = x>>16;
					x+=deltaX;
					polyScan->igs_c2 = i;
					i+=deltaI;

					polyScan->igs_y = y;
					NumScans++;
					polyScan++;
				}
			}
			curVertex--;
			if (curVertex<0) curVertex = maxVertexNum;

		}
		while(curVertex!=maxYVertex);

		/* scan out the left edge */
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		curVertex = minYVertex;
		do
		{
			int height;
			int i,x,y;
			int deltaX,deltaI;

			int nextVertex = curVertex+1;
			if (nextVertex>maxVertexNum) nextVertex = 0;
			
			height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
			if (height!=0)
			{
				int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
				int contrast = vertexPtr[nextVertex].i_int - vertexPtr[curVertex].i_int;

				deltaX = (width<<16)/height;
				deltaI = contrast/height;

				x = vertexPtr[curVertex].i_x<<16;
				i = vertexPtr[curVertex].i_int;

				for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
				{
					polyScan->igs_x1 = x>>16;
					x+=deltaX;
					polyScan->igs_c1 = i;
					i+=deltaI;
					
					SecondOpinion++;
					polyScan++;
				}
			}
			curVertex++;
			if (curVertex>maxVertexNum) curVertex = 0;

		}
		while(curVertex!=maxYVertex);

		if (SecondOpinion<NumScans)
		{
			textprint("WARNING: Rasterization error\n");
			NumScans = SecondOpinion;
//			LOCALASSERT(0);
  			return;
  		}

	}

	//draw 'em
	
	{
		unsigned char colour= pheader->PolyColour;
		I_GOURAUDPOLYGON_SCAN *polyScan;
		polyScan = (I_GOURAUDPOLYGON_SCAN*)ScanData;
		SCASM_Lighting = TextureLightingTable+colour;
		SCASM_ShadingTableSize = 256;

		while(NumScans)
		{
			int dx = polyScan->igs_x2 - polyScan->igs_x1;
			if (dx>0)
			{
				SCASM_Destination = ScreenBuffer+(polyScan->igs_y * BackBufferPitch) + polyScan->igs_x1;
				SCASM_ScanLength = dx;
				SCASM_DeltaI = (polyScan->igs_c2-polyScan->igs_c1)/dx;
				SCASM_StartI = polyScan->igs_c1;
			   	ScanDraw_GouraudScan();
			}
			NumScans--;
			polyScan++;
		}
	}
}
void KDraw_Item_2dTexturePolygon(int *itemptr)
{
	
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	int TxIndex;
	IMAGEHEADER *ImageHdr;
	ItemColour = pheader->PolyColour;

	TxIndex  = ItemColour & ClrTxDefn;
	ImageHdr = ImageHeaderPtrs[TxIndex];


	/* Colour */
	
	/* If MIP Mapping, calculate the scale */

	if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
	{
		I_2DTEXTUREPOLYGON_PT *vector1;
		I_2DTEXTUREPOLYGON_PT *vector2;
		I_2DTEXTUREPOLYGON_PT *vector3;
		int mip1, mip2;
		int xyd, uvd;
		VECTOR2D s0, s1;
		VECTOR2D t0, t1;


		/* Screen and Texture Space Vectors */
		/* Express the "uvd / xyd" ratio as a binary scale */

		vector1 = (I_2DTEXTUREPOLYGON_PT *) &pheader->Poly1stPt;
		vector2 = &vector1[1];
		vector3 = &vector1[2];


		/* Vector 1 */

		s0.vx = vector1->i_x;
		s0.vy = vector1->i_y;
		s1.vx = vector2->i_x;
		s1.vy = vector2->i_y;

		t0.vx = vector1->i_u >> 16;
		t0.vy = vector1->i_v >> 16;
		t1.vx = vector2->i_u >> 16;
		t1.vy = vector2->i_v >> 16;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip1 = FindShift32(uvd, xyd);


		/* Vector 2 */

		s0.vx = vector2->i_x;
		s0.vy = vector2->i_y;
		s1.vx = vector3->i_x;
		s1.vy = vector3->i_y;

		t0.vx = t1.vx;
		t0.vy = t1.vy;
		t1.vx = vector3->i_u >> 16;
		t1.vy = vector3->i_v >> 16;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip2 = FindShift32(uvd, xyd);


		/* Choose the larger of the two */

		if(pheader->PolyFlags & iflag_no_mip) {

			MIP_Index = 0;

		}

		else {

			#if UseMIPMax
			if(mip1 > mip2)
				MIP_Index = mip1;
			else
				MIP_Index = mip2;
			#endif

			#if UseMIPMin
			if(mip1 > mip2)
				MIP_Index = mip2;
			else
				MIP_Index = mip1;
			#endif

			#if UseMIPAvg
			MIP_Index = (mip1 + mip2) >> 1;
			#endif

		}


		/* Clamp "MIP_Index" */

		#if MIP_INDEX_SUBTRACT

			MIP_Index -= MIP_INDEX_SUBTRACT;
			if(MIP_Index < 0) MIP_Index = 0;
			else if(MIP_Index > 6) MIP_Index = 6;

		#else

			if(MIP_Index > 6) MIP_Index = 6;

		#endif

	}

    {
		int minYVertex = 0;
		int maxYVertex = 0;
		int maxVertexNum = 0;

		{
			I_2DTEXTUREPOLYGON_PT *vertexPtr = (I_2DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			int minY = vertexPtr->i_y;
			int maxY = vertexPtr->i_y;
			
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
			{
				vertexPtr->i_u >>= MIP_Index;
				vertexPtr->i_v >>= MIP_Index;
			}
			vertexPtr++;

			do
			{
				maxVertexNum++;
			
				if (minY > vertexPtr->i_y)
				{
					minY = vertexPtr->i_y;
					minYVertex = maxVertexNum;
				}
				else if (maxY < vertexPtr->i_y)
				{
					maxY = vertexPtr->i_y;
					maxYVertex = maxVertexNum;
				}
				
				if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
				{
					vertexPtr->i_u >>= MIP_Index;
					vertexPtr->i_v >>= MIP_Index;
				}
							
				vertexPtr++;
			}
			while(vertexPtr->i_x != Term);

		}

		
		/* Initialise the Scan Data Buffer */
		{
			I_2DTEXTUREPOLYGON_PT *vertexPtr = (I_2DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			I_2DTEXTUREPOLYGON_SCAN *polyScan;
			int curVertex;
			int SecondOpinion = 0;
				
			NumScans=0;
			/* scan out the right edge */
			polyScan = (I_2DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex-1;
				if (nextVertex<0) nextVertex = maxVertexNum;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int x,y,u,v;
					int deltaX,deltaU,deltaV;
					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int widthU = vertexPtr[nextVertex].i_u - vertexPtr[curVertex].i_u;
					int widthV = vertexPtr[nextVertex].i_v - vertexPtr[curVertex].i_v;

					deltaX = (width<<16)/height;
					deltaU = widthU/height;
					deltaV = widthV/height;

					x = vertexPtr[curVertex].i_x<<16;
					u = vertexPtr[curVertex].i_u;
					v = vertexPtr[curVertex].i_v;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->i2s_x2 = x>>16;
						x+=deltaX;
						polyScan->i2s_u2 = u;
						u+=deltaU;
						polyScan->i2s_v2 = v;
						v+=deltaV;

						polyScan->i2s_y = y;
						NumScans++;
						polyScan++;
					}
				}
				curVertex--;
				if (curVertex<0) curVertex = maxVertexNum;

			}
			while(curVertex!=maxYVertex);
	
			/* scan out the left edge */
			polyScan = (I_2DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex+1;
				if (nextVertex>maxVertexNum) nextVertex = 0;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int x,y,u,v;
					int deltaX,deltaU,deltaV;
					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int widthU = vertexPtr[nextVertex].i_u - vertexPtr[curVertex].i_u;
					int widthV = vertexPtr[nextVertex].i_v - vertexPtr[curVertex].i_v;

					deltaX = (width<<16)/height;
					deltaU = widthU/height;
					deltaV = widthV/height;

					x = vertexPtr[curVertex].i_x<<16;
					u = vertexPtr[curVertex].i_u;
					v = vertexPtr[curVertex].i_v;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->i2s_x1 = x>>16;
						x+=deltaX;
						polyScan->i2s_u1 = u;
						u+=deltaU;
						polyScan->i2s_v1 = v;
						v+=deltaV;

						SecondOpinion++;
						polyScan++;
					}
				}
				curVertex++;
				if (curVertex>maxVertexNum) curVertex = 0;

			}
			while(curVertex!=maxYVertex);

			if (SecondOpinion<NumScans)
			{
				textprint("WARNING: Rasterization error\n");
				NumScans = SecondOpinion;
//				LOCALASSERT(0);
				return;
			}
		}

		//draw 'em
		KR_ScanDraw_Item_2dTexturePolygon_VideoModeType_8(itemptr);
		
	}	
}
void KDraw_Item_Gouraud2dTexturePolygon(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	int TxIndex;
	IMAGEHEADER *ImageHdr;
	ItemColour = pheader->PolyColour;

	TxIndex  = ItemColour & ClrTxDefn;
	ImageHdr = ImageHeaderPtrs[TxIndex];



	/* Colour */


	/* If MIP Mapping, calculate the scale */

	if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
	{
		I_GOURAUD2DTEXTUREPOLYGON_PT *vector1;
		I_GOURAUD2DTEXTUREPOLYGON_PT *vector2;
		I_GOURAUD2DTEXTUREPOLYGON_PT *vector3;
		int mip1, mip2;
		int xyd, uvd;
		VECTOR2D s0, s1;
		VECTOR2D t0, t1;


		/* Screen and Texture Space Vectors */
		/* Express the "uvd / xyd" ratio as a binary scale */

		vector1 = (I_GOURAUD2DTEXTUREPOLYGON_PT *) &pheader->Poly1stPt;
		vector2 = &vector1[1];
		vector3 = &vector1[2];


		/* Vector 1 */

		s0.vx = vector1->i_x;
		s0.vy = vector1->i_y;
		s1.vx = vector2->i_x;
		s1.vy = vector2->i_y;

		t0.vx = vector1->i_u >> 16;
		t0.vy = vector1->i_v >> 16;
		t1.vx = vector2->i_u >> 16;
		t1.vy = vector2->i_v >> 16;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip1 = FindShift32(uvd, xyd);


		/* Vector 2 */

		s0.vx = vector2->i_x;
		s0.vy = vector2->i_y;
		s1.vx = vector3->i_x;
		s1.vy = vector3->i_y;

		t0.vx = t1.vx;
		t0.vy = t1.vy;
		t1.vx = vector3->i_u >> 16;
		t1.vy = vector3->i_v >> 16;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip2 = FindShift32(uvd, xyd);


		/* Choose the larger of the two */

		if(pheader->PolyFlags & iflag_no_mip) {

			MIP_Index = 0;

		}

		else {

			#if UseMIPMax
			if(mip1 > mip2)
				MIP_Index = mip1;
			else
				MIP_Index = mip2;
			#endif

			#if UseMIPMin
			if(mip1 > mip2)
				MIP_Index = mip2;
			else
				MIP_Index = mip1;
			#endif

			#if UseMIPAvg
			MIP_Index = (mip1 + mip2) >> 1;
			#endif

		}


		/* Clamp "MIP_Index" */

		#if MIP_INDEX_SUBTRACT

			MIP_Index -= MIP_INDEX_SUBTRACT;
			if(MIP_Index < 0) MIP_Index = 0;
			else if(MIP_Index > 6) MIP_Index = 6;

		#else

			if(MIP_Index > 6) MIP_Index = 6;

		#endif

	}

    {
		int minYVertex = 0;
		int maxYVertex = 0;
		int maxVertexNum = 0;

		{
			I_GOURAUD2DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD2DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			int minY = vertexPtr->i_y;
			int maxY = vertexPtr->i_y;
			
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
			{
				vertexPtr->i_u >>= MIP_Index;
				vertexPtr->i_v >>= MIP_Index;
			}
			vertexPtr++;

			do
			{
				maxVertexNum++;
			
				if (minY > vertexPtr->i_y)
				{
					minY = vertexPtr->i_y;
					minYVertex = maxVertexNum;
				}
				else if (maxY < vertexPtr->i_y)
				{
					maxY = vertexPtr->i_y;
					maxYVertex = maxVertexNum;
				}
				
				if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
				{
					vertexPtr->i_u >>= MIP_Index;
					vertexPtr->i_v >>= MIP_Index;
				}
							
				vertexPtr++;
			}
			while(vertexPtr->i_x != Term);

		}

		
		/* Initialise the Scan Data Buffer */
		{
			I_GOURAUD2DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD2DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			I_GOURAUD2DTEXTUREPOLYGON_SCAN *polyScan;
			int curVertex;
			int SecondOpinion=0;
				
			NumScans=0;
			/* scan out the right edge */
			polyScan = (I_GOURAUD2DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex-1;
				if (nextVertex<0) nextVertex = maxVertexNum;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y,u,v;
					int deltaX,deltaI,deltaU,deltaV;
					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_i - vertexPtr[curVertex].i_i;
					int widthU = vertexPtr[nextVertex].i_u - vertexPtr[curVertex].i_u;
					int widthV = vertexPtr[nextVertex].i_v - vertexPtr[curVertex].i_v;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_i;
					u = vertexPtr[curVertex].i_u;
					v = vertexPtr[curVertex].i_v;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig2s_x2 = x>>16;
						x+=deltaX;
						polyScan->ig2s_c2 = i;
						i+=deltaI;
						polyScan->ig2s_u2 = u;
						u+=deltaU;
						polyScan->ig2s_v2 = v;
						v+=deltaV;

						polyScan->ig2s_y = y;
						NumScans++;
						polyScan++;
					}
				}
				curVertex--;
				if (curVertex<0) curVertex = maxVertexNum;

			}
			while(curVertex!=maxYVertex);
	
			/* scan out the left edge */
			polyScan = (I_GOURAUD2DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex+1;
				if (nextVertex>maxVertexNum) nextVertex = 0;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y,u,v;
					int deltaX,deltaI,deltaU,deltaV;
					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_i - vertexPtr[curVertex].i_i;
					int widthU = vertexPtr[nextVertex].i_u - vertexPtr[curVertex].i_u;
					int widthV = vertexPtr[nextVertex].i_v - vertexPtr[curVertex].i_v;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_i;
					u = vertexPtr[curVertex].i_u;
					v = vertexPtr[curVertex].i_v;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig2s_x1 = x>>16;
						x+=deltaX;
						polyScan->ig2s_c1 = i;
						i+=deltaI;
						polyScan->ig2s_u1 = u;
						u+=deltaU;
						polyScan->ig2s_v1 = v;
						v+=deltaV;

						SecondOpinion++;
						polyScan++;
					}
				}
				curVertex++;
				if (curVertex>maxVertexNum) curVertex = 0;

			}
			while(curVertex!=maxYVertex);
		
			if (SecondOpinion<NumScans)
			{
				textprint("WARNING: Rasterization error\n");
				NumScans = SecondOpinion;
//				LOCALASSERT(0);
				return;
			}
		}
		//draw 'em
		KR_ScanDraw_Item_Gouraud2dTexturePolygon_VideoModeType_8(itemptr);
		
		#if 0
		{
			unsigned char colour= pheader->PolyColour;
			I_GOURAUD2DTEXTUREPOLYGON_SCAN *polyScan;
			polyScan = (I_GOURAUD2DTEXTUREPOLYGON_SCAN*)ScanData;
			SCASM_Lighting = TextureLightingTable+colour;
			SCASM_ShadingTableSize = 256;

			while(NumScans)
			{
				int dx = polyScan->ig2s_x2 - polyScan->ig2s_x1;
				if (dx>0)
				{
					SCASM_Destination = ScreenBuffer+(polyScan->ig2s_y * BackBufferPitch) + polyScan->ig2s_x1;
					SCASM_ScanLength = dx;
					SCASM_DeltaI = (polyScan->ig2s_c2-polyScan->ig2s_c1)/dx;
					SCASM_StartI = polyScan->ig2s_c1;
				   	ScanDraw_GouraudScan();
				}
				NumScans--;
				polyScan++;
			}
		}
		#endif
	}	
}
#endif
#if 0
void KDraw_Item_Gouraud3dTexturePolygon(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	int TxIndex;
	IMAGEHEADER *ImageHdr;
	ItemColour = pheader->PolyColour;

	TxIndex  = ItemColour & ClrTxDefn;
	ImageHdr = ImageHeaderPtrs[TxIndex];



	/* Colour */


	/* If MIP Mapping, calculate the scale */

	if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
	{
		I_GOURAUD3DTEXTUREPOLYGON_PT *vector1;
		I_GOURAUD3DTEXTUREPOLYGON_PT *vector2;
		I_GOURAUD3DTEXTUREPOLYGON_PT *vector3;
		int mip1, mip2;
		int xyd, uvd;
		VECTOR2D s0, s1;
		VECTOR2D t0, t1;


		/* Screen and Texture Space Vectors */
		/* Express the "uvd / xyd" ratio as a binary scale */

		vector1 = (I_GOURAUD3DTEXTUREPOLYGON_PT *) &pheader->Poly1stPt;
		vector2 = &vector1[1];
		vector3 = &vector1[2];


		/* Vector 1 */

		s0.vx = vector1->i_x;
		s0.vy = vector1->i_y;
		s1.vx = vector2->i_x;
		s1.vy = vector2->i_y;

		t0.vx = vector1->i_gtx3d_u / vector1->i_gtx3d_z;
		t0.vy = vector1->i_gtx3d_v / vector1->i_gtx3d_z;
		t1.vx = vector2->i_gtx3d_u / vector2->i_gtx3d_z;
		t1.vy = vector2->i_gtx3d_v / vector2->i_gtx3d_z;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip1 = FindShift32(uvd, xyd);


		/* Vector 2 */

		s0.vx = vector2->i_x;
		s0.vy = vector2->i_y;
		s1.vx = vector3->i_x;
		s1.vy = vector3->i_y;

		t0.vx = t1.vx;
		t0.vy = t1.vy;
		t1.vx = vector3->i_gtx3d_u / vector3->i_gtx3d_z;
		t1.vy = vector3->i_gtx3d_v / vector3->i_gtx3d_z;

		xyd = FandVD_Distance_2d(&s0, &s1);
		uvd = FandVD_Distance_2d(&t0, &t1);

		mip2 = FindShift32(uvd, xyd);


		/* Choose the larger of the two */

		if(pheader->PolyFlags & iflag_no_mip) {

			MIP_Index = 0;

		}

		else {

			#if UseMIPMax
			if(mip1 > mip2)
				MIP_Index = mip1;
			else
				MIP_Index = mip2;
			#endif

			#if UseMIPMin
			if(mip1 > mip2)
				MIP_Index = mip2;
			else
				MIP_Index = mip1;
			#endif

			#if UseMIPAvg
			MIP_Index = (mip1 + mip2) >> 1;
			#endif

		}


		/* Clamp "MIP_Index" */

		#if MIP_INDEX_SUBTRACT

			MIP_Index -= MIP_INDEX_SUBTRACT;
			if(MIP_Index < 0) MIP_Index = 0;
			else if(MIP_Index > 6) MIP_Index = 6;

		#else

			if(MIP_Index > 6) MIP_Index = 6;

		#endif

	}

    {
		int minYVertex = 0;
		int maxYVertex = 0;
		int maxVertexNum = 0;
		float MIP_Divide = 1<<MIP_Index;

		{
			I_GOURAUD3DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD3DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			int minY = vertexPtr->i_y;
			int maxY = vertexPtr->i_y;
			
			if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
			{
				vertexPtr->i_gtx3d_u /= MIP_Divide;
				vertexPtr->i_gtx3d_v /= MIP_Divide;
			}
			vertexPtr++;

			do
			{
				maxVertexNum++;
			
				if (minY > vertexPtr->i_y)
				{
					minY = vertexPtr->i_y;
					minYVertex = maxVertexNum;
				}
				else if (maxY < vertexPtr->i_y)
				{
					maxY = vertexPtr->i_y;
					maxYVertex = maxVertexNum;
				}
				
				if(ScreenDescriptorBlock.SDB_Flags & SDB_Flag_MIP && ImageHdr->ImageFlags & ih_flag_mip)
				{
					vertexPtr->i_gtx3d_u /= MIP_Divide;
					vertexPtr->i_gtx3d_v /= MIP_Divide;
				}
							
				vertexPtr++;
			}
			while(vertexPtr->i_x != Term);

		}

		
		/* Initialise the Scan Data Buffer */
		{
			I_GOURAUD3DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD3DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			I_GOURAUD3DTEXTUREPOLYGON_SCAN *polyScan;
			int curVertex;
			int SecondOpinion=0;
				
			NumScans=0;
			
			/* scan out the right edge */
			polyScan = (I_GOURAUD3DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex-1;
				if (nextVertex<0) nextVertex = maxVertexNum;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y;
					float u,v,z;
					int deltaX,deltaI;
					float deltaU,deltaV,deltaZ;

					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_gtx3d_i - vertexPtr[curVertex].i_gtx3d_i;
					float widthU = vertexPtr[nextVertex].i_gtx3d_u - vertexPtr[curVertex].i_gtx3d_u;
					float widthV = vertexPtr[nextVertex].i_gtx3d_v - vertexPtr[curVertex].i_gtx3d_v;
					float widthZ = vertexPtr[nextVertex].i_gtx3d_z - vertexPtr[curVertex].i_gtx3d_z;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;
					deltaZ = widthZ/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_gtx3d_i;
					u = vertexPtr[curVertex].i_gtx3d_u;
					v = vertexPtr[curVertex].i_gtx3d_v;
					z = vertexPtr[curVertex].i_gtx3d_z;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig3s_x2 = x>>16;
						x+=deltaX;
						polyScan->ig3s_c2 = i;
						i+=deltaI;
						polyScan->ig3s_u2 = u;
						u+=deltaU;
						polyScan->ig3s_v2 = v;
						v+=deltaV;
						polyScan->ig3s_z2 = z;
						z+=deltaZ;

						polyScan->ig3s_y = y;
						NumScans++;
						polyScan++;
					}
				}
				curVertex--;
				if (curVertex<0) curVertex = maxVertexNum;

			}
			while(curVertex!=maxYVertex);
	
			/* scan out the left edge */
			polyScan = (I_GOURAUD3DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex+1;
				if (nextVertex>maxVertexNum) nextVertex = 0;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y;
					float u,v,z;
					int deltaX,deltaI;
					float deltaU,deltaV,deltaZ;

					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_gtx3d_i - vertexPtr[curVertex].i_gtx3d_i;
					float widthU = vertexPtr[nextVertex].i_gtx3d_u - vertexPtr[curVertex].i_gtx3d_u;
					float widthV = vertexPtr[nextVertex].i_gtx3d_v - vertexPtr[curVertex].i_gtx3d_v;
					float widthZ = vertexPtr[nextVertex].i_gtx3d_z - vertexPtr[curVertex].i_gtx3d_z;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;
					deltaZ = widthZ/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_gtx3d_i;
					u = vertexPtr[curVertex].i_gtx3d_u;
					v = vertexPtr[curVertex].i_gtx3d_v;
					z = vertexPtr[curVertex].i_gtx3d_z;

				 	for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig3s_x1 = x>>16;
						x+=deltaX;
						polyScan->ig3s_c1 = i;
						i+=deltaI;
						polyScan->ig3s_u1 = u;
						u+=deltaU;
						polyScan->ig3s_v1 = v;
						v+=deltaV;
						polyScan->ig3s_z1 = z;
						z+=deltaZ;

						SecondOpinion++;
						polyScan++;
					}
				}
				curVertex++;
				if (curVertex>maxVertexNum) curVertex = 0;
			
			}
			while(curVertex!=maxYVertex);

			if (SecondOpinion<NumScans)
			{
				textprint("WARNING: Rasterization error\n");
				NumScans = SecondOpinion;
//				LOCALASSERT(0);
				return;
			}
		}

		//draw 'em
  		KR_ScanDraw_Item_Gouraud3dTexturePolygon_Linear_S_VideoModeType_8(itemptr);
	}
}
#else
void KDraw_Item_Gouraud3dTexturePolygon(int *itemptr)
{
	POLYHEADER *pheader = (POLYHEADER *) itemptr;
	int TxIndex;
	IMAGEHEADER *ImageHdr;
	ItemColour = pheader->PolyColour;

	TxIndex  = ItemColour & ClrTxDefn;
	ImageHdr = ImageHeaderPtrs[TxIndex];


  
    {
		int minYVertex = 0;
		int maxYVertex = 0;
		int maxVertexNum = 0;
  

		{
			I_GOURAUD3DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD3DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			int minY = vertexPtr->i_y;
			int maxY = vertexPtr->i_y;
			
			vertexPtr++;

			do
			{
				maxVertexNum++;
			
				if (minY > vertexPtr->i_y)
				{
					minY = vertexPtr->i_y;
					minYVertex = maxVertexNum;
				}
				else if (maxY < vertexPtr->i_y)
				{
					maxY = vertexPtr->i_y;
					maxYVertex = maxVertexNum;
				}
							
				vertexPtr++;
			}
			while(vertexPtr->i_x != Term);

		}

		
		/* Initialise the Scan Data Buffer */
		{
			I_GOURAUD3DTEXTUREPOLYGON_PT *vertexPtr = (I_GOURAUD3DTEXTUREPOLYGON_PT*)&pheader->Poly1stPt;
			I_GOURAUD3DTEXTUREPOLYGON_SCAN *polyScan;
			int curVertex;
			int SecondOpinion=0;
				
			NumScans=0;
			
			/* scan out the right edge */
			polyScan = (I_GOURAUD3DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex-1;
				if (nextVertex<0) nextVertex = maxVertexNum;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y;
					float u,v,z;
					int deltaX,deltaI;
					float deltaU,deltaV,deltaZ;

					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_gtx3d_i - vertexPtr[curVertex].i_gtx3d_i;
					float widthU = vertexPtr[nextVertex].i_gtx3d_u - vertexPtr[curVertex].i_gtx3d_u;
					float widthV = vertexPtr[nextVertex].i_gtx3d_v - vertexPtr[curVertex].i_gtx3d_v;
					float widthZ = vertexPtr[nextVertex].i_gtx3d_z - vertexPtr[curVertex].i_gtx3d_z;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;
					deltaZ = widthZ/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_gtx3d_i;
					u = vertexPtr[curVertex].i_gtx3d_u;
					v = vertexPtr[curVertex].i_gtx3d_v;
					z = vertexPtr[curVertex].i_gtx3d_z;

					for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig3s_x2 = x>>16;
						x+=deltaX;
						polyScan->ig3s_c2 = i;
						i+=deltaI;
						polyScan->ig3s_u2 = u;
						u+=deltaU;
						polyScan->ig3s_v2 = v;
						v+=deltaV;
						polyScan->ig3s_z2 = z;
						z+=deltaZ;

						polyScan->ig3s_y = y;
						NumScans++;
						polyScan++;
					}
				}
				curVertex--;
				if (curVertex<0) curVertex = maxVertexNum;

			}
			while(curVertex!=maxYVertex);
	
			/* scan out the left edge */
			polyScan = (I_GOURAUD3DTEXTUREPOLYGON_SCAN*)ScanData;
			curVertex = minYVertex;
			do
			{
				int height;

				int nextVertex = curVertex+1;
				if (nextVertex>maxVertexNum) nextVertex = 0;
				
				height = vertexPtr[nextVertex].i_y - vertexPtr[curVertex].i_y;
				if (height!=0)
				{
					int i,x,y;
					float u,v,z;
					int deltaX,deltaI;
					float deltaU,deltaV,deltaZ;

					int width = vertexPtr[nextVertex].i_x - vertexPtr[curVertex].i_x;
					int contrast = vertexPtr[nextVertex].i_gtx3d_i - vertexPtr[curVertex].i_gtx3d_i;
					float widthU = vertexPtr[nextVertex].i_gtx3d_u - vertexPtr[curVertex].i_gtx3d_u;
					float widthV = vertexPtr[nextVertex].i_gtx3d_v - vertexPtr[curVertex].i_gtx3d_v;
					float widthZ = vertexPtr[nextVertex].i_gtx3d_z - vertexPtr[curVertex].i_gtx3d_z;

					deltaX = (width<<16)/height;
					deltaI = contrast/height;
					deltaU = widthU/height;
					deltaV = widthV/height;
					deltaZ = widthZ/height;

					x = vertexPtr[curVertex].i_x<<16;
					i = vertexPtr[curVertex].i_gtx3d_i;
					u = vertexPtr[curVertex].i_gtx3d_u;
					v = vertexPtr[curVertex].i_gtx3d_v;
					z = vertexPtr[curVertex].i_gtx3d_z;

				 	for (y=vertexPtr[curVertex].i_y; y<vertexPtr[nextVertex].i_y; y++)
					{
						polyScan->ig3s_x1 = x>>16;
						x+=deltaX;
						polyScan->ig3s_c1 = i;
						i+=deltaI;
						polyScan->ig3s_u1 = u;
						u+=deltaU;
						polyScan->ig3s_v1 = v;
						v+=deltaV;
						polyScan->ig3s_z1 = z;
						z+=deltaZ;

						SecondOpinion++;
						polyScan++;
					}
				}
				curVertex++;
				if (curVertex>maxVertexNum) curVertex = 0;
			
			}
			while(curVertex!=maxYVertex);

			if (SecondOpinion<NumScans)
			{
				textprint("WARNING: Rasterization error\n");
				NumScans = SecondOpinion;
//				LOCALASSERT(0);
				return;
			}
		}

		//draw 'em
  		Draw_Gouraud3dTexture_Spans(itemptr);
	}
}

#define PERS_STEP 16
void Draw_Gouraud3dTexture_Spans(int *itemptr)
{
	I_GOURAUD3DTEXTUREPOLYGON_SCAN *sptr;

	if(NumScans)
	{
		int y = NumScans;
		/* Get the Image Data required for the Draw */
		{
			IMAGEHEADER *ImageHdr;
			{
				int TxIndex  = ItemColour & ClrTxDefn;
				ImageHdr = ImageHeaderPtrs[TxIndex];
			}
			{
				SCASM_TextureDeltaScan = ImageHdr->ImageWidth;
				SCASM_Bitmap = ImageHdr->ImagePtr;
				GLOBALASSERT(SCASM_Bitmap);
			}
		}
		SCASM_Lighting = TextureLightingTable;
		
		sptr = (I_GOURAUD3DTEXTUREPOLYGON_SCAN *) ScanData;
		do
		{
			int length = sptr->ig3s_x2 - sptr->ig3s_x1;
			int dx=length;

			if (dx>0 && !(sptr->ig3s_y & KRenderDrawMode) )
			{
				float deltaZ;
				float endZ;
				
				float endU,endV;
				float deltaU,deltaV;

				{
					int StartI = sptr->ig3s_c1;
					SCASM_DeltaI = (sptr->ig3s_c2 - StartI)/dx;
					SCASM_StartI = StartI;
				}

				SCASM_Destination = ScreenBuffer + (sptr->ig3s_y * BackBufferPitch) + sptr->ig3s_x1;
				SCASM_ScanLength = PERS_STEP;
				
				{
					float oneOverdx = 1.0/dx;

					endZ = sptr->ig3s_z1;
					deltaZ = (sptr->ig3s_z2 - endZ)*oneOverdx;

					endU = sptr->ig3s_u1;
					deltaU = (sptr->ig3s_u2 - endU)*oneOverdx;

					endV = sptr->ig3s_v1;
					deltaV = (sptr->ig3s_v2 - endV)*oneOverdx;
				}
				{
					float z = 65536.0/endZ;
					SCASM_StartU = endU*z;
					SCASM_StartV = endV*z;
				}
				while(dx>PERS_STEP)
				{
					/* subdivide! */
					int u,v;

					dx -= PERS_STEP;

					endZ += PERS_STEP*deltaZ;
					endU += PERS_STEP*deltaU;
					endV += PERS_STEP*deltaV;

					{
						float z = 65536.0/endZ;
						u = endU*z;
						v = endV*z;
					}
					SCASM_DeltaU = (u-SCASM_StartU)/PERS_STEP;
					SCASM_DeltaV = (v-SCASM_StartV)/PERS_STEP;
					
					/* draw PERS_STEP pixels */
					ScanDraw2D_Gouraud();
				
					SCASM_StartU = u;
					SCASM_StartV = v;
					
					SCASM_Destination +=PERS_STEP;
					SCASM_StartI += PERS_STEP*SCASM_DeltaI;
				}
				if (dx>0)
				{
					int u,v;
					SCASM_ScanLength = dx;
					{
						float z = 65536.0/sptr->ig3s_z2;
						u = sptr->ig3s_u2*z;
						v = sptr->ig3s_v2*z;
					}
					SCASM_DeltaU = (u-SCASM_StartU)/dx;
					SCASM_DeltaV = (v-SCASM_StartV)/dx;

					/* draw 8 pixels */
					ScanDraw2D_Gouraud();
				}



				
				
			}
			sptr++;
			y--;
		}
		while(y);
	}
	
}
#endif











#if 0
void MakeInverseLightingTable(void)
{
	int lookingForColour;

	for(lookingForColour=1; lookingForColour<=255; lookingForColour++)
	{
		int exit =0;
		int table;
		for (table=128; (table>0 && (!exit)); table--)
		{
			int entry;
			for(entry=1; (entry<=255 && (!exit)); entry++)
			{
				if(lookingForColour == *(TextureLightingTable + 256*(table) + entry))
				{
				LighterTable[lookingForColour] = entry;
				exit=1; 
				}
			}
		}
		if (exit==0) LighterTable[lookingForColour] = 255;
	}
}
#endif
struct ColourVector
{
	VECTORCH Direction;
	int	Magnitude;
};

struct ColourVector ColourTable[256];

void MakeInverseLightingTable(void)
{
	extern unsigned char TestPalette[];
	unsigned char *palPtr = TestPalette;
	int i;
	
	for(i = 0; i < 256; i++)
	{
		VECTORCH colour;
		int mag;

		colour.vx = *palPtr++;
		colour.vy = *palPtr++;
		colour.vz = *palPtr++;
		mag = Magnitude(&colour);
		
		if (mag!=0)
		{		
			colour.vx = (colour.vx*32)/mag;
			colour.vy = (colour.vy*32)/mag;
			colour.vz = (colour.vz*32)/mag;
		}
		ColourTable[i].Magnitude = mag;
		ColourTable[i].Direction = colour;
	}

	for(i = 0; i < 256; i++)
	{
		int entry;
		int brightest=0;

		for(entry = 0; entry < 256; entry++)
		{
			VECTORCH v1 = ColourTable[i].Direction;
			VECTORCH v2 = ColourTable[entry].Direction;
	
			if ((v1.vx == v2.vx)
			  &&(v1.vy == v2.vy)
			  &&(v1.vz == v2.vz)
			  &&(ColourTable[i].Magnitude < ColourTable[entry].Magnitude)
			  &&(ColourTable[brightest].Magnitude > ColourTable[entry].Magnitude ||(!brightest)))
				brightest = entry;
		}
		if (brightest==0)
		{
			for(entry = 0; entry < 256; entry++)
			{
				VECTORCH v1 = ColourTable[i].Direction;
				VECTORCH v2 = ColourTable[entry].Direction;
		
				if ((v1.vx>>2 == v2.vx>>1)
				  &&(v1.vy>>2 == v2.vy>>1)
				  &&(v1.vz>>2 == v2.vz>>1)
				  &&(ColourTable[i].Magnitude < ColourTable[entry].Magnitude)
				  &&(ColourTable[brightest].Magnitude > ColourTable[entry].Magnitude ||(!brightest)))

					brightest = entry;
			}
		}

		if (brightest==0)
		{
			for(entry = 0; entry < 256; entry++)
			{
				VECTORCH v1 = ColourTable[i].Direction;
				VECTORCH v2 = ColourTable[entry].Direction;
		
				if ((v1.vx>>2 == v2.vx>>2)
				  &&(v1.vy>>2 == v2.vy>>2)
				  &&(v1.vz>>2 == v2.vz>>2)
				  &&(ColourTable[i].Magnitude < ColourTable[entry].Magnitude)
			      &&(ColourTable[brightest].Magnitude > ColourTable[entry].Magnitude ||(!brightest)))
					brightest = entry;
			}
		}
		#if 0
		if (brightest==0)
		{
			for(entry = 0; entry < 256; entry++)
			{
				VECTORCH v1 = ColourTable[i].Direction;
				VECTORCH v2 = ColourTable[entry].Direction;
		
				if ((v1.vx>>3 == v2.vx>>3)
				  &&(v1.vy>>3 == v2.vy>>3)
				  &&(v1.vz>>3 == v2.vz>>3)
				  &&(ColourTable[i].Magnitude < ColourTable[entry].Magnitude)
			  	  &&(ColourTable[brightest].Magnitude > ColourTable[entry].Magnitude ||(!brightest)))
					brightest = entry;
			}
		}

		#endif
		if (brightest==0) brightest = i;
		
		LighterTable[i] = brightest;
	}
	

}


void DrawPaletteScreen(void)
{
	int sortedColours[256];
	{
		extern unsigned char TestPalette[];
		unsigned char *palPtr = TestPalette;
		int i;

		for(i = 0; i < 256; i++)
		{
			VECTORCH colour;
			int mag;

			colour.vx = *palPtr++;
			colour.vy = *palPtr++;
			colour.vz = *palPtr++;
			mag = Magnitude(&colour);
			
			if (mag!=0)
			{		
				colour.vx = (colour.vx*7)/mag;
				colour.vy = (colour.vy*7)/mag;
				colour.vz = (colour.vz*7)/mag;
			}
			ColourTable[i].Magnitude = mag;
			ColourTable[i].Direction = colour;
		}
		
		for(i = 0; i<256; i++)
		{
			int e;
			int maxKey=-1;
			int selectedEntry=0;

			for (e=0; e<256; e++)
			{
				int key = ColourTable[e].Direction.vx + ColourTable[e].Direction.vy*64+ColourTable[e].Direction.vz*64*64;
				if (key>maxKey)
				{
					maxKey = key;
					selectedEntry = e;
				}
				else if (key==maxKey)
				{
					if (ColourTable[e].Magnitude<ColourTable[selectedEntry].Magnitude)
					selectedEntry = e;
				}
			}

			sortedColours[i] = selectedEntry;
			ColourTable[selectedEntry].Direction.vx=-1;
			ColourTable[selectedEntry].Direction.vy=-1;
			ColourTable[selectedEntry].Direction.vz=-1;
		}

	}
	{
		char colour = 0;
		int *bufferPtr = (int*)ScreenBuffer;
		int i,x,y;

		for (i=0; i<=12; i++)
		{
			for (y=0; y<=6; y++)
			{
				for (x=0; x<=19; x++)
				{
					unsigned int c=colour+x;

					if (c<256)
					{
						c = sortedColours[c];
						c = c + (c<<8) + (c<<16) +(c<<24);
						*bufferPtr++=c;	
						*bufferPtr++=c;	
						*bufferPtr++=c;
						*bufferPtr++=c;
					}
					else
					{
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
					}
				}
			}
			colour+=20;
		}
	}

	#if 0
	{
		extern unsigned char TestPalette[];
		unsigned char *palPtr = TestPalette;
		int i;

		for(i = 0; i < 256; i++)
		{
			VECTORCH colour;
			int mag;

			colour.vx = *palPtr++;
			colour.vy = *palPtr++;
			colour.vz = *palPtr++;
			mag = Magnitude(&colour);
			if (mag!=0)
			{		
				colour.vx = (colour.vx*63)/mag;
				colour.vy = (colour.vy*63)/mag;
				colour.vz = (colour.vz*63)/mag;
			}
			
			ColourTable[i].Direction = colour;
			ColourTable[i].Magnitude = mag;
		}
		
		for(i = 0; i<256; i++)
		{
			int e;
			int maxKey=-1;
			int selectedEntry=0;

			for (e=0; e<256; e++)
			{
				int key = ColourTable[e].Magnitude;
				
				if (key>maxKey)
				{
					maxKey = key;
					selectedEntry = e;
				}
				else if (key==maxKey)
				{
					int key2 = ColourTable[e].Direction.vx + ColourTable[e].Direction.vy*64+ColourTable[e].Direction.vz*64*64;
					int key3 = ColourTable[selectedEntry].Direction.vx
							 + ColourTable[selectedEntry].Direction.vy*64
							 + ColourTable[selectedEntry].Direction.vz*64*64;

					if (key2<key3)
						selectedEntry=e;
				}

			}

			sortedColours[i] = selectedEntry;
			ColourTable[selectedEntry].Magnitude=-1;
		}

	}
	{
		char colour = 0;
		int *bufferPtr = (int*)(ScreenBuffer+32000);
		int i,x,y;

		for (i=0; i<=12; i++)
		{
			for (y=0; y<=6; y++)
			{
				for (x=0; x<=19; x++)
				{
					unsigned int c=colour+x;

					if (c<256)
					{
						c = sortedColours[c];
						c = c + (c<<8) + (c<<16) +(c<<24);
						*bufferPtr++=c;	
						*bufferPtr++=c;	
						*bufferPtr++=c;
						*bufferPtr++=c;
					}
					else
					{
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
					}
				}
			}
			colour+=20;
		}
	}
	#else
	{
		extern unsigned char TestPalette[];
		char colour = 0;
		int *bufferPtr = (int*)(ScreenBuffer+32000);
		int i,x,y;

		for (i=0; i<=12; i++)
		{
			for (y=0; y<=6; y++)
			{
				for (x=0; x<=19; x++)
				{
					unsigned int c=colour+x;

					if (c<256)
					{
						c = c + (c<<8) + (c<<16) +(c<<24);
						*bufferPtr++=c;	
						*bufferPtr++=c;	
						*bufferPtr++=c;
						*bufferPtr++=c;
					}
					else
					{
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
						bufferPtr++;
					}
				}
			}
			colour+=20;
		}
	}
	#endif
}

void OddLineScreenCopy(unsigned int *source,unsigned int *dest)
{
	int lines = 240;

	do
	{
		int i = 640/4;
		
		dest += 640/4;
		source += 640/4;

		do
		{
			*dest++=*source++;
		}
		while(--i);
	}
	while(--lines);
		
}



/*KJL*****************************************************
* Palette fading; a value of 65536 corresponds to normal *
* palette, 0 is completely black.                        *
*****************************************************KJL*/
void SetPaletteFadeLevel(int fadeLevel)
{
	extern int ScanDrawMode;
	if(ScanDrawDirectDraw == ScanDrawMode)
	{
		extern unsigned char TestPalette[];
		extern unsigned char TestPalette2[];
		{
			int x;
			for (x=0; x<768; x++)
			{
				TestPalette2[x] = ((unsigned int)TestPalette[x]*fadeLevel)>>16;
			}
		}
		ChangePalette(TestPalette2);
	}
	else
	{
		d3d_light_ctrl.ctrl = LCCM_CONSTCOLOUR;
		d3d_light_ctrl.r = fadeLevel;
		d3d_light_ctrl.g = fadeLevel;
		d3d_light_ctrl.b = fadeLevel;
	}
}

void FadeBetweenPalettes(unsigned char *palPtr, int fadeLevel)
{
	extern unsigned char TestPalette[];
	unsigned char TestPalette3[768];
	{
		int x;
		for (x=0; x<768; x++)
		{
			TestPalette3[x] = ( (unsigned int)TestPalette[x]*fadeLevel + (unsigned int)(palPtr[x])*(65536-fadeLevel) )>>16;
		}
	}
	ChangePalette(TestPalette3);
}
void FadePaletteToWhite(unsigned char *palPtr,int fadeLevel)
{
	unsigned char TestPalette3[768];
	{
		int x;
		for (x=0; x<768; x++)
		{
			TestPalette3[x] = ( (unsigned int)(palPtr[x])*fadeLevel + 63*(65536-fadeLevel) )>>16;
		}
	}
	ChangePalette(TestPalette3);
}


void BlankScreen(void)
{
	extern int ScanDrawMode;
	
	if (ScanDrawDirectDraw==ScanDrawMode)
	{
		extern unsigned char *ScreenBuffer;
		extern unsigned char TestPalette[];
		unsigned int *screenPtr;
		int i;							   
		
		screenPtr = (unsigned int *)ScreenBuffer;
		i = ScreenDescriptorBlock.SDB_Width * ScreenDescriptorBlock.SDB_Height /4;
		do
		{
			*screenPtr++=0;	
		}
		while(--i);
	}
	else
	{
		ColourFillBackBuffer(0);
	}

 //	FlipBuffers();
}




void GenerateReciprocalTable(void)
{
	int i=320;
	
	do
	{
		ReciprocalTable[i] = 65536/i;
	}
	while(--i);
}




#if 0
#define NO_OF_STARS 500
typedef struct
{
	VECTORCH Position;
	int Colour;

} STARDESC;
static STARDESC StarArray[NO_OF_STARS];
#endif
void CreateStarArray(void)
{
	#if 0
	int i;
	extern int sine[],cosine[];
	for(i=0; i<NO_OF_STARS; i++)
	{
		int phi = FastRandom()&4095;
		int theta = FastRandom()&4095;

		StarArray[i].Position.vx = MUL_FIXED(GetCos(phi),GetSin(theta));
		StarArray[i].Position.vy = MUL_FIXED(GetSin(phi),GetSin(theta));
		StarArray[i].Position.vz = GetCos(theta);
		StarArray[i].Colour = FastRandom()&32767;
	}
	#endif
}

/* KJL 12:10:36 9/30/97 - starfield, currently implemented for 8 & 16 bit displays */
void DrawStarfilledSky(void)
{
	#if 0
	int i;
	extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
	extern int ScanDrawMode;
	
	/* blank the screen */
	ColourFillBackBuffer(0);

	LockSurfaceAndGetBufferPointer();
	
	if(AvP.PlayerType==I_Alien)	/* wide frustrum */
	{
		for(i=0; i<NO_OF_STARS; i++)
		{
			VECTORCH rotatedPosition = StarArray[i].Position;
			
			/* rotate star into view space */
			RotateVector(&rotatedPosition,&(Global_VDB_Ptr->VDB_Mat));
			
			/* is star within alien (wide) view frustrum ? */
			if((-rotatedPosition.vx <= rotatedPosition.vz*2)
			&&(rotatedPosition.vx <= rotatedPosition.vz*2)
			&&(-rotatedPosition.vy*2 <= rotatedPosition.vz*3)
			&&(rotatedPosition.vy*2 <= rotatedPosition.vz*3))
			{
				/* project into screen space */
				int y = (rotatedPosition.vy*(Global_VDB_Ptr->VDB_ProjY))/rotatedPosition.vz+Global_VDB_Ptr->VDB_CentreY;
				int x = (rotatedPosition.vx*(Global_VDB_Ptr->VDB_ProjX))/rotatedPosition.vz+Global_VDB_Ptr->VDB_CentreX;
				
				/* draw pixel of required bit depth */
				if (ScanDrawMode == ScanDrawDirectDraw)
				{
					/* 8 bit mode */
					*(ScreenBuffer + x + y*BackBufferPitch) = StarArray[i].Colour&255;
				}
				else
				{
					/* 16 bit mode */
					*(unsigned short*)(ScreenBuffer + (x*2 + y*BackBufferPitch)) = StarArray[i].Colour;
				}
			}
		}
	}
	else /* normal frustrum */
	{
		for(i=0; i<NO_OF_STARS; i++)
		{
			VECTORCH rotatedPosition = StarArray[i].Position;
			
			/* rotate star into view space */
			RotateVector(&rotatedPosition,&(Global_VDB_Ptr->VDB_Mat));

			/* is star within normal view frustrum ? */
			if((-rotatedPosition.vx <= rotatedPosition.vz)
			&&(rotatedPosition.vx <= rotatedPosition.vz)
			&&(-rotatedPosition.vy*4 <= rotatedPosition.vz*3)
			&&(rotatedPosition.vy*4 <= rotatedPosition.vz*3))
			{
				/* project into screen space */
				int y = (rotatedPosition.vy*(Global_VDB_Ptr->VDB_ProjY))/rotatedPosition.vz+Global_VDB_Ptr->VDB_CentreY;
				int x = (rotatedPosition.vx*(Global_VDB_Ptr->VDB_ProjX))/rotatedPosition.vz+Global_VDB_Ptr->VDB_CentreX;
				
				/* draw pixel of required bit depth */
				if (ScanDrawMode == ScanDrawDirectDraw)
				{
					/* 8 bit mode */
					*(ScreenBuffer + x + y*BackBufferPitch) = StarArray[i].Colour&255;
				}
				else
				{
					/* 16 bit mode */
					*(unsigned short*)(ScreenBuffer + (x*2 + y*BackBufferPitch)) = StarArray[i].Colour;
				}
			}
		}
	}
	
	UnlockSurface();
	#endif
}