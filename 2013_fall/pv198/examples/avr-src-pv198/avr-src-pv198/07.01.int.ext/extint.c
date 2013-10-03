/*
 *                 ATmega128 External Interrupt example
 *
 * Description : Trigger external interrupt INT4 and toggle LEDs on port D.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: extint.c,v 1.2 2006/12/09 11:43:03 qceleda Exp $
 */
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * External interrupt INT4 interrupt service routine. Toggle LEDs on
 * port D every time, the interrupt service routine is called.
 */
ISR(INT4_vect)
{
	PORTD ^= 0xFF;		/* toggle port D leds */
}

int main(void)
{
	DDRD = 0xFF;		/* set port D as output */

	DDRE &= ~(_BV(PORTE4));	/* set PE4 as input */
	PORTE |= _BV(PORTE4);	/* enable PE4 pull up resistor */

	/* ATmega128 - data sheet note
	 * ---------------------------
	 * When changing the ISCn bit, an interrupt can occur. Therefore,
	 * it is recommended to first disable INTn by clearing its
	 * Interrupt Enable bit in the EIMSK Register. Then, the ISCn bit
	 * can be changed. Finally, the INTn interrupt flag should be
	 * cleared by writing a logical one to its Interrupt Flag bit
	 * (INTFn) in the EIFR Register before the interrupt is re-enabled.
	 *
	 * EICRB - External Interrupt Control B
	 * EIFR  - External Interrupt Flag Register
	 * EIMSK - External Interrupt Mask Register
	 */

	cli();			/* global interrupt disable */

	EIMSK = 0x00;		/* disable all external interrupts */

	/* 0 0 The low level of INTn generates an interrupt request. */
	/* EICRB = 0x00; */

	/* 1 0 The falling edge of INTn generates asynchronously an interrupt request. */
	EICRB = _BV(ISC41);

	/* 1 1 The rising edge of INTn generates asynchronously an  interrupt request. */
	/* EICRB = _BV(ISC41)|_BV(ISC40); */

	EIFR = 0x00;		/* clear all interrupt flags */
	EIMSK = _BV(INT4);	/* enable external interrupts */

	sei();			/* global interrupt enable */

	while(1){};		/* loop for ever */
}

