/*
	
	avppages.hpp

*/

#ifndef _avppages
#define _avppages 1

	#if ( defined( __WATCOMC__ ) || defined( _MSC_VER ) )
		#pragma once
	#endif

	#ifndef _rebmenus_hpp
		#include "rebmenus.hpp"
	#endif

#if UseRebMenus
class Command_QuitProgram : public Command
{
public:
	Command_QuitProgram() : Command()
	{
	}
	OurBool Execute(void);
};

class Command_StartSoloGame : public Command
{
public:
	Command_StartSoloGame
	(
		I_PLAYER_TYPE inPlayerType,
		I_AVP_ENVIRONMENTS inStartingEnv
	) : Command(),
		thePlayerType(inPlayerType),
		theStartingEnv(inStartingEnv)
	{
	}
	OurBool Execute(void);
private:
	const I_PLAYER_TYPE thePlayerType;		
	const I_AVP_ENVIRONMENTS theStartingEnv;
};

class Command_Multiplayer : public Command
{
public:
	Command_Multiplayer() : Command()
	{
	}
	OurBool Execute(void);

	// Only takes effect if the flag's been set (and clears the flag)
	// Triggers the multiplayer dialog boxes
	static void EndOfMenuLoopProcessing(void);

private:
	static OurBool bTriggerMenuDialog;
};

class Command_ExitCurrentGame : public Command
{
public:
	Command_ExitCurrentGame() : Command()
	{
	}

	OurBool Execute(void);
};

namespace RebMenus
{
	class Page_NoMenu : public Page
	{
	public:
		Page_NoMenu();
	};
	class Page_Initial : public Page
	{
	public:
		Page_Initial();
	};
	class Page_ChooseCharacter : public Page
	{
	public:
		Page_ChooseCharacter();
	};
	class Page_Options : public Page
	{
	public:
		Page_Options();
	};
	class Page_VideoOptions : public Page
	{
	public:
		Page_VideoOptions();
	private:
		void Hook_LeavingPage(void);
	};
	class Page_AudioOptions : public Page
	{
	public:
		Page_AudioOptions();
	};
	class Page_LevelsOfDetail : public Page
	{
	public:
		Page_LevelsOfDetail();
	};
	class Page_MarineBriefing : public Page
	{
	public:
		Page_MarineBriefing();
	};
	class Page_PredatorBriefing : public Page
	{
	public:
		Page_PredatorBriefing();
	};
	class Page_AlienBriefing : public Page
	{
	public:
		Page_AlienBriefing();
	};
	class Page_LoadGame : public Page
	{
	public:
		Page_LoadGame();
	};
	class Page_InputOptions : public Page
	{
	public:
		Page_InputOptions();
	private:	
		void Hook_LeavingPage(void);
	};
	class Page_ConfigMouse : public Page
	{
	public:
		Page_ConfigMouse();
	};
	class Page_PlaceholderMultiplayer : public Page
	{
	public:
		Page_PlaceholderMultiplayer();
	private:
		void Hook_EnteringPage(void);
		void Hook_LeavingPage(void);
	};
	class Page_MultiplayerErrorScreen : public Page
	{
	public:
		Page_MultiplayerErrorScreen();
	};
}; // namespace RebMenus
#endif // UseRebMenus


/* End of the header ****************************************************/



#endif
