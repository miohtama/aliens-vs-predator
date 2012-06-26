#include "3dc.h"
#include "inline.h"
#include "module.h"

#include "gamedef.h"
#include "font.h"

#include "indexfnt.hpp"

#define UseLocalAssert 1
#include "ourasert.h"


/*  Roxbys font stuff. any comments will be useful!!!

	font control - Fonts description and sturctures game/plat/font.h
								 Platform dependent processing		game/plat/platsup.c or ddplat.cpp
*/								 



/* general ideas. Crap I know!!!

	written to maintain compatiblity with the current texture (bitmap) processing tools.
	all the information about spacing of the font is maintained witin the bitmap itself,
	using hotspots for the texture processing, that way we dont need extra tools to convert
	fonts. It also allows complete font positioning (except different letter combos changing)
	I have shifted most of the emphasis onto the artists to set up the font and to mark it how they
	see fit. All the fonts must contain hotspots for all the letters. In that way we can easily 
	expand fonts. The character offests at the mo are simply -32. We may have to set up jump tables 
	at some later date to get chars for different languages.	

	Nothing supports anti-aliasing at the moment. Be careful with colours!!!
	
	Most of the info is being passed as locals. Clumsy I know. I didnt want to take up more
	space than was nessecery
	

	HARD coded. Names of the fonts
							Number of fonts to load
							Number of characters in the font. you can leave letters blank.	The number of
							characters in the fonts can be changed for different languages
								
	AVP-Win95
		
		This loads all the fonts in the structure PFFONT AvpFonts[] that is passed 
		to while NUM_FONTS. The Imageheader->ImagePtr for the data is maintained 
		untill we have processd	the characters. I dont fancy manipulating the data
		in a LPDIRECTDRAWSURFACE. The character descriptors in the font contain
		nothing but a src RECT. the void* pointer in the PFFONT structure in this
		case is cast to LPDIRECTDRAWSURFACE.
		
	
	
	Not that the entire font is placed into a 
	file That is then processed. CLUT where to
	put the CLU????	Put it into the vid memory.
	use a void*
*/
   
/*
	Only have 8 bit support a tne moment. More to come!!!
	Screen modes. I recommend loading up different fonts for different screen modes. 
	Simply reference a different structure.
*/
   

#define CHAR_WIDTH(font_num, offset) ((AvpFonts[font_num].srcRect[offset].right - AvpFonts[font_num].srcRect[offset].left))
#define CHAR_HEIGHT(font_num, offset) ((AvpFonts[font_num].srcRect[offset].bottom - AvpFonts[font_num].srcRect[offset].top))
#define IS_CHAR_WHITE_SPACE(ch)	((ch == '\t' || ch == '\n' || ch == ' ' || (int)ch == 0x0))

   

static int ProcessFontEntry
(
	PFFONT* font,
	unsigned char* fontstartaddress,
	unsigned pitch,
	int*  countfromrow,
	int*  countfromcol,
	int 	charnum
);

static int ProcessLineLength
(
	char* start_ch, 		// start char of string
	AVP_FONTS fontnum,		// the font we are using
	int offset,				// the offset from the font
	int max_width,			// the width of the line
	char** end_ch, 			// filled with end address
	int* line_length
);

void LoadAllFonts()
{
	// these fonts end up being in memory all the time,
	// I will also supply a function which is delete
	// specific font.

	int fontnum = 0;
	
	while(fontnum < NUM_FONTS)
		{
			/* load the font in turn */
			LoadPFFont(fontnum);
			fontnum ++;
		}
}


void LoadPFFont(int fontnum)
{
	// these fonts end up being in memory all the time,
	// I will also supply a function which is delete
	// specific font.

	PFFONT *font = &AvpFonts[fontnum];
	unsigned nPitch;
	unsigned char * pSurface;

	LoadFont(font);

	/* get the hotspot color  first entry in */
	
	if(font->fttexBitDepth == 15)font->fttexBitDepth = 16;
	
	pSurface = FontLock(font,&nPitch);
	GLOBALASSERT(pSurface);
	
	font->hotSpotValue = *(unsigned *)pSurface & (font->fttexBitDepth<32 ? (1<<font->fttexBitDepth)-1 : 0xffffffffU);
	
	/* the hotspot value is the top-left pixel */
	{			
		int charnum = 0;
		int countfromrow = 1;
		int countfromcol = 0;

		/*
			find the hotspots and send everything to the
			processing function. This part of routine find the
			hotspots in numbers of pixels
		*/
		
		/*Top line of the texture is redundent we get the hotspot from this*/
		/*edge of the texture has only lines*/


	 	while(charnum < font->num_chars_in_font)
 		{
			ProcessFontEntry(font,pSurface,nPitch,&countfromrow,&countfromcol,charnum);
			charnum++;
		}

		#if 0
		// Taken out by DHM 26/11/97:
		fontnum++;
		#endif
		charnum = 0;
	}
	
	FontUnlock(font);

	#if SupportWindows95
	INDEXFNT_PFLoadHook
	(
		(FontIndex) fontnum,
			// FontIndex I_Font_New,
		font // PFFONT *pffont_New
	);
	#endif
}



static int ProcessFontEntry
(
	PFFONT* font,
	unsigned char* fontstartaddress,
	unsigned pitch,
	int*  countfromrow,
	int*  countfromcol,
	int 	charnum
)
{
	/* 
		okay set the starting point
	
		countfromrow marks the current depth of the processing row in the texture.
		countfromcol marks how far along the current coloum we have processed
	
	 * = HOTSPOT .. the first pixel is used as the hotspot
	
	**********************
	 *	 **		**		****  *		<---- startfromrow
		###
		#
		##	 # #	###
		#		 # #	#	#			##
		###	 ###	####		##
	 *	 *	 # *		****	*  Blank chars marked by two hotspots adjacent
	  		 ###			 ^^			 |at the top and the bottom
				*		*			 ||			 |
	 ^							 ||______|
	 |							 
	 |
	 startfromcol
	 	
	********************* Note that the spot in col 0 marks a new linw of chars
	 
   */	 
	 

	int curr_row = *countfromrow;
	int curr_col = *countfromcol;
	int y_offset = 0, x_offset = curr_col;
				 
				 
	GLOBALASSERT(font);
	GLOBALASSERT(fontstartaddress);
	GLOBALASSERT(charnum < font->num_chars_in_font);
	GLOBALASSERT(curr_row < font->fttexHeight);
	GLOBALASSERT(curr_col <= font->fttexWidth);

	/*remember that all the corrdinates are done by pixels
		find the x and y corrdinates of the left lower hotspot
		we process each line (row) from startfromrow to the end
		first find the next marker in startfromrow
	*/
	
	// only supported if bit depth is a whole number of bytes
	GLOBALASSERT(8==font->fttexBitDepth || 16==font->fttexBitDepth || 24==font->fttexBitDepth || 32==font->fttexBitDepth);

	while(1)
		{
		// this bit processes the chars, finds uvs, extents and fills in the sturcture*/
		// count along the row to find the next x position
		
		unsigned int colour_here;

		if(x_offset > font->fttexWidth - 1)
		{
			// reached the end of the line! reset x and then y

			x_offset = 0;
			curr_col = 0;
			
			curr_row += font->fontHeight;	// max line height
			*countfromrow = curr_row;

			GLOBALASSERT(curr_row < font->fttexHeight);
		}
			
		switch (font->fttexBitDepth)
		{
			default:
				GLOBALASSERT(0);
			case 8:
				colour_here = *(fontstartaddress + (curr_row*pitch + x_offset));
				break;
			case 16:
				colour_here = *(unsigned short *)(fontstartaddress + (curr_row*pitch + 2*x_offset));
				break;
			case 24:
			{
				unsigned char * pPixel = fontstartaddress + (curr_row*pitch + 3*x_offset);
				// assuming the right endianness
				colour_here = pPixel[0] | (unsigned)pPixel[1] << 8 | (unsigned)pPixel[2] << 16;
				break;
			}
			case 32:
				colour_here = *(unsigned *)(fontstartaddress + (curr_row*pitch + 4*x_offset));
				break;
				
		}
		
		if(colour_here == font->hotSpotValue)
		{
			int width = -1, height = -1;
			/* set up the uv corrds of the top left corner*/
			int u = x_offset + 1;	 
			int v = curr_row + 1;	 


			/* scan down to give height*/
			for(y_offset = (curr_row + 1); y_offset < font->fttexHeight; y_offset++)
			{
				switch (font->fttexBitDepth)
				{
					default:
						GLOBALASSERT(0);
					case 8:
						colour_here = *(fontstartaddress + (y_offset*pitch + x_offset));
						break;
					case 16:
						colour_here = *(unsigned short *)(fontstartaddress + (y_offset*pitch + 2*x_offset));
						break;
					case 24:
					{
						unsigned char * pPixel = fontstartaddress + (y_offset*pitch + 3*x_offset);
						// assuming the right endianness
						colour_here = pPixel[0] | (unsigned)pPixel[1] << 8 | (unsigned)pPixel[2] << 16;
						break;
					}
					case 32:
						colour_here = *(unsigned *)(fontstartaddress + (y_offset*pitch + 4*x_offset));
						break;
						
				}
				
				if(colour_here == font->hotSpotValue)
				{
					height = y_offset - curr_row - 1;	 // -1 because we exclude the top and bottom hotspots
					break;
				}		
			}

		 	/* scan along to get the width*/
			for(++x_offset; x_offset < font->fttexWidth; x_offset ++)
			{
				switch (font->fttexBitDepth)
				{
					default:
						GLOBALASSERT(0);
					case 8:
						colour_here = *(fontstartaddress + (curr_row*pitch + x_offset));
						break;
					case 16:
						colour_here = *(unsigned short *)(fontstartaddress + (curr_row*pitch + 2*x_offset));
						break;
					case 24:
					{
						unsigned char * pPixel = fontstartaddress + (curr_row*pitch + 3*x_offset);
						// assuming the right endianness
						colour_here = pPixel[0] | (unsigned)pPixel[1] << 8 | (unsigned)pPixel[2] << 16;
						break;
					}
					case 32:
						colour_here = *(unsigned *)(fontstartaddress + (curr_row*pitch + 4*x_offset));
						break;
						
				}
				
				if(colour_here == font->hotSpotValue)
				{
					width = x_offset - curr_col - 1; // exclude end hotspot
					break;
				}	
			}
				
			*countfromcol = x_offset + 1; /* ready for the next char*/

			/*fill in the data structure -  platform dependent*/
			FillCharacterSlot(u, v, width, height, charnum, font);	
			return 0;														
		}

		x_offset++;

	}	
	return 0;
}											


#if 0 // obsolete
static int Process8BitEntry(PFFONT* font,
													char*  fontstartaddress,
													int*  countfromrow,
													int*  countfromcol,
													int   charnum)
{
	/* 
		okay set the starting point
	
		countfromrow marks the current depth of the processing row in the texture.
		countfromcol marks how far along the current coloum we have processed
	
	 * = HOTSPOT .. the first pixel is used as the hotspot
	
	**********************
	 *	 **		**		****  *		<---- startfromrow
		###
		#
		##	 # #	###
		#		 # #	#	#			##
		###	 ###	####		##
	 *	 *	 # *		****	*  Blank chars marked by two hotspots adjacent
	  		 ###			 ^^			 |at the top and the bottom
				*		*			 ||			 |
	 ^							 ||______|
	 |							 
	 |
	 startfromcol
	 	
	********************* Note that the spot in col 0 marks a new linw of chars
	 
   */	 
	 

	int curr_row = *countfromrow;
	int curr_col = *countfromcol;
	int y_offset = 0, x_offset = curr_col;
				 
				 
	GLOBALASSERT(font);
	GLOBALASSERT(fontstartaddress);
	GLOBALASSERT(charnum < font->num_chars_in_font);
	GLOBALASSERT(curr_row < font->fttexHeight);
	GLOBALASSERT(curr_col <= font->fttexWidth);

	/*remember that all the corrdinates are done by pixels
		find the x and y corrdinates of the left lower hotspot
		we process each line (row) from startfromrow to the end
		first find the next marker in startfromrow
	*/

	while(1)
		{
		// this bit processes the chars, finds uvs, extents and fills in the sturcture*/
		// count along the row to find the next x position
		
		unsigned int colour_here;

		if(x_offset > font->fttexWidth - 1)
		{
			// reached the end of the line! reset x and then y

			x_offset = 0;
			curr_col = 0;
			
			curr_row += font->fontHeight;	// max line height
			*countfromrow = curr_row;

			GLOBALASSERT(curr_row < font->fttexHeight);
		}
			
		colour_here = (int)*(fontstartaddress + (curr_row*font->fttexWidth	+ x_offset));
	
		
		if(colour_here == font->hotSpotValue)
		{
			int width = -1, height = -1;
			/* set up the uv corrds of the top left corner*/
			int u = x_offset + 1;	 
			int v = curr_row + 1;	 


			/* scan down to give height*/
			for(y_offset = (curr_row + 1); y_offset < font->fttexHeight; y_offset++)
			{
				colour_here = (int)*(fontstartaddress + (y_offset*font->fttexWidth	+ x_offset));

				if(colour_here == font->hotSpotValue)
				{
					height = y_offset - curr_row - 1;	 // -1 because we exclude the top and bottom hotspots
					break;
				}		
			}

		 	/* scan along to get the width*/
			for(++x_offset; x_offset < font->fttexWidth; x_offset ++)
			{
				colour_here = (int)*(fontstartaddress + (curr_row*font->fttexWidth	+ x_offset));
			

				if(colour_here == font->hotSpotValue)
				{
					width = x_offset - curr_col - 1; // exclude end hotspot
					break;
				}	
			}
				
			*countfromcol = x_offset + 1; /* ready for the next char*/

			/*fill in the data structure -  platform dependent*/
			FillCharacterSlot(u, v, width, height, charnum, font);	
			return 0;														
		}

		x_offset++;

	}	a
}



									
static int Process16BitEntry(PFFONT *font,
													char* fontstartaddress,
													int*  countfromrow,
													int*  countfromcol,
													int charnum)
{
int curr_row = *countfromrow;
int curr_col = *countfromcol;
int y_offset = 0, x_offset = curr_col;
			 
			 
GLOBALASSERT(font);
GLOBALASSERT(fontstartaddress);
GLOBALASSERT(charnum < font->num_chars_in_font);
GLOBALASSERT(curr_row < font->fttexHeight);
GLOBALASSERT(curr_col <= font->fttexWidth);

/*remember that all the corrdinates are done by pixels
	find the x and y corrdinates of the left lower hotspot
	we process each line (row) from startfromrow to the end
	first find the next marker in startfromrow
*/

	while(1)
	{
		// this bit processes the chars, finds uvs, extents and fills in the sturcture*/
		// count along the row to find the next x position
		
		unsigned int colour_here;

		if(x_offset > font->fttexWidth - 1)
		{
			// reached the end of the line! reset x and then y

			x_offset = 0;
			curr_col = 0;
			
			curr_row += font->fontHeight;	// max line height
			*countfromrow = curr_row;

			GLOBALASSERT(curr_row < font->fttexHeight);
		}

		{
			unsigned int colour_high  = 0x000000ff & (unsigned int)*(fontstartaddress + (curr_row*font->fttexWidth + x_offset)*2);
			unsigned int colour_low = 0x000000ff & (int)*(fontstartaddress + (curr_row*font->fttexWidth + x_offset)*2  + 1);
 			colour_here = (colour_high <<  8) | colour_low;
		}
			
		if(colour_here == font->hotSpotValue)
		{
			int width = -1, height = -1;
			/* set up the uv corrds of the top left corner*/
			int u = x_offset + 1;	 
			int v = curr_row + 1;	 


			/* scan down to give height*/
			for(y_offset = (curr_row + 1); y_offset < font->fttexHeight; y_offset++)
			{
				{
					int colour_high  = 0x000000ff & (int)*(fontstartaddress + (y_offset*font->fttexWidth + x_offset)*2);
					int colour_low = 0x000000ff & (int)*(fontstartaddress + (y_offset*font->fttexWidth + x_offset)*2  + 1);
		 			colour_here = (colour_high <<  8) | colour_low;
				}
		 			 
				if(colour_here == font->hotSpotValue)
				{
					height = y_offset - curr_row - 1;	 // -1 because we exclude the top and bottom hotspots
					break;
				}		
			}

		 	/* scan along to get the width*/
			for(++x_offset; x_offset < font->fttexWidth; x_offset ++)
			{
				{
					int colour_high  = 0x000000ff & (int)*(fontstartaddress + (curr_row*font->fttexWidth + x_offset)*2);
					int colour_low = 0x000000ff & (int)*(fontstartaddress + (curr_row*font->fttexWidth + x_offset)*2  + 1);
		 			colour_here = (colour_high <<  8) | colour_low;
				}
				
				if(colour_here == font->hotSpotValue)
				{
					width = x_offset - curr_col - 1; // exclude end hotspot
					break;
				}	
			}
				
			*countfromcol = x_offset + 1; /* ready for the next char*/

			/*fill in the data structure -  platform dependent*/
			FillCharacterSlot(u, v, width, height, charnum, font);	
			return 0;														
		}

		x_offset++;

	}	
	return 0;
}											

static int Process24BitEntry(PFFONT* font,
													char* fontstartaddress,
													int*  countfromrow,
													int*  countfromcol,
													int charnum)
{
	return 0;
}

#endif





void BLTWholeFont(int fontnum, int x , int y, int win_width)
{
	int i = 0;
	int plotto_x = x, plotto_y = y;

	while(i < AvpFonts[fontnum].num_chars_in_font)
		{
			int charwidth = AvpFonts[fontnum].srcRect[i].right - AvpFonts[fontnum].srcRect[i].left;
	
			if((charwidth + plotto_x - x) > win_width)
				{
					plotto_y += AvpFonts[fontnum].fontHeight;
					plotto_x= x;
				}

			BLTFontOffsetToHUD(&AvpFonts[fontnum], plotto_x, plotto_y, i);

			plotto_x += charwidth + 1;

			i++;
		}
	return;
}


void BLTString(FONT_DESC str_packet)
{
	PFFONT font = AvpFonts[str_packet.fontnum];
  
	unsigned char *strptr = str_packet.string;
	int offset = 0;
	int not_finished = Yes;
	int pos_x = str_packet.destx;
	int pos_y = str_packet.desty;
	int white_space_width = CHAR_WIDTH(str_packet.fontnum, 0);	


	// set up the font processing varibles depending on the type of font

	switch(font.font_type)
		{
			case(I_FONT_NUMERIC):
				{	
					offset = 0x30;
   					break;
				}
			case(I_FONT_UC_NUMERIC):
				{	
					offset = 0x20;
   					break;
				}
			case(I_FONT_UCLC_NUMERIC):
				{
					offset = 0x20;
 					break;
				}
			default:
				GLOBALASSERT(2<1);
		}			
	
	while(not_finished)
	{
		int line_length;
		char *end_char;
	
	
		// find the line length and the end char in the line
	
		not_finished = ProcessLineLength
		(
			strptr, 		// start char of string
			str_packet.fontnum,		// the font we are using
			offset,				// the offset from the font
			str_packet.width,			// the width of the line
			&end_char, 			// filled with end address
			&line_length		// filled with line length
		);			
					  
					  
		// work out where to print the line			  	

		if(line_length)						
		{
			switch(str_packet.just)
			{	
	
				case FJ_LEFT_JUST:
				{
					pos_x = str_packet.destx;					
					break;
				}
				case FJ_CENTRED:
				{
					pos_x = str_packet.destx + ((str_packet.width - line_length) >> 1);
					break;
				}
				case FJ_RIGHT_JUST:
				{
					pos_x = str_packet.destx + (str_packet.width - line_length);
					break;
				}
				default:
				{	
					;
				}
			}
				
			// now print the line untill we reach the address of
			// the end char

			do
			{
				if(*strptr == ' ')
				{
					pos_x += white_space_width;
				}
				else if(*strptr == '\t')
				{
					pos_x += 4*white_space_width;
				}	
				else if(*strptr == '\n' || strptr == 0x0)
				{
					GLOBALASSERT(strptr == end_char);
				}
				else if((int)*strptr == 0xD) 
				{
					// carrige return 
					// do nothing - our next char should be '\n'
					
					GLOBALASSERT(*(strptr + 1) == '\n');
				}
				else
				{
					extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;
					int end_pos = pos_x + CHAR_WIDTH(str_packet.fontnum, ((int)*strptr - offset));
					int bottom_pos = pos_y + CHAR_HEIGHT(str_packet.fontnum, ((int)*strptr - offset));

					if(end_pos > ScreenDescriptorBlock.SDB_Width || pos_x < 0)
					{			
						//dont draw		
						//not_finished = No;
					}					
					else if( bottom_pos > ScreenDescriptorBlock.SDB_Height || pos_y < 0)
					{
						not_finished = No;
					}
					else								
					{
						pos_x += BLTFontOffsetToHUD
						(
							&font, 
							pos_x, 
							pos_y, 
							(int)*strptr - offset
						);
						
						pos_x ++; // to make space between letters
					}
				}
			}		
			while(++strptr != end_char);

			pos_y += font.fontHeight - 2;
		}
		
		strptr++;
	}
}



/*
RWH - changes to font line processing
This function takes a pointer to a string a font and a width and
puts the length of the line into line_length AND puts the end char address into end_ch

It returns 0 when we have reached the end of a string

*/



int ProcessLineLength
(
	char* start_ch, 		// start char of string
	AVP_FONTS fontnum,		// the font we are using
	int offset,				// the offset from the font
	int max_width,			// the width of the line
	char** end_ch, 			// filled with end address
	int* line_length
)
{
	int continue_to_process_word = Yes;
	int white_space_width = CHAR_WIDTH(fontnum, 0);	
	char *word_ptr = start_ch;
		
	*line_length = 0;	

	if(start_ch == NULL)
	{		
		*end_ch = NULL;
		*line_length = 0;
		return 0;
	}	   		

	// first process any white space at the end of the
	// line out 
	
	while(*start_ch == ' ')
	{
		// make sure we havent run out
		// of chars to process
		if(*start_ch == 0x0)
		{
			*end_ch = NULL;
			*line_length = 0;
			return 0;		
		}	
		
		start_ch++;
	}	
	
	// Now we can start on the characters in the line	
	// note that we have to have two loops. The first
	// continues untill we break out of it. The second
	// adds up the length of each word and sees if the line
	// with the new word will overrun the max_width
	
	
	// the  word_ptr points to the current char - it is only incremented
	// when we can be sure that we can add the current letter to the word
	
	
	while(1)
	{
		int word_length = 0;

		continue_to_process_word = Yes;

		while(continue_to_process_word) 				
		{
			// is the next char white space ?
			// if so close the word
		
			if(IS_CHAR_WHITE_SPACE(*(word_ptr + 1)))
			{
				// hit white space - finish this current word but only
				// AFTER we have processed the current char
				continue_to_process_word = No;
			}	
			
			// need to process every white space seperately
 			if(*word_ptr == '\t')
 			{
 				// a one char word!
 				word_length = 4 * white_space_width;
				continue_to_process_word = No;
				word_ptr++;
 			}
 			else if(*word_ptr == 0x0)
 			{
				// reached end of file - need to return 0 
				
				*end_ch = word_ptr;				
				return 0;
 			}
 			else if(*word_ptr == '\n')
 			{
				*end_ch = word_ptr;				
				return 1;
 			}
 			else if(*word_ptr == ' ')
 			{
				if(word_length)
				{
					// tag on the white space onto the word length
					word_length += white_space_width;
				}
				// other wise keep on tiking on
				
 				word_ptr++;
 			}														
 			else
 			{
 				// yeah add a letter to the word length
				int char_off = (char)(*word_ptr - offset);
				word_length += CHAR_WIDTH(fontnum, char_off);
				word_length ++; //for space between lettes

				word_ptr++; //process next char
			}		
		}
		
		// okay we have the length of this word - check to see if
		// it overruns the end of the line - if it is too long,
		// break out of the line loop
		
		
		if((word_length + *line_length) >= max_width) 
		{
			*end_ch = start_ch;
			return 1;				
		}	
		else 	
		{
			*line_length += word_length;
			// set up the next word save the beginning of teh word in start_ch
			start_ch = word_ptr;
		}												
	}
}		