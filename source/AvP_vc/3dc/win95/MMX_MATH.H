#ifndef _included_mmx_math_h_
#define _included_mmx_math_h_

#if SUPPORT_MMX

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
Calling-convention independent
definitions of inline MMX assembler
functions and declarations for non-
inline MMX assembler functions
*/

/* SPECIFICATION */
/*
Dot Product and Vector Transform functions take
arguments referencing matrices or vectors whose
elements are 32 bit signed integers and arranged as
follows. All integers (including the results) are
in 16.16 fixed point form - ie. The 64-bit results
are shifted down 16 bits (divided by 65536) before
being written back as 32-bit values. Results are
rounded down (towards negative infinity).

the matrix structure looks like this (not ideal!)
[ +00 +0c +18 ]
[ +04 +10 +1c ]
[ +08 +14 +20 ]

and the vector structure looks like this
[ +00 ]
[ +04 ]
[ +08 ]
*/

/* TYPICAL CHARACTERISTICS */
/*
Accuracy

Internal rounding errors may be propogated, and
the results may not be exact. For the Dot Product
result and the Vector Transform results (x,y and z
independently), the error distributions are all
the same, as follows:

Exact:	25%
-1:   	50%
-2:   	25%

Better accuracy can be obtained by adding 1 to each integer result,
but this will produce poor results in the case of nice simple round
numbers, eg Dot({1.0,0.0,0.0},{0.0,1.0,0.0}) gives 1 not 0!

Speed

The DotProduct Takes 33 cycles (not including call instruction)
The inline DotProduct takes 30+1 cycles (the last instruction is pairable)
All Vector transforms take 63 cycles. These figures assume no
stalls due to cache misses or misaligned data. A matrix multiply
or cross product could be supplied if it is thought they would
be necessary


For optimal performance, it is recommended that vector and
matrix structures should be aligned to EIGHT byte boundaries.
To ensure this in arrays of vectors/matrices, the structure
should contain a dummy padding 32-bit value (recommended).
*/

/* storage class specifier for assembler calls */

#ifdef __WATCOMC__
#define _asmcall
#define _asminline
#elif defined(_MSC_VER)
#define _asmcall static __inline
#define _asminline static __inline
#else
#error "Unknown compiler"
#endif

/* forward reference declared in global scope */
struct vectorch;
struct matrixch;

/***********************/
/* F-U-N-C-T-I-O-N     */
/* P-R-O-T-O-T-Y-P-E-S */
/* F-O-R   A-L-L       */
/* P-U-B-L-I-C         */
/* F-U-N-C-T-I-O-N-S   */
/***********************/

/* overwrites the input vector with the new vector */
_asmcall void MMX_VectorTransform(struct vectorch * vector, struct matrixch const * matrix);
/* fills a new vector with the result of the input vector transformed by the matrix */
_asmcall void MMX_VectorTransformed(struct vectorch * v_result, struct vectorch const * v_parm, struct matrixch const * matrix);
/* overwrites the input vector with the new vector, then adds another vector */
_asmcall void MMX_VectorTransformAndAdd(struct vectorch * vector, struct matrixch const * matrix, struct vectorch const * v_add);
/* fills a new vector with the result of the input vector transformed by the matrix then added to another vector */
_asmcall void MMX_VectorTransformedAndAdd(struct vectorch * v_result, struct vectorch const * v_parm, struct matrixch const * matrix, struct vectorch const * v_add);
/* compute dot product */
_asmcall signed MMX_VectorDot(struct vectorch const * v1, struct vectorch const * v2);
/* this one assumes all the input vector elements are in the range [-32768,32767] */
_asmcall signed MMX_VectorDot16(struct vectorch const * v1, struct vectorch const * v2);

/* inline versions */
_asminline signed MMXInline_VectorDot(struct vectorch const * v1, struct vectorch const * v2);
_asminline signed MMXInline_VectorDot16(struct vectorch const * v1, struct vectorch const * v2);

/*****************/
/* PRIVATE PARTS */
/*****************/

/* Assembler labels */
extern void MMXAsm_VectorTransform(void);
extern void MMXAsm_VectorTransformed(void);
extern void MMXAsm_VectorTransformAndAdd(void);
extern void MMXAsm_VectorTransformedAndAdd(void);
extern void MMXAsm_VectorDot(void);
extern void MMXAsm_VectorDot16(void);

/* inline calls to MMX functions with correct parameters set */
#ifdef __WATCOMC__

#pragma aux MMX_VectorTransform = "call MMXAsm_VectorTransform" parm [eax] [edx];
#pragma aux MMX_VectorTransformed = "call MMXAsm_VectorTransformed" parm [eax] [edx] [ecx];
#pragma aux MMX_VectorTransformAndAdd = "call MMXAsm_VectorTransformAndAdd" parm [eax] [edx] [ecx];
#pragma aux MMX_VectorTransformedAndAdd = "call MMXAsm_VectorTransformedAndAdd" parm [eax] [edx] [ecx] [ebx];
#pragma aux MMX_VectorDot = "call MMXAsm_VectorDot" parm [eax] [edx] value [eax];
#pragma aux MMX_VectorDot16 = "call MMXAsm_VectorDot16" parm [eax] [edx] value [eax];

#elif defined(_MSC_VER)

_asmcall void MMX_VectorTransform(struct vectorch * vector, struct matrixch const * matrix)
{
	_asm
	{
		mov eax,vector
		mov edx,matrix
		call MMXAsm_VectorTransform
	}
}
_asmcall void MMX_VectorTransformed(struct vectorch * v_result, struct vectorch const * v_parm, struct matrixch const * matrix)
{
	_asm
	{
		mov eax,v_result
		mov edx,v_parm
		mov ecx,matrix
		call MMXAsm_VectorTransformed
	}
}
_asmcall void MMX_VectorTransformAndAdd(struct vectorch * vector, struct matrixch const * matrix, struct vectorch const * v_add)
{
	_asm
	{
		mov eax,vector
		mov edx,matrix
		mov ecx,v_add
		call MMXAsm_VectorTransformAndAdd
	}
}
_asmcall void MMX_VectorTransformedAndAdd(struct vectorch * v_result, struct vectorch const * v_parm, struct matrixch const * matrix, struct vectorch const * v_add)
{
	_asm
	{
		mov eax,v_result
		mov edx,v_parm
		mov ecx,matrix
		mov ebx,v_add
		call MMXAsm_VectorTransformedAndAdd
	}
}
_asmcall signed MMX_VectorDot(struct vectorch const * v1, struct vectorch const * v2)
{
	signed retval;
	_asm
	{
		mov eax,v1
		mov edx,v2
		call MMXAsm_VectorDot
		mov retval,eax
	}
	return retval;
}
_asmcall signed MMX_VectorDot16(struct vectorch const * v1, struct vectorch const * v2)
{
	signed retval;
	_asm
	{
		mov eax,v1
		mov edx,v2
		call MMXAsm_VectorDot16
		mov retval,eax
	}
	return retval;
}

#else

#error "Unknown compiler"

#endif


/* Cross product? Mod? MatrixMultiply? */

/* globals */

extern int use_mmx_math;

/* inline functions - no call */

extern __int64 const mmx_sign_mask;
extern __int64 const mmx_one_fixed_h;

#ifdef __WATCOMC__

#pragma aux MMXInline_VectorDot = \
\
"	movq mm0,[edx]" \
\
"	movd mm2,[edx+08h]" \
"	movq mm4,mm0" \
\
"	pand mm4,mmx_sign_mask" \
"	movq mm6,mm2" \
\
"	movq mm1,[eax]" \
"	paddd mm4,mm4" \
\
"	movd mm3,[eax+08h]" \
"	movq mm5,mm1" \
\
"	pand mm6,mmx_sign_mask" \
"	movq mm7,mm3" \
\
"	pand mm5,mmx_sign_mask" \
"	paddd mm6,mm6" \
\
"	pand mm7,mmx_sign_mask" \
"	paddd mm5,mm5" \
\
"	paddd mm0,mm4" \
"	paddd mm2,mm6" \
\
"	paddd mm7,mm7" \
"	movq mm4,mm2" \
\
"	punpcklwd mm4,mm0" \
"	paddd mm1,mm5" \
\
"	punpckhwd mm2,mm0" \
"	paddd mm3,mm7" \
\
"	movq mm5,mm3" \
"	punpckhwd mm3,mm1" \
\
"	punpcklwd mm5,mm1" \
"	movq mm0,mm2" \
\
"	movq mm1,mm4" \
"	pmaddwd mm0,mm3" \
\
"	movq mm6,mm3" \
"	psrlq mm3,32" \
\
"	movq mm7,mm5" \
"	punpckldq mm3,mm6" \
\
"	pmaddwd mm1,mm5" \
"	psrlq mm5,32" \
\
"	punpckldq mm5,mm7" \
"	pmaddwd mm2,mm3" \
\
"	pmaddwd mm4,mm5" \
"	movq mm3,mm0" \
\
"	punpckldq mm0,mm1" \
\
"	psubd mm0,mmx_one_fixed_h" \
"	punpckhdq mm1,mm3" \
\
"	psrad mm0,16" \
"	paddd mm2,mm4" \
\
"	pslld mm1,16" \
"	paddd mm2,mm0" \
\
"	paddd mm2,mm1" \
\
"	movq mm1,mm2" \
"	psrlq mm2,32" \
\
"	paddd mm1,mm2" \
\
"	movd eax,mm1" \
\
"	emms" \
\
"	inc eax" \
\
parm [eax] [edx] value [eax];

#pragma aux MMXInline_VectorDot16 = \
\
"	movd mm0,[edx+08h]" \
\
"	packssdw mm0,[edx]" \
\
"	movd mm1,[eax+08h]" \
\
"	packssdw mm1,[eax]" \
\
"	pmaddwd mm0,mm1" \
\
"	movq mm1,mm0" \
"	psrlq mm0,32" \
\
"	paddd mm0,mm1" \
\
"	movd eax,mm0" \
\
"	emms" \
\
parm [eax] [edx] value [eax];

#elif defined(_MSC_VER)

_asminline signed MMXInline_VectorDot(struct vectorch const * v1, struct vectorch const * v2)
{
	signed retval;
	_asm
	{
		mov edx,v1
		mov eax,v2

		movq mm0,[edx]

		movd mm2,[edx+08h]
		movq mm4,mm0

		pand mm4,mmx_sign_mask
		movq mm6,mm2

		movq mm1,[eax]
		paddd mm4,mm4

		movd mm3,[eax+08h]
		movq mm5,mm1

		pand mm6,mmx_sign_mask
		movq mm7,mm3

		pand mm5,mmx_sign_mask
		paddd mm6,mm6

		pand mm7,mmx_sign_mask
		paddd mm5,mm5

		paddd mm0,mm4
		paddd mm2,mm6

		paddd mm7,mm7
		movq mm4,mm2

		punpcklwd mm4,mm0
		paddd mm1,mm5

		punpckhwd mm2,mm0
		paddd mm3,mm7

		movq mm5,mm3
		punpckhwd mm3,mm1

		punpcklwd mm5,mm1
		movq mm0,mm2

		movq mm1,mm4
		pmaddwd mm0,mm3

		movq mm6,mm3
		psrlq mm3,32

		movq mm7,mm5
		punpckldq mm3,mm6

		pmaddwd mm1,mm5
		psrlq mm5,32

		punpckldq mm5,mm7
		pmaddwd mm2,mm3

		pmaddwd mm4,mm5
		movq mm3,mm0

		punpckldq mm0,mm1

		psubd mm0,mmx_one_fixed_h
		punpckhdq mm1,mm3

		psrad mm0,16
		paddd mm2,mm4

		pslld mm1,16
		paddd mm2,mm0

		paddd mm2,mm1

		movq mm1,mm2
		psrlq mm2,32

		paddd mm1,mm2

		movd retval,mm1

		emms
	}
	return retval+1;
}

_asminline signed MMXInline_VectorDot16(struct vectorch const * v1, struct vectorch const * v2)
{
	signed retval;
	_asm
	{
		mov eax,v1
		mov edx,v2

		movd mm0,[edx+08h]

		packssdw mm0,[edx]

		movd mm1,[eax+08h]

		packssdw mm1,[eax]

		pmaddwd mm0,mm1

		movq mm1,mm0
		psrlq mm0,32

		paddd mm0,mm1

		movd retval,mm0

		emms
	}
	return retval;
}

#else

#error "Unknown compiler"

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUPPORT_MMX */

#endif /* ! _included_mmx_math_h_ */
