/*
 *                 ATmega128 Timer/Counter Interrupt example
 *
 * Description : Trigger an output compare A match interrupt every 1
 *               second. The LED on port D will toggle every time
 *               interrupt is executed. For more details see ATmega128
 *               datasheet Figure 49. Output Compare Unit, Block Diagram
 *               and 16-bit Timer/Counter Register Description.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: ledint.c,v 1.1 2006/12/08 18:14:29 qceleda Exp $
 */
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Output compare A match interrupt service routine. Toggle LEDs on
 * port D every time, the interrupt service routine is called.
 */
ISR(TIMER1_COMPA_vect)
{
	static uint8_t led=1;	/* current LED */

	PORTD = ~led;		/* walking LED */
	/*PORTD = led;*/	/* inverse walking LED */
	/*PORTD ^= led;*/	/* LEDs like a snake */

	led <<= 1;		/* shift to next LED */

	if(!led)
		led = 1;	/* start again from zero */
}

int main(void)
{
	DDRD = 0xFF;		/* set port D as output */
	PORTD = 0xFF;		/* turn off all port D LEDs */

	/* ATmega128 - datasheet
	 * Table 62. Clock Select Bit Description
	 * CSn2  CSn1  CSn0  Description
	 * 0     1     1     clk/64   (from prescaler)
	 * 1     0     0     clk/256  (from prescaler)
	 * 1     0     1     clk/1024 (from prescaler)
	 *
	 * Table 61. Waveform Generation Mode Bit Description
	 * WGM12 - clear timer on compare match (CTC) mode
	 */

	/* set timer 1 control register B (set clock source)
	 * set prescaler 1024, clear timer on compare match
         */
	TCCR1B = _BV(CS12)|_BV(CS10)|_BV(WGM12);

	/* set output compare register 1 A - OCR1A = OCR1AH + OCR1AL
	 * set F_CPU/1024 = 14745600/1024 = 14400
	 */
	OCR1A = ((F_CPU/1024) - 1 );

	TIMSK  = _BV(OCIE1A);	/* enable timer 1 output compare A match interrupt */

	sei();			/* global interrupt enable */

	while(1){};		/* loop for ever */
}

