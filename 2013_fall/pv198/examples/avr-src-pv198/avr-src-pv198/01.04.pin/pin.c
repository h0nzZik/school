/*
 *                  I n p u t - P I N - 2
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
 * $Id: pin.c,v 1.6 2008/10/16 20:39:15 qceleda Exp $
 */
#include <avr/io.h>

#define LED_DDR			DDRD	/* LED data direction register */
#define LED_PORT		PORTD	/* LED port */

#define ALARM_LED_DDR	DDD0	/* ALARM led data direction bit */
#define ALARM_LED		PORTD0	/* ALARM led pin */

#define JMP_DDR			DDRB	/* jumper data direction register */
#define JMP_PIN			PINB	/* jumper pin */

#define SETUP_JMP_DDR	DDB7	/* SETUP jumper data direction bit */
#define SETUP_JMP		PINB7	/* SETUP jumper pin */

int main(void)
{
	LED_DDR |= _BV(ALARM_LED_DDR);		/* set ALARM led pin as output pin '1' */
	JMP_DDR &= ~(_BV(SETUP_JMP_DDR));	/* set SETUP jumper pin as input pin '0' */

	while(1)
	{
		/* bit_is_set(JMP_PIN, SETUP_JMP) ? (LED_PORT |= _BV(ALARM_LED)) : (LED_PORT &= ~(_BV(ALARM_LED))); */

		if(bit_is_set(JMP_PIN, SETUP_JMP)) 
		{
			LED_PORT |= _BV(ALARM_LED);		/* turn off led */
		}
		else 
		{
			LED_PORT &= ~(_BV(ALARM_LED));	/* turn on led */	
		}
	}
}

