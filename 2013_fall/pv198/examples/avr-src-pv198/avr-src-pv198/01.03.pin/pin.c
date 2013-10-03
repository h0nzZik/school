/*
 *                  I n p u t - P I N
 *
 * Description : Simple example to show how to read I/O port value.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: pin.c,v 1.2 2006/12/04 15:59:56 qceleda Exp $
 */
#include <avr/io.h>

int main(void)
{
	PORTB = 0xff;		/* activate internal pull-up resistors */
	PORTD = 0xff;		/* set output high -> turn all LEDs off */

	DDRD = 0xff;		/* use all pins on Port D for output */
	DDRB = 0x00;		/* use all pins on Port B for input */

	while(1)
	{
		PORTD = PINB;	/* mirror input bits on LEDs */
	}
}


