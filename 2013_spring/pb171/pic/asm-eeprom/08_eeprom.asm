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
tmp		EQU	0x20		; Temporary register

array	EQU	0x24		; data array


; inside band 1
e2_wr_addr	equ	0x30
e2_wr_data	equ	0x31

;#########################
;## MACROS		##
;#########################
#define	PAGE0		bcf	STATUS,RP0	; [M] Switch to page 0 in RAM
#define PAGE1		bsf	STATUS,RP0	; [M] Switch to page 1 in RAM

;###################################################################################
;###################################################################################
;####			S	T	A	R	T			####
;###################################################################################
;###################################################################################
;Initialization of basic registers
Reset_vector	org	0x0000
	goto	Init

Irq_vector	org	0x0004
	; test EEPROM interrupt status
	PAGE1
;	btfss	PIR1,7
;	goto	Irq_end
	movlw	0xFF
	movwf	GPIO
Irq_end

	goto $
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
	movlw	0x00
	movwf	GPIO			; Turn off LED1 and 2 leds, turn on LED3


; Set interrupts
	PAGE1
	bsf		PIE1, EEIE
	bsf		INTCON,PEIE
	bsf		INTCON,GIE
	PAGE0
	bcf		PIR1, EEIF
;###############
; Read LED state from EEPROM and invert it:
	PAGE1
	movlw	0x10		; Read address 0x10 from EEPROM
	movwf	EEADR
	bsf		EECON1,RD
	movf	EEDATA,W	; and store it to tmp
;	movwf	array + 0	; 0th byte
; Set GPIO state
	PAGE0
	xorlw	0x01
	movwf	array + 0
;	movwf	GPIO

; Write back the inverted value:
	; load parameters
	PAGE1
	movwf	e2_wr_data
	movlw	0x10		; write to 0x10
	movwf	e2_wr_addr

	PAGE1
	call	e2_write_byte
	PAGE0


;Loop:					; Wait until the value is written successfully
;	btfsc	EECON1,WR
;	goto	Loop
;	movlw	0x02
;	movwf	GPIO

;	movlw	0x00
;	movwf	GPIO
;#######################################
; MAIN LOOP
;#######################################
	goto	$

; Writes byte to eeprom memory. Don't check.
; @param e2_wr_addr	address
; @param e2_wr_data	data
; must be in bank 1
e2_write_byte:

	movfw	e2_wr_data
	movwf	EEDATA		; Set EEPROM address and data to write
	movfw	e2_wr_addr
	movwf	EEADR
	bcf		PIR1,EEIF
	bsf 	EECON1,WREN	; Enable write
	bcf		INTCON,GIE	; disable interrupts

	movlw	0x55		; Unlock write
	movwf	EECON2
	movlw	0xAA
	movwf	EECON2
	bsf		EECON1,WR	; Write

	bsf		INTCON,GIE	; enable interrupts
	return


	end
