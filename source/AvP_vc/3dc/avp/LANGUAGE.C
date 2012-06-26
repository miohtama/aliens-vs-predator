/*KJL***************************************
*    Language Internationalization Code    *
***************************************KJL*/
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "gamedef.h"


#include "langenum.h"
#include "language.h"
#include "huffman.hpp"

#if SupportWindows95
	// DHM 12 Nov 97: hooks for C++ string handling code:
	#include "strtab.hpp"
#endif

#define UseLocalAssert Yes
#include "ourasert.h"
#include "avp_menus.h"


#ifdef AVP_DEBUG_VERSION
	#define USE_LANGUAGE_TXT 0
#else
	#define USE_LANGUAGE_TXT 1
#endif

static char EmptyString[]="";

static char *TextStringPtr[MAX_NO_OF_TEXTSTRINGS]={&EmptyString,};
static char *TextBufferPtr;

void InitTextStrings(void)
{
	char *textPtr;
	int i;

	/* language select here! */
	GLOBALASSERT(AvP.Language>=0);
	GLOBALASSERT(AvP.Language<I_MAX_NO_OF_LANGUAGES);
	
	#if MARINE_DEMO
	TextBufferPtr = LoadTextFile("menglish.txt");
	#elif ALIEN_DEMO
	TextBufferPtr = LoadTextFile("aenglish.txt");
	#elif USE_LANGUAGE_TXT
	TextBufferPtr = LoadTextFile("language.txt");
	#else
	TextBufferPtr = LoadTextFile(LanguageFilename[AvP.Language]);
	#endif
	
	LOCALASSERT(TextBufferPtr);

	if (!strncmp (TextBufferPtr, "REBCRIF1", 8))
	{
		textPtr = (char*)HuffmanDecompress((HuffmanPackage*)(TextBufferPtr)); 		
		DeallocateMem(TextBufferPtr);
		TextBufferPtr=textPtr;
	}
	else
	{
		textPtr = TextBufferPtr;
	}

	#if SupportWindows95
	AddToTable( &EmptyString );
	#endif

	for (i=1; i<MAX_NO_OF_TEXTSTRINGS; i++)
	{	
		/* scan for a quote mark */
		while (*textPtr++ != '"');

		/* now pointing to a text string after quote mark*/
		TextStringPtr[i] = textPtr;

		/* scan for a quote mark */
		while (*textPtr != '"')
		{	
			textPtr++;
		}

		/* change quote mark to zero terminator */
		*textPtr = 0;

		#if SupportWindows95
		AddToTable( TextStringPtr[i] );
		#endif
	}
}
void KillTextStrings(void)
{
	UnloadTextFile(LanguageFilename[AvP.Language],TextBufferPtr);

	#if SupportWindows95
	UnloadTable();
	#endif
}

char *GetTextString(enum TEXTSTRING_ID stringID)
{
	LOCALASSERT(stringID<MAX_NO_OF_TEXTSTRINGS);

	return TextStringPtr[stringID];
}


