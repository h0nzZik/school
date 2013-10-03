/*
 *                  R S 2 3 2 - RTS/CTS signals - e x a m p l e
 *
 * Description : Displays RTS/CTS status on leds (PORTD).
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.3 2008/11/07 08:35:55 qceleda Exp $
 */
#include <avr/io.h>
					/* UART0 definitions */
#define UART0_PORT            PORTE
#define UART0_DDR             DDRE
#define UART0_PIN             PINE

#define UART0_RTS             PORTE3
#define UART0_CTS             PORTE2
#define UART0_TXD             PORTE1
#define UART0_RXD             PORTE0

#define UART0_RTS_DDR         DDE3
#define UART0_CTS_DDR         DDE2
#define UART0_TXD_DDR         DDE1
#define UART0_RXD_DDR         DDE0

int main(void)
{

	UART0_DDR |= _BV(UART0_RTS_DDR);	/* set RTS pin direction - output */
	UART0_DDR &= ~(_BV(UART0_CTS_DDR));	/* set CTS pin direction - input */
	UART0_DDR &= ~(_BV(UART0_RXD_DDR));	/* set RXD pin direction - input */
	UART0_PORT |= _BV(UART0_CTS);		/* enable AVR internal pull-up resistor */
	UART0_PORT |= _BV(UART0_RXD);		/* enable AVR internal pull-up resistor */

	DDRD  = 0xff;				/* use all pins on Port D for output */
	PORTD = 0xff;				/* set output high -> turn all LEDs off */

	while(1)
	{
		if(bit_is_set(UART0_PIN, UART0_CTS))		/* check CTS pin */
		{
			UART0_PORT |= _BV(UART0_RTS);		/* set RTS pin */
			PORTD &= ~(_BV(PORTD0));		/* show RTS pin high */
		}
		else
		{
			UART0_PORT &= ~(_BV(UART0_RTS));	/* clear RTS pin */
			PORTD |= _BV(PORTD0);			/* show RTS pin low */
		}

		if(bit_is_set(UART0_PIN, UART0_RXD))		/* check RXD pin */
		{
			PORTD &= ~(_BV(PORTD7));		/* show RXD pin high */
		}
		else
		{
			PORTD |= _BV(PORTD7);			/* show RXD pin low */
		}
	}
}

