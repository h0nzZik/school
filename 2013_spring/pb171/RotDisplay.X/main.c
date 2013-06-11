/* 
 * File:   main.c
 * Author: vyvoj
 *
 * Created on 24. duben 2013, 14:44
 *
 * Some info about fuses:
 * LVP_OFF	LVP uses RB3, but we have reserved RB3 for RTC clock input.
 */
#define USE_SECONDS_AS_MINUTES	0


#include <pic.h>
#include "main.h"
#include "i2c.h"
__CONFIG(WDTE_OFF & CP_OFF & MCLRE_OFF & FOSC_INTOSCIO & BOREN_OFF & LVP_OFF);


uns8 tmr1_counter = 0;
uns8 tmr0_counter = 0;

uns8 seg_ticks_h;	// how many overflows of Timer0 per segment
uns8 seg_ticks_l;	// and how many ticks..


typedef struct {
	uns8 start;
//	uns8 stop;
    uns8 ticks_to_stop;
//    uns8 countdown;
} start_stop_t;

typedef struct {
	uns8 s60;	/* one of 60 segments ... */
	uns8 tsl;
} segment_t;


segment_t seg;


#define SEG_VALUE_HOUR  0xE0
#define SEG_VALUE_MIN   0x3C
#define SEG_VALUE_ZERO  0x02

#define STATE_STOPPED 0
#define STATE_STARTED 1
#define STATE_RUNNING 2



#define INPUT_BIT	RB7

uns8 state = STATE_STOPPED;

typedef struct {
	uns8 min;
	uns8 hrs;
	uns8 hour60;	/* hrs * 5 */
} time_t;

/**
 * Readers:	next_minute()
 * Writers:	next_minute()
 * This variable does not need to be protected
 */
time_t time;



/* Mode */
#define MODE_NORMAL	0
#define MODE_SETUP	1
uns8 mode;





/* input == RB2, RB7 */

uns8 a_latch;
void bit_on(uns8 bt)
{
	a_latch |= bt;
	PORTA = a_latch;
}

void bit_off(uns8 bt)
{
	a_latch &= ~bt;
	PORTA = a_latch;
}


/**
 * Writes a byte to the shift register
 */

void shift_write(uns8 data)
{
	int i;

	bit_off(STCLK_BIT);
	bit_off(SHCLK_BIT);
	for (i=0; i<8; i++) {
		if ((data & 0x01) != 0) {
			bit_on(DATA_BIT);
		} else {
			bit_off(DATA_BIT);
		}

		bit_on(SHCLK_BIT);
		data = data >> 1;
		bit_off(SHCLK_BIT);
	}
}

void shift_show(void)
{
	bit_on(STCLK_BIT);
	asm("nop");
	bit_off(STCLK_BIT);
}

void shift_send(uns8 data)
{
	shift_write(data);
	shift_show();
}

/**
 * Sets higher frequency
 * @param how_much
 */
void set_it_faster(uns8 how_much)
{
//	shift_send(0x05);
	/* set higher frequency */
	if (how_much == 0)
		OSCCONbits.IRCF2 = 0;
	else
		OSCCONbits.IRCF2 = 1;

	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF0 = 1;
	/* wait until frequency is stable */
	while(!IOFS) {
		;
	}
//	shift_send(0x02);
}


void what_time_is_now(void)
{
	uns8 data;
	set_it_faster(0);
//	time.hrs = 6;
//	time.min = 0;
//	time.hour60 = 30;

//#if 0
	uns8 rv;

	rv = i2c_read(I2C_RTC_DEV, 0x04, &data);
	if (rv)
		goto error;
	time.hrs = data & 0x3F;

	rv = i2c_read(I2C_RTC_DEV, 0x03, &data);
	if (rv)
		goto error;

	time.min = data & 0x7F;
	time.hour60 = time.hrs*5;
//#endif
	set_it_faster(1);

	return;
error:
	shift_send(0x00);
	bit_on(LED_BIT);
	GIE = 0;
	while(1)
		;
	return;



}


/**
 * Partial inversion to çonvert() #1
 * @param x	minute or second	( 0 to  59)
 * @return	segment to start on	( 0 to 127)#### (0 to 63)
 */
uns8 sixty_start(uns8 x)
{
	/* 0..59 => 0..118 */
//	x = x << 1;
	if (x == 0)
		return /*127+1*/ 0;
	if (x <= 30)
		return x+1+1;
	if (x <= 60)
		return x+3+1;
	if (x <= 90)
		return x+5+1;
	/* now x is in 91..118 */
	return x + 7+1;
}

	/* this segment length */
/* hour or minute or something similar must be shown */
bit need_update;
bit updated;
void next_segment(void)
{
#if 1
	if (seg.s60 > 60)
		return;
	seg.tsl--;
	if (seg.tsl != 0)
		return;
#endif

//	uns8 need_update = 0;
	need_update = 0;
	uns8 data = 0;
	if (seg.s60 == 0) {
		data |= SEG_VALUE_ZERO;
		need_update = 1;
	}

	if (seg.s60 == time.min) {
		need_update = 1;
		data |= SEG_VALUE_MIN;
	}

	if (seg.s60 == time.hour60) {
		need_update = 1;
		data |= SEG_VALUE_HOUR;
	}
	/* ok, we now that clock pointer is 1/60 of the circle (== one minute) wide
	 * So if the pointer was shown last time,
	 * we must hide it now. That is the reason of 'need_update' and ''udated'.
	 */
	if (need_update || updated) {
		shift_send(data);
	}
	updated = need_update;


	seg.s60++;
#if 1
	if (seg.s60 == 1 || /*seg.s60 == 15 || */ seg.s60 == 31/* || seg.s60 == 45*/)
		seg.tsl = 2;
	else
		seg.tsl = 1;
#endif
	return;
}



void next_minute(void)
{
#if 1
	time.min++;
	if (time.min < 60)
		return;
#endif
	time.min = 0;
	time.hrs++;
	time.hour60 +=5;
	if (time.hrs < 12)
		return;
	time.hrs = 0;
	time.hour60=0;
#if 0
	/* modify current time. */
	time_next_minute();
	/* modify  */
	//seg.update = 1;
	//segment_calc();
#endif
}

bit need_sync;

void sync(void)
{
	TMR1ON = 0;

	switch (state) {
		case STATE_STOPPED:
			/* start measuring the period */
			TMR1H = 0;
			TMR1L = 0;
			tmr1_counter = 0;
			TMR1IF = 0;
			TMR1ON = 1;
			break;

		case STATE_STARTED:
			what_time_is_now();
			/* fall through */
		case STATE_RUNNING:
			/* ok, show first segment */
			seg.s60 = 0;
			seg.tsl=1;	// Don't wait
			next_segment();
			/* ok, we have the period */
			if (TMR1IF == 1)
				tmr1_counter++;

			/* Divide it by 64 */
			seg_ticks_l = (TMR1L >> 6) | ((TMR1H << 2) & 0xFF);
			seg_ticks_h = (TMR1H >> 6) | ((tmr1_counter << 2) & 0xFF);
            /* Sometimes we need to change the time */

			/* setup timer 0 */
			TMR0 = 256 - seg_ticks_l;
			tmr0_counter = 0;

			/* setup timer 1 */
			TMR1L = 0;
			TMR1H = 0;
			tmr1_counter = 0;
			TMR1IF = 0;
			TMR1ON = 1;


			TMR0IF = 0;	// test		// generate first segment


			break;
	}

	/* next state */
	if (state != STATE_RUNNING)
		state++;
	return;
}









/**
 * Sets RTC into 'normal' state:
 * Clear alarm
 * @return 0 on success, 0xFF on error
 */

uns8 rtc_setup(void)
{
	uns8 table[] = {
		/* basic setup */
		0x00, 0x00,	/* Control register 1 - clear stop and test */
		0x01, 0x11,	/* periodic, timer interrupt enable */
		0x0D, 0x83,	/* 1024 Hz frequency out */

		/* seconds or minutes? */
//		0x0E, USE_SECONDS_AS_MINUTES
		0x0E, 0x80,	/* timer uses minutes as units*/	// test:SECONDS
		0x0F, 0x01,	/* timer (in minutes, see above) */

		/* turn alarms off*/
		0x09, 0x00,	/* minute */
		0x0A, 0x00,	/* hour */
		0x0B, 0x00,	/* day */
		0x0C, 0x00,	/* weekday */

		0xFF, 0xFF	/* end */
	};

	uns8 i=0;
	while (table[i] != 0xFF) {
		uns8 rv;
		rv = i2c_write(I2C_RTC_DEV, table[i], table[i+1]);
		if (rv)
			return 0xFF;
		i += 2;
	}
}

/**
 * Resets the application, running in normal mode
 */
void application_reset(void)
{
	if (mode != MODE_NORMAL)
		return;
	state = STATE_STOPPED;
	shift_send(0x5A);

	tmr1_counter = 0;
	tmr0_counter = 0;



	seg.s60 = 0;
	what_time_is_now();

	/* clear timers */
	TMR0 = 0;
	TMR1L = 0;
	TMR1H = 0;

	/* clear interrupt requests */
	TMR0IF = 0;
	TMR1IF = 0;

    need_sync = 0;
}

void test_rtc(void)
{
	uns8 rv;
	uns8 data;
	rv = i2c_write(I2C_RTC_DEV, 0x09, 0x05);
	if (rv)
		goto error;
	data = 0x1C;	// dummy
	rv = i2c_read(I2C_RTC_DEV, 0x09, &data);
	if (rv)
		goto error;

	bit_on(LED_BIT);

	for (rv=255; rv > 0;rv--)
		;
	if (data != 0x05)
		goto error;

	while(1)
		;





error:
	bit_off(LED_BIT);
	while(1)
		;

}


/**
 * Initialize device into normal working state.
 * In this state device only shows what time is now.
 */
void init_normal(void)
{
	mode = MODE_NORMAL;
	if (rtc_setup())
		goto error;
	what_time_is_now();
	set_it_faster(1);


	/* enable timer 1, do not use interrupt */
	TMR1IF = 0;
	TMR1IE = 0;
	T1CON = 0;
	TMR1ON = 1;

	/* enable timer 0, do not use interrupt */
	TMR0IF = 0;
	TMR0IE = 0;
	T0CS = 0;	// timer mode

	state = STATE_STOPPED;

	/* enable portB interrupt */
	RBIE = 1;
	/* enable ext int */
	INTE = 0;	// TEST: stop it
	/* enable interrupts */
	PEIE = 1;
	GIE = 1;
	return;

error:
	bit_on(LED_BIT);
	shift_send(0x00);
	while(1)
		;
}

/**
 * Initialize device into setup state.
 * In this state user can set what time is now
 * and it will be written into RTC.
 */
void init_setup(void)
{
	mode = MODE_SETUP;
//	test_rtc();

}


void init(void)
{
	set_it_faster(0);
//    ANSEL  = 0;
	/* set portA as output */
	TRISA = 0;
	TRISA = 0x40;

	ADCON1 = 0;
//	asm("nop");
	PORTA = 0x00;
	a_latch = 0x00;
//	asm("nop");

	/* set portB as input */
	TRISB = 0xFF;

	
	if (BUTTON_LEFT == 0)
		init_setup();
	else
		init_normal();
}

/******************************************************************************/

/**
 * Handles interrupts
 */
void interrupt interrupt_handler(void)
{
	/* in normal mode interrupt occurs only if there is a change on diode */
	if (mode == MODE_NORMAL) {


		/* port B interrupt */
		static char c=0;
		if (RBIF) {
			/* button */
			if (INPUT_BIT == 1) {
				/* ignore every second pulse */
				if(c == 0){
                    need_sync = 1;
//                    sync();
					c = 1;
				} else {
					c = 0;
				}
			}
			RBIF = 0;
		}
#if 0
		if (INTF) {
//			set_it_faster(0);
//			i2c_write(I2C_RTC_DEV, 0x01, 0x11);
//			set_it_faster(1);
//			next_minute();
			INTF = 0;
		}
#endif
	}

}

bit last_clk = 0;

#define wait_for_release(x)	do {;} while((x))
#define wait_for_press(x)	do {;} while(!(x))

#define short_delay()	do {uns8 _i; for (_i=0; _i<200; _i++);} while(0)

#define EVENT_LEFT_UP	0x01
#define EVENT_LEFT_DOWN	0x02
#define EVENT_RIGHT_UP	0x10
#define EVENT_RIGHT_DOWN	0x20

bit _a, _b;
uns8 wait_for_event()
{
//	uns8 a;
//	uns8 b;
	_a = BUTTON_LEFT;
	_b = BUTTON_RIGHT;
	
	while(_a == BUTTON_LEFT && _b == BUTTON_RIGHT)
		;
	
	short_delay();
	uns8 rv = 0;

	if (_a == 0 && BUTTON_LEFT == 1)
		rv |= EVENT_LEFT_UP;
	if (_a == 1 && BUTTON_LEFT == 0)
		rv |= EVENT_LEFT_DOWN;
	
	if (_b == 0 && BUTTON_RIGHT == 1)
		rv |= EVENT_RIGHT_UP;
	if (_b == 1 && BUTTON_RIGHT == 0)
		rv |= EVENT_RIGHT_DOWN;
	return rv;
}


#if 0
void wait_for_release()
{
	while (BUTTON_LEFT)
		;
}
#endif

void main(void) {
main_label:
	init();


	uns8 i = 0;

	if (mode == MODE_NORMAL) {

		application_reset();
		bit_on(LED_BIT);
		while(1) {

            if (need_sync == 1) {
				need_sync = 0;
//				bit_on(LED_BIT);
				sync();
//				bit_off(LED_BIT);
            }


			/* Should we show next segment right now? */
			if (TMR0IF) {
				if (state == STATE_RUNNING) {
					/* show next */
					if (tmr0_counter == seg_ticks_h) {
						tmr0_counter = 0;
						TMR0 = /*256*/ TMR0 - seg_ticks_l;
						next_segment();
					} else {
						tmr0_counter++;
					}
				}
				TMR0IF = 0;
			}

			/* measuring the period */
			/* Poll Timer1 */
			if (TMR1IF) {
				tmr1_counter++;

				/* it is a long time ... */
				if (tmr1_counter >= 40) {
					tmr1_counter = 0;
					if (state != STATE_STOPPED)
						application_reset();
				}
				TMR1IF = 0;
			}


			/* poll clock source */
			if (last_clk != RTC_CLOCK_SRC) {
				if (last_clk == 0)
					next_minute();
				last_clk = RTC_CLOCK_SRC;
			}
		}
	} /* mode == MODE_NORMAL */

	/* only sets up the time (use buttons) */
	if (mode == MODE_SETUP) {
		uns8 ev;
		uns8 tmp = 0;
		uns8 which = 0;	// 1 == minutes, 0 == hours
		uns8 addr;

		bit_on(LED_BIT);
		/* wait for button release */
		while (BUTTON_LEFT == 0)
			;
		bit_off(LED_BIT);
		shift_send(0x00);

		do {
			ev = wait_for_event();
			switch(ev) {
				case EVENT_RIGHT_UP:
					if (which == 1 && tmp == 0)
						tmp = 59;
					else if (which == 0 && tmp == 0)
						tmp = 11;
					else
						tmp--;
					break;

				case EVENT_LEFT_UP:
					if ((which == 1 && tmp == 59) || (which == 0 && tmp == 11))
						tmp = 0;
					else
						tmp++;
					break;
				case EVENT_LEFT_UP | EVENT_RIGHT_UP:
					if (which == 0)
						addr = 0x04;
					if (which == 1)
						addr = 0x03;
					i2c_write(I2C_RTC_DEV, addr, tmp);
					tmp = 0;
					which++;

					break;
//				default:
					//tmp--;
			}
			shift_send(tmp | which << 6);
		} while (which < 2);

		shift_send(0xFE);
		goto main_label;
		while(1)
			;

	}
}

