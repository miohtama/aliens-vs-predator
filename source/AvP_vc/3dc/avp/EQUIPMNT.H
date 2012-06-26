#ifndef _equipmnt_h_
#define _equipmnt_h_ 1

#ifdef __cplusplus

	extern "C" {

#endif
#include "language.h"
/* has to come after gameplat.h in the setup*/
#include "gamedef.h"

/* KJL 12:34:46 09/18/96 - new stuff under development */

/* KJL 12:39:41 09/18/96 - 
	Okay, I think we should have two weapon structures:
    
    TEMPLATE_WEAPON_DATA:	First the basic template for each weapon, which remains constant
    						during the game & contains the base description of a weapon	eg.
    						ammo type, weight, graphics, etc.
							
    PLAYER_WEAPON_DATA:		Second is the structure which describes a player's weapon
    						eg. a pointer to a TEMPLATE_WEAPON_DATA, ammo remaining, jam
                            status, and so on.
                            
                            eg.	PLAYER_WEAPON_DATA PlayerWeapons[];
                            
*/
/* KJL 16:09:51 09/20/96 - Weapon States */
enum WEAPON_STATE 
{
	WEAPONSTATE_IDLE=0,
	
	WEAPONSTATE_FIRING_PRIMARY,
    WEAPONSTATE_RECOIL_PRIMARY,
	WEAPONSTATE_RELOAD_PRIMARY,

    WEAPONSTATE_FIRING_SECONDARY,
	WEAPONSTATE_RECOIL_SECONDARY,
	WEAPONSTATE_RELOAD_SECONDARY,
	
	WEAPONSTATE_SWAPPING_IN,
	WEAPONSTATE_SWAPPING_OUT,
	WEAPONSTATE_JAMMED,

	WEAPONSTATE_WAITING,
	
	WEAPONSTATE_READYING,
	WEAPONSTATE_UNREADYING,

	MAX_NO_OF_WEAPON_STATES
};
#define WEAPONSTATE_INITIALTIMEOUTCOUNT 65536
#define WEAPONSTATE_INSTANTTIMEOUT		0

/* KJL 10:42:38 09/19/96 - Weapon Enumerations */
enum WEAPON_ID
{
	/* USED TO DENOTE AN EMPTY SLOT */
	NULL_WEAPON=-1,

	/* MARINE WEAPONS */
	WEAPON_PULSERIFLE,
    WEAPON_AUTOSHOTGUN,
	WEAPON_SMARTGUN,
    WEAPON_FLAMETHROWER,
    WEAPON_PLASMAGUN,
    WEAPON_SADAR,
    WEAPON_GRENADELAUNCHER,
    WEAPON_MINIGUN,	 
	WEAPON_SONICCANNON,
	WEAPON_BEAMCANNON,
	WEAPON_MYSTERYGUN,

	/* PREDATOR WEAPONS */
    WEAPON_PRED_WRISTBLADE,
	WEAPON_PRED_PISTOL,
	WEAPON_PRED_RIFLE,
	WEAPON_PRED_SHOULDERCANNON,
	WEAPON_PRED_DISC,
	WEAPON_PRED_MEDICOMP,
	WEAPON_PRED_STAFF,
	    
    /* ALIEN WEAPONS */
    WEAPON_ALIEN_CLAW,
	WEAPON_ALIEN_GRAB,
    WEAPON_ALIEN_SPIT,

	WEAPON_CUDGEL,
	WEAPON_MARINE_PISTOL,
	WEAPON_FRISBEE_LAUNCHER,
	WEAPON_TWO_PISTOLS,

    MAX_NO_OF_WEAPON_TEMPLATES
};

enum WEAPON_SLOT
{
	WEAPON_SLOT_1=0,
	WEAPON_SLOT_2,
	WEAPON_SLOT_3,
	WEAPON_SLOT_4,
	WEAPON_SLOT_5,
	WEAPON_SLOT_6,
	WEAPON_SLOT_7,
	WEAPON_SLOT_8,
	WEAPON_SLOT_9,
	WEAPON_SLOT_10,
	WEAPON_SLOT_11,
	MAX_NO_OF_WEAPON_SLOTS,
	
    WEAPON_FINISHED_SWAPPING
};


enum AMMO_ID
{
	AMMO_NONE=-1,
	AMMO_10MM_CULW=0,
	AMMO_SHOTGUN,
	AMMO_SMARTGUN,
	AMMO_FLAMETHROWER,
	AMMO_PLASMA,
	AMMO_SADAR_TOW,
	AMMO_GRENADE, 
	AMMO_MINIGUN,
	AMMO_PULSE_GRENADE,
	AMMO_FLARE_GRENADE,
	AMMO_FRAGMENTATION_GRENADE,
	AMMO_PROXIMITY_GRENADE,
	AMMO_PARTICLE_BEAM,
	AMMO_SONIC_PULSE,
	
	AMMO_PRED_WRISTBLADE,
	AMMO_PRED_PISTOL,
	AMMO_PRED_RIFLE,
	AMMO_PRED_ENERGY_BOLT,
	AMMO_PRED_DISC,
	
	AMMO_ALIEN_CLAW,
	AMMO_ALIEN_TAIL,
	AMMO_ALIEN_SPIT,

	AMMO_AUTOGUN,
	AMMO_XENOBORG,
	AMMO_FACEHUGGER,
	AMMO_NPC_OBSTACLE_CLEAR,
	AMMO_ALIEN_FRAG,
	AMMO_ALIEN_DEATH,

	AMMO_SHOTGUN_BLAST,
	AMMO_SADAR_BLAST,
	AMMO_ALIEN_BITE_KILLSECTION,
	AMMO_PRED_DISC_PM,

	AMMO_NPC_ALIEN_CLAW,
	AMMO_NPC_PAQ_CLAW,
	AMMO_PULSE_GRENADE_STRIKE,
	AMMO_NPC_ALIEN_TAIL,
	AMMO_NPC_ALIEN_BITE,
	AMMO_NPC_PREDALIEN_CLAW,
	AMMO_NPC_PREDALIEN_BITE,
	AMMO_NPC_PREDALIEN_TAIL,
	AMMO_NPC_PRAETORIAN_CLAW,
	AMMO_NPC_PRAETORIAN_BITE,
	AMMO_NPC_PRAETORIAN_TAIL,
	AMMO_PRED_STAFF,
	AMMO_NPC_PRED_STAFF,
	AMMO_PC_ALIEN_BITE,
	AMMO_HEAVY_PRED_WRISTBLADE,
	AMMO_MARINE_PISTOL,
	AMMO_PREDPISTOL_STRIKE,
	AMMO_PLASMACASTER_NPCKILL,
	AMMO_PLASMACASTER_PCKILL,

	AMMO_10MM_CULW_NPC,
	AMMO_SMARTGUN_NPC,
	AMMO_MINIGUN_NPC,
	AMMO_MOLOTOV,
	AMMO_ALIEN_OBSTACLE_CLEAR,
	AMMO_PRED_TROPHY_KILLSECTION,

	AMMO_CUDGEL,
	AMMO_ALIEN_BITE_KILLSECTION_SUPER,
	AMMO_MARINE_PISTOL_PC,
	AMMO_FRISBEE,
	AMMO_FRISBEE_BLAST,
	AMMO_FRISBEE_FIRE,

	MAX_NO_OF_AMMO_TEMPLATES,
	AMMO_FLECHETTE_POSTMAX,
	AMMO_FALLING_POSTMAX,
	AMMO_FIREDAMAGE_POSTMAX

};

/* CDF - 15/9/97 New Damage System */

typedef struct {
	short Impact;
	short Cutting;
	short Penetrative;
	short Fire;
	short Electrical;
	short Acid;
	/* New additions, 4/8/98 */
	unsigned int ExplosivePower	:3;
	/* XP: 0 is nothing, 1 is little, 2 is big (SADAR),
		3 is pred pistol, 4 is plasmacaster, 5 is molotov, 6 is big with no collisions,
		7+ unused. */
	unsigned int Slicing		:2;
	unsigned int ProduceBlood	:1;
	unsigned int ForceBoom		:2;
	unsigned int BlowUpSections	:1;
	unsigned int Special		:1;
	unsigned int MakeExitWounds	:1;

	enum AMMO_ID Id;
	
} DAMAGE_PROFILE;

/* CDF - 15/9/97 New Damage System */

typedef struct {
	unsigned int MovementMultiple;
	unsigned int TurningMultiple;
	unsigned int JumpingMultiple;
	unsigned int CanCrouch:1;
	unsigned int CanRun:1;
} ENCUMBERANCE_STATE;

typedef struct
{
	int					AmmoPerMagazine;
	
	DAMAGE_PROFILE		MaxDamage[I_MaxDifficulties];
	int					MaxRange;

	enum TEXTSTRING_ID	ShortName;
		/* Added 20/11/97 by DHM: Abberviated name for ammo, to appear in HUD status panel */

    /* ammo flags */
	unsigned int		CreatesProjectile :1;
	unsigned int 		ExplosionIsFlat:1;
} TEMPLATE_AMMO_DATA;

typedef struct
{
	enum WEAPON_ID	 	WeaponIDNumber;
	/* eg.==MARINE_WEAPON_PULSE. This can be used to access the TemplateWeapon[] data, graphics, etc. */
 
    enum WEAPON_STATE 	CurrentState;
 
    int				 	StateTimeOutCounter; /* in 16.16 */   
        
    unsigned int		PrimaryRoundsRemaining; /* in 'current' magazine */
    unsigned int		SecondaryRoundsRemaining; /* in 'current' magazine */
    unsigned char		PrimaryMagazinesRemaining;
    unsigned char		SecondaryMagazinesRemaining;

	VECTORCH			PositionOffset; 
    EULER				DirectionOffset;

	SHAPEANIMATIONCONTROLLER ShpAnimCtrl;
    TXACTRLBLK			*TxAnimCtrl;
     
    /* general flags */
    signed int		Possessed :2; /* meaning you are carrying the weapon, not that an evil spirit etc etc. */

} PLAYER_WEAPON_DATA;

typedef struct
{
	enum AMMO_ID		PrimaryAmmoID;
	enum AMMO_ID		SecondaryAmmoID;

	int 				(*FirePrimaryFunction)(PLAYER_WEAPON_DATA *);
	int 				(*FireSecondaryFunction)(PLAYER_WEAPON_DATA *);
	void 				(*WeaponInitFunction)(PLAYER_WEAPON_DATA *);
	   
    int					TimeOutRateForState[MAX_NO_OF_WEAPON_STATES]; /* in 16.16 */
	void 				(*WeaponStateFunction[MAX_NO_OF_WEAPON_STATES])(void *, PLAYER_WEAPON_DATA *);	// void * is for PLAYER_STATUS
	int					ProbabilityOfJamming;
    int					FiringRate;
    
    /* display stuff */
	signed int 			SmartTargetSpeed;	/* how fast crosshair moves */
    unsigned int		GunCrosshairSpeed;	/* how fast the gun moves to catch up */
    unsigned int  		SmartTargetRadius;
    VECTORCH			RestPosition;
	
	int					RecoilMaxZ;
	int					RecoilMaxRandomZ;
	int					RecoilMaxXTilt;
	int					RecoilMaxYTilt;

	VECTORCH			StrikePosition;

	enum TEXTSTRING_ID	Name;

	/* shape reference */
	char *				WeaponShapeName;
	char *				MuzzleFlashShapeName;
	char *				RiffName;
	char *				HierarchyName;
    int					InitialSequenceType;
	int					InitialSubSequence;

	/* Encumberance */

	ENCUMBERANCE_STATE	Encum_Idle;	
	ENCUMBERANCE_STATE	Encum_FirePrime;	
	ENCUMBERANCE_STATE	Encum_FireSec;	

    /* flags */
	unsigned int 		UseStateMovement :1;
	unsigned int		IsSmartTarget :1;
	unsigned int		PrimaryIsRapidFire   :1;    
	unsigned int		PrimaryIsAutomatic   :1;    
	unsigned int		PrimaryIsMeleeWeapon :1;  
	unsigned int		SecondaryIsRapidFire   :1;    
	unsigned int		SecondaryIsAutomatic   :1;    
	unsigned int		SecondaryIsMeleeWeapon :1;  
	unsigned int 		HasShapeAnimation: 1;
	unsigned int 		HasTextureAnimation: 1;
	unsigned int 		FireWhenCloaked: 1;
	unsigned int 		FireInChangeVision: 1;
	unsigned int 		FirePrimaryLate: 1;
	unsigned int 		FireSecondaryLate: 1;
	unsigned int		PrimaryMuzzleFlash:	1;
	unsigned int 		SecondaryMuzzleFlash:	1;
	unsigned int 		LogAccuracy:	1;
	unsigned int 		LogShots:	1;

} TEMPLATE_WEAPON_DATA;





typedef struct 
{
	enum AMMO_ID	SelectedAmmo;

	unsigned int	StandardRoundsRemaining;
    unsigned char	StandardMagazinesRemaining;

	unsigned int	FlareRoundsRemaining;
    unsigned char	FlareMagazinesRemaining;

	unsigned int	ProximityRoundsRemaining;
    unsigned char	ProximityMagazinesRemaining;

	unsigned int	FragmentationRoundsRemaining;
    unsigned char	FragmentationMagazinesRemaining;

} GRENADE_LAUNCHER_DATA;

typedef enum Pred_Disc_Modes {
	I_Seek_Track,
	I_Search_Destroy,
	I_Proximity_Mine,
} PRED_DISC_MODES;

typedef enum Smartgun_Modes {
	I_Track,
	I_Free,
} SMARTGUN_MODES;

extern PRED_DISC_MODES ThisDiscMode;
extern SMARTGUN_MODES SmartgunMode;

extern TEMPLATE_WEAPON_DATA	TemplateWeapon[];
extern TEMPLATE_AMMO_DATA   TemplateAmmo[];
extern GRENADE_LAUNCHER_DATA GrenadeLauncherData;

extern enum WEAPON_ID MarineWeaponKey[MAX_NO_OF_WEAPON_SLOTS];
extern enum WEAPON_ID PredatorWeaponKey[MAX_NO_OF_WEAPON_SLOTS];
extern enum WEAPON_ID AlienWeaponKey[MAX_NO_OF_WEAPON_SLOTS];

extern DAMAGE_PROFILE certainDeath;
extern DAMAGE_PROFILE console_nuke;
extern DAMAGE_PROFILE firedamage;	

extern DAMAGE_PROFILE SmallExplosionDamage;
extern DAMAGE_PROFILE BigExplosionDamage;

extern void InitialiseEquipment(void);

/*compare two damage profiles to see if they are the same*/
extern BOOL AreDamageProfilesEqual(DAMAGE_PROFILE* profile1,DAMAGE_PROFILE* profile2);



#ifdef __cplusplus

	};

#endif
#endif


/*KJL*****************************************************************************************
* Roxby's old stuff follows. I'm keeping it here 'cos it's very useful as reference material *
*****************************************************************************************KJL*/

#if 0
/*
	Hmmm. Weapons need to be treated differnetly to Other Objects
	Such as AMMO and Tools. They have things like Info Screen data
	(Text and schematics) range, damage, weight, incumberance, DM
	status, Jam status, reliabilty, Vulnerablity.

	This contains the data sturcture and the weapons information 
	for the marine weapons - It may be extended to cover all 
	the predator weapons as well
*/

/*
	The player can carry up to 100 kg.. Strong huh. 

	encumberance - a measure of how `encumbering' the weapon may
	actually be. It is an abstrcted measure of how much it slows
	the player down - It shouldnt effect the players fatigue - and
	how difficult it is to change to another weapon. 
	
	To use a weapon it the players total `USING' encumberance has to
	less than 50 in a normal corridor and less the 30 in an airduct  

*/


/* 	 Ammo

	Ammo - this is where the weight is....
	All this rounding... lets play
	with integers only

	These are only the initial specs, I imagine dramatic alterations.
	Partic if we want to swap to weights to use shifts rather than divs


	10 mm culw rounds   (Ceramic - Ultra Light Weight) 0.05 Kg each. (1.5 oz!!)
	20 mm culw rounds   (Ceramic - Ultra Light Weight) 0.08 Kg each. (1.5 oz!!)
	standard grenades 0.2 Kg each .
	Heavy grenades 0.4 Kg each.
	shotgun rounds 0.1 each	

	0	standard grenade					1  Kg
	1  	99x10 mm magizines of culw rounds.  5  Kg 
 	2  	500x10 mm magazines of culw rounds.	25 Kg
	3	5xstandard grenades					1  Kg
	4   5 Heavy grenades HE		            2  Kg
	5	5 Heavy grenades Napalm				2  Kg
	6   5 HEavy grenades Canister			2  Kg
	7	5 Heavy Grenades Cluster			2  Kg
	8	5 Heavy Grenades WP					2  Kg						
	9	20x shotgun rounds					2  Kg
	10   20 litres of fuel					18 Kg
	11	500x20 mm magizines of culw rounds	40 Kg
	12	Sonic Gun Power Packs				5  Kg


*/

/*
	Weapons Information

	Weight Kilos
	Incumberance - Abstract use difficulty. 
	
*/

#define MAX_PLAYER_WEAPONS  10
#define MAX_AMMO_TYPES			20

typedef struct ammo_data{
	unsigned char   weight; 			/*in 10ths of KG*/
	unsigned char	num_magazines;
	unsigned short	magazine_size;
	unsigned short  spread_radius;						
	unsigned short 	damage;						
	unsigned char  	damage_flags;
	unsigned char	flight_flags;
} AMMO_DATA;



/*damage flags*/

#define DAMAGE_POINT		0x01   /*point pixel damage*/
#define DAMAGE_SPREAD		0x02   /*Equal damage along radius*/
#define DAMAGE_TAPER		0x04   /*Decreasing damge on radius*/
#define DAMAGE_EXPLOSIVE1	0x08   /*explodes*/
#define DAMAGE_EXPLOSIVE2	0x10   /*big explodes*/
#define DAMAGE_DELAY		0x20   /*limpet*/
#define DAMAGE_FLAME		0x40   /*sets things on fire*/


/*Flight Flags*/

#define FLIGHT_HAND_GRENADE 0x01
#define FLIGHT_BULLET				0x02
#define FLIGHT_GRENADE_LNCH 0x04


typedef struct weapon_ammo_attributes{
	unsigned char		ammotype;
 	unsigned char		remaining_ammo;	
	unsigned char 	targeting_flags;		
	unsigned char		rate_of_fire; 			     /*frames Between rounds */	
	unsigned char		magazine_reload_time;		 /*frames to reload magazine*/ 
   	unsigned short	failure_risk;				 /*0 - 2**16*/
	unsigned char		failure_flags;				 
		
} WEAP_AMMO_ATTR;

/* WEapon data contains the data to maintain,  draw weapons
	generation of these objects comes via the entity*/

/*
	*schematic --- for the data retrival
	*hudata - for drawing the HUD graphic . Note that this is PLATFORM dependent
	*descriptive text --- to print up in the data retrival
*/ 

typedef struct weapon_data{	

	char weapon_flags;
	char name[8];			/* can be made into something else if ness*/
																/* YES the address of the platform spec data
																	 structure*/
	void *schematic;	  /* these contain the file name before the */
	HUDGRAPHIC *huddata;			/* graphic is loaded into here -- LPDDS*/ 
	
	char  *descriptivetext;		 	

	unsigned char 	weight;			   /*in 10ths of KG*/			
	unsigned char   encumbrance;	 /*time it takes to swap the
																	 weapon in pixels per sec*/
																		
	
	WEAP_AMMO_ATTR  primary;
	WEAP_AMMO_ATTR  secondary;	

	#if Saturn
	char* WDroxptr;
	#endif

} WEAPON_DATA;


/*weapon_flags */

/*targeting flags*/

#define TARGETING_MANUAL  	0x01  
#define TARGETING_XHAIR			0x02
#define TARGETING_SMART			0x04
#define TARGETING_XTOS			0x08   /*targeting depends on equip*/
#define	TARGETING_FAF_TRACK	0x10    /*Fire and forget....!*/
#define TARGETING_THROWN		0x20   

/* failure flags */

#endif













