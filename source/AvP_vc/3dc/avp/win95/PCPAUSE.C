/*-----------------------------Patrick 14/5/97------------------------------
  Source for PC game pause stuff
  --------------------------------------------------------------------------*/
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "stratdef.h"
#include "gamedef.h"
#include "gameplat.h"
#include "bh_types.h"
#include "menudefs.h"
#include "menugfx.h"
#include "pcmenus.h"
#include "usr_io.h"

#define UseLocalAssert Yes
#include "ourasert.h"

extern void CheckForGamePause(void);
extern void SaveGameToFile(void);
extern void AccessDatabase(void);
extern int ScanDrawMode;


void DoPcPause(void)
{
	ReadUserInput();
	ReadPlayerGameInput(Player->ObStrategyBlock);

	if(AvP.Network!=I_No_Network) 
	{
		// escape ends game for network play
		AvP.MainLoopRunning = 0;
		AvP.GameMode = I_GM_Playing;
		return;
	}


	CheckForGamePause();

	#if 0
	FlushTextprintBuffer();
	AvpShowViews();		
	
	/* KJL 11:25:33 7/29/97 - this needs to be changed if
	   the HUD goes all 3D accelerator drawn */
	if (ScanDrawMode != ScanDrawDirectDraw)
	{
		WriteEndCodeToExecuteBuffer();
		UnlockExecuteBufferAndPrepareForUse();
		ExecuteBuffer();
		EndD3DScene();
	}

	BLTPausedToScreen();
	
	
	FlipBuffers();
	#endif
	
	AccessDatabase();
	
	ResetFrameCounter();
	
	AvP.GameMode = I_GM_Playing;
}

static int pauseOk = 1;
void CheckForGamePause(void)
{
	/* check for pause key */
	if(PlayerStatusPtr->Mvt_InputRequests.Flags.Rqst_PauseGame)
	{
		if(pauseOk)
		{
			/* switch state */
			if(AvP.GameMode == I_GM_Playing) AvP.GameMode = I_GM_Paused;
			else if(AvP.GameMode == I_GM_Paused) AvP.GameMode = I_GM_Playing;		
			pauseOk = 0;
		}
	}
	else pauseOk = 1;
}