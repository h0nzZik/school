/* 
 * File:   main.c
 * Author: vyvoj
 *
 * Created on 24. duben 2013, 14:44
 */

#include <pic.h>
__CONFIG(WDTE_OFF & CP_OFF & MCLRE_OFF & FOSC_INTOSCIO & BOREN_OFF );


#define LED_BIT		0x02
#define DATA_BIT	0x04

/* FIXME: Chyba v dokumentaci */
#define SHCLK_BIT	0x10    /* shift clock */
#define STCLK_BIT	0x08    /* store clock */

/* buttons */
#define BUTTON_LEFT	RB5

typedef unsigned char uns8;

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




uns8 tmr1_counter = 0;
uns8 tmr0_counter = 0;

uns8 seg_ticks_h;	// how many overflows of Timer0 per segment
uns8 seg_ticks_l;	// and how many ticks..

uns8 seg = 0;

#define STATE_STOPPED 0
#define STATE_STARTED 1
#define STATE_RUNNING 2



#define INPUT_BIT	RB7

char running = STATE_STOPPED;



/**
 * Writes a byte to the shift register
 */

void shift_write(uns8 data)
{
//	GIE = 0;
	/* DEBUG */
//	data = 0x00;
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
//	GIE = 1;
}

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
 * @param x	minute or second
 * @return	segment to start on
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
 * @param x	minute or second (range 0 to 59)
 * @return	segment to stop on
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

#if 1
uns8 twelve_start;
uns8 twelve_stop;
uns8 three_start;
uns8 three_stop;
uns8 six_start;
uns8 six_stop;
uns8 nine_start;
uns8 nine_stop;
#endif


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
	uns8 i;
	for (i=0; i<12; i++) {
		if (hour_start[i] == seg) {
			bit_on(LED_BIT);
			return;
		}
		if (hour_end[i] == seg) {
			bit_off(LED_BIT);
			return;
		}
	}
}

void next_segment(void)
{
	GIE = 0;

#if 0
	if (seg == twelve_start || seg == six_start)
		bit_on(LED_BIT);
	if (seg == twelve_stop || seg == six_stop)
		bit_off(LED_BIT);
#endif
	if (seg > 128)
		goto ns_end;

	if (seg < 128)
		show_face();

	if (seg == 0x80) {
		shift_write(0x00);
	}
	if (seg == 0x00)
		shift_write(0x10);
	if (seg == 0x20)
		shift_write(0x20);
	if (seg == 0x40)
		shift_write(0x40);
	if (seg == 0x60)
		shift_write(0x80);

	seg++;

ns_end:
	GIE = 1; // enable interrupts
	return;
}



void sync(void)
{
	bit_off(LED_BIT);
	/* <interrupts disabled> */
//	GIE = 0;	// volam to z interruptu
	TMR1ON = 0;

	switch (running) {
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
			seg = 0;
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
			TMR0IF = 0;


			break;
		default:
			;	/* ... */
	}

	/* next state */
	if (running != STATE_RUNNING)
		running++;
	/* enable interrupts */
//	GIE = 1;
	bit_on(LED_BIT);
	return;
}






void application_reset(void)
{
	running = STATE_STOPPED;
	shift_write(0x5A);
	seg = 0;
	tmr1_counter = 0;
	tmr0_counter = 0;
	
	/* clear timers */
	TMR0 = 0;
	TMR1L = 0;
	TMR1H = 0;

	/* clear interrupt requests */
	TMR0IF = 0;
	TMR1IF = 0;
}

void interrupt interrupt_handler(void)
{
	/* port B interrupt */
	static char c=0;
	if (RBIF) {
		if (INPUT_BIT == 1) {
			//sync();
			;
		}
		/* button */
		if (BUTTON_LEFT == 0 || INPUT_BIT == 1) {
			/*next*/
//			c++;
//			shift_write(c);
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
}



void init(void)
{
//    ANSEL  = 0;
	/* set portA as output */
	TRISA = 0;
	ADCON1 = 0;
	asm("nop");
	PORTA = 0x00;
	a_latch = 0x00;
	asm("nop");

	/* set portB as input */
	TRISB = 0xFF;
	//TRISB = 0x80;	// bit 7 as an input


	shift_write(0x05);
	/* set higher frequency */
	OSCCONbits.IRCF2 = 1;
	OSCCONbits.IRCF1 = 1;
	OSCCONbits.IRCF0 = 1;
	/* wait until frequency is stable */
	while(!IOFS) {
		;
	}
	shift_write(0x02);

	/* calculate something.. */
	hour_start_end_init();
#if 1
	twelve_start = sixty_start(0);
	twelve_stop = sixty_stop(0);
	three_start = sixty_start(15);
	three_stop = sixty_stop(15);
	six_start = sixty_start(30);
	six_stop = sixty_stop(30);
	nine_start = sixty_start(45);
	nine_stop = sixty_stop(45);
#endif
	/* enable timer 1 */
	TMR1IF = 0;
	TMR1IE = 0;	// TEST
	T1CON = 0;
	TMR1ON = 1;

	TMR0IF = 0;
	TMR0IE = 0;	// TEST
	T0CS = 0;	// timer mode
	running = STATE_STOPPED;

	/* enable portB interrupt */
	RBIE = 1;	// TEST
	/* enable interrupts */
	PEIE = 1;
	GIE = 1;	// test
}


void main(void) {

	init();
	application_reset();

	uns8 i = 0;


	bit_on(LED_BIT);	// test

	while(1) {
		if (TMR0IF) {
			if (running == STATE_RUNNING) {
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
		if (TMR1IF) {
			tmr1_counter++;

			/* it is a long time ... */
			if (tmr1_counter >= 40) {
				application_reset();
			}
			TMR1IF = 0;
		}


	}
}

