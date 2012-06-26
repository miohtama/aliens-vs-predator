#ifndef _included_cl_init_h_
#define _included_cl_init_h_

#error "This file is obsolete"

#ifdef __cplusplus
extern "C" {
#endif

#include "d3d.h" // required by d3_func.hpp
#include "d3_func.h" // for D3DINFO definition

extern int VideoModeTypeScreen;
extern int ScanDrawMode;
extern D3DINFO d3d;

void CL_Init_All(void);

#ifdef __cplusplus
};
#endif

#endif // !_included_cl_init_h_