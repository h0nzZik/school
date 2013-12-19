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
#include <stdlib.h>
#include "config.h"

#define nop() asm volatile("nop\n\t")

void DevBoardShiftLedOut( uint8_t val );
uint8_t DevBoardShiftByteIn( void );


typedef struct {
	void (*init)(void);
	void (*fun)(void);
} functionality_t;

union {
	struct {
		uint8_t cnt;
		uint8_t dir;
	} f_0;

	struct {
		uint8_t cnt;
		uint8_t dir;
	} f_1;

	struct {
		uint8_t i;
	} f_2;

	struct {

	} f_3;
} u;

void f0_bouncing_led_init(void)
{
	u.f_0.cnt = 0;
	u.f_0.dir = 0;
}


void f0_bouncing_led_step(void)
{
	DevBoardShiftLedOut(~(1 << u.f_0.cnt));
	if (u.f_0.dir == 1) {
		if (u.f_0.cnt != 0) {
			u.f_0.cnt--;
		} else {
			u.f_0.dir = 0;
		}
	} else {
		if (u.f_0.cnt != 7) {
			u.f_0.cnt++;
		} else {
			u.f_0.dir = 1;
		}
	}
}


void f1_bouncing_2leds_init(void)
{
	u.f_1.cnt = 0;
	u.f_1.dir = 0;
}


void f1_bouncing_2leds_step(void)
{
	DevBoardShiftLedOut(1 << u.f_1.cnt | 0x80 >> u.f_1.cnt);
	if (u.f_1.dir == 1) {
		if (u.f_1.cnt != 0) {
			u.f_1.cnt--;
		} else {
			u.f_0.dir = 0;
		}
	} else {
		if (u.f_1.cnt != 3) {
			u.f_1.cnt++;
		} else {
			u.f_1.dir = 1;
		}
	}
}

void f2_strange_led_init(void)
{
	u.f_2.i = 0;
}


void f2_strange_led_step(void)
{
	static const char arr[] = {0x01,0x80,0x44,0x12};
	DevBoardShiftLedOut(~(arr[u.f_2.i++]));
	u.f_2.i %= 4;
}




static functionality_t handlers[4] = {
	{
		.init = f0_bouncing_led_init,
		.fun = f0_bouncing_led_step
	},
	{
		.init = f1_bouncing_2leds_init,
		.fun = f1_bouncing_2leds_step
	},
	{
		.init = f2_strange_led_init,
		.fun = f2_strange_led_step
	
	},
	{
		.init = NULL,
		.fun = NULL
	}
};


/*
 * MAIN
 */
int main(void)
{
	uint8_t state = 0xFF;
	while(1)
	{
		uint8_t new_state;

		new_state = 0x03 & DevBoardShiftByteIn();
		if (new_state != state) {
			if (handlers[new_state].init)
				handlers[new_state].init();
			state = new_state;
		}

		if (handlers[state].fun)
			handlers[state].fun();


		// TODO: insert a delay here
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

		nop();
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
	}

	nop();
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

	nop();
	SHIFT_PORT |= _BV(SHIFT_IN_LOAD);       /* set load high -> serial out */

	/* read bit, MSB(most significant bit) first */
	bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0);
	while(i--)
	{
		SHIFT_PORT |= _BV(SHIFT_CLK);	/* 0 -> 1 next databit on serial output is valid */
		nop();
		in_byte <<= 1;			/* shift one bit left */
		bit_is_set(SHIFT_PIN, SHIFT_IN) ? (in_byte |= 1) : (in_byte |= 0); /* read next bit */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
		nop();
	}

	return(in_byte);			/* return received byte */
}

