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
 * $Id: main.c,v 1.2 2006/12/02 22:55:59 cvsd Exp $
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


FILE lcd_str = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);

/*!
 *      M       A       I       N
 *
 */
int main(void) {
    uint16_t poc = 0;
    uint8_t i;
        LCD_init();                                     /* Initialize display */
        LCD_sendcmd(LCD_EN1 | LCD_EN2 | 0xc);           /* Switch off cursor */

        stderr = &lcd_str;                              /* Set FILE stderr */
        while (1) {
                fprintf(stderr, "\1 First line -> Cnt %d .\n",poc++);
                fprintf(stderr, "\2 Second line\n");
                fprintf(stderr, " Next line");
                fprintf(stderr, "\4 Fourth line");
                for (i = 0; i < 10; i++)
                    _delay_ms(50);
                if (!(poc % 10))
                        fprintf(stderr, "\5 Clear disp.");
        }
        return(0);
}

/*!
 * \brief Putchar for fprintf
 * Send character c to the LCD display.  After a '\1' set cursor to first line, '\2' set cursor to secound line,
 * '\3' set cursor to third line, '\4' set cursor to fourth line, '\5' clear display.
 * the next character will first clear the display.
 * 
 * \param c input char
 * \param FILE * output FILE
 *
 * \return status output status
 *
 */
uint16_t lcd_putchar(uint8_t c, FILE *unused)
{
        static uint8_t line = 1;
        if (c == 1) {
                LCD_cursor_yx(1,1);                             /* Set cursor to first line */
                line = 1;
        }
        else if (c == 2) {
                LCD_cursor_yx(2,1);                             /* Set cursor to second line */
                line = 2;
        }
        else if (c == 3) {                                      
                LCD_cursor_yx(3,1);                             /* Set cursor to third line */
                line = 3;
        }
        else if (c == 4) {
                LCD_cursor_yx(4,1);                             /* Set cursor to fourth line */
                line = 4;
        }
        else if (c == 5) {
                LCD_sendcmd(LCD_EN1 | LCD_EN2 | 0x1);           /* Clear both displays */
                line = 1;
        }
        else if (c == '\n') {                                   /* Set cursor to next line */ 
                LCD_cursor_yx(((++line < 5) ? line : (line = 1)),1);   
        }
        else    LCD_senddata(((line <3) ? LCD_EN1 : LCD_EN2) | c); /* Send data to LCD */
        return 0;
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

