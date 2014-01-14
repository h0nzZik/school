/*************************************************************************\
*                  LED Stranger                                           *
*-------------------------------------------------------------------------*
* Description :  The aim of this example is to be strange as possible.    *
*                You should train how to build simple C program and how   *
*                to program ATmega128 via JTAG or SPI interface.          *
*-------------------------------------------------------------------------*
* Author      : Zbynek Bures & Pavel Celeda & Jan Tusil                   *
* Version     :
*-------------------------------------------------------------------------*
* Compiler    : gcc                       Version:  4.1.0                 *
*-------------------------------------------------------------------------*
* Target system : Charon 2 Development Kit                                *
*                 ATmega128 14,7456 MHz, UART: 9600,N,8,1                 *
\*************************************************************************/
#include <stdlib.h>
#include <util/delay.h>
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
		uint8_t state;
		uint8_t delay;

	} f_3;
} u;

void f0_bouncing_led_init(void)
{
	u.f_0.cnt = 0;
	u.f_0.dir = 0;
}


void f0_bouncing_led_step(void)
{
	PORTD = ~(1 << u.f_0.cnt);
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
	PORTD = 1 << u.f_1.cnt | 0x80 >> u.f_1.cnt;
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
	PORTD = ~(arr[u.f_2.i++]);
	u.f_2.i %= 4;
}


void f3_strange_led_init(void)
{
	u.f_3.state = 0;
	u.f_3.delay = 0;
}

void f3_strange_led_step(void)
{
	u.f_3.delay++;
	u.f_3.delay %= 10;
	if (u.f_3.delay)
		return;

	uint8_t data = 1 << (u.f_3.state & 0x07);
	if (u.f_3.state & 0x80)
	{
		data = ~data;
		u.f_3.state &= ~0x80;
		u.f_3.state++;
		if (u.f_3.state >= 8)
			u.f_3.state = 0;
	} else {
		u.f_3.state |= 0x80;
	}


	PORTD = data;
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
		.init = f3_strange_led_init,
		.fun = f3_strange_led_step
	}
};


/*
 * MAIN
 */
int main(void)
{
	uint8_t state = 0xFF;
	DDRD  = 0xff;		/* use all pins on Port D for output */
	DDRB  = 0x00;		/* use all pins on Port B for input */
	while(1)
	{
		uint8_t new_state;

		new_state = 0x03 & (PINB >> 6);
		if (new_state != state) {
			if (handlers[new_state].init)
				handlers[new_state].init();
			state = new_state;
		}

		if (handlers[state].fun)
			handlers[state].fun();


		_delay_ms(80);
	}
}

