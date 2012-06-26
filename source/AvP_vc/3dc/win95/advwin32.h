/*************************************************************
Module name: AdvWin32.H
Notices: Copyright (c) 1995 Jeffrey Richter
*************************************************************/

#ifndef ADVWIN32_H_INCLUDED
#define ADVWIN32_H_INCLUDED

/* Disable Visual C++ warnings which fire when reading Windows OS headers. */
#ifndef __WATCOMC__
 
/* Disable ridiculous warnings so that the code         */
/* compiles cleanly using warning level 4.              */

/* nonstandard extension 'single line comment' was used */
#pragma warning(disable: 4001)

// nonstandard extension used : nameless struct/union
#pragma warning(disable: 4201)

// nonstandard extension used : bit field types other than int
#pragma warning(disable: 4214)

// Note: Creating precompiled header 
#pragma warning(disable: 4699)

// unreferenced inline function has been removed
#pragma warning(disable: 4514)

// unreferenced formal parameter
#pragma warning(disable: 4100)

// 'type' differs in indirection to slightly different base 
// types from 'other type'
#pragma warning(disable: 4057)

// named type definition in parentheses
#pragma warning(disable: 4115)

// nonstandard extension used : benign typedef redefinition
#pragma warning(disable: 4209)

// conditional expression is constant : used to differentiate between internal and external versions
#pragma warning(disable: 4127)

// comma operator in array index, disabled due to assertions in array indicies
#pragma warning(disable: 4709)

// assignment operator could not be generated, disable as often the operator isn't used.
#pragma warning(disable: 4512) 

// 'function' selected for automatic inline expansion - when a function not declared inline was inlined: well done compiler, aren't you clever!
#pragma warning(disable: 4711) 

// 'function' not expanded.
#pragma warning(disable: 4710)

#pragma warning(disable: 4032)

#pragma warning(disable: 4702)

/////////////////////////////////////////////////////////////

// Create an ARRAY_SIZE macro that returns the number of
// elements in an array. This is a handy macro that I use
// frequently throughout the sample applications.
#define ARRAY_SIZE(Array) \
   (sizeof(Array) / sizeof((Array)[0]))

/////////////////////////////////////////////////////////////

// Create a BEGINTHREADEX macro that calls the C run-time's
// _beginthreadex function. The C run-time library doesn't
// want to have any reliance on Win32 data types such as
// HANDLE. This means that a Win32 programmer needs to cast
// the return value to a HANDLE. This is terribly inconvenient,
// so I have created this macro to perform the casting.
typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define BEGINTHREADEX(lpsa, cbStack, lpStartAddr, \
   lpvThreadParm, fdwCreate, lpIDThread)          \
      ((HANDLE)_beginthreadex(                    \
         (void *) (lpsa),                         \
         (unsigned) (cbStack),                    \
         (PTHREAD_START) (lpStartAddr),           \
         (void *) (lpvThreadParm),                \
         (unsigned) (fdwCreate),                  \
         (unsigned *) (lpIDThread)))


/////////////////////////////////////////////////////////////


// Compile all CONTEXT structures to use 32-bit members
// instead of 16-bit members.  Currently, the only sample
// application that requires this is TInjLib.16 in order
// for it to work correctly on the DEC Alpha AXP.
#define _PORTABLE_32BIT_CONTEXT

#endif	/* ifndef __WATCOMC__ */

/////////////////////////////////////////////////////////////

// Force all EXEs/DLLs to use STRICT type checking.
#ifndef STRICT
	#define STRICT 1
#endif

#endif	/* ifndef ADVWIN32_H_INCLUDED */

////////////////////////// End Of File //////////////////////
