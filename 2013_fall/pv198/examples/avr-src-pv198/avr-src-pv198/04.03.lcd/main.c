/*
 *                  LCD - e x a m p l e
 *
 * Description : Extended functions used for 4-bit communication with LCD
 *               between ATmega128 and 4 lines LCD MC4004
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.2 2006/12/03 11:03:26 cvsd Exp $
 */
static char text[]="0123456";


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "shift.h"


/* Function prototypes */
void Hw_init(void);


/*!
 *      M       A       I       N
 *
 */
int main(void) {
uint8_t i;
        Hw_init();                              /* Initialize LCD */
        LCD_wr_cgram_all(2);
        LCD_cursor_yx(1,1);                     /* Display first line */
        LCD_puts(&text[0],1);
        LCD_cursor_yx(4,1);
        LCD_puts(&text[0],4);                   /* Display fourth line */
        LCD_cursor_yx(2,1);
        LCD_puts(&text[0],2);                   /* Display second line */
        LCD_cursor_yx(3,1);
        LCD_puts(&text[0],3);                   /* Display third line */
        for (i = 0; i < 100; i++)
                _delay_ms(10);                  /* Wait 1 sec */
        LCD_sendcmd(LCD_EN1 | LCD_EN2 | 0x1);   /* clear whole display */
        LCD_puts_big(&text[0]);                 /* display number with big font */

        while (1);                              /* wait forever */
        return(0);
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

