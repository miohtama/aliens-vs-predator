/*******************************************************************
 *
 *    DESCRIPTION: 	r2base.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 14/11/97
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"
#include "r2base.h"
#include "inline.h"

	
	#define UseLocalAssert Yes
	#include "ourasert.h"

/* Version settings ************************************************/

/* Constants *******************************************************/

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif

		extern int D3DDriverMode;

		#if 0
		extern OurBool			DaveDebugOn;
		extern FDIEXTENSIONTAG	FDIET_Dummy;
		extern IFEXTENSIONTAG	IFET_Dummy;
		extern FDIQUAD			FDIQuad_WholeScreen;
		extern FDIPOS			FDIPos_Origin;
		extern FDIPOS			FDIPos_ScreenCentre;
		extern IFOBJECTLOCATION IFObjLoc_Origin;
		extern UncompressedGlobalPlotAtomID UGPAID_StandardNull;
		extern IFCOLOUR			IFColour_Dummy;
 		extern IFVECTOR			IFVec_Zero;
		#endif
#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/
	/*static*/ const r2pos r2pos :: Origin = r2pos(0,0);
	/*static*/ r2rect r2rect :: R2Rect_PhysicalScreen = r2rect(0,0,640,480);

/* Internal type definitions ***************************************/

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
r2pos r2pos :: FixP_Scale
(
	int FixP_ScaleFactor
) const
{
	// assumes the position to be in 16:16 fixed point,
	// returns the position scaled by the fixed pt factor

	return r2pos
	(
		MUL_FIXED(x, FixP_ScaleFactor),
		MUL_FIXED(y, FixP_ScaleFactor)
	);
}


// Repository for code that ought to be in D3_FUNC.CPP
#if 0
void r2rect :: AlphaFill
(
	unsigned char R,
	unsigned char G,
	unsigned char B,
	unsigned char translucency
) const
{
	GLOBALASSERT
	(
		bValidPhys()
	);

	#if 0
	float RecipW, RecipH;

    // Check for textures that have not loaded properly
    LOCALASSERT(TextureHandle != (D3DTEXTUREHANDLE) 0);
	
	if(ImageHeaderArray[imageNumber].ImageWidth==128)
	{
		RecipW = 1.0 /128.0;
	}
	else
	{
		float width = (float) ImageHeaderArray[imageNumber].ImageWidth;
		RecipW = (1.0 / width);
	}
	if(ImageHeaderArray[imageNumber].ImageHeight==128)
	{
		RecipH = 1.0 / 128.0;
	}
	else
	{
		float height = (float) ImageHeaderArray[imageNumber].ImageHeight;
		RecipH = (1.0 / height);
	}
	#endif


	/* OUTPUT quadVerticesPtr TO EXECUTE BUFFER */
	{
		D3DCOLOR Colour;

		switch (D3DDriverMode)
		{
			case D3DSoftwareRGBDriver:
			{
			  	Colour = RGBLIGHT_MAKE(R,G,B);
				break;
			}

			case D3DSoftwareRampDriver:
			{
				Colour = RGB_MAKE(0,0,B);
				break;
			}
			
			default:
			case D3DHardwareRGBDriver:
			{
		  		Colour = RGBALIGHT_MAKE(R,G,B,translucency);
				break;
			}
		}

		#if 1
		{
			D3DTLVERTEX *vertexPtr = &((LPD3DTLVERTEX)ExecuteBufferDataArea)[NumVertices];

			/* Vertex 0 = Top left */
			vertexPtr->sx= x0;
			vertexPtr->sy= y0;
			vertexPtr->color = Colour;
			
			NumVertices++;
			vertexPtr++;

			/* Vertex 1 = Top right */
			vertexPtr->sx=( x1 - 1);
			vertexPtr->sy=( y0 );
			vertexPtr->color = Colour;
			
			NumVertices++;
			vertexPtr++;

			/* Vertex 2 = Bottom right */
			vertexPtr->sx=( x1 - 1);
			vertexPtr->sy=( y1 - 1);
			vertexPtr->color = Colour;
			
			NumVertices++;
			vertexPtr++;

			/* Vertex 3 = Bottom left */
			vertexPtr->sx=x0;
			vertexPtr->sy=( y1 - 1);
			vertexPtr->color = Colour;
			
			NumVertices++;
		}
		#else
		int i = 4;
		do
		{
			D3DTLVERTEX *vertexPtr = &((LPD3DTLVERTEX)ExecuteBufferDataArea)[NumVertices];
			
//			textprint("x %d, y %d, u %d, v %d\n",quadVerticesPtr->X,quadVerticesPtr->Y,quadVerticesPtr->U,quadVerticesPtr->V);
			vertexPtr->sx=quadVerticesPtr->X;
			vertexPtr->sy=quadVerticesPtr->Y;
			#if 0
			vertexPtr->tu = ((float)(quadVerticesPtr->U)) * RecipW;
			vertexPtr->tv = ((float)(quadVerticesPtr->V)) * RecipH;
			#endif

			vertexPtr->color = Colour;
			}
			quadVerticesPtr++;
			NumVertices++;
		}
	  	while(--i);
		#endif
	}

	// set correct texture handle
    if (0 != CurrTextureHandle)
	{
       OP_STATE_RENDER(1, ExecBufInstPtr);
       STATE_DATA(D3DRENDERSTATE_TEXTUREHANDLE, 0, ExecBufInstPtr);
	   CurrTextureHandle = 0;
	}

	/* output triangles to execute buffer */
	OP_TRIANGLE_LIST(2, ExecBufInstPtr);
	OUTPUT_TRIANGLE(0,1,3, 4);
	OUTPUT_TRIANGLE(1,2,3, 4);
	
	/* check to see if buffer is getting full */
	if (NumVertices > (MaxD3DVertices-12)) 
	{
	   WriteEndCodeToExecuteBuffer();
  	   UnlockExecuteBufferAndPrepareForUse();
	   ExecuteBuffer();
  	   LockExecuteBuffer();
	}
}
#endif

r2pos operator+ ( const r2pos& R2Pos_1, const r2pos& R2Pos_2 )
{
	return r2pos
	(
		R2Pos_1 . x + R2Pos_2 . x,
		R2Pos_1 . y + R2Pos_2 . y
	);
}


extern void R2BASE_ScreenModeChange_Setup(void)
{
	/* PRECONDITION */
	{
	}

	/* CODE */
	{
	}
}

extern "C" extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

extern void R2BASE_ScreenModeChange_Cleanup(void)
{
	/* PRECONDITION */
	{
	}

	/* CODE */
	{

		r2rect :: R2Rect_PhysicalScreen .x1 = ScreenDescriptorBlock.SDB_Width;
		r2rect :: R2Rect_PhysicalScreen .y1 = ScreenDescriptorBlock.SDB_Height;
	}
}


/* Internal function definitions ***********************************/
