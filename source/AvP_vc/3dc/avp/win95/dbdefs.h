// enums for the number of information screens we have
// data description

typedef enum maps{
	I_No_Map,
	I_Map_Gen1,	I_Map_Gen2,	I_Map_Gen3,	I_Map_Gen4,
	I_Map_Cmc1,	I_Map_Cmc2,	I_Map_Cmc3,	I_Map_Cmc4,	I_Map_Cmc5,	I_Map_Cmc6,
	I_Map_Rnd1,	I_Map_Rnd2,	I_Map_Rnd3,	I_Map_Rnd4,
	I_Map_Mps1, I_Map_Mps2, I_Map_Mps3, I_Map_Mps4, 
	I_Map_Sp1, I_Map_Sp2, I_Map_Sp3,
	I_Map_Surface, I_Map_Medlab,
	I_Num_Maps
}DATABASE_MAP;
	
typedef enum
{
	I_Floor_0 = 0,
	I_Floor_1, 
	I_Floor_2, 
	I_Floor_3, 
	I_Floor_4, 
	I_Floor_5,
	I_Max_Num_Floors,

}FLOOR_NUM;

typedef enum
{
	I_No_Weapon,
	I_Weapon_Pulse,
	I_Num_Weapons

}DATABASE_WEAPON;


typedef enum 
{
	I_No_Message,
	I_Message_1,
	I_Num_Messages,

}DATABASE_MESSAGE;

typedef struct dbasedesc
{
	int floor;
	DATABASE_MAP map_enum;
	DATABASE_WEAPON weap_enum;
	DATABASE_MESSAGE message_enum;
	short pixel_x;
	short pixel_y;

}DBASEDESC;

// enums to control flow of the database screens
// data base states - each one is associated with a 
// enum to describes the option from the satae

typedef enum 
{
	DB_STATE_SELECT,
	DB_STATE_MAP,
	DB_STATE_MESSAGE,
	DB_STATE_WEAPON,
	DB_STATE_LOAD,
	DB_STATE_SAVE,
	DB_STATE_END,
	NUM_DB_STATES,

}DB_MENU_STATE;



typedef enum database_options
{
	DB_MAP_OPTION,
	DB_MESSAGE_OPTION,
	DB_WEAPON_OPTION,
	DB_LOAD_OPTION,
	DB_SAVE_OPTION,
	DB_LOGOFF_OPTION,
	DB_QUIT_OPTION,
	DB_NUM_OPTIONS

}DB_MENU_OPTIONS;


typedef enum map_screen_options
{
	DB_MAP_EXIT_OPTION,
	DB_MAP_LOGOFF_OPTION,
	DB_NUM_MAP_OPTIONS,

}DB_MAP_OPTIONS;

typedef enum
{
	DB_WEAPON_EXIT_OPTION,
	DB_WEAPON_LOGOFF_OPTION,
	DB_NUM_WEAPON_OPTIONS,

}DB_WEAPON_OPTIONS;

typedef enum
{
	DB_MESSAGE_PLAY_OPTION,
	DB_MESSAGE_STOP_OPTION,
	DB_MESSAGE_EXIT_OPTION,
	DB_MESSAGE_LOGOFF_OPTION,
	DB_NUM_MESSAGE_OPTIONS,

}DB_MESSAGE_OPTIONS;


extern void DatabaseMenus(DATABASE_BLOCK*);

extern void PlatformSpecificEnteringDatabase();
extern void PlatformSpecificExitingDatabase();
