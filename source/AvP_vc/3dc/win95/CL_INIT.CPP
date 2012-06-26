#include "cl_init.h"
#include "system.h" // because the 3dc header files don't automatically include the ones they need
#include "equates.h" // because the 3dc header files don't automatically include the ones they need
#include "platform.h" // for VideoModeTypes
#include "shape.h" // because the 3dc header files don't automatically include the ones they need
#include "prototyp.h" // for SDB
#include "d3_image.hpp" // for init functions

extern "C" extern SCREENDESCRIPTORBLOCK ScreenDescriptorBlock;

void CL_Init_All(void)
{
	switch (VideoModeTypeScreen)
	{
		case VideoModeType_8:
			if (ScreenDescriptorBlock.SDB_Flags & SDB_Flag_TLTPalette)
				CL_Init_DirectDrawMode(CLV_8TLT);
			else
				CL_Init_DirectDrawMode(CLV_8);
			break;
		case VideoModeType_15:
			CL_Init_DirectDrawMode(CLV_15);
			break;
		case VideoModeType_24:
			CL_Init_DirectDrawMode(CLV_24);
			break;
		case VideoModeType_8T:
			CL_Init_DirectDrawMode(CLV_8T);
			break;
	}

	if (ScanDrawDirectDraw != ScanDrawMode)
		CL_Init_D3DMode(&(d3d.TextureFormat[d3d.CurrentTextureFormat].ddsd));
}
