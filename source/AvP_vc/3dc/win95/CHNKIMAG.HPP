#ifndef _included__chnkimag_hpp_
#define _included__chnkimag_hpp_

#error "This file is obsolete"

#include <stdio.h>

#include "d3_image.hpp"
#include "mishchnk.hpp"
#include "bmpnames.hpp"

extern "C" extern char projectsubdirectory[];

extern char const * GameTex_Directory;
extern char const * GenTex_Directory;
extern char const * FixTex_Directory;
extern char const * ToolsTex_Directory;

struct CL_RIFFImage : public CL_Image
{
public:
	static char const * game_mode; // game mode defines palette and set of graphics - can be null or "" for default

	CL_RIFFImage() : CL_Image() {}
	CL_RIFFImage(CL_Image const & base) : CL_Image(base) {}

private:
	virtual CL_Error Locate(char const * iname, int const enum_id);
	
	void GetPath(ImageDescriptor const & idsc, Environment_Data_Chunk * envd, BMPN_Flags bflags);
};

#endif // !_included__chnkimag_hpp_
