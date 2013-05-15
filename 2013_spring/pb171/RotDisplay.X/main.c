/* 
 * File:   main.c
 * Author: vyvoj
 *
 * Created on 24. duben 2013, 14:44
 */

#include <pic.h>
__CONFIG(WDTE_OFF & CP_OFF & MCLRE_OFF & FOSC_INTOSCIO & BOREN_OFF );

//#define SHIFT_DATA	PORTAbits.RA2
//#define SHIFT_CLK	PORTAbits.RA3
//#define STOR_CLK	PORTAbits.RA4


#define LED_BIT		0x02
#define DATA_BIT	0x04

/* FIXME: Chyba v dokumentaci */
/* test */
#define SHCLK_BIT	0x10
#define STCLK_BIT	0x08


unsigned char a_latch;
void bit_on(unsigned char bt)
{
	a_latch |= bt;
	PORTA = a_latch;
}

void bit_off(unsigned char bt)
{
	a_latch &= ~bt;
	PORTA = a_latch;
}




/*
 * 
 */

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


}


void shift_write(char data)
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

void main(void) {

	init();

	unsigned char i = 0;

	shift_write(0x5A);


	bit_on(LED_BIT);

	unsigned char x = 0;
    while(1) {
		if (i == 0) {
			bit_off(LED_BIT);
		}
		if (i == 64){
			bit_on(LED_BIT);
			shift_write(++x);
		}

		i++;
    }


	do {
		while(++i)
			;
		asm("nop");
		RA1 = 1;
		while(++i)
			;
		asm("nop");
		RA1 = 0;
	} while(1);



}

