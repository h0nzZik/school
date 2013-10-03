/*
 *                  B l i n k i n g - L E D
 *
 * Description : The aim of this example is to be simple as possible.
 *               You should train how to build simple C program and how
 *               to program ATmega128 via JTAG or SPI interface.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: led.c,v 1.3 2006/12/04 15:46:56 qceleda Exp $
 */
#include <avr/io.h>
#include <util/delay.h>

static void delay_1s(void)
{

	uint8_t i;

	for (i = 0; i < 100; i++)
		_delay_ms(10);
}

int main(void)
{
	DDRD  = 0xff;		/* use all pins on Port D for output */

	while(1)
	{
		PORTD = 0xff;	/* set output high -> turn all LEDs off */
		delay_1s();
		PORTD = 0xfe;	/* turn on PD0 led */
		delay_1s();
	}
}


