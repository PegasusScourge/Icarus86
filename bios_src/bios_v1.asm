;
;	bios_v1.asm
;
;	Created by Matthew Clarke
;
;	Last updated: 2020-08-03
;
;	This is the BIOS ROM for the 8086 processor of the Icarus86 project.
;	To be loaded at FC00:0 in ROM
;	Reset vector aligned at FFFF:0 with far jump to FC00:0
;
;	JP = Jump point. Not a function and will not return
;	FN = Function. Called and will return
;	IFN = Interrupt Function
;



BITS 16 ; Target x86-16

; DEFINES for the absolute memory locations
%define RESET_VECTOR 0xFFFF0 			; Location in memory that the CPU will start at (absolute)
%define _BIOS 0xFC000 					; Location of the BIOS ROM in memory (absolute)

; DEFINES for addresses in segmented mode
%define SEG_BIOS 0xF000 				; Seg of bios code (CS)
%define OFF_BIOS 0xC000					; Offset of bios code in SEG_BIOS
%define SEG_IVT 0x0000					; Seg of the interrupt vector table
%define SEG_BIOS_DATTABLE 0x0040		; Segment of the bios data table

ORG _BIOS								; Set the code to start at _BIOS
	
	jmp bios_init
	
; =====================================================================================================================================================
;												Bios main
; =====================================================================================================================================================
	
; ******************************************************************************************************
;	JP bios_init
;	Initializes the state of the processor for the bios
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************

bios_init:
	; Set up the segment registers and stack. CS has been initialized by the far jump to SEG_BIOS
	mov bx, SEG_BIOS 					; Load the bios segment into AX
	mov ds, bx							; Load the DS (data segment) register to SEG_BIOS
	mov bx, 0x0030						; Load 0x0030 into ax in prep for SS loading
	mov ss, bx							; Load the SS (stack segment) register to 0x0000
	mov sp, 0x300						; Initialize the stack pointer. Stack is now at 0x0030:0x0300
	sti									; Re-enable interrupts (disabled by the 'mov ss, ax' instruction)
	
	; Display hello
	mov ax, hello						; Load string pointer
	call print_str
	call print_nl
	
	; Our next task is to initialize the interrupt vector table
	call ivt_init
	
	; Now we are ready to load the data table with information from the emulator
	; Start by setting up the data segment from now on
	call bdtbl_init						; Call the bios table init routine
	
	; Attempt boot, if it fails we call int 0x18 for the no bootable media
	int 0x18
	
	; If we ended up here, the bios failed
bios_error:
	mov ax, hltMsg						; Load and print the halt message if the bios fails
	call print_str
	hlt
	
; =====================================================================================================================================================
;												Initialization functions (data tables)
; =====================================================================================================================================================
	
; ******************************************************************************************************
;	FN bdtbl_init
;	Initializes the bios data table
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************

bdtbl_init:
	push bx
	push es

	mov bx, SEG_BIOS_DATTABLE
	mov es, bx							; Set ES to the bios data table
	
	
	
	pop es
	pop bx
	ret
	
; ******************************************************************************************************
;	FN ivt_init
;	Initializes the interrupt vector table
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************
	
ivt_init:
	push bx
	push es
	
	mov bx, SEG_IVT
	mov es, bx							; Load the ES register with the segment of the IVT
	mov bx, 0							; Start at the base of the IVT segment
	
	; We load undefined interrupt routine pointers in the whole table to start
ivt_ui_lp:
	mov [es:bx], word undefined_interrupt
	mov [es:bx+2], word SEG_BIOS
	add bx, 4
	cmp bx, 0xFF * 4
	jne ivt_ui_lp
	
	; We now load the relevant interrupt routines where needed
	
	; INT 0x18 (No bootable media)
	mov bx, 0x18 * 4					; Point to interrupt
	mov [es:bx], word noboot_media
	mov [es:bx+2], word SEG_BIOS		; Set the interrupt handler
	
	pop es
	pop bx
	ret
	
; =====================================================================================================================================================
;												Interrupts
; =====================================================================================================================================================
	
; ******************************************************************************************************
;	JP undefined_interrupt
;	Handler for an interrupt that wasn't implemented. Just halts
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************

undefined_interrupt:
	mov ax, udefIRMsg
	call print_str
	hlt
	
; ******************************************************************************************************
;	JP noboot_media
;	Handler for no bootable media (INT 0x18)
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************

noboot_media:
	mov ax, nobootMsg
	call print_str
	hlt
	
; =====================================================================================================================================================
;												Text output
; =====================================================================================================================================================
	
; ******************************************************************************************************
;	FN print_str
;	Prints a string to the display
;	args:
;		AX : pointer to string to write, null terminated.
;	ret:
;		NONE
; ******************************************************************************************************
	
print_str:
	push bx
	push dx
	mov bx, ax
printloop:
	mov dl, [bx]
	inc bx
	xchg al, dl
	call print_chr
	xchg al, dl
	and dl, dl
	jnz printloop
	pop dx
	pop bx
	ret

; ******************************************************************************************************
;	FN push_chr
;	Prints a character to the display
;	args:
;		AL : character to write
;	ret:
;		NONE
; ******************************************************************************************************

print_chr:
	; Push the registers we will use to the stack
	push bx								; Stores the base of VRAM
	push di								; Stores the cursor offset (x only)
	push es								; Stores the segment of the VRAM
	
	mov es, [seg_vram]					; Load the segment of vram into ES
	
	mov bx, [vram]
	mov di, [cursor]
	mov [es:bx+di], al
	inc di
	mov [cursor], di
	
	pop es								; Restore ES
	pop di								; Restore DI
	pop bx								; Restore BX
	ret
	
; ******************************************************************************************************
;	FN print_nl
;	Puts the cursor to the next line
;	args:
;		NONE
;	ret:
;		NONE
; ******************************************************************************************************
	
print_nl:
	push di
	mov di, [cursor]
printnlloop:
	sub di, 80
	jns printnlloop
	sub [cursor], di
	pop di
	ret
	
; ******************************************************************************************************
;	
;	VARIABLES / MEMORY
;
; ******************************************************************************************************
	
zerob:
	db 0 								; Zero byte
zerow:
	dw 0 								; Zero word
	
; Grahpics display memory variables
cursor: 
	dw 0
seg_vram:								; VRAM is in this segment
	dw 0
vram:
	dw 0x8000							; VRAM is here in the SEG_VRAM
	
; Strings
hello:									; Hello text
	db 'Icarus 8086 Bios (v1), Loading.',0
hltMsg:									; Displayed on halt
	db 'Bios error: Halted CPU',0
nobootMsg:								; Displayed when there is no bootable media found
	db 'No bootable media found!',0	
udefIRMsg:								; Displayed when an undefined interrupt is called
	db 'Undefined Interrupt',0	
	
; Serial information
serial_ready:
	db 0
	
; Align ourselves into the reset vector space (RESET_VECTOR)
TIMES (RESET_VECTOR-_BIOS)-($-$$) DB 0
	jmp word SEG_BIOS:OFF_BIOS			; Execute a far jump to F000:C000 (ORG _BIOS) from reset vector