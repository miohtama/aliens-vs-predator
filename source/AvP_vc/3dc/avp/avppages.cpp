/*******************************************************************
 *
 *    DESCRIPTION: 	avppages.cpp
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 27/3/98
 *
 *******************************************************************/

/* Includes ********************************************************/
#include "3dc.h"

	#include "rebmenus.hpp"

	#if UseRebMenus
		#include "avppages.hpp"
		#include "avpitems.hpp"

		// Network code includes:
		#include "dp_func.h"

			#include "stratdef.h"
			#include "equipmnt.h"
				// needed to include pldnet.h				

		#include "pldnet.h"

		// Control config includes:
		#include "usr_io.h"

	#endif // UseRebMenus
	
	#define UseLocalAssert Yes
	#include "ourasert.h"

/* Version settings ************************************************/

/* Constants *******************************************************/

/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
#ifdef __cplusplus
	extern "C"
	{
#endif
		extern int QuickStartMultiplayer;
		#if 0
		
		extern OurBool			DaveDebugOn;
		extern FDIEXTENSIONTAG	FDIET_Dummy;
		extern IFEXTENSIONTAG	IFET_Dummy;
		extern FDIQUAD			FDIQuad_WholeScreen;
		extern FDIPOS			FDIPos_Origin;
		extern FDIPOS			FDIPos_ScreenCentre;
		extern IFOBJECTLOCATION IFObjLoc_Origin;
		extern UncompressedGlobalPlotAtomID UGPAID_StandardNull;
		extern IFCOLOUR			IFColour_Dummy;
 		extern IFVECTOR			IFVec_Zero;
		#endif
		
		extern enum TexFmt { D3TF_4BIT, D3TF_8BIT, D3TF_16BIT, D3TF_32BIT, D3TF_MAX } d3d_desired_tex_fmt;

#ifdef __cplusplus
	};
#endif



/* Exported globals ************************************************/

/* Internal type definitions ***************************************/

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
#if UseRebMenus
///////////////////////////////////////////////////////////////////////////////
// Projects-specific commands: access to the "menu loop": /////////////////////
///////////////////////////////////////////////////////////////////////////////

//	class Command_QuitProgram : public Command
OurBool Command_QuitProgram :: Execute(void)
{
	#if OverrideOldMenus
	RebMenus :: MenuLoop :: QuitProgram();
	#endif
	return Yes;
}
// class Command_StartSoloGame : public Command
OurBool Command_StartSoloGame :: Execute(void)
{
	#if OverrideOldMenus
	RebMenus :: MenuLoop :: StartSoloGame
	(
		thePlayerType,
		theStartingEnv
	);
	#endif
	return Yes;
}
// class Command_Multiplayer : public Command
OurBool Command_Multiplayer :: Execute(void)
{
	#if OverrideOldMenus
	// Can only function outside the game:
	if ( !RebMenus :: InGame :: Get())
	{
		bTriggerMenuDialog = Yes;
		return Yes;
	}
	else
	{
		// Merely jump to the multiplayer menu without running dialog:
		RebMenus :: Page :: SelectPage(PageID_PlaceholderMultiplayer);
		return Yes;
	}
	#else
	return Yes;
	#endif
}

// Only takes effect if the flag's been set (and clears the flag)
// Triggers the multiplayer dialog boxes
#if OverrideOldMenus
// static
OurBool
Command_Multiplayer :: bTriggerMenuDialog = No;

// static
void
Command_Multiplayer :: EndOfMenuLoopProcessing(void)
{
	// Handle post-processing in the multiplayer menu page
	if
	(
		( AvP.Network != I_No_Network )
		&&
		(netGameData.myGameState == NGS_Joining)
	)
	{
		// Adapted from code in MULTMENU.C; see pp89-91 of DHM's AvP book
		// (taken from RunMultiplayerStartUp() )

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
			else
			{
				if(QuickStartMultiplayer || netGameData.playerData[PlayerIdInPlayerList(AVPDPNetID)].startFlag)
				{
					netGameData.myGameState = NGS_Playing;
				}
			}

		}
		else
		{
			/* host checks for peer time-out, and game start */
			LOCALASSERT(AvP.Network==I_Host);
			LOCALASSERT(PlayerIdInPlayerList(AVPDPNetID)!=NET_IDNOTINPLAYERLIST);
			
			if(QuickStartMultiplayer
				|| netGameData.playerData[PlayerIdInPlayerList(AVPDPNetID)].startFlag)
			{
				TransmitStartGameNetMsg();	
				netGameData.myGameState = NGS_Playing;
			}
		}		
	}

	// If network game state has been changed, potentially exit the menus:
	{
		if ( (AvP.Network!=I_No_Network) && (netGameData.myGameState!=NGS_Joining) )
		{
			// Copied/adapted from postprocessing in RunMultiplayerStartUp()
			// (see p91 of DHM's AvP book#1)

			/* examine our NGS and decide what to do */
			switch(netGameData.myGameState)
			{
				case(NGS_Playing):
				{
					/* everything went ok */
					switch(netGameData.myCharacterType)
					{
						case(NGCT_Marine):
						{
							RebMenus :: MenuLoop :: StartNetworkGame
							(
								I_Marine,
								I_Dml1
							);
							break;
						}
						case(NGCT_Predator):
						{
							RebMenus :: MenuLoop :: StartNetworkGame
							(
								I_Predator,
								I_Dml1
							);
							break;
						}
						case(NGCT_Alien):
						{
							RebMenus :: MenuLoop :: StartNetworkGame
							(
								I_Alien,
								I_Dml1
							);
							break;
						}
						default:
						{
							LOCALASSERT(1==0);
							break;
						}	
					}			
					break;
				}
				case(NGS_Leaving):
				case(NGS_EndGame):
				{
					EndAVPNetGame();
					RebMenus :: Page :: SelectPage( PageID_Initial );
					break;		
				}
				case(NGS_Error_GameFull):
				case(NGS_Error_GameStarted):
				case(NGS_Error_HostLost):
				{
					EndAVPNetGame();			
					RebMenus :: Page :: SelectPage( PageID_MultiplayerErrorScreen );
					break;
				}
				default:
				{
					LOCALASSERT(1==0);
					EndAVPNetGame();			
					RebMenus :: Page :: SelectPage( PageID_MultiplayerErrorScreen );
					break;
				}
			}

		}
	}


	// Possible requests to trigger the multiplayer dialog:
	if ( bTriggerMenuDialog )
	{
		bTriggerMenuDialog = No;

		// Adapted from code in MULTMENU.C; see pp89-91 of DHM's AvP book
		AvP.Network=I_No_Network;
		EndAVPNetGame();

		InvokeDirectPlayDialog();
			
		if(ProcessDirectPlayDialog()==0)
		{
			/* didn't connect */
			EndAVPNetGame();
			return;
		}

		
		/* we have successfully connected to a game...*/
		InitAVPNetGame();

		// Jump to page: placeholder multiplayer settings
		RebMenus :: Page :: SelectPage(PageID_PlaceholderMultiplayer);
	}
}
#endif
	// OverrideOldMenus

// class Command_ExitCurrentGame : public Command
OurBool Command_ExitCurrentGame :: Execute(void)
{
	switch (AvP.Network)
	{
	default: GLOBALASSERT(0);
	case I_No_Network:
		break;

	case I_Host:
		{
			TransmitEndOfGameNetMsg();
			netGameData.myGameState = NGS_EndGame;
		}
		break;

	case I_Peer:
		{
			TransmitPlayerLeavingNetMsg();
			netGameData.myGameState = NGS_Leaving;
		}		
		break;
	}

	// go to start menu
	AvP.MainLoopRunning = 0;

	RebMenus :: Page :: SelectPage_ClearingStack(PageID_Initial);

	return Yes;
}

class Command_KeyConfig_RestoreDefaults : public Command
{
public:
	OurBool Execute(void)
	{
		PlayerInputPrimaryConfig = DefaultPlayerInputPrimaryConfig;
		PlayerInputSecondaryConfig = DefaultPlayerInputSecondaryConfig;
		return Yes;
	}
};
class Command_MouseConfig_RestoreDefaults : public Command
{
public:
	OurBool Execute(void)
	{
		ControlMethods = DefaultControlMethods;
		return Yes;
	}
};

///////////////////////////////////////////////////////////////////////////////
// Export and selection variables for the menu pages: /////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace RebMenus
{
	// Types of export var for setting up selections that can only be changed
	// outside the game loop (bounded and unbounded):
	template <class T> class ExportVariable_NotInGame : public ExportVariable<T>
	{
	protected:
		ExportVariable_NotInGame
		(
		) : ExportVariable<T>
			(
			)
		{
		}

	private:
		// int Get(void) is left pure virtual

		virtual void OutOfGame_Set( T NewVal ) = 0;

		#if OverrideOldMenus
		void Set
		(
			T newVal
		)
		{
			if (RebMenus :: InGame :: Get() )
			{
				// Silently reject attempts to set these within the game
				return;
			}
			else
			{
				OutOfGame_Set( newVal );
			}
		}
		#else
		void Set
		(
			T // newVal
		)
		{
			// ignore; these menus are always in-game only
		}
		#endif
			
	private:
		
	};
	template <class T> class BoundedExportVariable_NotInGame : public BoundedExportVariable<T>
	{
	protected:
		BoundedExportVariable_NotInGame
		(
			T minVal_New,
			T maxVal_New
		) : BoundedExportVariable<T>
			(
				minVal_New,
				maxVal_New
			)
		{
		}

	private:
		// int Get(void) is left pure virtual
		
		virtual void OutOfGame_Set( T NewVal ) = 0;

		#if OverrideOldMenus
		void Implement_Set
		(
			T newVal
		)
		{
			if (RebMenus :: InGame :: Get() )
			{
				// Silently reject attempts to set these within the game
				return;
			}
			else
			{
				OutOfGame_Set( newVal );
			}
		}
		#else
		void Implement_Set
		(
			T // newVal
		)
		{
			// ignore; these menus are always in-game only
		}		
		#endif
	private:
		
	};


	///////////////////////////////////////////////////////////////////////////////
	// Export and selection variables for the network game pages: /////////////////
	///////////////////////////////////////////////////////////////////////////////
	
	// A class for handling the "start network game" toggle:
	class ExportVariable_StartNetworkGame : public ExportVariable<OurBool>
	{
	public:
		ExportVariable_StartNetworkGame
		(
		) : ExportVariable()
		{
		}

		OurBool Get(void) const
		{
			// Better than casting?
			if ( netGameData.myStartFlag )
			{
				return Yes;
			}
			else
			{
				return No;
			}
		}
		void Set(OurBool NewVal)
		{
			// Copied from logic in MULTMENU.C:
			netGameData.myStartFlag = NewVal ? 1 : 0;

			if(AvP.Network==I_Host)
			{
				// Then we can use this to set the start flag in the entry for
				// ourselves in the player list
				int myIndex;
				myIndex = PlayerIdInPlayerList(AVPDPNetID);
				LOCALASSERT(myIndex!=NET_IDNOTINPLAYERLIST);
				netGameData.playerData[myIndex].startFlag = 1;
			}
		}	
	};

	class BoundedExportVariable_MultiplayerSpecies : public BoundedExportVariable<int>
	{
	public:
		BoundedExportVariable_MultiplayerSpecies
		(
		) : BoundedExportVariable<int>
			(
				NGCT_Marine, // T minVal_New,
				NGCT_Alien // T maxVal_New
			)
		{
		}

	private:
		int Get(void) const
		{
			return netGameData.myCharacterType;
		}
		void Implement_Set(int newVal)
		{
			netGameData.myCharacterType = netgame_charactertype(newVal);

			if(AvP.Network==I_Host)
			{
				int myIndex;
				myIndex = PlayerIdInPlayerList(AVPDPNetID);
				LOCALASSERT(myIndex!=NET_IDNOTINPLAYERLIST);
				netGameData.playerData[myIndex].characterType = netGameData.myCharacterType;
			}
		}

	private:
	};

	class SelectionVariable_MultiplayerSpecies : public SelectionVariable
	{
	public:
		SelectionVariable_MultiplayerSpecies
		(
		) : SelectionVariable
			(
				new BoundedExportVariable_MultiplayerSpecies()
			)
		{
		}

		Appearance GetAppearance
		(
			OurBool // bSelected
		)
		{
			return Appearance
			(
				Item_NetworkStatus :: GetTextIDForCharacterType
				(
					netGameData.myCharacterType
				)
			);
		}
	};

	class BoundedExportVariable_MultiplayerGameMode : public BoundedExportVariable<int>
	{
	public:
		BoundedExportVariable_MultiplayerGameMode
		(
		) : BoundedExportVariable<int>
			(
				NGT_Individual, // T minVal_New,
				NGT_Coop // T maxVal_New
			)
		{
		}

	private:
		int Get(void) const
		{
			return netGameData.gameType;
		}
		void Implement_Set(int newVal)
		{
			if(AvP.Network==I_Host)
			{
				netGameData.gameType = netgame_type(newVal);
			}
			else
			{
				// ignore the attempt; it's not the host
			}
		}
	private:
	};


	class SelectionVariable_MultiplayerGameMode : public SelectionVariable
	{
	public:
		SelectionVariable_MultiplayerGameMode
		(
		) : SelectionVariable
			(
				new BoundedExportVariable_MultiplayerGameMode()
			)
		{
		}

		Appearance GetAppearance
		(
			OurBool // bSelected
		)
		{
			return Appearance
			(
				netGameData.gameType ? TEXTSTRING_MULTI_INDIVIDUAL : TEXTSTRING_MULTI_COOP
			);
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	// Export and selection variables for the video mode pages: ///////////////////
	///////////////////////////////////////////////////////////////////////////////

		// See PCMENUS.CPP

	class BoundedExportVariable_TextureBitDepth : public BoundedExportVariable_NotInGame<int>
	{
	public:
		BoundedExportVariable_TextureBitDepth
		(
		) : BoundedExportVariable_NotInGame
			(
				0, // T minVal_New,
				3  // T maxVal_New
			)
		{
		}

	private:
		int Get(void) const
		{
			return d3d_desired_tex_fmt;
		}
		void OutOfGame_Set(int newVal)
		{			
			d3d_desired_tex_fmt = (TexFmt)newVal;
		}
	private:
		
	};

	class SelectionVariable_TextureBitDepth : public SelectionVariable
	{
	public:
		SelectionVariable_TextureBitDepth
		(
		) : SelectionVariable
			(
				new BoundedExportVariable_TextureBitDepth()
			)
		{
		}

		Appearance GetAppearance
		(
			OurBool // bSelected
		)
		{
			return Appearance
			(
				GetTextIDForTextureBitDepth()
			);
		}

		TextID GetTextIDForTextureBitDepth(void)
		{
			switch(d3d_desired_tex_fmt)
			{
				case D3TF_4BIT:
					return TEXTSTRING_PC_TEXTURES_4BIT;
					break;
				case D3TF_8BIT:
					return TEXTSTRING_PC_TEXTURES_8BIT;
					break;
				case D3TF_16BIT:
					return TEXTSTRING_PC_TEXTURES_16BIT;
					break;
				case D3TF_32BIT:
					return TEXTSTRING_PC_TEXTURES_32BIT;
					break;
				default:
					return TEXTSTRING_BLANK;
					break;

			}
		}

	private:
	};

	///////////////////////////////////////////////////////////////////////////////
	// Export and selection variables for the mouse options page: /////////////////
	///////////////////////////////////////////////////////////////////////////////
	class BoundedExportVariable_MouseSensitivity : public BoundedExportVariable<int>
	{
	public:
		BoundedExportVariable_MouseSensitivity
		(
			OurBool bXAxis // else it's the y-axis
		) : bXAxis_Val(bXAxis),
			BoundedExportVariable<int>
			(
				(bXAxis ? (DEFAULT_MOUSEX_SENSITIVITY-20) : (DEFAULT_MOUSEY_SENSITIVITY-40) ), // T minVal_New,
				(bXAxis ? (DEFAULT_MOUSEX_SENSITIVITY+20) : (DEFAULT_MOUSEY_SENSITIVITY+40) ) // T maxVal_New
			)			
		{			
		}

		int Get(void) const
		{
			return
			(
				(bXAxis_Val)
				?
				ControlMethods.MouseXSensitivity
				:
				ControlMethods.MouseYSensitivity
			);			
		}

	private:
		void Implement_Set(int NewVal)
		{
			if (bXAxis_Val)
			{
				ControlMethods.MouseXSensitivity = NewVal;
			}
			else
			{
				ControlMethods.MouseYSensitivity = NewVal;
			}
		}

	private:
		const OurBool bXAxis_Val;
	};

	class ExportVariable_MouseAxisEffect_V : public ExportVariable<OurBool>
	{
	public:
		ExportVariable_MouseAxisEffect_V
		(
		) : ExportVariable<OurBool> ()
		{
		}

		OurBool Get(void) const
		{
			return ControlMethods.VAxisIsMovement;
		}
		void Set(OurBool NewVal)
		{
			ControlMethods.VAxisIsMovement = NewVal;
		}
	};
	class ExportVariable_MouseAxisEffect_H : public ExportVariable<OurBool>
	{
	public:
		ExportVariable_MouseAxisEffect_H
		(
		) : ExportVariable<OurBool> ()
		{
		}

		OurBool Get(void) const
		{
			return ControlMethods.HAxisIsTurning;
		}
		void Set(OurBool NewVal)
		{
			ControlMethods.HAxisIsTurning = NewVal;
		}
	};
	class ExportVariable_Mouse_FlipVertical : public ExportVariable<OurBool>
	{
	public:
		ExportVariable_Mouse_FlipVertical
		(
		) : ExportVariable<OurBool> ()
		{
		}

		OurBool Get(void) const
		{
			return ControlMethods.FlipVerticalAxis;
		}
		void Set(OurBool NewVal)
		{
			ControlMethods.FlipVerticalAxis = NewVal;
		}
	};
	class ExportVariable_Mouse_AutoCentre : public ExportVariable<OurBool>
	{
	public:
		ExportVariable_Mouse_AutoCentre
		(
		) : ExportVariable<OurBool> ()
		{
		}

		OurBool Get(void) const
		{
			return ControlMethods.AutoCentreOnMovement;
		}
		void Set(OurBool NewVal)
		{
			ControlMethods.AutoCentreOnMovement = NewVal;
		}
	};


}; // namespace RebMenus

///////////////////////////////////////////////////////////////////////////////
// Additions to the RebMenus namespace: Derived classes for each of the menu pages:
///////////////////////////////////////////////////////////////////////////////

namespace RebMenus
{
	// Globals for sliders, toggles, and selections
	// Note that all of these are unconnected to game effects;
	// if something appears in this list, modifying it in the menus does nothing
	OurBool bTest_3dAcceleration;

	int iTest_Brightness;
	int iTest_SmokeParticles;
	int iTest_BulletHoles;
	int iTest_WaterComplexity;
	int iTest_BloodParticles;

// class Page_NoMenu : public Page
	Page_NoMenu :: Page_NoMenu
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_NoMenu
		)
	{
		// empty
		AddNewJumpItem
		(
			OnOffAppearance(),
			PageID_Initial
		);
	}

// class Page_Initial : public Page
	Page_Initial :: Page_Initial
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_Initial
		)
	{
		AddNewJumpItem
		(
		 	OnOffAppearance(TEXTSTRING_MAINMENUS_NEWGAME),
		 	PageID_ChooseCharacter
		);

		AddNewJumpItem
		(
		 	OnOffAppearance(TEXTSTRING_MAINMENUS_LOADGAME),
		 	PageID_LoadGame
		);

		{
			Command* pCommand = new Command_Multiplayer();

			AddNewCommandItem
			(
			 	OnOffAppearance(TEXTSTRING_MAINMENUS_MULTIPLAYER),
				pCommand
			);

			pCommand -> R_Release();
		}

		AddNewJumpItem
		(
		 	OnOffAppearance(TEXTSTRING_MAINMENUS_OPTIONS),
		 	PageID_Options
		);

		#if 0
		AddUnimplementedItem
		(
		 	OnOffAppearance(TEXTSTRING_MAINMENUS_DEMO)
		);
		#endif

		{
			Command* pCommand_Quit = new Command_QuitProgram();

			AddNewCommandItem
			(
			 	OnOffAppearance(TEXTSTRING_MAINMENUS_EXIT),
				pCommand_Quit
			);

			pCommand_Quit -> R_Release();
		}

		#if 0
		// Tests:
		AddNewSelectionItem
		(
			OnOffAppearance(),
			D_Horiz
		);

		AddNewSliderItem
		(
			OnOffAppearance(),
			D_Horiz
		);
		#endif
	}

// class Page_ChooseCharacter : public Page
	Page_ChooseCharacter :: Page_ChooseCharacter
	(
	) : Page
		(
			Appearance
			(
				#if 0
				BitmapName("menugfx\\pg1.pg0")
				#endif
			),
			D_Vert,
			Align_Centre,
			PageID_ChooseCharacter
		)
	{
		#if 1
		AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("Graphics\\NewMenus\\Alien.rim") ),
				Appearance( BitmapName("Graphics\\NewMenus\\AlienDark.rim") )
			),
			PageID_AlienBriefing
		);
		AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("Graphics\\NewMenus\\Marine.rim") ),
				Appearance( BitmapName("Graphics\\NewMenus\\MarineDark.rim") )

			),
			PageID_MarineBriefing
		);
		AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("Graphics\\NewMenus\\Predator.rim") ),
				Appearance( BitmapName("Graphics\\NewMenus\\PredatorDark.rim") )
			),
			PageID_PredatorBriefing
		);
		#else
		CompositeItem* pComposite = AddNewCompositeItem
		(
			OnOffAppearance(),
			D_Horiz,
			Align_Centre
		);

		pComposite -> AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("menugfx\\pg1A.pg0") ),
				Appearance()
			),
			PageID_AlienBriefing
		);
		pComposite -> AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("menugfx\\pg1M.pg0") ),
				Appearance()
			),
			PageID_MarineBriefing
		);
		pComposite -> AddNewJumpItem
		(
			OnOffAppearance
			(
				Appearance( BitmapName("menugfx\\pg1P.pg0") ),
				Appearance()
			),
			PageID_PredatorBriefing
		);
		#endif

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_Options : public Page
	Page_Options :: Page_Options
	(
	) : Page
		(
			Appearance(TEXTSTRING_MAINMENUS_OPTIONS),
			D_Vert,
			Align_Centre,
			PageID_Options
		)
	{
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_OPTIONSMENU_VIDEO),
			PageID_VideoOptions
		);
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_OPTIONSMENU_AUDIO),
			PageID_AudioOptions
		);
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_OPTIONSMENU_INPUT),
			PageID_InputOptions
		);
		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);

		#if 0
		TEXTSTRING_RETURNTOLASTMENU
			"RETURN"

		TEXTSTRING_OPTIONSMENU_VIDEO
			"VIDEO"

		TEXTSTRING_OPTIONSMENU_AUDIO
			"AUDIO"

		TEXTSTRING_OPTIONSMENU_INPUT
			"INPUT"

		#endif
		#if 0
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_PC_KEYCONFIG),
			PageID_ConfigControls
		);
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOMAINMENU),
			PageID_Initial
		);
		#endif

			#if 0
				TEXTSTRING_PC_3DACCELERATION,
				TEXTSTRING_PC_MICROSOFTDIRECT3D,
				TEXTSTRING_PC_TEXTURES,
				TEXTSTRING_PC_DISPLAY,
				TEXTSTRING_PC_ZBUFFERING,
				TEXTSTRING_PC_MIPMAPPING,
				TEXTSTRING_PC_SHADING,
				TEXTSTRING_PC_KEYCONFIG,
				TEXTSTRING_PC_FLAT,
				TEXTSTRING_PC_GOURAUD,
				TEXTSTRING_PC_BILINEARFILTER,
				TEXTSTRING_CANCEL,
				TEXTSTRING_MORE,
				TEXTSTRING_NO,
				TEXTSTRING_YES,
				TEXTSTRING_NOTAVAILABLE,
				TEXTSTRING_RETURNTOMAINMENU,
			#endif

	}

// class Page_VideoOptions : public Page
	Page_VideoOptions :: Page_VideoOptions
	(
	) : Page
		(
			Appearance(TEXTSTRING_VIDEOOPTIONSMENU_TITLE),
			D_Vert,
			Align_Min,
			PageID_VideoOptions
		)
	{
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_VIDEOOPTIONSMENU_BRIGHTNESS),
			D_Horiz,
			new SimpleBoundedExportVariable<int>
			(
				iTest_Brightness,
				0,
				ONE_FIXED
			)

		);
		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_PC_3DACCELERATION),
			D_Horiz,
			OnOffAppearance
			(
				Appearance(TEXTSTRING_YES),
				Appearance(TEXTSTRING_NO)
			),
			new ExportVariable_3dAcceleration()
		);
		AddNewItem_Special
		(
			new Item_VideoModeSelector
			(			
				OnOffAppearance(TEXTSTRING_PC_DISPLAY)
			)
		);
		#if 0
		/*
			Disabled 1/4/98 by DHM since using it seems to crash
			on loading the game with z-buffering off; I don't know if this is a
			bug in the way I'm setting things, or it's that Z buffering "off"
			is no longer supported somehow
			(dies in first call to LoadDDGraphic, loading "blubrmr.pg0" for
			marine HUD)
		*/
		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_PC_ZBUFFERING),
			D_Horiz,
			OnOffAppearance
			(
				Appearance(TEXTSTRING_YES),
				Appearance(TEXTSTRING_NO)
			),
			new ExportVariable_ZBuffering()
		);
		#endif
		AddNewSelectionItem
		(
			OnOffAppearance(TEXTSTRING_PC_TEXTUREBITDEPTH),
			D_Horiz,
			new SelectionVariable_TextureBitDepth()
		);

		#if 0
		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_PC_BILINEARFILTER),
			D_Horiz,
			OnOffAppearance
			(
				Appearance(TEXTSTRING_YES),
				Appearance(TEXTSTRING_NO)
			),
			new ExportVariable_BilinearFiltering()
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_PC_MIPMAPPING)
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_PC_SHADING)
		);
		#endif
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_VIDEOOPTIONSMENU_LEVELSOFDETAIL),
			PageID_LevelsOfDetail
		);
		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_AudioOptions : public Page
	Page_AudioOptions :: Page_AudioOptions
	(
	) : Page
		(
			Appearance(TEXTSTRING_AUDIOOPTIONSMENU_TITLE),
			D_Vert,
			Align_Min,
			PageID_AudioOptions
		)
	{
		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_LevelsOfDetail : public Page
	Page_LevelsOfDetail :: Page_LevelsOfDetail
	(
	) : Page
		(
			Appearance(TEXTSTRING_VIDEOOPTIONSMENU_LEVELSOFDETAIL),
			D_Vert,
			Align_Min,
			PageID_LevelsOfDetail
		)
	{
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_LEVELSOFDETAIL_SMOKEPARTICLES),
			D_Horiz,
			new SimpleBoundedExportVariable<int>
			(
				iTest_SmokeParticles,
				0,
				ONE_FIXED
			)
		);
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_LEVELSOFDETAIL_BULLETHOLES),
			D_Horiz,
			new SimpleBoundedExportVariable<int>
			(
				iTest_BulletHoles,
				0,
				ONE_FIXED
			)
		);
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_LEVELSOFDETAIL_WATERCOMPLEXITY),
			D_Horiz,
			new SimpleBoundedExportVariable<int>
			(
				iTest_WaterComplexity,
				0,
				ONE_FIXED
			)
		);
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_LEVELSOFDETAIL_BLOODPARTICLES),
			D_Horiz,
			new SimpleBoundedExportVariable<int>
			(
				iTest_BloodParticles,
				0,
				ONE_FIXED
			)
		);

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_MarineBriefing : public Page
	Page_MarineBriefing :: Page_MarineBriefing
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_MarineBriefing
		)
	{
		{
			Command* pCommand_StartSoloGame = new Command_StartSoloGame
			(
				I_Marine,
				I_Entrance
			);

			AddNewCommandItem
			(
			 	OnOffAppearance(TEXTSTRING_MAINMENUS_STARTGAME),
				pCommand_StartSoloGame
			);

			pCommand_StartSoloGame -> R_Release();
		}

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_PredatorBriefing : public Page
	Page_PredatorBriefing :: Page_PredatorBriefing
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_PredatorBriefing
		)
	{
		{
			Command* pCommand_StartSoloGame = new Command_StartSoloGame
			(
				I_Predator,
				I_Cmc4
			);

			AddNewCommandItem
			(
			 	OnOffAppearance(TEXTSTRING_MAINMENUS_STARTGAME),
				pCommand_StartSoloGame
			);

			pCommand_StartSoloGame -> R_Release();
		}

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_AlienBriefing : public Page
	Page_AlienBriefing :: Page_AlienBriefing
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_AlienBriefing
		)
	{
		{
			Command* pCommand_StartSoloGame = new Command_StartSoloGame
			(
				I_Alien,
				I_Medlab
			);

			AddNewCommandItem
			(
			 	OnOffAppearance(TEXTSTRING_MAINMENUS_STARTGAME),
				pCommand_StartSoloGame
			);

			pCommand_StartSoloGame -> R_Release();
		}

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_LoadGame : public Page
	Page_LoadGame :: Page_LoadGame
	(
	) : Page
		(
			Appearance(),
			D_Vert,
			Align_Centre,
			PageID_LoadGame
		)
	{
		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_InputOptions : public Page
	Page_InputOptions :: Page_InputOptions
	(
	) : Page
		(
			Appearance(TEXTSTRING_PCKEY_TITLE),
			D_Vert,
			Align_Centre,
			PageID_InputOptions
		)
	{
		#if 0
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_CONTROLCONFIGURATION_ALIEN)
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_CONTROLCONFIGURATION_MARINE)
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_CONTROLCONFIGURATION_PREDATOR)
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_CONTROLCONFIGURATION_SHARED)
		);
		AddUnimplementedItem
		(
			OnOffAppearance(TEXTSTRING_SHARECONTROLCONFIGURATION)
		);
		#endif

		AddNewItem_Special
		(
			new Item_KeyConfig_PageView
			(
				OnOffAppearance(),
				7 // int NumEffectsPerPage
			)
		);

		#if 1
		{
			Command* pCommand = new Command_KeyConfig_PrvPage();
			AddNewCommandItem
			(
				OnOffAppearance(TEXTSTRING_PSXCOMPUTER_PREVIOUS),
				pCommand
			);
			pCommand -> R_Release();
		}
		{
			Command* pCommand = new Command_KeyConfig_NxtPage();
			AddNewCommandItem
			(
				OnOffAppearance(TEXTSTRING_PSXCOMPUTER_NEXT),
				pCommand
			);
			pCommand -> R_Release();
		}
		#else
		AddNewItem_Special
		(
			new Item_KeyConfig_Selector
			(
				OnOffAppearance()
			)
		);
		#endif

		{
			Command* pCommand = new Command_KeyConfig_RestoreDefaults();
			AddNewCommandItem
			(
				OnOffAppearance(TEXTSTRING_RESTORE_DEFAULTS),
				pCommand
			);
			pCommand -> R_Release();
		}		

		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_TITLE),
			PageID_MouseConfig
		);

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}


// class Page_ConfigMouse : public Page
	Page_ConfigMouse :: Page_ConfigMouse
	(
	) : Page
		(
			Appearance(TEXTSTRING_MOUSE_TITLE),
			D_Vert,
			Align_Min,
			PageID_MouseConfig
		)
	{
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_XSENSITIVITY),
			D_Horiz,
			new BoundedExportVariable_MouseSensitivity
			(
				Yes // OurBool bXAxis
			)
		);
		AddNewSliderItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_YSENSITIVITY),
			D_Horiz,
			new BoundedExportVariable_MouseSensitivity
			(
				No // OurBool bXAxis
			)
		);

		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_VERTICAL_AXIS),

			D_Horiz,

			OnOffAppearance
			(
				Appearance(TEXTSTRING_MOUSE_MOVEMENT),
				Appearance(TEXTSTRING_MOUSE_LOOKING)
			),

			new ExportVariable_MouseAxisEffect_V()
		);
		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_HORIZONTAL_AXIS),

			D_Horiz,

			OnOffAppearance
			(
				Appearance(TEXTSTRING_MOUSE_TURNING),
				Appearance(TEXTSTRING_MOUSE_SIDESTEPPING)
			),

			new ExportVariable_MouseAxisEffect_H()
		);

		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_FLIP_VERTICAL),

			D_Horiz,

			OnOffAppearance
			(
				Appearance(TEXTSTRING_YES),
				Appearance(TEXTSTRING_NO)
			),

			new ExportVariable_Mouse_FlipVertical()
		);
		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_MOUSE_AUTO_CENTRE),

			D_Horiz,

			OnOffAppearance
			(
				Appearance(TEXTSTRING_YES),
				Appearance(TEXTSTRING_NO)
			),

			new ExportVariable_Mouse_AutoCentre()
		);

		{
			Command* pCommand = new Command_MouseConfig_RestoreDefaults();
			AddNewCommandItem
			(
				OnOffAppearance(TEXTSTRING_RESTORE_DEFAULTS),
				pCommand
			);
			pCommand -> R_Release();
		}		

		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_PlaceholderMultiplayer : public Page
	Page_PlaceholderMultiplayer :: Page_PlaceholderMultiplayer
	(
	) : Page
		(
			Appearance(TEXTSTRING_MAINMENUS_MULTIPLAYER),
			D_Vert,
			Align_Min,
			PageID_PlaceholderMultiplayer
		)
	{
		AddNewItem_Special
		(
			new Item_NetworkStatus
			(
				OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
					// sort this out !
			)
		);
		AddNewSelectionItem
		(
			OnOffAppearance(TEXTSTRING_MULTI_CHARACTER),
			D_Horiz,
			new SelectionVariable_MultiplayerSpecies()
		);
		AddNewSelectionItem
		(
			OnOffAppearance(TEXTSTRING_MULTI_GAMEMODE),
			D_Horiz,
			new SelectionVariable_MultiplayerGameMode()
		);

#if 0
TEXTSTRING_MULTI_GAMEMODE,
TEXTSTRING_MULTI_INDIVIDUAL,
TEXTSTRING_MULTI_COOP,
#endif

		AddNewToggleItem
		(
			OnOffAppearance(TEXTSTRING_MULTI_START), // OnOffAppearance theOnOffApp_Label,
			D_Horiz, // enum Direction initDir,
			OnOffAppearance
			(
				Appearance(TEXTSTRING_MULTI_OK),
				Appearance(TEXTSTRING_BLANK)
			), // OnOffAppearance theOnOffApp_Choice,
			new ExportVariable_StartNetworkGame() // ExportVariable<OurBool>* pExpVar
		);
		AddNewCancelItem
		(
			OnOffAppearance(TEXTSTRING_RETURNTOLASTMENU)
		);
	}

// class Page_MultiplayerErrorScreen : public Page
	Page_MultiplayerErrorScreen :: Page_MultiplayerErrorScreen
	(
	) : Page
		(
			Appearance(TEXTSTRING_MAINMENUS_MULTIPLAYER),
			D_Vert,
			Align_Centre,
			PageID_MultiplayerErrorScreen
		)
	{
		AddNewItem_Special
		(
			new Item_NetworkErrorView()
		);
		AddNewJumpItem
		(
			OnOffAppearance(TEXTSTRING_MULTI_CANCEL),
			PageID_Initial
		);
	}
	

}; // end of namespace RebMenus


void
RebMenus :: Page_VideoOptions :: Hook_LeavingPage(void)
{
	SaveVideoModeSettings();
		// this function also copies the currently selected mode for the
		// currently selected DriverMode into a selected video mode global
}

void
RebMenus :: Page_InputOptions :: Hook_LeavingPage(void)
{
	SaveKeyConfiguration();
}

void
RebMenus :: Page_PlaceholderMultiplayer :: Hook_EnteringPage(void)
{
	// Set START button to a default "off" value:
	netGameData.myStartFlag = 0;

	// Leave everything else as it was (e.g. requested species, game mode, etc)
}

void
RebMenus :: Page_PlaceholderMultiplayer :: Hook_LeavingPage(void)
{
	if (AvP.Network!=I_No_Network)
	{
		if ( netGameData . myGameState == NGS_Joining )
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
		}
	}
}

#endif // UseRebMenus

/* Internal function definitions ***********************************/
#if 0
void MouseOptionsMenu(void)
{
	int selection = 0, numSensitivities;
	int exit = -1, debounce = 0;
  
	while (MouseXSensitivities[selection] != -1)
	{
		if (MouseXSensitivities[selection] == ControlMethods.MouseXSensitivity) mouseXsensitivity = selection;
		selection++;
	}	  

	selection = 0;
	while (MouseYSensitivities[selection] != -1)
	{
		if (MouseYSensitivities[selection] == ControlMethods.MouseYSensitivity) mouseYsensitivity = selection;
		selection++;
	}
	numSensitivities = selection;

	selection = 0;
						
	while (exit == -1)
	{
		// JCWH 18/02/98: allow ALT+TAB
		CheckForWindowsMessages();
			
		DrawMouseOptionsScreen(selection);
		 
		/* Deal with moving the selected item */
		ReadUserInput();

		if (IDemandGoForward() && debounce)
		{
			debounce = 0;
			selection--;
			if (selection < 0) selection = MOUSECONFIG_NUMITEMS - 1;
		}
		else if (IDemandGoBackward() && debounce)
		{
			debounce = 0;	
			selection++;
			if (selection >= MOUSECONFIG_NUMITEMS) selection = 0;
		}
		else if (IDemandTurnRight() && debounce)
		{
			debounce = 0;
			/* Execute appropriate action */

			if (selection == MOUSECONFIG_VAXIS) ControlMethods.VAxisIsMovement = 1 - ControlMethods.VAxisIsMovement;
		 	else if (selection == MOUSECONFIG_HAXIS) ControlMethods.HAxisIsTurning = 1 - ControlMethods.HAxisIsTurning;
			else if (selection == MOUSECONFIG_FLIPVERTICAL) ControlMethods.FlipVerticalAxis = 1 - ControlMethods.FlipVerticalAxis;
			else if (selection == MOUSECONFIG_AUTOCENTRE) ControlMethods.AutoCentreOnMovement = 1 - ControlMethods.AutoCentreOnMovement;
			else if (selection == MOUSECONFIG_XSENSITIVITY)
			{
				mouseXsensitivity++;
				if (mouseXsensitivity >= numSensitivities) mouseXsensitivity = numSensitivities - 1;
			}
			else if (selection == MOUSECONFIG_YSENSITIVITY)
			{
				mouseYsensitivity++;
				if (mouseYsensitivity >= numSensitivities) mouseYsensitivity = numSensitivities - 1;
			}
		}
		else if (IDemandTurnLeft() && debounce)
		{
			debounce = 0;
			/* ditto */

			if (selection == MOUSECONFIG_VAXIS) ControlMethods.VAxisIsMovement = 1 - ControlMethods.VAxisIsMovement;
		 	else if (selection == MOUSECONFIG_HAXIS) ControlMethods.HAxisIsTurning = 1 - ControlMethods.HAxisIsTurning;
			else if (selection == MOUSECONFIG_FLIPVERTICAL) ControlMethods.FlipVerticalAxis = 1 - ControlMethods.FlipVerticalAxis;
			else if (selection == MOUSECONFIG_AUTOCENTRE) ControlMethods.AutoCentreOnMovement = 1 - ControlMethods.AutoCentreOnMovement;
			else if (selection == MOUSECONFIG_XSENSITIVITY)
			{
				mouseXsensitivity--;
				if (mouseXsensitivity < 0) mouseXsensitivity = 0;
			}
			else if (selection == MOUSECONFIG_YSENSITIVITY)
			{
				mouseYsensitivity--;
				if (mouseYsensitivity < 0) mouseYsensitivity = 0;
			}
		}
		else if (IDemandSelect() && debounce) 
		{
			debounce = 0;

			if (selection == MOUSECONFIG_NUMITEMS - 1) 
			{
				/* Exit item */
				exit = 1;
			}
		}

		if ((!IDemandGoForward()) && (!IDemandGoBackward()) &&
			(!IDemandTurnLeft()) && (!IDemandTurnRight()) && 
			(!IDemandSelect()) ) debounce = 1;
	}

  	ControlMethods.MouseXSensitivity = MouseXSensitivities[mouseXsensitivity];
	ControlMethods.MouseYSensitivity = MouseYSensitivities[mouseYsensitivity];
  
}
#endif