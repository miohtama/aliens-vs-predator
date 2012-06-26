#ifndef EQUATES_INCLUDED


/*

 Equates & Enums for AVP

*/

#ifdef __cplusplus
extern "C" {
#endif

#define default_global_h1 5000000
#define default_global_h2 (5000000 + (ONE_FIXED << 5))
#define default_global_hs 5


#define default_zratio_threshold 320	/* 1.25 */

#define MaxObjectLights 50			/* Sources attached to the object */

#define maxlightblocks 100			/* This ought to be MORE than enough */

#if PSX
#define MaxLightsPerObject 10	/* Sources lighting the object */
#else
#define MaxLightsPerObject 100	/* Sources lighting the object */
#endif


/*

 3d Texture Scan Subdivision limits

*/

#define lin_s_max 5

#if 0

	#define lin_s_zthr 320		/* 1.25 */

#else

	#if 1

		#define lin_s_zthr 281		/* 1.1 */

	#else

		#define lin_s_zthr 260		/* 1.01 */

	#endif

#endif



/* AH Table */




#define RScale 2
#define VScale (6 + 3)

#define Wibble Yes

#define GlobalScale 1




/*
 Scenes and View Types
*/


typedef enum {

	AVP_Scene0,		/* environments*/
	AVP_Scene1,
	AVP_Scene2,
	AVP_Scene3,
	AVP_Scene4,
	AVP_Scene5,
	AVP_Scene6,
	AVP_Scene7,
	AVP_Scene8,
	AVP_Scene9,
	AVP_Scene10

} SCENE;


typedef enum {

	AVP_ViewType0,   /* worlds within env*/

} VIEWTYPE;

/*

 View Handler Function Array Indices

*/

typedef enum {

	VState_Inside,
	VState_RelativeRemote,
	VState_RelativeYRemote,
	VState_FixedRemote,
	VState_FlyBy,
	VState_LagRelRemote,
	VState_TrackingRemote,
	VState_LagRelYRemote,

	VState_Last

} VIEWSTATES;

#define CameraTrackingNormal  0x00000000
#define CameraTrackingSlew    0x00000001
#define CameraTrackingFollow  0x00000002
#define CameraTrackingTrakBak 0x00000004

#define PanChange             128


/*

 View Interior Types

*/

typedef enum {

	IType_Default,
	IType_Body,
	IType_Car,
	IType_Aircraft,

	IType_Last

} ITYPES;


/*

 Shape enum for mainshapelist[]	 

 We don't need this except for compiled in
 shapes. For pc riff loading the comipled in
 shape enum is in cnkhmaps.c in avp\win95
 
*/

#if PSX
#if BinaryLoading
#else
typedef enum {

	Shape_bob,
	Shape_Default,
	Shape_Alien,
	Shape_weapon,
	Shape_terminal,
	Shape_mmseg1,
	Shape_Cube,

} AVP_SHAPES;

#endif
#endif


/* Map Types */

typedef enum {

	MapType_Default,
	MapType_Player,
	MapType_PlayerShipCamera,
 	MapType_Sprite,
	MapType_Term

} AVP_MAP_TYPES;


/*  Strategies */

typedef enum {

	StrategyI_Null,
	StrategyI_Camera,
	StrategyI_Player,
	StrategyI_Test,
	StrategyI_NewtonTest,
	StrategyI_HomingTest,
	StrategyI_MissileTest,
	StrategyI_GravityOnly,
	StrategyI_Database,
	StrategyI_DoorPROX,
	StrategyI_Terminal,
	StrategyI_Last		/* Always the last */

} AVP_STRATEGIES;






#define MaxSint5SortArraySize 50 /* was 100, must be at least ml_shm_maxheights */



/***********end for C++************/

#ifdef __cplusplus
};
#endif

#define EQUATES_INCLUDED

#endif
