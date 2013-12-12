#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#include "czech.h"
#include "lcd.h"
#include "shift.h"

display_t coord;


inline void coord_newline(display_t *display)
{
	display->y++;
	display->y %= 4;

	switch (display->y)
	{
		case 0:
			LCD_sendcmd(LCD_EN1| 0x01); // clear display 1
			break;
		case 2:
			LCD_sendcmd(LCD_EN2| 0x01); // clear display 2
			break;
		default:
			break;			
	}
}

inline void coord_next(display_t *display)
{
	display->x++;
	if (display->x >= 40)
	{
		display->x = 0;
		coord_newline(display);
	}
}


								
							
/*!
 * \brief Send data to 2x16 LCD shift interface.
 *
 * \param val	Character to be send to LCD
 */
void DevBoardShiftLcdOut( uint8_t val )
{ uint8_t i=8;
	SHIFT_DDR |= _BV(SHIFT_LCD_SET_DDR);		/* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);      		/* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);      		/* serial data stream -> output pin */
	SHIFT_PORT &= ~(_BV(SHIFT_LCD_SET));
	SHIFT_PORT &= ~(_BV(SHIFT_CLK));	
	while( i-- )
	{ 						/* send data to 74595, MSB (most significant bit) first */
		if( val & (1 << i) ) SHIFT_PORT |= _BV(SHIFT_OUT);
		else SHIFT_PORT &= ~(_BV(SHIFT_OUT));
		SHIFT_PORT |= _BV(SHIFT_CLK);       	/* 0 -> 1 data valid */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));     	/* 1 -> 0 data valid */
	}
	SHIFT_PORT |= _BV(SHIFT_LCD_SET);       	/* 0 -> 1 data valid */
}

/**
 * @brief Puts a character into LCD display.
 * Character is put to current cursor and cursor is incremented.
 * @param char character to put
 */
void LCD_putc(char c)
{
	printf("LCD_putc(0x%x): cursor value [%u, %u]\n", 0xFF & c, 0xFF & (unsigned)coord.x, 0xFF & (unsigned)coord.y);
	switch(c)
	{
		case '\r':
			// fall through
		case '\n':
			coord_newline(&coord);
			return;
	}

	coord_next(&coord);
	lcd_cz_putc(c, &coord);
}

/*!
 * \brief Function display string on LCD 
 *
 * \param *str  Pointer to string (0x00 is expected on the end of the text)
 */
void LCD_puts( const char *str, uint8_t row)
{
        while(*str)
		{
			LCD_putc(*str++); /* Send one char to LCD */
		}
}

/*!
 * \brief Set cursor on LCD
 *
 * \param row           Row (1-4)
 * \param column        Column (1-40)
 *
 */
void LCD_cursor_yx(uint8_t row, uint8_t column )
{
	//printf("Setting cursor: [%u, %u]\n", 0xFF & (unsigned)row, 0xFF & (unsigned)column);

	uint16_t i;
    column--;
	switch( row ) {
		case 1 : i = (0x80 + column) | LCD_EN1; break;
		case 2 : i = (0xc0 + column) | LCD_EN1; break;
		case 3 : i = (0x80 + column) | LCD_EN2; break;
		case 4 : i = (0xc0 + column) | LCD_EN2; break;
		default: i = (0x80 + column) | LCD_EN1 | LCD_EN2; break;
	}
	LCD_sendcmd( i );
}

/*!
 * \brief Rewrite all LCD content
 *
 * \param *dta  Pointer to the memory
 *
 */
void LCD_write_all(uint8_t *dta)
{
	uint8_t i,j;
	for (i = 0; i < 4; i++) {
		/* Set line */
		switch (i) {
			case 0:	LCD_sendcmd(LCD_EN1 | 0x80); break;
			case 1: LCD_sendcmd(LCD_EN1 | 0xc0); break;
			case 2: LCD_sendcmd(LCD_EN2 | 0x80); break;
			case 3: LCD_sendcmd(LCD_EN2 | 0xc0); break;
				
		}
		for (j = 0; j < 40; j++) {
			/* ** Do you have some other data ? */
			if (!*dta) {
				j = 40;
				i = 5;
			}
			else {
				/* <N> ** Is it <ENTER> ? */
				if (*dta == '\n') {
					j = 40;
				}
				else {
					/* ** Is it first or second line ? */
					if (i <3)
						LCD_senddata(LCD_EN1 | *dta);           /* Send data to first or second line */
					else
						LCD_senddata(LCD_EN2 | *dta);           /* Send data to fird or fourht line */
				}
			}
			dta++;			
		}
	}
}		


/*!
 * \brief Send data to LCD 
 *
 * \param val   data for display                  
 *              low byte => character, high byte => control (EN1, EN2, RS)
 *
 */
void LCD_sendval( uint16_t val)
{
        uint8_t ctrl = 0;
        uint8_t tmp;
        if (LCD_RS & val)       ctrl |= P_LCD_RS;                       /* Do you expect a command ? */
        if (LCD_EN1 & val)      ctrl |= P_LCD_EN1;                      /* Do you expect work with first part of LCD ? */
        if (LCD_EN2 & val)      ctrl |= P_LCD_EN2;                      /* Do you expect work with second part of LCD ? */
        tmp = ctrl | (val & 0xF0);
        LCD_write( tmp );                                               /* high nibble ENABLE active */
        tmp = (ctrl & ~(P_LCD_EN1 | P_LCD_EN2)) | (val & 0xF0);
        LCD_write(tmp);                                                 /* high nibble */

        tmp = ctrl | ((val & 0xf) << 4) ;
        LCD_write( tmp ); /* low nibble ENABLE active */
        tmp = (ctrl & ~(P_LCD_EN1 | P_LCD_EN2)) | ((val & 0xf) << 4);
        LCD_write(  tmp );                       /* low nibble */

        _delay_ms(2);                                   /* The busy flag must be checked after the 4-bit data */
                                                        /* has been transferred twice. */
}

/*!
 * \brief Initializing LCD by Instruction - 4-bit initialization
 */
void LCD_init(void)
{
	_delay_ms(30);        	                /* power on delay - wait more than 30 ms */
	LCD_write(0);      		        /* set RS, RW and EN low */

	LCD_write( P_LCD_EN1 | P_LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */
	_delay_ms(5);			        /* wait 5 ms */

	LCD_write( P_LCD_EN1 | P_LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */	
	_delay_us(100);                         /* wait more than 100us */

	LCD_write( P_LCD_EN1 | P_LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */
	_delay_us(100);                         /* wait more than 100us */

	LCD_write( P_LCD_EN1 | P_LCD_EN2 | 0x20 );  /* lcd enable high */
	LCD_write( 0x20 );                      /* lcd enable low */
	_delay_us(100);                         /* wait more than 100us */

	LCD_sendcmd(LCD_EN1| LCD_EN2 | 0x28);                     /* 4 bit mode, 1/16 duty, 5x8 font */
	LCD_sendcmd(LCD_EN1| LCD_EN2 | 0x08);                     /* display off */
	LCD_sendcmd(LCD_EN1| LCD_EN2 | 0x01);                     /* display clear */
	LCD_sendcmd(LCD_EN1| LCD_EN2 | 0x06);                     /* entry mode */
	LCD_sendcmd(LCD_EN1| LCD_EN2 | 0x0f);                     /* display on, cursor off, blinking cursor off */


	coord.x = 0;
	coord.y = 0;
}
