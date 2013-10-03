/*
 *                 ATmega128 Power Management and Sleep Modes Example
 *
 * Description : Trigger external interrupt INT4 to wake up ATmega128.
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
#include <avr/sleep.h>
#include <util/delay.h>

/*
 * External interrupt INT4 interrupt service routine.
 */
ISR(INT4_vect)
{
				/* just wake up the CPU */
}

int main(void)
{
	uint8_t i, 
		led;		/* port D LEDs */

	DDRD = 0xFF;		/* set port D as output */
	DDRE &= ~(_BV(PORTE4));	/* set PE4 as input */
	PORTE |= _BV(PORTE4);	/* enable PE4 pull up resistor */

	cli();			/* global interrupt disable */
	EIMSK = 0x00;		/* disable all external interrupts */
	EICRB = 0x00;		/* 0 0 The low level of INT4 generates an interrupt request. */
	EIFR = 0x00;		/* clear all interrupt flags */
	EIMSK = _BV(INT4);	/* enable external interrupts */
	sei();			/* global interrupt enable */

	/*
	 * SLEEP_MODE_IDLE          ... Idle
	 * SLEEP_MODE_ADC           ... ADC Noise Reduction
	 * SLEEP_MODE_PWR_DOWN      ... Power-down
	 * SLEEP_MODE_PWR_SAVE      ... Power-save
	 * SLEEP_MODE_STANDBY       ... Standby
	 * SLEEP_MODE_EXT_STANDBY   ... Extended Standby
	 */

	set_sleep_mode(SLEEP_MODE_IDLE); /* set sleep mode */

	while(1) 
	{
		led = 1;	/* indicate we are ready */
		PORTD = ~led;

		sleep_enable();		/* set Sleep Enable bit */
		sleep_cpu();		/* sleep - waiting for INT4 */
		sleep_disable();	/* clear Sleep Enable bit */

		while (led)	/* action after wake-up */
		{
			led <<= 1;	/* walking LED */
			PORTD = ~led;	

			for (i = 0; i < 100; i++)	/* delay 1s */
				_delay_ms(10);
		}
	}
}
