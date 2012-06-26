/*
	
	equiputl.hpp

	Created 8/1/98 by DHM: Utilies for dealing with equipment

*/

#ifndef _equiputl
#define _equiputl 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif


	#ifndef _equipmnt_h_
	#include "equipmnt.h"
	#endif

	#ifndef _langenum_h_
	#include "langenum.h"
	#endif

#ifdef __cplusplus
	extern "C" {
#endif

/* Version settings *****************************************************/

/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/

/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
	namespace EquipmentUtil
	{
		// Functions to get the ID for the text string "ROUNDS" and "MAGAZINES"
		// Mostly returns TEXTSTRING_ROUNDS and TEXTSTRING_MAGAZINES,
		// but some weapons have different strings

		enum TEXTSTRING_ID GetTextID_Rounds
		(
			enum WEAPON_ID WeaponID
		);

		enum TEXTSTRING_ID GetTextID_Magazines
		(
			enum WEAPON_ID WeaponID
		);

	};
	


/* End of the header ****************************************************/


#ifdef __cplusplus
	};
#endif

#endif
