;.586
;.8087
;
;	Assembly scandraws - designed for Pentiums/Pentium Pros but should work ok
;   on 486s and strange hybrid processors. 
;
;	(C) Kevin Lea 12:03:17 96/12/05
;
;	Please excuse the mess, I haven't tidied up yet. Only the inner loops are
;	well coded, since these take far more runtime than initialisation routines.
;


SOURCE_SBITSV	EQU	16
SOURCE_SBITSU	EQU	7

_DATA	SEGMENT DWORD PUBLIC 'DATA'
	
	EXTRN _SCASM_Lighting:DWORD
	EXTRN _SCASM_Destination:DWORD    
	EXTRN _SCASM_Bitmap:DWORD          
	EXTRN _SCASM_StartU:DWORD
	EXTRN _SCASM_StartV:DWORD
	EXTRN _SCASM_StartI:DWORD
	EXTRN _SCASM_DeltaU:DWORD
	EXTRN _SCASM_DeltaV:DWORD
	EXTRN _SCASM_DeltaI:DWORD
	EXTRN _SCASM_ScanLength:DWORD
	EXTRN _SCASM_ShadingTableSize:DWORD
	EXTRN _SCASM_TextureDeltaScan:DWORD
; 	EXTRN _TLT:BYTE PTR

if 0
	EXTRN _sine:DWORD         ;
	EXTRN _cosine:DWORD       ; these 2 in 3D engine already
	EXTRN _MTRB_Bitmap:DWORD
	EXTRN _MTRB_Destination:DWORD
	EXTRN _MTRB_ScanOffset:DWORD
	EXTRN _MTRB_Angle:DWORD;
	EXTRN _MTRB_InvScale:DWORD;
	EXTRN _MTRB_ScreenHeight:DWORD; equ 48
	EXTRN _MTRB_ScreenWidth:DWORD; equ (49*2+1)
	EXTRN _MTRB_ScreenCentreX:DWORD; equ (MTRB_ScreenWidth/2+1)
	EXTRN _MTRB_ScreenCentreY:DWORD; equ MTRB_ScreenHeight
	EXTRN _MTRB_CentreU:DWORD
	EXTRN _MTRB_CentreV:DWORD
endif

	align


	FixedScale dd 65536.0
	FixedScale8 dd 8192.0 ; 2^16 / 8
	One dd 1.0
	FloatTemp dd ?
    FPUCW  word	?
    OldFPUCW  word ?

	DeltaUFrac dd ?
	DeltaVFrac dd ?
	DeltaIFrac dd ?
	Wholesections dd ?
	PixelsRemaining dd ?
	UVintVfracStepVCarry dd ?
	UVintVfracStepVNoCarry dd ?
	UVintVfracStep equ UVintVfracStepVNoCarry
	IintWithCarry dd ?
	IintNoCarry dd ?
	IintStep equ IintNoCarry

	StackStore dd ?

	ShadeTable equ _TextureLightingTable



	aspectAdjust dd (6 SHL 16) / 5
	startingU  	dd 0
 	startingV 	dd 0
	dUCol		dd 0
	dVCol 		dd 0
	dURow  		dd 0
	dVRow  		dd 0
	rowCount  	dd 0





_DATA	ENDS

_TEXT	SEGMENT BYTE PUBLIC 'CODE'
       	ASSUME	cs:_TEXT, ds:_DATA

.586

if 0
align
PUBLIC _ScanDraw_GouraudScan
PUBLIC ScanDraw_GouraudScan_
ScanDraw_GouraudScan_:
_ScanDraw_GouraudScan:

; calculate horizontal deltas
	pushad
 ;   mov 	[StackStore],esp
	
	mov     eax,_SCASM_ScanLength       
    mov		ebp,eax
    
  	and		eax,7
    shr     ebp,3                       
    
	mov		[PixelsRemaining],eax
    mov     [Wholesections],ebp          ; store widths


    ; setup initial coordinates
    mov     ebx,_SCASM_DeltaI           ; get i 16.16 step
    mov     eax,ebx                     ; copy it
    sar     eax,16                      ; get i int step
    shl     ebx,16                      ; get i frac step
    imul    eax,_SCASM_ShadingTableSize
    mov     IintNoCarry,eax				; save whole step in non-i-carry slot
    add     eax,_SCASM_ShadingTableSize      	; calculate whole step + i carry
    mov     IintWithCarry,eax  			; save in i-carry slot

	mov		esi,_SCASM_StartI
	mov		edx,esi
	sar		esi,16
	shl		edx,16
	imul	esi,_SCASM_ShadingTableSize
	add		esi,_SCASM_Lighting

	xor eax,eax
	mov	edi,_SCASM_Destination
	
	test ebp,ebp
	
	jz GS_EndPixels
if 1				 
GS_ScanLoop:
    ; 8 pixel span code
    ; edi = dest dib bits at current pixel
    ; esi = lighting pointer
    ; edx = i fraction 0.32
    ; ebp = carry scratch


;   mov     al,[esi]
;   add		edx,DeltaIFrac
;	sbb		ebp,ebp
;	add 	esi,[4*ebp + IintStep]
;	mov     [edi],al
	
    mov     al,[esi]				;get colour to draw
	
	add		edx,ebx			;increase intensity
	sbb		ebp,ebp					;check for overflow
	
	add 	esi,[4*ebp + IintStep]	;add to esi required change
	add		edx,ebx			;increase intensity
	
	sbb		ebp,ebp					;check for overflow
	mov     [edi+0],al				;draw out pixel
	
	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+1],al

	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+2],al

	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+3],al

	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+4],al
	
	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+5],al

	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	add		edx,ebx

	sbb		ebp,ebp
	mov     [edi+6],al

	mov     al,[esi]				
	
	add 	esi,[4*ebp + IintStep]
	mov     [edi+7],al


	lea edi,[edi+8]
    dec     Wholesections              ; decrement span count

    jnz     GS_ScanLoop                    ; loop back
endif   	

	mov		eax,[PixelsRemaining]
	test	eax,eax
	jz 		GS_finish 
GS_EndPixels:

    mov     al,[esi]          ; get texture pixel  
	
	lea 	edi,[edi+1]

   	add		edx,ebx
	sbb		ebp,ebp
	mov 	[edi-1],al
	
	add 	esi,[4*ebp + IintStep]
	dec		[PixelsRemaining]
	
	jnz		GS_EndPixels
GS_finish:			   
;    mov esp, [StackStore]
	popad
	ret
endif

if 1
align
PUBLIC _ScanDraw_GouraudScan
PUBLIC ScanDraw_GouraudScan_
ScanDraw_GouraudScan_:
_ScanDraw_GouraudScan:

; calculate horizontal deltas
	pushad
    mov 	[StackStore],esp
	
	mov     eax,_SCASM_ScanLength       
    mov		ecx,eax
    
 ;  	and		ecx,7
    shr     eax,3                       
    
	mov		[PixelsRemaining],ecx
    mov     [Wholesections],eax          ; store widths


    ; setup initial coordinates
    mov     esp,_SCASM_DeltaI           ; get i 16.16 step
;    sar     esp,8                      ; get i frac step

	mov		ebx,_SCASM_StartI
;	sar		ebx,8
	mov		edx,ebx
	sar		edx,8
	and		edx,0xff00h

	mov		esi,_SCASM_Lighting

	mov		edi,_SCASM_Destination
	add		edi,ecx
  ;	test	eax,eax
	neg 	ecx
	sub eax,eax	
	jz GS_EndPixels
if 1				 
	mov     ecx,eax
GS_ScanLoop:


	mov 	al,[esi+edx]
	mov		edx,ebx

	add		ebx,esp
	and		edx,0xff00h
	
	mov		[edi+0],al
	nop

	lea 	edi,[edi+8]
    dec     ecx              ; decrement span count

    jnz     GS_ScanLoop                    ; loop back
endif   	

	mov		ecx,[PixelsRemaining]
	test	ecx,ecx
	jz 		GS_finish 
GS_EndPixels:



	mov 	al,[esi+edx]
	mov		edx,ebx

	sar		edx,8
	add		ebx,esp
	
	and		edx,0xff00h
	mov		[edi+ecx],al
	
	inc		ecx
	jnz		GS_EndPixels
GS_finish:			   
    mov esp, [StackStore]
	popad
	ret
endif

align
; Drawing a 2D polygon which has DeltaV=0 and is transparent
PUBLIC _ScanDraw2D_VAlignedTransparent
PUBLIC ScanDraw2D_VAlignedTransparent_
_ScanDraw2D_VAlignedTransparent:
ScanDraw2D_VAlignedTransparent_:

	pushad
    mov [StackStore],esp
	mov     edx,_SCASM_ScanLength       

    mov     esi,_SCASM_StartU           ; get u 16.16 fixedpoint coordinate
    mov     esp,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     esp,16                      ; get fractional part

    mov     eax,_SCASM_StartV           ; get v 16.16 fixedpoint coordinate
    sar     eax,16                      ; get integer part
    imul    eax,_SCASM_TextureDeltaScan ; calc texture scanline address
    add     esi,eax                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address

	mov		edi,_SCASM_Destination
	mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step

	;	ebx		u int delta
	;	ecx		u frac delta
	;	esp		u frac total
	;	edi 	dest
	;	esi 	source
	;   edx		pixels to draw
VAT_ScanLoop:
    mov     al,[esi]                    ; get texture pixel 0
	add		esp,ecx
	
	adc		esi,ebx
	inc		edi

	test 	al,al
	jz 		VAT_SkipPixel

   	mov 	[edi-1],al          
 	VAT_SkipPixel:
	
	dec     edx              ; decrement span count
    jnz     VAT_ScanLoop                    ; loop back
   	
    mov esp, [StackStore]
	popad
	ret

; Drawing a 2D polygon which has DeltaV=0 and is opaque
align
PUBLIC ScanDraw2D_VAlignedOpaque_
PUBLIC _ScanDraw2D_VAlignedOpaque
ScanDraw2D_VAlignedOpaque_:
_ScanDraw2D_VAlignedOpaque:

	pushad
    mov [StackStore],esp
	mov     edx,_SCASM_ScanLength       

    mov     esi,_SCASM_StartU           ; get u 16.16 fixedpoint coordinate
    mov     esp,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     esp,16                      ; get fractional part

    mov     eax,_SCASM_StartV           ; get v 16.16 fixedpoint coordinate
    sar     eax,16                      ; get integer part
    imul    eax,_SCASM_TextureDeltaScan ; calc texture scanline address
    add     esi,eax                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address

	mov		edi,_SCASM_Destination
	
	mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step

	;	ebx		u int delta
	;	ecx		u frac delta
	;	esp		u frac total
	;	edi 	dest
	;	esi 	source
	;   edx		pixels to draw
VAO_ScanLoop:
    mov     al,[esi]                    ; get texture pixel 0
	add		esp,ecx
	
	adc		esi,ebx
	dec     edx              ; decrement span count
    
    mov     [edi],al          
	lea		edi,[edi+1]
	
    jnz     VAO_ScanLoop                    ; loop back
   	
    mov esp, [StackStore]
	popad
	ret


;
;  2d case with shading
;
;	mov eax,_SCASM_DeltaU
 ;	mov bl,ah
;	mov eax,_SCASM_DeltaV
 ;	mov cl,ah

if 0
align
PUBLIC ScanDraw2D_Gouraud_
PUBLIC _ScanDraw2D_Gouraud
ScanDraw2D_Gouraud_:
_ScanDraw2D_Gouraud:

; calculate horizontal deltas
	pushad
    
    mov 	[StackStore],esp
	mov     eax,_SCASM_ScanLength       
    
    mov		ebp,eax
 ;   and		eax,7
    
    shr     ebp,3                       
    mov		[PixelsRemaining],eax
    
    mov     [Wholesections],ebp          ; store widths
	mov     eax,_SCASM_DeltaV			; C1     1          ; get v 16.16 step
    
    mov     edx,_SCASM_DeltaU           ; C2	 1		    ; get u 16.16 step
    mov     ebx,eax                     ; C1	 2			; copy v 16.16 step

    sar     eax,16                      ; C1	 3			; get v int step
    mov     ecx,edx                     ; C2	 2			; copy u 16.16 step

    shl     ebx,16                      ; C1	 4			; get v frac step
    mov     esi,_SCASM_DeltaI           ; C3	 1			; get i 16.16 step
    
    sar     edx,16                      ; C2	 3			; get u int step
    mov     esp,esi                     ; C3	 2			; copy i 16.16 step
    
    shl     ecx,16                      ; C2	 4			; get u frac step
    mov     DeltaVFrac,ebx            	; C1	 5			; store it
  												 
    imul    eax,_SCASM_TextureDeltaScan ; C1	 6		    ; calculate texture step for v int step

    sar     esp,16-8                    ; C3	 3			; get i int step
  	mov     DeltaUFrac,ecx            	; C2	 5			; store it

    sar     esi,8                      ; C3	 4			; get i frac step
    add     eax,edx                     ; C1+C2	 1			; calculate uint + vint step
    
    mov     DeltaIFrac,esi      	    ; C3	 5			; store it
    mov     UVintVfracStepVNoCarry,eax	; C1+C2	 2			; save whole step in non-v-carry slot
    
	and		esp,0xffffff00h				; C3	 6
    add     eax,_SCASM_TextureDeltaScan ; C1+C2	 3	     	; calculate whole step + v carry

    mov     IintNoCarry,esp				; C3	 7			; save whole step in non-i-carry slot
	mov		edi,_SCASM_Destination
  
    mov     UVintVfracStepVCarry,eax	; C1+C2	 4			; save in v-carry slot
	add     esp,256						; C3	 8			; calculate whole step + i carry
    
    mov     IintWithCarry,esp  			; C3	 9			; save in i-carry slot
	mov		esp,_SCASM_StartI			; C4	 1

	mov		edx,esp						; C4	 2
    mov     esi,_SCASM_StartU           ; C5	 1			; get u 16.16 fixedpoint coordinate

	sar		esp,16-8					; C4	 3
    mov     ebx,esi                     ; C5	 2			; copy it

	sar		edx,8						; C4	 4
    mov     ecx,_SCASM_StartV           ; C6	 1			; get v 16.16 fixedpoint coordinate

	sar     esi,16                      ; C5	 3			; get integer part
	mov		eax,[edi]					; preread destination

    shl     ebx,16                      ; C5	 4			; get fractional part
	mov     eax,ecx                     ; C6	 2			; copy it
    
    sar     eax,16                      ; C6	 3			; get integer part
	and		esp,0xffffff00h				; C4	 5

	shl     ecx,16                      ; C6	 4			; get fractional part	shift [tears removal]
	add		esp,_SCASM_Lighting			; C4	 6
    
    imul    eax,_SCASM_TextureDeltaScan ; C6	 5			; calc texture scanline address


    add     esi,eax                     ; C7	 1			; calc texture offset
	xor 	eax,eax

    add     esi,_SCASM_Bitmap          	; C7	 2			; calc address
	test	ebp,ebp


	sar	ebx,16
	sar	ecx,16

	mov eax,_SCASM_DeltaU
	mov bl,ah
	mov eax,_SCASM_DeltaV
 	mov cl,ah
	
	mov		esp,[PixelsRemaining]
	add		edi,esp
	neg		esp
;	mov		eax,edx
;	add		ch,cl


 	jmp G3D_EndPixels
	jz		G3D_EndPixels
if 1
G3D_ScanLoop:
    ; 8 pixel span code
    ; edi = dest dib bits at current pixel
    ; esi = texture pointer at current u,v
    ; esp = lighting pointer
    ; ebx = u fraction 0.32
    ; ecx = v fraction 0.32	
    ; edx = i fraction 0.32
    ; ebp = carry scratch

;    mov     al,[edi]                    ; preread the destination cache line

    mov     al,[esi]                    ; get texture pixel 0
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
	add		ebx,DeltaUFrac

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi],al                  ; store pixel 0
	
	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 1

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

 	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+1],al                  ; store pixel 1
	
	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 2
				
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

  	sbb		ebp,ebp
	mov 	al,[esp+eax]

  	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+2],al                  ; store pixel 2

	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 3

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+3],al                  ; store pixel 3

	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 4

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+4],al                  ; store pixel 4

	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 5

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac
			
 	sbb		ebp,ebp
	mov 	al,[esp+eax]

 	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+5],al                  ; store pixel 5

	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 6

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

 	sbb		ebp,ebp
	mov 	al,[esp+eax]

 	add 	esp,[4*ebp + IintStep]
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+6],al                  ; store pixel 6

	add		ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 7

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
    dec     Wholesections              ; decrement span count

    mov     [edi+7],al                  ; store pixel 7
	lea 	edi,[edi+8]

    jnz     G3D_ScanLoop                    ; loop back
endif   	

	mov		eax,[PixelsRemaining]
	test	eax,eax
	jz 		G3D_finish 
G3D_EndPixels:

  	add		ch,cl
	mov		eax,edx
	
    sbb     ebp,ebp                     ; get -1 if carry
    mov     al,[esi]      
   	
	inc		esp
  	add		bh,bl

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
   	add		edx,DeltaIFrac

	mov     al,[eax+_TLT]
	test	esp,esp

	mov 	[edi+esp],al
	jnz		G3D_EndPixels
G3D_finish:			   
    mov esp, [StackStore]
	popad
	ret

endif

 
;
;
;   NEW CODE
;
;

if 1
align
PUBLIC ScanDraw2D_Gouraud_
PUBLIC _ScanDraw2D_Gouraud
ScanDraw2D_Gouraud_:
_ScanDraw2D_Gouraud:

; calculate horizontal deltas
	pushad
    
    mov 	[StackStore],esp
	mov     eax,_SCASM_ScanLength       
    
    mov		ebp,eax
    and		eax,7
    
    shr     ebp,3                       
    mov		[PixelsRemaining],eax
    
    mov     [Wholesections],ebp          ; store widths
	mov     eax,_SCASM_DeltaV			; C1     1          ; get v 16.16 step
    
    mov     edx,_SCASM_DeltaU           ; C2	 1		    ; get u 16.16 step
    mov     ebx,eax                     ; C1	 2			; copy v 16.16 step

    sar     eax,16                      ; C1	 3			; get v int step
    mov     ecx,edx                     ; C2	 2			; copy u 16.16 step

    shl     ebx,16                      ; C1	 4			; get v frac step
    mov     esi,_SCASM_DeltaI           ; C3	 1			; get i 16.16 step
    
    sar     edx,16                      ; C2	 3			; get u int step
    mov     esp,esi                     ; C3	 2			; copy i 16.16 step
    
    shl     ecx,16                      ; C2	 4			; get u frac step
    mov     DeltaVFrac,ebx            	; C1	 5			; store it
  				  								 
    imul    eax,_SCASM_TextureDeltaScan ; C1	 6		    ; calculate texture step for v int step

    sar     esp,16-8                    ; C3	 3			; get i int step
  	mov     DeltaUFrac,ecx            	; C2	 5			; store it

    shl     esi,16                      ; C3	 4			; get i frac step
    add     eax,edx                     ; C1+C2	 1			; calculate uint + vint step
    
    mov     DeltaIFrac,esi      	    ; C3	 5			; store it
    mov     UVintVfracStepVNoCarry,eax	; C1+C2	 2			; save whole step in non-v-carry slot
    
	and		esp,0xffffff00h				; C3	 6
    add     eax,_SCASM_TextureDeltaScan ; C1+C2	 3	     	; calculate whole step + v carry

    mov     IintNoCarry,esp				; C3	 7			; save whole step in non-i-carry slot
	mov		edi,_SCASM_Destination
  
    mov     UVintVfracStepVCarry,eax	; C1+C2	 4			; save in v-carry slot
	add     esp,256						; C3	 8			; calculate whole step + i carry
    
    mov     IintWithCarry,esp  			; C3	 9			; save in i-carry slot
	mov		esp,_SCASM_StartI			; C4	 1

	mov		edx,esp						; C4	 2
    mov     esi,_SCASM_StartU           ; C5	 1			; get u 16.16 fixedpoint coordinate

	sar		esp,16-8					; C4	 3
    mov     ebx,esi                     ; C5	 2			; copy it

	shl		edx,16						; C4	 4
    mov     ecx,_SCASM_StartV           ; C6	 1			; get v 16.16 fixedpoint coordinate

	sar     esi,16                      ; C5	 3			; get integer part
	mov		eax,[edi]					; preread destination

 ;  shl     ebx,16                      ; C5	 4			; get fractional part
	and		ebx,0xffffh
	mov     eax,ecx                     ; C6	 2			; copy it
    
    sar     eax,16                      ; C6	 3			; get integer part
	and		esp,0xffffff00h				; C4	 5

;	shl     ecx,16                      ; C6	 4			; get fractional part	shift [tears removal]
	and		ecx,0xffffh
	add		esp,_SCASM_Lighting			; C4	 6
    
    imul    eax,_SCASM_TextureDeltaScan ; C6	 5			; calc texture scanline address


    add     esi,eax                     ; C7	 1			; calc texture offset
	mov 	eax,_SCASM_DeltaU

	mov 	bl,ah
	mov 	eax,_SCASM_DeltaV
 	
 	mov 	cl,ah
	xor 	eax,eax

    add     esi,_SCASM_Bitmap          	; C7	 2			; calc address
	test	ebp,ebp

 ;	jmp G3D_EndPixels
	jz		G3D_EndPixels
if 1
G3D_ScanLoop:
    ; 8 pixel span code
    ; edi = dest dib bits at current pixel
    ; esi = texture pointer at current u,v
    ; esp = lighting pointer
    ; ebx = u fraction 0.32
    ; ecx = v fraction 0.32	
    ; edx = i fraction 0.32
    ; ebp = carry scratch

;    mov     al,[edi]                    ; preread the destination cache line

    mov     al,[esi]                    ; get texture pixel 0
  ; add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
;	add		ebx,DeltaUFrac
	add		bh,bl

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
 ;  	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi],al                  ; store pixel 0
	
;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 1

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

 	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
;   	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+1],al                  ; store pixel 1
	
 ;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 2
				
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

  	sbb		ebp,ebp
	mov 	al,[esp+eax]

  	add 	esp,[4*ebp + IintStep]
 ;  	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+2],al                  ; store pixel 2

;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 3

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
;   	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+3],al                  ; store pixel 3

;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 4

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
 ;  	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+4],al                  ; store pixel 4

;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 5

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac
			
 	sbb		ebp,ebp
	mov 	al,[esp+eax]

 	add 	esp,[4*ebp + IintStep]
  ; 	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+5],al                  ; store pixel 5

;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 6

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

 	sbb		ebp,ebp
	mov 	al,[esp+eax]

 	add 	esp,[4*ebp + IintStep]
;   	add		ecx,DeltaVFrac;test to remove tears
	add		ch,cl

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+6],al                  ; store pixel 6

 ;	add		ebx,DeltaUFrac
	add		bh,bl
    mov     al,[esi]                    ; get texture pixel 7

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov 	al,[esp+eax]

	add 	esp,[4*ebp + IintStep]
    dec     Wholesections              ; decrement span count

    mov     [edi+7],al                  ; store pixel 7
	lea 	edi,[edi+8]

    jnz     G3D_ScanLoop                    ; loop back
endif   	

	mov		eax,[PixelsRemaining]
	test	eax,eax
	jz 		G3D_finish 
G3D_EndPixels:

    mov     eax,[esi]      
	add		ch,cl
;   	add		ecx,DeltaVFrac
    
    sbb     ebp,ebp                     ; get -1 if carry
	and		eax,0xffh

	add		bh,bl
;	add		ebx,DeltaUFrac
	lea 	edi,[edi+1]

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
   	add		edx,DeltaIFrac

	sbb		ebp,ebp
	mov     al,[esp+eax]                
	
	mov 	[edi-1],al
	mov		eax,[PixelsRemaining]

	add 	esp,[4*ebp + IintStep]
	dec		eax
;	dec		[PixelsRemaining]
	
	mov		[PixelsRemaining],eax
	jnz		G3D_EndPixels
G3D_finish:			   
    mov esp, [StackStore]
	popad
	ret
endif
;
;
;   NEW CODE
;
;

align
PUBLIC ScanDraw2D_GouraudTransparent_
PUBLIC _ScanDraw2D_GouraudTransparent
ScanDraw2D_GouraudTransparent_:
_ScanDraw2D_GouraudTransparent:

; calculate horizontal deltas
	pushad
    mov 	[StackStore],esp
	
	mov     eax,_SCASM_ScanLength       
	mov		[PixelsRemaining],eax

    mov     eax,_SCASM_DeltaV                ; get v 16.16 step
    mov     ebx,eax                     ; copy it
    sar     eax,16                      ; get v int step
    shl     ebx,16                      ; get v frac step
    mov     DeltaVFrac,ebx            ; store it
  
    imul    eax,_SCASM_TextureDeltaScan      ; calculate texture step for v int step

    mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step
  
    mov     DeltaUFrac,ecx            ; store it
    add     eax,ebx                     ; calculate uint + vint step
    mov     UVintVfracStepVNoCarry,eax; save whole step in non-v-carry slot
    add     eax,_SCASM_TextureDeltaScan      ; calculate whole step + v carry
    mov     UVintVfracStepVCarry,eax  ; save in v-carry slot

    ; setup initial coordinates
    mov     edx,_SCASM_DeltaI           ; get i 16.16 step
    mov     eax,edx                     ; copy it
    sar     eax,16                      ; get i int step
    shl     edx,16                      ; get i frac step
    mov     DeltaIFrac,edx      	    ; store it
    imul    eax,_SCASM_ShadingTableSize
    mov     IintNoCarry,eax				; save whole step in non-i-carry slot
    add     eax,_SCASM_ShadingTableSize      	; calculate whole step + i carry
    mov     IintWithCarry,eax  			; save in i-carry slot

	mov		esp,_SCASM_StartI
	mov		edx,esp
	sar		esp,16
	shl		edx,16
	imul	esp,_SCASM_ShadingTableSize
	add		esp,_SCASM_Lighting

    mov     esi,_SCASM_StartU                ; get u 16.16 fixedpoint coordinate
    mov     ebx,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     ebx,16                      ; get fractional part

    mov     ecx,_SCASM_StartV                ; get v 16.16 fixedpoint coordinate
    mov     eax,ecx                     ; copy it
    sar     eax,16                      ; get integer part
    shl     ecx,16                      ; get fractional part	shift [tears removal]
    imul    eax,_SCASM_TextureDeltaScan      ; calc texture scanline address
    add     esi,eax                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address


;	mov eax,_SCASM_DeltaU
 ;	mov bl,ah
;	mov eax,_SCASM_DeltaV
 ;	mov cl,ah

	xor eax,eax
	mov	edi,_SCASM_Destination
	
G2DT_EndPixels:

    mov     al,[esi]          ; get texture pixel  
   	add		ecx,DeltaVFrac;test to remove tears
    
    sbb     ebp,ebp                     ; get -1 if carry
	test	eax,eax
	
	jz		G2DT_SkipPixel
	
	mov     al,[esp+eax]                  ; store pixel 0
	mov 	[edi],al
	
	
	G2DT_SkipPixel:
	
	lea 	edi,[edi+1]
	add		ebx,DeltaUFrac
        
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
   	add		edx,DeltaIFrac
	
	sbb		ebp,ebp
	add 	esp,[4*ebp + IintStep]
	
	dec		[PixelsRemaining]
	jnz		G2DT_EndPixels

    mov esp, [StackStore]
	popad
	ret







align
PUBLIC ScanDraw2D_Opaque_
PUBLIC _ScanDraw2D_Opaque
ScanDraw2D_Opaque_:
_ScanDraw2D_Opaque:

; calculate horizontal deltas
	pushad
    mov [StackStore],esp

	mov     eax,_SCASM_ScanLength       
    mov		ebp,eax
    
  	and		eax,7
    shr     ebp,3                       
    
	mov		[PixelsRemaining],eax
    mov     [Wholesections],ebp          ; store widths
	

    mov     eax,_SCASM_DeltaV                ; get v 16.16 step
    mov     ebx,eax                     ; copy it
    sar     eax,16                      ; get v int step
    shl     ebx,16                      ; get v frac step
    mov     DeltaVFrac,ebx            ; store it
    ;shl		eax,7
    imul    eax,_SCASM_TextureDeltaScan      ; calculate texture step for v int step

    mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step
    mov     DeltaUFrac,ecx            ; store it
    add     eax,ebx                     ; calculate uint + vint step
    mov     UVintVfracStepVNoCarry,eax; save whole step in non-v-carry slot
    add     eax,_SCASM_TextureDeltaScan      ; calculate whole step + v carry
    mov     UVintVfracStepVCarry,eax  ; save in v-carry slot

    ; setup initial coordinates

    mov     esi,_SCASM_StartU                ; get u 16.16 fixedpoint coordinate
    mov     ebx,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     ebx,16                      ; get fractional part

    mov     ecx,_SCASM_StartV                ; get v 16.16 fixedpoint coordinate
    mov     edx,ecx                     ; copy it
    sar     edx,16                      ; get integer part
    shl     ecx,16                      ; get fractional part
   ; shl		edx,7
    imul    edx,_SCASM_TextureDeltaScan      ; calc texture scanline address
    add     esi,edx                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address

    mov     edx,DeltaUFrac            ; get register copy
	mov		esp,DeltaVFrac
	mov		edi,_SCASM_Destination

	test ebp,ebp
	jz O2D_EndPixels
	
if 1
O2D_ScanLoop:
    ; 8 pixel span code
    ; edi = dest dib bits at current pixel
    ; esi = texture pointer at current u,v
    ; ebx = u fraction 0.32
    ; ecx = v fraction 0.32
    ; edx = u frac step
    ; ebp = v carry scratch

;    mov     al,[edi]                    ; preread the destination cache line

    mov     al,[esi]                    ; get texture pixel 0
 
    add     ecx,esp            ; increment v fraction
    sbb     ebp,ebp                     ; get -1 if carry
    add     ebx,edx                     ; increment u fraction

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+0],al                  ; store pixel 0
	
    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 1

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+1],al                  ; store pixel 1
	
    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 2
				
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+2],al                  ; store pixel 2

    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 3

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+3],al                  ; store pixel 3

    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 4

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+4],al                  ; store pixel 4

    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 5

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+5],al                  ; store pixel 5

    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 6

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    add     ecx,esp            ; increment v fraction

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+6],al                  ; store pixel 6

    add     ebx,edx                     ; increment u fraction
    mov     al,[esi]                    ; get texture pixel 7

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

    mov     [edi+7],al                  ; store pixel 7


    add     edi,8                       ; increment to next span
    dec     Wholesections              ; decrement span count
    jnz     O2D_ScanLoop                    ; loop back
endif   	

	mov		eax,[PixelsRemaining]
	test	eax,eax
	jz 		O2D_finish 
O2D_EndPixels:

    mov     al,[esi]          ; get texture pixel  
   	add		ecx,esp				; increment v fraction
    
    sbb     ebp,ebp                     ; get -1 if carry
    add     ebx,edx                     ; increment u fraction
	
	mov     [edi],al                  ; store pixel 
    lea 	edi,[edi+1]

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
	dec		[PixelsRemaining]
	
	jnz		O2D_EndPixels
   	


O2D_finish:
    mov esp, [StackStore]
	popad
	ret


align
PUBLIC ScanDraw2D_Transparent_
PUBLIC _ScanDraw2D_Transparent
ScanDraw2D_Transparent_:
_ScanDraw2D_Transparent:

; calculate horizontal deltas
	pushad
    mov [StackStore],esp

	mov     ebp,_SCASM_ScanLength       
	mov		[PixelsRemaining],ebp

    mov     eax,_SCASM_DeltaV                ; get v 16.16 step
    mov     ebx,eax                     ; copy it
    sar     eax,16                      ; get v int step
    shl     ebx,16                      ; get v frac step
    mov     DeltaVFrac,ebx            ; store it
    ;shl		eax,7
    imul    eax,_SCASM_TextureDeltaScan      ; calculate texture step for v int step

    mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step
    mov     DeltaUFrac,ecx            ; store it
    add     eax,ebx                     ; calculate uint + vint step
    mov     UVintVfracStepVNoCarry,eax; save whole step in non-v-carry slot
    add     eax,_SCASM_TextureDeltaScan      ; calculate whole step + v carry
    mov     UVintVfracStepVCarry,eax  ; save in v-carry slot

    ; setup initial coordinates

    mov     esi,_SCASM_StartU                ; get u 16.16 fixedpoint coordinate
    mov     ebx,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     ebx,16                      ; get fractional part

    mov     ecx,_SCASM_StartV                ; get v 16.16 fixedpoint coordinate
    mov     edx,ecx                     ; copy it
    sar     edx,16                      ; get integer part
    shl     ecx,16                      ; get fractional part
   ; shl		edx,7
    imul    edx,_SCASM_TextureDeltaScan      ; calc texture scanline address
    add     esi,edx                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address

    mov     edx,DeltaUFrac            ; get register copy
	mov		esp,DeltaVFrac
	mov		edi,_SCASM_Destination

T2D_EndPixels:

    mov     al,[esi]          ; get texture pixel  
   	add		ecx,esp				; increment v fraction
    
    sbb     ebp,ebp                     ; get -1 if carry
	test	al,al
	
	jz		T2D_SkipPixel
	
	mov     [edi],al                  ; store pixel 
    T2D_SkipPixel:
    
    lea 	edi,[edi+1]
    add     ebx,edx                     ; increment u fraction

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
	dec		[PixelsRemaining]
	
	jnz		T2D_EndPixels
   	
    mov esp, [StackStore]
	popad
	ret

align
PUBLIC ScanDraw2D_TransparentLit_
PUBLIC _ScanDraw2D_TransparentLit
ScanDraw2D_TransparentLit_:
_ScanDraw2D_TransparentLit:

; calculate horizontal deltas
	pushad
    mov [StackStore],esp

	mov     ebp,_SCASM_ScanLength       
	mov		[PixelsRemaining],ebp

	mov		esp,_SCASM_StartI
	imul	esp,_SCASM_ShadingTableSize
	add		esp,_SCASM_Lighting

    mov     eax,_SCASM_DeltaV                ; get v 16.16 step
    mov     ebx,eax                     ; copy it
    sar     eax,16                      ; get v int step
    shl     ebx,16                      ; get v frac step
    mov     DeltaVFrac,ebx            ; store it
    ;shl		eax,7
    imul    eax,_SCASM_TextureDeltaScan      ; calculate texture step for v int step

    mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step
    mov     DeltaUFrac,ecx            ; store it
    add     eax,ebx                     ; calculate uint + vint step
    mov     UVintVfracStepVNoCarry,eax; save whole step in non-v-carry slot
    add     eax,_SCASM_TextureDeltaScan      ; calculate whole step + v carry
    mov     UVintVfracStepVCarry,eax  ; save in v-carry slot

    ; setup initial coordinates

    mov     esi,_SCASM_StartU                ; get u 16.16 fixedpoint coordinate
    mov     ebx,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     ebx,16                      ; get fractional part

    mov     ecx,_SCASM_StartV                ; get v 16.16 fixedpoint coordinate
    mov     edx,ecx                     ; copy it
    sar     edx,16                      ; get integer part
    shl     ecx,16                      ; get fractional part
   ; shl		edx,7
    imul    edx,_SCASM_TextureDeltaScan      ; calc texture scanline address
    add     esi,edx                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address

    mov     edx,DeltaUFrac            ; get register copy
 ;	mov		esp,DeltaVFrac
	mov		edi,_SCASM_Destination
	xor		eax,eax

TL2D_EndPixels:

    mov     al,[esi]          ; get texture pixel  
   	add		ecx,DeltaVFrac				; increment v fraction
    
    sbb     ebp,ebp                     ; get -1 if carry
	test	al,al
	
	jz		TL2D_SkipPixel
	
	mov		al,[eax+esp]
	mov     [edi],al                  ; store pixel 
    TL2D_SkipPixel:
    
    lea 	edi,[edi+1]
    add     ebx,edx                     ; increment u fraction

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
	dec		[PixelsRemaining]
	
	jnz		TL2D_EndPixels
   	
    mov esp, [StackStore]
	popad
	ret

if 0
PUBLIC MotionTrackerRotateBlit_
MotionTrackerRotateBlit_:

; calculate horizontal deltas
	pushad
    mov [StackStore],esp


	mov	eax,_MTRB_Angle
	mov	eax,[_cosine + eax * 4]
	shl	eax,16 - 7 
  	imul	[_MTRB_InvScale]
  	shrd	eax,edx,16
	mov	[dUCol],eax


	mov	eax,[_MTRB_Angle]
	mov	eax,[_sine + eax * 4]
	shl	eax,16 - 7 
	imul	[_MTRB_InvScale]
  	shrd	eax,edx,16
	mov	[dVCol],eax
    mov esp,eax
; calculate vertical deltas

	mov	eax,[dVCol]
	neg	eax
 ;	imul	[aspectAdjust]
 ; 	shrd	eax,edx,16
	mov	[dURow],eax

	mov	eax,[dUCol]
;	imul	[aspectAdjust]
; 	shrd	eax,edx,16
	mov	[dVRow],eax

	mov	eax,_MTRB_CentreU       ; put CentreU&V in 16.16 for now
	shl	eax,16-7
	mov	[startingU],eax
	mov	eax,_MTRB_CentreV
	shl	eax,16-7
	mov	[startingV],eax


; move up by yOrg

	mov	eax,[dUCol]
	imul    eax,_MTRB_ScreenCentreX
	sub	[startingU],eax

	mov	eax,[dURow]
	imul	eax,_MTRB_ScreenCentreY
	sub	[startingU],eax

	mov	eax,[dVCol]
	imul	eax,_MTRB_ScreenCentreX
	sub	[startingV],eax
		 
	mov	eax,[dVRow]
	imul	eax,_MTRB_ScreenCentreY
	sub	[startingV],eax

; fixup end of row deltas

	mov	eax,[dUCol]
	imul	eax,_MTRB_ScreenWidth
	neg	eax
	add	eax,[dURow]
 	mov	[dURow],eax


	mov	eax,[dVCol]
	imul	eax,_MTRB_ScreenWidth
	neg	eax
	add	eax,[dVRow]
    mov	[dVRow],eax



	mov	esi,[_MTRB_Bitmap]
	mov	edi,[_MTRB_Destination]

	mov	ecx,[startingU]
	mov	edx,[startingV]

	mov	ebx,_MTRB_ScreenHeight		; initialize row count
	mov ah,bl
MTRB_RowLoop:
   	mov	ebx,_MTRB_ScreenWidth 		; initialize column count

MTRB_ColLoop:
 	mov	ebp,edx
	shr	ebp,32 - 7
	shld ebp,ecx,7

   	add	edx,esp
    add	ecx,[dUCol]

	mov	al,[esi+ebp]
  	test al,al
	jz MTRB_SkipPixel
	mov	[edi],al
	MTRB_SkipPixel:
 	
   	lea edi,[edi+1]

   	dec	ebx

    jnz	MTRB_ColLoop

    add	ecx,[dURow]
   	add	edx,[dVRow]
	add	edi,[_MTRB_ScanOffset]

    dec	ah
	jnz	MTRB_RowLoop


    mov esp, [StackStore]
	popad
	ret

PUBLIC MotionTrackerRotateBlit8_
PUBLIC _MotionTrackerRotateBlit8
MotionTrackerRotateBlit8_:
_MotionTrackerRotateBlit8:

; calculate horizontal deltas
	pushad
    mov [StackStore],esp


	mov	eax,_MTRB_Angle
	mov	eax,[_cosine + eax * 4]
	shl	eax,16 - 8 
  	imul	[_MTRB_InvScale]
  	shrd	eax,edx,16
	mov	[dUCol],eax


	mov	eax,[_MTRB_Angle]
	mov	eax,[_sine + eax * 4]
	shl	eax,16 - 8 
	imul	[_MTRB_InvScale]
  	shrd	eax,edx,16
	mov	[dVCol],eax
    mov esp,eax
; calculate vertical deltas

	mov	eax,[dVCol]
	neg	eax
 ;	imul	[aspectAdjust]
 ; 	shrd	eax,edx,16
	mov	[dURow],eax

	mov	eax,[dUCol]
;	imul	[aspectAdjust]
; 	shrd	eax,edx,16
	mov	[dVRow],eax

	mov	eax,_MTRB_CentreU       ; put CentreU&V in 16.16 for now
	shl	eax,16-8
	mov	[startingU],eax
	mov	eax,_MTRB_CentreV
	shl	eax,16-8
	mov	[startingV],eax


; move up by yOrg

	mov	eax,[dUCol]
	imul    eax,_MTRB_ScreenCentreX
	sub	[startingU],eax

	mov	eax,[dURow]
	imul	eax,_MTRB_ScreenCentreY
	sub	[startingU],eax

	mov	eax,[dVCol]
	imul	eax,_MTRB_ScreenCentreX
	sub	[startingV],eax
		 
	mov	eax,[dVRow]
	imul	eax,_MTRB_ScreenCentreY
	sub	[startingV],eax

; fixup end of row deltas

	mov	eax,[dUCol]
	imul	eax,_MTRB_ScreenWidth
	neg	eax
	add	eax,[dURow]
 	mov	[dURow],eax


	mov	eax,[dVCol]
	imul	eax,_MTRB_ScreenWidth
	neg	eax
	add	eax,[dVRow]
    mov	[dVRow],eax



	mov	esi,[_MTRB_Bitmap]
	mov	edi,[_MTRB_Destination]

	mov	ecx,[startingU]
	mov	edx,[startingV]

	mov	ebx,_MTRB_ScreenHeight		; initialize row count
	mov ah,bl
MTRB_RowLoop8:
   	mov	ebx,_MTRB_ScreenWidth 		; initialize column count

MTRB_ColLoop8:
 	mov	ebp,edx
	shr	ebp,32 - 8
	shld ebp,ecx,8

   	add	edx,esp
    add	ecx,[dUCol]

	mov	al,[esi+ebp]
  	test al,al
	jz MTRB_SkipPixel8
	mov	[edi],al
	MTRB_SkipPixel8:
 	
   	lea edi,[edi+1]

   	dec	ebx

    jnz	MTRB_ColLoop8

    add	ecx,[dURow]
   	add	edx,[dVRow]
	add	edi,[_MTRB_ScanOffset]

    dec	ah
	jnz	MTRB_RowLoop8


    mov esp, [StackStore]
	popad
	ret
endif






; floating point test area
if 0

PUBLIC ScanDrawF3D_Gouraud_
ScanDrawF3D_Gouraud_:

; calculate horizontal deltas

	pushad
 
    ; put the FPU in 32 bit mode
    ; @todo move this out of here!
;    fstcw   [OldFPUCW]                  ; store copy of CW
 ;   mov     ax,OldFPUCW                 ; get it in ax
;    and     eax,NOT 1100000000y         ; 24 bit precision
;    mov     [FPUCW],ax                  ; store it
;    fldcw   [FPUCW]                     ; load the FPU
 	finit
 ;   mov 	[StackStore],esp
	
	mov     eax,_SCASM_ScanLength       
    mov		ebp,eax
    
  	and		eax,7
    shr     ebp,3                       
    
	mov		[PixelsRemaining],eax
    mov     [Wholesections],ebp          ; store widths

if 0
    ; setup initial coordinates
    mov     edx,_SCASM_DeltaI           ; get i 16.16 step
    mov     eax,edx                     ; copy it
    sar     eax,16                      ; get i int step
    shl     edx,16                      ; get i frac step
    mov     DeltaIFrac,edx      	    ; store it
    imul    eax,_SCASM_ShadingTableSize
    mov     IintNoCarry,eax				; save whole step in non-i-carry slot
    add     eax,_SCASM_ShadingTableSize      	; calculate whole step + i carry
    mov     IintWithCarry,eax  			; save in i-carry slot

	mov		esp,_SCASM_StartI
	mov		edx,esp
	sar		esp,16
	shl		edx,16
	imul	esp,_SCASM_ShadingTableSize
	add		esp,_SCASM_Lighting
endif
	;******************************************************************************************
	mov     ebx,_ScanDescPtr

    ; calculate ULeft and VLeft         ; FPU Stack (ZL = ZLeft)
                                        ; st0  st1  st2  st3  st4  st5  st6  st7
    fld     DWORD PTR [ebx+4]			            ; V/ZL 
    fld     DWORD PTR [ebx]           		    ; U/ZL V/ZL 
    fld     DWORD PTR [ebx+8]              		; 1/ZL U/ZL V/ZL 
    fld1                                ; 1    1/ZL U/ZL V/ZL 
    fdiv    st,st(1)                    ; ZL   1/ZL U/ZL V/ZL 
    fld     st                          ; ZL   ZL   1/ZL U/ZL V/ZL 
    fmul    st,st(4)                    ; VL   ZL   1/ZL U/ZL V/ZL 
    fxch    st(1)                       ; ZL   VL   1/ZL U/ZL V/ZL 
    fmul    st,st(3)                    ; UL   VL   1/ZL U/ZL V/ZL 

    fstp    st(5)                       ; VL   1/ZL U/ZL V/ZL UL
    fstp    st(5)                       ; 1/ZL U/ZL V/ZL UL   VL

    ; calculate right side OverZ terms  ; st0  st1  st2  st3  st4  st5  st6  st7

    fadd    DWORD PTR [ebx+20]          ; 1/ZR U/ZL V/ZL UL   VL
    fxch    st(1)                       ; U/ZL 1/ZR V/ZL UL   VL
    fadd    DWORD PTR [ebx+12]            ; U/ZR 1/ZR V/ZL UL   VL
    fxch    st(2)            ; V/ZL 1/ZR U/ZR UL   VL
    fadd    DWORD PTR [ebx+16]            ; V/ZR 1/ZR U/ZR UL   VL
    
    ; calculate right side coords       ; st0  st1  st2  st3  st4  st5  st6  st7

    fld1                                ; 1    V/ZR 1/ZR U/ZR UL   VL
    ; @todo overlap this guy
    fdiv    st,st(2)                    ; ZR   V/ZR 1/ZR U/ZR UL   VL
    fld     st                          ; ZR   ZR   V/ZR 1/ZR U/ZR UL   VL
    fmul    st,st(2)                    ; VR   ZR   V/ZR 1/ZR U/ZR UL   VL
    fxch    st(1)                       ; ZR   VR   V/ZR 1/ZR U/ZR UL   VL
    fmul    st,st(4)                    ; UR   VR   V/ZR 1/ZR U/ZR UL   VL
	;******************************************************************************************
	xor eax,eax
	mov	edi,_SCASM_Destination
	
	test ebp,ebp
	jz GF3D_EndPixels

GF3D_ScanLoop:
    ; at this point the FPU contains    ; st0  st1  st2  st3  st4  st5  st6  st7
                                        ; UR   VR   V/ZR 1/ZR U/ZR UL   VL

    ; convert left side coords

    fld     st(5)                       ; UL   UR   VR   V/ZR 1/ZR U/ZR UL   VL
    fmul    [FixedScale]                ; UL16 UR   VR   V/ZR 1/ZR U/ZR UL   VL
    fistp   [_SCASM_StartU]                    ; UR   VR   V/ZR 1/ZR U/ZR UL   VL

    fld     st(6)                       ; VL   UR   VR   V/ZR 1/ZR U/ZR UL   VL
    fmul    [FixedScale]               ; VL16 UR   VR   V/ZR 1/ZR U/ZR UL   VL
    fistp   [_SCASM_StartV]                    ; UR   VR   V/ZR 1/ZR U/ZR UL   VL

    ; calculate deltas                  ; st0  st1  st2  st3  st4  st5  st6  st7

    fsubr   st(5),st                    ; UR   VR   V/ZR 1/ZR U/ZR dU   VL
    fxch    st(1)                       ; VR   UR   V/ZR 1/ZR U/ZR dU   VL
    fsubr   st(6),st                    ; VR   UR   V/ZR 1/ZR U/ZR dU   dV
    fxch    st(6)                       ; dV   UR   V/ZR 1/ZR U/ZR dU   VR

    fmul    [FixedScale8]               ; dV8  UR   V/ZR 1/ZR U/ZR dU   VR
    fistp   [_SCASM_DeltaV]                    ; UR   V/ZR 1/ZR U/ZR dU   VR

    fxch    st(4)                       ; dU   V/ZR 1/ZR U/ZR UR   VR
    fmul    [FixedScale8]               ; dU8  V/ZR 1/ZR U/ZR UR   VR
    fistp   [_SCASM_DeltaU]                    ; V/ZR 1/ZR U/ZR UR   VR

    ; increment terms for next span     ; st0  st1  st2  st3  st4  st5  st6  st7
    ; Right terms become Left terms---->; V/ZL 1/ZL U/ZL UL   VL

    mov     ebx,_ScanDescPtr
    fadd    DWORD PTR [ebx+16]            ; V/ZR 1/ZL U/ZL UL   VL
    fxch    st(1)                       ; 1/ZL V/ZR U/ZL UL   VL
    fadd    DWORD PTR [ebx+20]          ; 1/ZR V/ZR U/ZL UL   VL
    fxch    st(2)                       ; U/ZL V/ZR 1/ZR UL   VL
    fadd    DWORD PTR [ebx+12]            ; U/ZR V/ZR 1/ZR UL   VL
    fxch    st(2)                       ; 1/ZR V/ZR U/ZR UL   VL
    fxch    st(1)                       ; V/ZR 1/ZR U/ZR UL   VL

    ; calculate right side coords       ; st0  st1  st2  st3  st4  st5  st6  st7

    fld1                                ; 1    V/ZR 1/ZR U/ZR UL   VL
    fdiv    st,st(2)                    ; ZR   V/ZR 1/ZR U/ZR UL   VL

    ; set up affine registers

    ; setup delta values
    
    mov     eax,_SCASM_DeltaV                ; get v 16.16 step
    mov     ebx,eax                     ; copy it
    sar     eax,16                      ; get v int step
    shl     ebx,16                      ; get v frac step
    mov     DeltaVFrac,ebx            ; store it
  
    imul    eax,_SCASM_TextureDeltaScan      ; calculate texture step for v int step

    mov     ebx,_SCASM_DeltaU                ; get u 16.16 step
    mov     ecx,ebx                     ; copy it
    sar     ebx,16                      ; get u int step
    shl     ecx,16                      ; get u frac step
  
    mov     DeltaUFrac,ecx            ; store it
    add     eax,ebx                     ; calculate uint + vint step
    mov     UVintVfracStepVNoCarry,eax; save whole step in non-v-carry slot
    add     eax,_SCASM_TextureDeltaScan      ; calculate whole step + v carry
    mov     UVintVfracStepVCarry,eax  ; save in v-carry slot


    mov     esi,_SCASM_StartU                ; get u 16.16 fixedpoint coordinate
    mov     ebx,esi                     ; copy it
    sar     esi,16                      ; get integer part
    shl     ebx,16                      ; get fractional part

    mov     ecx,_SCASM_StartV                ; get v 16.16 fixedpoint coordinate
    mov     eax,ecx                     ; copy it
    sar     eax,16                      ; get integer part
    shl     ecx,16                      ; get fractional part	shift [tears removal]
    imul    eax,_SCASM_TextureDeltaScan      ; calc texture scanline address
    add     esi,eax                     ; calc texture offset
    add     esi,_SCASM_Bitmap          ; calc address


    ; 8 pixel span code
    ; edi = dest dib bits at current pixel
    ; esi = texture pointer at current u,v
    ; esp = lighting pointer
    ; ebx = u fraction 0.32
    ; ecx = v fraction 0.32
    ; edx = i fraction 0.32
    ; ebp = carry scratch

;    mov     al,[edi]                    ; preread the destination cache line

    mov     al,[esi]                    ; get texture pixel 0
 
   	add		ecx,DeltaVFrac;test to remove tears
    sbb     ebp,ebp                     ; get -1 if carry
	add	ebx,DeltaUFrac
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
   
;   add	edx,DeltaIFrac
;	sbb	ebp,ebp
;	mov al,[esp+eax]
;	add esp,[4*ebp + IintStep]
   
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi],al                  ; store pixel 0
	
	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 1

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    
;    add	edx,DeltaIFrac
; 	sbb	ebp,ebp
;	mov al,[esp+eax]
;	add esp,[4*ebp + IintStep]
    
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+1],al                  ; store pixel 1
	
	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 2
				
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

;    add	edx,DeltaIFrac
;  	sbb	ebp,ebp
;	mov al,[esp+eax]
;  	add esp,[4*ebp + IintStep]

   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+2],al                  ; store pixel 2

	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 3

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
    
;    add	edx,DeltaIFrac
;	sbb	ebp,ebp
;	mov al,[esp+eax]
;	add esp,[4*ebp + IintStep]
    
   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+3],al                  ; store pixel 3

	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 4

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

;    add	edx,DeltaIFrac
;	sbb	ebp,ebp
;	mov al,[esp+eax]
;	add esp,[4*ebp + IintStep]

   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+4],al                  ; store pixel 4

	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 5

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

;    add	edx,DeltaIFrac
; 	sbb	ebp,ebp
;	mov al,[esp+eax]
; 	add esp,[4*ebp + IintStep]

   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+5],al                  ; store pixel 5

	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 6

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

;    add	edx,DeltaIFrac
; 	sbb	ebp,ebp
;	mov al,[esp+eax]
; 	add esp,[4*ebp + IintStep]

   	add		ecx,DeltaVFrac;test to remove tears

    sbb     ebp,ebp                     ; get -1 if carry
    mov     [edi+6],al                  ; store pixel 6

	add	ebx,DeltaUFrac
    mov     al,[esi]                    ; get texture pixel 7

    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries

;    add	edx,DeltaIFrac
;	sbb	ebp,ebp
;	mov al,[esp+eax]
;	add esp,[4*ebp + IintStep]
    dec     Wholesections              ; decrement span count


    mov     [edi+7],al                  ; store pixel 7
	lea edi,[edi+8]


    ; the fdiv is done, finish right    ; st0  st1  st2  st3  st4  st5  st6  st7
                                        ; ZR   V/ZR 1/ZR U/ZR UL   VL

    fld     st                          ; ZR   ZR   V/ZR 1/ZR U/ZR UL   VL
    fmul    st,st(2)                    ; VR   ZR   V/ZR 1/ZR U/ZR UL   VL
    fxch    st(1)                       ; ZR   VR   V/ZR 1/ZR U/ZR UL   VL
    fmul    st,st(4)                    ; UR   VR   V/ZR 1/ZR U/ZR UL   VL

    jnz     GF3D_ScanLoop                    ; loop back

	mov		eax,[PixelsRemaining]
	test	eax,eax
	jz 		GF3D_finish 
GF3D_EndPixels:
if 0
    mov     al,[esi]          ; get texture pixel  
   	add		ecx,DeltaVFrac;test to remove tears
    
    sbb     ebp,ebp                     ; get -1 if carry
	add	ebx,DeltaUFrac
	
	lea 	edi,[edi+1]
	mov     al,[esp+eax]                  ; store pixel 0
        
    adc     esi,[4*ebp+UVintVfracStep]  ; add in step ints & carries
   	add		edx,DeltaIFrac
	
	sbb		ebp,ebp
	mov 	[edi-1],al
	
	add 	esp,[4*ebp + IintStep]
	dec		[PixelsRemaining]
	
	jnz		GF3D_EndPixels
endif

FPUReturn:

    ; busy FPU registers:               ; st0  st1  st2  st3  st4  st5  st6  st7
                                        ; xxx  xxx  xxx  xxx  xxx  xxx  xxx
    ffree   st(0)
    ffree   st(1)
    ffree   st(2)
    ffree   st(3)
    ffree   st(4)
    ffree   st(5)
    ffree   st(6)
	finit
;    fldcw   [OldFPUCW]                  ; restore the FPU

GF3D_finish:			   
;    mov esp, [StackStore]
	popad
	ret

endif




_TEXT	ENDS

END
