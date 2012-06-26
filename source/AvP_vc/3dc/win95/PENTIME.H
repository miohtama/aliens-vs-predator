/* pentime.h */
extern unsigned long int rdtsc_lo(void);
extern unsigned long int rdtsc_hi(void);
extern unsigned long int rdtsc_mid(void);

#define ProfileStart() \
{						   \
	int time = rdtsc_lo();
#define ProfileStop(x) \
	textprint("%s %d\n",x,rdtsc_lo()-time); \
}

#pragma aux rdtsc_lo = \
	"db 0fh, 31h"	\
	value [eax]		\
	modify [edx];

#pragma aux rdtsc_hi = \
	"db 0fh, 31h"	\
	value [edx]		\
	modify [eax];

#pragma aux rdtsc_mid = \
	"db 0fh, 31h"	\
	"shr eax, 10h" \
	"shl edx, 10h" \
	"add eax, edx" \
	value [eax]		\
	modify [edx];

/* Test to see if we have a Pentium or not. Note that this test is reliable 
 * enough for a tools project (where we can put in an overide switch) but not
 * for a released product.
 */
extern unsigned char Pentium(void);
#pragma aux Pentium = \
					"pushfd" \
					"pop eax" \
					"or eax, 00200000h" \
					"push eax" \
					"popfd" \
					"pushfd" \
					"pop eax" \
					"mov ecx, eax" \
					"and eax, 00200000h" \
					"cmp eax, 0" \
					"je not_Pentium" \
					"mov eax, ecx" \
					"and eax, 0ffdfffffh" \
					"push eax" \
					"popfd" \
					"pushfd" \
					"pop eax" \
					"and eax, 00200000h" \
					"cmp eax, 0" \
					"jne not_Pentium" \
	"is_Pentium: 	 mov al, 1" \
					"jmp finish" \
	"not_Pentium:    mov al, 0" \
	"finish: 		 nop" \
					value [al] \
					modify [eax ecx]


