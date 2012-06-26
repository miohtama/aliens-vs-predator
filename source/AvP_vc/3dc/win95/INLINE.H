#ifndef INLINE_INCLUDED

#if SUPPORT_MMX
#include "mmx_math.h"
#endif

/*


 Watcom PC Inline Functions.

 Watcom Standard C does not support the C++ "inline" directive, so these
 functions have been written as inline assembler instead.

*/

#ifdef __cplusplus
extern "C" {
#endif

/* 
	Standard macros.  Note that FIXED_TO_INT
	and INT_TO_FIXED are very suboptimal in 
	this version!!!
	Also, MUL_INT and ISR are ONLY intended 
	to be used in Win95 so that Saturn versions
	of the same code can be compiled using calls
	to hand optimised assembler functions, i.e.
	for code that is never intended to be run on
	a Saturn they are unnecessary.
*/

#define OUR_ABS(x)                (((x) < 0) ? -(x) : (x))
#define OUR_SIGN(x)	             (((x) < 0) ? -1 : +1)
#define OUR_INT_TO_FIXED(x)	 	 (int) ((x) * (65536))
#define OUR_FIXED_TO_INT(x)		 (int) ((x) / (65536))
#define OUR_MUL_INT(a, b)	       ((a) * (b))
#define OUR_ISR(a, shift)		    ((a) >> (shift))


/*

 win95\item.c functions

*/

void InitialiseTriangleArrayData(void);
void* AllocateTriangleArrayData(int tasize);


/*

 General Triangle Array Handler Null Case / Error

*/

void TriangleArrayNullOrError(TRIANGLEARRAY *tarr);


/*

 Item Polygon Triangle Array Functions

*/

void Item_Polygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_Polygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);


/*

 Item Gouraud Polygon Triangle Array Functions

*/

void Item_GouraudPolygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_GouraudPolygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);

/*

 Item 2d Textured Polygon Triangle Array Functions

*/

void Item_2dTexturedPolygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_2dTexturedPolygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);

/*

 Item Gouraud 2d Textured Polygon Triangle Array Functions

*/

void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_Gouraud2dTexturedPolygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);


/*

 Item 3d Textured Polygon Triangle Array Functions

*/

void Item_3dTexturedPolygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_3dTexturedPolygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);

/*

 Item Gouraud 3d Textured Polygon Triangle Array Functions

*/

void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_3(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_4(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_5(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_6(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_7(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_8(TRIANGLEARRAY *qarr);
void Item_Gouraud3dTexturedPolygon_PrepareTriangleArray_9(TRIANGLEARRAY *qarr);

/*

 Platform Specific 64-Bit Operator Functions

 Not all compilers support 64-bit operations, and some platforms may not
 even support 64-bit numbers. Support for 64-bit operations is therefore
 provided in the platform specific fucntions below.

 For C++ a mew class could be defined. However the current system is not
 compiled as C++ and the Cygnus GNU C++ is not currently working.

*/


/*
	These functions have been checked for suitability for 
	a Pentium and look as if they would pair up okay.
	Might be worth a more detailed look at optimising
	them though.
	Obviously there is a problem with values not being
	loaded into registers for these functions, but this
	may be unavoidable for 64 bit values on a Watcom
	platform.
*/


#ifdef __WATCOMC__ /* inline assember for the Watcom compiler */

/* ADD */

void ADD_LL(LONGLONGCH *a, LONGLONGCH *b, LONGLONGCH *c);
# pragma aux ADD_LL = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"add	eax,[edi]" \
"adc	edx,[edi+4]" \
"mov	[ebx],eax" \
"mov	[ebx+4],edx" \
parm[esi] [edi] [ebx] \
modify[eax edx];


/* ADD ++ */

void ADD_LL_PP(LONGLONGCH *c, LONGLONGCH *a);
# pragma aux ADD_LL_PP = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"add	[edi],eax" \
"adc	[edi+4],edx" \
parm[edi] [esi] \
modify[eax edx];


/* SUB */

void SUB_LL(LONGLONGCH *a, LONGLONGCH *b, LONGLONGCH *c);
# pragma aux SUB_LL = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"sub	eax,[edi]" \
"sbb	edx,[edi+4]" \
"mov	[ebx],eax" \
"mov	[ebx+4],edx" \
parm[esi] [edi] [ebx] \
modify[eax edx];



/* SUB -- */

void SUB_LL_MM(LONGLONGCH *c, LONGLONGCH *a);
# pragma aux SUB_LL_MM = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"sub	[edi],eax" \
"sbb	[edi+4],edx" \
parm[edi] [esi] \
modify[eax edx];


/*

 MUL

 This is the multiply we use, the 32 x 32 = 64 widening version

*/

void MUL_I_WIDE(int a, int b, LONGLONGCH *c);
# pragma aux MUL_I_WIDE = \
"imul	edx"\
"mov	[ebx],eax" \
"mov	[ebx+4],edx" \
parm[eax] [edx] [ebx] \
modify[eax edx];



/*

 CMP

 This substitutes for ==, >, <, >=, <=

*/

int CMP_LL(LONGLONGCH *a, LONGLONGCH *b);
# pragma aux CMP_LL = \
"mov	eax,[ebx]" \
"mov	edx,[ebx+4]" \
"sub	eax,[ecx]" \
"sbb	edx,[ecx+4]" \
"and	edx,edx" \
"jne	llnz" \
"and	eax,eax" \
"jne	llnz" \
"xor	eax,eax" \
"jmp	llgs" \
"llnz:" \
"mov	eax,1" \
"and	edx,edx" \
"jge	llgs" \
"neg	eax" \
"llgs:" \
parm[ebx] [ecx] \
value[eax] \
modify[edx];




/* EQUALS */

void EQUALS_LL(LONGLONGCH *a, LONGLONGCH *b);
# pragma aux EQUALS_LL = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"mov	[edi],eax" \
"mov	[edi+4],edx" \
parm[edi] [esi] \
modify[eax edx];


/* NEGATE */

void NEG_LL(LONGLONGCH *a);
# pragma aux NEG_LL = \
"not	dword ptr[esi]" \
"not	dword ptr[esi+4]" \
"add	dword ptr[esi],1" \
"adc	dword ptr[esi+4],0" \
parm[esi];


/* ASR */

void ASR_LL(LONGLONGCH *a, int shift);
# pragma aux ASR_LL = \
"and	eax,eax" \
"jle	asrdn" \
"asrlp:" \
"sar	dword ptr[esi+4],1" \
"rcr	dword ptr[esi],1" \
"dec	eax" \
"jne	asrlp" \
"asrdn:" \
parm[esi] [eax];


/* Convert int to LONGLONGCH */

void IntToLL(LONGLONGCH *a, int *b);
# pragma aux IntToLL = \
"mov	eax,[esi]" \
"cdq" \
"mov	[edi],eax" \
"mov	[edi+4],edx" \
parm[edi] [esi] \
modify[eax edx];









/*

 Fixed Point Multiply.


 16.16 * 16.16 -> 16.16
 or
 16.16 * 0.32 -> 0.32

 A proper version of this function ought to read
 16.16 * 16.16 -> 32.16
 but this would require a long long result

 Algorithm:

 Take the mid 32 bits of the 64 bit result

*/

/*
	These functions have been checked for suitability for 
	a Pentium and look as if they would work adequately.
	Might be worth a more detailed look at optimising
	them though.
*/

#if 0

int MUL_FIXED(int a, int b);
# pragma aux MUL_FIXED = \
"imul edx" \
"mov ax,dx" \
"rol eax,16" \
parm[eax] [edx] \
value[eax] \
modify[edx];

#else

int MUL_FIXED(int a, int b);
# pragma aux MUL_FIXED = \
"imul edx" \
"shrd	eax,edx,16" \
parm[eax] [edx] \
value[eax] \
modify[edx];

#endif


/*

 Fixed Point Divide - returns a / b

*/

int DIV_FIXED(int a, int b);
# pragma aux DIV_FIXED = \
"cdq" \
"rol eax,16" \
"mov dx,ax" \
"xor ax,ax" \
"idiv ebx" \
parm[eax] [ebx] \
value[eax] \
modify[edx];




/*

 Multiply and Divide Functions.

*/


/*

 32/32 division

 This macro is a function on some other platforms

*/

#define DIV_INT(a, b) ((a) / (b))




/*

 A Narrowing 64/32 Division

*/

int NarrowDivide(LONGLONGCH *a, int b);
# pragma aux NarrowDivide = \
"mov	eax,[esi]" \
"mov	edx,[esi+4]" \
"idiv	ebx" \
parm[esi] [ebx] \
value[eax] \
modify[edx];



/*

 This function performs a Widening Multiply followed by a Narrowing Divide.

 a = (a * b) / c

*/

int WideMulNarrowDiv(int a, int b, int c);
# pragma aux WideMulNarrowDiv = \
"imul	edx"\
"idiv	ebx" \
parm[eax] [edx] [ebx] \
value[eax];



/*

 Function to rotate a VECTORCH using a MATRIXCH

 This is the C function

	x =  MUL_FIXED(m->mat11, v->vx);
	x += MUL_FIXED(m->mat21, v->vy);
	x += MUL_FIXED(m->mat31, v->vz);

	y  = MUL_FIXED(m->mat12, v->vx);
	y += MUL_FIXED(m->mat22, v->vy);
	y += MUL_FIXED(m->mat32, v->vz);

	z  = MUL_FIXED(m->mat13, v->vx);
	z += MUL_FIXED(m->mat23, v->vy);
	z += MUL_FIXED(m->mat33, v->vz);

	v->vx = x;
	v->vy = y;
	v->vz = z;

 This is the MUL_FIXED inline assembler function

	imul edx
	shrd eax,edx,16


typedef struct matrixch {

	int mat11;	0
	int mat12;	4
	int mat13;	8

	int mat21;	12
	int mat22;	16
	int mat23;	20

	int mat31;	24
	int mat32;	28
	int mat33;	32

} MATRIXCH;

*/

void RotateVector_ASM(VECTORCH *v, MATRIXCH *m);
# pragma aux RotateVector_ASM = \
\
"push	eax" \
"push	ebx" \
"push	ecx" \
"push	edx" \
"push	ebp" \
\
"mov	eax,[edi + 0]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ecx,eax"\
"mov	eax,[edi + 12]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ecx,eax" \
"mov	eax,[edi + 24]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ecx,eax" \
\
"mov	eax,[edi + 4]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ebx,eax"\
"mov	eax,[edi + 16]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ebx,eax" \
"mov	eax,[edi + 28]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ebx,eax" \
\
"mov	eax,[edi + 8]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ebp,eax"\
"mov	eax,[edi + 20]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ebp,eax" \
"mov	eax,[edi + 32]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ebp,eax" \
\
"mov	[esi + 0],ecx" \
"mov	[esi + 4],ebx" \
"mov	[esi + 8],ebp" \
\
"pop	ebp" \
"pop	edx" \
"pop	ecx" \
"pop	ebx" \
"pop	eax" \
\
parm[esi] [edi];


/*

 Here is the same function, this time copying the result to a second vector

*/

void RotateAndCopyVector_ASM(VECTORCH *v1, VECTORCH *v2, MATRIXCH *m);
# pragma aux RotateAndCopyVector_ASM = \
\
"push	eax" \
"push	ebx" \
"push	ecx" \
"push	ebp" \
\
"push	edx" \
"mov	eax,[edi + 0]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ecx,eax"\
"mov	eax,[edi + 12]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ecx,eax" \
"mov	eax,[edi + 24]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ecx,eax" \
\
"mov	eax,[edi + 4]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ebx,eax"\
"mov	eax,[edi + 16]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ebx,eax" \
"mov	eax,[edi + 28]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ebx,eax" \
\
"mov	eax,[edi + 8]" \
"imul	DWORD PTR [esi + 0]" \
"shrd	eax,edx,16" \
"mov	ebp,eax"\
"mov	eax,[edi + 20]" \
"imul	DWORD PTR [esi + 4]" \
"shrd	eax,edx,16" \
"add	ebp,eax" \
"mov	eax,[edi + 32]" \
"imul	DWORD PTR [esi + 8]" \
"shrd	eax,edx,16" \
"add	ebp,eax" \
\
"pop	edx" \
"mov	[edx + 0],ecx" \
"mov	[edx + 4],ebx" \
"mov	[edx + 8],ebp" \
\
"pop	ebp" \
"pop	ecx" \
"pop	ebx" \
"pop	eax" \
\
parm[esi] [edx] [edi];




#if (SupportFPMathsFunctions || SupportFPSquareRoot)

/*

 Square Root

 Returns the Square Root of a 32-bit number

*/

static long temp;
static long temp2;

int SqRoot32(int A);
# pragma aux SqRoot32 = \
"finit" \
"mov	temp,eax" \
"fild temp" \
"fsqrt" \
"fistp temp2" \
"fwait" \
"mov	eax,temp2" \
parm[eax] \
value[eax];

#endif


/*

 This may look ugly (it is) but it is a MUCH faster way to convert "float" into "int" than
 the function call "CHP" used by the WATCOM compiler.

*/

static float fptmp;
static int itmp;

void FloatToInt(void);
# pragma aux FloatToInt = \
"fld fptmp" \
"fistp itmp";

/*

 This macro makes usage of the above function easier and more elegant

*/

#define f2i(a, b) { \
fptmp = (b); \
FloatToInt(); \
a = itmp;}

#elif defined(_MSC_VER) /* inline assember for the Microsoft compiler */

/* ADD */

static void ADD_LL(LONGLONGCH *a, LONGLONGCH *b, LONGLONGCH *c)
{
	_asm
	{
		mov esi,a
		mov edi,b
		mov ebx,c
		mov	eax,[esi]
		mov	edx,[esi+4]
		add	eax,[edi]
		adc	edx,[edi+4]
		mov	[ebx],eax
		mov	[ebx+4],edx
	}
}

/* ADD ++ */

static void ADD_LL_PP(LONGLONGCH *c, LONGLONGCH *a)
{
	_asm
	{
		mov edi,c
		mov esi,a
		mov	eax,[esi]
		mov	edx,[esi+4]
		add	[edi],eax
		adc	[edi+4],edx
	}
}

/* SUB */

static void SUB_LL(LONGLONGCH *a, LONGLONGCH *b, LONGLONGCH *c)
{
	_asm
	{
		mov esi,a
		mov edi,b
		mov ebx,c
		mov	eax,[esi]
		mov	edx,[esi+4]
		sub	eax,[edi]
		sbb	edx,[edi+4]
		mov	[ebx],eax
		mov	[ebx+4],edx
	}
}

/* SUB -- */

static void SUB_LL_MM(LONGLONGCH *c, LONGLONGCH *a)
{
	_asm
	{
		mov edi,c
		mov esi,a
		mov	eax,[esi]
		mov	edx,[esi+4]
		sub	[edi],eax
		sbb	[edi+4],edx
	}
}

/*

 MUL

 This is the multiply we use, the 32 x 32 = 64 widening version

*/

static void MUL_I_WIDE(int a, int b, LONGLONGCH *c)
{
	_asm
	{
		mov eax,a
		mov ebx,c
		imul b
		mov	[ebx],eax
		mov	[ebx+4],edx
	}
}

/*

 CMP

 This substitutes for ==, >, <, >=, <=

*/

static int CMP_LL(LONGLONGCH *a, LONGLONGCH *b)
{
	int retval = 0;
	_asm
	{
		mov ebx,a
		mov ecx,b
		mov	eax,[ebx]
		mov	edx,[ebx+4]
		sub	eax,[ecx]
		sbb	edx,[ecx+4]
		and	edx,edx
		jne	llnz
		and	eax,eax
		je	llgs
		llnz:
		mov	retval,1
		and	edx,edx
		jge	llgs
		neg	retval
		llgs:
	}
	return retval;
}

/* EQUALS */

static void EQUALS_LL(LONGLONGCH *a, LONGLONGCH *b)
{
	_asm
	{
		mov edi,a
		mov esi,b
		mov	eax,[esi]
		mov	edx,[esi+4]
		mov	[edi],eax
		mov	[edi+4],edx
	}
}

/* NEGATE */

static void NEG_LL(LONGLONGCH *a)
{
	_asm
	{
		mov esi,a
		not	dword ptr[esi]
		not	dword ptr[esi+4]
		add	dword ptr[esi],1
		adc	dword ptr[esi+4],0
	}
}

/* ASR */

static void ASR_LL(LONGLONGCH *a, int shift)
{
	_asm
	{
		mov esi,a
		mov eax,shift
		and	eax,eax
		jle	asrdn
		asrlp:
		sar	dword ptr[esi+4],1
		rcr	dword ptr[esi],1
		dec	eax
		jne	asrlp
		asrdn:
	}
}

/* Convert int to LONGLONGCH */

static void IntToLL(LONGLONGCH *a, int *b)
{
	_asm
	{
		mov esi,b
		mov edi,a
		mov	eax,[esi]
		cdq
		mov	[edi],eax
		mov	[edi+4],edx
	}
}

/*

 Fixed Point Multiply.


 16.16 * 16.16 -> 16.16
 or
 16.16 * 0.32 -> 0.32

 A proper version of this function ought to read
 16.16 * 16.16 -> 32.16
 but this would require a long long result

 Algorithm:

 Take the mid 32 bits of the 64 bit result

*/

/*
	These functions have been checked for suitability for 
	a Pentium and look as if they would work adequately.
	Might be worth a more detailed look at optimising
	them though.
*/

static int MUL_FIXED(int a, int b)
{
	int retval;
	_asm
	{
		mov eax,a
		imul b
		shrd eax,edx,16
		mov retval,eax
	}
	return retval;
}

/*

 Fixed Point Divide - returns a / b

*/

static int DIV_FIXED(int a, int b)
{
	int retval;
	_asm
	{
		mov eax,a
		cdq
		rol eax,16
		mov dx,ax
		xor ax,ax
		idiv b
		mov retval,eax
	}
	return retval;
}

/*

 Multiply and Divide Functions.

*/


/*

 32/32 division

 This macro is a function on some other platforms

*/

#define DIV_INT(a, b) ((a) / (b))

/*

 A Narrowing 64/32 Division

*/

static int NarrowDivide(LONGLONGCH *a, int b)
{
	int retval;
	_asm
	{
		mov esi,a
		mov	eax,[esi]
		mov	edx,[esi+4]
		idiv	b
		mov retval,eax
	}
	return retval;
}

/*

 This function performs a Widening Multiply followed by a Narrowing Divide.

 a = (a * b) / c

*/

static int WideMulNarrowDiv(int a, int b, int c)
{
	int retval;
	_asm
	{
		mov eax,a
		imul b
		idiv c
		mov retval,eax
	}
	return retval;
}

/*

 Function to rotate a VECTORCH using a MATRIXCH

 This is the C function

	x =  MUL_FIXED(m->mat11, v->vx);
	x += MUL_FIXED(m->mat21, v->vy);
	x += MUL_FIXED(m->mat31, v->vz);

	y  = MUL_FIXED(m->mat12, v->vx);
	y += MUL_FIXED(m->mat22, v->vy);
	y += MUL_FIXED(m->mat32, v->vz);

	z  = MUL_FIXED(m->mat13, v->vx);
	z += MUL_FIXED(m->mat23, v->vy);
	z += MUL_FIXED(m->mat33, v->vz);

	v->vx = x;
	v->vy = y;
	v->vz = z;

 This is the MUL_FIXED inline assembler function

	imul edx
	shrd eax,edx,16


typedef struct matrixch {

	int mat11;	0
	int mat12;	4
	int mat13;	8

	int mat21;	12
	int mat22;	16
	int mat23;	20

	int mat31;	24
	int mat32;	28
	int mat33;	32

} MATRIXCH;

*/

static void RotateVector_ASM(VECTORCH *v, MATRIXCH *m)
{
	_asm
	{
		mov esi,v
		mov edi,m

		mov	eax,[edi + 0]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ecx,eax
		mov	eax,[edi + 12]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ecx,eax
		mov	eax,[edi + 24]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ecx,eax

		mov	eax,[edi + 4]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ebx,eax
		mov	eax,[edi + 16]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ebx,eax
		mov	eax,[edi + 28]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ebx,eax

		mov	eax,[edi + 8]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ebp,eax
		mov	eax,[edi + 20]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ebp,eax
		mov	eax,[edi + 32]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ebp,eax

		mov	[esi + 0],ecx
		mov	[esi + 4],ebx
		mov	[esi + 8],ebp
	}
}

/*

 Here is the same function, this time copying the result to a second vector

*/

static void RotateAndCopyVector_ASM(VECTORCH *v1, VECTORCH *v2, MATRIXCH *m)
{
	_asm
	{
		mov esi,v1
		mov edi,m

		mov	eax,[edi + 0]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ecx,eax
		mov	eax,[edi + 12]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ecx,eax
		mov	eax,[edi + 24]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ecx,eax

		mov	eax,[edi + 4]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ebx,eax
		mov	eax,[edi + 16]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ebx,eax
		mov	eax,[edi + 28]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ebx,eax

		mov	eax,[edi + 8]
		imul	DWORD PTR [esi + 0]
		shrd	eax,edx,16
		mov	ebp,eax
		mov	eax,[edi + 20]
		imul	DWORD PTR [esi + 4]
		shrd	eax,edx,16
		add	ebp,eax
		mov	eax,[edi + 32]
		imul	DWORD PTR [esi + 8]
		shrd	eax,edx,16
		add	ebp,eax

		mov edx,v2
		mov	[edx + 0],ecx
		mov	[edx + 4],ebx
		mov	[edx + 8],ebp
	}
}

#if (SupportFPMathsFunctions || SupportFPSquareRoot)

/*

 Square Root

 Returns the Square Root of a 32-bit number

*/

static long temp;
static long temp2;

static int SqRoot32(int A)
{
	_asm
	{
		finit
		fild A
		fsqrt
		fistp temp2
		fwait
	}
	return (int)temp2;
}

#endif


/*

 This may look ugly (it is) but it is a MUCH faster way to convert "float" into "int" than
 the function call "CHP" used by the WATCOM compiler.

*/

static float fptmp;
static int itmp;

static void FloatToInt(void)
{
	_asm
	{
		fld fptmp
		fistp itmp
	}
}

/*

 This macro makes usage of the above function easier and more elegant

*/

#define f2i(a, b) { \
fptmp = (b); \
FloatToInt(); \
a = itmp;}

#else /* other compiler ? */

#error "Unknown compiler"

#endif


/* These functions are in plspecfn.c */

int WideMul2NarrowDiv(int a, int b, int c, int d, int e);
int _Dot(VECTORCH *vptr1, VECTORCH *vptr2);
void MakeV(VECTORCH *v1, VECTORCH *v2, VECTORCH *v3);
void AddV(VECTORCH *v1, VECTORCH *v2);
void RotVect(VECTORCH *v, MATRIXCH *m);
void CopyClipPoint(CLIP_POINT *cp1, CLIP_POINT *cp2);

#if SUPPORT_MMX

#define RotateVector(v,m) (use_mmx_math ? MMX_VectorTransform((v),(m)) : _RotateVector((v),(m)))
#define RotateAndCopyVector(v_in,v_out,m) (use_mmx_math ? MMX_VectorTransformed((v_out),(v_in),(m)) : _RotateAndCopyVector((v_in),(v_out),(m)))
#define Dot(v1,v2) (use_mmx_math ? MMXInline_VectorDot((v1),(v2)) : _Dot((v1),(v2)))
#define DotProduct(v1,v2) (use_mmx_math ? MMX_VectorDot((v1),(v2)) : _DotProduct((v1),(v2)))

#else /* ! SUPPORT_MMX */

#define RotateVector(v,m) (_RotateVector((v),(m)))
#define RotateAndCopyVector(v_in,v_out,m) (_RotateAndCopyVector((v_in),(v_out),(m)))
#define Dot(v1,v2) (_Dot((v1),(v2)))
#define DotProduct(v1,v2) (_DotProduct((v1),(v2)))

#endif /* ? SUPPORT_MMX */

#ifdef __cplusplus
}
#endif

#define INLINE_INCLUDED
#endif

