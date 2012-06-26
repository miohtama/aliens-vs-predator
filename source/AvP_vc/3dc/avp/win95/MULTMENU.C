/*-------------------------Patrick 7/5/97-----------------------------
  Source for the multiplayer menus: I have have heavily modified this
 ---------------------------------------------------------------------*/

#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "stratdef.h"
#include "gamedef.h"
#include "font.h"
#include "menudefs.h" 
#include "menugfx.h"
#include "dp_func.h"
#include "multmenu.h"
#include "equipmnt.h"
#include "pldnet.h"

#define UseLocalAssert Yes
#include "ourasert.h"

static char mPLevelNameString[] = "KIPPER";
static char mPScoreLimitString[] = "PAT";
static char mPTimeLimitString[] = "PAT";
static char mPBlankString[] = "\0";

/* Setup stuff for the Multiplayer Menus graphics and options */
static MENU_GRAPHIC_ITEM MPGameSelect_MenuItems[] = {{-1,}};

static MENUGRAPHIC MPGameSelect_Backdrop = {"menugfx\\pg0.pg0",-1,NULL,NULL,0,0,640,480,};

static MENU_TEXT_ITEM MPGameSelect_MenuFItems[] = 
{
	{MMSelect_Title, TEXTSTRING_MULTI_TITLE, NULL,  80, 40},
	{MMSelect_P1Name, TEXTSTRING_BLANK, mPBlankString, 80, 80},
	{MMSelect_P2Name, TEXTSTRING_BLANK, mPBlankString, 80, 100},
	{MMSelect_P3Name, TEXTSTRING_BLANK, mPBlankString, 80, 120},
	{MMSelect_P4Name, TEXTSTRING_BLANK, mPBlankString, 80, 140},
	{MMSelect_P5Name, TEXTSTRING_BLANK, mPBlankString, 80, 160},
	{MMSelect_P6Name, TEXTSTRING_BLANK, mPBlankString, 80, 180},
	{MMSelect_P7Name, TEXTSTRING_BLANK, mPBlankString, 80, 200},
	{MMSelect_P8Name, TEXTSTRING_BLANK, mPBlankString, 80, 220},
	{MMSelect_P1Type, TEXTSTRING_BLANK, mPBlankString, 240, 80},
	{MMSelect_P2Type, TEXTSTRING_BLANK, mPBlankString, 240, 100},
	{MMSelect_P3Type, TEXTSTRING_BLANK, mPBlankString, 240, 120},
	{MMSelect_P4Type, TEXTSTRING_BLANK, mPBlankString, 240, 140},
	{MMSelect_P5Type, TEXTSTRING_BLANK, mPBlankString, 240, 160},
	{MMSelect_P6Type, TEXTSTRING_BLANK, mPBlankString, 240, 180},
	{MMSelect_P7Type, TEXTSTRING_BLANK, mPBlankString, 240, 200},
	{MMSelect_P8Type, TEXTSTRING_BLANK, mPBlankString, 240, 220},
	{MMSelect_P1Ok, TEXTSTRING_BLANK, mPBlankString, 400, 80},
	{MMSelect_P2Ok, TEXTSTRING_BLANK, mPBlankString, 400, 100},
	{MMSelect_P3Ok, TEXTSTRING_BLANK, mPBlankString, 400, 120},
	{MMSelect_P4Ok, TEXTSTRING_BLANK, mPBlankString, 400, 140},
	{MMSelect_P5Ok, TEXTSTRING_BLANK, mPBlankString, 400, 160},
	{MMSelect_P6Ok, TEXTSTRING_BLANK, mPBlankString, 400, 180},
	{MMSelect_P7Ok, TEXTSTRING_BLANK, mPBlankString, 400, 200},
	{MMSelect_P8Ok, TEXTSTRING_BLANK, mPBlankString, 400, 220},
	{MMSelect_Character, TEXTSTRING_MULTI_CHARACTER, NULL,  80, 260},
	{MMSelect_Level, TEXTSTRING_MULTI_LEVEL, NULL,  80, 280},
	{MMSelect_Mode, TEXTSTRING_MULTI_GAMEMODE, NULL,  80, 300},
	{MMSelect_ScoreLimit, TEXTSTRING_MULTI_SCORELIMIT, NULL,  80, 320},
	{MMSelect_TimeLimit, TEXTSTRING_MULTI_TIMELIMIT, NULL,  80, 340},
	{MMSelect_Start, TEXTSTRING_MULTI_START, NULL,  80, 380},
	{MMSelect_Cancel, TEXTSTRING_MULTI_CANCEL, NULL,  240, 380},
	{MMSelect_CharacterSelection, TEXTSTRING_MULTI_MARINE, NULL,  240, 260},
	{MMSelect_LevelSelection, TEXTSTRING_BLANK, mPLevelNameString,  240, 280},
	{MMSelect_ModeSelection, TEXTSTRING_MULTI_INDIVIDUAL, NULL, 240, 300},
	{MMSelect_ScoreLimitSelection, TEXTSTRING_NO, NULL, 240, 320},
	{MMSelect_TimeLimitSelection, TEXTSTRING_NO, NULL, 240, 340},
	{-1,}
};

static AVP_MENU MPGameSelectMenu = 
{
	&MPGameSelect_Backdrop,
	&MPGameSelect_MenuItems[0],
	&MPGameSelect_MenuFItems[0],
	-1,
	0,
};


/* Local function prototypes */
static MULTIPLAYER_SELECT_OPTIONS DoSelectionState(void);
static void DrawMultiStartUpScreen(void);
static void DoMultiStartUpSelection(void);
static void DisplayMultiStartUpErrorMessage(void);

/* Externally defined functions and variables used in this file */
extern int IDemandGoBackward();
extern int IDemandGoForward();
extern int IDemandSelect();
extern int menustate;
extern int NormalFrameTime;

/* globals used in this file */
static int	debounce = 0;
static MULTIPLAYER_SELECT_OPTIONS thisSelection = MMSelect_Max;	
static int currentItem = MMSelect_Start;

/* The main multiplayer menu state loop function */
int RunMultiplayerStartUp(int lobbied)
{	
	/* first, run the connection dialog */
	if(!lobbied)
	{
		AvP.Network=I_No_Network;
		InvokeDirectPlayDialog();
		
		if(ProcessDirectPlayDialog()==0)
		{
			/* didn't connect */
			EndAVPNetGame();
			return 0;
		}
	}

	/* we have successfully connected to a game...*/
	InitAVPNetGame();

	/* init the menu items and strings */
	{ 
		int i;
		
		/* players names, types, and ok flags */
		for(i=0;i<NET_MAXPLAYERS;i++)
		{
			MPGameSelect_MenuFItems[(i+1)].StringID = TEXTSTRING_BLANK;
			MPGameSelect_MenuFItems[(i+1)].SecondStringPtr = mPBlankString;
			MPGameSelect_MenuFItems[(i+9)].StringID = TEXTSTRING_BLANK;
			MPGameSelect_MenuFItems[(i+9)].SecondStringPtr = mPBlankString;
			MPGameSelect_MenuFItems[(i+17)].StringID = TEXTSTRING_BLANK;
			MPGameSelect_MenuFItems[(i+17)].SecondStringPtr = mPBlankString;
		}		
	}

	/* init menu system for drawing */
	debounce = 0;
	thisSelection = MMSelect_Max;	
	currentItem = MMSelect_Start;
	Current_Menu = &MPGameSelectMenu;
	{
		int index;
		for (index=0; index<MMSelect_Max; index++) CurrentMenuStatus[index]=0;		
	}
	CurrentMenuStatus[MMSelect_Max]=-1;
	CurrentMenuStatus[currentItem]=1;
	ProcessGraphicForLoading(&MPGameSelectMenu);	

	/* do start-up loop */
	while(netGameData.myGameState==NGS_StartUp)
	{
 		NetCollectMessages();
		DrawMultiStartUpScreen();
		if(netGameData.myGameState != NGS_StartUp) break;
		
		/* we are still in start-up after collecting our messages */
		if(AvP.Network==I_Host) AddNetMsg_GameDescription();
		else AddNetMsg_PlayerDescription();
 		NetSendMessages();	
		DoMultiStartUpSelection();
		if(netGameData.myGameState != NGS_StartUp) break;

		/* we are still in start-up after processing our user inputs */
		/* ... now do some checks */
		if(AvP.Network==I_Peer)
		{
			if(PlayerIdInPlayerList(AVPDPNetID)==NET_IDNOTINPLAYERLIST)
			{
				if(EmptySlotInPlayerList() == NET_NOEMPTYSLOTINPLAYERLIST)
				{
					/* we are not in the list, and there are no free slots: there is no room */
					TransmitPlayerLeavingNetMsg();
					netGameData.myGameState = NGS_Error_GameFull;
				}
			}			
		}
		else
		{
			/* host checks for peer time-out, and game start */
			LOCALASSERT(AvP.Network==I_Host);
			LOCALASSERT(PlayerIdInPlayerList(AVPDPNetID)!=NET_IDNOTINPLAYERLIST);
			
			/* check start flags on all players (including ourselves) */
			{
				int i;
				int startOk = 1;
				for(i=0;i<NET_MAXPLAYERS;i++)
				{
					if(netGameData.playerData[i].playerId != NULL) 
					{
						if(netGameData.playerData[i].startFlag==0) startOk = 0;						
					}
				}
				if(startOk==1)
				{
					TransmitStartGameNetMsg();	
					netGameData.myGameState = NGS_Playing;
				}
			}
		}		
	}

	/* examine our NGS and decide what to do */
	switch(netGameData.myGameState)
	{
		case(NGS_Playing):
		{
			/* everything went ok */
			AvP.StartingEnv = I_Dml1;
			AvP.CurrentEnv = AvP.StartingEnv;
			switch(netGameData.myCharacterType)
			{
				case(NGCT_Marine):
				{
					AvP.PlayerType = I_Marine;
					break;
				}
				case(NGCT_Predator):
				{
					AvP.PlayerType = I_Predator;
					break;
				}
				case(NGCT_Alien):
				{
					AvP.PlayerType = I_Alien;
					break;
				}
				default:
				{
					LOCALASSERT(1==0);
					break;
				}	
			}			
			ProcessGraphicForUnloading(&MPGameSelectMenu);	
			return 1;
			break;
		}
		case(NGS_Leaving):
		case(NGS_EndGame):
		{
			EndAVPNetGame();
			ProcessGraphicForUnloading(&MPGameSelectMenu);	
			return 0;
			break;		
		}
		case(NGS_Error_GameFull):
		case(NGS_Error_GameStarted):
		case(NGS_Error_HostLost):
		{
			EndAVPNetGame();			
			DisplayMultiStartUpErrorMessage();
			ProcessGraphicForUnloading(&MPGameSelectMenu);	
			return 0;
			break;
		}
		default:
		{
			LOCALASSERT(1==0);
			EndAVPNetGame();			
			ProcessGraphicForUnloading(&MPGameSelectMenu);	
			return 0;
			break;
		}
	}
}

static void DrawMultiStartUpScreen(void)
{
	/* update all the menu items... */
	int i;

	for(i=0;i<NET_MAXPLAYERS;i++)
	{
		if(netGameData.playerData[i].playerId!=NULL)
		{
			MPGameSelect_MenuFItems[(i+1)].SecondStringPtr = netGameData.playerData[i].name;

			switch(netGameData.playerData[i].characterType)
			{
				case(NGCT_Marine):
				{
					MPGameSelect_MenuFItems[(i+9)].StringID = TEXTSTRING_MULTI_MARINE;
					break;
				}
				case(NGCT_Predator):
				{
					MPGameSelect_MenuFItems[(i+9)].StringID = TEXTSTRING_MULTI_PREDATOR;
					break;
				}
				case(NGCT_Alien):
				{
					MPGameSelect_MenuFItems[(i+9)].StringID = TEXTSTRING_MULTI_ALIEN;
					break;
				}
				default:
				{
					LOCALASSERT(1==0);
					break;
				}
			}

			if(netGameData.playerData[i].startFlag)
				MPGameSelect_MenuFItems[(i+17)].StringID = TEXTSTRING_MULTI_OK;
			else
				MPGameSelect_MenuFItems[(i+17)].StringID = TEXTSTRING_BLANK;
		}
		else
		{
			MPGameSelect_MenuFItems[(i+1)].SecondStringPtr = mPBlankString;
			MPGameSelect_MenuFItems[(i+9)].StringID = TEXTSTRING_BLANK;
			MPGameSelect_MenuFItems[(i+17)].SecondStringPtr = mPBlankString;
		}
	}
		
	/* my character selection */
	switch(netGameData.myCharacterType)
	{
		case(NGCT_Marine):
		{
			MPGameSelect_MenuFItems[(32)].StringID = TEXTSTRING_MULTI_MARINE;
			break;
		}
		case(NGCT_Predator):
		{
			MPGameSelect_MenuFItems[(32)].StringID = TEXTSTRING_MULTI_PREDATOR;
			break;
		}
		case(NGCT_Alien):
		{
			MPGameSelect_MenuFItems[(32)].StringID = TEXTSTRING_MULTI_ALIEN;
			break;
		}
		default:
		{
			LOCALASSERT(1==0);
			break;
		}
	}
	
	/* level selection (actually doesn't do anything at the moment) */
	MPGameSelect_MenuFItems[33].SecondStringPtr = mPLevelNameString;

	/* mode selection */
	switch(netGameData.gameType)
	{
		case(NGT_Individual):
		{
			MPGameSelect_MenuFItems[34].StringID = TEXTSTRING_MULTI_INDIVIDUAL;
			break;
		}
		case(NGT_Coop):
		{
			MPGameSelect_MenuFItems[34].StringID = TEXTSTRING_MULTI_COOP;
			break;
		}
		default:
		{
			LOCALASSERT(1==0);
			break;
		}	
	}
	
	/* score and time limits */
	if(netGameData.scoreLimit==0)
	{
		MPGameSelect_MenuFItems[35].StringID = TEXTSTRING_NO;
		MPGameSelect_MenuFItems[35].SecondStringPtr = mPBlankString;
	}
	else
	{
		sprintf(mPScoreLimitString,"%.3d",netGameData.scoreLimit);
		MPGameSelect_MenuFItems[35].StringID = TEXTSTRING_BLANK;
		MPGameSelect_MenuFItems[35].SecondStringPtr = mPScoreLimitString;
	}
	if(netGameData.timeLimit==0)
	{
		MPGameSelect_MenuFItems[36].StringID = TEXTSTRING_NO;
		MPGameSelect_MenuFItems[36].SecondStringPtr = mPBlankString;
	}
	else
	{
		sprintf(mPTimeLimitString,"%.3d",netGameData.timeLimit);
		MPGameSelect_MenuFItems[36].StringID = TEXTSTRING_BLANK;
		MPGameSelect_MenuFItems[36].SecondStringPtr = mPTimeLimitString;
	}
	
	/* and draw everything */
	DrawEntireMenuScreen();
}

static void DoMultiStartUpSelection(void)
{
	/* if we have selected start, we can't do any more but wait */
	if(netGameData.myStartFlag==1) return;
	
	ReadUserInput();
	if(IDemandGoBackward() && debounce) 
	{
		debounce=0;
		CurrentMenuStatus[currentItem++]=0;
		/* wrap around */
		if(AvP.Network==I_Host)
		{
			if(currentItem==MMSelect_Title) currentItem=MMSelect_Start;		
		}
		else
		{
			LOCALASSERT(AvP.Network==I_Peer);
			if(currentItem==MMSelect_LevelSelection) currentItem=MMSelect_Start;		
		}
		CurrentMenuStatus[currentItem]=1;
	}
	else if(IDemandGoForward() && debounce) 
	{
		debounce=0;
		CurrentMenuStatus[currentItem--]=0;
		/* wrap around */
		if(AvP.Network==I_Host)
		{
			if(currentItem==-1) currentItem=MMSelect_TimeLimitSelection;		
		}
		else
		{
			LOCALASSERT(AvP.Network==I_Peer);
			if(currentItem==-1) currentItem=MMSelect_CharacterSelection;		
		}
		CurrentMenuStatus[currentItem]=1;
	}
	else if (IDemandSelect() && debounce) 
	{
		debounce=0;
		thisSelection=currentItem;
		switch(thisSelection)
		{
			case(MMSelect_Start):
			{
				netGameData.myStartFlag=1;
				if(AvP.Network==I_Host)
				{
					int myIndex;
					myIndex = PlayerIdInPlayerList(AVPDPNetID);
					LOCALASSERT(myIndex!=NET_IDNOTINPLAYERLIST);
					netGameData.playerData[myIndex].startFlag = 1;
				}
				break;
			}
			case(MMSelect_Cancel):
			{
				if(AvP.Network==I_Host)
				{
					TransmitEndOfGameNetMsg();
					netGameData.myGameState=NGS_EndGame;
				}
				else 
				{
					TransmitPlayerLeavingNetMsg();					
					netGameData.myGameState=NGS_Leaving;
				}
				break;
			}
			case(MMSelect_CharacterSelection):
			{
				if(netGameData.myCharacterType==NGCT_Marine) netGameData.myCharacterType=NGCT_Predator;
				else if(netGameData.myCharacterType==NGCT_Predator) netGameData.myCharacterType=NGCT_Alien;
				else netGameData.myCharacterType=NGCT_Marine;

				if(AvP.Network==I_Host)
				{
					int myIndex;
					myIndex = PlayerIdInPlayerList(AVPDPNetID);
					LOCALASSERT(myIndex!=NET_IDNOTINPLAYERLIST);
					netGameData.playerData[myIndex].characterType = netGameData.myCharacterType;
				}
				break;
			}
			case(MMSelect_LevelSelection):
			{
				LOCALASSERT(AvP.Network==I_Host);
				/* do nothing at the moment */
				break;
			}
			case(MMSelect_ModeSelection):
			{
				LOCALASSERT(AvP.Network==I_Host);
				if(netGameData.gameType==NGT_Individual) netGameData.gameType=NGT_Coop;
				else netGameData.gameType=NGT_Individual;
				break;
			}
			case(MMSelect_ScoreLimitSelection):
			{
				LOCALASSERT(AvP.Network==I_Host);
				if(netGameData.scoreLimit>=NET_MAXPLAYERSCORE) netGameData.scoreLimit=0;				
				else netGameData.scoreLimit+=5;
				break;
			}
			case(MMSelect_TimeLimitSelection):
			{
				LOCALASSERT(AvP.Network==I_Host);
				if(netGameData.timeLimit>=NET_MAXGAMETIME) netGameData.timeLimit=0;						
				else netGameData.timeLimit+=5;
				break;
			}
			default:
			{
				LOCALASSERT(1==0);
				break;
			}
		}
	}
	else if((IDemandGoForward()==0)&&(IDemandGoBackward()==0)&&(IDemandSelect()==0)) 
		debounce =1;
}

/* Patrick 19/7/97---------------------------------------------------
function for displaying multiplayer startup error messages :
adjusts existing menu...
---------------------------------------------------------------------*/
static MENU_TEXT_ITEM MPStartUpError_MenuFItems[] = 
{
	{MMSelect_Title, TEXTSTRING_MULTI_TITLE, NULL,  80, 40},
	{MMSelect_Start, TEXTSTRING_BLANK, NULL, 80, 80},
	{MMSelect_Cancel, TEXTSTRING_MULTI_OK, NULL, 80, 120},
	{-1,}
};
static AVP_MENU MPStartUpErrorMenu = 
{
	&MPGameSelect_Backdrop,
	&MPGameSelect_MenuItems[0],
	&MPStartUpError_MenuFItems[0],
	-1,
	0,
};


static void DisplayMultiStartUpErrorMessage(void)
{
	/* examine our NGS and decide what message to display */
	switch(netGameData.myGameState)
	{
		case(NGS_Error_GameFull):
		{
			MPStartUpError_MenuFItems[1].StringID = TEXTSTRING_MULTI_GAMEFULL;
			break;
		}
		case(NGS_Error_GameStarted):
		{
			MPStartUpError_MenuFItems[1].StringID = TEXTSTRING_MULTI_ALREADYSTARTED;
			break;
		}
		case(NGS_Error_HostLost):
		{
			MPStartUpError_MenuFItems[1].StringID = TEXTSTRING_MULTI_CONNECTIONLOST;
			break;
		}
		default:
		{
			LOCALASSERT(1==0);
			return;
			break;
		}
	}

	thisSelection = MMSelect_Max;	
	currentItem = MMSelect_Cancel;
	Current_Menu = &MPStartUpErrorMenu;
	{
		int index;
		for (index=0; index<MMSelect_Max; index++) CurrentMenuStatus[index]=0;		
	}
	CurrentMenuStatus[MMSelect_Max]=-1;
	CurrentMenuStatus[currentItem]=1;

	while(!IDemandSelect()) 
	{
		DrawEntireMenuScreen();
		ReadUserInput();
	}
}

/*---------------------------Patrick 5/5/97----------------------------
  These functions support the end of net-game information screen
  ----------------------------------------------------------------------*/
MENUGRAPHIC EndNetGame_Backdrop = { "menugfx\\pg0.pg0", -1, NULL, NULL, 0, 0, 640, 480,};
static MENU_GRAPHIC_ITEM EndNetGame_MenuItems[] = {{-1,}};
static MENU_TEXT_ITEM EndNetGame_MenuFItems[] =
{
	{MMEndItem_PlayerName1, TEXTSTRING_BLANK, mPBlankString, 60, 40},
	{MMEndItem_PlayerName2, TEXTSTRING_BLANK, mPBlankString, 60, 60},
	{MMEndItem_PlayerName3, TEXTSTRING_BLANK, mPBlankString, 60, 80},
	{MMEndItem_PlayerName4, TEXTSTRING_BLANK, mPBlankString, 60, 100},
	{MMEndItem_PlayerName5, TEXTSTRING_BLANK, mPBlankString, 60, 120},
	{MMEndItem_PlayerName6, TEXTSTRING_BLANK, mPBlankString, 60, 140},
	{MMEndItem_PlayerName7, TEXTSTRING_BLANK, mPBlankString, 60, 160},
	{MMEndItem_PlayerName8, TEXTSTRING_BLANK, mPBlankString, 60, 180},
	{MMEndItem_PlayerCharacter1, TEXTSTRING_BLANK, mPBlankString, 220, 40},
	{MMEndItem_PlayerCharacter2, TEXTSTRING_BLANK, mPBlankString, 220, 60},
	{MMEndItem_PlayerCharacter3, TEXTSTRING_BLANK, mPBlankString, 220, 80},
	{MMEndItem_PlayerCharacter4, TEXTSTRING_BLANK, mPBlankString, 220, 100},
	{MMEndItem_PlayerCharacter5, TEXTSTRING_BLANK, mPBlankString, 220, 120},
	{MMEndItem_PlayerCharacter6, TEXTSTRING_BLANK, mPBlankString, 220, 140},
	{MMEndItem_PlayerCharacter7, TEXTSTRING_BLANK, mPBlankString, 220, 160},
	{MMEndItem_PlayerCharacter8, TEXTSTRING_BLANK, mPBlankString, 220, 180},
	{MMEndItem_PlayerTotalScore1, TEXTSTRING_BLANK, mPBlankString, 380, 40},
	{MMEndItem_PlayerTotalScore2, TEXTSTRING_BLANK, mPBlankString, 380, 60},
	{MMEndItem_PlayerTotalScore3, TEXTSTRING_BLANK, mPBlankString, 380, 80},
	{MMEndItem_PlayerTotalScore4, TEXTSTRING_BLANK, mPBlankString, 380, 100},
	{MMEndItem_PlayerTotalScore5, TEXTSTRING_BLANK, mPBlankString, 380, 120},
	{MMEndItem_PlayerTotalScore6, TEXTSTRING_BLANK, mPBlankString, 380, 140},
	{MMEndItem_PlayerTotalScore7, TEXTSTRING_BLANK, mPBlankString, 380, 160},
	{MMEndItem_PlayerTotalScore8, TEXTSTRING_BLANK, mPBlankString, 380, 180},
	{MMEndItem_Player1Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 240},
	{MMEndItem_Player2Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 260},
	{MMEndItem_Player3Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 280},
	{MMEndItem_Player4Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 300},
	{MMEndItem_Player5Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 320},
	{MMEndItem_Player6Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 340},
	{MMEndItem_Player7Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 360},
	{MMEndItem_Player8Initial1, TEXTSTRING_BLANK, mPBlankString, 60, 380},
	{MMEndItem_Player1Initial2, TEXTSTRING_BLANK, mPBlankString, 110, 220},
	{MMEndItem_Player2Initial2, TEXTSTRING_BLANK, mPBlankString, 170, 220},
	{MMEndItem_Player3Initial2, TEXTSTRING_BLANK, mPBlankString, 230, 220},
	{MMEndItem_Player4Initial2, TEXTSTRING_BLANK, mPBlankString, 290, 220},
	{MMEndItem_Player5Initial2, TEXTSTRING_BLANK, mPBlankString, 350, 220},
	{MMEndItem_Player6Initial2, TEXTSTRING_BLANK, mPBlankString, 410, 220},
	{MMEndItem_Player7Initial2, TEXTSTRING_BLANK, mPBlankString, 470, 220},
	{MMEndItem_Player8Initial2, TEXTSTRING_BLANK, mPBlankString, 530, 220},
	{MMEndItem_Player1Score1, TEXTSTRING_BLANK, mPBlankString, 100, 240},
	{MMEndItem_Player1Score2, TEXTSTRING_BLANK, mPBlankString, 160, 240},
	{MMEndItem_Player1Score3, TEXTSTRING_BLANK, mPBlankString, 220, 240},
	{MMEndItem_Player1Score4, TEXTSTRING_BLANK, mPBlankString, 280, 240},
	{MMEndItem_Player1Score5, TEXTSTRING_BLANK, mPBlankString, 340, 240},
	{MMEndItem_Player1Score6, TEXTSTRING_BLANK, mPBlankString, 400, 240},
	{MMEndItem_Player1Score7, TEXTSTRING_BLANK, mPBlankString, 460, 240},
	{MMEndItem_Player1Score8, TEXTSTRING_BLANK, mPBlankString, 520, 240},
	{MMEndItem_Player2Score1, TEXTSTRING_BLANK, mPBlankString, 100, 260},
	{MMEndItem_Player2Score2, TEXTSTRING_BLANK, mPBlankString, 160, 260},
	{MMEndItem_Player2Score3, TEXTSTRING_BLANK, mPBlankString, 220, 260},
	{MMEndItem_Player2Score4, TEXTSTRING_BLANK, mPBlankString, 280, 260},
	{MMEndItem_Player2Score5, TEXTSTRING_BLANK, mPBlankString, 340, 260},
	{MMEndItem_Player2Score6, TEXTSTRING_BLANK, mPBlankString, 400, 260},
	{MMEndItem_Player2Score7, TEXTSTRING_BLANK, mPBlankString, 460, 260},
	{MMEndItem_Player2Score8, TEXTSTRING_BLANK, mPBlankString, 520, 260},
	{MMEndItem_Player3Score1, TEXTSTRING_BLANK, mPBlankString, 100, 280},
	{MMEndItem_Player3Score2, TEXTSTRING_BLANK, mPBlankString, 160, 280},
	{MMEndItem_Player3Score3, TEXTSTRING_BLANK, mPBlankString, 220, 280},
	{MMEndItem_Player3Score4, TEXTSTRING_BLANK, mPBlankString, 280, 280},
	{MMEndItem_Player3Score5, TEXTSTRING_BLANK, mPBlankString, 340, 280},
	{MMEndItem_Player3Score6, TEXTSTRING_BLANK, mPBlankString, 400, 280},
	{MMEndItem_Player3Score7, TEXTSTRING_BLANK, mPBlankString, 460, 280},
	{MMEndItem_Player3Score8, TEXTSTRING_BLANK, mPBlankString, 520, 280},
	{MMEndItem_Player4Score1, TEXTSTRING_BLANK, mPBlankString, 100, 300},
	{MMEndItem_Player4Score2, TEXTSTRING_BLANK, mPBlankString, 160, 300},
	{MMEndItem_Player4Score3, TEXTSTRING_BLANK, mPBlankString, 220, 300},
	{MMEndItem_Player4Score4, TEXTSTRING_BLANK, mPBlankString, 280, 300},
	{MMEndItem_Player4Score5, TEXTSTRING_BLANK, mPBlankString, 340, 300},
	{MMEndItem_Player4Score6, TEXTSTRING_BLANK, mPBlankString, 400, 300},
	{MMEndItem_Player4Score7, TEXTSTRING_BLANK, mPBlankString, 460, 300},
	{MMEndItem_Player4Score8, TEXTSTRING_BLANK, mPBlankString, 520, 300},
	{MMEndItem_Player5Score1, TEXTSTRING_BLANK, mPBlankString, 100, 320},
	{MMEndItem_Player5Score2, TEXTSTRING_BLANK, mPBlankString, 160, 320},
	{MMEndItem_Player5Score3, TEXTSTRING_BLANK, mPBlankString, 220, 320},
	{MMEndItem_Player5Score4, TEXTSTRING_BLANK, mPBlankString, 280, 320},
	{MMEndItem_Player5Score5, TEXTSTRING_BLANK, mPBlankString, 340, 320},
	{MMEndItem_Player5Score6, TEXTSTRING_BLANK, mPBlankString, 400, 320},
	{MMEndItem_Player5Score7, TEXTSTRING_BLANK, mPBlankString, 460, 320},
	{MMEndItem_Player5Score8, TEXTSTRING_BLANK, mPBlankString, 520, 320},
	{MMEndItem_Player6Score1, TEXTSTRING_BLANK, mPBlankString, 100, 340},
	{MMEndItem_Player6Score2, TEXTSTRING_BLANK, mPBlankString, 160, 340},
	{MMEndItem_Player6Score3, TEXTSTRING_BLANK, mPBlankString, 220, 340},
	{MMEndItem_Player6Score4, TEXTSTRING_BLANK, mPBlankString, 280, 340},
	{MMEndItem_Player6Score5, TEXTSTRING_BLANK, mPBlankString, 340, 340},
	{MMEndItem_Player6Score6, TEXTSTRING_BLANK, mPBlankString, 400, 340},
	{MMEndItem_Player6Score7, TEXTSTRING_BLANK, mPBlankString, 460, 340},
	{MMEndItem_Player6Score8, TEXTSTRING_BLANK, mPBlankString, 520, 340},
	{MMEndItem_Player7Score1, TEXTSTRING_BLANK, mPBlankString, 100, 360},
	{MMEndItem_Player7Score2, TEXTSTRING_BLANK, mPBlankString, 160, 360},
	{MMEndItem_Player7Score3, TEXTSTRING_BLANK, mPBlankString, 220, 360},
	{MMEndItem_Player7Score4, TEXTSTRING_BLANK, mPBlankString, 280, 360},
	{MMEndItem_Player7Score5, TEXTSTRING_BLANK, mPBlankString, 340, 360},
	{MMEndItem_Player7Score6, TEXTSTRING_BLANK, mPBlankString, 400, 360},
	{MMEndItem_Player7Score7, TEXTSTRING_BLANK, mPBlankString, 460, 360},
	{MMEndItem_Player7Score8, TEXTSTRING_BLANK, mPBlankString, 520, 360},
	{MMEndItem_Player8Score1, TEXTSTRING_BLANK, mPBlankString, 100, 380},
	{MMEndItem_Player8Score2, TEXTSTRING_BLANK, mPBlankString, 160, 380},
	{MMEndItem_Player8Score3, TEXTSTRING_BLANK, mPBlankString, 220, 380},
	{MMEndItem_Player8Score4, TEXTSTRING_BLANK, mPBlankString, 280, 380},
	{MMEndItem_Player8Score5, TEXTSTRING_BLANK, mPBlankString, 340, 380},
	{MMEndItem_Player8Score6, TEXTSTRING_BLANK, mPBlankString, 400, 380},
	{MMEndItem_Player8Score7, TEXTSTRING_BLANK, mPBlankString, 460, 380},
	{MMEndItem_Player8Score8, TEXTSTRING_BLANK, mPBlankString, 520, 380},
	{MMEndItem_Ok, TEXTSTRING_MULTI_OK, NULL, 60, 420},
	{-1,}
};


static AVP_MENU	EndNetGameMenu =
{
	&EndNetGame_Backdrop,
	&EndNetGame_MenuItems[0],
	&EndNetGame_MenuFItems[0],
	-1,
	0,
};

static char scoreStringsTotal[NET_MAXPLAYERS][4];
static char initialString[NET_MAXPLAYERS][2];
static char scoreStrings[NET_MAXPLAYERS][NET_MAXPLAYERS][4];

void EndOfNetworkGameScreen(void)
{
	extern int VideoMode;
	extern void (*UpdateScreen[]) (void);	
		
 	PlatformSpecificEnteringMenus();
	Current_Menu = &EndNetGameMenu;
	ProcessGraphicForLoading(&EndNetGameMenu);	
	thisSelection = MMEndItem_Max;	
	currentItem = MMEndItem_Ok;

	{
		int index;
		for (index=0; index<MMEndItem_Max; index++) CurrentMenuStatus[index]=0;		
	}
	CurrentMenuStatus[MMEndItem_Max]=-1;
	CurrentMenuStatus[currentItem]=1;

	/* set up names, etc, from the gamedata */
	{
		int i;
		for(i=0;i<NET_MAXPLAYERS;i++)
		{
			if(netGameData.playerData[i].playerId)
			{
				/* name... */
				EndNetGame_MenuFItems[i].SecondStringPtr = netGameData.playerData[i].name;
				
				/* character type... */
				switch(netGameData.playerData[i].characterType)
				{
					case(NGCT_Marine):
					{
						EndNetGame_MenuFItems[(i+NET_MAXPLAYERS)].StringID = TEXTSTRING_MULTI_MARINE;
						break;
					}
					case(NGCT_Alien):
					{
						EndNetGame_MenuFItems[(i+NET_MAXPLAYERS)].StringID = TEXTSTRING_MULTI_ALIEN;
						break;
					}
					case(NGCT_Predator):
					{
						EndNetGame_MenuFItems[(i+NET_MAXPLAYERS)].StringID = TEXTSTRING_MULTI_PREDATOR;
						break;
					}
					default:
					{
						LOCALASSERT(1==0);
						break;
					}
				}
				
				/* the total score... */
				{
					int totalScore = AddUpPlayerScore(i);
					if(totalScore>=0) sprintf(scoreStringsTotal[i],"%.3d",totalScore);
					else if(totalScore>=(-99)) sprintf(scoreStringsTotal[i],"%.2d",totalScore);
					else sprintf(scoreStringsTotal[i],"%.2d",(-99));
				}
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*2))].SecondStringPtr = scoreStringsTotal[i];
			
				/* initials for score table ... */
				initialString[i][0] = netGameData.playerData[i].name[0];
				initialString[i][1] = '\0';
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*3))].SecondStringPtr = initialString[i];
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*4))].SecondStringPtr = initialString[i];

				/* score: player by player... */
				{
					int j;
					for(j=0;j<NET_MAXPLAYERS;j++)
					{
						if(netGameData.playerData[j].playerId)
						{
							int thisScore;
							
							thisScore = netGameData.playerData[i].scores[j];
							if(thisScore>=0) sprintf(scoreStrings[i][j],"%.3d",thisScore);
							else if(thisScore>=(-99)) sprintf(scoreStrings[i][j],"%.2d",thisScore);
							else sprintf(scoreStrings[i][j],"%.2d",thisScore);							
							EndNetGame_MenuFItems[(j+(NET_MAXPLAYERS*(5+i)))].SecondStringPtr = scoreStrings[i][j];
						}
						else
						{
							EndNetGame_MenuFItems[(j+(NET_MAXPLAYERS*(5+i)))].SecondStringPtr = mPBlankString;
						}
					}				
				}
			}
			else
			{
				/* make sure it's blank... incase this screen has been displayed previously */
				EndNetGame_MenuFItems[i].SecondStringPtr = mPBlankString;
				EndNetGame_MenuFItems[(i+NET_MAXPLAYERS)].StringID = TEXTSTRING_BLANK;
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*2))].SecondStringPtr = mPBlankString;
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*3))].SecondStringPtr = mPBlankString;
				EndNetGame_MenuFItems[(i+(NET_MAXPLAYERS*4))].SecondStringPtr = mPBlankString;			
				{
					int j;
					for(j=0;j<NET_MAXPLAYERS;j++)
					{
						EndNetGame_MenuFItems[(j+(NET_MAXPLAYERS*(5+i)))].SecondStringPtr = mPBlankString;
					}				
				}
			}
		}
	}

	/* wait until the user presses return */
	while(!IDemandSelect()) 
	{	
		DrawEntireMenuScreen();
		ReadUserInput();
	}	
	/* clean up */
	ProcessGraphicForUnloading(&EndNetGameMenu);
	PlatformSpecificExitingMenus();
}
















