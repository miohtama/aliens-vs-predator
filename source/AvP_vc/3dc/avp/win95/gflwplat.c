#include "3dc.h"
#include "module.h"
#include "inline.h"

#include "stratdef.h"
#include "gamedef.h"
#include "bh_types.h"
#include "gameplat.h"
#include "gameflow.h"

static char * messages[PlayerMissions_Last]=
{
"BASE COMMANDER:\n\
WOAH, CAN'T SPEAK NOW, THERE'S \n\
SOME MAJOR SHIT GOING DOWN.  GET\n\
YOUR ARSE TO GENERAL.",
 
"BASE COMMANDER:\n\
HEADS UP TROOP WE'RE IN SOME PRETTY \n\
SHIT. TAKE OUT THE 4 VEHICLE LIFT \n\
MANUAL OVERRIDE CONTROLS IN \n\
LIVING QUARTERS AND HANGAR ONE. WE'LL \n\
OPEN ACCESS TO THE LIFT OUT.  THEY'RE\n\
THE ONES WITH THE COMPUTER TERMINAL AT\n\
THE TOP AND A YELLOW STRIPED PIPE AT\n\
THE BOTTOM. OH, AND, WATCH YOUR BACK,\n\
THERE IS SOME ER.. EXPERIMENTAL TECH\n\
STUFF LOOSE IN THE BASE.",

"BASE COMMANDER:\n\
TROOP, FURTHER ORDERS FROM ON HIGH.\n\
GET TO SECURITY 2, THERE'S SOME AMMO\n\
WAITING. BASE COM OUT.",

"BASE COMMANDER:\n\
BASE COM HERE. I DON'T LIKE THIS MUCH, \n\
BUT THIS PRETTY LI'L LADY WON'T TAKE\n\
NO FOR AN ANSWER. THE SCI TECH STUFF,\n\
ITS WORSE THAN WE THOUGHT, 'SPECIALLY\n\
IN R'N'D LABS.  WE CAN'T ALLOW ACCESS\n\
TO THE LIFT.  I GUESS THE ONLY OPTION\n\
IS TO GO THE LONG WAY ROUND.  WATCH\n\
YOUR BACK, AND TRY TO KILL AS MANY AS\n\
YOU CAN, OK?",

"BASE COMMANDER:\n\
AN ESCAPE SHIP IS PREPPED, WE'LL WAIT\n\
AS LONG AS WE CAN.  MEET POINT IS MY \n\
OFFICE IN MAIN CONTROL, GET HERE ASAP.",

"BASE COMMANDER:\n\
SHIT, TROOP WE COULDN'T WAIT.  WE'RE \n\
NOW IN MAIN ARMOURY. ACCESS TO GENERAL \n\
AND MAIN CONTROL IS SEALED OFF.  WE'LL \n\
HOLD DOWN IN HANGAR TWO FOR YOU.\n\
WATCH OUT FOR SENTRY GUNS, WE'VE\n\
PLACED THEM TO COVER OUR BACKS.  THE\n\
ARMOURY IS STILL FULL OF AMMO.",

"CORPORATION WOMAN:\n\
WE ARE NOW IN ORBIT.  YOUR ONLY CHANCE\n\
TO SURVIVE IS TO REACH THE EMERGENCY \n\
SHUTTLE, BUT IT WILL ONLY WORK IF THE \n\
BASE SELF-DESTRUCT IS SET.  YOU CAN \n\
REACH THE DESTRUCT CORE IN MAIN \n\
REACTOR.  ONCE YOU'VE SET IT, YOU \n\
CAN GET OUT ACROSS THE SURFACE TO THE\n\
SHUTTLE.",

"CONGRATULATIONS, YOU HAVE WON.\n\
ENJOY THE SURFACE LEVEL.",




"DESTROY THE COMPUTERS IN MAIN CONTROL\n",

"RECOVER SHIP PART FROM HANGAR ONE\n",

"RECOVER SHIP PART FROM HANGAR TWO\n",

"KILL PREDATOR ALIENS\n",

"KILL ALIEN QUEEN AND SET SELF-\n\
DESTRUCT IN MAIN REACTOR\n",

"RETURN TO SHIP IN NON-TERRESTRIAL\n\
TECHNOLOGY LAB.\n",



	
	0,
	0,
	0,
	0,
	
};

extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;


void DoStartMissionSequence (PLAYERMISSION mission)
{
	
	char buffer[1024];

	sprintf(buffer, "%s\n\nPress Return\n", messages[mission]);

	ColourFillBackBuffer(0);
#if PreBeta
	jtextprint ("%s", buffer);
	FlipBuffers();
#elif debug
	textprint ("%s", buffer);
	FlushTextprintBuffer();
	FlipBuffers();
#endif
	
}

char * GetCurrentMissionString ()
{
	if (PlayerStatusPtr->CurrentMission < PlayerMissions_Last)
	{
		return(messages[PlayerStatusPtr->CurrentMission]);
	}
	else
	{
		return(0);
	}
}

void ShowMissionMessage ()
{
	char buffer[1024];
	
	if (!messages[PlayerStatusPtr->CurrentMission])
	{
		return;
	}
	
	sprintf(buffer, "%s\nSecurity %x Flags %x\n", messages[PlayerStatusPtr->CurrentMission], 
						PlayerStatusPtr->securityClearances,
						PlayerStatusPtr->StateChangeObjectFlags);
	
#if PreBeta
	jtextprint ("%s", buffer);
#elif debug
	textprint ("%s", buffer);
#endif	
}

// this will only print one string from the top of the screen

static int CharWidthInPixels(char Ch)
{
	if 
	(
		(
			(Ch>=FontStart)
			&&
			(Ch<=FontEnd)
		)
		||
		(' '==Ch)

	)
	{
		return CharWidth;
	}
	else
	{
		return 0;
	}
}

static int LastDisplayableXForChars(void)
{
	return ScreenDescriptorBlock.SDB_Width-CharWidth;
}

void jtextprint (const char* t, ...)
{
	char buffer [1024];
	/*
	Get message string from arguments into buffer...
	*/
	{
		va_list ap;
	
		va_start(ap, t);
		vsprintf(&buffer[0], t, ap);
		va_end(ap);
	}
	
	{
		int x=0,y=0;
		char * cptr = &buffer[0];
		
		while (*cptr != 0)
		{
			switch (*cptr)
			{
				case '\n':
				{
					y += CharVertSep;
					x = 0;
					break;
				}
				
				default:
				{
					BlitWin95Char (x, y, *cptr);

					x+=CharWidthInPixels(*cptr);
					if (x>LastDisplayableXForChars())
					{
						y += CharVertSep;
						x = 0;
					}

					break;
				}
			}
			
			cptr ++;
		}
		
		
	}
	


}
