/* 
 * File:   main.c
 * Author: vyvoj
 *
 * Created on 24. duben 2013, 14:44
 *
 * Some info about fuses:
 * LVP_OFF	LVP uses RB3, but we have reserved RB3 for RTC clock input.
 */

#include <pic.h>
#include "main.h"
#include "i2c.h"
__CONFIG(WDTE_OFF & CP_OFF & MCLRE_OFF & FOSC_INTOSCIO & BOREN_OFF & LVP_OFF);




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
//		bit_on(STCLK_BIT);
		data = data >> 1;
		bit_off(SHCLK_BIT);
//		bit_off(STCLK_BIT);
	}
	bit_on(STCLK_BIT);
	asm("nop");
	bit_off(STCLK_BIT);

}


uns8 tmr1_counter = 0;
uns8 tmr0_counter = 0;

uns8 seg_ticks_h;	// how many overflows of Timer0 per segment
uns8 seg_ticks_l;	// and how many ticks..


typedef struct {
	uns8 start;
	uns8 stop;
} start_stop_t;

struct {
	uns8 raw;	/* one of 128 segments where we are right now */
	uns8 hrl;	/* one of  12 hour-like segments we are going to be */

	start_stop_t min;
	start_stop_t hour;
	
	start_stop_t min_next;
	start_stop_t hour_next;
	
#define SEGF_NEW_MIN	0x01
#define SEGF_NEW_HOUR	0x02
	uns8 flags;

#define SEG_SHOW_HOUR	0x01
#define SEG_SHOW_MIN	0x02
	uns8 show;
} segment;



#define STATE_STOPPED 0
#define STATE_STARTED 1
#define STATE_RUNNING 2



#define INPUT_BIT	RB7

uns8 state = STATE_STOPPED;

typedef struct {
	uns8 min;
	uns8 hrs;
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



/**
 * Converts x/128 fraction to x/120
 * @param x
 * @return	number in range 0..119
 */

uns8 convert(uns8 x)
{
	if (x < 2 || x == 127)
		return 0;
	if (x < 31)
		return x-1;
	if (x < 34)
		return 30;
	if (x < 63)
		return x-3;
	if (x < 66)
		return 60;
	if (x < 95)
		return x-5;
	if (x < 98)
		return 90;
	/* now x is in 28..126 */
	return x-7;
}


/**
 * Partial inversion to çonvert() #1
 * @param x	minute or second	( 0 to  59)
 * @return	segment to start on	( 0 to 127)
 */
uns8 sixty_start(uns8 x)
{
	/* 0..59 => 0..118 */
	x = x << 1;
	if (x == 0)
		return 127;
	if (x <= 30)
		return x+1;
	if (x <= 60)
		return x+3;
	if (x <= 90)
		return x+5;
	/* now x is in 91..118 */
	return x + 7;
}

/**
 * Partial inversion to çonvert() #2
 * @param x	minute or second	( 0 to  59)
 * @return	segment to stop on	( 0 to 127)
 */
uns8 sixty_stop(uns8 x)
{
	x = x << 1;
	if (x < 30)
		return x+2;
	if (x < 60)
		return x+4;
	if (x < 90)
		return x+6;
	/* x is in 90..119*/
	return x+8;
}


uns8 hour_start[12];
uns8 hour_end[12];

void hour_start_end_init(void)
{
	uns8 i;
	for (i=0; i<12; i++) {
		uns8 min;
		min = i << 2 + i;	// i*5
		min = i*5;
		hour_start[i] = sixty_start(min);
		hour_end[i] = sixty_stop(min);
	}
}
void show_face()
{
	if (segment.hrl >= 12)
		return;
	if (hour_start[segment.hrl] == segment.raw)
		bit_on(LED_BIT);
	else if (hour_end[segment.hrl] == segment.raw){
		bit_off(LED_BIT);
		segment.hrl++;
	}

}



void show_clk(void)
{
	/* change time */
	if (segment.raw == 0 && segment.flags != 0) {
		segment.flags = 0;
		segment.hour = segment.hour_next;
		segment.min = segment.min_next;
	}
	/* show minute */
	if (segment.raw == segment.min.start) {
		segment.show |= SEG_SHOW_MIN;
	} else if (segment.raw == segment.min.stop ) {
		segment.show &=~SEG_SHOW_MIN;
	}

	/* show hour */
	if (segment.raw == segment.hour.start) {
		segment.show |= SEG_SHOW_HOUR;
	} else if (segment.raw == segment.hour.stop ) {
		segment.show &=~SEG_SHOW_HOUR;
	}

	switch (segment.show) {
		case 0:
			shift_write(0x00);
			break;
		case SEG_SHOW_MIN:
			shift_write(0x1F);
			break;
		case SEG_SHOW_HOUR:
			shift_write(0xF0);
			break;
		case SEG_SHOW_HOUR | SEG_SHOW_MIN:
			shift_write(0xFF);
			break;
	}
}

uns8 red_start;
uns8 red_stop;
void next_segment(void)
{
	GIE = 0;

	if (segment.raw >= 128) {
		// shift_write(0x00);	/* ok, turn all leds off. See segment.show */
		goto ns_end;
	}

	if (segment.raw == red_start)
		bit_on(LED_BIT);
	else if (segment.raw == red_stop)
		bit_off(LED_BIT);

//	show_face();
	show_clk();

	segment.raw++;

ns_end:
	GIE = 1; // enable interrupts
	return;
}



void time_next_minute(void)
{
//	new_time = time;
	time.min++;
	if (time.min < 60)
		return;

	time.min = 0;
	time.hrs++;
	if (time.hrs < 12)
		return;
	time.hrs = 0;
}

/**
 * Calculate new segment numbers for current time.
 * 
 */

void segment_calc(void)
{
	segment.min_next.start = sixty_start(time.min);
	segment.min_next.stop = sixty_stop(time.min);

	segment.hour_next.start = sixty_start(time.hrs * 12);
	segment.hour_next.stop = sixty_stop(time.hrs * 12);

	segment.flags = SEGF_NEW_HOUR | SEGF_NEW_MIN;
	//segment.show = 0;
}

void next_minute(void)
{
	/* modify current time. */
	time_next_minute();
	/* modify  */
	segment_calc();
}

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
		case STATE_RUNNING:
			/* ok, we have the period */
			if (TMR1IF == 1)
				tmr1_counter++;
			/* divide it by 128 */
			seg_ticks_l = (TMR1L >> 7) |  ((TMR1H << 1) & 0xFF);
			seg_ticks_h = (TMR1H >> 7) | ((tmr1_counter << 1) & 0xFF);
			/* ok, show first segment */
			segment.raw = 0;
			segment.hrl = 0;
	//		next_segment();

			/* setup timer 1 */
			TMR1L = 0;
			TMR1H = 0;
			tmr1_counter = 0;
			TMR1IF = 0;
			TMR1ON = 1;

			/* setup timer 0*/
			TMR0 = 256 - seg_ticks_l;
			tmr0_counter = 0;
			TMR0IF = 1;	// test		// generate first segment


			break;
		default:
			;	/* ... */
	}

	/* next state */
	if (state != STATE_RUNNING)
		state++;
	return;
}


void what_time_is_now()
{
	time.hrs = 3;
	time.min = 25;
	/* read RTC's current time */
//	uns8 h,m,s;
//	s =

}






/**
 * Sets higher frequency
 * @param how_much
 */
void set_it_faster(uns8 how_much)
{
	shift_write(0x05);
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
	shift_write(0x02);
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
	shift_write(0x5A);

	tmr1_counter = 0;
	tmr0_counter = 0;


	segment.show = 0;
	what_time_is_now();
	segment_calc();

	/* clear timers */
	TMR0 = 0;
	TMR1L = 0;
	TMR1H = 0;

	/* clear interrupt requests */
	TMR0IF = 0;
	TMR1IF = 0;
}


/**
 * Initialize device into normal working state.
 * In this state device only shows what time is now.
 */
void init_normal(void)
{
	mode = MODE_NORMAL;
	what_time_is_now();
	set_it_faster(1);
	hour_start_end_init();

	red_start = sixty_start(45);
	red_stop = sixty_stop(15);

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
}

/**
 * Initialize device into setup state.
 * In this state user can set what time is now
 * and it will be written into RTC.
 */
void init_setup(void)
{
	mode = MODE_SETUP;

}


void init(void)
{
	set_it_faster(0);
//    ANSEL  = 0;
	/* set portA as output */
	TRISA = 0;
	ADCON1 = 0;
//	asm("nop");
	PORTA = 0x00;
	a_latch = 0x00;
//	asm("nop");

	/* set portB as input */
	TRISB = 0xFF;

	rtc_setup();
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
					sync();
					c = 1;
				} else {
					c = 0;
				}
			}
			RBIF = 0;
		}
		if (INTF) {
//			set_it_faster(0);
//			i2c_write(I2C_RTC_DEV, 0x01, 0x11);
//			set_it_faster(1);
//			next_minute();
			INTF = 0;
		}
	}
}

bit last_clk = 0;

void main(void) {

	init();

	application_reset();

	uns8 i = 0;

	if (mode == MODE_NORMAL) {
		bit_on(LED_BIT);
		while(1) {
			/* Should we show next segment right now? */
			if (TMR0IF) {
				if (state == STATE_RUNNING) {
					/* show next */
					if (tmr0_counter == seg_ticks_h) {
						tmr0_counter = 0;
						TMR0 = 256 - seg_ticks_l;
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
					application_reset();
				}
				TMR1IF = 0;
			}

			/* poll clock source */

			if (last_clk == 1 && RTC_CLOCK_SRC == 0) {
				next_minute();
			}

			last_clk = RTC_CLOCK_SRC;


		}
	} /* mode == MODE_NORMAL */

	/* only sets up the time (use buttons) */
	if (mode == MODE_SETUP) {
		while(1)
			;

	}
}

