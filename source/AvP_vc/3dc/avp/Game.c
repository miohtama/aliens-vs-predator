#include "3dc.h"
#include <math.h>
#include "inline.h"
#include "module.h"

#include "stratdef.h"
#include "gamedef.h"
#include "dynblock.h"
#include "dynamics.h"

#include "bh_types.h"
#include "bh_alien.h"
#include "pheromon.h"
#include "pfarlocs.h"
#include "bh_gener.h"
#include "pvisible.h"
#include "lighting.h"
#include "bh_pred.h"
#include "bh_lift.h"
#include "avpview.h"
#include "psnd.h"
#include "psndplat.h"
#include "particle.h"
#include "sfx.h"
#include "version.h"
#include "bh_RubberDuck.h"
#include "bh_marin.h"
#include "dxlog.h"
#include "avp_menus.h"
#include "avp_userprofile.h"
#include "davehook.h"
#include "CDTrackSelection.h"
#include "savegame.h"
	// Added 18/11/97 by DHM: all hooks for my code

#define UseLocalAssert Yes
#include "ourasert.h"

/* KJL 09:47:33 03/19/97 - vision stuff for marine and predator.
Currently PC only because it will probably be implemented in a completely
different way on the consoles, so I won't worry the PSX guys for now.
*/
#if SupportWindows95

#include "vision.h"

#include "cheat.h"	
#include "pldnet.h"								 

#endif

#if SupportWindows95 || Saturn
#include "kshape.h"
#include "krender.h"
#endif

/* KJL 16:00:13 11/22/96 - One of my evil experiments....   */
#define PENTIUM_PROFILING_ON 0

#define PROFILING_ON 0

#if PENTIUM_PROFILING_ON
#include "pentime.h"
#else
#if SupportWindows95
#define gProfileStart();
#define ProfileStop(x);
#endif
#endif

#define	VERSION_DisableStartupMenus 	Yes
#define VERSION_DisableStartupCredits	Yes
#include "avp_menus.h"

/******************
Extern Engine Varibles
******************/

extern void (*UpdateScreen[]) (void);
extern int VideoMode;

#if PSX
#else
extern void (*SetVideoMode[]) (void);
#endif

extern int FrameRate;
extern int NormalFrameTime;
extern int FrameRate;

extern int HWAccel;
extern int Resolution;

unsigned char Null_Name[8];

extern int PlaySounds;

/*******************************
EXPORTED GLOBALS
*******************************/


AVP_GAME_DESC AvP;		 /* game description */

char projectsubdirectory[] = {"avp/"};
int SavedFrameRate;
int SavedNormalFrameTime;


/* Andy 13/10/97

   This global is set by any initialisation routine if a call to AllocateMem fails.
   It can be checked during debugging after all game and level initialisation to see
   if we have run out of memory.
*/
int memoryInitialisationFailure = 0;


/* start inits for the game*/

void ProcessSystemObjects();

void LevelSpecificChecks(void);

/*runtime maintainance*/

void FindObjectOfFocus();
void MaintainPlayer(void);

extern void CheckCDStatus(void);

/*********************************************

Init Game and Start Game

*********************************************/


void InitGame(void)
{
	/*
		RWH 
		InitGame is to be used only to set platform independent
		varibles. It will be called ONCE only by the game
	*/

	/***** Set up default game settings*/

	AvP.Language = I_English;
	AvP.GameMode = I_GM_Playing;
	AvP.Network = I_No_Network;
	AvP.Difficulty = I_Medium;

 	// Modified by Edmond for Mplayer demo
 	#ifdef MPLAYER_DEMO
 		AvP.StartingEnv = I_Dml1;
 	#else
 		AvP.StartingEnv = I_Entrance;
 	#endif
	AvP.CurrentEnv = AvP.StartingEnv;
	AvP.PlayerType = I_Marine;

#if SupportWindows95

	AvP.GameVideoRequestMode = VideoMode_DX_320x200x8; /* ignored */
	if(HWAccel)
		AvP.MenuVideoRequestMode = VideoMode_DX_640x480x15;
	else
		AvP.MenuVideoRequestMode = VideoMode_DX_640x480x8;

#endif

	AvP.ElapsedSeconds = 0;
	AvP.ElapsedMinutes = 0;
	AvP.ElapsedHours = 0;
	
	AvP.NetworkAIServer = 0;


	// Added by DHM 18/11/97: Hook for my initialisation code:
	DAVEHOOK_Init();

	/* KJL 15:17:35 28/01/98 - Initialise console variables */
	{
		extern void CreateGameSpecificConsoleVariables(void);
		extern void CreateGameSpecificConsoleCommands(void);

		extern void CreateMoreGameSpecificConsoleVariables(void);

		/* KJL 12:03:18 30/01/98 - Init console variables and commands */
		CreateGameSpecificConsoleVariables();
		CreateGameSpecificConsoleCommands();
		
		/* Next one is CDF's */
		CreateMoreGameSpecificConsoleVariables();
	}
	#if DEATHMATCH_DEMO
	SetToMinimalDetailLevels();
	#else
	SetToDefaultDetailLevels();
	#endif
}

extern void create_strategies_from_list ();
extern void AssignAllSBNames();
extern BOOL Current_Level_Requires_Mirror_Image();

void StartGame(void)
{
	/* called whenever we start a game (NOT when we change */
	/* environments - destroy anything from a previous game*/

	/*
	Temporarily disable sounds while loading. Largely to avoid 
	some irritating teletext sounds starting up
	*/
	int playSoundsStore=PlaySounds;
	PlaySounds=0;

	//get the cd to start again at the beginning of the play list.
	ResetCDPlayForLevel();
	
	
	ProcessSystemObjects();
	
	create_strategies_from_list ();
	AssignAllSBNames();
	
	SetupVision();
	/*-------------- Patrick 11/1/97 ----------------
	  Initialise visibility system and NPC behaviour 
	  systems for new level.	  
	  -----------------------------------------------*/
	
	InitObjectVisibilities();
	InitPheromoneSystem();
	BuildFarModuleLocs();
	InitHive();
	InitSquad();

	InitialiseParticleSystem();
	InitialiseSfxBlocks();
	InitialiseLightElementSystem();

	#if PSX
	{
	extern int RedOut;
	RedOut=0;
	}
	#endif
	AvP.DestructTimer=-1;

	// DHM 18/11/97: I've put hooks for screen mode changes here for the moment:
	DAVEHOOK_ScreenModeChange_Setup();
	DAVEHOOK_ScreenModeChange_Cleanup();

	/* KJL 11:46:42 30/03/98 - I thought it'd be nice to display the version details
	when you start a game */
	#if PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO
	#else
//	GiveVersionDetails();
	#endif

	#if MIRRORING_ON
	if(Current_Level_Requires_Mirror_Image())
	{
		CreatePlayersImageInMirror();
	}
	#endif

	/* KJL 16:13:30 01/05/98 - rubber ducks! */
	CreateRubberDucks();
	
	CheckCDStatus();

	{
		extern int LeanScale;
		if (AvP.PlayerType==I_Alien)
		{
			LeanScale=ONE_FIXED*3;
		}
		else
		{
			LeanScale=ONE_FIXED;
		}
	}
	{
		extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
		extern int MotionTrackerScale;
		MotionTrackerScale = DIV_FIXED(ScreenDescriptorBlock.SDB_Width,640);
	}
 //	BuildInvSqrtTable();

	InitialiseTriggeredFMVs();
	CreateStarArray();

	{
		//check the containing modules for preplaced decals
		void check_preplaced_decal_modules();
		check_preplaced_decal_modules();
	}

	CurrentGameStats_Initialise();
	MessageHistory_Initialise();

	if (DISCOINFERNO_CHEATMODE || TRIPTASTIC_CHEATMODE)
	{
		MakeLightElement(&Player->ObWorld,LIGHTELEMENT_ROTATING);
	}

	//restore the play sounds setting
	PlaySounds=playSoundsStore;

  	//make sure the visibilities are up to date
  	Global_VDB_Ptr->VDB_World = Player->ObWorld;
  	AllNewModuleHandler();
  	DoObjectVisibilities();
}


#define FIXED_MINUTE ONE_FIXED*60

void DealWithElapsedTime()
{
	AvP.ElapsedSeconds += NormalFrameTime;
	
	if(AvP.ElapsedSeconds  >= FIXED_MINUTE)
	{
		AvP.ElapsedSeconds -= FIXED_MINUTE;
		AvP.ElapsedMinutes ++;
	}
	
	if(AvP.ElapsedMinutes >= 60)
	{
		AvP.ElapsedMinutes -= 60;
		AvP.ElapsedHours ++;
	}		
}
	


/**********************************************

 Main Loop Game Functions

**********************************************/
void UpdateGame(void)
{
	/* Read Keyboard, Keypad, Joystick etc. */
	ReadUserInput();

	/* DHM 18/11/97: hook for my code */
	#if PENTIUM_PROFILING_ON
	ProfileStart();
	#endif	
	DAVEHOOK_Maintain();
	#if PENTIUM_PROFILING_ON
	ProfileStop("DAEMON");
	#endif

	/*-------------- Patrick 14/11/96 ----------------
	 call the pheronome system maintainence functions
	-------------------------------------------------*/
	#if PENTIUM_PROFILING_ON
	ProfileStart();
	#endif	
	PlayerPheromoneSystem();
	AiPheromoneSystem();
	#if PENTIUM_PROFILING_ON
	ProfileStop("PHEROMONE");
	#endif	

	/*-------------- Patrick 11/1/97 ----------------
	Call the alien hive management function
	-------------------------------------------------*/
	DoHive();
	DoSquad();
	

   	#if PROFILING_ON
	ProfileStart();
	#endif
 	ObjectBehaviours();
	#if PROFILING_ON 
	ProfileStop("BEHAVS");
	#endif	
	 
	/* KJL 10:32:55 09/24/96 - update player */
	#if PENTIUM_PROFILING_ON 
	ProfileStart();
	#endif
	MaintainPlayer();
	#if PENTIUM_PROFILING_ON 
	ProfileStop("MNT PLYR");
	#endif

	/* KJL 12:54:08 21/04/98 - make sure the player's matrix is always normalised */
	#if PENTIUM_PROFILING_ON
	ProfileStart();
	#endif	
	MNormalise(&(Player->ObStrategyBlock->DynPtr->OrientMat));
	#if PENTIUM_PROFILING_ON
	ProfileStop("MNorm");
	#endif

	/* netgame support: it seems necessary to collect all our messages here, as some
	things depend on the player's behaviour running before anything else... 
	including firing the player's weapon */
	#if SupportWindows95
	if(AvP.Network != I_No_Network)	NetCollectMessages();
	#endif

	RemoveDestroyedStrategyBlocks();

	{

		if(SaveGameRequest != SAVELOAD_REQUEST_NONE)
		{
			SaveGame();
		}
		else if(LoadGameRequest != SAVELOAD_REQUEST_NONE)
		{
			LoadSavedGame();
		}
	}
	
	#if PENTIUM_PROFILING_ON 
  	ProfileStart();
	#endif
	ObjectDynamics();
	#if PENTIUM_PROFILING_ON 
	ProfileStop("DYNAMICS");
	#endif

	// now for the env teleports
	
	if(RequestEnvChangeViaLift)
	{
		CleanUpLiftControl();								
 	}	

	#if 0
	Player->ObStrategyBlock->DynPtr->Position.vx = -71893;
	Player->ObStrategyBlock->DynPtr->Position.vy = 36000;
	Player->ObStrategyBlock->DynPtr->Position.vz = -52249;
	Player->ObWorld.vx = -71893;
	Player->ObWorld.vy = 36000;
	Player->ObWorld.vz = -42249;
	#endif
	/* netgame support */
	#if SupportWindows95
	if(AvP.Network != I_No_Network)	NetSendMessages();

	/* KJL 11:50:18 03/21/97 - cheat modes */
	HandleCheatModes();
	#endif
	
	#if PSX
	HandleCheatModes();
	#endif

	/*------------Patrick 1/6/97---------------
	New sound system 
	-------------------------------------------*/
	#if PENTIUM_PROFILING_ON 
  	ProfileStart();
	#endif
	
	if(playerPherModule)
	{
		PlatSetEnviroment(playerPherModule->m_sound_env_index,playerPherModule->m_sound_reverb);		
	}
	SoundSys_Management();
	DoPlayerSounds();
	#if PENTIUM_PROFILING_ON 
  	ProfileStop("SOUND SYS");
	#endif
 
	LevelSpecificChecks();
//	NormaliseTest();
	MessageHistory_Maintain();

	if(AvP.LevelCompleted)
	{
		/*
		If player is dead and has also completed level , then cancel
		level completion.
		*/
		PLAYER_STATUS* PlayerStatusPtr = (PLAYER_STATUS*) Player->ObStrategyBlock->SBdataptr;
		if(!PlayerStatusPtr->IsAlive)
		{
			AvP.LevelCompleted=0;
		}
	}

	if (TRIPTASTIC_CHEATMODE)
	{
		extern int TripTasticPhase;
		extern int CloakingPhase;
	 	int a = GetSin(CloakingPhase&4095);
	 	TripTasticPhase = MUL_FIXED(MUL_FIXED(a,a),128)+64;
	}
	if (JOHNWOO_CHEATMODE)
	{
		extern int LeanScale;
		extern int TimeScale;
		TimeScaleThingy();
		
		//in john woo mode leanscale is dependent on the TimeScale
		if (AvP.PlayerType==I_Alien)
		{
			LeanScale=ONE_FIXED*3;
		}
		else
		{
			LeanScale=ONE_FIXED;
		}

		LeanScale+=(ONE_FIXED-TimeScale)*5;
		
	}
	

}





/* MODULE CALL BACK FUNCTIONS .... */



void ModuleObjectJustAllocated(MODULE *mptr)
{
}


void ModuleObjectAboutToBeDeallocated(MODULE *mptr)
{
}


void NewAndOldModules(int num_new, MODULE **m_new, int num_old, MODULE **m_old, char *m_currvis)
{
	
	/* this is the important bit */
	DoObjectVisibilities();

}


void LevelSpecificChecks(void)
{
/* ahem, level specific hacks might be more accurate */
}

extern void CheckCDStatus(void)
{
	#if PREDATOR_DEMO||MARINE_DEMO||ALIEN_DEMO
//	CDCommand_PlayLoop(2);
	#endif	
}



void TimeStampedMessage(char *stringPtr)
{
#if 0
	static int time=0;
	int t=timeGetTime();
	LOGDXFMT(("%s %fs\n",stringPtr,(float)(t-time)/1000.0f ));
	time = t;
#endif
}


#if 0
/* KJL 14:24:34 01/05/98 - Interesting floating point experiments
(UpdateGame is a useful point at which to test things) */

#define IEEE_MANT_BITS	23
#define IEEE_EXP_BITS	8
#define IEEE_SIGN_BITS	1
#define IEEE_EXP_BIAS	127

#define INVSQRT_TABLE_SEED_MANT_BITS	9
#define INVSQRT_TABLE_SEED_EXP_BITS		1
#define INVSQRT_TABLE_LENGTH_BITS		(INVSQRT_TABLE_SEED_MANT_BITS + INVSQRT_TABLE_SEED_EXP_BITS)
#define INVSQRT_TABLE_NUM_ENTRIES		(1 << INVSQRT_TABLE_LENGTH_BITS)
#define INVSQRT_TABLE_ENTRY_BITS		10

#define EXP_OF(x)	(*(DWORD *)&(x) & 0x7f800000)


typedef struct _tab_in
{
unsigned int mpad:			((IEEE_MANT_BITS + 1) - INVSQRT_TABLE_LENGTH_BITS);
unsigned int lookup:		INVSQRT_TABLE_LENGTH_BITS;
unsigned int epad:			7;
unsigned int spad:			1;
} tab_in;

typedef struct _tab_out
{
unsigned int mpad:			(IEEE_MANT_BITS - INVSQRT_TABLE_ENTRY_BITS);
unsigned int lookup:		INVSQRT_TABLE_ENTRY_BITS;
unsigned int epad:			8;
unsigned int spad:			1;
} tab_out;


union myfp
{
float fp;

// used to build the lookup table
tab_in	tab_in_;
tab_out	tab_out_;
};


unsigned int InvSqrtTab[INVSQRT_TABLE_NUM_ENTRIES];


void
BuildInvSqrtTable()
{
	static int done = FALSE;
	int i;

	if (done) return;
	done = TRUE;

	for (i = 0; i < INVSQRT_TABLE_NUM_ENTRIES; i++)
	{
		union myfp fin, fout;

		fin.fp = 1.0F;
		fin.tab_in_.lookup = i;

		// calculate the real value
		fout.fp = 1.0F / (float)sqrt((double)fin.fp);

		// Add the value to the table.  1.0 is special.
		if (fout.fp == 1.0F)
		InvSqrtTab[i] = 0x3FF << (IEEE_MANT_BITS - INVSQRT_TABLE_ENTRY_BITS);
		else
		InvSqrtTab[i] = fout.tab_out_.lookup << (IEEE_MANT_BITS -
		INVSQRT_TABLE_ENTRY_BITS);
	}
} // BuildInvSqrtTable()


float __stdcall
InverseSquareRoot(float x)
{
unsigned int index;
float r;
DWORD *dptr = (DWORD *)&r;

*(DWORD *)&r = ((((3 * IEEE_EXP_BIAS - 1) << IEEE_MANT_BITS) - EXP_OF(x)) >>
1) & 0x7f800000;

index = ((*(DWORD *)&x) >> (IEEE_MANT_BITS - INVSQRT_TABLE_ENTRY_BITS + 1))
& (INVSQRT_TABLE_NUM_ENTRIES - 1);

*dptr |= InvSqrtTab[index];

return r;
} // InverseSquareRoot()


void NormaliseTest(void)
{
	int i;
	float d;
	int outside;
	#if 0
	i = 10000;
	outside = 0;
	ProfileStart();
	while(i--)
	{
		VECTORCH v;
		v.vx = (FastRandom()&65535)+1;
		v.vy = (FastRandom()&65535)+1;
		v.vz = (FastRandom()&65535)+1;

		Normalise(&v);
		{
			int m = Magnitude(&v);
			if (m<65530 || m>65540)
				outside++;
		}
	}
	ProfileStop("OLD NORM");
	textprint("Outside Range: %d\n",outside);

	i = 10000;
	outside = 0;
	ProfileStart();
	while(i--)
	{
		VECTORCH v;
		v.vx = (FastRandom()&65535)+1;
		v.vy = (FastRandom()&65535)+1;
		v.vz = (FastRandom()&65535)+1;

		NewNormalise(&v);
		{
			int m = Magnitude(&v);
			if (m<65536-50|| m>65536+50)
				outside++;
		}
	}
	ProfileStop("NEW NORM");
	textprint("Outside Range: %d\n",outside);
	#endif
	i = 10000;
	d=0;
	while(--i)
	{
		int m;
		VECTORCH v;
		v.vx = FastRandom()&65535;
		v.vy = FastRandom()&65535;
		v.vz = FastRandom()&65535;
		m = InverseMagnitude(&v);
		d+=m;
	}

	#if 0
	textprint("%f\n",d);
	i = 10000;
	d=0;
	ProfileStart();
	while(--i)
	{
		float m = sqrt((float)i);
		d+=m;
	}
	ProfileStop("FSQRT");
	#endif
	textprint("%f\n",d);

}
static float fptmp;
static int itmp;

void IntToFloat(void);
# pragma aux IntToFloat = \
"fild itmp" \
"fstp fptmp";

/*

 This macro makes usage of the above function easier and more elegant

*/

#define i2f(a, b) { \
itmp = (a); \
IntToFloat(); \
b = fptmp;}

int InverseMagnitude(VECTORCH *v)
{
	int answer;
	float m;
	float mag;
	{
		float x,y,z;
		x = v->vx;
		y = v->vy;
		z = v->vz;
		mag = x*x+y*y+z*z;
	}
	{
		unsigned int index;
		float r;
		DWORD *dptr = (DWORD *)&r;

		*(DWORD *)&r = ((((3 * IEEE_EXP_BIAS - 1) << IEEE_MANT_BITS) - EXP_OF(mag)) >>
		1) & 0x7f800000;

		index = ((*(DWORD *)&mag) >> (IEEE_MANT_BITS - INVSQRT_TABLE_ENTRY_BITS + 1))
		& (INVSQRT_TABLE_NUM_ENTRIES - 1);

		*dptr |= InvSqrtTab[index];

		m = 65536.0*r;	
	}

	
	f2i(answer,m);	
	return answer;
}


void CurrentQNormalise(QUAT *q)
{
	/* Normalise */
	double oos = 1.0/(65536.0*65536.0); 

	double wsq = (double)q->quatw * (double)q->quatw * oos;
	double xsq = (double)q->quatx * (double)q->quatx * oos;
	double ysq = (double)q->quaty * (double)q->quaty * oos;
	double zsq = (double)q->quatz * (double)q->quatz * oos;

	double m = sqrt(wsq + xsq + ysq + zsq);

	if(m == 0.0) m = 1.0;			/* Just in case */
	m = 1.0 / m;

	q->quatw = (int) ((double)q->quatw * m);
	q->quatx = (int) ((double)q->quatx * m);
	q->quaty = (int) ((double)q->quaty * m);
	q->quatz = (int) ((double)q->quatz * m);

}

void NewQNormalise(QUAT *q)
{
	float nw = q->quatw;
	float nx = q->quatx;
	float ny = q->quaty;
	float nz = q->quatz;
	
	float m = sqrt(nw*nw+nx*nx+ny*ny+nz*nz);
	
	if (!m) return;
		
	m = 65536.0/m;

	f2i(q->quatw,nw * m);
	f2i(q->quatx,nx * m);
	f2i(q->quaty,ny * m);
	f2i(q->quatz,nz * m);
}

void QNormaliseTest(void)
{
	QUAT q,q2;
	int i;

	for (i=0; i<10000; i++)
	{
		q.quatw = FastRandom()&65535;
		q.quatx = FastRandom()&65535;
		q.quaty = FastRandom()&65535;
		q.quatz = FastRandom()&65535;
		q2=q;

		NewQNormalise(&q);
		CurrentQNormalise(&q2);

		if (q.quatw!=q2.quatw)
		textprint("w%d ",q.quatw-q2.quatw);
		if (q.quatx!=q2.quatx)
		textprint("x%d ",q.quatx-q2.quatx);
		if (q.quaty!=q2.quaty)
		textprint("y%d ",q.quaty-q2.quaty);
		if (q.quatz!=q2.quatz)
		textprint("z%d ",q.quatz-q2.quatz);
	}
		
}
#endif

