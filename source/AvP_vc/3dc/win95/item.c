
#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "krender.h" /* KJL 11:37:53 12/05/96 - new scandraws patch */

/* KJL 15:02:50 05/14/97 - new max lighting intensity */
#define MAX_INTENSITY (65536*4-1)


#if StandardShapeLanguage


#define UseLocalAssert Yes

#include "ourasert.h"

/*
  The outputclockwise compile option has been
  maintained in case anyone wants to use the existing
  triangle array code with counterclockwise ordering.
  In the case of Win95, we might want to this if we can ever
  persuade the Direct3D rasterisation module to come up
  with CULL_CCW set. Note that outputtriangles has now been purged
  from the system.
*/

#define outputclockwise Yes

#define use_div_fixed	Yes

#define trip_debugger No

#if trip_debugger
int testa = 0;
int testb = 100;
int testc = 0;
#endif

/*
	To make scan draws work backwards as
	well as forwards, i.e. to cope with
    items that would be backface culled
	without the no cull flag set, as in racing
	game TLOs.
	This should now work for all polygon types.
*/

#define ReverseDraws Yes

/*
	To optimise scan draws with local variables
	explicitly braced to improve compiler
	optimisation.
    NOTE THIS IS -->ONLY<-- IMPLEMENTED FOR
	VIDEOMODETYPE_8, 2DTEXTURE, 3DTEXTURE (LINEAR)
	AND 3DTEXTURE (LINEAR_S) AT PRESENT.
	AND NOT ALL THE OPTIONS INSIDE THOSE!!!
	NOTE ALSO THAT THIS APPEARS TO PRODUCE BAD CODE
	WITH WATCOM 10.0 AND -oeitlr, BUT WE HOPE IT WILL
	WORK WITH WATCOM 10.6
	-->CRASHES, AS IT HAPPENS!!!<--
*/
/*
	Prototypes
*/



/*

 externs for commonly used global variables and arrays

*/

	extern int VideoModeType;
	extern int VideoModeTypeScreen;
	extern int ScanDrawMode;
	extern int **ShadingTableArray;
	extern unsigned char **PaletteShadingTableArray;
	extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
	extern unsigned char *ScreenBuffer;



	extern IMAGEHEADER *ImageHeaderPtrs[MaxImages];
	extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
	extern long BackBufferPitch;

    /* For multiple execute buffers */
	extern int NumVertices;


#endif /* for support3dtextures */

/*

 Global Variables

*/


//	int *ItemPointers[maxpolyptrs];

//	int ItemData[maxpolys * avgpolysize];


	int **NextFreeItemPtr;
	int *NextFreeItemData;

	int NumFreeItemPtrs;
	int NumFreeItemData;

	int ItemCount;							/* Instead of a Terminator */

	POLYHEADER *global_pheader;

	int MIP_Index;

/*

 Global Variables For Scan Convertors

*/

	int ScanData[maxpolys*maxscansize];
	int *NextFreeScanData;
	int NumFreeScanData;
	int NumScans;

   
	int NumPolyPts;



/*

 Item Data Structures.

 Buffer Initialisation and Allocation Functions.

*/




/*
	John and Neal's code
	Note this is only experimental, for the interface engine,
	and therefore does not go through the standard scan draw
	function array!!!  Also, it only works in VideoModeType_8!!!
	USE AT YOUR PERIL!!!!
*/

void Draw_Line_VMType_8(VECTOR2D* LineStart, VECTOR2D* LineEnd, int LineColour)
{
	int gosa, tame, hani, dire;
	int w, x, y;
	int x1, y1, x2, y2;
	unsigned char *sbufferptr;
	unsigned char colour = LineColour;

	x1 = LineStart->vx;
	y1 = LineStart->vy;
	x2 = LineEnd->vx;
	y2 = LineEnd->vy;

	x1=max(x1,Global_VDB_Ptr->VDB_ClipLeft);
	x1=min(x1,Global_VDB_Ptr->VDB_ClipRight-1);
	x2=max(x2,Global_VDB_Ptr->VDB_ClipLeft);
	x2=min(x2,Global_VDB_Ptr->VDB_ClipRight-1);

	y1=max(y1,Global_VDB_Ptr->VDB_ClipUp);
	y1=min(y1,Global_VDB_Ptr->VDB_ClipDown-1);
	y2=max(y2,Global_VDB_Ptr->VDB_ClipUp);
	y2=min(y2,Global_VDB_Ptr->VDB_ClipDown-1);

	if( abs( x2 - x1 ) < abs( y2 - y1 ) ) {
		if( y2 < y1 ) {
			w = y1;
			y1 = y2;
			y2 = w;
			w = x1;
			x1 = x2;
			x2 = w;
		}
		dire = x1 < x2 ? 1 : -1;
		gosa = abs( x2 - x1 );
		hani = y2 - y1;
		tame = hani / 2;
		x = x1;
		for( y = y1; y <= y2; y++ ) {
			sbufferptr =
				ScreenBuffer +
				(y * BackBufferPitch) + x;
			*sbufferptr = colour;
			tame += gosa;
			if( tame > hani ) {
				x += dire;
				tame -= hani;
			}
		}
	} else {
		if( x2 < x1 ) {
			w = x1;
			x1 = x2;
			x2 = w;
			w = y1;
			y1 = y2;
			y2 = w;
		}
		dire = y1 < y2 ? 1 : -1;
		gosa = abs( y2 - y1 );
		hani = x2 - x1;
		tame = hani / 2;
		y = y1;
		for( x = x1; x <= x2; x++ ) {
			sbufferptr =
				ScreenBuffer +
				(y * BackBufferPitch) + x;
			*sbufferptr = colour;
			tame += gosa;
			if( tame > hani ) {
				y += dire;
				tame -= hani;
			}
		}
	}

}



/*

 Scan Convert and Draw I_GouraudPolygon

*/


void Draw_Item_GouraudPolygon(int *itemptr)
{
	if (ScanDrawMode == ScanDrawDirectDraw)
   	{
   		KDraw_Item_GouraudPolygon(itemptr);
	}
}


void Draw_Item_2dTexturePolygon(int *itemptr)
{
	if (ScanDrawMode == ScanDrawDirectDraw)
    {
		KDraw_Item_2dTexturePolygon(itemptr);
	}
}

void Draw_Item_Gouraud2dTexturePolygon(int *itemptr)
{
	if (ScanDrawMode == ScanDrawDirectDraw)
	{
		KDraw_Item_Gouraud2dTexturePolygon(itemptr);
	}
}

void Draw_Item_Gouraud3dTexturePolygon(int *itemptr)
{
	if (ScanDrawMode == ScanDrawDirectDraw)
	{
		KDraw_Item_Gouraud3dTexturePolygon(itemptr);
	}
}

void Draw_Item_ZB_Gouraud3dTexturePolygon(int *itemptr)
{
}

void Draw_Item_ZB_GouraudPolygon(int *itemptr)
{
}


void Draw_Item_ZB_2dTexturePolygon(int *itemptr)
{
}


void Draw_Item_ZB_Gouraud2dTexturePolygon(int *itemptr)
{
}



void Draw_Item_ZB_3dTexturedPolygon(int *itemptr)
{
}










