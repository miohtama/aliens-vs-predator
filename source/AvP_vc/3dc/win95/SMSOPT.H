//-------------------------------------------------------------------
//    DESCRIPTION:	SMSOPT.H - Options for SMS compilation
//
//    AUTHOR:		Mark Tolley
//
//    HISTORY:		Created 19th Sept 1996    
//
//-------------------------------------------------------------------

// Incorporated into sndmanag.h and xxxxxsnd.h

#ifndef SMSOPT_H
#define SMSOPT_H


// #DEFINES
// General switches
#define SOUND_ON 0 // Compile sound commands in main game code
#define SOUND_3D 1 // Compile 3D sound functions

						// (NB switching this off makes SMS independent of 3DC)

// Platform switches - ONLY ONE OF THESE SHOULD BE ON!!
#define SMS_SATURN 0	// Compile SMS for Saturn
#define SMS_PSX 0			// Compile SMS for PSX
#define SMS_PCDOS 0 	// Compile SMS for PC-DOS
#define SMS_WIN32 1		// Compile SMS for PC-Win95

// Sound source switches
#define MIDI_ON 1		// Compile MIDI-specific code  
#define DIGI_ON 1		// Compile WAV-specific code
#define CDDA_ON 1		// Compile CDDA-specific code

// Any other sound-specific compiler switches
#define SMS_FORCE_PENTIUM_TO_DOS_QUALITY 0	// Forces Pentium to use
						// DOS quality sound. May help to speed things
						// up... NB 3D SOUND WON'T WORK PROPERLY 
#define SMS_TIMER 1		// Implement timing for ONEHI. Relies on NormalFrameTime
#endif // SMSOPT_H
// END OF FILE
