#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "dynblock.h"
#include "dynamics.h"
#include "hud_map.h"
#include "gamedef.h"

#define UseLocalAssert Yes
#include "ourasert.h"

#define PENTIUM_PROFILING_ON 0
#if PENTIUM_PROFILING_ON
#include "pentime.h"
#else
#define ProfileStart();
#define ProfileStop(x);
#endif


#define CircleRadius 99
#define DEFAULT_MAP_SCALE 400
#define DEFAULT_MAP_RADIUS 80

#define MAP_MAX_ZOOM 2000

#if PSX
	#define MAP_MIN_ZOOM 200
#else
	#define MAP_MIN_ZOOM 100
#endif

extern int sine[], cosine[]; /*Needed for GetSin and GetCos  */

static void UpdateMapVisibilities(void);
static void DrawMapOverlay(void);
static void DrawPlayerArrow(void);
static int MapModuleCompletelyClipped(MODULE *modulePtr);
static void DrawMapModule(MODULE *modulePtr, enum MAP_COLOUR_ID colourID);

#if !PSX
static int ClipLine(VECTOR2D vertex[]);
static void MakeCircleLookUpTable(void);
static int PointInsideCircle(int x, int y);
#endif

static int IsHUDMapOn = 0;

extern int SetupPolygonAccessFromShapeIndex(int shapeIndex);
extern void RotateVertex(VECTOR2D *vertexPtr, int theta);

extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
extern DISPLAYBLOCK *Player;
extern int NormalFrameTime;

static int MapScale = DEFAULT_MAP_SCALE;
static int MapRadius = DEFAULT_MAP_RADIUS;
static int ClipScale;
static int MapCentreX;
static int MapCentreY;

static int MapMinX,MapMaxX;											  
static int MapMinY,MapMaxY;

/* point in the environment which is the centre of the map */
static VECTORCH MapWorldCentre;
static int MapOrientation;

void InitHUDMap(void)
{
	#if !PSX
	MakeCircleLookUpTable();
	#endif

	PlatformSpecificInitHUDMap();

	#if PSX

	MapCentreX = 128; 
	MapCentreY = 64;																																	
	
	#else
	
	if (AvP.PlayerType == I_Alien)
	{
		MapCentreX = ScreenDescriptorBlock.SDB_Width-MapRadius*2;
		MapCentreY = ScreenDescriptorBlock.SDB_Height-MapRadius*2;
	}
	else if (AvP.PlayerType == I_Predator)
	{
		MapCentreX = ScreenDescriptorBlock.SDB_Width-MapRadius;
		MapCentreY = ScreenDescriptorBlock.SDB_Height/2-MapRadius;
	}
	else
	{
		MapCentreX = ScreenDescriptorBlock.SDB_Width-MapRadius;
		MapCentreY = ScreenDescriptorBlock.SDB_Height-MapRadius;
	}

	#endif

}

void UpdateHUDMap(void)
{
	/* update which modules are visible on the map */
	UpdateMapVisibilities();
		
	/* draw current map to screen */
	ProfileStart();		
	if (IsHUDMapOn) DrawMapOverlay();
	ProfileStop("MAP");
}

void HUDMapOn(void)
{
	IsHUDMapOn=1;
}

void HUDMapOff(void)
{
	IsHUDMapOn=0;
}

void HUDMapZoomOut(void)
{
	MapScale-= (NormalFrameTime * MapScale) >> 17;
	if (MapScale<MAP_MIN_ZOOM) MapScale=MAP_MIN_ZOOM;
}
 
void HUDMapZoomIn(void)
{
	MapScale+= (NormalFrameTime * MapScale) >> 17;
	if (MapScale>MAP_MAX_ZOOM) MapScale=MAP_MAX_ZOOM;
}

void HUDMapSmaller(void)
{
	MapRadius-= NormalFrameTime>>10;
	if (MapRadius<50) MapRadius=50;
}

void HUDMapLarger(void)
{
	MapRadius+= NormalFrameTime>>10;
	if (MapRadius>ScreenDescriptorBlock.SDB_Height/2)
		MapRadius=ScreenDescriptorBlock.SDB_Height/2;

	/* check it fits on screen... */
	{
		int allowedX = ScreenDescriptorBlock.SDB_Width - MapRadius;

		if (MapCentreX>allowedX)
			MapCentreX=allowedX;
		else if (MapCentreX<MapRadius)
			MapCentreX=MapRadius;
	}
	{
		int allowedY = ScreenDescriptorBlock.SDB_Height - MapRadius;
		if (MapCentreY>allowedY)
			MapCentreY=allowedY;
		else if (MapCentreY<MapRadius)
			MapCentreY=MapRadius;
	}

}

void HUDMapLeft(void)
{
	MapCentreX -= NormalFrameTime>>9;

	if (MapCentreX<MapRadius)
		MapCentreX=MapRadius;
}
void HUDMapRight(void)
{
	int allowedX = ScreenDescriptorBlock.SDB_Width - MapRadius;
	MapCentreX += NormalFrameTime>>9;

	if (MapCentreX>allowedX)
		MapCentreX=allowedX;
}
void HUDMapUp(void)
{
	MapCentreY -= NormalFrameTime>>9;

	if (MapCentreY<MapRadius)
		MapCentreY=MapRadius;
}
void HUDMapDown(void)
{
	int allowedY = ScreenDescriptorBlock.SDB_Height - MapRadius;
	MapCentreY += NormalFrameTime>>9;

	if (MapCentreY>allowedY)
		MapCentreY=allowedY;
}

void HUDMapWorldCentreLeft(void)
{
	int mult = ((2000-MapScale) << 10)/500 + (1 << 10);
	MapWorldCentre.vx -= (NormalFrameTime * mult) >> 19;

	HUDMapClamp();
}
void HUDMapWorldCentreRight(void)
{
	int mult = ((2000-MapScale) << 10)/500 + (1 << 10);
	MapWorldCentre.vx += (NormalFrameTime * mult) >> 19;

	HUDMapClamp();
}
void HUDMapWorldCentreUp(void)
{
	int mult = ((2000-MapScale) << 10)/500 + (1 << 10);
	MapWorldCentre.vy -= (NormalFrameTime * mult) >> 19;

	HUDMapClamp();
}
void HUDMapWorldCentreDown(void)
{
	int mult = ((2000-MapScale) << 10)/500 + (1 << 10);
	MapWorldCentre.vy += (NormalFrameTime * mult) >> 19;

	HUDMapClamp();
}

void HUDMapRotateRight(void)
{
	MapOrientation += NormalFrameTime>>6;
	if (MapOrientation>4095) MapOrientation -= 4096;
}	

void HUDMapRotateLeft(void)
{
	MapOrientation -= NormalFrameTime>>6;
 	if (MapOrientation<0) MapOrientation += 4096;
}	

void HUDMapWorldForward(void)
{
	int mult = ((2000-MapScale) << 6)/500 + (1 << 6);

	MapWorldCentre.vx += (GetSin(MapOrientation) * mult) >> 16;
	MapWorldCentre.vz += (GetCos(MapOrientation) * mult) >> 16;

	HUDMapClamp();

}	

void HUDMapWorldBackward(void)
{
	int mult = ((2000-MapScale) << 6)/500 + (1 << 6);

	MapWorldCentre.vx -= (GetSin(MapOrientation) * mult) >> 16;
	MapWorldCentre.vz -= (GetCos(MapOrientation) * mult) >> 16;

	HUDMapClamp();
}	

void HUDMapWorldSlideLeft(void)
{
	int mult = ((2000-MapScale) >> 2) + (1 << 6);

	MapWorldCentre.vx -= (GetCos(MapOrientation) * mult) >> 16;
	MapWorldCentre.vz += (GetSin(MapOrientation) * mult) >> 16;
	
	HUDMapClamp();
}	

void HUDMapWorldSlideRight(void)
{
	int mult = ((2000-MapScale) >> 2) + (1 << 6);

	MapWorldCentre.vx += (GetCos(MapOrientation) * mult) >> 16;
	MapWorldCentre.vz -= (GetSin(MapOrientation) * mult) >> 16;
	
	HUDMapClamp();
}	

void HUDMapClamp()
{
	if (MapWorldCentre.vx < MapMinX) MapWorldCentre.vx = MapMinX;
	else if (MapWorldCentre.vx > MapMaxX) MapWorldCentre.vx = MapMaxX;
	if (MapWorldCentre.vz < MapMinY) MapWorldCentre.vz = MapMinY;
	else if (MapWorldCentre.vz > MapMaxY) MapWorldCentre.vz = MapMaxY;
}	

void HUDMapResetDefaults(void)
{
	MapWorldCentre = Player->ObWorld;
	MapOrientation = Player->ObEuler.EulerY;

	MapScale = DEFAULT_MAP_SCALE;
	MapRadius = DEFAULT_MAP_RADIUS;

	MapMaxX = Player->ObWorld.vx;
	MapMinX = MapMaxX;
	MapMaxY = Player->ObWorld.vz;
	MapMinY = MapMaxY;
}	

static void UpdateMapVisibilities(void)
{
	extern int NumOnScreenBlocks;
	extern DISPLAYBLOCK *OnScreenBlockList[];

   	int numberOfObjects = NumOnScreenBlocks;

   	while (numberOfObjects--)
	{
		DISPLAYBLOCK *objectPtr = OnScreenBlockList[numberOfObjects];
		MODULE *modulePtr = objectPtr->ObMyModule;

   		if (modulePtr) /* Is object a module ? */
    	{
			/* If module is not yet visible, flag it so that it is. */
			if (!(modulePtr->m_flags & m_flag_visible_on_map))
				modulePtr->m_flags |= m_flag_visible_on_map;
        }
	}	
}

static void DrawMapOverlay(void)
{
	extern SCENE Global_Scene;
	extern int ModuleArraySize;
	MODULE **moduleList;
	
	int i;
	
	PlatformSpecificEnteringHUDMap();
	moduleList = MainSceneArray[Global_Scene]->sm_marray;
	ClipScale = (MapScale*CircleRadius)/MapRadius;

	#if !PSX
	MapWorldCentre = Player->ObWorld;
	MapOrientation = Player->ObEuler.EulerY;
	#endif

	for(i = 0; i < ModuleArraySize; i++)
	{
		MODULE *modulePtr = moduleList[i];

		if (modulePtr)
		{
			if (modulePtr->m_flags & m_flag_visible_on_map)
			{
				int yCoord = MapWorldCentre.vy-modulePtr->m_world.vy;
				if (!MapModuleCompletelyClipped(modulePtr))
				{
					if ((modulePtr->m_maxy < yCoord)||(modulePtr->m_miny > yCoord))
					{
						DrawMapModule(modulePtr,MAP_COLOUR_GREY);
					}
					else
					{
						DrawMapModule(modulePtr,MAP_COLOUR_WHITE);
					}
				}
			}
		}
	}    					       
 
 	DrawPlayerArrow();
	
	PlatformSpecificExitingHUDMap();
  return;

}
 
static int MapModuleCompletelyClipped(MODULE *modulePtr)
{
	VECTOR2D offset;

    /* calculate offset to map space */
    offset.vx = modulePtr->m_world.vx-MapWorldCentre.vx;
    offset.vy = modulePtr->m_world.vz-MapWorldCentre.vz;
	
	/* test simple bounding boxes */
	{
		int maxX = MUL_FIXED(modulePtr->m_maxx + offset.vx,ClipScale);
		if (maxX >= -CircleRadius)
		{
			int minX = MUL_FIXED(modulePtr->m_minx + offset.vx,ClipScale);
			if (minX <= CircleRadius)
				return 0;
		}
	}
	{
		int maxY = MUL_FIXED(modulePtr->m_maxy + offset.vy,ClipScale);
		if (maxY >= -CircleRadius)
		{
			int minY = MUL_FIXED(modulePtr->m_miny + offset.vy,ClipScale);
			if (minY <= CircleRadius)
				return 0;
		}
	}


	return 1;
}

static void DrawMapModule(MODULE *modulePtr, enum MAP_COLOUR_ID colourID)
{		    
	MODULEMAPBLOCK *mapPtr = modulePtr->m_mapptr;
	int numberOfItems;
	VECTOR2D offset;

    offset.vx = mapPtr->MapWorld.vx;
    offset.vy = mapPtr->MapWorld.vz;
    /* okay, let's setup the shape's data and access the first poly */
	if (modulePtr->m_dptr)
	{
		numberOfItems = SetupPolygonAccess(modulePtr->m_dptr);
	}
	else if (mapPtr->MapMorphHeader)
	{
		numberOfItems = SetupPolygonAccessFromShapeIndex(mapPtr->MapMorphHeader->mph_frames->mf_shape2);
	}
	else
	{
		numberOfItems = SetupPolygonAccessFromShapeIndex(mapPtr->MapShape);
	}
  	/* go through polys looking for those which intersect with the bounding box */
  	while(numberOfItems)
	{
 	    struct ColPolyTag polyPtr;
		AccessNextPolygon();
       	
       	GetPolygonNormal(&polyPtr);
		if (polyPtr.PolyNormal.vy>-20000 && polyPtr.PolyNormal.vy<20000)
		{
			VECTOR2D vertex[6];
			GetPolygonVertices(&polyPtr);

			{
				int highestVertex=0;
				int nextHighestVertex=0;
				int highestY=0x7fffffff;
				int nextHighestY=0x7fffffff;
				
				int v;
				
				v = polyPtr.NumberOfVertices;
				do
				{
					v--;
					if (polyPtr.PolyPoint[v].vy<highestY)
					{
						highestVertex = v;
						highestY = polyPtr.PolyPoint[v].vy;
					}
				}
				while(v);
			
				v = polyPtr.NumberOfVertices;
				do
				{
					v--;
					if (v==highestVertex) continue;
					if (polyPtr.PolyPoint[v].vy<nextHighestY)
					{
						nextHighestVertex = v;
						nextHighestY = polyPtr.PolyPoint[v].vy;
					}
				}
				while(v);
				
				vertex[0].vx = (polyPtr.PolyPoint[highestVertex].vx+offset.vx);
				vertex[0].vy = (polyPtr.PolyPoint[highestVertex].vz+offset.vy);
				vertex[1].vx = (polyPtr.PolyPoint[nextHighestVertex].vx+offset.vx);
				vertex[1].vy = (polyPtr.PolyPoint[nextHighestVertex].vz+offset.vy);
			
				if (vertex[0].vx > MapMaxX) MapMaxX = vertex[0].vx;
				else if (vertex[0].vx < MapMinX) MapMinX = vertex[0].vx;

				if (vertex[0].vy > MapMaxY) MapMaxY = vertex[0].vy;
				else if (vertex[0].vy < MapMinY) MapMinY = vertex[0].vy;
				
				if (vertex[1].vx > MapMaxX) MapMaxX = vertex[1].vx;
				else if (vertex[1].vx < MapMinX) MapMinX = vertex[1].vx;

				if (vertex[1].vy > MapMaxY) MapMaxY = vertex[1].vy;
				else if (vertex[1].vy < MapMinY) MapMinY = vertex[1].vy;

				vertex[0].vx -= MapWorldCentre.vx;
				vertex[0].vy -=	MapWorldCentre.vz;

				vertex[1].vx -= MapWorldCentre.vx;
				vertex[1].vy -=	MapWorldCentre.vz;
			
			}

			#if PSX

			// PSX has HUD map lines clipped to screen coordinates in DrawHUDMapLine

			RotateVertex(&vertex[0],MapOrientation);
		 	vertex[0].vx = MUL_FIXED(vertex[0].vx,MapScale);
			vertex[0].vy = MUL_FIXED(vertex[0].vy,MapScale);
			vertex[0].vx = MapCentreX + vertex[0].vx;
			vertex[0].vy = MapCentreY - vertex[0].vy;

			RotateVertex(&vertex[1],MapOrientation);
			vertex[1].vx = MUL_FIXED(vertex[1].vx,MapScale);
			vertex[1].vy = MUL_FIXED(vertex[1].vy,MapScale);
			vertex[1].vx = MapCentreX + vertex[1].vx;
			vertex[1].vy = MapCentreY - vertex[1].vy;
				
			DrawHUDMapLine(&vertex[0],&vertex[1],colourID);

			#else
			
			if (ClipLine(vertex))
			{
				RotateVertex(&vertex[0],Player->ObEuler.EulerY);
	   	 	vertex[0].vx = MUL_FIXED(vertex[0].vx,MapScale);
	   		vertex[0].vy = MUL_FIXED(vertex[0].vy,MapScale);
				vertex[0].vx = MapCentreX + vertex[0].vx;
				vertex[0].vy = MapCentreY - vertex[0].vy;

				RotateVertex(&vertex[1],Player->ObEuler.EulerY);
		 		vertex[1].vx = MUL_FIXED(vertex[1].vx,MapScale);
		 		vertex[1].vy = MUL_FIXED(vertex[1].vy,MapScale);
				vertex[1].vx = MapCentreX + vertex[1].vx;
				vertex[1].vy = MapCentreY - vertex[1].vy;
				
				DrawHUDMapLine(&vertex[0],&vertex[1],colourID);
      }
    	
			#endif
    
    }
                
		numberOfItems--;
	}
}

static void DrawPlayerArrow(void)
{
	VECTOR2D vertex1,vertex2;
	int arrowLength; 
 
	#if PSX
	VECTOR2D vertex3;

	arrowLength = 475;
	
	vertex1.vx = 0;
	vertex1.vy = arrowLength;
	RotateVertex(&vertex1,4096-Player->ObEuler.EulerY);
	vertex1.vx += Player->ObWorld.vx - MapWorldCentre.vx;
	vertex1.vy += Player->ObWorld.vz - MapWorldCentre.vz;
	RotateVertex(&vertex1,MapOrientation);
	vertex1.vx = MUL_FIXED(vertex1.vx,MapScale);
	vertex1.vy = MUL_FIXED(vertex1.vy,MapScale);
	vertex1.vx = MapCentreX + vertex1.vx;
	vertex1.vy = MapCentreY - vertex1.vy;
							
	vertex3.vx = vertex1.vx;
	vertex3.vy = vertex1.vy;
								
	vertex2.vx = 0;
	vertex2.vy = -arrowLength;
	RotateVertex(&vertex2,4096-Player->ObEuler.EulerY);
	vertex2.vx += Player->ObWorld.vx - MapWorldCentre.vx;
	vertex2.vy += Player->ObWorld.vz - MapWorldCentre.vz;
	RotateVertex(&vertex2,MapOrientation);
	vertex2.vx = MUL_FIXED(vertex2.vx,MapScale);
	vertex2.vy = MUL_FIXED(vertex2.vy,MapScale);
	vertex2.vx = MapCentreX + vertex2.vx;
	vertex2.vy = MapCentreY - vertex2.vy;
	
	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	vertex2.vx = -arrowLength;
	vertex2.vy = 0;
	RotateVertex(&vertex2,4096-Player->ObEuler.EulerY);
	vertex2.vx += Player->ObWorld.vx - MapWorldCentre.vx;
	vertex2.vy += Player->ObWorld.vz - MapWorldCentre.vz;
	RotateVertex(&vertex2,MapOrientation);
	vertex2.vx = MUL_FIXED(vertex2.vx,MapScale);
	vertex2.vy = MUL_FIXED(vertex2.vy,MapScale);
	vertex2.vx = MapCentreX + vertex2.vx;
	vertex2.vy = MapCentreY - vertex2.vy;

	vertex1.vx = vertex3.vx;
	vertex1.vy = vertex3.vy;

	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	vertex2.vx = arrowLength;
	vertex2.vy = 0;
 	RotateVertex(&vertex2,4096-Player->ObEuler.EulerY);
	vertex2.vx += Player->ObWorld.vx - MapWorldCentre.vx;
	vertex2.vy += Player->ObWorld.vz - MapWorldCentre.vz;
	RotateVertex(&vertex2,MapOrientation);
 	vertex2.vx = MUL_FIXED(vertex2.vx,MapScale);
	vertex2.vy = MUL_FIXED(vertex2.vy,MapScale);
	vertex2.vx = MapCentreX + vertex2.vx;
	vertex2.vy = MapCentreY - vertex2.vy;

	vertex1.vx = vertex3.vx;
	vertex1.vy = vertex3.vy;

	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	vertex1.vx = MapCentreX;
	vertex1.vy = MapCentreY;

	vertex2.vx = MapCentreX;
	vertex2.vy = MapCentreY;
																										 
	DrawHUDMapLine (&vertex1,&vertex2,MAP_COLOUR_RED);
	
	#else

	arrowLength = MUL_FIXED(300,MapScale);
	
	vertex1.vx = MapCentreX;
	vertex1.vy = MapCentreY - arrowLength;
					 									
	vertex2.vx = MapCentreX;
	vertex2.vy = MapCentreY + arrowLength;
				 	
	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	vertex2.vx = MapCentreX - arrowLength;
	vertex2.vy = MapCentreY;

	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	vertex2.vx = MapCentreX + arrowLength;
	DrawHUDMapLine(&vertex1,&vertex2,MAP_COLOUR_RED);

	#endif
}

#if !PSX
static int ClipLine(VECTOR2D vertex[])
{
	int vertex0Inside=0,vertex1Inside=0;

	if (PointInsideCircle(vertex[0].vx,vertex[0].vy))
		vertex0Inside = 1;
	if (PointInsideCircle(vertex[1].vx,vertex[1].vy))
		vertex1Inside = 1;

	/* line completely in */
	if (vertex0Inside && vertex1Inside)		
		return 1;

	/* line completely out */
	if (!vertex0Inside && !vertex1Inside)		
		return 0;
	
	/* crosses clipping boundary */
	{
		VECTOR2D minBound,maxBound;
		VECTOR2D newGuess;
		int searchCounter = 10;

		if (vertex0Inside)
		{
			minBound = vertex[0];
			maxBound = vertex[1];
		}
		else
		{
			minBound = vertex[1];
			maxBound = vertex[0];
		}

		do
		{
			int pointInside;
			newGuess.vx = (minBound.vx + maxBound.vx)/2;
			newGuess.vy = (minBound.vy + maxBound.vy)/2;

			pointInside = PointInsideCircle(newGuess.vx,newGuess.vy);

			if(pointInside == 0)
			{
				maxBound = newGuess;
			}
			else if (pointInside == -1)
			{
				minBound = newGuess;
			} 
			else break;
		}
		while(searchCounter--);
		
		if (vertex0Inside)
		{
			vertex[1] = newGuess;
		}
		else
		{
			vertex[0] = newGuess;
		}
	}
	return 1;
}

static char CircleXCoord[CircleRadius+1];

static void MakeCircleLookUpTable(void)
{
	int y=CircleRadius;
	extern unsigned char *ScreenBuffer;
	do
	{
		CircleXCoord[y]	= SqRoot32(CircleRadius*CircleRadius-y*y);
	}
	while(y--);
}

static int PointInsideCircle(int x, int y)
{
	if (y<0) y=-y;	
	y = MUL_FIXED(y,ClipScale);

	if (y>CircleRadius) return 0;

	if (x<0) x=-x;
	x = MUL_FIXED(x,ClipScale);

	if (CircleXCoord[y]>x) return -1;
	if (CircleXCoord[y]<x) return 0;
	return 1;
}
#endif




























