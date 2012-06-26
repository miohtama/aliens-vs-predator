/*
	
	statpane.h

	Created 18/11/97 by DHM:  status panels for Marine HUD
*/

#ifndef _statpane_h
#define _statpane_h 1

#ifdef __cplusplus
	extern "C" {
#endif

/* Version settings *****************************************************/

/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/
	enum StatusPanelIndex
	{
		I_StatusPanel_Weapons,
			// assumed to be the first

		I_StatusPanel_Inventory,
		I_StatusPanel_Objectives,
		I_StatusPanel_GameStats,

		NUM_STATUS_PANELS
	}; /* suggested naming: "I_StatusPanel" */


/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
	void STATPANE_RequestStatusPanel
	(
		enum StatusPanelIndex I_StatusPanel
	);
		/* this should be called once per frame, or else call STATPANE_NoRequestedIndex()
		Call it if the button for a panel is in a "down" state.
		Currently the system has an internal order of precedence using the
		enum StatusPanelIndex; the earlier entries have greater priority.
		*/

	void STATPANE_NoRequestedPanel(void);
		/*
			Makes the status panel retreats off the screen; call if none of the buttons
			for selecting status panels are depressed
		*/



/* End of the header ****************************************************/


#ifdef __cplusplus
	};
#endif

#endif
