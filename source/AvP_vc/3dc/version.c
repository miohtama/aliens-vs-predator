/* KJL 16:41:33 29/03/98 - not the most complicated code I've ever written... */
#include "version.h"
extern void NewOnScreenMessage(unsigned char *messagePtr);


void GiveVersionDetails(void)
{
	/* KJL 15:54:25 29/03/98 - give version details; this is not language localised since I thought that would be a little odd */
//	NewOnScreenMessage("ALIENS VS PREDATOR - DEATHMATCH DEMO V1.0 - REBELLION DEVELOPMENTS 1998/11/28");
//	NewOnScreenMessage("Aliens vs Predator - Alien Demo V1.3 - Rebellion Developments 1999/1/25");
//	NewOnScreenMessage("Aliens vs Predator - \n   Build 103 \n   Rebellion Developments 1999/7/13 \n   (European Release)");
//	NewOnScreenMessage("Aliens vs Predator - \n   Build 103 \n   Rebellion Developments 1999/7/13 \n   (American Release)");
	NewOnScreenMessage("Aliens vs Predator - Gold\n   Build 116 \n   Rebellion Developments 2000/16/3 \n"); //11:25

}
