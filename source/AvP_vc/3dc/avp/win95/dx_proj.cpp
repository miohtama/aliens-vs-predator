// Project specific parts of the Win95 DirectX system,
// largely concerned with the rules for selecting a driver,
// draw mode, memory mode etc during system initialisation, which
// cannot readily be decided at a project independent level.

// Must link to C code in main engine system

// Note that TestInitD3DObject is called during InitialiseSystem, immediately
// after initialisation of the basic DirectDraw object.  It is normally used 
// to detect the hardware acceleration capabilities of a system, by creating a Direct3D
// object which is then collapsed again, so that the information can be passed to e.g.
// menu software to provide recommended options.  The video mode can potentially 
// be rewritten here, so that a 320x200x8 request mode is interpreted as e.g. 640x480x15
// if hardware acceleration is detected and it is decided that it will be used.

// Note that the ProcessorType global should be valid at this stage.


// TestMemoryAccess is run immediately after TestInitD3DObject in InitialiseSystem,
// and is used to determine whether the back buffer should be in system or video memory.
// Note that if you intend to use MMX scan draws the back buffer should be in system memory,
// if you are running with hardware accelerated draws it MUST be in video memory, and otherwise
// I would recommend video memory except in extreme circumstances (i.e. very old video cards
// with very slow VRAM access).  Note that whether or not we will boot on hardware should have been
// decided by this stage.

// NOTE THAT THE REQUESTSYSTEMMEMORYALWAYS OPTION IS A DEBUGGING FEATURE, SINCE THE WATCOM
// DEBUGGER CAN ONLY BE USED ON FUNCTIOSN WITHIN DRAWITEMLISTCONTENTS ETC IF THE RENDERING 
// TARGET IS IN SYSTEM MEMORY AND THE SYSTEM DOES NOT HAVE TO TAKE A WIN16 LOCK TO
// REACH THE SURFACE.  THIS REQUEST MODE OPTION REPLACES FORCEBUFFERSINTOSYSTEMMEMORY, WHICH
// WAS AT THE TOP OF DD_FUNC.CPP

// SelectD3DDriverAndDrawMode is the final selection function for hardware acceleration, z buffering
// etc, run during SetVideoMode from InitialiseDirect3DImmediateMode.  Note that your target video
// mode must have been selected before you enter this routine.  This routine is separate so that a call
// to ChangeDisplayModes (in io.c) can be used to change the video mode, driver etc.

// NOTE: USE OF CHANGEDISPLAYMODES MUST BE INTEGRATED WITH USE OF THE CHUNK SYSTEM, SINCE IT WILL
// REQUIRE A TEXTURE RELOAD AND PROBABLY A SHAPE RELOAD.

// NOTE: IF YOU INTEND TO LET THE USER OVERRIDE VIDEO MODE, DRIVER ETC SETTINGS FROM MENUS RUN
// (UNAVOIDABLY, I SUSPECT) AFTER INITIALISESYSTEM, YOU MUST CALL SETVIDEOMODE BEFORE THE MENUS
// IF YOU WANT THEM TO BE DISPLAYED USING ENGINE (DIRECTX) FUNCTIONS AND THEN CALL CHANGEDISPLAYMODES
// TO CHANGE THE MODES TO THE FINAL ONES.  DO NOT CALL SETVIDEOMODE TWICE DURING A SINGLE PROGRAM RUN.
// CHANGEDISPLAYMODES WILL CALL YOUR VERSION OF SELECTD3DDRIVERANDDRAWMODE FOR YOU, BUT TESTMEMORYACCESS
// AND TESTINITD3DOBJECT WILL NOT BE CALLED AGAIN.  IF YOU WANT TO RESELECT THE MEMORY MODE OR RETEST
// HARDWARE ACCELERATION CAPABILITIES, YOU MUST DO IT YOURSELF IN THE MENUS SYSTEM.

// NOTE: TO CHANGE PALETTE IN A PALETTISED MODE, CALL CHANGEPALETTE IN IO.C, NOT CHANGEDISPLAYMODES.

extern "C" {

#include "system.h"
#include "equates.h"
#include "platform.h"
#include "shape.h"
#include "prototyp.h"
#include "inline.h"

#include "d3_func.h"
#include "d3dmacs.h"

extern "C++"
{
#include "pcmenus.h"
};

#include "dxlog.h"

// Tell system to prefer palettised formats
// for textures.  The alternative is to prefer
// non-palettised modes.
// Note that non-palettised modes certainly seem
// to look better on the software RGB driver, anyway,
// leading me to suspect that my runtime ppm quantiser
// is, ahem, less than totally perfect...
#define PreferPalettisedTextures Yes

// Test hack to force driver zero (mono
// rather than RGB), plus others.  Should
// all be No in theory.  Turn on only one
// at a time!!!
#define ForceRampDriver No
#define ForceRGBDriver No
#define ForceHardwareDriver No
#define ForceDirectDraw No

// Externs

extern int VideoMode;
extern int VideoModeColourDepth;
extern int WindowMode;
extern int DrawMode;
extern int ScanDrawMode;
extern int ZBufferMode;
extern int DXMemoryMode;
extern int DXMemoryRequestMode;
extern int ZBufferRequestMode;
extern int RasterisationRequestMode;
extern int SoftwareScanDrawRequestMode;
extern int D3DDriverMode;
extern BOOL D3DHardwareAvailable;
extern D3DINFO d3d;
extern int StartDriver;
extern int StartFormat;
extern HRESULT LastError;
extern BOOL MMXAvailable;
extern LPDIRECTDRAW lpDD;

// Globals
// Test only!!!
int TestHw = No;


// Callback function to enumerate texture
// formats available on the current D3D
// driver.  Obviously this should be run
// AFTER the driver callback, and after
// a driver has been selected.
// A palette will be picked as the default 
// (with preference given to palettized 
// formats) and returned via the lpContext.

HRESULT CALLBACK TextureFormatsEnumerator
        (LPDDSURFACEDESC lpDDSD, LPVOID lpContext)
{
    unsigned long m;
    int r, g, b;
    int *lpStartFormat = (int*) lpContext;

    /*
      Record the DDSURFACEDESC of this texture format
    */

    memset(&d3d.TextureFormat[d3d.NumTextureFormats], 0,
           sizeof(D3DTEXTUREFORMAT));
    memcpy(&d3d.TextureFormat[d3d.NumTextureFormats].ddsd, 
          lpDDSD, sizeof(DDSURFACEDESC));

    /*
      Is this format palettized?  How many bits?  Otherwise, how many RGB
      bits?
    */

    if (lpDDSD->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
	  {
       d3d.TextureFormat[d3d.NumTextureFormats].Palette = Yes;
       d3d.TextureFormat[d3d.NumTextureFormats].IndexBPP = 8;
      } 
    else if (lpDDSD->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) 
      {
       d3d.TextureFormat[d3d.NumTextureFormats].Palette = Yes;
       d3d.TextureFormat[d3d.NumTextureFormats].IndexBPP = 4;
      } 
    else if (lpDDSD->ddpfPixelFormat.dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS|DDPF_FOURCC))
      {
       return DDENUMRET_OK;
      } 
    else if (lpDDSD->ddpfPixelFormat.dwFlags & DDPF_RGB)
      {
	   // This bit is all fiendishly cunning, Dr Tringham,
	   // but one has to wonder if it actually, as such,
	   // works...
       d3d.TextureFormat[d3d.NumTextureFormats].Palette = No;
       d3d.TextureFormat[d3d.NumTextureFormats].IndexBPP = 0;
       for (r = 0, m = lpDDSD->ddpfPixelFormat.dwRBitMask; 
          !(m & 1); r++, m >>= 1);
       for (r = 0; m & 1; r++, m >>= 1);
       for (g = 0, m = lpDDSD->ddpfPixelFormat.dwGBitMask; 
          !(m & 1); g++, m >>= 1);
       for (g = 0; m & 1; g++, m >>= 1);
       for (b = 0, m = lpDDSD->ddpfPixelFormat.dwBBitMask; 
          !(m & 1); b++, m >>= 1);
       for (b = 0; m & 1; b++, m >>= 1);
       d3d.TextureFormat[d3d.NumTextureFormats].RedBPP = r;
       d3d.TextureFormat[d3d.NumTextureFormats].GreenBPP = g;
       d3d.TextureFormat[d3d.NumTextureFormats].BlueBPP = b;
      }
    else  
      {
       return DDENUMRET_OK;
      } 

    /*
      If lpStartFormat is -1, this is the first format.  
      Select it.
    */

    if (*lpStartFormat == -1)
      *lpStartFormat = d3d.NumTextureFormats;
	else
	{
		if (PreferTextureFormat(&d3d.TextureFormat[*lpStartFormat],&d3d.TextureFormat[d3d.NumTextureFormats]))
			*lpStartFormat = d3d.NumTextureFormats;
	}

    d3d.NumTextureFormats++;
    return DDENUMRET_OK;
}


// Code to pick a driver and scan draw mode
// based on what hardware is present on the
// system and the current request modes 
// (specified by default in the InitialVideoMode
// call).  
// The basic rules are that a hardware driver will
// always be picked if one is available unless
// software only rasterisation has been requested,
// in which case we will get a software driver anyway,
// and that hardware rgb scan draws will be picked if
// we are running on hardware, but otherwise the ramp
// software scan draw mode will be picked unless DirectDraw
// scan draws have been specifically requested.

// Note that since rewrites to the VideoMode really ought
// to be done BEFORE SetVideoMode is run, this function's
// effects must be combined with those of 
// InitialiseDirectDrawObject and TestInitD3DObject!!!!

void SelectD3DDriverAndDrawMode(void)

{
// Note that if we have requested default rasterisation and hw is 
// available, we may want to pick the software RGB driver if MMXAvailable
// is Yes!!! Fix later!!!
    if ((D3DHardwareAvailable) 
      && (RasterisationRequestMode != RequestSoftwareRasterisation))
	  {
	   int i=0;
	   BOOL EarlyExit = No;

       do 
	     {
		  if (d3d.Driver[i].Hardware)
		    {
		     d3d.CurrentDriver = i;
			 EarlyExit = Yes;
			}
		 }
	   while ((++i < d3d.NumDrivers) && !EarlyExit);

	   D3DDriverMode = D3DHardwareRGBDriver;
	   ScanDrawMode = ScanDrawD3DHardwareRGB;
	  }
// Note that we may want to select the software RGB driver if default scan
// draws have been requested, should MMXAvailable be Yes...
	else if (SoftwareScanDrawRequestMode == RequestScanDrawSoftwareRGB)
	  {
	   int i=0;
	   BOOL EarlyExit = No;

       do 
	     {
		  if ((!d3d.Driver[i].Hardware) &&
		    (d3d.Driver[i].Desc.dcmColorModel == D3DCOLOR_RGB))
		    {
		     d3d.CurrentDriver = i;
			 EarlyExit = Yes;
			}
		 }
	   while ((++i < d3d.NumDrivers) && !EarlyExit);

	   D3DDriverMode = D3DSoftwareRGBDriver;
	   ScanDrawMode = ScanDrawD3DSoftwareRGB;
	  }
// Note that once everything, like, works, and Microsoft's code
// is dead fast and doesn't crash or anything, the default software
// scan draw mode should be changed from DirectDraw to Ramp.
// Probably.
	else if (SoftwareScanDrawRequestMode == RequestScanDrawRamp)
	  {
	   int i=0;
	   BOOL EarlyExit = No;

       do 
	     {
		  if ((!d3d.Driver[i].Hardware) &&
		    (d3d.Driver[i].Desc.dcmColorModel == D3DCOLOR_MONO))
		    {
		     d3d.CurrentDriver = i;
			 EarlyExit = Yes;
			}
		 }
	   while ((++i < d3d.NumDrivers) && !EarlyExit);

	   D3DDriverMode = D3DSoftwareRampDriver;
	   ScanDrawMode = ScanDrawD3DRamp;
	  }
	else if ((SoftwareScanDrawRequestMode == RequestScanDrawDirectDraw)
	     || (SoftwareScanDrawRequestMode == RequestScanDrawDefault))
	  {
	   int i=0;
	   BOOL EarlyExit = No;

       // Set ramp driver anyway for convenience
       do 
	     {
		  if ((!d3d.Driver[i].Hardware) &&
		    (d3d.Driver[i].Desc.dcmColorModel == D3DCOLOR_MONO))
		    {
		     d3d.CurrentDriver = i;
			 EarlyExit = Yes;
			}
		 }
	   while ((++i < d3d.NumDrivers) && !EarlyExit);

	   D3DDriverMode = D3DSoftwareRampDriver;
	   ScanDrawMode = ScanDrawDirectDraw;
	  }
	else // bollocks
	  {
	   ReleaseDirect3D();
	   exit(0xdeaf);
	  }

    #if SupportZBuffering
    if (ZBufferRequestMode == RequestZBufferAlways)
	  {
	   if (d3d.Driver[d3d.CurrentDriver].ZBuffer)
	     ZBufferMode = ZBufferOn;
	   else
	     ZBufferMode = ZBufferOff;
	  }
	else if (ZBufferRequestMode == RequestZBufferDefault)
	  {
	   if ((d3d.Driver[d3d.CurrentDriver].ZBuffer) 
	      && (d3d.Driver[d3d.CurrentDriver].Hardware))
	     ZBufferMode = ZBufferOn;
	   else
	     ZBufferMode = ZBufferOff;
	  }
	else
	  ZBufferMode = ZBufferOff;
    #endif

	
    // Overrides for test purposes only

    #if ForceRampDriver
    d3d.CurrentDriver = 0;
	D3DDriverMode = D3DSoftwareRampDriver;
	ScanDrawMode = ScanDrawD3DRamp;
    #endif

    #if ForceRGBDriver
    d3d.CurrentDriver = 1;
	D3DDriverMode = D3DSoftwareRGBDriver;
	ScanDrawMode = ScanDrawD3DSoftwareRGB;
    #endif

    #if ForceHardwareDriver
    d3d.CurrentDriver = 2;
	D3DDriverMode = D3DHardwareRGBDriver;
	ScanDrawMode = ScanDrawD3DHardwareRGB;
    #endif

    #if ForceDirectDraw
    d3d.CurrentDriver = 0;
	D3DDriverMode = D3DSoftwareRampDriver;
	ScanDrawMode = ScanDrawDirectDraw;
    #endif

}

#if SUPPORT_MMX
int use_mmx_math;
void SelectMMXOptions(void)
{
	if (MMXAvailable)
		use_mmx_math = 1;
	else
		use_mmx_math = 0;
}
#endif

// Initialise temporary D3D object and then destroy
// it again, SOLELY for the purpose of determining
// whether hardware acceleration is available

// NOTE THIS -->MUST<-- BE RUN -->AFTER<--
// InitialiseDirectDrawObject!!!

BOOL TestInitD3DObject(void)
{
    // Zero hardware available global
	D3DHardwareAvailable = No;

// Zero d3d structure
    memset(&d3d, 0, sizeof(D3DINFO));

//  Set up Direct3D interface object
    LastError = lpDD->QueryInterface(IID_IDirect3D, (LPVOID*) &d3d.lpD3D);

    if (LastError != DD_OK)
	  return FALSE;

// Use callback function to enumerate available devices on system
// and acquire device GUIDs etc

    LastError = d3d.lpD3D->EnumDevices(DeviceEnumerator, (LPVOID)&StartDriver);

    if (LastError != D3D_OK)
      return FALSE;
	  
	// select the usual driver - get its description
	int old_rrm = RasterisationRequestMode;
	RasterisationRequestMode = RequestDefaultRasterisation;
	SelectD3DDriverAndDrawMode();
	d3d.ThisDriver = d3d.Driver[d3d.CurrentDriver].Desc;
	RasterisationRequestMode = old_rrm;

// THE TEXTURE FORMATS ENUMERATOR COULD ALSO BE CALLED HERE TO DETERMINE
// WHETHER PALETTISED FORMATS ETC ARE AVAILABLE.

// NOTE THAT GETAVAILABLEVIDEOMEMORY (IN DD_FUNC.CPP) CAN BE USED TO REPORT
// THE FREE VIDEO MEMORY AT ANY TIME.


// Eliminate Direct3D object again
   RELEASE(d3d.lpD3D);

// Code to reset video mode depending on 
// presence of hardware acceleration should 
// go here.  NOTE THAT AT A LATER DATE THIS CODE 
// MUST ALSO TAKE ACCOUNT OF THE SETTING OF 
// MMXAVAILABLE AND SUCH INFORMATION IN THE D3DINFO
// STRUCTURE AS DOES THE DRIVER DO TEXTURES
// OR Z-BUFFERING.  FIXME!!!

   #if 0
   if ((D3DHardwareAvailable) && 
     (RasterisationRequestMode == RequestDefaultRasterisation))
     {
	  if ((VideoMode == VideoMode_DX_320x200x8) ||
	     (VideoMode == VideoMode_DX_320x240x8))
		{
		 if (CheckForVideoModes(VideoMode_DX_320x200x15))
		   {
		    VideoMode = VideoMode_DX_320x200x15;
			return TRUE;
		   }
		 else if (CheckForVideoModes(VideoMode_DX_640x480x15))
		   {
		    VideoMode = VideoMode_DX_640x480x15;
			return TRUE;
		   }
         else
		   return FALSE;
		}
	  else if (VideoMode == VideoMode_DX_640x480x8)
	    {
		 if (CheckForVideoModes(VideoMode_DX_640x480x15))
		   {
		    VideoMode = VideoMode_DX_640x480x15;
			return TRUE;
		   }
		 else
		   return FALSE;
		}
	 }
   #else
   // test only!!!
     {
	  if (D3DHardwareAvailable)
        TestHw = Yes;
	  else
	    TestHw = No;
	 }
   #endif

   return TRUE;
}

// Function to test access speed of VRAM
// against system memory using the DD blt
// members, and set DXMemoryMode using this
// result and the DXMemoryRequestMode as a basis.
// Note that we must allow for bank flipping
// speed.
// Potentially we should also test processor
// copy against video card blitter if the blitter
// is in hardware, since some blitters can be slower
// than the system processor.

// THIS FUNCTION HAS NOT YET BEEN DONE, SEEING AS IT
// DOESN'T ACTUALLY MAKE MUCH DIFFERENCE TO THE SPEED
// IN PRACTICE.

// NOTE ALSO that DXMemoryMode has itself 
// not yet been fully implemented, since with
// all buffers forced into system memory DD
// doesn't seem to work properly anyway...

// NOTE, HOWEVER, THAT THIS FUNCTION MUST READ
// MMXAvailable and D3DHardwareAvailable,
// since if MMX driver is picked we want the
// back buffer in SYSTEM memory, not video
// (as of 26 / 7 / 96).

// MUST PUT BACK BUFFER IN VIDEO MEMORY ON
// AN ACCELERATOR!!!

BOOL TestMemoryAccess(void)

{
    // We take this request AT IT'S WORD, i.e. to mean
	// that we will always do our best to provide a system
	// memory target.  Note that this means that if we then 
	// go on to boot up on hardware nothing will actually appear
	// on the screen, though the debugger should work (in SubWindow
	// mode, obviously).  A more subtle test might be appropriate here,
	// though it IS a debugging thing...
	if (DXMemoryRequestMode == RequestSystemMemoryAlways)
	  DXMemoryMode = SystemMemoryPreferred;
	else // assume default
	  {
	// Note that at this stage we have a DirectDraw object on which
	// to run GetCaps, we have detected the processor type and we have run
	// TestInitD3DObject, so we have determined whether or not hardware
	// acceleration is available.

    // Rendering target must be in VRAM if we are to run accelerated.
	// Note that we really want to know whether we are going to run
	// accelerated, not whether we can do, but this requires integration
	// with the project's rules for deciding on its running mode.
	// This function should probably be moved to d3_proj.cpp along
	// with SelectD3DDriverAndMode and TestInitD3DObject...
	   if (D3DHardwareAvailable)
	     DXMemoryMode = VideoMemoryPreferred;
	// For MMX we are currently assuming a high scan draw throughput, 
	// as VideoModeType_15 is likely to be the optimal solution, which
	// is liable to result in a system gated by VRAM speed for a video
	// memory rendering target.  For this system we therfore want a system
	// memory target.
	   else if (MMXAvailable)
	     DXMemoryMode = SystemMemoryPreferred;
	// We are now assuming a DirectDraw software solution with our
	// scan draws, on a standard(ish) video card.  At this point we need
	// to make an intelligent decision, remembering that in practice
	// system memory targets are rarely faster than video memory ones
	// for this situation.
	   else
	     {
		    // On the basis of experiment, these are the important
			// video card capabilities for 3dc.
		 	BOOL Flip;
			BOOL BlitSysMem;
			BOOL BlitVidMem;
			BOOL BlitFloodFill;
			BOOL BlitAsyncQueue;
			BOOL BankSwitched;
			// Timing factors
			int TimeForVramWrite;
			int TimeForSysMemWrite;
			unsigned char* SurfacePtr;
            // DirectX intfc
            DDCAPS              ddcaps;
            HRESULT             ddrval;
			LPDIRECTDRAWSURFACE lpTestSurf;
			DDSURFACEDESC       ddsd;
			// etc
			int i;
			int TestSurfHeight = 480;
			int TestSurfWidth = 640;

            // Get caps on the DirectDraw object
            memset(&ddcaps, 0, sizeof(ddcaps));
            ddcaps.dwSize = sizeof(ddcaps);
            ddrval = lpDD->GetCaps(&ddcaps, NULL);
			LOGDXERR(ddrval);

  			if (ddrval != DD_OK)
  			#if debug
     		   {
                ReleaseDirect3D();
                exit(ddrval);
     		   }
  			#else
			   {
     		    return FALSE;
			   }
  			#endif

            if (ddcaps.dwCaps & DDCAPS_BLT)
			  BlitVidMem = TRUE;
			else
			  BlitVidMem = FALSE;

            if (ddcaps.dwCaps & DDCAPS_BANKSWITCHED)
			  BankSwitched = TRUE;
			else
			  BankSwitched = FALSE;

            if (ddcaps.dwCaps & DDCAPS_BLTCOLORFILL)
			  BlitFloodFill = TRUE;
			else
			  BlitFloodFill = FALSE;

            if (ddcaps.dwCaps & DDCAPS_BLTQUEUE)
			  BlitAsyncQueue = TRUE;
			else
			  BlitAsyncQueue = FALSE;

            if (ddcaps.dwCaps & DDCAPS_CANBLTSYSMEM)
			  BlitSysMem = TRUE;
			else
			  BlitSysMem = FALSE;

            if (ddcaps.ddsCaps.dwCaps & DDSCAPS_FLIP)
			  Flip = TRUE;
			else
			  Flip = FALSE;
     
	        // Creating surfaces like this before setting exclusive mode
			// und so weiter seems to REALLY UPSET IT.  So for now, we
			// shall say sod it and decide purely on the basis of the caps.
	        #if 0
	        // Create a DD surface in Vram
            memset(&ddsd,0,sizeof(DDSURFACEDESC));
            ddsd.dwSize = sizeof(DDSURFACEDESC);
            ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			// 640 x 480 would be a typical screen
            ddsd.dwHeight = TestSurfHeight;
            ddsd.dwWidth = TestSurfWidth;
            ddsd.ddsCaps.dwCaps= (DDSCAPS_OFFSCREENPLAIN);

            ddrval = lpDD->CreateSurface(&ddsd, &lpTestSurf, NULL);
			LOGDXERR(ddrval);
            if (ddrval != DD_OK)
	        #if debug
		      {
		       ReleaseDirect3D();
	           exit(ddrval);
	          }
	        #else
	        return FALSE;
	        #endif

            SurfacePtr = (unsigned char*) ddsd.lpSurface;

            // Time write
			TimeForVramWrite = timeGetTime();
			for (i=0; i<TestSurfHeight; i++)
			  {
			   memset(SurfacePtr, 0, TestSurfWidth);
			   SurfacePtr += ddsd.lPitch;
			  }
			TimeForVramWrite = (timeGetTime() - TimeForVramWrite);

            // Kill the surface
            ReleaseDDSurface((void*) lpTestSurf);
            
	        // Create a DD surface in System Memory
            memset(&ddsd,0,sizeof(DDSURFACEDESC));
            ddsd.dwSize = sizeof(DDSURFACEDESC);
            ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
			// 640 x 480 would be a typical screen
            ddsd.dwHeight = TestSurfHeight;
            ddsd.dwWidth = TestSurfWidth;
            ddsd.ddsCaps.dwCaps= (DDSCAPS_OFFSCREENPLAIN 
                                 | DDSCAPS_SYSTEMMEMORY);

            ddrval = lpDD->CreateSurface(&ddsd, &lpTestSurf, NULL);
			LOGDXERR(ddrval);
            if (ddrval != DD_OK)
	        #if debug
		      {
		       ReleaseDirect3D();
	           exit(ddrval);
	          }
	        #else
	        return FALSE;
	        #endif

            SurfacePtr = (unsigned char*) ddsd.lpSurface;

            // Time write
			TimeForSysMemWrite = timeGetTime();
			for (i=0; i<TestSurfHeight; i++)
			  {
			   memset(SurfacePtr, 0, TestSurfWidth);
			   SurfacePtr += ddsd.lPitch;
			  }
			TimeForSysMemWrite = (timeGetTime() - TimeForSysMemWrite);

            ReleaseDDSurface((void*) lpTestSurf);
            #endif

			// Now decide!!!!
			// Benchmarking memory speed at this point in the initialisation
			// sequence seems tricky, so for now we shall simply try deciding on
			// the basis of the caps, seeing as video memory always seems faster
			// even on dubious 1992 cards anyway.  This should probably be fixed,
			// though.
			if (Flip || BlitSysMem || BlitFloodFill || BlitAsyncQueue)
			  DXMemoryMode = VideoMemoryPreferred;
			else
			  DXMemoryMode = SystemMemoryPreferred;
		 }
	  }

	return TRUE;
}


// For extern "C"

};

