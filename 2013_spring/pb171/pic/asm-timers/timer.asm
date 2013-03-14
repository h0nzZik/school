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
w_tmp		EQU	0x20			; Temp reg for W, in both banks (0x20 and 0xA0)
stat_tmp	EQU	0x21			; Temp reg for Status

counter     EQU 0x24
count_m     EQU 0x25

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
	; Save W and STATUS
	movwf	w_tmp		; saving W register to actual page
	movf	STATUS,W	; saving STATUS register to W
	movwf	stat_tmp	; saving STATUS register from W to stat_tmp register

	; Check IRQ from timer 0
;	btfss	INTCON,T0IF	; ** Do we have TMR0 overflow?
;	goto	Irq_n1		; <NO> -> skip to label Irq_nl
;	bcf		INTCON,T0IF	; <YES> -> Clear interrupt bit
;   call    Switch_d0
Irq_n1


    ; Check timer 1
    btfss   PIR1,TMR1IF
    goto    Irq_n2
    bcf     PIR1,TMR1IF

	call 	Switch_d1
;    movlw   0x00
;    movwf   TMR0

;Irq_end

Irq_n2				; <N> No we don't have TMR0 interrupt
	; Restore W and STATUS
	movf	stat_tmp,W
	movwf	STATUS
	movf	w_tmp,W
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
	movlw	0xfe
	movwf	GPIO			; Turn off LED1 and 2 leds, turn on LED3

; set counter
;    movlw   0x00
;    movwf   counter
;    movlw   0xFE
;    movwf   count_m

;###############
; Set TIMER0
	PAGE1
	movlw	b'110'			; Divider 1:256
	movwf	OPTION_REG;
	PAGE0
	bcf	INTCON,T0IF		; Clear T0_OVER flag
;	bsf	INTCON,T0IE		; Enable T0 IRQ
	clrf	TMR0			; Clear TMR
;
	bsf	INTCON,GIE		; Enable all interrupt

    ;setup timer 1
    PAGE0
    bcf T1CON,TMR1CS    ; Internal clock source
    bcf T1CON,TMR1GE    ; turn off external pulling
    ; enable interrupts
    bsf INTCON,6
    bsf INTCON,7

    PAGE1
    bsf PIE1,0          ; enable timer 1 interrupt

    PAGE0

    clrf    TMR1H
    clrf    TMR1L
    bsf     T1CON,TMR1ON    ; turn it on


    bcf INTCON,T0IF
;#######################################
; MAIN LOOP
;#######################################
main_loop

;    call Switch_d0
	; Check IRQ from timer 0
	btfss	INTCON,T0IF	; ** Do we have TMR0 overflow?
	goto	Main_skip		; <NO> -> skip to label Irq_nl
	bcf		INTCON,T0IF	; <YES> -> Clear interrupt bit
    call    Switch_d0

Main_skip
	goto	main_loop

Switch_d0
    movlw   0x01
    xorwf   GPIO
    return

Switch_d1
    movlw   0x02
    xorwf   GPIO
    return

Switch_diods
    call    Switch_d1
    call    Switch_d0
;	movlw 	LED1_P		; Flip led
;	xorwf	GPIO
	return

	end
