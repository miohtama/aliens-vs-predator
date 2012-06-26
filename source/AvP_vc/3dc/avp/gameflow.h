#ifndef _gameflow_h
#define _gameflow_h 1

// allow a compile flag to switch system on and off
// otherwise assert frenzy may ensue
#include "gamedef.h"
#include "system.h"

extern BOOL GameFlowOn;

typedef enum
{
	IGFT_NewGame,
	IGFT_ResumeGame,
}INITGAMEFLOWTYPE;

typedef enum
{
	MarineMission_NoneAsYet,
	MarineMission_DisableLifts,
	MarineMission_GoToSP2,
	MarineMission_LeaveRnDs,
	MarineMission_GoToCMC6,
	MarineMission_GoToCMC2,
	MarineMission_DestructBase,
	MarineMission_Won,
	
	PredMission_DestroyComputers,
	PredMission_GetShipGen3,
	PredMission_GetShipCMC2,
	PredMission_KillPredAlien,
	PredMission_KillQueen,
	PredMission_ReturnToShip,
	
	AlienMission_DestroyMarines,
	AlienMission_DestroyMarinesWithSpit,
	AlienMission_DestoryOtherBaddies,
	AlienMission_DestroyQueen,
	
	PlayerMissions_Last,
	
} PLAYERMISSION;

typedef enum
{
	LS_Gen1_Entrance = 1,
	LS_Gen1_Medlab,
	LS_Gen1_Gen2,
	LS_Gen1_Gen3,
	LS_Gen1_SP3,
	LS_Gen1_CMC6,
	LS_Gen1_CMC2,
	LS_Gen1_RnD4,
	LS_Gen1_SP2,
	LS_Gen2_Gen3,
	LS_CMC2_CMC4,
	LS_CMC4_CMC6,
	LS_SP3_MPS2,
	LS_MPS2_MPS4,
	LS_MPS4_SURFACE,
	LS_SP2_RnD2,
	LS_RnD2_RnD3,
	LS_RnD3_RnD4,
	LS_Gen3Internal,
	LS_MaxLifts,

} LIFTSECURITYNUMBERS;

typedef enum
{
	GOT_LIFTOVERRIDE1,
	GOT_LIFTOVERRIDE2,
	GOT_LIFTOVERRIDE3,
	GOT_LIFTOVERRIDE4,
	GOT_SELFDESTRUCT,
	GOT_COMPUTERSYSTEM,
	GOT_PREDSHIP1,
	GOT_PREDSHIP2,
	/* New stuff for Jules's level: 1/12/97 CDF */
	GOT_FAKESWITCH_IN_GENERATOR,
	GOT_DATALOG_1,
	GOT_DATALOG_2,
	GOT_DATALOG_3,
	GOT_ENDLEVEL_EVENT,	

	GOT_LASTGAMEOBJECT,
} GAMESTATECHANGEOBJECTS;

typedef struct 
{
	GAMESTATECHANGEOBJECTS StateChangeObjectNum;
	int id1;
	int id2;
}	GAMEOBJECT;

typedef enum {
	GT_YARD_DOOR_1,
	GT_YARD_DOOR_2,
	GT_YARD_DOOR_3,
	GT_YARD_DOOR_4,

	GT_LAST_TARGET_OBJECT,
} GAMETARGETOBJECTS;

typedef struct {
	GAMETARGETOBJECTS TargetObjectNum;
	char SBname[SB_NAME_LENGTH];
} GAME_STRATBLOCK_LIST_ENTRY;
	
struct player_status;

// Internal Platform independent fns.
extern int ChangingLevelGameFlowUpdate (I_AVP_ENVIRONMENTS current, I_AVP_ENVIRONMENTS next);
// returns 1 if it is playing an FMV, or 0 otherwise

extern void InitGameFlowSystem (struct player_status * ps, INITGAMEFLOWTYPE igft);

// returns 1 if the Object should be picked up, 0 otherwise
extern BOOL GameFlowStateChangeObjectEncountered (STRATEGYBLOCK * sbPtr);

// Internal Platform dependent fns.
// see glfwplat.c
extern void DoStartMissionSequence (PLAYERMISSION mission);


#endif