/****

Project specific (or potentially 
project specific) windows functionality

****/

// To link code to main C functions 

extern "C" {

#include "3dc.h"
#include "inline.h"
#include "cd_player.h"
#include "psndplat.h"

#include "rentrntq.h"
	// Added 21/11/97 by DHM: support for a queue of Windows
	// messages to avoid problems with re-entrancy due to WinProc()
	
#include "alt_tab.h"
	
#include "dxlog.h"
#include "zmouse.h"

void MakeToAsciiTable(void);

// mousewheel msg id
UINT const RWM_MOUSEWHEEL = RegisterWindowMessage(MSH_MOUSEWHEEL);
signed int MouseWheelStatus;

extern LPDIRECTDRAWSURFACE lpDDSBack;
extern LPDIRECTDRAWSURFACE lpDDSPrimary;
extern LPDIRECTDRAWSURFACE lpZBuffer;
extern LPDIRECTDRAWSURFACE lpDDBackdrop;

unsigned char ksarray[256];
unsigned char ToAsciiTable[256][256];

// Dubious
#define grabmousecapture No

/*
	Name of project window etc for Win95 interface
	Project specific (fairly obviously...).
	Determines the default menu in which the application
	appears (altho' other code will undoubtedly be needed
	as well...), so that a NULL here should ensure no menu.
*/

#define NAME "AvP"
#define TITLE "AvP"

//	Necessary globals

HWND 		hWndMain;
BOOL        bActive = TRUE;        // is application active?

// Parameters for main (assumed full screen) window
int WinLeftX, WinRightX, WinTopY, WinBotY;
int WinWidth, WinHeight;

// Externs



extern int VideoMode;
extern int WindowMode;
extern WINSCALEXY TopLeftSubWindow;
extern WINSCALEXY ExtentXYSubWindow;

// Window procedure (to run continuously while WinMain is active).
// Only necessary functions are handling keyboard input (for the moment
// at least - cf. DirectInput) and dealing with important system 
// messages, e.g. WM_PAINT.

// Remember to support all the keys you need for your project
// for both KEYUP and KEYDOWN messages!!!

// IMPORTANT!!! The WindowProc is project specific
// by default, since various nifty hacks can always
// be implemented directly via the windows procedure

#define RESTORE_SURFACE(lpDDS) { \
	if (lpDDS) { \
		if (DDERR_SURFACELOST == (lpDDS)->IsLost()) { \
    		HRESULT hResult = (lpDDS)->Restore(); \
    		LOGDXFMT(("%s surface was restored", #lpDDS )); \
    		LOGDXERR(hResult); \
    	} else { \
    		LOGDXFMT(("%s surface wasn't lost", #lpDDS )); \
    	} \
	} \
	else { \
     	LOGDXFMT(("?&@#! no %s surface", #lpDDS )); \
	} \
}

extern void KeyboardEntryQueue_Add(char c);
extern IngameKeyboardInput_KeyDown(unsigned char key);
extern IngameKeyboardInput_KeyUp(unsigned char key);
extern IngameKeyboardInput_ClearBuffer(void);


long FAR PASCAL WindowProc(HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC         hdc;
	RECT		NewWindCoord;

 	if (message==RWM_MOUSEWHEEL)
	{
		message = WM_MOUSEWHEEL;
		wParam <<= 16;
	}


	switch(message)
    {

		case WM_MOUSEWHEEL:
		{
			MouseWheelStatus = wParam;
			MouseWheelStatus>>=16;
			return 0;
		}	
	
	 // 21/11/97 DHM: Added porcessing of WM_CHAR messages:
	 case WM_CHAR:
		RE_ENTRANT_QUEUE_WinProc_AddMessage_WM_CHAR
		(
			(char) wParam
		);
		KeyboardEntryQueue_Add((char)wParam);
		return 0;
	case WM_KEYDOWN:
		RE_ENTRANT_QUEUE_WinProc_AddMessage_WM_KEYDOWN
		(
			wParam
		);
		// it's intentional for this case to fall through to WM_SYSKEYDOWN
    case WM_SYSKEYDOWN:
		{
			int scancode = (lParam>>16)&255;
			unsigned char vkcode = (wParam&255);
		
			// ignore the status of caps lock
			//ksarray[VK_CAPITAL] = 0;	
		 	//ksarray[VK_CAPITAL] = GetKeyState(VK_CAPITAL);	
			if (vkcode!=VK_CAPITAL && vkcode!=VK_SCROLL)
			{
			 	#if 0
				WORD output;
			 	if (ToAscii(vkcode,scancode,&ksarray[0],&output,0))
				{
					IngameKeyboardInput_KeyDown((unsigned char)(output));
				}
				#else
				if (ToAsciiTable[vkcode][scancode])
				{
					IngameKeyboardInput_KeyDown(ToAsciiTable[vkcode][scancode]);
				}
				#endif
			}
			// reset caps lock status
			//ksarray[VK_CAPITAL] = GetKeyState(VK_CAPITAL);	
			//ToAscii(wParam&255,scancode,&ksarray[0],&output,0);
		}
		return 0;

	case WM_SYSKEYUP:
	case WM_KEYUP:						
		{
			int scancode = (lParam>>16)&255;
			unsigned char vkcode = (wParam&255);

			
			// ignore the status of caps lock
			//ksarray[VK_CAPITAL] = 0;	
//MakeToAsciiTable();			
		  	//ksarray[VK_CAPITAL] = GetKeyState(VK_CAPITAL);	
			if (vkcode!=VK_CAPITAL && vkcode!=VK_SCROLL)
			{
				#if 0
			 	WORD output;
			 	unsigned char z = ToAscii(vkcode,scancode,&ksarray[0],&output,0);
				unsigned char a = (unsigned char)output;
				unsigned char b = ToAsciiTable[vkcode][scancode];
				#endif
				#if 0
				WORD output;
			 	if (ToAscii(vkcode,scancode,&ksarray[0],&output,0))
				{
					IngameKeyboardInput_KeyUp((unsigned char)(output));
				}
				#else
				if (ToAsciiTable[vkcode][scancode])
				{
					IngameKeyboardInput_KeyUp(ToAsciiTable[vkcode][scancode]);
				}
				#endif
			}
			// reset caps lock status
			//ksarray[VK_CAPITAL] = GetKeyState(VK_CAPITAL);	
			//ToAscii(wParam&255,scancode,&ksarray[0],&output,0);
		}
		return 0;
		 
	 // This, in combination with code in win_func,
	 // will hopefully disable Alt-Tabbing...
     case WM_ACTIVATEAPP:
        bActive = (BOOL) wParam;
        
        LOGDXFMT(("WM_ACTIVATEAPP msg: bActive = %d",(int)bActive));
        
        if (bActive)
        {
        	// need to restore all surfaces - do the special ones first
        	RESTORE_SURFACE(lpDDSPrimary)
        	RESTORE_SURFACE(lpDDSBack)
        	RESTORE_SURFACE(lpZBuffer)
        	// dodgy, this is meant to be graphic, so it'll really need to be reloaded
        	RESTORE_SURFACE(lpDDBackdrop)
        	// now do all the graphics surfaces and textures, etc.
        	ATOnAppReactivate();
        }
		IngameKeyboardInput_ClearBuffer();
        
		return 0;

     // Three below are for safety, to turn off
	 // as much as possible of the more annoying 
	 // functionality of the default Windows 
	 // procedure handler

	 case WM_ACTIVATE:
	    return 0;
#if 0
     case WM_SYSKEYUP:
	    return 0;

	case WM_SYSKEYDOWN:
	    return 0;
#endif
     case WM_CREATE:
        break;

     case WM_MOVE:
	    // Necessary to stop it crashing in 640x480
		// FullScreen modes on window initialisation
	    if (WindowMode == WindowModeSubWindow)
		  {
	       GetWindowRect(hWndMain, &NewWindCoord);
		   WinLeftX = NewWindCoord.left;
		   WinTopY = NewWindCoord.top;
		   WinRightX = NewWindCoord.right;
		   WinBotY = NewWindCoord.bottom;
		  }
	    break;

     case WM_SIZE:
	    // Necessary to stop it crashing in 640x480
		// FullScreen modes on window initialisation
	    if (WindowMode == WindowModeSubWindow)

		  {
	       GetWindowRect(hWndMain, &NewWindCoord);
		   WinLeftX = NewWindCoord.left;
		   WinTopY = NewWindCoord.top;
		   WinRightX = NewWindCoord.right;
		   WinBotY = NewWindCoord.bottom;
		  }
	    break;

     case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

	 case WM_ERASEBKGND:
        return TRUE;

     case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return TRUE;

	 /* Patrick 11/6/97: this to detects the end of a cdda track */
	 case MM_MCINOTIFY:
		PlatCDDAManagementCallBack(wParam, lParam);
	 	break;

     case WM_DESTROY:
	 // Calls ReleaseDirect3D DIRECTLY,
	 // so as to avoid calling ExitSystem and exiting the
	 // Windows system inside the windows procedure
	 // IMPORTANT!!! For this to work, release functions 
	 // must be re-entrant.  Since this may be causing
	 // problems under obscure cirumstances, I am removing 
	 // this now (25/7/96).
	 // And putting it back... (20/9/96)
    	ReleaseDirect3D();
	 	/* patrick 9/6/97: hmmmmm.... */  	
	   	PlatEndSoundSys();

	   	PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
	Persuade Win95 to give us the window we want and, like,
	TOTAL CONTROL, and then shut up, stop whinging and
	go away.
	Or at least as much control as we can get.. safely...
	elegantly... ummm...
*/

// IMPORTANT!!! Windows initialisation is project specific,
// because of the project name and title if nothing else

// This function now takes a mode which is
// set to full or change.  Full should be
// run ONLY when the system is starting.  
// Change is used to change the window 
// characteristics during a run, e.g. to
// change from SubWindow to FullScreen
// mode, and will not attempt to register
// the windows class.

BOOL InitialiseWindowsSystem(HANDLE hInstance, int nCmdShow,
     int WinInitMode)
{
    WNDCLASS            wc;
	BOOL			    rc;

	MakeToAsciiTable();
/*
	Set up the width and height we want from
	the VideoMode, taking account of WindowMode.
*/

// This has now been modified to just set the
// size to the current system metrics, which
// may or may not be ideal.  Surprisingly, it
// seems not to make much difference.

   if (WindowMode == WindowModeSubWindow)
     {
	  
	  //force window to be 640x480 to avoid stretch blits.
	  WinWidth=640;
	  WinHeight=480;	

      WinLeftX = (int) (TopLeftSubWindow.x * 
	     (float) GetSystemMetrics(SM_CXSCREEN));
	  WinTopY = (int) (TopLeftSubWindow.y *
	     (float) GetSystemMetrics(SM_CYSCREEN));
	  WinRightX = (WinLeftX + WinWidth);
	  WinBotY = (WinTopY + WinHeight);
     }
   else if (WindowMode == WindowModeFullScreen)
     {
      #if 1
      WinWidth = GetSystemMetrics(SM_CXSCREEN);
      WinHeight = GetSystemMetrics(SM_CYSCREEN);
      #else
      // This version of the code MUST be
      // kept up to date with new video modes!!!
      if ((VideoMode == VideoMode_DX_320x200x8) ||
        (VideoMode == VideoMode_DX_320x200x8T))
	     {
	      WinWidth = 320;
	      WinHeight = 200;
	     }
      else if (VideoMode == VideoMode_DX_320x240x8)
         {
	      WinWidth = 320;
	      WinHeight = 240;
	     }
      else // Default to 640x480
         {
	      WinWidth = 640;
	      WinHeight = 480;
	     }
      #endif

      // Set up globals for window corners
      WinLeftX = 0;
      WinTopY = 0;
      WinRightX = WinWidth;
      WinBotY = WinHeight;
	 }
   else
     return FALSE;

// We only want to register the class in
// WinInitFull mode!!!

   if (WinInitMode == WinInitFull)
     {

/*
	Set up and register window class
*/
// get double click messages from mouse if user double-clicks it 
      wc.style = CS_DBLCLKS;
// Name of window procedure (see above) 
      wc.lpfnWndProc = WindowProc;
/*
	Extra bytes for  obscure purposes bearing a sordid relationship to
	dialog box conventions.  Zero for us.
*/
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
// Instance which window is within 
      wc.hInstance = (HINSTANCE) hInstance;
/*
	System icon resource.  This one is generic.  For an actual
	game this icon will be project specific.
*/
      #if 1
      wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	  #else
      wc.hIcon = NULL;
	  #endif
// System cursor resource.  This one is generic.
      #if 1
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	  #else
      wc.hCursor = NULL;
	  #endif
/*
	NULL background forces application to redraw
	the background ITSELF when it receives a WM_ERASEBKGND
	message, leaving graphical control with the engine
*/
      wc.hbrBackground = NULL;
// Project name and class for windows menus etc.
      wc.lpszMenuName = NAME;
      wc.lpszClassName = NAME;
/*
	Register the class we have constructed as a valid window that
	can then be created.   Return code indicates success or
	failure.
*/
      rc = RegisterClass(&wc);

	  if (!rc)
		return FALSE;
	 }

/*
  Create a window  (extended function  call) and return
  handle.  Before  returning, WM_CREATE, WM_GETMINMAXINFO
  and WM_NCCREATE messages will be sent to the window
  procedure.
 
  NOTE!!! AT present even with debug on we get a
  topmost full screen window.
*/

#if debug
    if (WindowMode == WindowModeSubWindow)
	  {
       hWndMain = CreateWindowEx(
           0, // WS_EX_TOPMOST
		   NAME, //  Name of class (registered by RegisterClass call above) 
           TITLE, // Name of window 
		   WS_OVERLAPPED |
		   WS_CAPTION |
		   WS_THICKFRAME,
/*
	Initial horizontal and  vertical position.  For a pop-up window,
	these are the coordinates of the upper left corner.
*/
           WinLeftX,
           WinTopY,
/*
	Width and height of window. These are set to the current full
	screen widths as determined by a Win32 GetSystemMetrics call
	(GetSystemMetrics(SM_CXSCREEN) and 
	GetSystemMetrics(SM_CYSCREEN)).
*/
           WinWidth,
		   WinHeight,
// Parent window (could possibly be set in tools system?)
           NULL,
// Child/menu window (could possibly be set in tools system?) 
           NULL,
// Handle for module associated with window 
           (HINSTANCE)hInstance,
// Parameter for associated structure (null in this case)
           NULL);
	  }
	else if (WindowMode == WindowModeFullScreen)
	  {
       hWndMain = CreateWindowEx(
/*
	WS_EX_TOPMOST forces this window to be topmost except
	for other topmost	windows, even when deactivated.
*/
           WS_EX_TOPMOST,
		   NAME, //  Name of class (registered by RegisterClass call above) 
           TITLE, // Name of window 
		   WS_VISIBLE | // kills Alt-Space and strews its entrails over fifteen miles.  Heh heh heh.
           WS_POPUP, // i.e. specify window is style pop up, i.e. non-application 
/*
	Initial horizontal and  vertical position.  For a pop-up window,
	these are the coordinates of the upper left corner.
*/
           WinLeftX,
           WinTopY,
/*
	Width and height of window. These are set to the current full
	screen widths as determined by a Win32 GetSystemMetrics call
	(GetSystemMetrics(SM_CXSCREEN) and 
	GetSystemMetrics(SM_CYSCREEN)).
*/
           WinWidth,
		   WinHeight,
// Parent window (null for a full screen game) 
           NULL,
// Child/menu window (null for a full screen game) 
           NULL,
// Handle for module associated with window 
           (HINSTANCE)hInstance,
// Parameter for associated structure (null in this case)
           NULL);
      }
	else
	  return FALSE;

#else
    if (WindowMode == WindowModeSubWindow)
	  {
       hWndMain = CreateWindowEx(
          0, // WS_EX_TOPMOST
          NAME, //  Name of class (registered by RegisterClass call above) 
          TITLE, // Name of window
		  WS_OVERLAPPED |
		  WS_CAPTION |
		  WS_THICKFRAME,
/*
	Initial horizontal and  vertical position.  For a pop-up window,
	these are the coordinates of the upper left corner.
*/
          WinLeftX,
          WinTopY,
/*
	Width and height of window. These are set to the current full
	screen widths as determined by a Win32 GetSystemMetrics call
	(GetSystemMetrics(SM_CXSCREEN) and 
	GetSystemMetrics(SM_CYSCREEN)).
*/
          WinWidth,
		  WinHeight,
// Parent window (could be set in tools system?) 
          NULL,
// Child/menu window (could be set in tools system?) 
          NULL,
// Handle for module associated with window 
          hInstance,
// Parameter for associated structure (null in this case) 
          NULL);
	  }
	else if (WindowMode == WindowModeFullScreen)
	  {
       hWndMain = CreateWindowEx(
/*
	WS_EX_TOPMOST forces this window to be topmost except
	for other topmost	windows, even when deactivated.
*/
          WS_EX_TOPMOST,
          NAME, //  Name of class (registered by RegisterClass call above) 
          TITLE, // Name of window
		  WS_VISIBLE | // kills Alt-Space and strews its entrails for fifteen miles.  Heh heh heh.
          WS_POPUP, // Specify window is style pop up, i.e. non-application 
/*
	Initial horizontal and  vertical position.  For a pop-up window,
	these are the coordinates of the upper left corner.
*/
          WinLeftX,
          WinTopY,
/*
	Width and height of window. These are set to the current full
	screen widths as determined by a Win32 GetSystemMetrics call
	(GetSystemMetrics(SM_CXSCREEN) and 
	GetSystemMetrics(SM_CYSCREEN)).
*/
          WinWidth,
		  WinHeight,
// Parent window (null for a full screen game) 
          NULL,
// Child/menu window (null for a full screen game) 
          NULL,
// Handle for module associated with window 
          hInstance,
// Parameter for associated structure (null in this case) 
          NULL);
	  }
	else
	  return FALSE;

#endif

    if (!hWndMain)
      return FALSE;

// Experiment only!!!!

// Set the window up to be displayed 
    ShowWindow(hWndMain, nCmdShow);
// Update once (i.e. send WM_PAINT message to the window procedure) 
    UpdateWindow(hWndMain);

// Grab ALL mouse messages for our window.
// Note this will only work if the window is
// foreground (as it is... ).  This ensures that
// we will still get MOUSEMOVE etc messages even
// if the mouse is out of the defined window area.

    #if grabmousecapture
    SetCapture(hWndMain);
// Load null cursor shape
	SetCursor(NULL);
	#endif
	MakeToAsciiTable();

    return TRUE;

}

// Project specific to go with the initialiser

BOOL ExitWindowsSystem(void)

{
   BOOL rc = TRUE;

   // Release dedicated mouse capture
   #if grabmousecapture
   ReleaseCapture();
   #endif

   rc = DestroyWindow(hWndMain);

   return rc;
}

void MakeToAsciiTable(void)
{
	WORD output;
	for (int k=0; k<=255; k++)
	{
		ksarray[k]=0;
	}

	for (int i=0; i<=255; i++)
	{
		for (int s=0; s<=255; s++)
		{
			if(ToAscii(i,s,&ksarray[0],&output,0)!=0)
			{
				ToAsciiTable[i][s] = (unsigned char)output;
			}
			else 
			{
				ToAsciiTable[i][s] = 0;
			}
		}
	}
}
// End of extern C declaration 

};




