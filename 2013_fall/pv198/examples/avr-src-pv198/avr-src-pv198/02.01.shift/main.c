/*************************************************************************\
*                  S h i f t - O U T                                      *
*-------------------------------------------------------------------------*
* Description :  The aim of this example is to be simple as possible.     *
*                You should train how to build simple C program and how   *
*                to program ATmega128 via JTAG or SPI interface.          *
*-------------------------------------------------------------------------*
* Author      : Zbynek Bures & Pavel Celeda                               *
* Version     :
*-------------------------------------------------------------------------*
* Compiler    : gcc                       Version:  4.1.0                 *
*-------------------------------------------------------------------------*
* Target system : Charon 2 Development Kit                                *
*                 ATmega128 14,7456 MHz, UART: 9600,N,8,1                 *
\*************************************************************************/
#include <util/delay.h>
#include "shift.h"

void Delay_1s( void );
void DevBoardShiftLedOut( uint8_t val );

/* time structure tens of seconds and seconds */
typedef struct {
	uint8_t	sec_l:4;	/* low digit - units seconds */
	uint8_t sec_h:4;	/* high digit - tens seconds */
} _TIME;

_TIME TIME;

/* MAIN */
int main(void)
{
	uint8_t tmp;

	while(1)
	{
		tmp = (TIME.sec_h << 4) | (TIME.sec_l);	/* concatenate seconds */
		DevBoardShiftLedOut(~tmp);		/* send value to bargraph */
		TIME.sec_l++;				/* inc sel_l */
		if (TIME.sec_l == 0xa) {		/* make sec_l correction */
			TIME.sec_l = 0;
			TIME.sec_h++;			/* inc sel_h */
			if (TIME.sec_h == 6) {		/* make sec_h correction */
				TIME.sec_h = 0;
				TIME.sec_l = 0;
			}
		}
		Delay_1s();				/* wait 1 sec */
	}
}

/*
 * Delay function : 1 second
 */
void Delay_1s(void)
{
	uint8_t i;

	for (i = 0; i < 100; i++)	/* wait 100 x 10 ms <=> 1 sec */
		_delay_ms(10);
}

/*
 * Send data to 8x shift LED shift interface.
 */
void DevBoardShiftLedOut( uint8_t val )
{
	uint8_t i=8;

	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */

	SHIFT_PORT &= ~(_BV(SHIFT_LED_SET));	/* define start values */
	SHIFT_PORT &= ~(_BV(SHIFT_CLK));

	while( i-- )
	{	 /* send data to 74595, MSB (most significant bit) first */
		if( val & (1 << i) ) SHIFT_PORT |= _BV(SHIFT_OUT);
		else SHIFT_PORT &= ~(_BV(SHIFT_OUT));

		SHIFT_PORT |= _BV(SHIFT_CLK);   /* 0 -> 1 data valid */

		asm volatile("nop\n\t");	/* wait */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
	}

	asm volatile("nop\n\t");		/* wait */
	SHIFT_PORT |= _BV(SHIFT_LED_SET);	/* 0 -> 1 parallel write */
}

