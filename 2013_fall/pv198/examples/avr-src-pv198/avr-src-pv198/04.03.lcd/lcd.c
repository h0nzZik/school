#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "shift.h"

/* First CG-ROM */
const uint8_t cgrom[] = {  0x0e, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0e, 0x00, /* O full */
                        0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00, /* \ */
                        0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, /* | */
                        0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, /* || */
						0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, /* ||| */
						0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e }; /* |||| */
/* Second CG-ROM */
const uint8_t cgrom2[] = { 0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x00, /* ""   0 */
						0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, /* ,,   1 */
						0x01, 0x07, 0x0f, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, /* /|   2 */
						0x10, 0x1c, 0x1e, 0x1e, 0x1f, 0x1f, 0x1f, 0x1f, /* |\   3 */
						0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x0f, 0x07, 0x01, /* \|   4 */
						0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x1e, 0x1c, 0x10, /* |/   5 */
						0x00, 0x00, 0x00, 0x00, 0x10, 0x1c, 0x1e, 0x1e, /* |\maly 6 */
						0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x00};/* . */
/* Third CG-ROM */
const uint8_t cgrom3[] = { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x00, 0x00, /* ""   0 */
						0x00, 0x00, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, /* ,,   1 */
						0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x1f, 0x00, /* /|   2 */
						0x00, 0x10, 0x18, 0x1c, 0x1e, 0x1f, 0x1f, 0x00, /* |\   3 */
						0x00, 0x1f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, /* \|   4 */
						0x00, 0x1f, 0x1f, 0x1e, 0x1c, 0x18, 0x10, 0x00}; /* |/   5 */


/* Table data for big number */
const uint8_t big_number[][16] = {{ 2,0,0,3,255,32,32,255,255,32,32,255,4,1,1,5},		// Char 0
				{32,1,255,32,32,32,255,32,32,32,255,32,32,1,255,1},             // Char 1
				{2,0,0,3,32,32,1,5,2,0,32,32,255,1,1,1},			// Char 2
				{2,0,0,3,32,32,1,5,32,32,32,3,4,1,1,5},				// Char 3
				{255,32,32,32,255,32,32,32,255,1,255,1,32,32,255,32},	        // Char 4
				{255,0,0,0,255,1,1,6,32,32,32,255,4,1,1,5},			// Char 5
				{2,0,0,3,255,1,1,6,255,32,32,255,4,1,1,5}, 			// Char 6
				{0,0,0,255,32,32,32,255,32,32,255,32,32,255,32,32}, 	        // Char 7
				{2,0,0,3,4,1,1,5,2,32,32,3,4,1,1,5}, 				// Char 8
				{2,0,0,3,4,1,1,255,32,32,32,255,4,1,1,5}};			// Char 9
				

									
							
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
 * \brief Function display string on LCD 
 *
 * \param *str  Pointer to string (0x00 is expected on the end of the text)
 */
void LCD_puts( uint8_t *str, uint8_t row)
{
        while(*str) LCD_senddata( ((row < 3) ? LCD_EN1 : LCD_EN2) | *str++);                       /* Send one char to LCD */
}

/*!
 * \brief Function display string of big numbers on LCD 
 *
 * \param *str  Pointer to string (0x00 is expected on the end of the text)
 */
void LCD_puts_big( uint8_t *str )
{
        uint8_t col = 0;
        while(*str) LCD_write_big_num( (col += 5), (*str++ & 0xf));                       /* Send one char to LCD */
}



/*!
 * \brief Send one digit to LCD
 *
 * \param pos   start position
 * \param val   value (only number)
 * 
 */
void LCD_write_big_num(uint8_t pos, uint8_t val)
{
	uint8_t i,j;
	for (i = 0; i <4; i++) {
		switch (i) {
			case 0:	LCD_sendcmd(LCD_EN1 | (0x80 + pos)); break;
			case 1: LCD_sendcmd(LCD_EN1 | (0xc0 + pos)); break;
			case 2: LCD_sendcmd(LCD_EN2 | (0x80 + pos)); break;
			case 3: LCD_sendcmd(LCD_EN2 | (0xc0 + pos)); break;
		}
		for (j = 0; j < 4; j++) {
			if (i <2)
				LCD_senddata(LCD_EN1 | big_number[val][4*i+j]);
			else
				LCD_senddata(LCD_EN2 | big_number[val][4*i+j]);
		}		
	}
}

/*!
 * \brief Pre-set all CGRAM characters
 *
 * \param set   Type of CGRAM stored in rom
 *
 */

void LCD_wr_cgram_all(uint8_t set) {
	if (!set) {
		LCD_wr_cgram(0, &cgrom[0]);
		LCD_wr_cgram(1, &cgrom[8]);
		LCD_wr_cgram(2, &cgrom[16]);
		LCD_wr_cgram(3, &cgrom[24]);
		LCD_wr_cgram(4, &cgrom[32]);
		LCD_wr_cgram(5, &cgrom[40]);
	}
	else {
		LCD_wr_cgram(0, &cgrom2[0]);
		LCD_wr_cgram(1, &cgrom2[8]);
		LCD_wr_cgram(2, &cgrom2[16]);
		LCD_wr_cgram(3, &cgrom2[24]);
		LCD_wr_cgram(4, &cgrom2[32]);
		LCD_wr_cgram(5, &cgrom2[40]);
		LCD_wr_cgram(6, &cgrom2[48]);
		LCD_wr_cgram(7, &cgrom2[56]);
	}
	
}

/*!
 * \brief Pre-set one CGRAM character
 *
 * \param char_val      Number of char. Only 0 - 7 are permitted.
 * \param *char_buf     Pointer to the memory on the first microline
 *
 */
void LCD_wr_cgram(uint8_t char_val, const uint8_t *char_buf )
{ uint8_t i = 8;

  char_val <<= 3;
  LCD_sendcmd(LCD_EN1 | LCD_EN2 | cmd_set_cgram_addr | char_val );

  while(i--) LCD_senddata( LCD_EN1 | LCD_EN2 | *char_buf++ );

  LCD_sendcmd(LCD_EN1 | LCD_EN2 | cmd_set_ddram_addr);
}

/*!
 * \brief Set cursor on LCD
 *
 * \param row           Row (1-4)
 * \param column        Column (1-40)
 *
 */
void LCD_cursor_yx(uint8_t row, uint8_t column ) {
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

}
