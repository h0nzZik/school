/*
 *                  LCD - e x a m p l e
 *
 * Description : Basic functions used for 4-bit communication with LCD
 *               between ATmega128 and 4 lines LCD MC4004
 *
 * Author: Zbynek Bures
 *
 * Target system : Charon 2 Development Kit - www.hwg.cz
 *                 ATmega128 14,7456 MHz, UART: 9600,N,8,1
 *
 * Compiler : avr-gcc 4.1.0
 *
 * $Id: main.c,v 1.2 2006/12/02 22:20:18 cvsd Exp $
 */
static char text[]="Hello world!!";


#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "shift.h"


/* Function prototypes */
void Hw_init(void);
void DevBoardShiftLcdOut( uint8_t );
void LCD_init(void);
void LCD_sendval( uint8_t, uint8_t);
void LCD_putch( uint8_t);
void LCD_puts( uint8_t *);


/*!
 *      M       A       I       N
 *
 */
int main(void) {
        Hw_init();                              /* Initialize LCD */
        LCD_puts(&text[0]);                     /* Display string on both part of display */
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


/*!
 * \brief Function send one character to LCD 
 *
 * \param val	data for LCD
 */
void LCD_putch( uint8_t val )
{
	LCD_send_data( val );
}

/*!
 * \brief Function display string on LCD 
 *
 * \param *str  Pointer to string (0x00 is expected on the end of the text)
 */
void LCD_puts( uint8_t *str )
{
  while(*str) LCD_putch( *str++);                       /* Send one char to LCD */
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


/*!
 * \brief Send data to LCD 
 *
 * \param val	 data for display
 * \param rs	 type of data 	0 .. command, 1 .. data
 *
 */
void LCD_sendval( uint8_t val, uint8_t RS )
{
	if( RS )                                        /* RS ... 0 (command), RS ... 1 (data) */
	{       /* data */
		LCD_write( LCD_RS | LCD_EN1 | LCD_EN2 | (val & 0xF0) ); /* high nibble ENABLE active */
		LCD_write( LCD_RS | (val & 0xF0) );                     /* high nibble */

		LCD_write( LCD_RS | LCD_EN1 | LCD_EN2 | (val << 4) );   /* low nibble ENABLE active */
		LCD_write( LCD_RS | (val << 4) );                       /* low nibble */
	}
	else { 
                /* command */
		LCD_write( LCD_EN1 | LCD_EN2 | (val & 0xF0) );          /* high nibble ENABLE active */
		LCD_write( (val & 0xF0) );                              /* high nibble */

		LCD_write( LCD_EN1 | LCD_EN2 | (val << 4) );            /* low nibble ENABLE active */
		LCD_write( (val << 4) );                                /* low nibble */
	}

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

	LCD_write( LCD_EN1 | LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */
	_delay_ms(5);			        /* wait 5 ms */

	LCD_write( LCD_EN1 | LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */	
	_delay_us(100);                         /* wait more than 100us */

	LCD_write( LCD_EN1 | LCD_EN2 | 0x30 );  /* lcd enable high */
	LCD_write( 0x30 );                      /* lcd enable low */
	_delay_us(100);                         /* wait more than 100us */

	LCD_write( LCD_EN1 | LCD_EN2 | 0x20 );  /* lcd enable high */
	LCD_write( 0x20 );                      /* lcd enable low */
	_delay_us(100);                         /* wait more than 100us */

	LCD_send_cmd(0x28);                     /* 4 bit mode, 1/16 duty, 5x8 font */
	LCD_send_cmd(0x08);                     /* display off */
	LCD_send_cmd(0x01);                     /* display clear */
	LCD_send_cmd(0x06);                     /* entry mode */
	LCD_send_cmd(0x0C);                     /* display on, cursor off, blinking cursor off */
}
