/*------------------- Patrick 15/10/95 ----------------------
  Special Move data file:
  Data is hard-coded at the moment.
  -----------------------------------------------------------*/
#include "3dc.h"
#include "module.h"

#include "stratdef.h"
#include "gamedef.h"

#include "bh_types.h"
#include "pmove.h"


/*------------------- Patrick 23/10/95 ----------------------
  Special Move control data
  -----------------------------------------------------------*/

/* this special move rolls thro 360 degrees to the left, while firing */
static SMV_CONTROLPOINT SpecialMoveControlData0[] =
{ 
	{	/* control point 0 */
		{-2000,0,0},
		{0,0,0},
		0,
		SMv_Flag_FirePrimaryWeapon|SMv_Flag_HitTheDeck,
	},
	{	/* control point 1: rotated thro' 360 */
		{0,0,0},
		{0,0,(-4096*2)},
		ONE_FIXED,
		SMv_Flag_HitTheDeck,
	},

	
};

/* this special move rolls thro 360 degrees to the right, while firing */
static SMV_CONTROLPOINT SpecialMoveControlData1[] =
{ 
	{	/* control point 0 */
		{2000,0,0},
		{0,0,0},
		0,
		SMv_Flag_FirePrimaryWeapon|SMv_Flag_HitTheDeck,
	},
	{	/* control point 1: rotated thro' 360 */
		{0,0,0},
		{0,0,(4096*2)},
		ONE_FIXED,
		0,
	},
	
};

/* this special move does the shoulder roll */
static SMV_CONTROLPOINT SpecialMoveControlData2[] =
{ 
	{	/* control point 0 */
		{0,0,3000},
		{0,0,0},
		0,
		SMv_Flag_HitTheDeck,
	},
	{	/* control point 1 : fall forward*/
		{0,0,0},
		{0,-1024,1024},
		ONE_FIXED/3,
		SMv_Flag_HitTheDeck,
	},
	{	/* control point 2 : pause */
		{0,0,0},
		{0,-1024,1024},
		(ONE_FIXED/3 + ONE_FIXED/6),
		SMv_Flag_FirePrimaryWeapon,
	},
	{	/* control point 3 : whip round*/
		{0,0,0},
		{0,-2048,0},
		(ONE_FIXED/3 + ONE_FIXED/6 + ONE_FIXED/3),
		0,
	},


	
};


/*------------------- Patrick 23/10/95 ----------------------
  Special move headers
  -----------------------------------------------------------*/

SMV_HEADER SpecialMoveHeader0 =
{
	SMvType_Marine,
	2,
	&SpecialMoveControlData0[0],
};			 

SMV_HEADER SpecialMoveHeader1 =
{
	SMvType_Marine,
	2,
	&SpecialMoveControlData1[0],

};
SMV_HEADER SpecialMoveHeader2 =
{
	SMvType_Marine,
	4,
	&SpecialMoveControlData2[0],
};




/*------------------- Patrick 30/10/95 ----------------------
  INPUT COMBINATION SEQUENCE DATA

  The first 2 are for roll left + shoulder roll
  (which start with same combo).
  -----------------------------------------------------------*/

static INPUT_SEQUENCE_NODE InSeq_1G =
{
	(INPUT_BITMASK_LEFT|INPUT_BITMASK_FIRE),
	&SpecialMoveHeader0, /* roll left */
	0,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1F =
{
	(INPUT_BITMASK_BACKWARD|INPUT_BITMASK_FIRE),
	&SpecialMoveHeader2, /* shoulder roll */
	0,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1E =
{
	(INPUT_BITMASK_LEFT|INPUT_BITMASK_FORWARD),
	0, 
	&InSeq_1G,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1D =
{
	(INPUT_BITMASK_BACKWARD|INPUT_BITMASK_LEFT),
	0,
	&InSeq_1F,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1C =
{
	(INPUT_BITMASK_LEFT),
	0, 
	&InSeq_1D,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1B =
{
	(INPUT_BITMASK_FORWARD),
	0,
	&InSeq_1E,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_1A =
{
	(INPUT_BITMASK_FORWARD|INPUT_BITMASK_LEFT),
	0,
	&InSeq_1B,
	&InSeq_1C,	
};

/*------------------- Patrick 30/10/95 ----------------------
  And the next set are for roll	right
  -----------------------------------------------------------*/

static INPUT_SEQUENCE_NODE InSeq_2D =
{
	(INPUT_BITMASK_RIGHT|INPUT_BITMASK_FIRE),
	&SpecialMoveHeader1, /* roll right */ 
	0,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_2C =
{
	(INPUT_BITMASK_FORWARD|INPUT_BITMASK_RIGHT),
	0, 
	&InSeq_2D,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_2B =
{
	(INPUT_BITMASK_FORWARD),
	0, 
	&InSeq_2C,
	0,	
};

static INPUT_SEQUENCE_NODE InSeq_2A =
{
	(INPUT_BITMASK_FORWARD|INPUT_BITMASK_RIGHT),
	0, 
	&InSeq_2B,
	0,	
};

/*------------------- Patrick 23/10/95 ----------------------
  And the global array of sequence starters
  -----------------------------------------------------------*/

INPUT_SEQUENCE_NODE *MarineInputSequences[] =
{
	&InSeq_1A,
	&InSeq_2A,
	0, /* terminate list */
};