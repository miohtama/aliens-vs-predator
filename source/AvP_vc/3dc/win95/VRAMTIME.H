#ifndef _included_vramtime_h_
#define _included_vramtime_h_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum VramWaitState
{
	VWS_DDRELEASE,
	VWS_D3DTEXRELEASE,
	VWS_DDCREATE,
	VWS_D3DTEXCREATE,
	VWS_UNKNOWN,
	VWS_MAXSTATES

} VRAM_WAIT_STATE;

void WaitForVRamReady(VRAM_WAIT_STATE);

#ifdef __cplusplus
}
#endif

#endif /* ! _included_vramtime_h_ */
