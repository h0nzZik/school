/*
 *                  LCD - e x a m p l e
 *
 * Description : Simple example with CG-RAM programinng used for 4-bit communication with LCD
 *               between ATmega128 and 4 lines LCD MC4004
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.1 2006/12/04 09:05:11 bubulak Exp $
 */
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "shift.h"


/* Function prototypes */
void Hw_init(void);
uint16_t lcd_putchar(uint8_t , FILE *);

/* Data of char figure */
const uint8_t cgrom_p[] = {0xe, 0xe, 0xe, 0x4, 0x1f, 0x4, 0xa, 0x11};

/*!
 *      M       A       I       N
 *
 */
int main(void) {
    uint16_t poc = 0;
    uint8_t i,j;
    uint8_t dir;
    uint8_t tmp;

        LCD_init();                                     /* Initialize display */

        LCD_wr_cgram(0,&cgrom_p[0]);                    /* Program first char in CG-RAM */
        LCD_sendcmd(LCD_EN1 | LCD_EN2 | 0xc);           /* Disable cursors */

        for (i = 1; i < 5; i++)                         /* Fill display with pattern */
                for (j = 1; j < 41; j++) {
                        LCD_cursor_yx(i,j);
                        LCD_senddata(((i <3) ? LCD_EN1 : LCD_EN2)| 0);
                        _delay_ms(100);                 
                }
        LCD_sendcmd(LCD_EN1 | LCD_EN2 | 0x1);           /* Clear display */
                 
        i = 1; j = 1;
        dir = 0;
        do{
                LCD_cursor_yx(i,j);                                     /* Display figure */
                LCD_senddata(((i <3) ? LCD_EN1 : LCD_EN2)| 0);
                for (tmp = 0; tmp < 20; tmp++)
                        _delay_ms(10);                                  /* Wait for while */

                LCD_cursor_yx(i,j);
                LCD_senddata(((i <3) ? LCD_EN1 : LCD_EN2)| ' ');        /* Clear figure */
                switch(dir) {                                           /* State machine - direction control */
                        case 0: j++;                                    
                                if (j == 40) dir = 1;                   /* Right */
                                break;

                        case 1: i++;                                            
                                if (i == 4) dir = 2;                    /* Down */
                                break;
                        case 2: j--;
                                if (j == 1) dir = 3;                    /* Left */
                                break;
                        case 3: i--;
                                if (i == 1) dir =0;                     /* Up */
                                break;  
                }
        } while(1);
}
/*!
 * \brief Function Hw_init sets basic HW configuration
 */
void Hw_init(void)
{      
	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR);	/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);	/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);	/* serial data stream -> output pin */ 
	SHIFT_DDR |= _BV(SHIFT_LCD_SET_DDR);	/* parallel write -> output pin */

        LCD_init();                             /* Init LCD display */
}


