/*
 *                 ATmega128 Power Management and Sleep Modes Example
 *
 * Description : Trigger timer 0 compare match interrupt to wake up ATmega128.
 *
 * Author: Pavel Celeda - celeda@liberouter.org
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *                 XTAL 32,768 kHz - TOSC1 and TOSC2
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
 * Timer/Counter 0 output compare match interrupt service routine.
 */
ISR(TIMER0_COMP_vect)
{
	static uint8_t led = 1;	/* current LED */

	PORTD = ~led;		/* walking LED */

	led <<= 1;		/* shift to next LED */

	if(!led)
		led = 1;	/* start again from zero */
}

int main(void)
{
	DDRD = 0xFF;			/* set port D as output */
	PORTD = 0xFF;			/* turn all LEDs off */

	#if 1				/* select asynchronous timer/counter 0 clock source */
					/* XTAL - 32,768 kHz - TOSC1 and TOSC2 */
		ASSR |= _BV(AS0);	/* clocked from a crystal oscillator connected to TOSC1 pin */
	#else
					/* XTAL - 14,7456 MHz - XTAL1 and XTAL2 */
		ASSR &= ~(_BV(AS0));	/* clocked from the I/O clock */
	#endif

	TIMSK |= _BV(OCIE0);		/* enable counter 0 output compare match interrupt */
	TCCR0 |= _BV(WGM01);		/* set clear timer 0 on compare match (CTC) mode */
	TIFR = 0x00;			/* clear timer/counter interrupt flag register */

					/* 32768 Hz / 1024 = 32 */
	OCR0 = 32;			/* set output compare register */
	TCCR0 |= _BV(CS02)|_BV(CS01)|_BV(CS00);	/* set 1024 prescaler and start counting */

	_delay_ms(1);			/* setup delay - this is vital !! */

	/* see ATmega128 datasheet - page 44.
	 * SLEEP_MODE_IDLE          ... Idle
	 * SLEEP_MODE_ADC           ... ADC Noise Reduction
	 * SLEEP_MODE_PWR_DOWN      ... Power-down
	 * SLEEP_MODE_PWR_SAVE      ... Power-save
	 * SLEEP_MODE_STANDBY       ... Standby
	 * SLEEP_MODE_EXT_STANDBY   ... Extended Standby
	 */

	//set_sleep_mode(SLEEP_MODE_IDLE);	/* both clocks wake-up CPU */
	//set_sleep_mode(SLEEP_MODE_ADC);	/* both clocks wake-up CPU */
	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);	/* no wake-up possible */
	//set_sleep_mode(SLEEP_MODE_PWR_SAVE);	/* asynchronously clocked timer0 wakes-up CPU */
	//set_sleep_mode(SLEEP_MODE_STANDBY);	/* no wake-up possible */
	set_sleep_mode(SLEEP_MODE_EXT_STANDBY);	/* asynchronously clocked timer0 wakes-up CPU */

	sei();				/* global interrupt enable */

	while(1) 
	{
		sleep_mode(); 		/* sleep - waiting for TIMER0 CTC INT */
	}
}
