; Include file, change directory if needed
include "p12f675.inc"


;#########################
;## uC Chip settings	##
;#########################
;   Set User ID Memory
	__idlocs		0x55aa
;   Set configuration bits using definitions from the include file, p16f84.inc
	__config	_INTRC_OSC_NOCLKOUT & _WDT_OFF & _CP_OFF & _MCLRE_OFF



;#########################
;## PIN DEFINITIONS	##
;#########################
; GP5	I/O     	()
; GP4	I/O			()
; GP3	nMCLR		()
; GP2	I/O			()
; GP1	OUT	LED1	()
; GP0	OUT LED0	()
LED1_P		EQU	b'111111'


; OUTPUTs
LED1	EQU	GP0
LED0    EQU GP1


; INPUTs

;#########################
;## CONSTANTS		##
;#########################

;#########################
;## GLOBAL REGISTERS	##
;#########################
count_l	EQU	0x20		; Low byte of counter
count_m EQU	0x21		; Middle byte of counter
count_h EQU	0x22		; High byte of counter

stor_m	equ	0x24

;#########################
;## MACROS		##
;#########################
#define	PAGE0		bcf	STATUS,RP0	; [M] Switch to page 0 in RAM
#define PAGE1		bsf	STATUS,RP0	; [M] Switch to page 1 in RAM

LED1_ON macro				; [M] Turn on LED1
	bsf	GPIO,LED1
	endm

LED1_OFF macro				; [M] Turn off LED1
	bcf	GPIO,LED1
	endm

LED0_ON macro				; [M] Turn on LED0
	bsf	GPIO,LED0
	endm

LED0_OFF macro				; [M] Turn off LED0
	bcf	GPIO,LED0
	endm

;###################################################################################
;###################################################################################
;####			S	T	A	R	T			####
;###################################################################################
;###################################################################################
;Initialization of basic registers
Reset_vector	org	0x0000
	goto	Init

Irq_vector	org	0x0004
	retfie

Init
; ##############
; Adjust osc calibration constant, read it from EEPROM

	PAGE1				; Set page 0
	call	0x3ff
	movwf	OSCCAL			; Read osc calibration value and set it
	PAGE0

	movlw	0x07
	movwf	CMCON           ; Disable comparator

    PAGE1
    movlw   0x00
    movwf   ANSEL           ; Disable ADC

    movlw   0x00
    movwf   ADCON0          ; Disable ADC
    PAGE0

	clrwdt				; Reset watchdog
;###############
; Set Inputs/Outputs
	PAGE1
	movlw	~(LED1_P)		; LED1 will be output
	movwf	TRISIO			; Set direction
	PAGE0
; Set GPIO
	movlw	0xff
	movwf	GPIO			; Turn off LED1 and 2 leds, turn on LED3

;#######################################
; MAIN LOOP
;#######################################

; Some mnemonics:
; movwf - Move 'Working' (accumulator) into register
; movlw - move literal (some direct value) into working
; btfss - bit test file (some register), skip if set
; btfsc - bit -||-, skip if clear
; incfsz - increment register and skip next if is zero


;	some my own init
	movlw	0xFF
	movwf	stor_m

M_L0:
	; clear lower byte of counter
	clrw
	movwf	count_l
	; set middle byte to some value
;	movlw	0x40
	; decrement stored value and load it into middle counter
	decf	stor_m
	movfw	stor_m

	movwf	count_m			; Set counter to 0x40
	; set high byte
	movlw	0xFF
	movwf	count_h



	; test the lowest bit of GPIO register?
	btfss	GPIO,GP0
	goto	Led_on
	goto	Led_off
wait_loop:
	
	incfsz	count_l			; inner loop
	goto	wait_loop

	incfsz	count_m			; outer loop
	goto 	wait_loop

	incfsz	count_h
	goto	wait_loop

	goto	M_L0


;###############
;Functions
;###############
Led_off:
	LED1_OFF
	LED0_OFF
	goto	wait_loop

Led_on:
	LED1_ON
	LED0_ON
	goto	wait_loop

	end
