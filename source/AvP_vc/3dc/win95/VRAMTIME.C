#include <windows.h>

#include "vramtime.h"

static DWORD transition_times_matrix[][VWS_MAXSTATES] =
{
/* from DDRELEASE */
	{   0,  20,   0, 100, 100 },
/* from D3DTEXRELEASE */
	{  20,   0, 100, 100, 100 },
/* from DDCREATE */
	{   0, 100,   0,  20, 100 },
/* from D3DTEXCREATE */
	{  20, 100,  20,  20, 100 },
/* from UNKNOWN */
	{ 100, 100, 100, 100, 100 }
};


void WaitForVRamReady(VRAM_WAIT_STATE vws)
{
	static DWORD old_time = 0;
	static VRAM_WAIT_STATE old_vws = VWS_UNKNOWN;
	
	DWORD new_time;

	if (0==old_time) old_time = timeGetTime();

	do new_time = timeGetTime(); while (new_time - old_time < transition_times_matrix[old_vws][vws]);

	old_time = new_time;
	old_vws = vws;
}
