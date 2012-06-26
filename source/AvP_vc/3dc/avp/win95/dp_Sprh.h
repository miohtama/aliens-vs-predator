/*--------------------Patrick 18/3/97-------------------- 
  Dave's square peg round hole interface for direct
  play gdi dialog stuff
---------------------------------------------------------*/ 

#ifndef _dpsprh_h_
#define _dpsprh_h_ 1

#ifdef __cplusplus
extern "C" {
#endif



/* Type definitions *****************************************************/

	typedef struct exchangeddatatype
	{
		/* Output fields from DLL*/
			LPDIRECTPLAY2A	lpDP2A_Returned;
				/*
					pointer to direct play object
					Will be NULL if an error occurred, or a "Cancel"
				*/

			BOOL bWasHost;
				/*
					set to TRUE or FALSE
				*/

			LPTSTR lptszFormalPlayerName_Out;
			unsigned int FormalName_MaxSize;

			LPTSTR lptszFriendlyPlayerName_Out;
			unsigned int FriendlyName_MaxSize;
				/*
					These must be non-NULL pointers to buffers in yuor EXE with > 0 attached 
					sizes.  The DLL will write the requested names to these buffers, truncating
					if necessary.  Note that the size includes any zero termination character.
				*/



		/* Input fields to DLL */
			HWND 		hWndMain;
			HINSTANCE	hInst;		

			LPTSTR lptszFormalPlayerName_In;
			LPTSTR lptszFriendlyPlayerName_In;
				/*
					Both of these are allowed to be NULL; 
					if non-NULL they point to zero-terminated ANSI strings
				*/

	} ExchangedDataType;
		/*
			We pass a 4-byte pointer to one of these to the DLL;
			some are read as inputs, some written to as outputs.

			Assumes Watcom and Visual C++ access structures in the
			same way...
		*/

	
/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
extern WINAPI InvokeDavesDirectPlayDLL(ExchangedDataType* pExchangedData);


#ifdef __cplusplus
	};
#endif

#endif
