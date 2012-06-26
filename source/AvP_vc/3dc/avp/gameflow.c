#include "3dc.h"
#include "module.h"
#include "inline.h"

#include "stratdef.h"
#include "gamedef.h"
#include "bh_types.h"
#include "inventry.h"
#include "gameplat.h"

#include "gameflow.h"

#include "bh_types.h"
#include "bh_gener.h"

#define UseLocalAssert Yes

#include "ourasert.h"

extern void ActivateSelfDestructSequence(int seconds);

GAMEOBJECT GameObjects[] = 
{
	GOT_LIFTOVERRIDE1,
	//0x4b630a49,0xe6d1ca,
	0,0,
	
	GOT_LIFTOVERRIDE2,
	//0xa90f8f50,0x252139,
	0,0,
	
	GOT_LIFTOVERRIDE3,
	//0x32d82514,0x286a42,
	0,0,
	
	GOT_LIFTOVERRIDE4,
	//0xa2befeb4,0xe2e009,
	0,0,

	GOT_SELFDESTRUCT,
	//0xf2d0d89c,0x923401,
	0,0,
	
	GOT_PREDSHIP1,
	//0x385fb8b9,0x7e14ad,
	0,0,
	
	GOT_PREDSHIP2,
	0,0,

	/* New stuff for Jules's level: 1/12/97 CDF */

	GOT_FAKESWITCH_IN_GENERATOR,
	0x5caeb020,0x88620,

	GOT_DATALOG_1,
	0xbbfbd11a,0x1d34bf,

	GOT_DATALOG_2,
	0x68045d04,0x69092e,

	GOT_DATALOG_3,
	0x3d784e97,0x31a9c5,

	GOT_ENDLEVEL_EVENT,	
	0x5655280,0xb5aef1,
		
	// This has to be last in the list
	GOT_LASTGAMEOBJECT,
	0,0
	
};

GAME_STRATBLOCK_LIST_ENTRY GameStratblockList[] = {

	GT_YARD_DOOR_1,
	{0xd9,0x15,0xd7,0xcc,0xa7,0xf4,0xb6,0x00,},
	GT_YARD_DOOR_2,
	{0x01,0x0a,0x0f,0xf5,0x76,0x0b,0x5a,0x00,},
	GT_YARD_DOOR_3,
	{0x7b,0x18,0x0f,0xa7,0x14,0xae,0x67,0x00,},
	GT_YARD_DOOR_4,
	{0xd7,0x52,0x95,0x48,0xcc,0xf2,0x14,0x00,},

	GT_LAST_TARGET_OBJECT,
	{0,0,0,0,0,0,0,0,},

};

BOOL GameFlowOn = 1;

static unsigned long MakeFlag (unsigned long num)
{
	return(1<<num);
}

int ChangingLevelGameFlowUpdate (I_AVP_ENVIRONMENTS current, I_AVP_ENVIRONMENTS next)
{
	int retval = 0;
	
	if (!(AvP.Network == I_No_Network) || !GameFlowOn)
	{
 		return(0);
	}
	
	/* Hack for safety.  I fear change.  CDF 1/12/97 */
	return(0);
		
	switch (AvP.PlayerType)
	{
		case I_Marine:
		{
			switch (PlayerStatusPtr->CurrentMission)
			{
				case MarineMission_NoneAsYet:
				{
					GLOBALASSERT (current == I_Entrance && next == I_Gen1);
					
					DoStartMissionSequence (MarineMission_DisableLifts);
					retval = 1;
					PlayerStatusPtr->CurrentMission = MarineMission_DisableLifts;
					PlayerStatusPtr->securityClearances = MakeFlag (LS_Gen1_Gen2) + 
																								MakeFlag (LS_Gen3Internal) +
																								MakeFlag (LS_Gen2_Gen3);
					
					break;
				}
				
				case MarineMission_DisableLifts:
				{
					GLOBALASSERT ((current == I_Gen1 ||
												current == I_Gen2 ||
												current == I_Gen3)
												&& 
												(next == I_Gen1 ||
												next == I_Gen2 ||
												next == I_Gen3)
												);
					
					if (next == I_Gen2)
					{
						PlayerStatusPtr->securityClearances &= ~MakeFlag (LS_Gen1_Gen2);
					}
					
					if (current == I_Gen3 && next == I_Gen1)
					{
						DoStartMissionSequence (MarineMission_GoToSP2);
						retval = 1;
						PlayerStatusPtr->CurrentMission = MarineMission_GoToSP2;
						PlayerStatusPtr->securityClearances = MakeFlag (LS_Gen1_SP2);
					}
					
					break;
				}
				
				case MarineMission_GoToSP2:
				{
					GLOBALASSERT (current == I_Gen1 && next == I_Sp2);
					
					DoStartMissionSequence (MarineMission_LeaveRnDs);
					retval = 1;
					PlayerStatusPtr->CurrentMission = MarineMission_LeaveRnDs;
					PlayerStatusPtr->securityClearances = MakeFlag (LS_SP2_RnD2) + 
																								MakeFlag (LS_RnD2_RnD3) + 
																								MakeFlag (LS_Gen1_RnD4) + 
																								MakeFlag (LS_RnD3_RnD4);
					
					break;
				}
				
				case MarineMission_LeaveRnDs:
				{
					GLOBALASSERT ((current == I_Rnd4 ||
												current == I_Rnd2 ||
												current == I_Gen1 ||
												current == I_Sp2 ||
												current == I_Rnd3)
												&& 
												(next == I_Rnd4 ||
												next == I_Rnd2 ||
												next == I_Gen1 ||
												next == I_Sp2 ||
												next == I_Rnd3)
												);
					
					if (current == I_Rnd4 && next == I_Gen1)
					{
						DoStartMissionSequence (MarineMission_GoToCMC6);
						retval = 1;
						PlayerStatusPtr->CurrentMission = MarineMission_GoToCMC6;
						PlayerStatusPtr->securityClearances = MakeFlag (LS_Gen1_CMC6);
					}
					
					break;
				}
				
				case MarineMission_GoToCMC6:
				{
					GLOBALASSERT (current == I_Gen1 && next == I_Cmc6);

					DoStartMissionSequence (MarineMission_GoToCMC2);
					retval = 1;
					PlayerStatusPtr->CurrentMission = MarineMission_GoToCMC2;
					PlayerStatusPtr->securityClearances = MakeFlag (LS_CMC2_CMC4) + 
																								MakeFlag (LS_CMC4_CMC6);
					
					break;
				}
				
				case MarineMission_GoToCMC2:
				{
					GLOBALASSERT ((current == I_Cmc4 ||
												current == I_Cmc2 ||
												current == I_Cmc6)
												&& 
												(next == I_Cmc4 ||
												next == I_Cmc2 ||
												next == I_Cmc6)
												);

					if (current == I_Cmc4 && next == I_Cmc2)
					{
						DoStartMissionSequence (MarineMission_DestructBase);
						retval = 1;
						PlayerStatusPtr->CurrentMission = MarineMission_DestructBase;
						PlayerStatusPtr->securityClearances = MakeFlag (LS_MPS2_MPS4) +
																									MakeFlag (LS_SP3_MPS2) +
																									MakeFlag (LS_Gen1_Medlab) +
																									MakeFlag (LS_Gen1_CMC2) +
																									MakeFlag (LS_Gen1_SP3);
					}
					
					break;
				}
				
				case MarineMission_DestructBase:
				{
					if (current == I_Mps4 && next == I_Surface)
					{
						DoStartMissionSequence (MarineMission_Won);
					}
					
					break;
				}
				
				case MarineMission_Won:
				{
					break;
				}
				
				
				default:
				{
					GLOBALASSERT (0 == "Mission not in marine list");
					break;
				}
			}
			break;
		}
		
		case I_Predator:
		{
			switch (PlayerStatusPtr->CurrentMission)
			{
				case PredMission_DestroyComputers:
				{
					if (current == I_Cmc4 && next == I_Cmc6)
					{
						PlayerStatusPtr->securityClearances = 
							MakeFlag (LS_Gen1_Entrance)+
							MakeFlag (LS_Gen1_Medlab)+
							MakeFlag (LS_Gen1_Gen2)+
							MakeFlag (LS_Gen1_Gen3)+
							MakeFlag (LS_Gen1_SP3)+
							MakeFlag (LS_Gen1_CMC6)+
							MakeFlag (LS_Gen1_CMC2)+
							MakeFlag (LS_Gen1_RnD4)+
							MakeFlag (LS_Gen1_SP2)+
							MakeFlag (LS_Gen2_Gen3)+
							MakeFlag (LS_CMC2_CMC4)+
							MakeFlag (LS_CMC4_CMC6)+
							MakeFlag (LS_SP3_MPS2)+
							MakeFlag (LS_MPS2_MPS4)+
							MakeFlag (LS_MPS4_SURFACE)+
							MakeFlag (LS_SP2_RnD2)+
							MakeFlag (LS_RnD2_RnD3)+
							MakeFlag (LS_RnD3_RnD4)+
							MakeFlag (LS_Gen3Internal)+
							MakeFlag (LS_MaxLifts);
																									
					}
					else if (current == I_Cmc6)
					{
						DoStartMissionSequence (PredMission_GetShipGen3);
						PlayerStatusPtr->CurrentMission = PredMission_GetShipGen3;
						retval = 1;
					}
					
					break;
				}

				case PredMission_GetShipGen3:
				{
					if (PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_PREDSHIP1))
					{
						if (PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_PREDSHIP2))
						{
							DoStartMissionSequence (PredMission_KillPredAlien);
							PlayerStatusPtr->CurrentMission = PredMission_KillPredAlien;
							retval = 1;
						}
						else
						{
							DoStartMissionSequence (PredMission_GetShipCMC2);
							PlayerStatusPtr->CurrentMission = PredMission_GetShipCMC2;
							retval = 1;
						}
					}
					break;
				}

				case PredMission_GetShipCMC2:
				{
					if (PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_PREDSHIP2))
					{
						DoStartMissionSequence (PredMission_KillPredAlien);
						PlayerStatusPtr->CurrentMission = PredMission_KillPredAlien;
						retval = 1;
					}
					break;
				}

				case PredMission_KillPredAlien:
				{
					break;
				}

				case PredMission_KillQueen:
				{
					break;
				}

				case PredMission_ReturnToShip:
				{
					break;
				}

				default:
				{
					GLOBALASSERT (0 == "Mission not in Predator list");
					break;
				}
			}
			break;
		}
		
		case I_Alien:
		{
			break;
		}
		
		default:
		{
			GLOBALASSERT (0 == "Player type is wrong");
			break;
		}
	}
	return(retval);
	
}

void InitGameFlowSystem (struct player_status * ps, INITGAMEFLOWTYPE igft)
{
	if (!GameFlowOn)
	{
		ps->securityClearances = 0xfffffff;
		return;
	}
	if (igft == IGFT_NewGame)
	{
		switch (AvP.PlayerType)
		{
			case I_Marine:
			{
				ps->CurrentMission = MarineMission_NoneAsYet;
				ps->StateChangeObjectFlags = 0;
				//ps->securityClearances = MakeFlag (LS_Gen1_Entrance);
				ps->securityClearances = 0;
				
				break;
			}
			
			case I_Predator:
			{
				ps->CurrentMission = PredMission_DestroyComputers;
				ps->StateChangeObjectFlags = 0;
				ps->securityClearances = MakeFlag (LS_CMC4_CMC6);
				break;
			}
			
			case I_Alien:
			{
				ps->CurrentMission = AlienMission_DestroyMarines;
				ps->StateChangeObjectFlags = 0;
				
				break;
			}
			
			
			default:
			{
				break;
			}
		}
	}
}

static GAMEOBJECT * GetGameObject (const char * id)
{
	GAMEOBJECT * goptr = &GameObjects[0];
	
	while (goptr->StateChangeObjectNum != GOT_LASTGAMEOBJECT)
	{
		int id1, id2;
		
		id1 = *(int*)id;
		id2 = *(((int*)id) + 1);
	
		if ((id1 == goptr->id1) && (id2 == goptr->id2))
		{
			return(goptr);
		}
		
		goptr++;
		
	}
	return(0);
}

STRATEGYBLOCK *GetMeThisStrategyBlock(GAMETARGETOBJECTS This_One) {
	
	int a,num;

	a=0;
	num=-1;
	
	while (GameStratblockList[a].TargetObjectNum!=GT_LAST_TARGET_OBJECT) {
		if (GameStratblockList[a].TargetObjectNum==This_One) {
			num=a;
			break;
		}
		a++;
	}

	if (num==-1) {
		return(NULL);
	}

	return(FindSBWithName(GameStratblockList[num].SBname));

}

BOOL GameFlowStateChangeObjectEncountered (STRATEGYBLOCK * sbPtr)
{
	//I don't hink any of these are necessary any more - Richard.
	#if 0
	char * id = sbPtr->SBname;

	if (!(AvP.Network == I_No_Network) || !GameFlowOn)
	{
 		return (0);
	}

	switch (AvP.PlayerType)
	{
		case I_Marine:
		{
			switch (PlayerStatusPtr->CurrentMission)
			{
				case MarineMission_DisableLifts:
				{
					GAMEOBJECT * goptr = GetGameObject (id);
				
					GLOBALASSERT(0);
					/* How could that happen??? */
					
					if (goptr)
					{
						if (goptr->StateChangeObjectNum >= GOT_LIFTOVERRIDE1 && 
								goptr->StateChangeObjectNum <= GOT_LIFTOVERRIDE4)
						{
							PlayerStatusPtr->StateChangeObjectFlags |= MakeFlag (goptr->StateChangeObjectNum);
						}
						if (goptr->StateChangeObjectNum == GOT_LIFTOVERRIDE3)
						{
							PlayerStatusPtr->securityClearances &= ~MakeFlag (LS_Gen3Internal);
						}
					}
					
					if (
							(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_LIFTOVERRIDE1)) &&
							(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_LIFTOVERRIDE2)) &&
							(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_LIFTOVERRIDE3)) &&
							(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_LIFTOVERRIDE4)) 
							)
					{
						PlayerStatusPtr->securityClearances |= MakeFlag (LS_Gen1_Gen3);
					}
					
					break;
				}
				
				case MarineMission_DestructBase:
				{
					GAMEOBJECT * goptr = GetGameObject (id);

					GLOBALASSERT(0);
					/* How could that happen??? */

					if (goptr) if (goptr->StateChangeObjectNum == GOT_SELFDESTRUCT)
					{
						ActivateSelfDestructSequence(150);
						PlayerStatusPtr->securityClearances |= MakeFlag (LS_MPS4_SURFACE);
					}
					
					break;
				}
				
				case MarineMission_NoneAsYet:
				{
					/* This is Jules's level.  CDF 1/12/97 */
					GAMEOBJECT * goptr = GetGameObject (id);
					
					if (goptr) {
						switch (goptr->StateChangeObjectNum) {
							case GOT_FAKESWITCH_IN_GENERATOR:
							{
								STRATEGYBLOCK *target;
								
								target=NULL;
								target=GetMeThisStrategyBlock(GT_YARD_DOOR_1);
								if (target) {
									UnlockThisProxdoor(target);
								}
								target=NULL;
								target=GetMeThisStrategyBlock(GT_YARD_DOOR_2);
								if (target) {
									UnlockThisProxdoor(target);
								}
								target=NULL;
								target=GetMeThisStrategyBlock(GT_YARD_DOOR_3);
								if (target) {
									UnlockThisProxdoor(target);
								}
								target=NULL;
								target=GetMeThisStrategyBlock(GT_YARD_DOOR_4);
								if (target) {
									UnlockThisProxdoor(target);
								}
								ActivateHive();
																								
								break;
							}
							case GOT_DATALOG_1:
							case GOT_DATALOG_2:
							case GOT_DATALOG_3:
								PlayerStatusPtr->StateChangeObjectFlags |= MakeFlag (goptr->StateChangeObjectNum);
								return(1);
								break;
							case GOT_ENDLEVEL_EVENT:
								if (
										(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_DATALOG_1)) &&
										(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_DATALOG_2)) &&
										(PlayerStatusPtr->StateChangeObjectFlags & MakeFlag (GOT_DATALOG_3)) 
									) {
										/* I guess we'd better end the level, then. */
										textprint("\n\n\nEND OF LEVEL!\n\n\n");
										AvP.MainLoopRunning = 0;
									}
								break;
							default:
								textprint("Unknown gameflow object %d! I'm well confused!\n",goptr->StateChangeObjectNum);
								GLOBALASSERT(0);
								break;
						}
					}					

					break;
				}
				
				default:
				{
					break;
				}
			}
			
			break;
		}
		
		case I_Predator:
		{
			#if 1
			return(1);
			#else
			switch (PlayerStatusPtr->CurrentMission)
			{
				case PredMission_GetShipGen3:
				case PredMission_GetShipCMC2:
				{
					GAMEOBJECT * goptr = GetGameObject (id);
					if (goptr) 
						if (goptr->StateChangeObjectNum == GOT_PREDSHIP1 ||
								goptr->StateChangeObjectNum == GOT_PREDSHIP2)
						{
							PlayerStatusPtr->StateChangeObjectFlags |= MakeFlag (goptr->StateChangeObjectNum);
							return(1);
						}
				
					break;
				}
			
				default:
				{
					break;
				}
			}
			#endif
			break;
		}

		default:
		{
			break;
		}
	}
	#endif
	return(0);
}



