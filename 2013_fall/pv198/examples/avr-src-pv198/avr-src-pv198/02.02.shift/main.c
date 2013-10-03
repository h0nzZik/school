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
#include "shift.h"

void DevBoardShiftLedOut( uint8_t val );
uint8_t DevBoardShiftByteIn( void );

/*
 * MAIN
 */
int main(void)
{
	while(1)
	{
		/* Rewrite inputs from DIP SWITCH to bargraph */
		DevBoardShiftLedOut(DevBoardShiftByteIn());
	}
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

/*
 * Read data from 8x shift input interface.
 */
uint8_t DevBoardShiftByteIn( void )
{
	uint8_t in_byte=0, i=7;

	SHIFT_DDR |= _BV(SHIFT_IN_LOAD_DDR);	/* shift/!load set as output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* clock set as output pin */
	SHIFT_DDR &= ~(_BV(SHIFT_IN_DDR));	/* serial data set as input pin */

	SHIFT_PORT &= ~(_BV(SHIFT_CLK));	/* set clock low  */
	SHIFT_PORT &= ~(_BV(SHIFT_IN_LOAD));	/* set load low  -> parallel in */

	asm volatile("nop\n\t");		/* wait */
	SHIFT_PORT |= _BV(SHIFT_IN_LOAD);       /* set load high -> serial out */

	/* read bit, MSB(most significant bit) first */
	bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0);
	while(i--)
	{
		SHIFT_PORT |= _BV(SHIFT_CLK);	/* 0 -> 1 next databit on serial output is valid */
		asm volatile("nop\n\t");	/* wait */
		in_byte <<= 1;			/* shift one bit left */
		bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0); /* read next bit */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
		asm volatile("nop\n\t");	/* wait */
	}

	return(in_byte);			/* return received byte */
}

