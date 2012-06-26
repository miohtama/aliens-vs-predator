/* 
	this now contains only the map processing
	functions	for the PC and saturn - compiled in maos
	have been moved to comp_map
*/
#include "3dc.h"
#include "module.h"
#include "inline.h"

#include "stratdef.h"
#include "gamedef.h"

#define UseLocalAssert Yes

#include "ourasert.h"


/************ GLOBALS **********/


DISPLAYBLOCK *Player;


/***************************************************************/

					/* MAP TYPE PROCESSING FUNCTIONS*/

/**************************************************************/
/*
 The functions require a pointer to a display block. It uses "ObType"
 in the block to select the function.
*/

static void Process_Default_MapType(DISPLAYBLOCK *dblockptr);
static void Process_Player_MapType(DISPLAYBLOCK *dblockptr);
static void Process_PlayerShipCamera_MapType(DISPLAYBLOCK *dblockptr);
static void Process_Sprite_MapType(DISPLAYBLOCK *dblockptr);
static void Process_Term_MapType(DISPLAYBLOCK *dblockptr);


void (*MapTypeFunctions[])(DISPLAYBLOCK *dblockptr) = 
{
	Process_Default_MapType,
	Process_Player_MapType,
	Process_PlayerShipCamera_MapType,	/* Player ship camera */
	Process_Sprite_MapType,
	Process_Term_MapType
};


static void Process_Default_MapType(DISPLAYBLOCK *dptr)
{
	dptr->ObLightType = LightType_PerVertex;
	dptr->ObFlags |= ObFlag_MultLSrc;
}


static void Process_Player_MapType(DISPLAYBLOCK *dptr)
{
  	Player = dptr;
}


static void Process_PlayerShipCamera_MapType(DISPLAYBLOCK *dptr)
{
}



static void Process_Sprite_MapType(DISPLAYBLOCK *dblockptr)
{
}


static void Process_Term_MapType(DISPLAYBLOCK *dptr)
{}


