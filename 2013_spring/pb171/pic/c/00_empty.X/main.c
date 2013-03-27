/*
 * 01.01 -- basic demo
 * for miniPICkit by z0r0 & res
 *
 * kit connection:
 *  GP0: LED0 (red)
 *  GP1: LED1 (green)
 *  GP2: BTN0 (via jumper J1) + extension header
 *  GP4,5: extension header
 */


//#include <htc.h>
#include <pic.h>
// fuses
/*__CONFIG(MCLRDIS & WDTDIS & INTIO);*/
__CONFIG(MCLRE_OFF & WDTE_OFF & FOSC_INTRCIO);

void init(void)
{


}

int wait_bounce()
{
    int i = 0x20;
    while(--i)
        ;
    return 1;
}

void old(void)
{
    /* main loop */

    int state = 0;
    int last = 1;
    int speed = 0;
	int cntr;
    do {
        cntr = 0x7000/(1+speed);
        while (--cntr) {};


        if (GPIO2 == 0 && last == 1){
	    wait_bounce();
	    if (GPIO2 == 0){
                state = !state;
		if (state == 0)
		    speed = (speed + 1) % 8;
	    }
	}
	last = GPIO2;

        if (state) {
            GPIO0 = !GPIO0;
            GPIO1 = !GPIO0;
        } else {
            GPIO0 = !GPIO0;
            GPIO1 = GPIO0;
        }

    } while (1);
}

#define MAGIC ((char)0xC4)
void save_to_eeprom(char low, char high)
{
	eeprom_write(0x1F, MAGIC);
	eeprom_write(0x20, low);
	eeprom_write(0x21, high);
}

int load_from_eeprom(char *low, char *high)
{
	char tmp;

	if (MAGIC != eeprom_read(0x1F))
		return 1;

	*low = eeprom_read(0x20);
	*high = eeprom_read(0x21);
}

void main(void) {
    int cntr = 0;

    /* init */
    CMCON = 7;      // disable comparator
    ANSEL = 0;      // disble analog function
    TRISIO = 0;     // set all pins as output
    TRISIO2 = 1;      // set GP2 as input


    // timer 1
    TMR1CS = 0;     // set internal clocks as clock source
    T1CKPS0 = 1;    // set prescaler to 1:8
    T1CKPS1 = 1;
    TMR1ON = 1;



    /* main program */
    GPIO0 = 1;
    asm("nop");     // prevent read-modify-write when assigning constant
    GPIO1 = 0;
    asm("nop");


    int cntr = 0x7000;
    while(cntr--)
		;

	char high, low;
	int state = 0;
	int saved;
	int old;

	high = low = 0;
	load_from_eeprom(&low, &high);
	TMR1L = low;
	TMR1H = high;

	while(1){
		if (GPIO2 == 0 && old == 1) {
			TMR1L = 0;
			TMR1H = 0;
			state = 1;
			GPIO1 = 0;
			wait_bounce();
		}
		if (GPIO2 == 1 && old == 0) {
			saved = TMR1H << 8 | TMR1L;

			saved = 65536 - saved;
			high = saved >> 8;
			low = saved & 0x00FF;

			TMR1L = low;
			TMR1H = high;
			state = 0;
			GPIO1 = 0;
			save_to_eeprom(low, high);
			wait_bounce();
		} else if (PIR1bits.T1IF == 1) {
			TMR1L = low;
			TMR1H = high;
			PIR1bits.T1IF = 0;
			GPIO1 = !GPIO1;

		}
		old = GPIO2;
	}


}


